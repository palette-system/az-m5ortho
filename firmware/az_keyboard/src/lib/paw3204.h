#ifndef paw3204_h
#define paw3204_h


struct paw3204_xy {
    int8_t x;
    int8_t y;
};


// クラスの定義
class Paw3204
{
    public:
        short _data_pin; // データピン
        short _sclk_pin; // クロックピン
        Paw3204();   // コンストラクタ
        void begin(short sclk_pin, short data_pin); // トラックボール初期化処理
        int8_t exec_cmd(uint8_t send_cmd); // コマンド送信
        paw3204_xy getxy(); // 入力座標を取得
};

#endif
