#include <M5Core2.h>
#include "Arduino.h"
#include "neopixel.h"


// コンストラクタ
Neopixel::Neopixel() {
	this->_data_pin = -1;
	this->_led_length = -1;
	this->lotate_index = 0;
}


// LED制御初期化
void Neopixel::begin(short data_pin, short led_length) {
	this->_led_length = led_length;
    // RGB_LEDピン用の初期化
    if (data_pin >= 0 && this->_led_length > 0) {
        this->rgb_led = new Adafruit_NeoPixel(this->_led_length, data_pin, NEO_GRB + NEO_KHZ800);
    	hide_all();
    }
	this->_data_pin = data_pin;
}

// 設定ファイルの読み込み
void Neopixel::setting_load() {
	// デフォルト値
	this->_setting.status = 1;
	this->_setting.bright = 8;
	this->_setting.color_type = 2;
	this->_setting.shine_type = 2;
	// 設定ファイルがあれば読み込む
	if (SPIFFS.exists(NEO_SETTING_PATH)) {
		File fp = SPIFFS.open(NEO_SETTING_PATH, "r");
		int i = fp.read((uint8_t *)&this->_setting, sizeof(neo_setting));
		fp.close();
	}
}

// 設定ファイルに保存
void Neopixel::setting_save() {
	File fp = SPIFFS.open(NEO_SETTING_PATH, "w");
	int i = fp.write((uint8_t *)&this->_setting, sizeof(neo_setting));
	fp.close();
}


// 消灯
void Neopixel::hide_all() {
	if (this->_data_pin < 0) return; // RGB_LEDが無ければ何もしない
	int i;
	uint32_t n = this->rgb_led->Color(0, 0, 0);
    // LEDを点灯
    for (i=0; i<this->_led_length; i++) {
        this->rgb_led->setPixelColor(i, n);
    }
	// LEDにデータを送る
    this->rgb_led->show();
};


//    2
//  3 1 4
//    5

//    6
//  7   8
//    9

// RGB_LEDを制御する定期処理
void Neopixel::rgb_led_loop_exec() {
    // RGB_LED の設定が無ければ何もしない
    if (this->_data_pin < 0 || this->_led_length <= 0) return;
	int i;
    // LEDを点灯
    for (i=0; i<this->_led_length; i++) {
    	if ((this->lotate_index % 10) == i) {
            this->rgb_led->setPixelColor(i, this->rgb_led->Color(0, 0, 0));
    	} else {
            this->rgb_led->setPixelColor(i, this->rgb_led->Color(0, 0, 0));
    	}
    }
	// LEDにデータを送る
    this->rgb_led->show();
	this->lotate_index++;
	if (this->lotate_index >= 256) this->lotate_index = 0;
}



