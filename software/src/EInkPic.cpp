#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <GxEPD2_7C.h>
#include <DisplayConfig.hpp>
#include <ArduinoJson.h>
#include "config.hpp"
#include "nosignalimage.hpp"


enum state_t{
  WAIT_FOR_IMAGE = 0,
  PART_1_REVCIEVED = 1,
  PART_2_REVCIEVED = 2,
  PART_3_REVCIEVED = 3,
  PART_4_REVCIEVED = 4,
  CONFIG_RECIEVED = 5,
  NO_CONNECTION = 6
};
state_t state = WAIT_FOR_IMAGE;
uint64_t sleep_time = DEFAULT_SLEEP_TIME;
unsigned long last_message = 0;
WiFiClient espClient;
StaticJsonDocument<200> doc;
void callback(char* topic, byte* message, unsigned int length) ;

PubSubClient client(mqtt_server, port,callback,espClient);

void handle_states(byte number){
  last_message = millis();
  Serial.print("State handling:  ");
  Serial.print(number);
  if (state == ((int)number-48-1)){
      state = (state_t)(number-48);
      }
  else{
    Serial.print("UPS");
    state = CONFIG_RECIEVED; // wil try again in 30 minutes
  }

}

void set_new_sleep_time(byte* message){
  DeserializationError error = deserializeJson(doc, message);
  if (error) { //Check for errors in parsing
    Serial.println("Parsing failed");
    sleep_time = 30;
  }
  const char * image = doc["image_name"];
  Serial.println("Sucessfully updated image to:");
  Serial.println(image);
  sleep_time = doc["refresh"];
  Serial.println("Updated refresh Time to :");
  Serial.println(sleep_time);
}

void callback(char* topic, byte* message, unsigned int length) {
  Serial.print("Message arrived on topic: ");
  Serial.print(topic);
  handle_states(message[9]);
  if(state < 5){
    display.copyToRawPixelBuffer(&(message[21]));
    if(state != PART_4_REVCIEVED) display.nextPage(); //4. slides will cause the break of the callback
    client.publish(("Eink_frame/"+String(name)+"/ack").c_str(),String(state).c_str());
  }
  else
    {
      set_new_sleep_time(message);
      client.publish(("Eink_frame/"+String(name)+"/ack").c_str(),String(state).c_str());
    }
}





void setup() {
  Serial.begin(115200);
  while(!Serial) {}
  initialiseDisplay();
  display.firstPage();
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_AP_NAME, WIFI_PWD);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    delay(1000);
  } 
  client.setBufferSize(50000);
  if( client.connect(name,user,pswd)){
    client.subscribe(("Eink_frame/"+String(name)+"/send_image").c_str());
  }
  client.publish(("Eink_frame/"+String(name)+"/get_image").c_str(),"hi_guys"); 
  last_message = millis();
  while (state < CONFIG_RECIEVED)
  {
    client.loop();
    if(millis()-last_message>TIMOUT_MQTT){
      display.writeNative(testPic, nullptr, 0, 0, 800, 480, false, false, true);
      state = NO_CONNECTION;
      break;} 
  }
  if(state != NO_CONNECTION) display.nextPage(); //Needed to display 4th slice
  display.refresh();
  Serial.println("Entering deep sleep ");
  Serial.flush();  
  display.powerOff();
  esp_deep_sleep(sleep_time*60000000);
}


void loop() {} // this will never run!