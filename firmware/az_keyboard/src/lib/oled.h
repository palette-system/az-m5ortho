#ifndef oled_h
#define oled_h


#include "Arduino.h"
#include "FS.h"
#include "SPIFFS.h"
#include <Wire.h> 
#include <Adafruit_SSD1306.h> 


#define SCR_W_DEF 128 
#define SCR_H_DEF 32 
#define OLED_RESET_PIN -1 

#define OLED_DISP_DEF_PATH "/dmdef"

// クラスの定義
class Oled
{
    public:
        short _screen_w;
        short _screen_h;
        uint8_t _screen_addr;
        uint8_t *default_image;
        short default_image_load_flag;
        Adafruit_SSD1306 *display; // OLED制御用オブジェクト
		Oled();   // コンストラクタ
		void begin(short scr_w, short scr_h, uint8_t scr_addr, int khz); // LED制御初期化
        void view_default(); // デフォルト画面表示
};

#endif
