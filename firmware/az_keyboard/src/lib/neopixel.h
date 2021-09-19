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
	{128, 128, 0},
	{0, 128, 128}
};



// クラスの定義
class Neopixel
{
    public:
		short _data_pin; // データピン
		short _led_length; // 接続LED数
		short _row_size; // マトリックス縦サイズ
		short _col_size; // マトリックス横サイズ
		int8_t _hide_flag; // 消灯フラグ
		int8_t *led_buf;
		int8_t *led_num;
		int8_t *key_matrix;
		int *_select_layer_no;
		neo_select_key select_key[NEO_SELECT_KEY_MAX]; // 常に光らせるキー
		neo_setting _setting; // 光らせ方の設定
		int8_t  setting_change; // 設定変更フラグ
		uint16_t  lotate_index; // グラデーションインデックス
		Adafruit_NeoPixel *rgb_led; // RGB_LEDオブジェクト
		Neopixel();   // コンストラクタ
		void begin(short data_pin, short row_size, short col_size, int *select_layer, int8_t *led_num, int8_t *key_matrix); // LED制御初期化
		void setting_load();
		void setting_save();
		void set_led_num(int8_t key, int8_t val);
		void set_key_matrix(int8_t key, int8_t val);
		void set_led_buf(int8_t key_id, int8_t set_num);
		void setting_status();
		void setting_bright_up();  // LEDの明るさアップ
		void setting_bright_down();  // LEDの明るさダウン
		void setting_color_type();  // LEDの色変更
		void setting_shine_type();  // LEDの光らせ方変更
		uint32_t get_setting_color(); // 現在設定されている色と明るさのカラーを取得
		uint32_t get_lotate_color(uint8_t i); // 0 - 255 に該当する色を取得
		void select_key_cler();
		void select_key_add(int8_t layer_id, uint8_t key_num);
		void select_key_show();
		void hide_all(); // 全てのLEDを消す
		void rgb_led_loop_exec();
		void rgb_led_loop_type_0(); // ずっと光ってる
		void rgb_led_loop_type_1(); // ボタンを押した所から周りに広がっていく
		void rgb_led_loop_type_2(); // 
		void rgb_led_loop_type_3(); // 
		void rgb_led_loop_type_4(); // 
};

#endif
