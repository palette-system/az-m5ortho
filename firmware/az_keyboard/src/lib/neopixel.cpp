#include <M5Core2.h>
#include "Arduino.h"
#include "neopixel.h"

#include "../../az_common.h"


// コンストラクタ
Neopixel::Neopixel() {
	this->_data_pin = -1;
	this->_led_length = -1;
	this->_row_size = -1;
	this->_col_size = -1;
	this->lotate_index = 0;
}


// LED制御初期化
void Neopixel::begin(short data_pin, short led_length, short row_size, short col_size, int *select_layer, int8_t *led_num, short led_num_len, int8_t *key_matrix, short key_matrix_len) {
	int i;
	this->_led_length = led_length;
	this->_row_size = row_size;
	this->_col_size = col_size;
	this->_select_layer_no = select_layer;
	this->led_buf = new int8_t[key_matrix_len];
	for (i=0; i<key_matrix_len; i++) { this->led_buf[i] = 0; }
	this->led_num = led_num;
	this->_led_num_len = led_num_len;
	this->key_matrix = key_matrix;
	this->_key_matrix_len = key_matrix_len;
	// キーに割り当てられていないLEDをtrueにする
	this->_back_flag = new bool[this->_led_length];
	for (i=0; i<this->_led_length; i++) { this->_back_flag[i] = true; } // 一旦全てtrue
	for (i=0; i<this->_key_matrix_len; i++) {
		if (this->key_matrix[i] >= 0) this->_back_flag[i] = false; // キーに割り当てられてる所をfalseにする
	}
	// 消灯フラグ(最初は１回消灯させる)
	this->_hide_flag = 1;
	// 設定ファイル読み込み
	this->setting_load();
	// 設定変更フラグ
	this->setting_change = 0;
	// 常に光らせるキー
    this->select_key_cler();
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

// LED番号のリストのデータを受け取る
void Neopixel::set_led_num(int8_t key, int8_t val) {
    this->led_num[key] = val;
}

// キーマトリックスのデータを受け取る
void Neopixel::set_key_matrix(int8_t key, int8_t val) {
    this->key_matrix[key] = val;
}

// 指定したキーにアクション番号を入れる
void Neopixel::set_led_buf(int8_t key_id, int8_t set_num) {
    if (this->_data_pin < 0 || this->_led_length <= 0) return;
    int i;
    for (i=0; i<this->_key_matrix_len; i++) {
        if (this->key_matrix[i] == key_id) {
            this->led_buf[i] = set_num;
            return;
        }
    }
}

// LEDを光らせる、光らせないを切り替える
void Neopixel::setting_status() {
	if (this->_setting.status) {
		this->_setting.status = 0;
		this->_hide_flag = 1; // 消灯フラグ ON
	} else {
		this->_setting.status = 1;
	}
	this->setting_save(); // 設定を保存
	this->setting_change = 1;
}

// LEDの明るさアップ
void Neopixel::setting_bright_up() {
	if (this->_setting.bright < NEO_BRIGHT_MAX) {
		this->_setting.bright++;
	}
	this->setting_save(); // 設定を保存
	this->setting_change = 2;
}

// LEDの明るさダウン
void Neopixel::setting_bright_down() {
	if (this->_setting.bright > 0) {
		this->_setting.bright--;
	}
	this->setting_save(); // 設定を保存
	this->setting_change = 2;
}

// LEDの色変更
void Neopixel::setting_color_type() {
	if (this->_setting.color_type < 5) {
		this->_setting.color_type++;
	} else {
		this->_setting.color_type = 0;
	}
	this->setting_save(); // 設定を保存
	this->setting_change = 3;
}

// LEDの光らせ方変更
void Neopixel::setting_shine_type() {
	if (this->_setting.shine_type < 3) {
		this->_setting.shine_type++;
	} else {
		this->_setting.shine_type = 0;
	}
	this->hide_all(); // 一旦すべて消す
	this->setting_save(); // 設定を保存
	this->setting_change = 4;
}


// ON/OFFを設定する
void Neopixel::set_status(int stat) {
	// 設定が変わらなければ何もしない
	if (this->_setting.status == stat) return;
	if (stat == 0) this->_hide_flag = 1; // 消灯フラグ ON
	this->_setting.status = stat;
	this->setting_save(); // 設定を保存
	this->setting_change = 1;
}

// 明るさを設定する
void Neopixel::set_bright(int bright) {
	if (bright < 0) {
		this->_setting.bright = 0;
	} else if (bright > NEO_BRIGHT_MAX) {
		this->_setting.bright = NEO_BRIGHT_MAX;
	} else {
		this->_setting.bright = bright;
	}
	this->setting_save(); // 設定を保存
	this->setting_change = 2;
}

// LEDの色を設定する
void Neopixel::set_color_type(int color_type) {
	if (color_type > 5) {
		this->_setting.color_type = 5;
	} else if (color_type < 0 ){
		this->_setting.color_type = 0;
	} else {
		this->_setting.color_type = color_type;
	}
	this->setting_save(); // 設定を保存
	this->setting_change = 3;
}

// 光り方を設定する
void Neopixel::set_shine_type(int shine_type) {
	if (shine_type > 3) {
		this->_setting.shine_type = 3;
	} else if (shine_type < 0) {
		this->_setting.shine_type = 0;
	} else {
		this->_setting.shine_type = shine_type;
	}
	this->hide_all(); // 一旦すべて消す
	this->setting_save(); // 設定を保存
	this->setting_change = 4;
}


// 選択キークリア
void Neopixel::select_key_cler() {
	int i;
	for (i=0; i<NEO_SELECT_KEY_MAX; i++) {
		this->select_key[i].layer_id = -1;
		this->select_key[i].key_num = 0;
	}
}


// 選択キーを追加
void Neopixel::select_key_add(int8_t layer_id, uint8_t key_num) {
	int i;
	for (i=0; i<NEO_SELECT_KEY_MAX; i++) {
		if (this->select_key[i].layer_id >= 0) continue;
		this->select_key[i].layer_id = layer_id;
		this->select_key[i].key_num = key_num;
	}
}


// 選択キーを点灯
void Neopixel::select_key_show() {
	int i;
	for (i=0; i<NEO_SELECT_KEY_MAX; i++) {
		if (this->select_key[i].layer_id < 0 || *this->_select_layer_no != this->select_key[i].layer_id) continue;
		this->rgb_led->setPixelColor(this->led_num[this->select_key[i].key_num], this->rgb_led->Color(80, 0, 0));
	}
}


// 今設定されている色を取得
uint32_t Neopixel::get_setting_color() {
	int c, r, g, b;
	c = this->_setting.color_type;
	r = (neo_color_list[c][0] * this->_setting.bright) / NEO_BRIGHT_MAX;
	g = (neo_color_list[c][1] * this->_setting.bright) / NEO_BRIGHT_MAX;
	b = (neo_color_list[c][2] * this->_setting.bright) / NEO_BRIGHT_MAX;
	return this->rgb_led->Color(r, g, b);
};


// 0 - 255 に該当する色を取得
uint32_t Neopixel::get_lotate_color(uint8_t i) {
	if(i < 85) {
		return this->rgb_led->Color(
			(i * 3 * this->_setting.bright) / NEO_BRIGHT_MAX,
			((255 - (i * 3)) * this->_setting.bright) / NEO_BRIGHT_MAX,
			0);
	} else if(i < 170) {
		i -= 85;
		return this->rgb_led->Color(
			((255 - (i * 3)) * this->_setting.bright) / NEO_BRIGHT_MAX,
			0,
			(i * 3 * this->_setting.bright) / NEO_BRIGHT_MAX);
	} else {
		i -= 170;
		return this->rgb_led->Color(
			0,
			(i * 3 * this->_setting.bright) / NEO_BRIGHT_MAX,
			((255 - (i * 3)) * this->_setting.bright) / NEO_BRIGHT_MAX);
	}
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
    for (i=0; i<this->_key_matrix_len; i++) {
        this->led_buf[i] = 0;
    }
	// LEDにデータを送る
    this->rgb_led->show();
};


// NeoPixelに設定したデータのCRC32を取得
int Neopixel::get_data_crc32() {
	uint8_t *p;
	p = this->rgb_led->getPixels();
	return azcrc32(p, this->_led_length * 3);
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
	if (this->_hide_flag) {  // 消灯フラグが立っていれば消灯
		this->hide_all();
		this->_hide_flag = 0;
	}
	// LED のステータスOFFならば何もしない
	if (this->_setting.status == 0) return;
	int start_crc = this->get_data_crc32();
	if (this->_setting.shine_type == 0) {
		this->rgb_led_loop_type_0();
	} else if (this->_setting.shine_type == 1) {
		this->rgb_led_loop_type_1();
	} else if (this->_setting.shine_type == 2) {
		this->rgb_led_loop_type_2();
	} else if (this->_setting.shine_type == 3) {
		this->rgb_led_loop_type_3();
	} else if (this->_setting.shine_type == 4) {
		this->rgb_led_loop_type_4();
	}
	// 選択キーを点灯
	this->select_key_show();
	int end_crc = this->get_data_crc32();
	// LEDにデータを送る
    if (start_crc != end_crc) this->rgb_led->show();
}


// ずっと光ってる
void Neopixel::rgb_led_loop_type_0() {
	int i;
	uint32_t c = this->get_setting_color();
    // LEDを点灯
    for (i=0; i<this->_led_length; i++) {
        this->rgb_led->setPixelColor(i, c);
    }
}


// 押すと光る
/*
void Neopixel::rgb_led_loop_type_1() {
	int i;
	uint32_t n = this->rgb_led->Color(0, 0, 0);
	uint32_t c = this->get_setting_color();
    for (i=0; i<this->_led_length; i++) {
    	if (this->led_buf[i]) {
            this->rgb_led->setPixelColor(i, c);
    	} else {
            this->rgb_led->setPixelColor(i, n);
    	}
    }
}
*/
void Neopixel::rgb_led_loop_type_1() {
	int i;
	uint32_t n = this->rgb_led->Color(0, 0, 0);
	uint32_t c = this->get_setting_color();
    for (i=0; i<this->_key_matrix_len; i++) {
        if (this->key_matrix[i] < 0) continue;
    	if (this->led_buf[i]) {
            this->rgb_led->setPixelColor(this->led_num[this->key_matrix[i]], c);
    	} else {
            this->rgb_led->setPixelColor(this->led_num[this->key_matrix[i]], n);
    	}
    }
}


// ボタンを押した所から周りに広がっていく
void Neopixel::rgb_led_loop_type_2() {
    int i;
    int csize = this->_col_size;
    int rsize = this->_row_size;
    int rmax = this->_led_length - this->_col_size;
    int cmax = csize - 1;
	uint32_t n = this->rgb_led->Color(0, 0, 0);
	uint32_t c = this->get_setting_color();
    int8_t read_buf[this->_key_matrix_len];
    // led_bufの値をコピーしておいて作業はread_bufのデータをもとにled_bufを書き換える
    for (i=0; i<this->_key_matrix_len; i++) {
        read_buf[i] = this->led_buf[i];
    }
    for (i=0; i<this->_key_matrix_len; i++) {
            if (read_buf[i] == 1) {
                // 上下左右に作る
                if (i >= csize) this->led_buf[i - csize] = 2; // 上
                if (i < rmax) this->led_buf[i + csize] = 5; // 下
                if ((i % csize) > 0) this->led_buf[i - 1] = 3; // 左
                if ((i % csize) < cmax) this->led_buf[i + 1] = 4; // 右
                this->led_buf[i] = 10;
            } else if (read_buf[i] == 2) {
                // 上、右に作る
                if (i >= csize) this->led_buf[i - csize] = 2; // 上
                if ((i % csize) < cmax) this->led_buf[i + 1] = 8; // 右
                this->led_buf[i] = 10;
            } else if (read_buf[i] == 3) {
                // 左、上に作る
                if ((i % csize) > 0) this->led_buf[i - 1] = 3; // 左
                if (i >= csize) this->led_buf[i - csize] = 6; // 上
                this->led_buf[i] = 10;
            } else if (read_buf[i] == 4) {
                // 右、下に作る
                if ((i % csize) < cmax) this->led_buf[i + 1] = 4; // 右
                if (i < rmax) this->led_buf[i + csize] = 9; // 下
                this->led_buf[i] = 10;
            } else if (read_buf[i] == 5) {
                // 下、左に作る
                if (i < rmax) this->led_buf[i + csize] = 5; // 下
                if ((i % csize) > 0) this->led_buf[i - 1] = 7; // 左
                this->led_buf[i] = 10;
            } else if (read_buf[i] == 6) {
                // 上に作る
                if (i >= csize) this->led_buf[i - csize] = 6; // 上
                this->led_buf[i] = 10;
            } else if (read_buf[i] == 7) {
                // 左に作る
                if ((i % csize) > 0) this->led_buf[i - 1] = 7; // 左
                this->led_buf[i] = 10;
            } else if (read_buf[i] == 8) {
                // 右に作る
                if ((i % csize) < cmax) this->led_buf[i + 1] = 8; // 右
                this->led_buf[i] = 10;
            } else if (read_buf[i] == 9) {
                // 下に作る
                if (i < rmax) this->led_buf[i + csize] = 9; // 下
                this->led_buf[i] = 10;
            } else if (read_buf[i] > 9) {
                // それ以外は消す
                this->led_buf[i] = 0;
            }
    }
    // LEDを点灯
    for (i=0; i<this->_led_length; i++) {
        this->rgb_led->setPixelColor(i, n);
    }
    for (i=0; i<this->_key_matrix_len; i++) {
        if (this->led_buf[i] && this->key_matrix[i] >= 0) {
            this->rgb_led->setPixelColor(this->led_num[this->key_matrix[i]], c);
        }
    }
}


// 
void Neopixel::rgb_led_loop_type_3() {
    int i;
    int csize = this->_col_size;
    int rsize = this->_row_size;
    int rmax = this->_led_length - this->_col_size;
    int cmax = csize - 1;
    // LEDを点灯
    for (i=0; i<this->_led_length; i++) {
        if (this->key_matrix[i] >= 0) {
            this->rgb_led->setPixelColor(this->led_num[this->key_matrix[i]], this->get_lotate_color((i + this->lotate_index) % 255));
        }
    }
	// キー割り当てのないLEDに色をつける
    for (i=0; i<this->_led_length; i++) {
		if (!this->_back_flag[i]) continue;
		this->rgb_led->setPixelColor(i, this->get_lotate_color((i + this->lotate_index) % 255));
	}
	// ローテーションインデックス更新
	if (this->lotate_index >= 254) {
		this->lotate_index = 0;
	} else {
		this->lotate_index++;
	}
}
// 
void Neopixel::rgb_led_loop_type_4() {
	int i;
    // LEDを点灯
    for (i=0; i<this->_led_length; i++) {
        this->rgb_led->setPixelColor(i, this->rgb_led->Color(0, 0, 0));
    }
}
