#ifndef display_h
#define display_h


#include "Arduino.h"
#include <lvgl.h>


// LVGL用
bool my_touchpad_read(lv_indev_drv_t * indev_driver, lv_indev_data_t * data);
void my_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p);
void lv_setup();
extern uint8_t _disp_rotate; // 画面の向き


// クラスの定義
class Display
{
	public:
		short _width; // 液晶のサイズ幅
		short _height; // 液晶のサイズ高さ
		Display();   // コンストラクタ
		void begin(uint8_t disp_rotate); // 初期化
		void loop_exec(); // 定期処理
};



#endif

