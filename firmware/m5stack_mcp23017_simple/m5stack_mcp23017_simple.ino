// 押したキーの番号を液晶に表示するだけのサンプル

#include <M5Core2.h>
#include <Wire.h>
#include <Adafruit_MCP23X17.h>

// I/Oエキスパンダの数
#define  IOMCP_LENGTH  1

// チェックするキーの数
#define  INPUT_KEY_LENGTH  (16 * IOMCP_LENGTH)


// I2Cのピン
#define IOMCP_SDA  26
#define IOMCP_SCL  25

// I/Oエキスパンダ用
Adafruit_MCP23X17 *iomcp;

// 押されているかどうかのフラグ
bool input_key[INPUT_KEY_LENGTH];

// 前回取得した入力データ
bool input_key_old[INPUT_KEY_LENGTH];

// 初期化
void setup() {
    // M5Stack初期化
    M5.begin(true, true, true, true);
    M5.Lcd.setRotation(0); // 画面を横向きにする
    M5.Lcd.fillScreen(BLACK); // 画面を黒でぬりつぶす
    M5.Lcd.setCursor(0, 0, 2); // カーソルの位置を0,0に移動(16pxフォント)
    M5.Lcd.setTextColor(DARKGREEN); // フォントの色

    // I2C初期化
    if (Wire.begin(IOMCP_SDA, IOMCP_SCL)) {
        Wire.setClock(1700000);
        M5.Lcd.printf("Wire1 begin ok\n");
    } else {
        M5.Lcd.printf("Wire1 begin ng\n");
    }

    // MCP23017 初期化
    int i, j;
    M5.Lcd.printf("Adafruit_MCP23X17 %D\n", IOMCP_LENGTH);
    iomcp = new Adafruit_MCP23X17[IOMCP_LENGTH];
    for (i=0; i<IOMCP_LENGTH; i++) {
      if (iomcp[i].begin_I2C(0x20 + i, &Wire)) {
          M5.Lcd.printf("begin_I2C %D  %D OK\n", i);
      } else {
          M5.Lcd.printf("begin_I2C %D %D NG\n", i);
          continue;
      }
      // 全ピン入力にする
      for (j=0; j<16; j++) {
          iomcp[i].pinMode(j, INPUT_PULLUP);
      }
    }

    // キー入力データ初期化
    for (i=0; i<INPUT_KEY_LENGTH; i++) {
        input_key[i] = false;
        input_key_old[i] = false;
    }

    delay(2000);
    M5.Lcd.fillScreen(BLACK); // 画面を黒でぬりつぶす
}

// キーの入力状態を取得
void key_read() {
    int c, i, j, k, n;
    int m[IOMCP_LENGTH];
    
    // 前回の入力情報を old へ
    for (i=0; i<INPUT_KEY_LENGTH; i++) {
        input_key_old[i] = input_key[i];
    }
    
    // 現在の入力情報取得
    for (i=0; i<IOMCP_LENGTH; i++) {
      m[i] = iomcp[i].readGPIOAB();
      // 全ピン入力無しならエキスパンダがリセットされた可能性があるので、ピンの初期化をもっかいやって取得しなおす
      if (m[i] == 0x00) {
        for (j=0; j<16; j++) { // 全ピン入力にする
            iomcp[i].pinMode(j, INPUT_PULLUP);
        }
        m[i] = iomcp[i].readGPIOAB();
        if (m[i] == 0x00) m[i] = 0xffff; // 取り直してもデータが無いなら全ピンボタンが押されてない状態にする
      }
    }
    for (i=0; i<16; i++) {
        c = 1 << i;
        n = 0;
        for (j=0; j<IOMCP_LENGTH; j++) {
            input_key[i + n] = (!(m[j] & c));
            n += 16;
        }
    }
}

// 動作ループ
void loop() {
    int i;
    bool change_flag = false;

    // キーの入力情報取得
    key_read();

    // 前回の入力から変更があったかチェック
    for (i=0; i<INPUT_KEY_LENGTH; i++) {
        if (input_key[i] != input_key_old[i]) change_flag = true;
    }

    // 変更が無ければ何もしない
    if (!change_flag) {
        delay(20);
        return;
    }

    // 変更があれば画面に入力中のキー番号を表示
    M5.Lcd.fillScreen(BLACK); // 画面を黒でぬりつぶす
    M5.Lcd.setCursor(0, 0, 7); // カーソルの位置を0,0に移動(48pxフォント)
    for (i=0; i<INPUT_KEY_LENGTH; i++) {
        if (input_key[i]) M5.Lcd.printf("%D.", i);
    }

    delay(20);
}
