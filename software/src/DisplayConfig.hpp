#ifndef DISPLAY_CONFIG_HPP_
#define DISPLAY_CONFIG_HPP_

#include <GxEPD2_7C.h>

#define GxEPD2_DISPLAY_CLASS GxEPD2_7C
#define GxEPD2_DRIVER_CLASS GxEPD2_730c

// Connections for e.g. LOLIN D32
#define EPD_BUSY  4 // to EPD BUSY
#define EPD_CS    5 // to EPD CS
#define EPD_RST  16 // to EPD RST
#define EPD_DC   17 // to EPD DC
#define EPD_SCK  18 // to EPD CLK
#define EPD_MISO 19 // Master-In Slave-Out not used, as no data from display
#define EPD_MOSI 23 // to EPD DIN

#define MAX_DISPLAY_BUFFER_SIZE 48000UL
#define MAX_HEIGHT(EPD) (EPD::HEIGHT <= (MAX_DISPLAY_BUFFER_SIZE) / (EPD::WIDTH / 2) ? EPD::HEIGHT : (MAX_DISPLAY_BUFFER_SIZE) / (EPD::WIDTH / 2))
GxEPD2_DISPLAY_CLASS<GxEPD2_DRIVER_CLASS, MAX_HEIGHT(GxEPD2_DRIVER_CLASS)> display(GxEPD2_DRIVER_CLASS(EPD_CS, EPD_DC, EPD_RST, EPD_BUSY));
#undef MAX_DISPLAY_BUFFER_SIZE
#undef MAX_HEIGHT

SPIClass hspi(HSPI);

void initialiseDisplay() {
    hspi.begin(EPD_SCK, EPD_MISO, EPD_MOSI, EPD_CS);
    display.epd2.selectSPI(hspi, SPISettings(4000000, MSBFIRST, SPI_MODE0));
    display.init(115200, true, 2, false);
    display.clearScreen();
    display.setFullWindow();
}

#endif