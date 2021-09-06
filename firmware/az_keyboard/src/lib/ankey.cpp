#include "Arduino.h"
#include "ankey.h"


// コンストラクタ
Ankey::Ankey() {
}


// 暗記キー初期化処理
void Ankey::begin(AzKeyboard  *azkb) {
	int i;
    this->_azkb = azkb;
	this->ankey_flag = 0;
	this->_data_index = 0;
	this->_loop_index = 0;
	this->ankey_count = 0;
	this->ankey_key_num = 0;
	for (i=0; i<ANKEY_DATA_MAX_LENGTH; i++) {
		this->_andata[i].type = 0;
		this->_andata[i].key_num = 0;
	}
}

// キーが押された
void Ankey::key_down(int key_num) {
	// 暗記中で無ければ何もしない
	if (this->ankey_flag != 1) return;
	// 暗記ボタンであれば記憶しない
	if (this->ankey_layer_id == select_layer_no && this->ankey_key_num == key_num) return;
	// 入力データを保持
	int i = this->_data_index;
	this->_andata[i].type = 1; // key_down
	this->_andata[i].key_num = key_num;
	this->_data_index++;
	this->_loop_index = 0;
	// 覚えれる上限に行ってしまったらそこで暗記終了
	if (this->_data_index >= ANKEY_DATA_MAX_LENGTH) this->input_end();
}


// キーが離された
void Ankey::key_up(int key_num) {
	// 暗記中で無ければ何もしない
	if (this->ankey_flag != 1) return;
	// 暗記ボタンであれば記憶しない
	if (this->ankey_layer_id == select_layer_no && this->ankey_key_num == key_num) return;
	// 入力データを保持
	int i = this->_data_index;
	this->_andata[i].type = 2; // key_up
	this->_andata[i].key_num = key_num;
	this->_data_index++;
	this->_loop_index = 0;
	// 覚えれる上限に行ってしまったらそこで暗記終了
	if (this->_data_index >= ANKEY_DATA_MAX_LENGTH) this->input_end();
}


// 暗記開始
void Ankey::input_start() {
	int i;
	// 暗記開始処理
	this->ankey_flag = 1; // 暗記中
	this->_data_index = 0;
	// バッファに入ってる暗記データをクリア
	for (i=0; i<ANKEY_DATA_MAX_LENGTH; i++) {
		this->_andata[i].type = 0;
		this->_andata[i].key_num = 0;
	}
	// ヒートマップ表示中であればヒートマップ上の推してるボタンも全て消す
    if (common_cls.on_tft_unit()) {
    	disp->view_daken_key_reset();
    }
	// 開始時入力状態をリセット
	this->_azkb->press_data_reset();
	common_cls.input_key[this->ankey_key_num] = 1;
	// 暗記ボタンを光らせる
	rgb_led_cls.select_key_cler();
	rgb_led_cls.select_key_add(this->ankey_layer_id, this->ankey_key_num);
	// 暗記中画面表示
    if (common_cls.on_tft_unit()) {
        disp->view_type = DISP_TYPE_ANKYNOW;
    }
}

// 暗記終了
void Ankey::input_end() {
	int i, data_size;
	data_size = this->_data_index * sizeof(ankey_data);
	// 暗記終了
	this->ankey_flag = 0; // 処理なし
	this->_data_index = 0;
	// ヒートマップ表示中であればヒートマップ上の推してるボタンも全て消す
    if (common_cls.on_tft_unit()) {
    	disp->view_daken_key_reset();
    }
	// 入力状態をリセット
	this->_azkb->press_data_reset();
	// 記憶データをファイルに出力
	JsonObject key_set = common_cls.get_key_setting(this->ankey_layer_id, this->ankey_key_num); // 暗記キーの設定情報取得
	String fpath = "/" + key_set["press"]["ankey_file"].as<String>();
	File fp = SPIFFS.open(fpath, "w");
	if (!fp) {
		// ファイル書込み失敗
		return;
	}
	fp.write((uint8_t *)&this->_andata, data_size);
	fp.close();
	// 暗記ボタンの光を消す
	rgb_led_cls.select_key_cler();
	// 全ての光を消す
	rgb_led_cls.hide_all();
	// 待ち受け画像表示
    if (common_cls.on_tft_unit()) {
        disp->view_type = disp->_back_view_type;
    }
}


// 暗記したデータのキー入力開始
void Ankey::output_start() {
	// 記憶データをファイルから読み込む
	JsonObject key_set = common_cls.get_key_setting(this->ankey_layer_id, this->ankey_key_num); // 暗記キーの設定情報取得
	String fpath = "/" + key_set["press"]["ankey_file"].as<String>();
	File fp = SPIFFS.open(fpath, "r");
	if (!fp) {
		// ファイル読み込み失敗
		return;
	}
	int i = 0;
	if (fp.available()) {
		i = fp.read((uint8_t *)&this->_andata, ANKEY_DATA_MAX_LENGTH * sizeof(ankey_data));
	}
	this->_andata[i / 2].type = 0;
	fp.close();
	// 暗記データが無ければ何もしない
	if (this->_andata[0].type == 0) {
		return;
	}
	// 入力処理開始
	this->ankey_flag = 2; // キー入力中
	this->_data_index = 0;
	// ヒートマップ表示中であればヒートマップ上の推してるボタンも全て消す
    if (common_cls.on_tft_unit()) {
    	disp->view_daken_key_reset();
    }
	// 入力状態をリセット
	this->_azkb->press_data_reset();
}


// 暗記したデータのキー入力終了
void Ankey::output_end() {
	this->ankey_flag = 0; // 動作無し
	this->_data_index = 0;
	this->_loop_index = 0;
	// ヒートマップ表示中であればヒートマップ上の推してるボタンも全て消す
    if (common_cls.on_tft_unit()) {
    	disp->view_daken_key_reset();
    }
	// 入力状態をリセット
	this->_azkb->press_data_reset();
	// 全ての光を消す
	rgb_led_cls.hide_all();
}

// 暗記ボタン押された
void Ankey::ankey_down(short layer_id, int key_num) {
	// 暗記入力中は他の暗記キー全て無視
	if (this->ankey_flag == 2) return;
	// 暗記中
	if (this->ankey_flag == 1) {
		return;
	}
	// 押されてからの経過時間を図るようにindexをリセット
	if (this->ankey_count == 0) this->_loop_index = 0;
	// 押している暗記ボタンの数をカウントアップ
	this->ankey_count++;
	// 最後に押された暗記キーの情報を保持
	this->ankey_layer_id = layer_id;
	this->ankey_key_num = key_num;
}

// 暗記ボタン離された
void Ankey::ankey_up(short layer_id, int key_num) {
	// 暗記中で暗記開始したボタンなら暗記終了。それ以外の暗記ボタンは無視
	if (this->ankey_flag == 1 && this->ankey_layer_id == layer_id && this->ankey_key_num == key_num) {
		this->input_end();
		this->ankey_count = 0;
		return;
	}
	// 暗記ボタン押されていなければ何もしない
	if (this->ankey_count == 0) return;
	this->ankey_count--;
	if (this->ankey_count == 0) {
		// 暗記処理なしで、短押しだった場合キー入力開始
		if (this->ankey_flag == 0 && this->_loop_index < 100) {
			this->output_start();
		}
		
	}
}

// 定期処理
void Ankey::loop_exec() {
	int i;
    if (common_cls.on_tft_unit()) {
/*        disp->view_int(0, 0, this->ankey_count);
        disp->view_int(0, 26, this->_loop_index);
        disp->view_int(0, 52, this->_data_index);
        disp->view_int(30, 0, this->ankey_flag);
    	if (this->ankey_flag == 2) {
    		disp->view_int(60, 0, this->_andata[this->_data_index].wait);
    	}
*/    }
	// 暗記ボタンが長押しされたら暗記開始
	if (this->ankey_flag == 0 && this->ankey_count > 0 && this->_loop_index >= 100) {
		this->ankey_count = 0;
		this->input_start();
	}
	// 暗記データキー入力
	if (this->ankey_flag == 2) {
		i = this->_data_index;
		// タイプがなしになったら終了
		if (this->_andata[i].type == 0) {
			this->output_end();
			return;
		}
		// if (this->_andata[i].wait <= this->_loop_index) {
		if (0 <= this->_loop_index) {
			// 入力処理
			if (this->_andata[i].type == 1) {
				// キーダウン
				this->_azkb->key_down_action(this->_andata[i].key_num);
			} else if (this->_andata[i].type == 2) {
				// キーアップ
				this->_azkb->key_up_action(this->_andata[i].key_num);
			}
			this->_data_index++;
			this->_loop_index = 0;
		}
	}
	this->_loop_index++;
}

