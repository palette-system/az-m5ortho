#ifndef AzKeyboard_h
#define AzKeyboard_h

static void background_touch_loop(void* arg);

// クラスの定義
class AzKeyboard
{
    public:
        AzKeyboard();   // コンストラクタ
        void start_keyboard(); // キーボードとして処理開始
        void touch_loop_exec(void); // タッチパッド操作用のループ処理
        void loop_exec();         // キーボード定期処理
        void key_action_exec(); // ステータスが変更されたキーのアクションを実行する
        void key_repeat_exec(); // キー連打処理
        void send_webhook(char *jstr); // WEBフックを送信する
        void send_string(char *send_char); // テキストを送信する
        void hold_press(int hold, int key_num); // hold キーを押す
        void soft_key_click_action(int layer, int key_num); // ソフトキークリック
        void key_down_action(int key_num, short press_type); // キーが押された時のアクション
        void key_up_action(int key_num, short press_type); // キーが離された時のアクション
        void tap_key_disable_all(); // tap / hold の単押しを無効化する
        void press_data_reset(); // 入力状態を全てクリアする
        void press_key_list_push(int action_type, int key_num, int key_id, int layer_id, int repeat_interval, short press_type); // キーを押しましたリストに追加
        void move_mouse_loop(); // マウス移動処理
        void mouse_loop_joy(); // マウスジョイスティック動作
        void mouse_loop_pad(); // マウスタッチパッド動作
        void mouse_loop_none(); // マウス操作なし(長押しで設定画面に移動する所だけ実装)
        void press_data_clear(); // キーを押したままリストをクリア
    
    private:
};

#endif
