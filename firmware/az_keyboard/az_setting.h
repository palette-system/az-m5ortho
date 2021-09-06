#ifndef AzSetting_h
#define AzSetting_h

#include "az_common.h"


// クラスの定義
class AzSetting
{
    public:
      AzSetting();   // コンストラクタ
      void start_setting();          // 初期処理
      void loop_exec(); // 設定モード中のループ処理
      void key_action_exec(); // キー入力の処理
      void key_down_action(int key_num); // キーが押された時のアクション
    
    private:
  
};

#endif
