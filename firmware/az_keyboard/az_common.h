#ifndef AzCommon_h
#define AzCommon_h

#include <M5Core2.h>
#include <Arduino.h>
#include <lvgl.h>
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_MCP23X17.h>

#include "FS.h"
#include "SPIFFS.h"


#include <ArduinoJson.h>
#include <WiFi.h>
#include <WiFiMulti.h>
#include <HTTPClient.h>
#include <Adafruit_NeoPixel.h>
#include  <AXP192.h>

#include "src/lib/oled.h"
#include "src/lib/neopixel.h"
#include "src/lib/wirelib.h"
#include "src/lib/display.h"
#include "src/lib/sound.h"
#include "src/lib/HTTPClient_my.h"
#include "src/lib/ble_keyboard_jis.h"

#include "az_keyboard.h"

// キーボード別のデフォルト設定ファイル
#include "src/keyboard/setting_emugotch_default.h"
#include "src/keyboard/setting_m5ortho_default.h"
#include "src/keyboard/setting_m5orthow_default.h"


#if defined(CONFIG_ARDUHAL_ESP_LOG)
  #include "esp32-hal-log.h"
  #define LOG_TAG ""
#else
  #include "esp_log.h"
  static const char* LOG_TAG = "AZM";
#endif


// ファームウェアのバージョン文字
#define FIRMWARE_VERSION   "000222"

// EEPROMに保存しているデータのバージョン文字列
#define EEP_DATA_VERSION    "AZM024"

// vid pid のデフォルト値
#define BLE_HID_VID  0xE502
#define BLE_HID_PID  0xA111

// キースキャンループの待ち時間デフォルト(ms)
#define LOOP_DELAY_DEFAULT  5

// JSON のファイルパス
#define SETTING_JSON_PATH "/setting.json"

// 起動回数を保存するファイルのパス
#define  BOOT_COUNT_PATH  "/boot_count"

// システム情報を保存するファイルのパス
#define  AZ_SYSTEM_FILE_PATH  "/sys_data"

// 打鍵数を自動保存するかどうかの設定を保存するファイルパス
#define  KEY_COUNT_AUTO_SAVE_PATH  "/key_count_auto_save"

// マウスパッドの設定を保存するファイルのパス
#define  MOUSE_PAD_SETTING_PATH  "/mousepad_data"

// 画面設定を保存するファイルのパス
#define  MONITER_SETTING_PATH  "/moniter_data"

// BLE用のMACアドレスを保存するファイルのパス
#define  BLEMAC_ADDR_PATH  "/blemac"

// レイヤー切り替え同時押し許容数
#define PRESS_KEY_MAX 8

// マウス移動ボタン同時押し許容数
#define PRESS_MOUSE_MAX 4

// WEBフック用のバッファサイズ
#define WEBFOOK_BUF_SIZE 512

// 設定JSONのバッファサイズ
#define SETTING_JSON_BUF_SIZE 131072

// 暗記ボタンで暗記できる数
#define ANKEY_DATA_MAX_LENGTH  32

// アクチュエーションタイプデフォルト
#define  ACTUATION_TYPE_DEFAULT  0

// アクチュエーションポイントデフォルト
#define  ACTUATION_POINT_DEFAULT  160

// ラピットトリガーデフォルト
#define  RAPID_TRIGGER_DEFAULT  120

// ホールセンサーのアナログ値読み取り範囲デフォルト
#define  HALL_RANGE_MIN_DEFAULT -50;
#define  HALL_RANGE_MAX_DEFAULT 1200;


// 今押されているボタンの情報
struct press_key_data {
    short action_type; // キーの動作タイプ 0=設定なし / 1=通常入力 / 2=テキスト入力 / 3=レイヤー変更 / 4=WEBフック
    short layer_id; // キーを押した時のレイヤーID
    short key_num; // キー番号
    short press_type; // 入力タイプ(0=press / 1=sub_press)
    short key_id; // 送信した文字
    short press_time; // キーを押してからどれくらい経ったか
    short unpress_time; // キーを離してからどれくらい経ったか
    short repeat_interval; // 連打の間隔
    short repeat_index; // 現在の連打カウント
};

// 今押されているマウスボタン情報
struct press_mouse_data {
    short key_num; // キー番号
    short action_type; // 動作のタイプ(5=マウス移動 / 10=アナログマウス移動)
    short move_x; // X座標
    short move_y; // Y座標
    short move_wheel; // 縦ホイール
    short move_hWheel; // 横ホイール
    short move_speed; // 移動速度
    short move_index; // 移動index
};

// EEPROMに保存するデータ
struct mrom_data_set {
    char check[10];
    char keyboard_type[16];
    int boot_mode; // 起動モード 0=キーボード / 1=設定モード
    char uid[12];
};

// マウスパッド設定
struct mousepad_data_set {
    int8_t mouse_type; // マウス操作のタイプ 0=無し / 1=タッチパッド / 2=ジョイスティック
    uint8_t mouse_speed; // 感度 0 - 100
    uint8_t value_1;
    uint8_t value_2;
};

// 画面設定
struct moniterset_data_set {
    int8_t st_brightness;
    int8_t menu_brightness;
    uint8_t value_1;
    uint8_t value_2;
};

// BLE 用macアドレス
struct ble_mac_addr {
    uint8_t addr[6];
    char *name;
};


// ArduinoJSON SPRAM用の定義
struct SpiRamAllocator {
  void* allocate(size_t size) {
    return heap_caps_malloc(size, MALLOC_CAP_SPIRAM);
  }
  void deallocate(void* pointer) {
    heap_caps_free(pointer);
  }
  void* reallocate(void* ptr, size_t new_size) {
    return heap_caps_realloc(ptr, new_size, MALLOC_CAP_SPIRAM);
  }
};
using SpiRamJsonDocument = BasicJsonDocument<SpiRamAllocator>;


// 通常キー入力(キー設定用)
struct setting_normal_input {
    uint8_t key_length; // 登録しているキーコードの数
    uint16_t *key; // キーコードの配列
    uint16_t hold; // 長押し設定
    short repeat_interval; // 連打用設定
};

// マウス移動(キー設定用)
struct setting_mouse_move {
    int16_t x;
    int16_t y;
    int16_t wheel;
    int16_t hWheel;
    int16_t speed;
};

// レイヤー切り替え(キー設定用)
struct setting_layer_move {
    int8_t layer_id; // 切り替え先のレイヤーID
    int8_t layer_type; // 切り替えのタイプ
};

// キーを押した時の設定
struct setting_key_press {
    short layer; // どのレイヤーか
    short key_num; // どのキーか
    char *key_name; // キーの名前
    uint8_t actuation_type; // アクチュエーションタイプ
    uint8_t actuation_point; // アクチュエーションポイント
    uint8_t rapid_trigger; // ラピットトリガー
    short action_type; // 入力するタイプ
    short data_size; // データのサイズ
    char *data; // 入力データ
    bool sub_press_flag; // サブ入力データの有無
    void *sub_press; // サブ入力用データ
};


// IOエキスパンダオプションの設定
struct ioxp_option {
    uint8_t addr; // IOエキスパンダのアドレス
    uint8_t *row; // row のピン
    uint16_t row_mask; // row output する時に使う全ROWのOR
    uint16_t *row_output; // row output する時のピンにwriteするデータ
    uint8_t row_len;
    uint8_t *col;
    uint8_t col_len;
    uint8_t *direct;
    uint8_t direct_len;
};

// I2Cオプション　マップ設定
struct i2c_map {
    short map_start; // キー設定の番号開始番号
    short *map; // キーとして読み取る番号の配列
    uint8_t map_len; // マッピング設定の数
};

// I2Cオプション　IOエキスパンダ（MCP23017
struct i2c_ioxp {
    ioxp_option *ioxp; // 使用するIOエキスパンダの設定
    uint8_t ioxp_len; // IOエキスパンダ設定の数
};

// I2Cオプション　Tiny202　ロータリエンコーダ
struct i2c_rotary {
    uint8_t *rotary; // ロータリーエンコーダのアドレス
    uint8_t rotary_len; // ロータリーエンコーダの数
};

// I2Cオプション　1Uトラックボール　PIM447
struct i2c_pim447 {
    uint8_t addr; // PIM447のアドレス
    short speed; // マウス移動速度
    uint8_t rotate; // マウスの向き
};

// I2Cオプション AZ-Expander 
struct i2c_azxp {
    uint8_t setting[18]; // AZ-Expanderに送る設定データ
    azxp_key_info key_info; // キー読み込みのバイト数とか
};

// i2cオプションの設定
struct i2c_option {
    uint8_t opt_type; // オプションのタイプ 1: ioエキスパンダキーボード
    uint8_t *data;
    i2c_map *i2cmap;
};

// レイヤーの名前データ保持用
struct layer_name_data {
    short layer; // レイヤーのID
    char *layer_name; // レイヤーの登録名
};


// WIFI設定
struct azsetting_wifi {
    char *ssid;
    char *pass;
};

// クラスの定義
class AzCommon
{
    public:
        AzCommon();   // コンストラクタ
        void common_start(); // 共通処理の初期処理(setup時に呼ばれる)
        void restart_loop(); // リスタート用ループ処理
        int split(String data, char delimiter, String *dst); // 区切り文字で分割する
        void wifi_connect(); // WIFI接続
        String get_wifi_ap_list_json(); // wifiアクセスポイントのリストをJSONで取得
        void get_domain(char *url, char *domain_name); // URLからドメイン名だけ取得
        String send_webhook_simple(char *url); // 単純なGETリクエストのWEBフック
        String send_webhook_post_file(char *url, char *file_path); // POSTでファイルの内容を送信する
        String send_webhook(char *setting_data); // httpかhttpsか判断してリクエストを送信する
        String http_request(char *url, const JsonObject &prm); // httpリクエスト送信
        bool create_setting_json(); // デフォルトの設定json作成
        void load_setting_json(); // jsonデータロード
        void clear_keymap(); // キーマップ用に確保しているメモリを解放
        void get_keymap(JsonObject setting_obj, char *key_name); // JSONデータからキーマップの情報を読み込む
        void get_keymap_one(JsonObject json_obj, setting_key_press *press_obj, uint16_t lnum, uint16_t knum); // JSONデータからキーマップの情報を読み込む(1キー分)
        void get_keyboard_type_int(String t); // キーボードのタイプ番号を取得
        int read_file(char *file_path, String &read_data); // ファイルからデータを読み出す
        int write_file(char *file_path, String &write_data); // ファイルにデータを保存する
        int remove_file(char *file_path); // ファイルを削除する
        int mouse_pad_load(); // マウスパッド設定読み込み
        int mouse_pad_save(); // マウスパッド設定保存
        int moniterset_load(); // 画面設定読み込み
        int moniterset_save(); // 画面設定保存
        bool addrcmp(uint8_t *addr1, uint8_t *addr2); // アドレス１とアドレス２が同じかどうかチェック
        void blemac_load(); // BLE用macアドレスリスト読み込み
        void blemac_add(uint8_t *addr); // BLE用macアドレス追加
        void moniter_brightness(int set_type); // 画面の明るさ設定
        int i2c_setup(int p, i2c_option *opt); // IOエキスパンダの初期化(戻り値：増えるキーの数)
        void pin_setup(); // キーの入力ピンの初期化
        bool layers_exists(int layer_no); // レイヤーが存在するか確認
        void layer_set(int layer_no); // 現在のレイヤーを指定したレイヤーにする
        setting_key_press get_key_setting(int layer_id, int key_num, short press_type); // 指定したキーの入力設定を取得する
        setting_key_press get_soft_key_setting(int layer_id, int key_num); // 指定したキーの入力設定を取得する
        void load_data(); // EEPROMからデータをロードする
        void save_data(); // EEPROMに保存する
        void load_boot_count(); // 起動回数を取得してカウントアップする
        void load_file_data(char *file_path, uint8_t *load_point, uint16_t load_size); // ファイルから設定値を読み込み
        void save_file_data(char *file_path, uint8_t *save_point, uint16_t save_size); // ファイルに設定値を書込み
        void set_boot_mode(int set_mode); // 起動モードを切り替えてEEPROMに保存
        void change_mode(int set_mode); // モードを切り替えて再起動
        int i2c_read(int p, i2c_option *opt, char *read_data); // I2C機器のキー状態を取得
        void key_read(); // 現在のキーの状態を取得
        void key_old_copy(); // 現在のキーの状態を過去用配列にコピー
        uint8_t *input_key_analog; // 今入力中のアナログ値
        char *analog_stroke_most; // 最も押し込んだ時のアナログ値
        char *input_key; // 今入力中のキー(ステータス)
        char *input_key_last; // 最後にチェックした入力中のキー(ステータス)
        short *key_point; // 入力キーに該当する設定が何番目に入っているか
        uint16_t *key_count; // キー別の打鍵した数
        uint16_t key_count_total;
        void delete_all(void); // 全てのファイルを削除
        void delete_indexof_all(String check_str); // 指定した文字から始まるファイルすべて削除
        int spiffs_total(void); // ファイル領域合計サイズを取得
        int spiffs_used(void); // 使用しているファイル領域サイズを取得
        void press_mouse_list_clean(); // マウス移動中リストを空にする
        void press_mouse_list_push(int key_num, short action_type, short move_x, short move_y, short move_wheel, short move_hWheel, short move_speed); // マウス移動中リストに追加
        void press_mouse_list_remove(int key_num); // マウス移動中リストから削除
    
    private:

};

// hid
extern uint16_t hid_vid;
extern uint16_t hid_pid;

// ステータスLED今0-9
extern int status_led_bit;

// ステータスLED表示モード
extern int status_led_mode;

// キーボードのステータス
extern int8_t keyboard_status;

// IOエキスパンダオブジェクト
extern Adafruit_MCP23X17 *ioxp_obj[8];

// 入力用ピン情報
extern short col_len;
extern short row_len;
extern short direct_len;
extern short touch_len;
extern short hall_len;
extern short *col_list;
extern short *row_list;
extern short *direct_list;
extern short *touch_list;
extern short *hall_list;
extern short *hall_offset;

extern short ioxp_sda;
extern short ioxp_scl;
extern int ioxp_hz;
extern short ioxp_status[8];
extern int ioxp_hash[8];

extern short ioxp_len;
extern short *ioxp_list;

// I2Cオプションの設定
extern i2c_option *i2copt;
extern short i2copt_len;

// 液晶表示用オブジェクト
extern Display *disp;

// rgb_led制御用クラス
extern Neopixel rgb_led_cls;

// I2Cライブラリ用クラス
extern Wirelib wirelib_cls;

// サウンド制御用クラス
extern Sound sound_cls;

//timer オブジェクト
extern hw_timer_t *timer;

// WIFI接続オブジェクト
extern WiFiMulti wifiMulti;

// WIFI接続フラグ
extern int wifi_conn_flag;

// http用のバッファ
extern char webhook_buf[WEBFOOK_BUF_SIZE];

// 入力キーの数
extern int key_input_length;

// キースキャンループの待ち時間
extern short loop_delay;

// ディスプレイの向き
extern uint8_t disp_rotation;

// キーボードタイプの番号
extern int keyboard_type_int;

// キーボードの名前
extern char keyboard_name_str[32];

// キーボードの言語(日本語=0/ US=1 / 日本語(US記号) = 2)
extern uint8_t keyboard_language;

// トラックボールの方向
extern uint8_t trackball_direction;

// トラックボールのカーソル移動速度
extern uint8_t trackball_speed;

// オープニングムービー再生フラグ
extern bool op_movie_flag;

// タップした位置
extern short start_touch_x;
extern short start_touch_y;
extern short last_touch_x;
extern short last_touch_y;
extern short touch_move_x;
extern short touch_move_y;
extern short touch_send_count;
extern short touch_send_index;
extern short last_touch_index;


// デフォルトのレイヤー番号と、今選択しているレイヤー番号
extern int default_layer_no;
extern int select_layer_no;
extern int last_select_layer_key; // レイヤーボタン最後に押されたボタン(これが離されたらレイヤーリセット)

// ホールセンサーの範囲
extern short hall_range_min;
extern short hall_range_max;

// holdの設定
extern uint8_t hold_type;
extern uint8_t hold_time;

// display 初期化フラグ
extern bool disp_enable;

// 押している最中のキーデータ
extern press_key_data press_key_list[PRESS_KEY_MAX];

// 押している最中のマウス移動
extern press_mouse_data press_mouse_list[PRESS_MOUSE_MAX];

// マウスのスクロールボタンが押されているか
extern bool mouse_scroll_flag;

// オールクリア送信フラグ
extern int press_key_all_clear;

// EEPROMデータリンク
extern mrom_data_set eep_data;

// 起動回数
extern uint32_t boot_count;

// 打鍵数を自動保存するかどうか
extern uint8_t key_count_auto_save;

// 共通クラスリンク
extern AzCommon common_cls;

// キーボードモードクラス
extern AzKeyboard azkb;

// BLEクラス
extern BleKeyboardJIS bleKeyboard;

// 設定JSONオブジェクト
extern JsonObject setting_obj;

// バッテリーオブジェクト
extern AXP192 power;

// バッテリ残量更新フラグ
extern int pw_update_index;

// キーが押された時の設定
extern uint16_t setting_length;
extern setting_key_press *setting_press;

// ソフトキーが押された時の設定
extern uint16_t soft_setting_length;
extern setting_key_press *soft_setting_press;

// ソフトキーが押された時入る変数(押されてない間は-1)
extern int16_t soft_click_layer;
extern int16_t soft_click_key;

// レイヤー名のデータ
extern layer_name_data *layer_name_list;
extern uint16_t layer_name_length;
extern layer_name_data *soft_layer_name_list;
extern uint16_t soft_layer_name_length;

// リスタート用のフラグ
extern int8_t restart_flag;
extern int8_t restart_index;

// 設定メニューを表示しているかどうか
extern bool menu_mode_flag;

// aztoolで設定中かどうか
extern uint8_t aztool_mode_flag;

// wifi設定
extern uint8_t wifi_data_length;
extern azsetting_wifi *wifi_data;

// アクセスポイントパスワード
extern char *ap_pass_char;

// マウスパッドステータス
extern int8_t mouse_pad_status;

// マウスパッド操作方法設定
extern mousepad_data_set mouse_pad_setting;

// 画面設定
extern moniterset_data_set moniter_setting;

// BLE 用macアドレスリスト
extern ble_mac_addr *blemac_list;

// BLE macアドレス(現在どれを利用しているか、何件あるか)
extern uint8_t blemac_index;
extern uint8_t blemac_len;

// BLE 新しいmacアドレス取得用ステータス
extern uint8_t blemac_stat;
extern uint8_t macaddr_new[6];

// RGBLED
extern int8_t rgb_pin;
extern int16_t rgb_len;
extern int8_t matrix_row;
extern int8_t matrix_col;
extern int8_t *led_num;
extern int8_t *key_matrix;
extern uint8_t led_num_length;
extern uint8_t key_matrix_length;

// LVGL用
extern TFT_eSPI lvtft;
extern lv_disp_buf_t disp_buf;
extern lv_color_t lvbuf[LV_HOR_RES_MAX * 10];
extern int8_t lvgl_loop_index;

// ハッシュ値計算用
int azcrc32(uint8_t* d, int len);


#endif
