#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <GxEPD2_7C.h>
#include <DisplayConfig.hpp>
#include <ArduinoJson.h>
#include "config.hpp"
#include "nosignalimage.hpp"
#include "mbedtls/base64.h"

enum state_t{
  CONFIG_RECIEVED = 0,
  WAIT_FOR_IMAGE = 1,
  PART_1_REVCIEVED = 2,
  PART_2_REVCIEVED = 3,
  PART_3_REVCIEVED = 4,
  PART_4_REVCIEVED = 5
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
  if (state == ((int)number-48-1)) {
    state = (state_t)(number-48);
  } else {
    Serial.print("UPS");
    state = CONFIG_RECIEVED; // will try again in 30 minutes
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

uint8_t pixelBuffer[MAX_DISPLAY_BUFFER_SIZE];

void convertToPanelFormat(const byte *b64, const uint8_t pageNr) {
  uint8_t decBuf[3];
  size_t olen;
  for(uint16_t i = 0; i < MAX_DISPLAY_BUFFER_SIZE / 4; i++) {
    mbedtls_base64_decode(&(decBuf[0]), 3, &olen, &(b64[i*4]), 4);
    pixelBuffer[i*4+0] = ((decBuf[0] & 0b00000111)     ) | ((decBuf[0] & 0b00111000) << 1);
    pixelBuffer[i*4+1] = ((decBuf[0] & 0b11000000) >> 6) | ((decBuf[1] & 0b00000001) << 2) | ((decBuf[1] & 0b00001110) << 3);
    pixelBuffer[i*4+2] = ((decBuf[1] & 0b01110000) >> 4) | ((decBuf[1] & 0b10000000) >> 3) | ((decBuf[2] & 0b00000011) << 5);
    pixelBuffer[i*4+3] = ((decBuf[2] & 0b00011100) >> 2) | ((decBuf[2] & 0b11100000) >> 1);
  }
  display.writeNative(pixelBuffer, 0, 0, MAX_DISPLAY_BUFFER_SIZE / (GxEPD2_DRIVER_CLASS::WIDTH / 2) * pageNr,
      GxEPD2_DRIVER_CLASS::WIDTH, MAX_DISPLAY_BUFFER_SIZE / (GxEPD2_DRIVER_CLASS::WIDTH / 2), false, false, false);
}

void callback(char* topic, byte* message, unsigned int length) {
  Serial.print("Message arrived on topic: ");
  Serial.print(topic);
  handle_states(message[9]);
  if (state < 5) {
    //display.copyToRawPixelBuffer(&(message[21]));
    if (state != PART_4_REVCIEVED) display.nextPage(); //4. slides will cause the break of the callback
    client.publish(("Eink_frame/" + String(name) + "/ack").c_str(), String(state).c_str());
  } else {
    set_new_sleep_time(message);
    client.publish(("Eink_frame/" + String(name) + "/ack").c_str(), String(state).c_str());
  }
}

void enterSleepMode(boolean displayDefaultPicture = false) {
  if (displayDefaultPicture) {
    for (uint8_t i = 0; i < 4; i++) {
      convertToPanelFormat((const byte*)&testPic[MAX_DISPLAY_BUFFER_SIZE*i], i);
    }
  }
  display.refresh();
  Serial.println("Entering deep sleep ");
  Serial.flush();  
  display.powerOff();
  esp_deep_sleep(sleep_time*60000000);
}

void setup() {
  Serial.begin(115200);
  while(!Serial) {}
  initialiseDisplay();
  display.firstPage();
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_AP_NAME, WIFI_PWD);
  last_message = millis();
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    delay(1000);
    if (millis() - last_message > TIMEOUT_WIFI_MS) {
      enterSleepMode(true);
    }
  }
  client.setBufferSize(50000);
  if (client.connect(name, user, pswd)) {
    client.subscribe(("Eink_frame/" + String(name) + "/send_image").c_str());
  }
  client.publish(("Eink_frame/" + String(name) + "/get_image").c_str(), "hi_guys"); 
  last_message = millis();
  while (state > CONFIG_RECIEVED) {
    client.loop();
    if (millis() - last_message > TIMEOUT_MQTT_MS) {
      enterSleepMode(true);
    }
  }
  display.nextPage(); // Needed to display the last slice after successfully receiving all parts
  enterSleepMode();
}

void loop() {} // this will never run!