#ifndef dakey_h
#define dakey_h


#include "../../az_common.h"


// 自動保存する間隔(30分)単位はミリ秒
#define  AUTO_SAVE_INTERVAL  1800000


// クラスの定義
class Dakey
{
	public:
		int last_save_key_count; // 最後に保存した打鍵数
		unsigned long last_save_time; // 最後に保存した時間（millis）
		Dakey();   // コンストラクタ
		void begin(); // 初期化
		void set_auto_save_change(); // 打鍵数自動保存するかどうかの設定を変更
		void save_dakey(uint8_t view_flag); // 現在の打鍵数を保存する
		void loop_exec(); // 定期処理
		
};

#endif
