#include "Arduino.h"
#include "display.h"

#include "../../az_common.h"
#include "../image/gimp_image.h"
#include "../image/tanoshii.h"
#include "../image/wificonn1.h"
#include "../image/wificonn2.h"





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
//    this->_tft->fillScreen(BLACK);
}


// 起動ムービー(AZ-Macro用)
#ifdef KEYBOARD_AZMACRO
void Display::open_movie() {
}
// 設定モード画面表示
void Display::view_setting_mode() {
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
	if (this->_stimg_load_flag) {
		// SPRAMにデータロード済みならSPRAMに入ってる待ち受け画像を表示
//		this->_tft->viewBMPspi_head(0, 0, 135, 240);
//		this->_tft->viewBMPspi_data(this->_stimg_data, 64800);
	} else if (ESP.getFreePsram() > 64800) {
		// SPRAMに空き容量があれば待ち受け画像をSPRAMにロード
		this->_stimg_data = (uint8_t *)ps_malloc(64800);
		if(SPIFFS.exists("/stimg.dat")){
			File fp = SPIFFS.open("/stimg.dat", "r");
			int s = fp.read(this->_stimg_data, 64800);
			fp.close();
		}
//		this->_tft->viewBMPspi_head(0, 0, 135, 240);
//		this->_tft->viewBMPspi_data(this->_stimg_data, 64800);
	} else {
		// SPRAMに空きが無ければファイルから直接表示
//	    this->_tft->viewBMPFile(0,0, 135, 240, "/stimg.dat", 0);
	}
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

#endif




// 定期処理
void Display::loop_exec() {
    unsigned long n;
    n = millis();
    // this->_tft->fillRect(0, 0,  240, 50, BLACK);
    // this->_tft->setCursor(4, 4);
    // this->_tft->setTextSize(2);
    // this->_tft->printf("%D / %D / %D\n", getXtalFrequencyMhz(), getCpuFrequencyMhz(), getApbFrequency());
    // this->_tft->printf("%D / %D\n", ESP.getHeapSize(), ESP.getFreeHeap());
    // this->_tft->printf("%D / %D\n", ESP.getPsramSize(), ESP.getFreePsram());
    // this->_tft->printf("%D / %D\n", ESP.getFlashChipSize(), ESP.getFlashChipSpeed());
	// 待ちIndexがあれば待ち時間終わるまで画面の変更なし
	// if (this->_wait_index && rgb_led_cls.setting_change == 0) {
	// 	this->_wait_index--;
	// 	return;
	// }
	if (this->_wait_index) this->_wait_index--;
	if (this->_info_index) this->_info_index--;
	// 最後に表示したタイプとこれから表示しようとしている物が変わったら、最後に表示したタイプをbackに保持しておく
	if (this->view_type != this->_last_view_type) {
		this->_back_view_type = this->_last_view_type;
	}
	// 各表示処理
	if (rgb_led_cls.setting_change == 1) {
		this->view_led_stat();  // LEDステータス設定変更があった

	} else if (rgb_led_cls.setting_change == 2) {
		this->view_led_bright();  // LED明るさ設定変更があった
	
	} else if (rgb_led_cls.setting_change == 3) {
		this->view_led_color();  // LED色設定変更があった
	
	} else if (rgb_led_cls.setting_change == 4) {
		this->view_led_shine();  // LED光らせ方設定変更があった

	} else if (this->view_type == DISP_TYPE_DKQRCOD) {
		// 打鍵QRコード
		this->view_dakagi_qr();
		// info表示を終わる時にQRコードを表示しなおしたいので表示している内容を何もなしにする
		if (this->_wait_index == 1) this->_last_view_type = 255;

	} else if (this->view_type == DISP_TYPE_DKTHERM) {
		// 打鍵サーモ(最終表示がサーモでもまた実行する)
		this->view_dakagi_thermo();
		// 打鍵表示
		if (this->_wait_index == 0) this->view_dakagi();

	} else if (this->view_type == DISP_TYPE_STANDBY) {
		// 打鍵表示があるから最終表示が待ち受けでもまた実行する
		// 待ち受け画像
		this->view_standby_image();
		// 打鍵表示
		if (this->_wait_index == 0) {
			if (this->dakagi_last_view != common_cls.key_count_total && common_cls.key_count_total != 0) {
				// 打鍵数が増えたら打鍵数表示
				this->view_dakagi();
				this->_info_index = 150;
			} else if (this->_info_index == 1) {
				// info表示を終わる時に待ち受け画像を表示しなおしたいので表示してる内容を何もなしにする
				this->_info_spot = 1;
				this->_last_view_type = 255;
			}
		}

	} else if (this->view_type == this->_last_view_type) {
		// 最後に表示した内容と一緒であれば何もしない

	} else if (this->view_type == DISP_TYPE_SETTING) {
		// 設定画面
		this->view_setting_mode();

	} else if (this->view_type == DISP_TYPE_SAVENOW) {
		// 保存中画面
		this->view_save();

	} else if (this->view_type == DISP_TYPE_WIFICNN) {
		// Wifi接続中
		this->view_wifi_conn();

	} else if (this->view_type == DISP_TYPE_WEBFOOK) {
		// WEBフック中
		this->view_webhook();

	} else if (this->view_type == DISP_TYPE_ANKYNOW) {
		// 暗記中画面
		this->view_ankey_now();


	}
	
}

