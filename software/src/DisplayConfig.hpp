#ifndef DISPLAY_CONFIG_HPP_
#define DISPLAY_CONFIG_HPP_

#include <GxEPD2_7C.h>

#define GxEPD2_DISPLAY_CLASS GxEPD2_7C
#define GxEPD2_DRIVER_CLASS GxEPD2_730c_ACeP_730

// Connections for e.g. LOLIN D32
#define EPD_BUSY  4 // to EPD BUSY
#define EPD_CS    5 // to EPD CS
#define EPD_RST  16 // to EPD RST
#define EPD_DC   17 // to EPD DC
#define EPD_SCK  18 // to EPD CLK
#define EPD_MISO 19 // Master-In Slave-Out not used, as no data from display
#define EPD_MOSI 23 // to EPD DIN

#define MAX_DISPLAY_BUFFER_SIZE 16000UL
#define PAGE_HEIGHT (GxEPD2_DRIVER_CLASS::HEIGHT <= (MAX_DISPLAY_BUFFER_SIZE) / (GxEPD2_DRIVER_CLASS::WIDTH / 2) ? GxEPD2_DRIVER_CLASS::HEIGHT : (MAX_DISPLAY_BUFFER_SIZE) / (GxEPD2_DRIVER_CLASS::WIDTH / 2))
#define NUMBER_OF_PAGES (GxEPD2_DRIVER_CLASS::HEIGHT / PAGE_HEIGHT + ((GxEPD2_DRIVER_CLASS::HEIGHT % PAGE_HEIGHT) > 0))
GxEPD2_DISPLAY_CLASS<GxEPD2_DRIVER_CLASS, /*PAGE_HEIGHT*/1> display(GxEPD2_DRIVER_CLASS(EPD_CS, EPD_DC, EPD_RST, EPD_BUSY));
//#undef MAX_DISPLAY_BUFFER_SIZE
//#undef PAGE_HEIGHT
//#undef NUMBER_OF_PAGES

SPIClass hspi(HSPI);

void initialiseDisplay() {
    hspi.begin(EPD_SCK, EPD_MISO, EPD_MOSI, EPD_CS);
    display.epd2.selectSPI(hspi, SPISettings(4000000, MSBFIRST, SPI_MODE0));
    display.init(115200, true, 2, false);
    display.clearScreen();
    display.setFullWindow();
}

// custom base64 map to directly map to the E-Ink 7 color schema
const uint8_t PROGMEM base64Map[256] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, /*0*/ 0b01100100, /*1*/ 0b01100101, /*2*/ 0b01100110, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    /*A*/ 0b00000000, /*B*/ 0b00000001, /*C*/ 0b00000010, /*D*/ 0b00000011, /*E*/ 0b00000100, /*F*/ 0b00000101, /*G*/ 0b00000110, 0, // no H
    /*I*/ 0b00010000, /*J*/ 0b00010001, /*K*/ 0b00010010, /*L*/ 0b00010011, /*M*/ 0b00010100, /*N*/ 0b00010101, /*O*/ 0b00010110, 0, // no P
    /*Q*/ 0b00100000, /*R*/ 0b00100001, /*S*/ 0b00100010, /*T*/ 0b00100011, /*U*/ 0b00100100, /*V*/ 0b00100101, /*W*/ 0b00100110, 0, // no X
    /*Y*/ 0b00110000, /*Z*/ 0b00110001, 0, 0, 0, 0, 0, 0, /*a*/ 0b00110010, /*b*/ 0b00110011, /*c*/ 0b00110100, /*d*/ 0b00110101, /*e*/ 0b00110110, 0, // no f
    /*g*/ 0b01000000, /*h*/ 0b01000001, /*i*/ 0b01000010, /*j*/ 0b01000011, /*k*/ 0b01000100, /*l*/ 0b01000101, /*m*/ 0b01000110, 0, // no n
    /*o*/ 0b01010000, /*p*/ 0b01010001, /*q*/ 0b01010010, /*r*/ 0b01010011, /*s*/ 0b01010100, /*t*/ 0b01010101, /*u*/ 0b01010110, 0, // no v
    /*w*/ 0b01100000, /*x*/ 0b01100001, /*y*/ 0b01100010, /*z*/ 0b01100011, // no 3, 4, 5, 6, 7, 8, 9, +, /
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

#endif