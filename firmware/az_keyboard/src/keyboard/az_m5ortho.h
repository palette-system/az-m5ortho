// AZ-M5ortho 用設定

// 初期起動時のデフォルト設定
#include "setting_m5ortho_default.h"
#include "../image/common.h"
#include "../image/az_m5ortho_img.h"

// キーボードの定義
#define KEYBOARD_AZM5ORTHO

// ESP32の種類 (0 = ESP32 / 1 = ESP32-WROVER)
#define  AZESP32_TYPE  1

// M5Stackの画面の向き(0 = 縦長 / 1 = 横長)
#define  DISPLAY_ROTATION  0

// メモリに保持するキーの数(メモリを確保するサイズ)
#define KEY_INPUT_MAX  55

// レイヤー切り替え同時押し許容数
#define PRESS_KEY_MAX 16

// マウス移動ボタン同時押し許容数
#define PRESS_MOUSE_MAX 4

// WIFIアクセスポイントの名前
#define WIFI_AP_SSI_NAME    "AZ-M5ortho"

// Bluetooth で検索した時表示される名前
#define BLUETOOTH_SEARCH_NAME    "az-m5ortho"

// EEPROM 読み込み時のサイズ
#define EEPROM_BUF_SIZE   256

// WEBフック用のバッファサイズ
#define WEBFOOK_BUF_SIZE 512

// 設定JSONのバッファサイズ
#define SETTING_JSON_BUF_SIZE 131072

// 設定モードに入るためのキー番号
#define SETTING_MODE_KEY 11

// 暗記ボタンで暗記できる数
#define ANKEY_DATA_MAX_LENGTH  32
