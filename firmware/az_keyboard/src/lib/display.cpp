#include "Arduino.h"
#include "display.h"

#include "../../az_common.h"

// フォント
// http://www.fontna.com/blog/1226/
// LogoTypeGothic ロゴたいぷゴシック 07LogoTypeGothic7.ttf
// コンバーター： https://lvgl.io/tools/fontconverter

// 画像
// Color format：true color
// array： C array
// コンバーター：https://lvgl.io/tools/imageconverter

LV_IMG_DECLARE(setting_img);
LV_IMG_DECLARE(stimg_default);
LV_IMG_DECLARE(emugotch_logo_320);
LV_IMG_DECLARE(power_0_img);
LV_IMG_DECLARE(power_1_img);
LV_IMG_DECLARE(power_2_img);
LV_IMG_DECLARE(power_3_img);
LV_IMG_DECLARE(power_4_img);



lv_img_dsc_t stimg_obj;
bool stimg_load_fl;
uint8_t _disp_rotate; // 画面の向き

// マトリックスメニュー表示用のボタンデータ
const char *btnm_map[16];
// マトリックス長押しインデックス
uint16_t mx_pressing_index;


// リストオブジェクト
lv_obj_t * lv_list_obj;
// ドロップダウンオブジェクト
lv_obj_t * lv_drop_down_obj;
lv_obj_t * lv_drop_down_obj_2;
// スライダーオブジェクト
lv_obj_t * lv_slider_obj;
lv_obj_t * lv_slider_obj_2;
// テキストオブジェクト
lv_obj_t * lv_text_obj;
lv_obj_t * lv_text_obj_2;

// バッテリー残量アイコン
lv_obj_t * pwicon;

void _view_top_page(); // トップページ画面表示
void view_soft_key_page(); // ソフトウェアキー画面表示
void view_setting_mousepad(lv_obj_t * obj, lv_event_t event); // マウスパッド設定画面表示
void view_setting_led(lv_obj_t * obj, lv_event_t event); // バックライト設定画面表示
void view_setting_sound(lv_obj_t * obj, lv_event_t event); // サウンド設定画面表示
void view_setting_moniter(lv_obj_t * obj, lv_event_t event); // モニター設定画面表示
void view_setting_menu_fnc(); // 設定メニュー表示
void view_power_gate(); // バッテリー残量を表示


// 
void btmmtx_eve(lv_obj_t * obj, lv_event_t event) {
	// const char * txt = lv_btnmatrix_get_active_btn_text(obj);
	int ix;
	// LV_EVENT_PRESSED 押した時
	// LV_EVENT_PRESSING 押されてる間発生し続ける
	// LV_EVENT_SHORT_CLICKED 短押し(離された時1回)
	// LV_EVENT_LONG_PRESSED 長押し(長い判定された時1回)
	// LV_EVENT_LONG_PRESSED_REPEAT 長押し判定された後発生し続ける
	// LV_EVENT_CLICKED クリック(離された時1回、短押し判定の時も発生する)

    if (event == LV_EVENT_PRESSED) { // 押された時
		mx_pressing_index = 0; // マトリックス長押しインデックス初期化
	}
    if(event == LV_EVENT_CLICKED) { // 離された時
		ix = lv_btnmatrix_get_active_btn(obj);
		soft_click_layer = 0;
		soft_click_key = ix;
    }
	if (event == LV_EVENT_PRESSING) { // 押し続けてる間
		mx_pressing_index++;
		if (mx_pressing_index > 30) { // 閾値を超えたら設定画面表示
			M5.Axp.SetLDOEnable(3, true);
			delay(100);
			M5.Axp.SetLDOEnable(3, false);
			while (M5.Touch.ispressed()) delay(100);
			disp->view_setting_menu();
		}
	} 
}

//=====================================================================
/*Read the touchpad*/
bool my_touchpad_read(lv_indev_drv_t * indev_driver, lv_indev_data_t * data){
	// 操作なし、タッチパッド、ジョイスティックの場合はタッチ反応無視
	if (mouse_pad_status == 0 || mouse_pad_status == 1 || mouse_pad_status == 2) {
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

// 設定メニュー閉じるイベント
void close_setting(lv_obj_t * obj, lv_event_t event) {
	if (event == LV_EVENT_CLICKED) { // クリック
		// トップページ画面表示
        _view_top_page();
	}
}

void lv_setup() {
  lvtft.begin();
  lvtft.setRotation(_disp_rotate);
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
  
  int i;
  for (i=0; i<16; i++) {
	  btnm_map[i] = new char[24];
	  btnm_map[i] = "";
  }
  // init spiffs system driver
  // init_file_system_driver();
  stimg_load_fl = false;
  pw_update_index = -1;


}

// 設定モードで起動
static void reboot_setting_mode(lv_obj_t * obj, lv_event_t event) {
	if(event == LV_EVENT_VALUE_CHANGED) {
		// 画面上のオブジェクト全て削除
		lv_obj_clean(lv_scr_act());
		int b = lv_msgbox_get_active_btn(obj);
		if (b == 0) { // はい
			// 再起動
			restart_flag = 1; // 設定モードで再起動
			restart_index = 0; // カウント用のインデックス0
			lvgl_loop_index = 2; // 表示インデックス
			menu_mode_flag = false;
		} else { // いいえ
			// 設定メニュー表示
			view_setting_menu_fnc();
		}
	}
}

// 設定モードで起動確認ウィンドウ表示
void reboot_setting_mode_alert(lv_obj_t * obj, lv_event_t event) {
    if(event == LV_EVENT_CLICKED) { // クリック
		// 画面上のオブジェクト全て削除
		lv_obj_clean(lv_scr_act());
		// メッセージボックス表示
    	static const char * btns[] ={"はい", "いいえ", ""};
		lv_obj_t * mbox1 = lv_msgbox_create(lv_scr_act(), NULL);
		lv_msgbox_set_text(mbox1, "設定モードで再起動します");
		lv_msgbox_add_btns(mbox1, btns);
		lv_obj_set_width(mbox1, 200);
		lv_obj_set_event_cb(mbox1, reboot_setting_mode);
		lv_obj_align(mbox1, NULL, LV_ALIGN_CENTER, 0, 0); /*Align to the corner*/
    	return;
    }
}


// キーボード変更画面クローズボタンイベント
void view_keyboard_select_close(lv_obj_t * obj, lv_event_t event) {
	if(event == LV_EVENT_CLICKED) { // クリック
		view_setting_menu_fnc(); // 設定メニューに戻る
	}
}

// キーボード変更画面決定ボタンイベント(通常時)
void view_keyboard_select_exec(lv_obj_t * obj, lv_event_t event) {
	if(event == LV_EVENT_CLICKED) { // クリック
		// 選択した
		bool change_flag = false;
		int select_no = lv_dropdown_get_selected(lv_drop_down_obj);
		if (select_no == 0 && strcmp(eep_data.keyboard_type, "az_emugotch") != 0) {
			strcpy(eep_data.keyboard_type, "az_emugotch");
			change_flag = true;
		} else if (select_no == 1 && strcmp(eep_data.keyboard_type, "az_m5ortho") != 0) {
			strcpy(eep_data.keyboard_type, "az_m5ortho");
			change_flag = true;
		} else if (select_no == 2 && strcmp(eep_data.keyboard_type, "az_m5orthow") != 0) {
			strcpy(eep_data.keyboard_type, "az_m5orthow");
			change_flag = true;
		}
		if (change_flag) {
			// 変更があれば保存して再起動
			common_cls.delete_all(); // 全てのファイルを削除
			common_cls.save_data(); // eep_data保存
			common_cls.create_setting_json(); // 設定ファイル作成
			lv_obj_clean(lv_scr_act()); // 画面上のオブジェクト全て削除
			restart_flag = 0; // キーボードモードで再起動
			restart_index = 0; // カウント用のインデックス0
			menu_mode_flag = false; // メニューモード終了
		} else {
			// 変更が無ければメニューに戻る
			view_setting_menu_fnc(); // 設定メニューに戻る
		}
	}
}


// キーボード選択画面表示(select_mode = 0 設定メニューから来た / 1 初期起動時のキーボード選択)
void view_keyboard_select(int select_mode) {

	// モニタ描画停止
	disp_enable = false;

	// 画面上のオブジェクト全て削除
	lv_obj_clean(lv_scr_act());
	
	// window作成
    lv_obj_t * win = lv_win_create(lv_scr_act(), NULL);
    lv_win_set_title(win, "キーボード選択");

	// テキスト
    lv_obj_t * txt = lv_label_create(win, NULL);
    lv_label_set_text(txt, "接続しているキーボー\nドを選んで下さい。");
    lv_obj_align(txt, NULL, LV_ALIGN_IN_TOP_MID, 0, 15);

	// セレクトメニュー表示
	lv_drop_down_obj = lv_dropdown_create(win, NULL);
	lv_dropdown_set_options(lv_drop_down_obj, "えむごっち\nAZ-M5ortho\nAZ-M5orthoW");
	lv_obj_set_size(lv_drop_down_obj, 200, 34);
	int selected_num = 0;
	if (strcmp(eep_data.keyboard_type, "az_emugotch") == 0) selected_num = 0;
	if (strcmp(eep_data.keyboard_type, "az_m5ortho") == 0) selected_num = 1;
	if (strcmp(eep_data.keyboard_type, "az_m5orthow") == 0) selected_num = 2;
	lv_dropdown_set_selected(lv_drop_down_obj, selected_num);
	lv_dropdown_set_symbol(lv_drop_down_obj, "▼");
	lv_obj_align(lv_drop_down_obj, NULL, LV_ALIGN_IN_TOP_MID, 0, 70);
	// lv_obj_set_event_cb(lv_drop_down_obj, change_mousepad_type);

	// 決定ボタン
	lv_obj_t * btn1 = lv_btn_create(win, NULL);
	lv_obj_set_size(btn1, 100, 40);
	if (select_mode == 1) {
		lv_obj_align(btn1, NULL, LV_ALIGN_IN_TOP_MID, 0, 150);
	} else {
		lv_obj_align(btn1, NULL, LV_ALIGN_IN_TOP_MID, -60, 150);
	}
	lv_obj_set_event_cb(btn1, view_keyboard_select_exec);
	lv_obj_set_style_local_value_str(btn1, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, "決定");

	// 閉じるボタン
	if (select_mode != 1) {
		lv_obj_t * btn2 = lv_btn_create(win, NULL);
		lv_obj_set_size(btn2, 100, 40);
		lv_obj_align(btn2, NULL, LV_ALIGN_IN_TOP_MID, 50, 150);
		lv_obj_set_event_cb(btn2, view_keyboard_select_close);
		lv_obj_set_style_local_value_str(btn2, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, "閉じる");
	}

	// モニタ描画開始
	disp_enable = true;
}

// キーボード選択ボタンイベント
void view_keyboard_select_event(lv_obj_t * obj, lv_event_t event) {
	if(event == LV_EVENT_CLICKED) { // クリック
		view_keyboard_select(0);
	}
}


// 設定メニュー画面表示
void view_setting_menu_fnc() {

	// モニタ描画停止
	disp_enable = false;

    // バッテリ残量更新フラグ(-1 = 更新しない)
	pw_update_index = -1;

	// 画面上のオブジェクト全て削除
	lv_obj_clean(lv_scr_act());

	// 設定画面用の画面の明るさに設定
	common_cls.moniter_brightness(1);


	if (_disp_rotate == 0 || _disp_rotate == 2) { // 縦長
	    // テキスト
        lv_obj_t * txt = lv_label_create(lv_scr_act(), NULL);
        lv_label_set_text(txt, "設定メニュー");
        lv_obj_align(txt, NULL, LV_ALIGN_IN_TOP_MID, 0, 5);

        // リスト
    	lv_list_obj = lv_list_create(lv_scr_act(), NULL);
		lv_obj_set_size(lv_list_obj, 230, 280);
	    lv_obj_align(lv_list_obj, NULL, LV_ALIGN_CENTER, 0, 10);

	} else { // 横長
		// リスト
    	lv_list_obj = lv_list_create(lv_scr_act(), NULL);
		lv_obj_set_size(lv_list_obj, 280, 230);
	    lv_obj_align(lv_list_obj, NULL, LV_ALIGN_CENTER, 0, 0);
	}
	
	// リストにデータ追加
	lv_obj_t * btn;
    btn = lv_list_add_btn(lv_list_obj, NULL, "キーボード選択");
    lv_obj_set_event_cb(btn, view_keyboard_select_event);
    btn = lv_list_add_btn(lv_list_obj, NULL, "画面設定");
    lv_obj_set_event_cb(btn, view_setting_moniter);
    btn = lv_list_add_btn(lv_list_obj, NULL, "待ち受け操作");
    lv_obj_set_event_cb(btn, view_setting_mousepad);
    btn = lv_list_add_btn(lv_list_obj, NULL, "バックライト設定");
    lv_obj_set_event_cb(btn, view_setting_led);
    btn = lv_list_add_btn(lv_list_obj, NULL, "サウンド設定");
    lv_obj_set_event_cb(btn, view_setting_sound);
    btn = lv_list_add_btn(lv_list_obj, NULL, "設定モードで再起動");
    lv_obj_set_event_cb(btn, reboot_setting_mode_alert);
    btn = lv_list_add_btn(lv_list_obj, "×", "閉じる");
    lv_obj_set_event_cb(btn, close_setting);

	// マウスパッド操作停止
	mouse_pad_status = -1;

	// メニュー表示中にする
	menu_mode_flag = true;

	// モニタ描画開始
	disp_enable = true;
}

// マウスパッド設定画面決定クリック
void setting_mousepad_save(lv_obj_t * obj, lv_event_t event) {
	// クリック以外のイベントは無視
	if (event != LV_EVENT_CLICKED) return;
	// 設定を反映
	mouse_pad_setting.mouse_type = lv_dropdown_get_selected(lv_drop_down_obj);
	mouse_pad_setting.mouse_speed = lv_slider_get_value(lv_slider_obj);
	// 設定を保存
	common_cls.mouse_pad_save();
	// 設定メニュー表示
	view_setting_menu_fnc();
}

// 設定メニューに戻る
void view_setting_menu_ev(lv_obj_t * obj, lv_event_t event) {
	// クリック以外のイベントは無視
	if (event != LV_EVENT_CLICKED) return;
	// 設定メニュー表示
	view_setting_menu_fnc();
}


// マウス操作設定セレクトが変更された
void change_mousepad_type(lv_obj_t * obj, lv_event_t event) {
    if(event == LV_EVENT_VALUE_CHANGED) {
        mouse_pad_setting.mouse_type = lv_dropdown_get_selected(obj);
    }
}

// マウス速度が変更された
void change_mousepad_speed(lv_obj_t * obj, lv_event_t event) {
    if(event == LV_EVENT_VALUE_CHANGED) {
        char kand_buf[32];
        sprintf(kand_buf, "感度: %D", lv_slider_get_value(obj));
        lv_label_set_text(lv_text_obj, kand_buf);
    }
}


// マウスパッド設定画面表示
void view_setting_mousepad(lv_obj_t * obj, lv_event_t event) {
	// クリック以外のイベントは無視
	if (event != LV_EVENT_CLICKED) return;

	// モニタ描画停止
	disp_enable = false;

	// 画面上のオブジェクト全て削除
	lv_obj_clean(lv_scr_act());

	// window作成
    lv_obj_t * win = lv_win_create(lv_scr_act(), NULL);
    lv_win_set_title(win, "待ち受け操作");

	// テキスト
    lv_obj_t * txt = lv_label_create(win, NULL);
    lv_label_set_text(txt, "操作方法");
    lv_obj_align(txt, NULL, LV_ALIGN_IN_TOP_MID, -70, 20);

	// セレクトメニュー表示
	lv_drop_down_obj = lv_dropdown_create(win, NULL);
	lv_dropdown_set_options(lv_drop_down_obj, "なし\nマウスパッド\nジョイスティック\nソフトウェアキー");
	lv_obj_set_size(lv_drop_down_obj, 180, 34);
	lv_dropdown_set_selected(lv_drop_down_obj, mouse_pad_setting.mouse_type);
	lv_dropdown_set_symbol(lv_drop_down_obj, "▼");
	lv_obj_align(lv_drop_down_obj, NULL, LV_ALIGN_IN_TOP_MID, -10, 60);

	// 感度
	char kand_buf[32];
	sprintf(kand_buf, "感度: %D", mouse_pad_setting.mouse_speed);
    lv_text_obj = lv_label_create(win, NULL);
    lv_label_set_text(lv_text_obj, kand_buf);
    lv_obj_align(lv_text_obj, NULL, LV_ALIGN_IN_TOP_MID, -80, 140);

	// 感度調節のスライダー
	lv_slider_obj = lv_slider_create(win, NULL);
	lv_obj_set_size(lv_slider_obj, 150, 10);
	lv_obj_align(lv_slider_obj, NULL, LV_ALIGN_IN_TOP_MID, -10, 180);
	lv_obj_set_event_cb(lv_slider_obj, change_mousepad_speed);
	lv_slider_set_range(lv_slider_obj, 0, 100);
	lv_slider_set_value(lv_slider_obj, mouse_pad_setting.mouse_speed, LV_ANIM_OFF);

	// 決定ボタン
	lv_obj_t * btn1 = lv_btn_create(win, NULL);
	lv_obj_set_size(btn1, 100, 50);
	lv_obj_align(btn1, NULL, LV_ALIGN_IN_TOP_MID, -60, 240);
	lv_obj_set_event_cb(btn1, setting_mousepad_save);
	lv_obj_set_style_local_value_str(btn1, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, "決定");

	// 閉じるボタン
	lv_obj_t * btn2 = lv_btn_create(win, NULL);
	lv_obj_set_size(btn2, 100, 50);
	lv_obj_align(btn2, NULL, LV_ALIGN_IN_TOP_MID, 50, 240);
	lv_obj_set_event_cb(btn2, view_setting_menu_ev);
	lv_obj_set_style_local_value_str(btn2, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, "閉じる");
	
	// 下がギリギリ過ぎてボタンが押せないから空のラベルを置いて下に隙間を作る
    lv_obj_t * txt2 = lv_label_create(win, NULL);
    lv_label_set_text(txt2, " ");
    lv_obj_align(txt2, NULL, LV_ALIGN_IN_TOP_MID, 0, 300);

	// ドラッグした時だけスクロールバー表示
	// lv_win_set_scrollbar_mode(win, LV_SCRLBAR_MODE_DRAG);

	// モニタ描画開始
	disp_enable = true;
}

// バックライトON/OFF
void led_enable_set(lv_obj_t * obj, lv_event_t event)
{
    if(event == LV_EVENT_VALUE_CHANGED) {
    	if (lv_switch_get_state(obj)) {
    		rgb_led_cls.set_status(1);
    	} else {
    		rgb_led_cls.set_status(0);
    	}
    }
}

// バックライト明るさアップ
void led_bright_ev(lv_obj_t * obj, lv_event_t event) {
	if(event == LV_EVENT_VALUE_CHANGED) {
		rgb_led_cls.set_bright(lv_slider_get_value(obj));
	}
}


// 光り方変更
void led_shine_type(lv_obj_t * obj, lv_event_t event) {
    if(event == LV_EVENT_VALUE_CHANGED) {
    	rgb_led_cls.set_shine_type(lv_dropdown_get_selected(obj));
    }
}

// 色変更
void led_color_type(lv_obj_t * obj, lv_event_t event) {
    if(event == LV_EVENT_VALUE_CHANGED) {
    	rgb_led_cls.set_color_type(lv_dropdown_get_selected(obj));
    }
}


// バックライト設定画面表示
void view_setting_led(lv_obj_t * obj, lv_event_t event) {
	// クリック以外のイベントは無視
	if (event != LV_EVENT_CLICKED) return;

	// モニタ描画停止
	disp_enable = false;

	// 画面上のオブジェクト全て削除
	lv_obj_clean(lv_scr_act());
	// window作成
    lv_obj_t * win = lv_win_create(lv_scr_act(), NULL);
    lv_win_set_title(win, "バックライト設定");

	// テキスト
    lv_obj_t * txt = lv_label_create(win, NULL);
    lv_label_set_text(txt, "ON/OFF");
    lv_obj_align(txt, NULL, LV_ALIGN_IN_TOP_MID, -50, 40);

	// ON/OFF のスイッチ
    lv_obj_t * sw;
    sw = lv_switch_create(win, NULL);
    lv_obj_set_event_cb(sw, led_enable_set);
	lv_obj_set_size(sw, 70, 40);
	if (rgb_led_cls._setting.status) {
		lv_switch_on(sw, LV_ANIM_OFF);
	} else {
		lv_switch_off(sw, LV_ANIM_OFF);
	}
    lv_obj_align(sw, NULL, LV_ALIGN_IN_TOP_MID, 30, 30);

	// 明るさ
    lv_obj_t * txt2 = lv_label_create(win, NULL);
    lv_label_set_text(txt2, "明るさ");
    lv_obj_align(txt2, NULL, LV_ALIGN_IN_TOP_MID, -80, 100);

	// 明るさ調節のスライダー
	lv_obj_t * slider = lv_slider_create(win, NULL);
	lv_obj_set_size(slider, 150, 10);
	lv_obj_align(slider, NULL, LV_ALIGN_IN_TOP_MID, 0, 140);
	lv_obj_set_event_cb(slider, led_bright_ev);
	lv_slider_set_range(slider, 0, 15);
	lv_slider_set_value(slider, rgb_led_cls._setting.bright, LV_ANIM_OFF);

	// 光らせ方メニュー
	lv_drop_down_obj = lv_dropdown_create(win, NULL);
	lv_dropdown_set_options(lv_drop_down_obj, "常に点灯\n押したら点灯\n周りに広がる\nグラデーション");
	lv_obj_set_size(lv_drop_down_obj, 200, 34);
	lv_dropdown_set_selected(lv_drop_down_obj, rgb_led_cls._setting.shine_type);
	lv_dropdown_set_symbol(lv_drop_down_obj, "▼");
	lv_obj_align(lv_drop_down_obj, NULL, LV_ALIGN_IN_TOP_MID, 0, 200);
	lv_obj_set_event_cb(lv_drop_down_obj, led_shine_type);

	// 色メニュー
	lv_drop_down_obj_2 = lv_dropdown_create(win, NULL);
	lv_dropdown_set_options(lv_drop_down_obj_2, "ホワイト\nレッド\nグリーン\nブルー\nスカイブルー\nイエロー");
	lv_obj_set_size(lv_drop_down_obj_2, 200, 34);
	lv_dropdown_set_selected(lv_drop_down_obj_2, rgb_led_cls._setting.color_type);
	lv_dropdown_set_symbol(lv_drop_down_obj_2, "▼");
	lv_obj_align(lv_drop_down_obj_2, NULL, LV_ALIGN_IN_TOP_MID, 0, 270);
	lv_obj_set_event_cb(lv_drop_down_obj_2, led_color_type);

	// 閉じるボタン
	lv_obj_t * btn5 = lv_btn_create(win, NULL);
	lv_obj_set_size(btn5, 150, 50);
	lv_obj_align(btn5, NULL, LV_ALIGN_IN_TOP_MID, 0, 340);
	lv_obj_set_event_cb(btn5, view_setting_menu_ev);
	lv_obj_set_style_local_value_str(btn5, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, "閉じる");
	
	// モニタ描画開始
	disp_enable = true;
}

// サウンドON/OFF
void sound_enable_set(lv_obj_t * obj, lv_event_t event)
{
    if(event == LV_EVENT_VALUE_CHANGED) {
    	if (lv_switch_get_state(obj)) {
    		sound_cls.set_enable(1);
    	} else {
    		sound_cls.set_enable(0);
    	}
    }
}

// サウンド音量調節
void sound_volume_ev(lv_obj_t * obj, lv_event_t event) {
	if(event == LV_EVENT_VALUE_CHANGED) {
		sound_cls.set_volume(lv_slider_get_value(obj));
	}
}

// 音のタイプ
void sound_key_type(lv_obj_t * obj, lv_event_t event) {
    if(event == LV_EVENT_VALUE_CHANGED) {
    	sound_cls.set_type_default(lv_dropdown_get_selected(obj));
    }
}

// サウンド設定画面表示
void view_setting_sound(lv_obj_t * obj, lv_event_t event) {
	// クリック以外のイベントは無視
	if (event != LV_EVENT_CLICKED) return;

	// モニタ描画停止
	disp_enable = false;

	// 画面上のオブジェクト全て削除
	lv_obj_clean(lv_scr_act());

	// window作成
    lv_obj_t * win = lv_win_create(lv_scr_act(), NULL);
    lv_win_set_title(win, "サウンド設定");

	// テキスト
    lv_obj_t * txt = lv_label_create(win, NULL);
    lv_label_set_text(txt, "ON/OFF");
    lv_obj_align(txt, NULL, LV_ALIGN_IN_TOP_MID, -50, 40);

	// ON/OFF のスイッチ
    lv_obj_t * sw;
    sw = lv_switch_create(win, NULL);
    lv_obj_set_event_cb(sw, sound_enable_set);
	lv_obj_set_size(sw, 70, 40);
	if (sound_cls._setting.sound_enable) {
		lv_switch_on(sw, LV_ANIM_OFF);
	} else {
		lv_switch_off(sw, LV_ANIM_OFF);
	}
    lv_obj_align(sw, NULL, LV_ALIGN_IN_TOP_MID, 30, 30);

	// 音量
    lv_obj_t * txt2 = lv_label_create(win, NULL);
    lv_label_set_text(txt2, "音量");
    lv_obj_align(txt2, NULL, LV_ALIGN_IN_TOP_MID, -80, 100);

	// 音量調節のスライダー
	lv_obj_t * slider = lv_slider_create(win, NULL);
	lv_obj_set_size(slider, 150, 10);
	lv_obj_align(slider, NULL, LV_ALIGN_IN_TOP_MID, 0, 140);
	lv_obj_set_event_cb(slider, sound_volume_ev);
	lv_slider_set_range(slider, 0, 255);
	lv_slider_set_value(slider, sound_cls._setting.volume, LV_ANIM_OFF);

	// 音の種類メニュー
	lv_drop_down_obj = lv_dropdown_create(win, NULL);
	lv_dropdown_set_options(lv_drop_down_obj, "なし \nカスタム音 \nタイプ 1\nタイプ 2\nタイプ 3\nタイプ 4\nタイプ 5\nタイプ 6\nタイプ 7");
	lv_obj_set_size(lv_drop_down_obj, 200, 34);
	lv_dropdown_set_selected(lv_drop_down_obj, sound_cls._setting.type_default);
	lv_dropdown_set_symbol(lv_drop_down_obj, "▼");
	lv_obj_align(lv_drop_down_obj, NULL, LV_ALIGN_IN_TOP_MID, 0, 200);
	lv_obj_set_event_cb(lv_drop_down_obj, sound_key_type);

	// 閉じるボタン
	lv_obj_t * btn5 = lv_btn_create(win, NULL);
	lv_obj_set_size(btn5, 150, 50);
	lv_obj_align(btn5, NULL, LV_ALIGN_IN_TOP_MID, 0, 280);
	lv_obj_set_event_cb(btn5, view_setting_menu_ev);
	lv_obj_set_style_local_value_str(btn5, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, "閉じる");
	
	// モニタ描画開始
	disp_enable = true;
}




// マウスパッド操作画面
void _view_top_page() {
	
	// 待ち受け画面用の画面の明るさに設定
	common_cls.moniter_brightness(0);

	// ソフトウェアキーが設定されていたらソフトウェアキー画面表示
	if (mouse_pad_setting.mouse_type == 3) {
		view_soft_key_page();
		return;
	}

	// モニタ描画停止
	disp_enable = false;

	// 画面上のオブジェクト全て削除
	lv_obj_clean(lv_scr_act());
	uint8_t head_data[4];


	// 待ち受け画像表示
	char stimg_file_name[32];
	if (_disp_rotate == 0 || _disp_rotate == 2) { // 縦長
	    strcpy(stimg_file_name, "/stimg.bin");
	} else { // 横長
	    strcpy(stimg_file_name, "/stimg_w.bin");
	}
	if (SPIFFS.exists(stimg_file_name)) {
		if (!stimg_load_fl) {
			stimg_obj.header.always_zero = 0;
			stimg_obj.header.cf = LV_IMG_CF_TRUE_COLOR;
			File fp = SPIFFS.open(stimg_file_name, FILE_READ);
			fp.read((unsigned char*)head_data, 4);
			stimg_obj.header.w = (head_data[1] >> 2) | ((head_data[2] & 0x1F) << 6);
			stimg_obj.header.h = ((head_data[2] & 0xE0) >> 5) | (head_data[3] << 3);
			stimg_obj.data_size = stimg_obj.header.w * stimg_obj.header.h * 2;
			stimg_obj.data = (uint8_t *)heap_caps_malloc(stimg_obj.data_size, MALLOC_CAP_SPIRAM); // new uint8_t[stimg_obj.data_size];
			fp.read((unsigned char*)stimg_obj.data, stimg_obj.data_size);
			fp.close();
			stimg_load_fl = true;
		}
		
		
		lv_obj_t * icon = lv_img_create(lv_scr_act(), NULL);
		lv_img_set_src(icon, &stimg_obj);
		lv_obj_align(icon, NULL, LV_ALIGN_CENTER, 0, 0);
	} else {
		// ファイルが無ければデフォルトの画像表示
		lv_obj_t * icon = lv_img_create(lv_scr_act(), NULL);
		if (keyboard_type_int == 3) { // えむごっち
			lv_img_set_src(icon, &emugotch_logo_320); // えむごっち logo
		} else {
    		lv_img_set_src(icon, &stimg_default); // パレットシステムロゴ (たて)
		}
		lv_obj_align(icon, NULL, LV_ALIGN_CENTER, 0, 0);
	}

    // バッテリ残量表示
	view_power_gate();

	// 閉じるボタン
	// lv_obj_t * btn5 = lv_btn_create(lv_scr_act(), NULL);
	// lv_obj_set_size(btn5, 50, 50);
	// lv_obj_align(btn5, NULL, LV_ALIGN_IN_TOP_LEFT, 10, 10);
	// // lv_obj_set_event_cb(btn5, view_setting_menu_ev);
	// lv_obj_set_style_local_value_str(btn5, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, "１");

	// マウスパッド操作
	mouse_pad_status = mouse_pad_setting.mouse_type;
	// メニュー表示終了
	menu_mode_flag = false;
	// 表示インデックス
	lvgl_loop_index = 2;
	// モニタ描画開始
	disp_enable = true;
	
}

// バッテリー残量を表示
void view_power_gate() {
	float pw = power.GetBatVoltage();
	if (pw_update_index >= 0) {
		lv_obj_del(pwicon);
	}
	pw_update_index = 0;
	pwicon = lv_img_create(lv_scr_act(), NULL);
	if (pw > 3.9) {
	    lv_img_set_src(pwicon, &power_4_img);
	} else if (pw > 3.7) {
	    lv_img_set_src(pwicon, &power_3_img);
	} else if (pw > 3.6) {
	    lv_img_set_src(pwicon, &power_2_img);
	} else if (pw > 3.4) {
	    lv_img_set_src(pwicon, &power_1_img);
	} else {
	    lv_img_set_src(pwicon, &power_0_img);
	}
	lv_obj_align(pwicon, NULL, LV_ALIGN_IN_TOP_RIGHT, -10, 10);

}

// ソフトウェアキー画面表示
void view_soft_key_page() {
	// モニタ描画停止
	disp_enable = false;

	// 画面上のオブジェクト全て削除
	lv_obj_clean(lv_scr_act());

    int i, n;
	n = 0;
	for (i=0; i<soft_setting_length; i++) {
		btnm_map[n] = soft_setting_press[i].key_name;
		n++;
		if ((i % 2) && (soft_setting_length - 1) > i) {
			btnm_map[n] = "\n";
			n++;
		}
	}
	btnm_map[n] = "";
    lv_obj_t * btnm1 = lv_btnmatrix_create(lv_scr_act(), NULL);
    lv_btnmatrix_set_map(btnm1, btnm_map);
    // lv_btnmatrix_set_btn_width(btnm1, 2, 2); // 指定したボタンの幅の割合を指定
    // lv_btnmatrix_set_btn_ctrl(btnm1, 6, LV_BTNMATRIX_CTRL_CHECKABLE); // トグルボタン
    // lv_btnmatrix_set_btn_ctrl(btnm1, 7, LV_BTNMATRIX_CTRL_CHECK_STATE); // トグル選択しっぱなし固定
	if (_disp_rotate == 0 || _disp_rotate == 2) {
        lv_obj_set_size(btnm1, 240, 320);
	} else {
        lv_obj_set_size(btnm1, 320, 240);
	}
	lv_obj_align(btnm1, NULL, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_event_cb(btnm1, btmmtx_eve);

    // バッテリ残量表示
	view_power_gate();

	// マウスパッド操作
	mouse_pad_status = mouse_pad_setting.mouse_type;
	// メニュー表示終了
	menu_mode_flag = false;
	// 表示インデックス
	lvgl_loop_index = 0;
	// モニタ描画開始
	disp_enable = true;
}


// バックライト明るさアップ
void moniter_menu_bright_ev(lv_obj_t * obj, lv_event_t event) {
	if(event == LV_EVENT_VALUE_CHANGED) {
		int v = lv_slider_get_value(obj);
		int s;
		if (v > 0) {
			s = 2500 + ((600 * v) / 100);
			M5.Axp.SetDCDC3(true);
			M5.Axp.SetLcdVoltage(s);
			// M5.Lcd.setBrightness(s);
		} else {
			M5.Axp.SetDCDC3(false);
		}
	}
}

// 画面設定画面決定クリック
void setting_moniter_save(lv_obj_t * obj, lv_event_t event) {
	// クリック以外のイベントは無視
	if (event != LV_EVENT_CLICKED) return;
	// 設定を反映
	moniter_setting.st_brightness = lv_slider_get_value(lv_slider_obj);
	moniter_setting.menu_brightness = lv_slider_get_value(lv_slider_obj_2);
	// 設定を保存
	common_cls.moniterset_save();
	// 設定メニュー表示
	view_setting_menu_fnc();
}

// 画面設定ページ表示
void view_setting_moniter(lv_obj_t * obj, lv_event_t event) {
	// クリック以外のイベントは無視
	if (event != LV_EVENT_CLICKED) return;

	// 画面上のオブジェクト全て削除
	lv_obj_clean(lv_scr_act());

	// window作成
    lv_obj_t * win = lv_win_create(lv_scr_act(), NULL);
    lv_win_set_title(win, "画面設定");

	// 待ち受け画面の明るさ
    lv_text_obj = lv_label_create(win, NULL);
    lv_label_set_text(lv_text_obj, "待ち受け画面の明るさ");
    lv_obj_align(lv_text_obj, NULL, LV_ALIGN_IN_TOP_MID, -10, 40);

	// 明るさ調節のスライダー
	lv_slider_obj = lv_slider_create(win, NULL);
	lv_obj_set_size(lv_slider_obj, 150, 10);
	lv_obj_align(lv_slider_obj, NULL, LV_ALIGN_IN_TOP_MID, 0, 90);
	// lv_obj_set_event_cb(lv_slider_obj, led_bright_ev);
	lv_slider_set_range(lv_slider_obj, 0, 100);
	lv_slider_set_value(lv_slider_obj, moniter_setting.st_brightness, LV_ANIM_OFF);

	// メニュー画面の明るさ
    lv_text_obj_2 = lv_label_create(win, NULL);
    lv_label_set_text(lv_text_obj_2, "メニュー画面の明るさ");
    lv_obj_align(lv_text_obj_2, NULL, LV_ALIGN_IN_TOP_MID, -10, 140);

	// 明るさ調節のスライダー
	lv_slider_obj_2 = lv_slider_create(win, NULL);
	lv_obj_set_size(lv_slider_obj_2, 150, 10);
	lv_obj_align(lv_slider_obj_2, NULL, LV_ALIGN_IN_TOP_MID, 0, 190);
	lv_obj_set_event_cb(lv_slider_obj_2, moniter_menu_bright_ev);
	lv_slider_set_range(lv_slider_obj_2, 0, 100);
	lv_slider_set_value(lv_slider_obj_2, moniter_setting.menu_brightness, LV_ANIM_OFF);

	// 決定ボタン
	lv_obj_t * btn1 = lv_btn_create(win, NULL);
	lv_obj_set_size(btn1, 100, 50);
	lv_obj_align(btn1, NULL, LV_ALIGN_IN_TOP_MID, -60, 240);
	lv_obj_set_event_cb(btn1, setting_moniter_save);
	lv_obj_set_style_local_value_str(btn1, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, "決定");

	// 閉じるボタン
	lv_obj_t * btn2 = lv_btn_create(win, NULL);
	lv_obj_set_size(btn2, 100, 50);
	lv_obj_align(btn2, NULL, LV_ALIGN_IN_TOP_MID, 50, 240);
	lv_obj_set_event_cb(btn2, view_setting_menu_ev);
	lv_obj_set_style_local_value_str(btn2, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, "閉じる");
	
	// 下がギリギリ過ぎてボタンが押せないから空のラベルを置いて下に隙間を作る
    lv_obj_t * txt2 = lv_label_create(win, NULL);
    lv_label_set_text(txt2, " ");
    lv_obj_align(txt2, NULL, LV_ALIGN_IN_TOP_MID, 0, 300);

}






// コンストラクタ
Display::Display() {
}

// 液晶制御初期化
void Display::begin(uint8_t disp_rotate) {
	this->_info_spot = 0;
	this->dakagi_last_view = -1;
	this->_wait_index = 0;
	this->_stimg_load_flag = 0;
	this->_thermo_flag = 0;
	this->_qr_flag = 0;
	this->_last_view_type = 255;
	this->_last_view_info = 255;
	_disp_rotate = 0;
	if (disp_rotate >= 0 && disp_rotate <= 3) {
	  _disp_rotate = disp_rotate;
	}
	lv_setup();
	// disp_enable = true;
	// _view_top_page();
}

// 待ち受けページ表示
void Display::view_top_page() {
	_view_top_page();
}

// 設定メニュー表示
void Display::view_setting_menu() {
	// 設定画面用の画面の明るさに設定
	common_cls.moniter_brightness(1);
	// 設定画面表示
	view_setting_menu_fnc();
}

// キーボード選択画面表示
void Display::view_keyboard_setting() {
	view_keyboard_select(1);
}

// 画面いっぱい黒い画面
void Display::view_black() {
}

// 画面いっぱいに画像を表示する
void Display::view_full_image(uint8_t *image_data) {
}

// 数字を表示
void Display::view_int(uint16_t x, uint16_t y, int v) {
	/*
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
	*/
	
}

// 渡した画像データを画面に表示する
void Display::view_raw_image(uint8_t *img_data) {
	stimg_obj.header.always_zero = 0;
	stimg_obj.header.cf = LV_IMG_CF_TRUE_COLOR;
	stimg_obj.header.w = (img_data[1] >> 2) | ((img_data[2] & 0x1F) << 6);
	stimg_obj.header.h = ((img_data[2] & 0xE0) >> 5) | (img_data[3] << 3);
	stimg_obj.data_size = stimg_obj.header.w * stimg_obj.header.h * 2;
	stimg_obj.data = (uint8_t *)&img_data[4];

	// 画面上のオブジェクト全て削除
	lv_obj_clean(lv_scr_act());

	// 画像オブジェクト作成
	lv_obj_t * icon = lv_img_create(lv_scr_act(), NULL);
	lv_img_set_src(icon, &stimg_obj);
	lv_obj_align(icon, NULL, LV_ALIGN_CENTER, 0, 0);
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
    lv_obj_t * img1 = lv_img_create(lv_scr_act(), NULL);
    lv_img_set_src(img1, &setting_img);
    lv_obj_align(img1, NULL, LV_ALIGN_CENTER, 0, 0);
    // M5.Lcd.drawBitmap(0, 0, 240, 320, (uint16_t *)m5_setting_img);
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
// バッテリー残量更新
void Display::view_power() {
	view_power_gate();
}




// 定期処理
void Display::loop_exec() {
	// LVGL表示処理
	lv_task_handler();
	// キー入力判定だけする(ボタンが押したらLED光るをやるため)
	common_cls.key_read(); // 現在のキーの状態を取得
	int i;
	for (i=0; i<key_input_length; i++) {
		if (common_cls.input_key_last[i] == common_cls.input_key[i]) continue;
		if (common_cls.input_key[i]) {
			// キーが押された
			rgb_led_cls.set_led_buf(i, 1); // LED に押したよを送る
			sound_cls.daken_down(i); // サウンドクラスに押したよを送る
		} else {
			// キーは離された
			rgb_led_cls.set_led_buf(i, 0); // LED に離したよを送る
		}
	}
	common_cls.key_old_copy(); // 現在のキーの状態を前回部分にコピー
	vTaskDelay(10);
}

