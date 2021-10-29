#include <NimBLEServer.h>

#include "Arduino.h"
#include "ble_callbacks.h"

#if defined(CONFIG_ARDUHAL_ESP_LOG)
  #include "esp32-hal-log.h"
  #define LOG_TAG ""
#else
  #include "esp_log.h"
  static const char* LOG_TAG = "NimBLEDevice";
#endif


// remapへ返事を返す用のバッファ
uint8_t remap_buf[36];

// remapで設定変更があったかどうかのフラグ
uint8_t remap_change_flag;


/* ====================================================================================================================== */
/** Characteristic コールバック クラス */
/* ====================================================================================================================== */

CharacteristicCallbacks::CharacteristicCallbacks(void) {
};

void CharacteristicCallbacks::onRead(NimBLECharacteristic* pCharacteristic){
    // Serial.print(pCharacteristic->getUUID().toString().c_str());
    // Serial.print(": onRead(), value: ");
    // Serial.println(pCharacteristic->getValue().c_str());
};

void CharacteristicCallbacks::onWrite(NimBLECharacteristic* pCharacteristic) {
    // Serial.print(pCharacteristic->getUUID().toString().c_str());
    // Serial.print(": onWrite(), value: ");
    // Serial.println(pCharacteristic->getValue().c_str());
};

void CharacteristicCallbacks::onNotify(NimBLECharacteristic* pCharacteristic) {
    // Serial.println("Sending notification to clients");
};

void CharacteristicCallbacks::onStatus(NimBLECharacteristic* pCharacteristic, Status status, int code) {
    String str = ("Notf/Ind stscode: ");
    str += status;
    str += ", retcode: ";
    str += code;
    str += ", "; 
    str += NimBLEUtils::returnCodeToString(code);
    //Serial.print(str);
};

void CharacteristicCallbacks::onSubscribe(NimBLECharacteristic* pCharacteristic, ble_gap_conn_desc* desc, uint16_t subValue) {
    String str = "Client ID: ";
    str += desc->conn_handle;
    str += " Address: ";
    str += std::string(NimBLEAddress(desc->peer_ota_addr)).c_str();
    if(subValue == 0) {
        str += " Unsubscribed to ";
    }else if(subValue == 1) {
        str += " Subscribed to notfications for ";
    } else if(subValue == 2) {
        str += " Subscribed to indications for ";
    } else if(subValue == 3) {
        str += " Subscribed to notifications and indications for ";
    }
    str += std::string(pCharacteristic->getUUID()).c_str();

    // Serial.println(str);
};

/* ====================================================================================================================== */
/** Descriptor コールバック クラス */
/* ====================================================================================================================== */

DescriptorCallbacks::DescriptorCallbacks(void) {
};

void DescriptorCallbacks::onWrite(NimBLEDescriptor* pDescriptor) {
    std::string dscVal((char*)pDescriptor->getValue(), pDescriptor->getLength());
    // Serial.print("Descriptor witten value:");
    // Serial.println(dscVal.c_str());
};

void DescriptorCallbacks::onRead(NimBLEDescriptor* pDescriptor) {
    // Serial.print(pDescriptor->getUUID().toString().c_str());
    // Serial.println(" Descriptor read");
};

/* ====================================================================================================================== */
/** コネクションクラス */
/* ====================================================================================================================== */

BleConnectionStatus::BleConnectionStatus(void) {
};

void BleConnectionStatus::onConnect(NimBLEServer* pServer)
{
  this->connected = true;
};

void BleConnectionStatus::onDisconnect(NimBLEServer* pServer)
{
  this->connected = false;
};

/* ====================================================================================================================== */
/** Output コールバック クラス */
/* ====================================================================================================================== */

KeyboardOutputCallbacks::KeyboardOutputCallbacks(void) {
}

void KeyboardOutputCallbacks::onWrite(NimBLECharacteristic* me) {
  uint8_t* value = (uint8_t*)(me->getValue().c_str());
  ESP_LOGI(LOG_TAG, "special keys: %d", *value);
}


/* ====================================================================================================================== */
/** Remap Descriptor コールバック クラス */
/* ====================================================================================================================== */

RemapDescriptorCallbacks::RemapDescriptorCallbacks(void) {
};

void RemapDescriptorCallbacks::onWrite(NimBLEDescriptor* pDescriptor) {
    std::string dscVal((char*)pDescriptor->getValue(), pDescriptor->getLength());
    Serial.print("RemapDescriptorCallbacks: onWrite: ");
    Serial.println(dscVal.c_str());
};

void RemapDescriptorCallbacks::onRead(NimBLEDescriptor* pDescriptor) {
    Serial.print("RemapDescriptorCallbacks: onRead: ");
    Serial.println(pDescriptor->getUUID().toString().c_str());
};

/* ====================================================================================================================== */
/** Remap Output コールバック クラス */
/* ====================================================================================================================== */

RemapOutputCallbacks::RemapOutputCallbacks(void) {
	remap_change_flag = 0;
}



void RemapOutputCallbacks::onWrite(NimBLECharacteristic* me) {
	uint8_t* data = (uint8_t*)(me->getValue().c_str());
	size_t data_length = me->getDataLength();
	memcpy(remap_buf, data, data_length);
	int i, j, m;
    uint8_t *command_id   = &(remap_buf[0]);
    uint8_t *command_data = &(remap_buf[1]);

	/* REMAP から受け取ったデータデバッグ表示
	Serial.printf("get: (%d) ", data_length);
	for (i=0; i<data_length; i++) {
		Serial.printf("%02x ", remap_buf[i]);
	}
	Serial.printf("\n");
	*/

	// 設定変更がされていて設定変更以外のコマンドが飛んできたら設定を保存
	if (remap_change_flag && *command_id != 0x05) {
		common_cls.remap_save_setting_json(); // JSONに保存
		remap_change_flag = 0;
	}
	
	switch (*command_id) {
		case id_get_keyboard_value: { // 0x02 キーボードの情報を送る
			switch (command_data[0]) {
				case id_uptime: { // 0x01
					break;
				}
				case id_layout_options: { // 0x02 レイアウトオプション
					remap_buf[2] = 0x00;
					remap_buf[3] = 0x00;
					remap_buf[4] = 0x00;
					remap_buf[5] = 0x00;
					break;
				}
				case id_switch_matrix_state: { // 0x03 スイッチの状態(入力テスト用)
					remap_buf[2] = 0x00;
					remap_buf[3] = 0x00;
					remap_buf[4] = 0x00;
					remap_buf[5] = 0x00;
					m = 3;
					for (i=0; i<key_input_length; i++) {
						remap_buf[m] |= common_cls.input_key[i] << (i % 8);
						if ((i %8) == 7) m++;
					}
					break;
				}
			}
			break;
		}
		case id_dynamic_keymap_set_keycode: { // 0x05 設定した内容を保存
			m = (remap_buf[1] * key_max * 2) + (remap_buf[3] * 2);
			setting_remap[m] = remap_buf[4];
			setting_remap[m + 1] = remap_buf[5];
			remap_change_flag = 1;
			break;
		}
		case id_dynamic_keymap_reset: { // 0x06 キーマップリセット とりあえず何もしない
			break;
		}
		case id_dynamic_keymap_macro_get_buffer_size: { // 0x0D マクロキー用のバッファサイズ
			// uint16_t size   = dynamic_keymap_macro_get_buffer_size();
			m = 0;
			remap_buf[1] = (m >> 8) & 0xFF;
            remap_buf[2] = m & 0xFF;
			break;
		}
        case id_dynamic_keymap_macro_get_count: { // 0x0C マクロキーを登録できる数
            remap_buf[1] = 0x00; // dynamic_keymap_macro_get_count();
            break;
        }
        case id_dynamic_keymap_macro_get_buffer: { // 0x0E マクロキーデータを送る
            // uint16_t offset = (command_data[0] << 8) | command_data[1];
            // uint16_t size   = command_data[2];  // size <= 28
            // dynamic_keymap_macro_get_buffer(offset, size, &command_data[3]);
            remap_buf[4] = 0x00;
            break;
        }
        case id_dynamic_keymap_macro_set_buffer: { // 0x0F マクロキーデータを保存
            // uint16_t offset = (command_data[0] << 8) | command_data[1];
            // uint16_t size   = command_data[2];  // size <= 28
            // dynamic_keymap_macro_set_buffer(offset, size, &command_data[3]);
            break;
        }
		case id_dynamic_keymap_get_layer_count: { // 0x11 レイヤー数を送る
			remap_buf[1] = layer_max;
			break;
		}
		case id_dynamic_keymap_get_buffer: { // 0x12 設定データを送る(レイヤー×ROW×COL×2)
			uint16_t r_offset = (command_data[0] << 8) | command_data[1];
			uint16_t r_size   = command_data[2];  // size <= 28
			for (i=0; i<r_size; i++) {
				remap_buf[4 + i] = setting_remap[r_offset + i];
			}
			break;
		}
		default: {
			remap_buf[0] = 0xFF;
			break;
		}
	}

	/* REMAPに送信するデータデバッグ表示
	Serial.printf("put: (%d) ", data_length);
	for (i=0; i<data_length; i++) Serial.printf("%02x ", remap_buf[i]);
	Serial.printf("\n\n");
	*/
	delay(10);
	this->sendRawData(remap_buf, data_length);
	delay(10);
	
}

// Remapにデータを返す
void RemapOutputCallbacks::sendRawData(uint8_t *data, uint8_t data_length) {
	this->pInputCharacteristic->setValue(data, data_length);
    this->pInputCharacteristic->notify();
	// delay(1);
}



