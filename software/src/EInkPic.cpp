#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <GxEPD2_7C.h>
#include <DisplayConfig.hpp>
#include <ArduinoJson.h>
#include "config.hpp"
#include "nosignalimage.hpp"

enum state_t {
  WAIT_FOR_IMAGE  = 0,
  CONFIG_RECIEVED = 1
};

state_t state = WAIT_FOR_IMAGE;
uint8_t pagesReceived = 0;
uint64_t sleep_time = DEFAULT_SLEEP_TIME;
unsigned long last_message = 0;
WiFiClient espClient;
StaticJsonDocument<200> doc;
void callback(char* topic, byte* message, unsigned int length);

PubSubClient client(mqtt_server, port, callback, espClient);

void handle_states(byte number) {
  last_message = millis();
  Serial.print("State handling: ");
  Serial.print(number);
  if (pagesReceived == ((int)number-48-1)) {
    pagesReceived = (state_t)(number-48);
  } else {
    Serial.print("UPS");
    state = CONFIG_RECIEVED; // will try again in 30 minutes
  }
}

void set_new_sleep_time(byte* message) {
  DeserializationError error = deserializeJson(doc, message);
  if (error) { //Check for errors in parsing
    Serial.println("Parsing failed");
    sleep_time = 30;
  }
  const char * image = doc["image_name"];
  Serial.print("Sucessfully updated image to: ");
  Serial.println(image);
  sleep_time = doc["refresh"];
  Serial.print("Updated refresh time to: ");
  Serial.println(sleep_time);
}

uint8_t pixelBuffer[MAX_DISPLAY_BUFFER_SIZE];

void convertToPanelFormat(const byte *b64, const uint8_t pageNr) {
  for(uint16_t i = 0; i < MAX_DISPLAY_BUFFER_SIZE; i++) {
    pixelBuffer[i] = base64Map[(const uint8_t)b64[i]];
  }
  display.writeNative(pixelBuffer, 0, 0, PAGE_HEIGHT * pageNr,
      GxEPD2_DRIVER_CLASS::WIDTH, PAGE_HEIGHT, false, false, false);
}

void callback(char* topic, byte* message, unsigned int length) {
  Serial.print("Message arrived on topic: ");
  Serial.print(topic);
  handle_states(message[9]);
  convertToPanelFormat(&(message[21]), pagesReceived);
  if (pagesReceived >= NUMBER_OF_PAGES) {
    set_new_sleep_time(message);
  }
  client.publish(("Eink_frame/" + String(name) + "/ack").c_str(), String(state).c_str());
}

void enterSleepMode(boolean displayDefaultPicture = false) {
  if (displayDefaultPicture) {
    for (uint8_t i = 0; i < NUMBER_OF_PAGES; i++) {
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
  client.setBufferSize(20000);
  if (client.connect(name, user, pswd)) {
    client.subscribe(("Eink_frame/" + String(name) + "/send_image").c_str());
  }
  client.publish(("Eink_frame/" + String(name) + "/get_image").c_str(), "hi_guys"); 
  last_message = millis();
  while (state < CONFIG_RECIEVED) {
    client.loop();
    if (millis() - last_message > TIMEOUT_MQTT_MS) {
      enterSleepMode(true);
    }
  }
  //display.nextPage(); // Needed to display the last slice after successfully receiving all parts
  enterSleepMode();
}

void loop() {} // this will never run!