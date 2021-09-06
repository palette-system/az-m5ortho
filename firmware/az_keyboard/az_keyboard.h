#ifndef AzKeyboard_h
#define AzKeyboard_h

#include "az_common.h"


// クラスの定義
class AzKeyboard
{
    public:
        AzKeyboard();   // コンストラクタ
        void start_keyboard(); // キーボードとして処理開始
        void start_unit(); // 各ユニットの初期化
        void loop_exec();         // キーボード定期処理
        void unit_loop_exec(); // 各ユニットごとの定期処理
        void key_action_exec(); // ステータスが変更されたキーのアクションを実行する
        void key_repeat_exec(); // キー連打処理
        void send_webhook(const JsonObject &key_set); // WEBフックを送信する
        void send_string(char *send_char); // テキストを送信する
        void key_down_action(int key_num); // キーが押された時のアクション
        void key_up_action(int key_num); // キーが離された時のアクション
        void press_data_reset(); // 入力状態を全てクリアする
        void press_key_list_push(int action_type, int key_num, int key_id, int layer_id, int repeat_interval); // キーを押しましたリストに追加
        void press_mouse_list_clean(); // マウス移動中リストを空にする
        void press_mouse_list_push(int key_num, short move_x, short move_y, short move_speed); // マウス移動中リストに追加
        void press_mouse_list_remove(int key_num); // マウス移動中リストから削除
        void move_mouse_loop(); // マウス移動処理
        void press_data_clear(); // キーを押したままリストをクリア
    
    private:
};

#endif
