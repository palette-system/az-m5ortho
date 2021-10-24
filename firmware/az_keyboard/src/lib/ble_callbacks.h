#ifndef BleCallbacks_h
#define BleCallbacks_h
#include "sdkconfig.h"
#if defined(CONFIG_BT_ENABLED)

#include "nimconfig.h"
#if defined(CONFIG_BT_NIMBLE_ROLE_PERIPHERAL)

#include <NimBLEServer.h>
#include <NimBLEDevice.h>
#include "HIDKeyboardTypes.h"
#include "HIDTypes.h"
#include "NimBLECharacteristic.h"

#include "../../az_common.h"


enum via_command_id {
    id_get_protocol_version                 = 0x01,  // always 0x01
    id_get_keyboard_value                   = 0x02,
    id_set_keyboard_value                   = 0x03,
    id_dynamic_keymap_get_keycode           = 0x04,
    id_dynamic_keymap_set_keycode           = 0x05,
    id_dynamic_keymap_reset                 = 0x06,
    id_lighting_set_value                   = 0x07,
    id_lighting_get_value                   = 0x08,
    id_lighting_save                        = 0x09,
    id_eeprom_reset                         = 0x0A,
    id_bootloader_jump                      = 0x0B,
    id_dynamic_keymap_macro_get_count       = 0x0C,
    id_dynamic_keymap_macro_get_buffer_size = 0x0D,
    id_dynamic_keymap_macro_get_buffer      = 0x0E,
    id_dynamic_keymap_macro_set_buffer      = 0x0F,
    id_dynamic_keymap_macro_reset           = 0x10,
    id_dynamic_keymap_get_layer_count       = 0x11,
    id_dynamic_keymap_get_buffer            = 0x12,
    id_dynamic_keymap_set_buffer            = 0x13,
    id_unhandled                            = 0xFF,
};

enum via_keyboard_value_id {
    id_uptime              = 0x01,  //
    id_layout_options      = 0x02,
    id_switch_matrix_state = 0x03
};

extern uint8_t remap_buf[36];

// Characteristic コールバック クラス
class CharacteristicCallbacks: public NimBLECharacteristicCallbacks {
  public:
    CharacteristicCallbacks();
    void onRead(NimBLECharacteristic* pCharacteristic);
    void onWrite(NimBLECharacteristic* pCharacteristic);
    void onNotify(NimBLECharacteristic* pCharacteristic);
    void onStatus(NimBLECharacteristic* pCharacteristic, Status status, int code);
    void onSubscribe(NimBLECharacteristic* pCharacteristic, ble_gap_conn_desc* desc, uint16_t subValue);
};

// Descriptor コールバック クラス
class DescriptorCallbacks : public NimBLEDescriptorCallbacks {
  public:
    DescriptorCallbacks();
    void onWrite(NimBLEDescriptor* pDescriptor);
    void onRead(NimBLEDescriptor* pDescriptor);
};

// コネクションクラス
class BleConnectionStatus : public NimBLEServerCallbacks {
  public:
    BleConnectionStatus(void);
    bool connected = false;
    void onConnect(NimBLEServer* pServer);
    void onDisconnect(NimBLEServer* pServer);
};

// Output コールバック クラス
class KeyboardOutputCallbacks : public NimBLECharacteristicCallbacks {
  public:
    KeyboardOutputCallbacks(void);
    void onWrite(NimBLECharacteristic* me);
};

// Remap Descriptor コールバック クラス
class RemapDescriptorCallbacks : public NimBLEDescriptorCallbacks {
  public:
    RemapDescriptorCallbacks();
    void onWrite(NimBLEDescriptor* pDescriptor);
    void onRead(NimBLEDescriptor* pDescriptor);
};


// REMAP Output コールバック クラス
class RemapOutputCallbacks : public NimBLECharacteristicCallbacks {
  public:
	NimBLECharacteristic* pInputCharacteristic; // REMAP 送信用
    RemapOutputCallbacks(void);
    void onWrite(NimBLECharacteristic* me);
	void sendRawData(uint8_t *data, uint8_t data_length); // Remapにデータを返す
};



#endif // CONFIG_BT_NIMBLE_ROLE_PERIPHERAL
#endif // CONFIG_BT_ENABLED
#endif
