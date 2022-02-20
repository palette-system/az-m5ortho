

#include "common.h"


// LVGL用
TFT_eSPI lvtft = TFT_eSPI();
lv_disp_buf_t disp_buf;
lv_color_t lvbuf[LV_HOR_RES_MAX * 10];
int8_t lvgl_loop_index;

// 液晶表示用オブジェクト
Display *disp_cls;

// rgb_led制御用クラス
Neopixel rgb_led_cls = Neopixel();

// サウンド制御用クラス
Sound sound_cls = Sound();

// キー入力情報
int8_t input_key[KEY_PIN_LENGTH];
int8_t input_key_last[KEY_PIN_LENGTH];


// NeoPixcel、サウンドループ処理
static void background_loop(void* arg) {
  while (true) {
    unsigned long start_time = millis();
    // LED制御用処理
  	rgb_led_cls.rgb_led_loop_exec();
    // サウンドを制御する定期処理
    sound_cls.loop_exec();
    unsigned long work_time = millis() - start_time;
    if (work_time < 10) { vTaskDelay(10 - work_time); }
  }
}

// モニタループ処理
static void display_loop(void* arg) {
  while (true) {
    unsigned long start_time = millis();
  	disp_cls->loop_exec();
    unsigned long work_time = millis() - start_time;
    	if (work_time < 40) { vTaskDelay(40 - work_time); }
  }
}


// コンストラクタ
Common::Common() {
}

// 共通処理の初期化
void Common::common_start() {
    int i;
    // M5Stack初期化
    M5.begin(true, true, true, true);
    M5.Axp.SetLed(false); // サイドの緑LEDを消す
    // 乱数初期化
    randomSeed(millis());
    // ファイルシステム初期化
    if(!SPIFFS.begin(true)){
        return;
    }
	  // neopixcel初期化
	  rgb_led_cls.begin( NEOPIXEL_DATA_PIN, NEOPIXEL_LED_LENGTH);
	  // サウンドクラス初期化
	  sound_cls.begin();
  	// 画面クラス初期化
    disp_cls = new Display();
    disp_cls->begin(DISPLAY_LOTATE);
    // キーのピンの初期化
    for (i=0; i<KEY_PIN_LENGTH; i++) {
        pinMode(key_pin_num[i], INPUT_PULLUP);
    }
    // キー入力の情報初期化
    for (i=0; i<KEY_PIN_LENGTH; i++) {
        input_key[i] = 0;
        input_key_last[i] = 0;
    }
    // サウンド、Neopixcel用バック処理
    xTaskCreatePinnedToCore(background_loop, "bgloop", 2048, NULL, 20, NULL, 0);
    // 画面用バック処理
    xTaskCreatePinnedToCore(display_loop, "disploop", 2048, NULL, 20, NULL, 1);
}

// 現在のキーの入力状態を取得
void Common::key_read(void) {
    int i, j, n, c, p;
    p = 0;
    // 前の状態を保持
    for (i=0; i<KEY_PIN_LENGTH; i++) {
        input_key_last[i] = input_key[i];
    }
    // ダイレクト入力の取得
    for (i=0; i<KEY_PIN_LENGTH; i++) {
        input_key[i] = !digitalRead(key_pin_num[i]);
    }
}

