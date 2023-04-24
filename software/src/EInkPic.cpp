#include <Arduino.h>

#include <GxEPD2_BW.h>
#include <Fonts/FreeSans12pt7b.h>

#include "DisplayConfig.hpp"

void setup() {
  Serial.begin(115200);
  while(!Serial) {}
  display.init(115200, true, 20, false);
  display.setRotation(1);
  display.setFullWindow();
  display.firstPage();
  display.setFont(&FreeSans12pt7b);
  display.setTextColor(GxEPD_BLACK);
}

void loop() {
  do {
    display.fillScreen(GxEPD_WHITE);
    display.setCursor(25, 85);
    display.print("ESP and EPaper is easy!");
  } while (display.nextPage());
  display.powerOff();
  ESP.deepSleep(0);
}
