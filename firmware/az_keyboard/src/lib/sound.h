#ifndef sound_h
#define sound_h

#include <M5Core2.h>
#include <driver/i2s.h>

#include "FS.h"
#include "SPIFFS.h"

#include "AudioFileSourcePROGMEM.h"
#include "AudioGeneratorWAV.h"
#include "AudioOutputI2S.h"
#include "AudioOutputMixer.h"


// スピーカー用DAC I2S ピン
#define SOUND_BCLK_PIN  12
#define SOUND_LRCK_PIN   0
#define SOUND_SADTA_PIN  2

// 再生チャンネル数
#define SOUND_CH_MAX  1

// 音量デフォルト
#define SOUND_VOLUME_DEFAULT  128

// 設定ファイルのパス
#define SOUND_SETTING_PATH  "/sound.dat"


// サウンド用設定データ
struct sound_setting {
	int8_t  sound_enable;   // サウンドステータス (0=音無し / 1=音あり)
	uint8_t  volume;  // 音量 (0-255)
	uint16_t  type_default;  // デフォルトの音(0=無し / )
};


class Sound
{
    public:
        AudioGeneratorWAV *_wav[SOUND_CH_MAX];
        AudioFileSourcePROGMEM *_file[SOUND_CH_MAX];
        AudioOutputI2S *_out;
        AudioOutputMixer *_mixer;
        AudioOutputMixerStub *_stub[SOUND_CH_MAX];
		sound_setting _setting; // 設定
        float _volgain; // gainに設定する数字
        bool _play_flag[SOUND_CH_MAX]; // 再生中かどうかのフラグ
        uint8_t _last_ch; // 最後に再生したチャンネル
        Sound();   // コンストラクタ
        void begin(); // サウンド初期化
        void setting_load(); // 設定ファイルの読み込み
        void setting_save(); // 設定ファイルに保存
        int _get_chnum(); // 空いているチャンネルを取得
        void wav_PROGMEM(const void *sound_data, uint32_t sound_len); // メモリ上のwavを再生
        void stop(int ch); // サウンド停止
        void set_enable(uint8_t en); // ON/OFF設定(0=OFF / 1=ON)
        void set_volume(uint8_t vl); // 音量設定(0-255)
        void set_type_default(uint16_t tp); // 音のタイプ設定
        void daken_down(int key_num); // キーDOWN音再生
        int loop_exec(); // 定期処理
};

#endif
