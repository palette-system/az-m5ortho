#ifndef ankey_h
#define ankey_h

#include "../../az_common.h"
#include "../../az_keyboard.h"



// 暗記するデータの構造
struct ankey_data {
	uint8_t type; // 0=データなし / 1=down / 2=up
	uint8_t key_num; // キーの番号
};



// クラスの定義
class Ankey
{
    public:
	    AzKeyboard  *_azkb; // キーボードクラス
	    uint16_t _data_index;
	    uint16_t _loop_index;
	    int ankey_layer_id;
	    uint8_t ankey_key_num; // 暗記開始したキーの番号
	    uint8_t  ankey_flag; // 0=何もなし / 1=暗記中 / 2=暗記した入力実行中
	    uint8_t  ankey_count; // 0=暗記ボタン押されていない / 1=暗記ボタン押されている
	    ankey_data _andata[ANKEY_DATA_MAX_LENGTH];
        Ankey();   // コンストラクタ
        void begin(AzKeyboard  *azkb); // 初期化
        void key_down(int key_num); // キーダウン
        void key_up(int key_num); // キーアップ
        void input_start(); // 暗記開始
        void input_end(); // 暗記終了
        void output_start(); // 暗記したデータのキー入力開始
        void output_end(); // 暗記したデータのキー入力終了
        void ankey_down(short layer_id, int key_num); // 暗記ボタン押された
        void ankey_up(short layer_id, int key_num); // 暗記ボタン離された
        void loop_exec(); // 定期処理
};

#endif
