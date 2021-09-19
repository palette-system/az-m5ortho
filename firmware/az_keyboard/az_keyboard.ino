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
    // 設定jsonの読み込み
    common_cls.load_setting_json();
    // ステータス表示用のLED初期化
    if (status_pin >= 0 && option_type_int != 3) {
        pinMode(status_pin, OUTPUT);
        digitalWrite(status_pin, 0);
        status_led_mode = 0;
        common_cls.set_status_led_timer();
    }
    // ディスプレイ初期化
    disp = new Display();
    disp->begin(option_type_int);
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
    if (common_cls.input_key[SETTING_MODE_KEY]) boot_type = 1;
    if (common_cls.input_key[42]) boot_type = 1;
    if (common_cls.input_key[48]) boot_type = 1;
    // 0番目と3番目と4番目のキーが押されていて、1番目と2番目は離されていたら初期化（１，２が離されているのを確認する事で上に物が乗ってて全押しされて誤って初期化されないようにしている）
    if (common_cls.input_key[0] && !common_cls.input_key[1] && !common_cls.input_key[2] && common_cls.input_key[3] && common_cls.input_key[4]) {
        // 全てのファイルを削除
        common_cls.delete_all();
        // ディスプレイに初期化完了の表示
        if (common_cls.on_tft_unit()) {
            disp->view_setting_init_comp();
        }
        // キーが離されたら終了して再起動
        while (true) {
            common_cls.key_read(); // キーの状態を取得
            // 0～4全てのキーが離されたら終了
            if (!common_cls.input_key[0] && !common_cls.input_key[1] && !common_cls.input_key[2] && !common_cls.input_key[3] && !common_cls.input_key[4]) break;
            delay(50);
        }
        // 画面を真っ暗にする
        if (common_cls.on_tft_unit()) {
            disp->view_full_black();
        }
        // キーボードモードに切り替え
        common_cls.change_mode(0);
        return;
    }
    // 4～5番目のキーが押されていたらデフォルトレイヤーを切り替える
    common_cls.set_default_layer_no();
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
