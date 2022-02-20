#ifndef neopixel_h
#define neopixel_h


#include "Arduino.h"
#include "FS.h"
#include "SPIFFS.h"
#include <Adafruit_NeoPixel.h>



// 常に光らせるキー最大数
#define  NEO_SELECT_KEY_MAX  8

// 明るさの最大値
#define  NEO_BRIGHT_MAX  16

// 設定ファイルのパス
#define NEO_SETTING_PATH  "/neo.dat"

// 常に光らせるキー
struct neo_select_key {
	int8_t layer_id; // レイヤー
	uint8_t key_num; // キーの番号
};


// カラーLED用設定データ
struct neo_setting {
	int8_t  status;   // LEDステータス (0=光らせない / 1=光らせる)
	uint8_t  bright;  // 明るさ (0-5)
	uint8_t  color_type;  // 光らせる色 (あらかじめ定義している色)
	uint8_t  shine_type; // 光り方のタイプ ()
};


// カラーバリエーション
const char neo_color_list[6][3] PROGMEM = {
	{128, 128, 128},
	{128, 0, 0},
	{0, 128, 0},
	{0, 0, 128},
	{0, 128, 128},
	{128, 128, 0}
};



// クラスの定義
class Neopixel
{
    public:
		short _data_pin; // データピン
		short _led_length; // 接続LED数
		uint16_t  lotate_index; // グラデーションインデックス
		neo_setting _setting; // 光らせ方の設定
		Adafruit_NeoPixel *rgb_led; // RGB_LEDオブジェクト
		Neopixel();   // コンストラクタ
		void begin(short data_pin, short led_length); // LED制御初期化
		void setting_load();
		void setting_save();
		void hide_all(); // 全てのLEDを消す
		void rgb_led_loop_exec();
};

#endif
