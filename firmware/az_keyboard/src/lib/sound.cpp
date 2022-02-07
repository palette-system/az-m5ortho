#include "sound.h"

#include "../sound/cursor_01.c"
#include "../sound/cursor_02.c"
#include "../sound/cursor_03.c"
#include "../sound/daken_01.c"
#include "../sound/daken_02.c"
#include "../sound/daken_03.c"
#include "../sound/wav_toh.c"


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

    this->_volgain = (float)SOUND_VOLUME_DEFAULT / 255;
}

// サウンド初期化
void Sound::begin() {
    int i;

    // スピーカーの初期化
    // M5.Axp.SetSpkEnable(false); 

    // サウンド初期化
    this->_out = new AudioOutputI2S(0, 0);
    this->_out->SetPinout(SOUND_BCLK_PIN, SOUND_LRCK_PIN, SOUND_SADTA_PIN);
    this->_out->SetOutputModeMono(true);
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
    // 全部埋まっていたら古いチャンネルから停止して返す
    i = this->_last_ch + 1;
    if (i >= SOUND_CH_MAX) i = 0;
    this->stop(i); // 停止
    this->_last_ch = i;
    return i;
}

// メモリ上のwavを再生
void Sound::wav_PROGMEM(const void *sound_data, uint32_t sound_len) {
    if (!this->_setting.sound_enable) return;
    int c;
    c = this->_get_chnum(); // 再生するチャンネルを取得
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

// キーDOWN音再生
void Sound::daken_down(int key_num) {
    if (!this->_setting.sound_enable) return;
    if (this->_setting.type_default == 1) {
        this->wav_PROGMEM(daken_01_wav_bin, sizeof(daken_01_wav_bin));
    } else if (this->_setting.type_default == 2) {
        this->wav_PROGMEM(daken_02_wav_bin, sizeof(daken_02_wav_bin));
    } else if (this->_setting.type_default == 3) {
        this->wav_PROGMEM(daken_03_wav_bin, sizeof(daken_03_wav_bin));
    } else if (this->_setting.type_default == 4) {
        this->wav_PROGMEM(cursor_01_wav_bin, sizeof(cursor_01_wav_bin));
    } else if (this->_setting.type_default == 5) {
        this->wav_PROGMEM(cursor_02_wav_bin, sizeof(cursor_02_wav_bin));
    } else if (this->_setting.type_default == 6) {
        this->wav_PROGMEM(cursor_03_wav_bin, sizeof(cursor_03_wav_bin));
    } else if (this->_setting.type_default == 7) {
        this->wav_PROGMEM(wav_toh, sizeof(wav_toh));
    }
}

// 定期処理
int Sound::loop_exec() {
    if (!this->_setting.sound_enable) return 0;
    int i;
    int r = 0;
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