#include "sound.h"


// コンストラクタ
Sound::Sound() {
    int i;

    // 再生フラグ
    for (i=0; i<SOUND_CH_MAX; i++) {
        this->_play_flag[i] = false;
    }

	// デフォルト値
	this->_setting.sound_enable = 0;
	this->_setting.volume = SOUND_VOLUME_DEFAULT;
	this->_setting.type_default = 0;

    // 最後に再生したチャンネル
    this->_last_ch = 0;

    // 再生中に押されたキー
    this->_stack_keynum = -1;

    // フェードアウト用インデックス
    this->_feed_out_index = 0;
    this->_feed_out_ch = 0;

    this->_volgain = (float)SOUND_VOLUME_DEFAULT / 255;
}

// サウンド初期化
void Sound::begin() {
    int i;

    // サウンド初期化
    this->_out = new AudioOutputI2S(0, 0);
    this->_out->SetPinout(SOUND_BCLK_PIN, SOUND_LRCK_PIN, SOUND_SADTA_PIN);
    this->_out->SetOutputModeMono(true);
    this->_out->stop();
    this->_mixer = new AudioOutputMixer(32, this->_out);

    // チャンネル初期化
    for (i=0; i<SOUND_CH_MAX; i++) {
        this->_stub[i] = this->_mixer->NewInput(); // 入力取得
        this->_stub[i]->SetGain(this->_volgain); // 音量
    }

    // 設定ファイル読み込み
    this->setting_load();

}

// 設定ファイルの読み込み
void Sound::setting_load() {
	// デフォルト値
	this->_setting.sound_enable = 0;
	this->_setting.volume = SOUND_VOLUME_DEFAULT;
	this->_setting.type_default = 0;
	// 設定ファイルがあれば読み込む
	if (SPIFFS.exists(SOUND_SETTING_PATH)) {
		File fp = SPIFFS.open(SOUND_SETTING_PATH, "r");
		int i = fp.read((uint8_t *)&this->_setting, sizeof(sound_setting));
		fp.close();
	}
    this->_volgain = (float)this->_setting.volume / 255;
    if (this->_setting.sound_enable) {
        M5.Axp.SetSpkEnable(true);
    } else {
        M5.Axp.SetSpkEnable(false);
    }
}

// 設定ファイルに保存
void Sound::setting_save() {
	File fp = SPIFFS.open(SOUND_SETTING_PATH, "w");
	int i = fp.write((uint8_t *)&this->_setting, sizeof(sound_setting));
	fp.close();
}

// 空いているチャンネルを取得
int Sound::_get_chnum() {
    int i;
    // 空いているチャンネルを返す
    for (i=0; i<SOUND_CH_MAX; i++) {
        if (!this->_play_flag[i]) {
            this->_last_ch = i;
            return i;
        }
    }
    return -1;
}

// SPIFFS 上のファイルを再生
void Sound::wav_SPIFFS(int c) {
    if (!this->_setting.sound_enable) return;
    // this->_spifile = new AudioFileSourceSPIFFS(SOUND_DAKEN_WAV_PATH);
    // this->_stub[c]->SetGain(this->_volgain);
    // this->_wav[c] = new AudioGeneratorWAV();
    // this->_wav[c]->begin(this->_spifile, this->_stub[c]);
    // this->_play_flag[c] = 1;
}

// メモリ上のwavを再生
void Sound::wav_PROGMEM(const void *sound_data, uint32_t sound_len, int c) {
    if (!this->_setting.sound_enable) return;
    this->_file[c] = new AudioFileSourcePROGMEM( sound_data, sound_len );
    this->_stub[c]->SetGain(this->_volgain);
    this->_wav[c] = new AudioGeneratorWAV();
    this->_wav[c]->begin(this->_file[c], this->_stub[c]);
    this->_play_flag[c] = 1;
}

// サウンド停止
void Sound::stop(int ch) {
    if (this->_play_flag[ch]) {
        this->_wav[ch]->stop();
        this->_stub[ch]->stop();
        this->_out->stop();
        this->_play_flag[ch] = 0;
    }
}

// ON/OFF設定
void Sound::set_enable(uint8_t en) {
    this->_setting.sound_enable = en;
    if (en) {
        M5.Axp.SetSpkEnable(true);
    } else {
        M5.Axp.SetSpkEnable(false);
    }
    // 設定ファイルに保存
    this->setting_save();
}

// 音量設定
void Sound::set_volume(uint8_t vl) {
    int i;
    this->_setting.volume = vl;
    this->_volgain = (float)this->_setting.volume / 255;
    for (i=0; i<SOUND_CH_MAX; i++) {
        this->_stub[i]->SetGain(this->_volgain); // 音量
    }
    // 設定ファイルに保存
    this->setting_save();
}

// 音のタイプ設定
void Sound::set_type_default(uint16_t tp) {
    this->_setting.type_default = tp;
    // 設定ファイルに保存
    this->setting_save();
}


// 定期処理
int Sound::loop_exec() {
    if (!this->_setting.sound_enable) return 0;
    int i;
    int r = 0;
    // フェードアウト
    if (this->_feed_out_index == 1) {
        this->_stub[this->_feed_out_ch]->SetGain(this->_volgain * 0.8);
        this->_feed_out_index++;
    } else if (this->_feed_out_index == 2) {
        this->_stub[this->_feed_out_ch]->SetGain(this->_volgain * 0.6);
        this->_feed_out_index++;
    } else if (this->_feed_out_index == 3) {
        this->_stub[this->_feed_out_ch]->SetGain(this->_volgain * 0.3);
        this->_feed_out_index++;
    } else if (this->_feed_out_index == 4) {
        this->stop(this->_feed_out_ch);
        // this->daken_down(this->_stack_keynum);
        this->_stack_keynum = -1;
        this->_feed_out_index = 0;
    }
    // 各チャンネル再生
    for (i=0; i<SOUND_CH_MAX; i++) {
        if (this->_play_flag[i] && this->_wav[i]->isRunning()) {
            if (!this->_wav[i]->loop()) {
                this->stop(i);
            }
            r++;
        }
    }
    
    return r;
}