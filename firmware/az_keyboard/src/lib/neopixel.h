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
		short _row_size; // マトリックス縦サイズ
		short _col_size; // マトリックス横サイズ
		short _led_num_len; // led_numのサイズ
		short _key_matrix_len; // キーマトリックスのサイズ
		int8_t _hide_flag; // 消灯フラグ
		int8_t *led_buf;
		int8_t *led_num; // キーのIDにLEDのIDを紐づけたデータ
		int8_t *key_matrix; // マトリックス上にどうキーが並んでいるかデータ
		int *_select_layer_no; // 今選択中のキーレイヤー
		bool *_back_flag; // キーに割り当てられていないLEDかどうかのフラグ(後ろ向きのLEDとか)
		neo_select_key select_key[NEO_SELECT_KEY_MAX]; // 常に光らせるキー
		neo_setting _setting; // 光らせ方の設定
		int8_t  setting_change; // 設定変更フラグ
		uint16_t  lotate_index; // グラデーションインデックス
		Adafruit_NeoPixel *rgb_led; // RGB_LEDオブジェクト
		Neopixel();   // コンストラクタ
		void begin(short data_pin, short led_length, short row_size, short col_size, int *select_layer, int8_t *led_num, short led_num_len, int8_t *key_matrix, short key_matrix_len); // LED制御初期化
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
		void set_status(int stat); // ON/OFFを設定する
		void set_bright(int bright); // 明るさを設定する
		void set_color_type(int color_type); // 色を設定する
		void set_shine_type(int shine_type); // 光り方を設定する
		uint32_t get_setting_color(); // 現在設定されている色と明るさのカラーを取得
		uint32_t get_lotate_color(uint8_t i); // 0 - 255 に該当する色を取得
		void select_key_cler();
		void select_key_add(int8_t layer_id, uint8_t key_num);
		void select_key_show();
		void hide_all(); // 全てのLEDを消す
		int get_data_crc32(); // NeoPixcelに設定したデータのCRC32を取得
		void rgb_led_loop_exec();
		void rgb_led_loop_type_0(); // ずっと光ってる
		void rgb_led_loop_type_1(); // ボタンを押した所から周りに広がっていく
		void rgb_led_loop_type_2(); // 
		void rgb_led_loop_type_3(); // 
		void rgb_led_loop_type_4(); // 
};

#endif
