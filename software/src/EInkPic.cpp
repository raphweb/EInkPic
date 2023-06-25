#include <Arduino.h>

#include <GxEPD2_BW.h>
#include <Fonts/FreeSans12pt7b.h>

#include <DisplayConfig.hpp>

void setup() {
  Serial.begin(115200);
  while(!Serial) {}
  initialiseDisplay();
  display.firstPage();
  display.setFont(&FreeSans12pt7b);
  display.setTextColor(GxEPD_BLACK);
  do {
    display.fillScreen(GxEPD_WHITE);
    display.setCursor(25, 85);
    display.print("ESP and EPaper is easy!");
    Serial.println("Page!");
  } while (display.nextPage());
  //display.powerOff();
  Serial.println("Entering deep sleep forever.");
  delay(1000);
  Serial.flush();
  esp_deep_sleep_start();
}

void loop() {} // this will never run!