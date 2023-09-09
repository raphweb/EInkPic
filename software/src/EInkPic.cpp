#include <Arduino.h>

#include <GxEPD2_7C.h>
#include <Fonts/FreeSans12pt7b.h>

#include <DisplayConfig.hpp>
#include <image.hpp>

void fillWithColor(uint16_t color) {
  display.firstPage();
  do {
    display.fillScreen(color);
  } while (display.nextPage());
}

void clearAllColors() {
  fillWithColor(GxEPD_BLACK);
  Serial.println("Display is now black!");
  delay(5000);
  fillWithColor(GxEPD_GREEN);
  Serial.println("Display is now green!");
  delay(5000);
  fillWithColor(GxEPD_BLUE);
  Serial.println("Display is now blue!");
  delay(5000);
  fillWithColor(GxEPD_RED);
  Serial.println("Display is now red!");
  delay(5000);
  fillWithColor(GxEPD_YELLOW);
  Serial.println("Display is now yellow!");
  delay(5000);
  fillWithColor(GxEPD_ORANGE);
  Serial.println("Display is now orange!");
  delay(5000);
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
  Serial.flush();
  esp_deep_sleep_start();
}

void loop() {} // this will never run!