
#include "oled.h"


// 初期処理
Oled::Oled() {
    this->_screen_w = 128;
    this->_screen_h = 32;
    this->_screen_addr = 0x3C;
}

// 初期処理
void Oled::begin(short scr_w, short scr_h, uint8_t scr_addr, int khz) {
    this->_screen_w = scr_w;
    this->_screen_h = scr_h;
    this->_screen_addr = scr_addr;
    this->display = new Adafruit_SSD1306(this->_screen_w, this->_screen_h, &Wire, OLED_RESET_PIN, khz);
    this->display->begin(SSD1306_SWITCHCAPVCC, this->_screen_addr); 
    this->display->setTextColor(WHITE);
    this->display->setTextSize(1);
    this->default_image_load_flag = -1;
    // デフォルト画面の表示
    this->view_default();
}

//デフォルトの画面を表示する
void Oled::view_default() {
    // 画面のクリア
    display->clearDisplay();
    // デフォルト画像が登録されていなければ文字だけ出す
    if (!SPIFFS.exists(OLED_DISP_DEF_PATH)) {
        this->display->setTextSize(2);
        display->setCursor(0,8);
        display->printf("AZKeyboard");
        display->display();
        return;
    }
    // デフォルト画像があればデフォルト画像読み込み
    if (this->default_image_load_flag < 0) {
        // view_default 初実行
        File fp;
        fp = SPIFFS.open(OLED_DISP_DEF_PATH, "r");
        if(!fp){
            this->display->setTextSize(1);
            display->setCursor(0,0);
            display->printf("Image load Error");
            display->display();
            this->default_image_load_flag = 0;
            return;
        }
        int load_size = fp.size();
        if (fp.available() && load_size > 0) {
            this->default_image = (uint8_t *)malloc(load_size);
            fp.read(this->default_image, load_size);
        }
        fp.close();
        this->default_image_load_flag = 1; // ステータスを画像読み込み済みにする
    } else if (this->default_image_load_flag == 0) {
        // 1回ロードしようとしたけど読み込めなかった
        this->display->setTextSize(1);
        display->setCursor(0,0);
        display->printf("Image load Error");
        display->display();
        return;
    }
    // 画像を表示
    display->drawBitmap(0, 0, this->default_image, this->_screen_w, this->_screen_h,  WHITE);
    display->display();
}
