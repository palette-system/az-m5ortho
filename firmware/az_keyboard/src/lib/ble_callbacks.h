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


enum via_command_id {
    // remap用
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

    // aztool用
    id_get_file_start                       = 0x30,
    id_get_file_data                        = 0x31,
    id_save_file_start                      = 0x32,
    id_save_file_data                       = 0x33,
    id_save_file_complate                   = 0x34,
	id_remove_file                          = 0x35,
	id_move_file                            = 0x36,
	id_get_file_list                        = 0x37,
	id_restart                              = 0x38,
	id_get_ioxp_key                         = 0x39,
	id_set_mode_flag                        = 0x3A,

    // システム用
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
	uint16_t target_handle;
    void onConnect(NimBLEServer* pServer);
	void onConnect(NimBLEServer* pServer, ble_gap_conn_desc* desc);
    void onDisconnect(NimBLEServer* pServer);
	void onAuthenticationComplete(ble_gap_conn_desc* desc);
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
