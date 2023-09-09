#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <GxEPD2_7C.h>
#include <DisplayConfig.hpp>



const char* mqtt_server = "homeassistant.local";
const char* user = "mqttuser";
const char* pswd = "pwd";
const char* name = "eink1";
const int port = 1883;
enum state_t{
  WAIT_FOR_IMAGE = 0,
  PART_1_REVCIEVED = 1,
  PART_2_REVCIEVED = 2,
  PART_3_REVCIEVED = 3,
  PART_4_REVCIEVED = 4,
  CONFIG_RECIEVED = 5,
};
state_t state = WAIT_FOR_IMAGE;
uint64_t sleep_time = 30;
WiFiClient espClient;

void callback(char* topic, byte* message, unsigned int length) ;

PubSubClient client(mqtt_server, port,callback,espClient);

void handle_states(byte number){
  Serial.print("State handling:  ");
  Serial.print(number);
  if (state == ((int)number-48-1)){
      state = (state_t)(number-48);
      }
  else{
    Serial.print("UPS");
  }

}

void callback(char* topic, byte* message, unsigned int length) {
  Serial.print("Message arrived on topic: ");
  Serial.print(topic);
  handle_states(message[9]);
  if(state < 5){
    display.copyToRawPixelBuffer(&(message[21]));
    display.nextPage();
    client.publish(("Eink_frame/"+String(name)+"/ack").c_str(),String(state).c_str());
  }
  else
    {
      sleep_time = 30; //TODO change later
      client.publish(("Eink_frame/"+String(name)+"/ack").c_str(),String(state).c_str());
      //TODO set time to sleep
    }
}





void setup() {
  Serial.begin(115200);
  while(!Serial) {}
  initialiseDisplay();
  display.firstPage();
  WiFi.mode(WIFI_STA);
  WiFi.begin("WIFI", "pwd");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    delay(1000);
  } 
  client.setBufferSize(64000);
  if( client.connect(name,user,pswd)){
    client.subscribe(("Eink_frame/"+String(name)+"/send_image").c_str());
  }
  client.publish(("Eink_frame/"+String(name)+"/get_image").c_str(),"hi_guys"); 
  while (state < CONFIG_RECIEVED)
  {
    client.loop();
  }
  display.refresh();
  Serial.println("Entering deep sleep ");
  Serial.flush();  
  display.powerOff();
  esp_deep_sleep(sleep_time*60000000);
}


void loop() {} // this will never run!