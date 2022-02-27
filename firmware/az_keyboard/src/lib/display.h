#ifndef display_h
#define display_h


#include "Arduino.h"
#include <lvgl.h>


// サーモ表示用のボタンの位置
const uint8_t az66jp_key_position[70][2] PROGMEM = {
	{4, 4}, {8, 19}, {10, 35}, {14, 51}, {5, 67}, {20, 4}, {28, 19}, {32, 35}, {40, 51}, {24, 67},
	{36, 4}, {43, 19}, {47, 35}, {55, 51}, {0, 0}, {51, 4}, {59, 19}, {63, 35}, {71, 51}, {0, 0},
	{67, 4}, {75, 19}, {79, 35}, {87, 51}, {87, 67}, {83, 4}, {91, 19}, {95, 35}, {103, 51}, {106, 67},
	{99, 4}, {107, 19}, {111, 35}, {119, 51}, {125, 67}, {115, 4}, {123, 19}, {127, 35}, {135, 51}, {0, 0},
	{131, 4}, {139, 19}, {143, 35}, {151, 51}, {144, 67}, {147, 4}, {155, 19}, {159, 35}, {167, 51}, {163, 67},
	{163, 4}, {171, 19}, {175, 35}, {182, 51}, {179, 67}, {179, 4}, {186, 19}, {190, 35}, {198, 51}, {195, 67},
	{195, 4}, {202, 19}, {206, 35}, {0, 0}, {210, 67}, {210, 4}, {226, 4}, {224, 27}, {220, 51}, {226, 67}
};

// サーモ表示用のrgb565カラーコード
const uint16_t thermo_color[128] PROGMEM = {
	0xFFFF, 0xF7FF, 0xEFFF, 0xE7FF, 0xDFFF, 0xD7FF, 0xCFFF, 0xC7FF, 0xBFFF, 0xB7FF,
	0xAFFF, 0xA7FF, 0x9FFF, 0x97FF, 0x8FFF, 0x87FF, 0x7FFF, 0x77FF, 0x6FFF, 0x67FF,
	0x5FFF, 0x57FF, 0x4FFF, 0x47FF, 0x3FFF, 0x37FF, 0x2FFF, 0x27FF, 0x1FFF, 0x17FF,
	0x0FFF, 0x07FF, 0x07FF, 0x07BF, 0x077F, 0x073F, 0x06FF, 0x06BF, 0x067F, 0x063F,
	0x05FF, 0x05BF, 0x057F, 0x053F, 0x04FF, 0x04BF, 0x047F, 0x043F, 0x03FF, 0x03BF,
	0x037F, 0x033F, 0x02FF, 0x02BF, 0x027F, 0x023F, 0x01FF, 0x01BF, 0x017F, 0x013F,
	0x00FF, 0x00BF, 0x007F, 0x003F, 0x001F, 0x081F, 0x101F, 0x181F, 0x201F, 0x281F,
	0x301F, 0x381F, 0x401F, 0x481F, 0x501F, 0x581F, 0x601F, 0x681F, 0x701F, 0x781F,
	0x801F, 0x881F, 0x901F, 0x981F, 0xA01F, 0xA81F, 0xB01F, 0xB81F, 0xC01F, 0xC81F,
	0xD01F, 0xD81F, 0xE01F, 0xE81F, 0xF01F, 0xF81F, 0xF81F, 0xF81E, 0xF81D, 0xF81C,
	0xF81B, 0xF81A, 0xF819, 0xF818, 0xF817, 0xF816, 0xF815, 0xF814, 0xF813, 0xF812,
	0xF811, 0xF810, 0xF80F, 0xF80E, 0xF80D, 0xF80C, 0xF80B, 0xF80A, 0xF809, 0xF808,
	0xF807, 0xF806, 0xF805, 0xF804, 0xF803, 0xF802, 0xF801, 0xF800
};

#define  DISP_TYPE_STANDBY  0
#define  DISP_TYPE_SETTING  1
#define  DISP_TYPE_SAVENOW  2
#define  DISP_TYPE_WIFICNN  3
#define  DISP_TYPE_WEBFOOK  4
#define  DISP_TYPE_ANKYNOW  5
#define  DISP_TYPE_ANKINPT  6
#define  DISP_TYPE_DKTHERM  7
#define  DISP_TYPE_DKQRCOD  8


// LVGL用
bool my_touchpad_read(lv_indev_drv_t * indev_driver, lv_indev_data_t * data);
void my_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p);
void event_handler(lv_obj_t * obj, lv_event_t event);
void lv_setup();
extern uint8_t _disp_rotate; // 画面の向き


// クラスの定義
class Display
{
	public:
		short _width; // 液晶のサイズ幅
		short _height; // 液晶のサイズ高さ
		uint8_t  _info_spot; // 待ち受け等を表示する時インフォメーション部分だけを表示しなおすかどうかのフラグ
		// 最後に表示した画面
		// 0=待ち受け、1=設定モード、2=保存中、3=wifi接続中、4=WEBフック中、5=暗記中、6=暗記入力中、7=サーモ表示中、8=QRコード表示中
		uint8_t  _last_view_type;
		uint8_t  view_type; // 現在の表示モード(内容は_last_view_typeと一緒)
		uint8_t  _back_view_type; // １つ前表示していたタイプ
		// 画面下のinfo枠に最後に表示した内容(0=白枠、1=打鍵数、2=LEDステータス、3=LED明るさ、4=LED色、5=LED光らせ方、6=打鍵自動保存設定、7=打鍵数保存しました   255=何も表示していない)
		uint8_t  _last_view_info;
		short _wait_index;
		short _info_index; // infoを表示し続ける時間(待ち受け画面用)
		int dakagi_last_view; // 最後に表示した打鍵数
		uint8_t  *_stimg_data; // 待ち受け画像をRAMに読み込むポインタ
		uint8_t  _stimg_load_flag; // 待ち受け画像をRAMに読み込んでいるかどうか
		uint8_t  _thermo_flag; // サーモ表示モードかどうか
		uint8_t  _qr_flag; // 打鍵QRコード表示中かどうか
		Display();   // コンストラクタ
		void begin(uint8_t disp_rotate); // 初期化
		void view_top_page(); // 待ち受けページ表示
		void view_full_image(uint8_t *image_data); // 画面いっぱいに画像を表示する
		void view_full_black(); // 画面を真っ暗にする
		void view_int(uint16_t x, uint16_t y, int v); // 数字を表示する
		void view_dakagi(); // 打鍵数表示
		void view_dakagi_thermo(); // 打鍵サーモグラフ表示
		void view_daken_key_reset(); // 打鍵ヒートマップで今押されて緑色になっている所をヒートマップカラーにする
		void view_dakagi_qr(); // 打鍵QRコードモード表示
		void viewBMPspi_head(); // データを流し込んで画像を表示する(ヘッダ)
		void viewBMPspi_data(uint8_t *wbuf, int wsize); // データを流し込んで画像を表示する(データ)
		void view_dakagi_thermo_on(); // サーモグラフモードをON/OFF切り替える
		void view_dakagi_qr_on(); // 打鍵QRコードモードをON/OFF切り替える
		void view_black(); // 画面全体黒い画面表示
		void view_standby_image(); // 待ち受け画面表示
		void view_led_stat(); // LEDステータス表示
		void view_led_bright(); // LED 明るさ設定表示
		void view_led_color(); // LED 色設定表示
		void view_led_shine(); // LED 光らせ方設定表示
		void view_dakey_auto_save(); // 打鍵自動保存設定表示
		void view_dakey_save_comp(); // 打鍵数を保存しましたテキスト表示
		void view_setting_mode(); // 設定モード画面表示
		void view_save(); // 保存中表示
		void view_wifi_conn(); // WiFi接続中画面表示
		void view_webhook(); // Webhook中表示
		void view_ankey_now(); // 暗記中表示
		void view_setting_init_comp(); // 設定の初期化が完了しました画面表示
		void view_error_wifi_conn(); // Wifi接続して下さいエラー表示
		void open_movie(); // 起動ムービー
		void view_setting_menu(); // 設定メニュー表示
		void view_keyboard_setting(); // キーボード選択画面表示
		void view_raw_image(uint8_t *img_data); // 渡された画像データを表示
		void loop_exec(); // 定期処理
};



#endif
