#include "Arduino.h"
#include "paw3204.h"


// コンストラクタ
Paw3204::Paw3204() {
}


// トラックボール初期化処理
void Paw3204::begin(short sclk_pin, short data_pin) {
    this->_sclk_pin = sclk_pin;
    this->_data_pin = data_pin;
    pinMode(this->_sclk_pin, OUTPUT); // SCLK
    pinMode(this->_data_pin, OUTPUT); // SDIO
    delay(500);
  
    exec_cmd(0x00);
    delay(500);
}

// コマンド送信
int8_t Paw3204::exec_cmd(uint8_t send_cmd) {
    int8_t idx;
    pinMode(this->_data_pin, OUTPUT); // SDIO

    for (idx = 7; idx >= 0; idx--) {
        digitalWrite(this->_sclk_pin, 0);
        ets_delay_us(1);
        digitalWrite(this->_data_pin, (send_cmd >> idx) & 1);
        ets_delay_us(1);
        digitalWrite(this->_sclk_pin, 1);
        ets_delay_us(1);
    }

    ets_delay_us(5);
    pinMode(this->_data_pin, INPUT_PULLUP);

    int8_t res = 0;
    for (idx = 7; idx >= 0; idx--) {
        digitalWrite(this->_sclk_pin, 0);
        ets_delay_us(1);
        digitalWrite(this->_sclk_pin, 1);
        ets_delay_us(1);
        res |= !digitalRead(this->_data_pin) << idx;
        ets_delay_us(1);
    }

    return res;
}

// 入力座標を取得
paw3204_xy Paw3204::getxy() {
    int8_t d;
    paw3204_xy res;
    d = exec_cmd(0x02);
    res.x = exec_cmd(0x03);
    res.y = exec_cmd(0x04);
    return res;
}
