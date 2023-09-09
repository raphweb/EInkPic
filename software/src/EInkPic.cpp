#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <GxEPD2_7C.h>
#include <Fonts/FreeSans12pt7b.h>

#include <DisplayConfig.hpp>
#include <image.hpp>


const char* mqtt_server = "homeassistant.local";
const char* user = "mqttuser";
const char* pswd = "pwd";
const int port = 1883;
int state = 0;

WiFiClient espClient;

void handle_states(byte number){


}

void callback(char* topic, byte* message, unsigned int length) {
  Serial.print("Message arrived on topic: ");
  Serial.print(topic);
  handle_states(message[11]);
  Serial.print(". Message: ");
  String messageTemp;
  
  for (int i = 0; i < length; i++) {
    Serial.print((char)message[i]);
    messageTemp += (char)message[i];
  }
  Serial.println();
}

PubSubClient client(mqtt_server, port,callback,espClient);

void reconnect(){
  WiFi.mode(WIFI_STA);
  WiFi.begin("WIFI", "PWD");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    delay(1000);
  }
  if( client.connect("einkpanel1",user,pswd)){
    client.subscribe("Eink_frame/eink1/send_image");
  }
}

uint32_t currIndex = 0;

void drawPicture(const void*) {
  display.copyToRawPixelBuffer(&(testPic[currIndex]));
  currIndex += GxEPD2_DRIVER_CLASS::WIDTH/2 * display.pageHeight();
}

void setup() {
  Serial.begin(115200);
  while(!Serial) {}
  initialiseDisplay();
  display.drawPaged(drawPicture, nullptr);
  currIndex = 0;
  display.refresh();
  display.powerOff();
  Serial.println("Entering deep sleep forever.");
  delay(1000);
  //initialiseDisplay();
  WiFi.mode(WIFI_STA);
  WiFi.begin("nicht mit dem Internet", "HuFwz@R#221");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    delay(1000);
  }
  client.setBufferSize(64000);
  if( client.connect("einkpanel1",user,pswd)){
    client.subscribe("Eink_frame/eink1/send_image");
  }
  Serial.println(WiFi.localIP());
  //display.writeNative(gImage_7in3f, nullptr, 0, 0, 800, 480, false, false, true);
  //display.refresh();
  //clearAllColors();
  /*display.firstPage();

  
  display.setFont(&FreeSans12pt7b);
  display.setTextColor(GxEPD_BLACK);
  do {
    display.fillScreen(GxEPD_WHITE);
    display.setCursor(25, 85);
    display.print("ESP and EPaper is easy!");
  } while (display.nextPage());
  */
  //display.powerOff(); 
  while (true)
  {
    client.loop();
  }
  Serial.println("Entering deep sleep ");
 
  Serial.flush();
  esp_deep_sleep_start();
}


void loop() {} // this will never run!