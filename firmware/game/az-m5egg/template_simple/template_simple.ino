
#include "src/lib/common.h"

LV_IMG_DECLARE(ankinow);
LV_IMG_DECLARE(logo);

#include "src/sound/daken_02.c"


// 共通クラス
Common common_cls = Common();

lv_obj_t * icon;
int move_index;

void setup() {
  // 共通クラス初期化
  common_cls.common_start();
  delay(1000);

    move_index = 0;
  

}

void loop() {
    int i, c;
    c = sound_cls._get_chnum(); // 再生するチャンネルを取得
    sound_cls.wav_PROGMEM(daken_02_wav_bin, sizeof(daken_02_wav_bin), c);
    icon = lv_img_create(lv_scr_act(), NULL);
    if (move_index == 0) {
        lv_img_set_src(icon, &ankinow); // M5egg logo
    } else if (move_index == 1) {
        lv_img_set_src(icon, &logo); // M5egg logo
    }
    lv_obj_align(icon, NULL, LV_ALIGN_CENTER, 0, 0);
    for (i=0; i<200; i+=5) {
        lv_obj_align(icon, NULL, LV_ALIGN_CENTER, i, 0);
        common_cls.key_read();
        if (input_key[0] == 1 && input_key_last[0] == 0) break;
        delay(20);
    }
    for (i=200; i>0; i-=5) {
        lv_obj_align(icon, NULL, LV_ALIGN_CENTER, i, 0);
        common_cls.key_read();
        if (input_key[0] == 1 && input_key_last[0] == 0) break;
        delay(20);
    }
    lv_obj_del(icon);
    move_index++;
    if (move_index >= 2) move_index = 0;

}
