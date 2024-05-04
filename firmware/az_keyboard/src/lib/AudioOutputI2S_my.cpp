
#include "AudioOutputI2S_my.h"

// コンストラクタ
AudioOutputI2S_my::AudioOutputI2S_my(int port, int output_mode, int dma_buf_count, int use_apll) {
    AudioOutputI2S(port, output_mode, dma_buf_count, use_apll);
}

// サンプリングレートの設定値を2倍にする
bool AudioOutputI2S_my::SetRate(int hz) {
    return AudioOutputI2S::SetRate(hz * 2);
}
