
#include "src/lib/common.h"

LV_IMG_DECLARE(ankinow);
LV_IMG_DECLARE(logo);
LV_IMG_DECLARE(hiyo_1);
LV_IMG_DECLARE(hiyo_2);
LV_IMG_DECLARE(emugotch_logo_320);


// static const lv_img_dsc_t * anim_imgs[2] = {
//     &hiyo_1,
//     &hiyo_2
// };

#include "src/sound/daken_02.c"


// 共通クラス
Common common_cls = Common();

lv_obj_t * bg_img;

lv_obj_t * icon;
int move_index;

void setup() {
  // 共通クラス初期化
  common_cls.common_start();
  delay(1000);

    move_index = 0;

    // 背景表示
    bg_img = lv_img_create(lv_scr_act(), NULL);
    lv_img_set_src(bg_img, &emugotch_logo_320); // 背景
    lv_obj_align(bg_img, NULL, LV_ALIGN_IN_TOP_LEFT, 0, 0);

    // icon = lv_img_create(lv_scr_act(), NULL);
    // lv_img_set_src(icon, &hiyo_1); // M5egg logo
    // lv_obj_align(icon, NULL, LV_ALIGN_TOP_LEFT, 0, 0);

    // lv_obj_t * animimg0 = lv_animimg_create(lv_scr_act());
    // lv_obj_center(animimg0);
    // lv_animimg_set_src(animimg0, (lv_img_dsc_t **) anim_imgs, 2);
    // lv_animimg_set_duration(animimg0, 1000);
    // lv_animimg_set_repeat_count(animimg0, LV_ANIM_REPEAT_INFINITE);
    // lv_animimg_start(animimg0);
}

void loop() {
    int i, c;
    c = sound_cls._get_chnum(); // 再生するチャンネルを取得
    // sound_cls.wav_PROGMEM(daken_02_wav_bin, sizeof(daken_02_wav_bin), c);
    icon = lv_img_create(lv_scr_act(), NULL);
    if (move_index == 0) {
        lv_img_set_src(icon, &hiyo_1); // M5egg logo
    } else if (move_index == 1) {
        lv_img_set_src(icon, &hiyo_2); // M5egg logo
    }
    lv_obj_align(icon, NULL, LV_ALIGN_IN_TOP_LEFT, 200, 140);
    // common_cls.key_read();
    // if (input_key[0] == 1 && input_key_last[0] == 0) break;
    disp_stop_flag = false;
    delay(300);
    disp_stop_flag = true;
    lv_obj_del(icon);
    move_index++;
    if (move_index >= 2) move_index = 0;

}
