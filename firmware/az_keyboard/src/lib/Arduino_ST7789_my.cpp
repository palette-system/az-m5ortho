/***************************************************
  This is a library for the ST7789 IPS SPI display.

  Originally written by Limor Fried/Ladyada for 
  Adafruit Industries.

  Modified by Ananev Ilia
 ****************************************************/

#include "Arduino_ST7789_my.h"
#include <limits.h>
#include "pins_arduino.h"
#include "wiring_private.h"
#include <SPI.h>


static const uint8_t PROGMEM
  cmd_240x240[] = {                     // Initialization commands for 7789 screens
    10,                               // 9 commands in list:
    ST7789_SWRESET,   ST_CMD_DELAY,     // 1: Software reset, no args, w/delay
      150,                            // 150 ms delay
    ST7789_SLPOUT ,   ST_CMD_DELAY,     // 2: Out of sleep mode, no args, w/delay
      255,                            // 255 = 500 ms delay
    ST7789_COLMOD , 1+ST_CMD_DELAY,     // 3: Set color mode, 1 arg + delay:
      0x55,                           // 16-bit color
      10,                             // 10 ms delay
    ST7789_MADCTL , 1,            // 4: Memory access ctrl (directions), 1 arg:
      0x00,                           // Row addr/col addr, bottom to top refresh
    ST7789_CASET  , 4,            // 5: Column addr set, 4 args, no delay:
      0x00, ST7789_240x240_XSTART,          // XSTART = 0
    (ST7789_TFTWIDTH+ST7789_240x240_XSTART) >> 8,
    (ST7789_TFTWIDTH+ST7789_240x240_XSTART) & 0xFF,   // XEND = 240
    ST7789_RASET  , 4,            // 6: Row addr set, 4 args, no delay:
      0x00, ST7789_240x240_YSTART,          // YSTART = 0
      (ST7789_TFTHEIGHT+ST7789_240x240_YSTART) >> 8,
    (ST7789_TFTHEIGHT+ST7789_240x240_YSTART) & 0xFF,  // YEND = 240
    ST7789_INVON ,   ST_CMD_DELAY,      // 7: Inversion ON
      10,
    ST7789_NORON  ,   ST_CMD_DELAY,     // 8: Normal display on, no args, w/delay
      10,                             // 10 ms delay
    ST7789_DISPON ,   ST_CMD_DELAY,     // 9: Main screen turn on, no args, w/delay
    255 };                          // 255 = 500 ms delay

inline uint16_t swapcolor(uint16_t x) { 
  return (x << 11) | (x & 0x07E0) | (x >> 11);
}


// Constructor when using software SPI.  All output pins are configurable.
Arduino_ST7789::Arduino_ST7789(int8_t dc, int8_t rst, int8_t sid, int8_t sclk, int8_t cs) 
  : Adafruit_GFX(ST7789_TFTWIDTH, ST7789_TFTHEIGHT)
{
  _cs   = cs;
  _dc   = dc;
  _sid  = sid;
  _sclk = sclk;
  _rst  = rst;
}

// Constructor when using hardware SPI.  Faster, but must use SPI pins
// specific to each board type (e.g. 11,13 for Uno, 51,52 for Mega, etc.)
Arduino_ST7789::Arduino_ST7789(int8_t dc, int8_t rst, int8_t cs) 
  : Adafruit_GFX(ST7789_TFTWIDTH, ST7789_TFTHEIGHT) {
  _cs   = cs;
  _dc   = dc;
  _rst  = rst;
  _sid  = _sclk = -1;
}

inline void Arduino_ST7789::spiwrite(uint8_t c) 
{
  SPI.transfer(c);
}

inline void Arduino_ST7789::spiwrite_color(uint16_t send_color, uint16_t send_size) 
{
  uint16_t i, j;
  uint8_t wbuf[5120];
  uint8_t hi = send_color >> 8, lo = send_color;
	i = 0;
	while (i < 5120) {
		wbuf[i] = hi;
		i++;
		wbuf[i] = lo;
		i++;
	}
	i = 0;
	while (i < send_size) {
		j = send_size - i;
		if (j >= 2560) j = 2560;
		SPI.writeBytes(wbuf, j * 2);
		i += j;
	}
}



void Arduino_ST7789::writecommand(uint8_t c) {

  digitalWrite(_dc, LOW); // dc low はここだけ

  spiwrite(c);

}

void Arduino_ST7789::writedata(uint8_t c) {
  digitalWrite(_dc, HIGH);
    
  spiwrite(c);

}

// Companion code to the above tables.  Reads and issues
// a series of LCD commands stored in PROGMEM byte array.
void Arduino_ST7789::displayInit(const uint8_t *addr) {

  uint8_t  numCommands, numArgs;
  uint16_t ms;
  //<-----------------------------------------------------------------------------------------
  digitalWrite(_dc, HIGH);
      digitalWrite(_sclk, HIGH);
  //<-----------------------------------------------------------------------------------------

  numCommands = pgm_read_byte(addr++);   // Number of commands to follow
  while(numCommands--) {                 // For each command...
    writecommand(pgm_read_byte(addr++)); //   Read, issue command
    numArgs  = pgm_read_byte(addr++);    //   Number of args to follow
    ms       = numArgs & ST_CMD_DELAY;   //   If hibit set, delay follows args
    numArgs &= ~ST_CMD_DELAY;            //   Mask out delay bit
    while(numArgs--) {                   //   For each argument...
      writedata(pgm_read_byte(addr++));  //     Read, issue argument
    }

    if(ms) {
      ms = pgm_read_byte(addr++); // Read post-command delay time (ms)
      if(ms == 255) ms = 500;     // If 255, delay for 500 ms
      delay(ms);
    }
  }
}


// Initialization code common to all ST7789 displays
void Arduino_ST7789::commonInit(const uint8_t *cmdList) {
  _ystart = _xstart = 0;
  _colstart  = _rowstart = 0; // May be overridden in init func

    pinMode(_dc, OUTPUT);

  SPI.begin(_sclk,-1,_sid,-1);
  SPI.beginTransaction(SPISettings(20000000, MSBFIRST, SPI_MODE3)); // 20MHz


    pinMode(_rst, OUTPUT);
    digitalWrite(_rst, HIGH);
    delay(50);
    digitalWrite(_rst, LOW);
    delay(50);
    digitalWrite(_rst, HIGH);
    delay(50);

  
  
  if(cmdList) 
    displayInit(cmdList);
}

void Arduino_ST7789::setRotation(uint8_t m) {

  writecommand(ST7789_MADCTL);
  uint8_t rotation = m % 4; // can't be higher than 3
  switch (rotation) {
   case 0:
     writedata(ST7789_MADCTL_MX | ST7789_MADCTL_MY | ST7789_MADCTL_RGB);

     _xstart = _colstart;
     _ystart = _rowstart;
     break;
   case 1:
     writedata(ST7789_MADCTL_MY | ST7789_MADCTL_MV | ST7789_MADCTL_RGB);

     _ystart = _colstart;
     _xstart = _rowstart;
     break;
  case 2:
     writedata(ST7789_MADCTL_RGB);
 
     _xstart = _colstart;
     _ystart = _rowstart;
     break;

   case 3:
     writedata(ST7789_MADCTL_MX | ST7789_MADCTL_MV | ST7789_MADCTL_RGB);

     _ystart = _colstart;
     _xstart = _rowstart;
     break;
  }
}

void Arduino_ST7789::setAddrWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1) {

  uint16_t x_start = x0 + _xstart, x_end = x1 + _xstart;
  uint16_t y_start = y0 + _ystart, y_end = y1 + _ystart;
  

  writecommand(ST7789_CASET); // Column addr set
  writedata(x_start >> 8);
  writedata(x_start & 0xFF);     // XSTART 
  writedata(x_end >> 8);
  writedata(x_end & 0xFF);     // XEND

  writecommand(ST7789_RASET); // Row addr set
  writedata(y_start >> 8);
  writedata(y_start & 0xFF);     // YSTART
  writedata(y_end >> 8);
  writedata(y_end & 0xFF);     // YEND

  writecommand(ST7789_RAMWR); // write to RAM
}

void Arduino_ST7789::pushColor(uint16_t color) {
  digitalWrite(_dc, HIGH);

  spiwrite_color(color, 1);

}

void Arduino_ST7789::drawPixel(int16_t x, int16_t y, uint16_t color) {

  if((x < 0) ||(x >= _width) || (y < 0) || (y >= _height)) return;

  setAddrWindow(x,y,x+1,y+1);

  digitalWrite(_dc, HIGH);

  spiwrite_color(color, 1);

}

void Arduino_ST7789::drawFastVLine(int16_t x, int16_t y, int16_t h,
 uint16_t color) {

  // Rudimentary clipping
  if((x >= _width) || (y >= _height)) return;
  if((y+h-1) >= _height) h = _height-y;
  setAddrWindow(x, y, x, y+h-1);
    
  digitalWrite(_dc, HIGH);
  spiwrite_color(color, h);
}

void Arduino_ST7789::drawFastHLine(int16_t x, int16_t y, int16_t w,
  uint16_t color) {

  // Rudimentary clipping
  if((x >= _width) || (y >= _height)) return;
  if((x+w-1) >= _width)  w = _width-x;
  setAddrWindow(x, y, x+w-1, y);

  digitalWrite(_dc, HIGH);
  spiwrite_color(color, w);

}

void Arduino_ST7789::fillScreen(uint16_t color) {
  fillRect(0, 0,  _width, _height, color);
}

// fill a rectangle
void Arduino_ST7789::fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color) {

  // rudimentary clipping (drawChar w/big text requires this)
  if((x >= _width) || (y >= _height)) return;
  if((x + w - 1) >= _width)  w = _width  - x;
  if((y + h - 1) >= _height) h = _height - y;

  setAddrWindow(x, y, x+w-1, y+h-1);
  digitalWrite(_dc, HIGH);
  spiwrite_color(color, (w * h));
}


// Pass 8-bit (each) R,G,B, get back 16-bit packed color
uint16_t Arduino_ST7789::Color565(uint8_t r, uint8_t g, uint8_t b) {
  return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
}

uint16_t color_opa_b(uint8_t hi, uint8_t lo, uint8_t opa) {
  uint8_t r = (hi & 0xF8) >> 3;
  uint8_t g = ((hi & 0x07) << 3) | ((lo & 0xE0) >> 5);
  uint8_t b = lo & 0x1F;
  int ir = r * opa / 10;
  int ig = g * opa / 10;
  int ib = b * opa / 10;
  return (ir << 11) | (ig << 5) | (ib);
}

uint16_t color_opa_w(uint8_t hi, uint8_t lo, uint8_t opa) {
  uint8_t r = (hi & 0xF8) >> 3;
  uint8_t g = ((hi & 0x07) << 3) | ((lo & 0xE0) >> 5);
  uint8_t b = lo & 0x1F;
	int ir = 31 - ((31 - r) * opa / 10);
	int ig = 63 - ((63 - g) * opa / 10);
	int ib = 31 - ((31 - b) * opa / 10);
  return (ir << 11) | (ig << 5) | (ib);
}

void Arduino_ST7789::invertDisplay(boolean i) {
  writecommand(i ? ST7789_INVON : ST7789_INVOFF);
}

void Arduino_ST7789::viewBMP(int16_t x, int16_t y, int16_t w, int16_t h, uint8_t * bmp_data, uint8_t opa) {
	uint8_t wbuf[512];
	uint16_t c;
	int i,j,k,p;
	int vx, vy, vw, vh;
	if ((x + w) > _width) {
		if (x < 0) {
			vx = 0;
			vw = _width - 1;
		} else {
			vx = x;
			vw = _width - x - 1;
		}
	} else {
		if (x < 0) {
			vx = 0;
			vw = w + x - 1;
		} else {
			vx = x;
			vw = w - 1;
		}
	}
	if ((y + h) > _height) {
		if (y < 0) {
			vy = 0;
			vh = _height - 1;
		} else {
			vy = y;
			vh = _height - y - 1;
		}
	} else {
		if (y < 0) {
			vy = 0;
			vh = h + y - 1;
		} else {
			vy = y;
			vh = h - 1;
		}
	}
	setAddrWindow(vx, vy, vx + vw, vy + vh);
	digitalWrite(_dc, HIGH);
	p = 0;
	for (i=0; i<h; i++) {
		if ((i + y) > (vy + vh)) continue;
		if (y < 0 && (i + y) < 0) continue;
		for (j=0; j<w; j++) {
			if ((j + x) > (vx + vw)) continue;
			if (x < 0 && (j + x) < 0) continue;
			k = (i * 2 * w) + (j*2);
			if (opa > 9) {
				c = bmp_data[k + 1] << 8 | bmp_data[k];
			} else {
				c = color_opa_w(bmp_data[k + 1], bmp_data[k], opa);
			}
			wbuf[p] = c >> 8;
			p++;
			wbuf[p] = c & 0xFF;
			p++;
			if (p > 500) {
				SPI.writeBytes(wbuf, p);
				p = 0;
			}
		}
	}
	if (p > 0) {
		SPI.writeBytes(wbuf, p);
		p = 0;
	}
}

// 画像ファイルを開いて表示する
void Arduino_ST7789::viewBMPFile(int16_t x, int16_t y, int16_t w, int16_t h, String file_path, int offset) {
    uint8_t wbuf[512];
    int i, b;
    int s = w * h * 2;
    setAddrWindow(x, y, x + w - 1, y + h - 1);
    digitalWrite(_dc, HIGH);
    if(SPIFFS.exists(file_path)){
        File fp = SPIFFS.open(file_path, "r");
    	if (offset > 0) {
    		fp.seek(offset);
    	}
        i = 0;
        while(fp.available() && s > 0){
        	b = (s < 512)? s: 512;
            i = fp.read(wbuf, b);
        	SPI.writeBytes(wbuf, i);
        	s -= i;
        }
        fp.close();
    }
}

// データを流し込んで画像を表示する(ヘッダ)
void Arduino_ST7789::viewBMPspi_head(int16_t x, int16_t y, int16_t w, int16_t h) {
    setAddrWindow(x, y, x + w - 1, y + h - 1);
    digitalWrite(_dc, HIGH);
}

// データを流し込んで画像を表示する(データ)
void Arduino_ST7789::viewBMPspi_data(uint8_t *wbuf, int wsize) {
    SPI.writeBytes(wbuf, wsize);
}


/******** low level bit twiddling **********/



void Arduino_ST7789::init(uint16_t width, uint16_t height) {
  commonInit(NULL);

  _colstart = ST7789_240x240_XSTART;
  _rowstart = ST7789_240x240_YSTART;
  _height = height;
  _width = width;

  displayInit(cmd_240x240);

  // setRotation(1);
}
