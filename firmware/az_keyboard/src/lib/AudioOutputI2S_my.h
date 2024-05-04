
#ifndef AudioOutputI2S_my_h
#define AudioOutputI2S_my_h

#include "AudioOutputI2S.h"

// サウンド出力クラスのサンプルレートをいじるために継承して自前の物を使う
class AudioOutputI2S_my : public AudioOutputI2S
{
  public:
    AudioOutputI2S_my(int port=0, int output_mode=EXTERNAL_I2S, int dma_buf_count = 8, int use_apll=APLL_DISABLE);
    virtual bool SetRate(int hz);
};

#endif
