#include "Arduino.h"
#include "wirelib.h"


// コンストラクタ
Wirelib::Wirelib() {
}

// I2Cへデータ送信
int Wirelib::write(int addr, uint8_t *send_data, int send_len) {
    int i;
    Wire.beginTransmission(addr);
    for (i=0; i<send_len; i++) {
        Wire.write(send_data[i]);
    }
    return Wire.endTransmission();
}

// I2Cからデータ読み込み
int Wirelib::read(int addr, uint8_t *read_data, int read_len) {
    int i;
    Wire.requestFrom(addr, read_len);
    for (i=0; i<read_len; i++) {
        if (!Wire.available()) break;
        read_data[i] = Wire.read();
    }
    return i;
}

// ロータリエンコーダの入力取得
uint8_t Wirelib::read_rotary(int addr) {
    Wire.requestFrom(addr, 1); // 指定したアドレスのTinyにデータ取得要求
    return Wire.read(); // データ受け取る
};

void Wirelib::set_az1uball_read_type(int addr, int set_mode) {
    Wire.beginTransmission(addr);
    if (set_mode) {
        Wire.write(0x91);
    } else {
        Wire.write(0x90);
    }
    Wire.endTransmission();
};

// 1Uトラックボール PIM447 から入力を取得する
tracktall_pim447_data Wirelib::read_trackball_pim447(int addr) {
    // int wire_err;
    tracktall_pim447_data r;
    // Wire.beginTransmission(addr);
    // Wire.write(0x04);
    // wire_err = Wire.endTransmission();
    Wire.requestFrom(addr, 5);
    r.left = Wire.read();  // 左回転
    r.right = Wire.read(); // 右回転
    r.up = Wire.read();    // 上回転
    r.down = Wire.read();  // 下回転
    r.click = Wire.read(); // スイッチ
    return r;
};

// AZエクスパンダ コンフィグ送信
void Wirelib::send_azxp_setting(int addr, uint8_t *setting) {
    int i;
    // コンフィグ送信
    Wire.beginTransmission(addr);
    Wire.write(0x01); // コンフィグ設定コマンド
    for (i=0; i<18; i++) {
        Wire.write(setting[i]);
    }
    Wire.endTransmission();
    // 結果取得
    Wire.requestFrom(addr, 2);
    while (Wire.available()) Wire.read(); // とりあえず空読み込み
};

// AZエクスパンダ キー数取得
azxp_key_info Wirelib::read_key_info(int addr) {
    azxp_key_info r;
    // コマンド送信
    Wire.beginTransmission(addr);
    Wire.write(0x03); // キー数取得コマンド
    Wire.endTransmission();
    // 結果取得
    Wire.requestFrom(addr, 2);
    r.key_count = (Wire.available())? Wire.read(): 0;
    r.key_byte = (Wire.available())? Wire.read(): 0;
    return r;
}

// AZエクスパンダのキー入力状態を取得
azxp_key_data Wirelib::read_azxp_key(int addr, azxp_key_info kinfo) {
    int i;
    azxp_key_data r;
    for (i=0; i<16; i++) r.key_input[i] = 0x00;
    // コマンド送信
    Wire.beginTransmission(addr);
    Wire.write(0x04); // キー読み込みコマンド
    Wire.endTransmission();
    // 結果取得
    Wire.requestFrom(addr, kinfo.key_byte); // キー読み込みコマンド
    for (i=0; i<kinfo.key_byte; i++) {
        if (i>16) break;
        r.key_input[i] = (Wire.available())? Wire.read(): 0;
    }
    return r;
}
