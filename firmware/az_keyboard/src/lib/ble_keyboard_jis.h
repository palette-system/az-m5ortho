#ifndef BleKeyboardJIS_h
#define BleKeyboardJIS_h
#include "sdkconfig.h"
#if defined(CONFIG_BT_ENABLED)

#include "nimconfig.h"
#if defined(CONFIG_BT_NIMBLE_ROLE_PERIPHERAL)

#include "Arduino.h"
#include "NimBLEHIDDevice.h"
#include "NimBLECharacteristic.h"

#include "ble_callbacks.h"
#include "../../az_common.h"


#define JIS_SHIFT 0x1000
#define MOUSE_CODE 0x2000
#define MOUSE_CODE 0x4000

// マウスボタン
#define MOUSE_BUTTON_LEFT  0x01
#define MOUSE_BUTTON_RIGHT  0x02
#define MOUSE_BUTTON_MIDDLE 0x04
#define MOUSE_BUTTON_BACK   0x08


// メディアキー構造
typedef uint8_t MediaKeyReport[2];

// 通常キー構造
typedef struct
{
  uint8_t modifiers;
  uint8_t reserved;
  uint8_t keys[6];
} KeyReport;

// HIDのデバイスID
#define REPORT_KEYBOARD_ID 0x01
#define REPORT_MEDIA_KEYS_ID 0x02
#define REPORT_MOUSE_ID 0x03


// HID レポートデフォルト
const uint8_t _hidReportDescriptorDefault[] PROGMEM = {
  USAGE_PAGE(1),      0x01,          // USAGE_PAGE (Generic Desktop Ctrls)
  USAGE(1),           0x06,          // USAGE (Keyboard)
  COLLECTION(1),      0x01,          // COLLECTION (Application)
  // ------------------------------------------------- Keyboard
  REPORT_ID(1),       REPORT_KEYBOARD_ID,   //   REPORT_ID (1)
  USAGE_PAGE(1),      0x07,          //   USAGE_PAGE (Kbrd/Keypad)
  USAGE_MINIMUM(1),   0xE0,          //   USAGE_MINIMUM (0xE0)
  USAGE_MAXIMUM(1),   0xE7,          //   USAGE_MAXIMUM (0xE7)
  LOGICAL_MINIMUM(1), 0x00,          //   LOGICAL_MINIMUM (0)
  LOGICAL_MAXIMUM(1), 0x01,          //   Logical Maximum (1)
  REPORT_SIZE(1),     0x01,          //   REPORT_SIZE (1)
  REPORT_COUNT(1),    0x08,          //   REPORT_COUNT (8)
  HIDINPUT(1),        0x02,          //   INPUT (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
  REPORT_COUNT(1),    0x01,          //   REPORT_COUNT (1) ; 1 byte (Reserved)
  REPORT_SIZE(1),     0x08,          //   REPORT_SIZE (8)
  HIDINPUT(1),        0x01,          //   INPUT (Const,Array,Abs,No Wrap,Linear,Preferred State,No Null Position)
  REPORT_COUNT(1),    0x05,          //   REPORT_COUNT (5) ; 5 bits (Num lock, Caps lock, Scroll lock, Compose, Kana)
  REPORT_SIZE(1),     0x01,          //   REPORT_SIZE (1)
  USAGE_PAGE(1),      0x08,          //   USAGE_PAGE (LEDs)
  USAGE_MINIMUM(1),   0x01,          //   USAGE_MINIMUM (0x01) ; Num Lock
  USAGE_MAXIMUM(1),   0x05,          //   USAGE_MAXIMUM (0x05) ; Kana
  HIDOUTPUT(1),       0x02,          //   OUTPUT (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
  REPORT_COUNT(1),    0x01,          //   REPORT_COUNT (1) ; 3 bits (Padding)
  REPORT_SIZE(1),     0x03,          //   REPORT_SIZE (3)
  HIDOUTPUT(1),       0x01,          //   OUTPUT (Const,Array,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
  REPORT_COUNT(1),    0x06,          //   REPORT_COUNT (6) ; 6 bytes (Keys)
  REPORT_SIZE(1),     0x08,          //   REPORT_SIZE(8)
  LOGICAL_MINIMUM(1), 0x00,          //   LOGICAL_MINIMUM(0)
  LOGICAL_MAXIMUM(1), 0xEF,          //   LOGICAL_MAXIMUM(0x65) ; 101 keys
  USAGE_PAGE(1),      0x07,          //   USAGE_PAGE (Kbrd/Keypad)
  USAGE_MINIMUM(1),   0x00,          //   USAGE_MINIMUM (0)
  USAGE_MAXIMUM(1),   0xEF,          //   USAGE_MAXIMUM (0x65)
  HIDINPUT(1),        0x00,          //   INPUT (Data,Array,Abs,No Wrap,Linear,Preferred State,No Null Position)
  END_COLLECTION(0),                 // END_COLLECTION
  // ------------------------------------------------- Media Keys
  USAGE_PAGE(1),      0x0C,          // USAGE_PAGE (Consumer)
  USAGE(1),           0x01,          // USAGE (Consumer Control)
  COLLECTION(1),      0x01,          // COLLECTION (Application)
  REPORT_ID(1),       REPORT_MEDIA_KEYS_ID, //   REPORT_ID (3)
  USAGE_PAGE(1),      0x0C,          //   USAGE_PAGE (Consumer)
  LOGICAL_MINIMUM(1), 0x00,          //   LOGICAL_MINIMUM (0)
  LOGICAL_MAXIMUM(1), 0x01,          //   LOGICAL_MAXIMUM (1)
  REPORT_SIZE(1),     0x01,          //   REPORT_SIZE (1)
  REPORT_COUNT(1),    0x10,          //   REPORT_COUNT (16)
  USAGE(1),           0xB8,          //   USAGE (Eject)     ; bit 0: 1
  USAGE(1),           0xB5,          //   USAGE (Scan Next Track)     ; bit 0: 2
  USAGE(1),           0xB6,          //   USAGE (Scan Previous Track) ; bit 1: 4
  USAGE(1),           0xB7,          //   USAGE (Stop)                ; bit 2: 8
  USAGE(1),           0xCD,          //   USAGE (Play/Pause)          ; bit 3: 16
  USAGE(1),           0xE2,          //   USAGE (Mute)                ; bit 4: 32
  USAGE(1),           0xE9,          //   USAGE (Volume Increment)    ; bit 5: 64
  USAGE(1),           0xEA,          //   USAGE (Volume Decrement)    ; bit 6: 128
  USAGE(2),           0x94, 0x01,    //   Usage (My Computer) ; bit 0: 1
  USAGE(2),           0x92, 0x01,    //   Usage (Calculator)  ; bit 1: 2
  USAGE(2),           0x2A, 0x02,    //   Usage (WWW fav)     ; bit 2: 4
  USAGE(2),           0x21, 0x02,    //   Usage (WWW search)  ; bit 3: 8
  USAGE(2),           0x26, 0x02,    //   Usage (WWW stop)    ; bit 4: 16
  USAGE(2),           0x24, 0x02,    //   Usage (WWW back)    ; bit 5: 32
  USAGE(2),           0x83, 0x01,    //   Usage (Media sel)   ; bit 6: 64
  USAGE(2),           0x8A, 0x01,    //   Usage (Mail)        ; bit 7: 128
  HIDINPUT(1),        0x02,          //   INPUT (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
  END_COLLECTION(0),                 // END_COLLECTION

  // ------------------------------------------------- Mouse
  USAGE_PAGE(1),       0x01, // USAGE_PAGE (Generic Desktop)
  USAGE(1),            0x02, // USAGE (Mouse)
  COLLECTION(1),       0x01, // COLLECTION (Application)
  USAGE(1),            0x01, //   USAGE (Pointer)
  COLLECTION(1),       0x00, //   COLLECTION (Physical)
  REPORT_ID(1),        REPORT_MOUSE_ID, //     REPORT_ID (1)
  // ------------------------------------------------- Buttons (Left, Right, Middle, Back, Forward)
  USAGE_PAGE(1),       0x09, //     USAGE_PAGE (Button)
  USAGE_MINIMUM(1),    0x01, //     USAGE_MINIMUM (Button 1)
  USAGE_MAXIMUM(1),    0x05, //     USAGE_MAXIMUM (Button 5)
  LOGICAL_MINIMUM(1),  0x00, //     LOGICAL_MINIMUM (0)
  LOGICAL_MAXIMUM(1),  0x01, //     LOGICAL_MAXIMUM (1)
  REPORT_SIZE(1),      0x01, //     REPORT_SIZE (1)
  REPORT_COUNT(1),     0x05, //     REPORT_COUNT (5)
  HIDINPUT(1),         0x02, //     INPUT (Data, Variable, Absolute) ;5 button bits
  // ------------------------------------------------- Padding
  REPORT_SIZE(1),      0x03, //     REPORT_SIZE (3)
  REPORT_COUNT(1),     0x01, //     REPORT_COUNT (1)
  HIDINPUT(1),         0x03, //     INPUT (Constant, Variable, Absolute) ;3 bit padding
  // ------------------------------------------------- X/Y position, Wheel
  USAGE_PAGE(1),       0x01, //     USAGE_PAGE (Generic Desktop)
  USAGE(1),            0x30, //     USAGE (X)
  USAGE(1),            0x31, //     USAGE (Y)
  USAGE(1),            0x38, //     USAGE (Wheel)
  LOGICAL_MINIMUM(1),  0x81, //     LOGICAL_MINIMUM (-127)
  LOGICAL_MAXIMUM(1),  0x7f, //     LOGICAL_MAXIMUM (127)
  REPORT_SIZE(1),      0x08, //     REPORT_SIZE (8)
  REPORT_COUNT(1),     0x03, //     REPORT_COUNT (3)
  HIDINPUT(1),         0x06, //     INPUT (Data, Variable, Relative) ;3 bytes (X,Y,Wheel)
  // ------------------------------------------------- Horizontal wheel
  USAGE_PAGE(1),       0x0c, //     USAGE PAGE (Consumer Devices)
  USAGE(2),      0x38, 0x02, //     USAGE (AC Pan)
  LOGICAL_MINIMUM(1),  0x81, //     LOGICAL_MINIMUM (-127)
  LOGICAL_MAXIMUM(1),  0x7f, //     LOGICAL_MAXIMUM (127)
  REPORT_SIZE(1),      0x08, //     REPORT_SIZE (8)
  REPORT_COUNT(1),     0x01, //     REPORT_COUNT (1)
  HIDINPUT(1),         0x06, //     INPUT (Data, Var, Rel)
  END_COLLECTION(0),         //   END_COLLECTION
  END_COLLECTION(0)          // END_COLLECTION
};

// 
const unsigned short _asciimap_default[] PROGMEM =
{
  // 0x00
  0x00,
  0x00,
  0x00,
  0x00,
  0x00,
  0x00,
  0x00,
  0x00,
  0x00,
  0x00,
  0x28,
  0x00,
  0x00,
  0x00,
  0x00,
  0x00,

  // 0x10
  0x00,
  0x00,
  0x00,
  0x00,
  0x00,
  0x00,
  0x00,
  0x00,
  0x00,
  0x00,
  0x00,
  0x00,
  0x00,
  0x00,
  0x00,
  0x00,

  // 0x20
  0x2c, // ' '
  0x1e | JIS_SHIFT, // !
  0x1f | JIS_SHIFT, // "
  0x20 | JIS_SHIFT, // #
  0x21 | JIS_SHIFT, // $
  0x22 | JIS_SHIFT, // %
  0x23 | JIS_SHIFT, // &
  0x24 | JIS_SHIFT, // '
  0x25 | JIS_SHIFT, // (
  0x26 | JIS_SHIFT, // )
  0x34 | JIS_SHIFT, // *
  0x33 | JIS_SHIFT, // +
  0x36, // ,
  0x2d, // -
  0x37, // .
  0x38, // /

  // 0x30
  0x27, // 0
  0x1e, // 1
  0x1f, // 2
  0x20, // 3
  0x21, // 4
  0x22, // 5
  0x23, // 6
  0x24, // 7
  0x25, // 8
  0x26, // 9
  0x34, // :
  0x33, // ;
  0x36 | JIS_SHIFT, // <
  0x2d | JIS_SHIFT, // =
  0x37 | JIS_SHIFT, // >
  0x38 | JIS_SHIFT, // ?

  // 0x40
  0x2f, // @
  0x04 | JIS_SHIFT, // A
  0x05 | JIS_SHIFT, // B
  0x06 | JIS_SHIFT, // C
  0x07 | JIS_SHIFT, // D
  0x08 | JIS_SHIFT, // E
  0x09 | JIS_SHIFT, // F
  0x0a | JIS_SHIFT, // G
  0x0b | JIS_SHIFT, // H
  0x0c | JIS_SHIFT, // I
  0x0d | JIS_SHIFT, // J
  0x0e | JIS_SHIFT, // K
  0x0f | JIS_SHIFT, // L
  0x10 | JIS_SHIFT, // M
  0x11 | JIS_SHIFT, // N
  0x12 | JIS_SHIFT, // O
  
  // 0x50
  0x13 | JIS_SHIFT, // P
  0x14 | JIS_SHIFT, // Q
  0x15 | JIS_SHIFT, // R
  0x16 | JIS_SHIFT, // S
  0x17 | JIS_SHIFT, // T
  0x18 | JIS_SHIFT, // U
  0x19 | JIS_SHIFT, // V
  0x1a | JIS_SHIFT, // W
  0x1b | JIS_SHIFT, // X
  0x1c | JIS_SHIFT, // Y
  0x1d | JIS_SHIFT, // Z
  0x30, // [
  0x89, // yen
  0x32, // ]
  0x2e, // ^
  0x87 | JIS_SHIFT, // _

  // 0x60
  0x2f | JIS_SHIFT, // `
  0x04, // a
  0x05, // b
  0x06, // c
  0x07, // d
  0x08, // e
  0x09, // f
  0x0a, // g
  0x0b, // h
  0x0c, // i
  0x0d, // j
  0x0e, // k
  0x0f, // l
  0x10, // m
  0x11, // n
  0x12, // o

  // 0x70
  0x13, // p
  0x14, // q
  0x15, // r
  0x16, // s
  0x17, // t
  0x18, // u
  0x19, // v
  0x1a, // w
  0x1b, // x
  0x1c, // y
  0x1d, // z
  0x30 | JIS_SHIFT, // {
  0x89 | JIS_SHIFT, // |
  0x32 | JIS_SHIFT, // }
  0x2e | JIS_SHIFT, // ~
  
  0       // DEL
};


// ble code [jis -> us]
const unsigned short _codemap_us_default[] PROGMEM =
{
  4127, 4148, // "
  47, 4127, // @
  46, 4131, // ^
  4131, 4132, // &
  4132, 52, // '
  4148, 4133, // *
  4133, 4134, // (
  4134, 4135, // )
  4141, 46, // =
  4231, 4141, // _
  4147, 4142, // +
  48, 47, // [
  4144, 4143, // {
  50, 48, // ]
  4146, 4144, // }
  135, 49, // yen
  137, 49, // yen
  4233, 4145, // |
  52, 4147, // :
  4143, 53, // `
  4142, 4149, // ~
  0, 0
};


// BLEキーボードクラス
class BleKeyboardJIS
{
  public:
    /* プロパティ */
    BleConnectionStatus* connectionStatus;
    NimBLEServer* pServer; // ble サービス
    NimBLEService* pDeviceInfoService; // bleサービス INFO
    NimBLECharacteristic* pPnpCharacteristic; // bleサービス pnp
    NimBLECharacteristic* pManufacturerCharacteristic; // bleサービス Manufacturer
    NimBLEService* pHidService; // HID サービス
    NimBLECharacteristic* pHidInfoCharacteristic; // HID サービスINFO
    NimBLECharacteristic* pReportMapCharacteristic; // HID レポートマップ
    NimBLECharacteristic* pHidControlCharacteristic; // HID コントローラー
    NimBLECharacteristic* pProtocolModeCharacteristic; // HID モード
    NimBLECharacteristic* pInputCharacteristic; // HID input 1 (キーコードの送信)
    NimBLEDescriptor* pDesc1; // HID input 1 (キーコードの送信)
    NimBLECharacteristic* pOutputCharacteristic; // HID output 1 (capslockとかの情報取得)
    NimBLEDescriptor* pDesc2; //  HID output 1 (capslockとかの情報取得)
    NimBLECharacteristic* pInputCharacteristic2; // HID input 2 (メディアキーコード送信)
    NimBLEDescriptor* pDesc3; // HID input 2 (メディアキーコード送信)
    NimBLECharacteristic* pInputCharacteristic3; // HID input 3 (マウス送信)
    NimBLEDescriptor* pDesc4; // HID input 3 (マウス送信)
    NimBLEService* pBatteryService; // バッテリーサービス
    NimBLECharacteristic* pBatteryLevelCharacteristic; // バッテリーサービス レベル
    NimBLE2904* pBatteryLevelDescriptor; // バッテリーサービス レベル
    NimBLEAdvertising* pAdvertising; // アドバタイズ (ペアリングを待つ情報を送信)
    uint8_t *_hidReportDescriptor; // HID レポート設定
    unsigned short _hidReportDescriptorSize; // HID レポートのサイズ
    uint8_t keyboard_language; // 入力タイプ 0=jis, 1=us
    uint8_t batteryLevel; // バッテリーレベル 0-100
    KeyReport _keyReport;
    MediaKeyReport _mediaKeyReport;
    uint8_t _MouseButtons; // マウスボタン情報
    std::string deviceManufacturer; // 会社名
    std::string deviceName; // デバイス名
    unsigned short *_asciimap; // asciiマップ jis
    unsigned short *_codemap_us; // US書換えマップ    
    static void taskServer(void* pvParameter);

    /* メソッド */
    BleKeyboardJIS(std::string deviceName = "BleKeyboardJIS", std::string deviceManufacturer = "PaletteSystem"); // コンストラクタ
    void set_keyboard_language(uint8_t kl);
    void set_report_map(uint8_t * report_map, unsigned short report_size);
    void begin(void);
    void end(void);
    bool isConnected(void);
    unsigned short modifiers_press(unsigned short k);
    unsigned short modifiers_release(unsigned short k);
    void shift_release(); // Shiftを離す
    unsigned short modifiers_media_press(unsigned short k);
    unsigned short modifiers_media_release(unsigned short k);
    unsigned short code_convert(unsigned short k);
    void sendReport(KeyReport* keys);
    void sendReport(MediaKeyReport* keys);
    void mouse_click(uint8_t b);
    void mouse_press(uint8_t b);
    void mouse_release(uint8_t b);
    void mouse_move(signed char x, signed char y, signed char wheel, signed char hWheel);
    size_t press_set(uint8_t k); // 指定したキーだけ押す
    size_t press_raw(unsigned short k);
    size_t release_raw(unsigned short k);
    void releaseAll(void);
    bool onShift(); // Shiftが押されている状態かどうか(物理的に)
};




#endif // CONFIG_BT_NIMBLE_ROLE_PERIPHERAL
#endif // CONFIG_BT_ENABLED
#endif
