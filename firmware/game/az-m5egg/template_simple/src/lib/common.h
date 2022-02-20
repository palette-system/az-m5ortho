#ifndef Common_h
#define Common_h

#include <M5Core2.h>
#include <Arduino.h>
#include <lvgl.h>
#include <Wire.h>
#include <SPI.h>

#include "FS.h"
#include "SPIFFS.h"


#include <ArduinoJson.h>
#include <WiFi.h>
#include <WiFiMulti.h>
#include <HTTPClient.h>
#include <Adafruit_NeoPixel.h>
#include  <AXP192.h>


#include "display.h"
#include "neopixel.h"
#include "sound.h"


// RGBLEDのピンと個数
#define NEOPIXEL_DATA_PIN    14
#define NEOPIXEL_LED_LENGTH  10

// キーのピン数
#define  KEY_PIN_LENGTH  6

// 画面の向き(0=左 / 1=上 / 2=右 / 3=下)
#define DISPLAY_LOTATE  1

// LVGL用
extern TFT_eSPI lvtft;
extern lv_disp_buf_t disp_buf;
extern lv_color_t lvbuf[LV_HOR_RES_MAX * 10];
extern int8_t lvgl_loop_index;

// キーピン
const int8_t key_pin_num[KEY_PIN_LENGTH] = {34, 19, 27, 13, 36, 35};

// 液晶表示用オブジェクト
extern Display *disp_cls;

// rgb_led制御用クラス
extern Neopixel rgb_led_cls;

// サウンド制御用クラス
extern Sound sound_cls;

// キー入力情報
extern int8_t input_key[KEY_PIN_LENGTH];
extern int8_t input_key_last[KEY_PIN_LENGTH];



class Common
{
    public:
        Common();   // コンストラクタ
        void common_start(); // 共通処理の初期処理(setup時に呼ばれる)
        void key_read(void);
};


#endif

