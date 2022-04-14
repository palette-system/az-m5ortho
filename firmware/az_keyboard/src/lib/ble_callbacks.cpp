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

#include "../../az_common.h"


// remapへ返事を返す用のバッファ
uint8_t remap_buf[36];

// ファイル送受信用バッファ
uint8_t send_buf[36];
char target_file_path[36];

// ファイル保存用バッファ
uint8_t *save_file_data;
int save_file_length;
uint8_t save_file_step;
uint8_t save_file_index;
bool save_step_flag[8];

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

// ファイル送受信用のファイルポインタ
File open_file;

// ステップ分受信したか確認
int check_step() {
	int i, r = 0;
	for (i=0; i<8; i++) {
		if (save_step_flag[i]) r++;
	}
	return r;
};

// crc32のハッシュ値を計算
int crc32(uint8_t* d, int len) {
	int i;
    uint32_t r = 0 ^ (-1);
    for (i=0; i<len; i++) {
        r = (r >> 8) ^ crc_table_crc32[(r ^ d[i]) & 0xFF];
    }
    return (r ^ (-1));
};

RemapOutputCallbacks::RemapOutputCallbacks(void) {
	remap_change_flag = 0;
}

void RemapOutputCallbacks::onWrite(NimBLECharacteristic* me) {
	uint8_t* data = (uint8_t*)(me->getValue().c_str());
	size_t data_length = me->getDataLength();
	memcpy(remap_buf, data, data_length);
	int h, i, j, k, l, m, s, p;
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
		case id_get_file_start: { // 0x30 ファイル取得開始
			// ファイル名を取得
			i = 1;
			while (remap_buf[i]) {
				target_file_path[i - 1] = remap_buf[i];
				i++;
				if (i >= 32) break;
			}
			target_file_path[i - 1] = 0x00;
		    // ファイルが無ければ0を返す
			if (!SPIFFS.exists(target_file_path)) {
				for (i=1; i<32; i++) send_buf[i] = 0x00;
				this->sendRawData(remap_buf, 32);
				return;
			}
			open_file = SPIFFS.open(target_file_path, "r");
			save_file_length = open_file.size();
			// Serial.printf("ps_malloc load: %d %d\n", save_file_length, heap_caps_get_free_size(MALLOC_CAP_INTERNAL));
			save_file_data = (uint8_t *)ps_malloc(save_file_length);
			i = 0;
			while (open_file.available()) {
				save_file_data[i] = open_file.read();
				i++;
			}
			open_file.close();
			send_buf[0] = 0x30;
			send_buf[1] = 0x01; // ファイルは存在する
			send_buf[2] = ((save_file_length >> 24) & 0xff);
			send_buf[3] = ((save_file_length >> 16) & 0xff);
			send_buf[4] = ((save_file_length >> 8) & 0xff);
			send_buf[5] = (save_file_length & 0xff);
			for (i=6; i<32; i++) send_buf[i] = 0x00;
			this->sendRawData(send_buf, 32);
			return;
		    
		}
		case id_get_file_data: { // 0x31 ファイルデータ要求
		    // 情報を取得
			s = remap_buf[1]; // ステップ数
			p = (remap_buf[2] << 16) + (remap_buf[3] << 8) + remap_buf[4]; // 読み込み開始位置
			h = (remap_buf[5] << 24) + (remap_buf[6] << 16) + (remap_buf[7] << 8) + remap_buf[8]; // ハッシュ値
			if (h != 0) {
				l = s * (data_length - 4); // ステップ数 x 1コマンドで送るデータ数
				m = crc32(&save_file_data[p - l], l); // 前回送った所のハッシュを計算
				if (h != m) { // ハッシュ値が違えば前に送った所をもう一回送る
					// Serial.printf("NG : [%d %d] [ %d -> %d ]\n", h, m, p, (p - l));
				    p = p - l;
				} else {
					// Serial.printf("OK : [%d %d] [ %d ]\n", h, m, p);
				}
			}
			j = 0;
			// open_file = SPIFFS.open(target_file_path, "r");
			// open_file.seek(p, SeekSet);
			for (j=0; j<s; j++) {
				send_buf[0] = 0x31;
				send_buf[1] = ((p >> 16) & 0xff);
				send_buf[2] = ((p >> 8) & 0xff);
				send_buf[3] = (p & 0xff);
				i = 4;
				// while (open_file.available()) {
				while (p < save_file_length) {
					// send_buf[i] = open_file.read();
					send_buf[i] = save_file_data[p];
					i++;
					p++;
					if (i >= 32) break;
				}
				while (i<32) {
					send_buf[i] = 0x00;
					i++;
				}
				this->sendRawData(send_buf, 32);
				// if (!open_file.available()) break;
				if (p >= save_file_length) break;

			}
			if (p >= save_file_length) {
				// Serial.printf("free load: %d %d\n", save_file_length, heap_caps_get_free_size(MALLOC_CAP_INTERNAL));
				free(save_file_data);
			}

			// open_file.close();
			return;
		}
		case id_save_file_start: { // 0x32 ファイル保存開始
		    // 容量を取得
			save_file_length = (remap_buf[1] << 24) + (remap_buf[2] << 16) + (remap_buf[3] << 8) + remap_buf[4];
			// 保存時のステップ数
			save_file_step = remap_buf[5];
			// ファイル名を取得
			i = 6;
			while (remap_buf[i]) {
				target_file_path[i - 6] = remap_buf[i];
				i++;
				if (i >= 32) break;
			}
			target_file_path[i - 6] = 0x00;
			// データ受け取りバッファクリア
			// for (i=0; i<512; i++) save_file_data[i] = 0x00;
			// 取得したステップのインデックス
			for (i=0; i<8; i++) save_step_flag[i] = false;
			// ファイルオープン
			// Serial.printf("ps_malloc save: %d %d\n", save_file_length, heap_caps_get_free_size(MALLOC_CAP_INTERNAL));
			save_file_data = (uint8_t *)ps_malloc(save_file_length);
			// open_file = SPIFFS.open(target_file_path, "w");
			// データ要求コマンド送信
			send_buf[0] = 0x33;
			send_buf[1] = save_file_step;
			for (i=2; i<32; i++) send_buf[i] = 0x00;
			this->sendRawData(send_buf, 32);
			return;

		}
		case id_save_file_data: { // 0x33 ファイルデータ受け取り
		    s = remap_buf[1]; // 何ステップ目のデータか
			j = (remap_buf[2] << 16) + (remap_buf[3] << 8) + remap_buf[4]; // 何処開始のデータか
			m = data_length - 5; // データの長さ
			// バッファにデータを貯める
			p = s * m; // バッファの書込み開始位置
			// Serial.printf("save: %d %d %d\n", j, p, m);
			for (i=0; i<m; i++) {
				// save_file_data[p + i] = remap_buf[i + 5];
				if ((j + p + i) >= save_file_length) break;
				save_file_data[j + p + i] = remap_buf[i + 5];
			}
			// ステップのインデックス加算
			save_step_flag[s] = true;
			// 全ステップ取得した
			if (check_step() >= save_file_step) {
				// ステップインデックスをリセット
				for (i=0; i<8; i++) save_step_flag[i] = false;
				// バッファに入ったデータをファイルに書き出し
				l = m * save_file_step; // 書込みを行うサイズ
				k = j + l; // 書込み後のシークポイント
				// 書込みサイズが保存予定のサイズを超えたら超えない数値にする
				if (k > save_file_length) {
					l = save_file_length - j;
					k = j + l;
				}
				// 書き込む
				h = crc32(&save_file_data[j], l);
				if (k < save_file_length) {
					// まだデータを全部受け取って無ければ次を要求するコマンドを送信
					send_buf[0] = 0x33;
					send_buf[1] = save_file_step;
					send_buf[2] = (k >> 24) & 0xff; // データの開始位置 1
					send_buf[3] = (k >> 16) & 0xff; // データの開始位置 2
					send_buf[4] = (k >> 8) & 0xff;  // データの開始位置 3
					send_buf[5] = k & 0xff;         // データの開始位置 4
					send_buf[6] = (h >> 24) & 0xff; // データ確認用ハッシュ 1
					send_buf[7] = (h >> 16) & 0xff; // データ確認用ハッシュ 2
					send_buf[8] = (h >> 8) & 0xff;  // データ確認用ハッシュ 3
					send_buf[9] = h & 0xff;         // データ確認用ハッシュ 4
					for (i=10; i<32; i++) send_buf[i] = 0x00;
					this->sendRawData(send_buf, 32);
					return;
				} else {
					// データを全部受け取り終わり
					// 完了を送る
					h = crc32(save_file_data, save_file_length);
					send_buf[0] = 0x34; // 保存完了
					send_buf[1] = 0x00; // データ受信完了
					send_buf[2] = (h >> 24) & 0xff; // データ確認用ハッシュ 1
					send_buf[3] = (h >> 16) & 0xff; // データ確認用ハッシュ 2
					send_buf[4] = (h >> 8) & 0xff;  // データ確認用ハッシュ 3
					send_buf[5] = h & 0xff;         // データ確認用ハッシュ 4
					for (i=6; i<32; i++) send_buf[i] = 0x00;
					this->sendRawData(send_buf, 32);
					return;
				}


			}
			return;
		}
		case id_save_file_complate: {
			// 保存完了
			open_file = SPIFFS.open(target_file_path, "w");
			open_file.write(save_file_data, save_file_length);
			open_file.close(); // ファイルクローズ
			// Serial.printf("free save: %d %d\n", save_file_length, heap_caps_get_free_size(MALLOC_CAP_INTERNAL));
			free(save_file_data);
			send_buf[0] = 0x34; // 保存完了
			send_buf[1] = 0x01; // データ保存完了
			for (i=2; i<32; i++) send_buf[i] = 0x00;
			this->sendRawData(send_buf, 32);
			return;
		}
		case id_get_file_list: {
			// ファイルリストの取得
			File dirp = SPIFFS.open("/");
			File filep = dirp.openNextFile();
			String res = "";
			while(filep){
				if (res.length()) res += "\n";
				res += String(filep.name()) + "\n" + String(filep.size());
				filep = dirp.openNextFile();
			}
			save_file_length = res.length();
			m = save_file_length + 1;
			// ファイルリストの結果を送信用バッファに入れる
			save_file_data = (uint8_t *)ps_malloc(m);
			res.toCharArray((char *)save_file_data, m);
			// 結果を返すコマンドを送信
			send_buf[0] = 0x35;
			send_buf[1] = ((save_file_length >> 24) & 0xff);
			send_buf[2] = ((save_file_length >> 16) & 0xff);
			send_buf[3] = ((save_file_length >> 8) & 0xff);
			send_buf[4] = (save_file_length & 0xff);
			for (i=5; i<32; i++) send_buf[i] = 0x00;
			this->sendRawData(send_buf, 32);
			return;


		}
		case id_restart: {
			// M5StackCore2 の再起動
		    m = remap_buf[1]; // 起動モード取得
			common_cls.change_mode(m);
			return;

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



