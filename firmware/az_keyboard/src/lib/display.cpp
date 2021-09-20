#include "Arduino.h"
#include "display.h"

#include "../../az_common.h"
#include "../image/gimp_image.h"
#include "../image/tanoshii.h"
#include "../image/wificonn1.h"
#include "../image/wificonn2.h"


void view_mouse_page(); // マウスパッド画面表示
void view_setting_mousepad(lv_obj_t * obj, lv_event_t event); // マウスパッド設定画面表示

//=====================================================================
/*Read the touchpad*/
bool my_touchpad_read(lv_indev_drv_t * indev_driver, lv_indev_data_t * data){
	if (mouse_pad_status >= 0) {
		data->state = LV_INDEV_STATE_REL;
		return false;
	}
	TouchPoint_t pos = M5.Touch.getPressPoint();
  bool touched = ( pos.x == -1 ) ? false : true;
  if(!touched) {
    data->state = LV_INDEV_STATE_REL;
  } else {
    data->state = LV_INDEV_STATE_PR; 
    /*Set the coordinates*/
    data->point.x = 240 - pos.y;
    data->point.y = pos.x;
  }
  return false; 
//Return `false` because we are not buffering and no more data to read
}
//=====================================================================
/* Display flushing */
void my_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p){
  uint32_t w = (area->x2 - area->x1 + 1);
  uint32_t h = (area->y2 - area->y1 + 1);
  lvtft.startWrite();
  lvtft.setAddrWindow(area->x1, area->y1, w, h);
  lvtft.pushColors(&color_p->full, w * h, true);
  lvtft.endWrite();
  lv_disp_flush_ready(disp);
}
//=====================================================================
void close_setting(lv_obj_t * obj, lv_event_t event) {
    // イベントについて
    // https://docs.lvgl.io/master/overview/event.html
	/*
    if(event == LV_EVENT_CLICKED) { // クリック
        M5.Lcd.print(" c\n");
    }
    else if (event == LV_EVENT_PRESSED) { // 押された時
        M5.Lcd.print(" p\n");
    }
    else if (event == LV_EVENT_RELEASED) { // 離された時
        // M5.Lcd.print("LV_EVENT_RELEASED\n");
        // lv_obj_set_hidden(obj, true);
        // M5.Lcd.printf("m %D %D\n", heap_caps_get_free_size(MALLOC_CAP_32BIT), heap_caps_get_free_size(MALLOC_CAP_8BIT));
        lv_obj_clean(lv_scr_act());
        mouse_pad_status = 2;
        M5.Lcd.printf("m %D %D\n", ESP.getHeapSize(), ESP.getFreeHeap());
    }
    else if (event == LV_EVENT_LONG_PRESSED) { // 長押し
        M5.Lcd.print(" n\n");
    }
    else if(event == LV_EVENT_VALUE_CHANGED) { // 変更された時
        M5.Lcd.print("LV_EVENT_VALUE_CHANGED!\n");
    }
	*/
	if (event == LV_EVENT_CLICKED) { // クリック
		// マウスパッド画面表示
        view_mouse_page();
	}
}

void lv_setup() {
  lvtft.begin();
  lvtft.setRotation(0);
	/*
  M5.Axp.SetLcdVoltage(2800);
  M5.Axp.SetLcdVoltage(3300);
  M5.Axp.SetBusPowerMode(0);
  M5.Axp.SetCHGCurrent(AXP192::kCHG_190mA);
  M5.Axp.SetLDOEnable(3, true);
  delay(150);
  M5.Axp.SetLDOEnable(3, false);
  M5.Axp.SetLed(1);
  delay(100);
  M5.Axp.SetLed(0);
  M5.Axp.SetLDOVoltage(3, 3300);
  M5.Axp.SetLed(1);
  */
  // M5.Lcd.printf("m %D %D\n", heap_caps_get_free_size(MALLOC_CAP_32BIT), heap_caps_get_free_size(MALLOC_CAP_8BIT));
  // return;
  //-------------------------------------------------------------------
  lv_disp_buf_init(&disp_buf, lvbuf, NULL, LV_HOR_RES_MAX * 10);
  lv_init();
  startTime = millis();
  
  //-------------------------------------------------------------------
  /*Initialize the display*/
  lv_disp_drv_t disp_drv;
  lv_disp_drv_init(&disp_drv);
  disp_drv.hor_res  = 240;
  disp_drv.ver_res  = 320;
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

void view_setting_menu_fnc() {
	// 画面上のオブジェクト全て削除
	lv_obj_clean(lv_scr_act());

	//-------------------------------------------------------------------
    lv_obj_t * win = lv_win_create(lv_scr_act(), NULL);
    lv_win_set_title(win, "設定メニュー");                        /*Set the title*/
    /*Add control button to the header*/
    // lv_obj_t * close_btn = lv_win_add_btn(win, LV_SYMBOL_CLOSE);           /*Add close button and use built-in close action*/
    // lv_obj_set_event_cb(close_btn, lv_win_close_event_cb);
    // lv_win_add_btn(win, LV_SYMBOL_SETTINGS);        /*Add a setup button*/

    /*Add some dummy content*/
    // lv_obj_t * txt = lv_label_create(win, NULL);
    // lv_label_set_text(txt, "選んで下さい");
    // lv_obj_align(txt, NULL, LV_ALIGN_IN_TOP_MID, 0, 0);
    
  
  lv_obj_t * btn1 = lv_btn_create(win, NULL);
  lv_obj_set_size(btn1, 200, 70);
  lv_obj_align(btn1, NULL, LV_ALIGN_IN_TOP_MID, 0, 20);
  lv_obj_set_event_cb(btn1, view_setting_mousepad);
  lv_obj_set_style_local_value_str(btn1, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, "マウスパッド設定");
  
  lv_obj_t * btn2 = lv_btn_create(win, NULL);
  lv_obj_set_size(btn2, 200, 70);
  lv_obj_align(btn2, NULL, LV_ALIGN_IN_TOP_MID, 0, 130);
  lv_obj_set_event_cb(btn2, close_setting);
  lv_obj_set_style_local_value_str(btn2, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, "閉じる");
	
	// マウスパッド操作停止
	mouse_pad_status = -1;
}

// マウスパッド設定画面終了
void view_setting_mousepad_close(lv_obj_t * obj, lv_event_t event) {
	// クリック以外のイベントは無視
	if (event != LV_EVENT_CLICKED) return;
	// 設定メニュー表示
	view_setting_menu_fnc();
}


// マウス操作設定セレクトが変更された
void change_mousepad_type(lv_obj_t * obj, lv_event_t event) {
    if(event == LV_EVENT_VALUE_CHANGED) {
        mouse_pad_setting = lv_dropdown_get_selected(obj);
    }
}


// マウスパッド設定画面表示
void view_setting_mousepad(lv_obj_t * obj, lv_event_t event) {
	// クリック以外のイベントは無視
	if (event != LV_EVENT_CLICKED) return;

	// 画面上のオブジェクト全て削除
	lv_obj_clean(lv_scr_act());

	// window作成
    lv_obj_t * win = lv_win_create(lv_scr_act(), NULL);
    lv_win_set_title(win, "マウスパッド設定");

	// テキスト
    lv_obj_t * txt = lv_label_create(win, NULL);
    lv_label_set_text(txt, "操作方法");
    lv_obj_align(txt, NULL, LV_ALIGN_IN_TOP_MID, 0, 0);

	// セレクトメニュー表示
	lv_obj_t * ddlist = lv_dropdown_create(win, NULL);
	lv_dropdown_set_options(ddlist, "なし\nマウスパッド\nジョイスティック");
	lv_obj_set_size(ddlist, 200, 34);
	lv_dropdown_set_selected(ddlist, mouse_pad_setting);
	lv_dropdown_set_symbol(ddlist, "v");
	lv_obj_align(ddlist, NULL, LV_ALIGN_IN_TOP_MID, 0, 60);
	lv_obj_set_event_cb(ddlist, change_mousepad_type);

	// 閉じるボタン
	lv_obj_t * btn1 = lv_btn_create(win, NULL);
	lv_obj_set_size(btn1, 200, 70);
	lv_obj_align(btn1, NULL, LV_ALIGN_IN_TOP_MID, 0, 150);
	lv_obj_set_event_cb(btn1, view_setting_mousepad_close);
	lv_obj_set_style_local_value_str(btn1, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, "閉じる");
}


// マウスパッド操作画面
void view_mouse_page() {
	// 画面上のオブジェクト全て削除
	lv_obj_clean(lv_scr_act());
	
	// マウスパッド操作
	mouse_pad_status = mouse_pad_setting;
}



// コンストラクタ
Display::Display() {
}

// 液晶制御初期化
void Display::begin(int option_type) {
	_option_type = option_type;
	this->_info_spot = 0;
	this->dakagi_last_view = -1;
	this->_wait_index = 0;
	this->_stimg_load_flag = 0;
	this->_thermo_flag = 0;
	this->_qr_flag = 0;
	this->_last_view_type = 255;
	this->_last_view_info = 255;
	lv_setup();
	view_mouse_page();
}

void Display::view_setting_menu() {
	view_setting_menu_fnc();
}

// 画面いっぱい黒い画面
void Display::view_black() {
}

// 画面いっぱいに画像を表示する
void Display::view_full_image(uint8_t *image_data) {
}

// 数字を表示
void Display::view_int(uint16_t x, uint16_t y, int v) {
	char s[12];
	uint8_t *set_img;
	int i;
	sprintf(s, "%D", v);
	i = 0;
	while (i < 12 && s[i] && x < this->_width) {
		if (s[i] == 0x30) set_img = (uint8_t *)int_0_img;
		if (s[i] == 0x31) set_img = (uint8_t *)int_1_img;
		if (s[i] == 0x32) set_img = (uint8_t *)int_2_img;
		if (s[i] == 0x33) set_img = (uint8_t *)int_3_img;
		if (s[i] == 0x34) set_img = (uint8_t *)int_4_img;
		if (s[i] == 0x35) set_img = (uint8_t *)int_5_img;
		if (s[i] == 0x36) set_img = (uint8_t *)int_6_img;
		if (s[i] == 0x37) set_img = (uint8_t *)int_7_img;
		if (s[i] == 0x38) set_img = (uint8_t *)int_8_img;
		if (s[i] == 0x39) set_img = (uint8_t *)int_9_img;
//		this->_tft->viewBMP(x, y, 16, 22, set_img, 10);
		x += 16;
		i++;
	}
	
}


// データを流し込んで画像を表示する(ヘッダ)
void Display::viewBMPspi_head() {
//	this->_tft->viewBMPspi_head(0, 0, this->_width, this->_height);
}
// データを流し込んで画像を表示する(データ)
void Display::viewBMPspi_data(uint8_t *wbuf, int wsize) {
//	this->_tft->viewBMPspi_data(wbuf, wsize);
}

// サーモグラフモードをON/OFF切り替える
void Display::view_dakagi_thermo_on() {
	if (this->_thermo_flag) {
		this->_thermo_flag = 0;
		this->_qr_flag = 0;
		this->view_type = DISP_TYPE_STANDBY;
	} else {
		this->_thermo_flag = 1;
		this->_qr_flag = 0;
		this->view_type = DISP_TYPE_DKTHERM;
	}
}

// QRコードモードをON/OFF切り替える
void Display::view_dakagi_qr_on() {
	if (this->_qr_flag) {
		this->_qr_flag = 0;
		if (this->_thermo_flag) {
			this->view_type = DISP_TYPE_DKTHERM;
		} else {
			this->view_type = DISP_TYPE_STANDBY;
		}
	} else {
		this->_qr_flag = 1;
		this->view_type = DISP_TYPE_DKQRCOD;
	}
}


// 画面を真っ暗にする
void Display::view_full_black() {
    M5.Lcd.fillScreen(BLACK);
	this->_last_view_type = 255;
}


void Display::open_movie() {
}
// 設定モード画面表示
void Display::view_setting_mode() {
    M5.Lcd.drawBitmap(0, 0, 240, 320, (uint16_t *)m5_setting_img);
	this->_last_view_type = DISP_TYPE_SETTING;
}
// 保存中画面表示
void Display::view_save() {
	this->_last_view_type = DISP_TYPE_SAVENOW;
}
// wifi 接続中
void Display::view_wifi_conn() {
	this->_last_view_type = DISP_TYPE_WIFICNN;
}
// Webhook中
void Display::view_webhook() {
	this->_last_view_type = DISP_TYPE_WEBFOOK;
}
// 暗記中
void Display::view_ankey_now() {
}
// 設定の初期化が完了しました画面表示
void Display::view_setting_init_comp() {
}
// 打鍵数を表示
void Display::view_dakagi() {
	// 最後に表示したInfoが打鍵数で最後に表示した打鍵数が今の打鍵数と一緒なら何もしない
	if (this->_last_view_info == 1 && this->dakagi_last_view == common_cls.key_count_total) return;
	// 最後に表示したInfoが空か、打鍵でなければInfoを空にする
	if (this->_last_view_info != 0 && this->_last_view_info != 1) {
//		this->_tft->fillRect(0, 210,  135, 30, WHITE);
	}
	// 打鍵数表示
//	this->_tft->viewBMP(5, 215, 57, 25, (uint8_t *)dakagi_img, 10);
	this->view_int(65, 217, common_cls.key_count_total);
	this->dakagi_last_view = common_cls.key_count_total;
	this->_last_view_info = 1;
}
// 打鍵サーモグラフを表示
void Display::view_dakagi_thermo() {
}
// 打鍵ヒートマップで今押されて緑色になっている所をヒートマップカラーにする
void Display::view_daken_key_reset() {
}
// 打鍵QRコードを表示
void Display::view_dakagi_qr() {
}

// 待ち受け画像表示
void Display::view_standby_image() {
	if (this->_last_view_type == DISP_TYPE_STANDBY) return; // 最後に表示したのが待ち受けなら何もしない
	M5.Lcd.drawBitmap(0, 0, 240, 320, (uint16_t *)m5_palettesystem_img);
	M5.Lcd.printf("BAT: %f", power.GetBatVoltage());
	this->_last_view_type = DISP_TYPE_STANDBY;
	this->_last_view_info = 255;
}
// LED ステータス表示
void Display::view_led_stat() {
}
// LED 明るさ設定表示
void Display::view_led_bright() {
}
// LED 色設定表示
void Display::view_led_color() {
}
// LED 光らせ方設定表示
void Display::view_led_shine() {
}
// 打鍵自動保存設定表示
void Display::view_dakey_auto_save() {
}
// Wifi接続して下さいエラー表示
void Display::view_error_wifi_conn() {
//	this->_tft->fillRect(0, 0,  135, 240, WHITE);
//	this->_tft->viewBMP(0, 90, 199, 26, (uint8_t *)please_wifi_txt_img, 10);
	this->_last_view_type = 255;
	delay(1500);
}
// 打鍵数を保存しましたテキスト表示
void Display::view_dakey_save_comp() {
}





// 定期処理
void Display::loop_exec() {
	lv_task_handler();
	
}

