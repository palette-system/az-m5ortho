

#include "display.h"

#include "common.h"

uint8_t _disp_rotate; // 画面の向き


//=====================================================================
/*Read the touchpad*/
bool my_touchpad_read(lv_indev_drv_t * indev_driver, lv_indev_data_t * data){
	// if (mouse_pad_status >= 0) {
	//	data->state = LV_INDEV_STATE_REL;
	//	return false;
	// }
	TouchPoint_t pos = M5.Touch.getPressPoint();
  bool touched = ( pos.x == -1 ) ? false : true;
  if(!touched) {
    data->state = LV_INDEV_STATE_REL;
  } else {
    data->state = LV_INDEV_STATE_PR; 
    /*Set the coordinates*/
	if (_disp_rotate == 0) {
      data->point.x = 240 - pos.y; // 左が上
      data->point.y = pos.x;
	} else if (_disp_rotate == 1) { // 上が上
      data->point.x = pos.x;
      data->point.y = pos.y;
	} else if (_disp_rotate == 2) { // 右が上
      data->point.x = pos.y;
      data->point.y = 320 - pos.x;
	} else if (_disp_rotate == 3) { // 下が上
      data->point.x = 320 - pos.x;
      data->point.y = 240 - pos.y;
	}
  }
  return false; 
}

//=====================================================================
/* Display flushing */
void my_disp_flush(lv_disp_drv_t *disp_obj, const lv_area_t *area, lv_color_t *color_p){
  uint32_t w = (area->x2 - area->x1 + 1);
  uint32_t h = (area->y2 - area->y1 + 1);
  lvtft.startWrite();
  lvtft.setAddrWindow(area->x1, area->y1, w, h);
  lvtft.pushColors(&color_p->full, w * h, true);
  lvtft.endWrite();
  lv_disp_flush_ready(disp_obj);
}


void lv_setup() {
  lvtft.begin();
  lvtft.setRotation(_disp_rotate);
  //-------------------------------------------------------------------
  lv_disp_buf_init(&disp_buf, lvbuf, NULL, LV_HOR_RES_MAX * 10);
  lv_init();
  
  //-------------------------------------------------------------------
  /*Initialize the display*/
  lv_disp_drv_t disp_drv;
  lv_disp_drv_init(&disp_drv);
  if (_disp_rotate == 0) { // 左が上
    disp_drv.hor_res  = 240;
    disp_drv.ver_res  = 320;
  } else if (_disp_rotate == 1) { // 上が上
    disp_drv.hor_res  = 320;
    disp_drv.ver_res  = 240;
  } else if (_disp_rotate == 2) { // 右が上
    disp_drv.hor_res  = 240;
    disp_drv.ver_res  = 320;
  } else if (_disp_rotate == 3) { // 下が上
    disp_drv.hor_res  = 320;
    disp_drv.ver_res  = 240;
  }
  disp_drv.flush_cb = my_disp_flush;
  disp_drv.buffer   = &disp_buf;
  lv_disp_drv_register(&disp_drv);
  
  //-------------------------------------------------------------------
  /*Initialize the (dummy) input device driver*/
  lv_indev_drv_t indev_drv;
  lv_indev_drv_init(&indev_drv);
  indev_drv.type = LV_INDEV_TYPE_POINTER;
  indev_drv.read_cb = my_touchpad_read;
  lv_indev_drv_register(&indev_drv);


}


// コンストラクタ
Display::Display() {
}

// 液晶制御初期化
void Display::begin(uint8_t disp_rotate) {
	_disp_rotate = 0;
	if (disp_rotate >= 0 && disp_rotate <= 3) {
	  _disp_rotate = disp_rotate;
	}
	lv_setup();
}


// 定期処理
void Display::loop_exec() {
	// LVGL表示処理
	lv_task_handler();
	vTaskDelay(10);
}


