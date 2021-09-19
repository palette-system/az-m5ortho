#ifndef CustomFunc_h
#define CustomFunc_h

#include "../../az_common.h"


// クラスの定義
class CustomFunc
{
    public:
      CustomFunc();   // コンストラクタ
      void begin(); // 開始処理
      void loop(); // 繰り返し処理
      void key_press(int key_num, const setting_key_press &key_set); // キーが押された
      void key_release(int key_num, const press_key_data &key_data); // キーが離された
  
};

#endif
