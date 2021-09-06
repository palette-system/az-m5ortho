#include "../../az_common.h"
#include "custom_func.h"

// 初期処理
CustomFunc::CustomFunc()
{
};

// 開始処理
void CustomFunc::begin()
{
};

// 繰り返し処理
void CustomFunc::loop()
{
};

// キーが押された時
void CustomFunc::key_press(int key_num, const JsonObject &key_set)
{
  // key_num : キーID(0～7)
  // key_set : キーを押した時に設定されている情報
  //           key_set["press"]["action_type"].as<signed int>() // 1=キー入力 2=固定テキスト入力 3=レイヤー切り替え 4=WEBフック
  ESP_LOGD(LOG_TAG, "custom function : key press %D", key_num);
};

// キーが離された時
void CustomFunc::key_release(int key_num, const press_key_data &key_data)
{
  // key_num : キーID(0～7)
  ESP_LOGD(LOG_TAG, "custom function : key release %D", key_num);
};
