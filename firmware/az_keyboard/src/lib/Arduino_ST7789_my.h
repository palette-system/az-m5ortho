/***************************************************
  This is a library for the ST7789 IPS SPI display.

  Originally written by Limor Fried/Ladyada for 
  Adafruit Industries.

  Modified by Ananev Ilia
 ****************************************************/

#ifndef _ADAFRUIT_ST7789H_
#define _ADAFRUIT_ST7789H_

#include "Arduino.h"
#include "SPIFFS.h"
#include "Print.h"
#include <Adafruit_GFX.h>


#define ST7789_TFTWIDTH   135
#define ST7789_TFTHEIGHT  240

#define ST7789_240x240_XSTART 52
#define ST7789_240x240_YSTART 40

#define ST_CMD_DELAY   0x80    // special signifier for command lists

#define ST7789_NOP     0x00
#define ST7789_SWRESET 0x01
#define ST7789_RDDID   0x04
#define ST7789_RDDST   0x09

#define ST7789_SLPIN   0x10
#define ST7789_SLPOUT  0x11
#define ST7789_PTLON   0x12
#define ST7789_NORON   0x13

#define ST7789_INVOFF  0x20
#define ST7789_INVON   0x21
#define ST7789_DISPOFF 0x28
#define ST7789_DISPON  0x29
#define ST7789_CASET   0x2A
#define ST7789_RASET   0x2B
#define ST7789_RAMWR   0x2C
#define ST7789_RAMRD   0x2E

#define ST7789_PTLAR   0x30
#define ST7789_COLMOD  0x3A
#define ST7789_MADCTL  0x36

#define ST7789_MADCTL_MY  0x80
#define ST7789_MADCTL_MX  0x40
#define ST7789_MADCTL_MV  0x20
#define ST7789_MADCTL_ML  0x10
#define ST7789_MADCTL_RGB 0x00

#define ST7789_RDID1   0xDA
#define ST7789_RDID2   0xDB
#define ST7789_RDID3   0xDC
#define ST7789_RDID4   0xDD

// Color definitions
#define BLACK   0x0000
#define BLUE    0x001F
#define RED     0xF800
#define GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF

class Arduino_ST7789 : public Adafruit_GFX {

 public:

  Arduino_ST7789(int8_t DC, int8_t RST, int8_t SID, int8_t SCLK, int8_t CS = -1);
  Arduino_ST7789(int8_t DC, int8_t RST, int8_t CS = -1);

  void     setAddrWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1),
           pushColor(uint16_t color),
           fillScreen(uint16_t color),
           drawPixel(int16_t x, int16_t y, uint16_t color),
           drawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color),
           drawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color),
           fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color),
           setRotation(uint8_t r),
           invertDisplay(boolean i),
           viewBMP(int16_t x, int16_t y, int16_t w, int16_t h, uint8_t * bmp_data, uint8_t opa),
	       viewBMPFile(int16_t x, int16_t y, int16_t w, int16_t h, String file_path, int offset),
	       viewBMPspi_head(int16_t x, int16_t y, int16_t w, int16_t h), // �f�[�^�𗬂�����ŉ摜��\������(�w�b�_)
	       viewBMPspi_data(uint8_t *wbuf, int wsize), // �f�[�^�𗬂�����ŉ摜��\������(�f�[�^)
       init(uint16_t width, uint16_t height);
  uint16_t Color565(uint8_t r, uint8_t g, uint8_t b);
  uint16_t color565(uint8_t r, uint8_t g, uint8_t b) { return Color565(r, g, b); } 

 protected:
  uint8_t  _colstart, _rowstart, _xstart, _ystart; // some displays need this changed

  void     displayInit(const uint8_t *addr);
  void     spiwrite(uint8_t),
           spiwrite_color(uint16_t send_color, uint16_t send_size),
           writecommand(uint8_t c),
           writedata(uint8_t d),
           commonInit(const uint8_t *cmdList);

 private:



  int8_t  _cs, _dc, _rst, _sid, _sclk;

#if defined(USE_FAST_IO)
  volatile RwReg  *dataport, *clkport, *csport, *dcport;

  #if defined(__AVR__) || defined(CORE_TEENSY)  // 8 bit!
    uint8_t  datapinmask, clkpinmask, cspinmask, dcpinmask;
  #else    // 32 bit!
    uint32_t  datapinmask, clkpinmask, cspinmask, dcpinmask;
  #endif
#endif

};

#endif
