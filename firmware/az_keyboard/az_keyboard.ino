// 環境の作り方
// https://raspberrypi.mongonta.com/howto-start-m5stack-core2arduinoide/

#include "az_common.h"
#include "az_keyboard.h"
#include "az_setting.h"


// 起動モード
int boot_type;

// 共通クラス
AzCommon common_cls = AzCommon();

// キーボードモードクラス
AzKeyboard azkb = AzKeyboard();

// 設定モードクラス
AzSetting azstg = AzSetting();

void setup() {
    int i, j, s;
    // 共通処理の初期化
    common_cls.common_start();
    // eepromからデータ読み込み
    common_cls.load_data();
    // ディスプレイ初期化
    disp = new Display();
    disp->begin();
    // キーボードが選択されていなければキーボード選択画面表示
    if (strlen(eep_data.keyboard_type) == 0) {
        disp->view_keyboard_setting(); // キーボード選択画面表示
        mouse_pad_status = -1;
        while (true) {
            disp->loop_exec(); // 表示用ループ処理
            common_cls.restart_loop(); // リスタート用ループ処理
        }
    }
    // 設定jsonの読み込み
    common_cls.load_setting_json();
    // RGB_LEDクラス初期化
    if (rgb_pin > 0 && matrix_row > 0 && matrix_col > 0) {
        rgb_led_cls.begin( rgb_pin, matrix_row, matrix_col, &select_layer_no, led_num, key_matrix);
    }
    // キーの入力ピンの初期化
    common_cls.pin_setup();
    // 起動回数を読み込み
    common_cls.load_boot_count();
    // 打鍵数を自動保存するかどうかの設定を読み込み
    key_count_auto_save = 0;
    common_cls.load_file_data(KEY_COUNT_AUTO_SAVE_PATH, (uint8_t *)&key_count_auto_save, 1);
    ESP_LOGD(LOG_TAG, "boot_mode = %D\r\n", eep_data.boot_mode);
    boot_type = eep_data.boot_mode;
    // 0番目のキーが押されていたら設定モードにする
    common_cls.key_read(); // キーの状態を取得
    // 今選択してるレイヤーをデフォルトに
    select_layer_no = default_layer_no;
    if (boot_type == 1) {
        ESP_LOGD(LOG_TAG, "設定モード起動\n");
        // 設定モード開始
        azstg.start_setting();
        // 次回起動時キーボード起動するようモード設定
        common_cls.set_boot_mode(0);

    } else {
        ESP_LOGD(LOG_TAG, "キーボードモード起動\n");
        if (common_cls.on_tft_unit() && op_movie_flag) {
            disp->open_movie();
        }
        // キーボードとして起動
        azkb.start_keyboard();
    }
}


// ループ処理本体
void loop() {
    if (boot_type == 1) {
      // 設定モード用ループ
      azstg.loop_exec();

    } else {
      // キーボードモード用ループ
      azkb.loop_exec();
    }
}
