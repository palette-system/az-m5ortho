#include "az_common.h"
#include "src/lib/cnc_table.h"

// キーが押された時の設定
uint16_t setting_length;
setting_key_press *setting_press;

// ソフトキーが押された時の設定
uint16_t soft_setting_length;
setting_key_press *soft_setting_press;

// ソフトキーが押された時入る変数(押されてない間は-1)
int16_t soft_click_layer;
int16_t soft_click_key;

// remapに送る用のデータ
uint8_t  *setting_remap;
uint16_t  layer_max;
uint16_t  key_max;

// レイヤー名のデータ
layer_name_data *layer_name_list;
uint16_t layer_name_length;
layer_name_data *soft_layer_name_list;
uint16_t soft_layer_name_length;

// wifi設定
uint8_t wifi_data_length;
azsetting_wifi *wifi_data;
// アクセスポイントパスワード
char *ap_pass_char;
// RGBLED
int8_t rgb_pin;
int16_t rgb_len;
int8_t matrix_row;
int8_t matrix_col;
int8_t *led_num;
int8_t *key_matrix;
uint8_t led_num_length;
uint8_t key_matrix_length;

// hid
uint16_t hid_vid;
uint16_t hid_pid;

// ステータスLED今0-9
int status_led_bit = 0;

// ステータスLED表示モード
int status_led_mode;

// マウスパッドステータス
int8_t mouse_pad_status;

// マウスパッド操作方法設定
mousepad_data_set mouse_pad_setting;

// 画面設定
moniterset_data_set moniter_setting;

// BLE 用macアドレスリスト
ble_mac_addr *blemac_list;

// BLE macアドレス(現在どれを利用しているか)
uint8_t blemac_index;
uint8_t blemac_len;

// BLE 新しいmacアドレス取得用ステータス
uint8_t blemac_stat;
uint8_t macaddr_new[6];

// 液晶表示用オブジェクト
Display *disp;

// display 初期化フラグ
bool disp_enable;

// rgb_led制御用クラス
Neopixel rgb_led_cls = Neopixel();

// サウンド制御用クラス
Sound sound_cls = Sound();

//timer オブジェクト
hw_timer_t *timer = NULL;

// WIFI接続オブジェクト
WiFiMulti wifiMulti;

// WIFI接続フラグ
int wifi_conn_flag;

// http用のバッファ
char webhook_buf[WEBFOOK_BUF_SIZE];

// 入力キーの数
int key_input_length;

// ディスプレイの向き
uint8_t disp_rotation;

// キーボードタイプの番号
int keyboard_type_int;

// キーボードの名前
char keyboard_name_str[32];

// キーボードの言語(日本語=0/ US=1 / 日本語(US記号) = 2)
uint8_t keyboard_language;

// オプションタイプの番号
int option_type_int;

// トラックボールの方向
uint8_t trackball_direction;

// トラックボールのカーソル移動速度
uint8_t trackball_speed;

// 踏みキーの反転フラグ
bool foot_inversion;

// オープニングムービー再生フラグ
bool op_movie_flag;

// タップした位置
short start_touch_x;
short start_touch_y;
short last_touch_x;
short last_touch_y;
short touch_move_x;
short touch_move_y;
short touch_send_count;
short touch_send_index;
short last_touch_index;

// デフォルトのレイヤー番号と、今選択しているレイヤー番号と、最後に押されたレイヤーボタン
int default_layer_no;
int select_layer_no;
int last_select_layer_key;


// 押している最中のキーデータ
press_key_data press_key_list[PRESS_KEY_MAX];

// 押している最中のマウス移動
press_mouse_data press_mouse_list[PRESS_MOUSE_MAX];

// マウスのスクロールボタンが押されているか
bool mouse_scroll_flag;

// リスタート用のフラグ
int8_t restart_flag;
int8_t restart_index;

// 設定メニューを表示しているかどうか
bool menu_mode_flag;

// aztoolで設定中かどうか
uint8_t aztool_mode_flag;

// オールクリア送信フラグ
int press_key_all_clear;

// eepromのデータ
mrom_data_set eep_data;

// 起動回数
uint32_t boot_count;

// 打鍵数を自動保存するかどうか
uint8_t key_count_auto_save;

// キーボードのステータス
int8_t keyboard_status;

// IOエキスパンダオブジェクト
Adafruit_MCP23X17 *ioxp_obj[8];

// 入力用ピン情報
short direct_len;
short touch_len;
short *direct_list;
short *touch_list;
short ioxp_len;
short *ioxp_list;
short ioxp_sda;
short ioxp_scl;
int ioxp_hz;
short ioxp_status[8];
int ioxp_hash[8];

// I2Cオプションの設定
i2c_option *i2copt;
short i2copt_len;

// バッテリーオブジェクト
AXP192 power;

// バッテリ残量更新フラグ
int pw_update_index;

// LVGL用
TFT_eSPI lvtft = TFT_eSPI();
lv_disp_buf_t disp_buf;
lv_color_t lvbuf[LV_HOR_RES_MAX * 10];
int8_t lvgl_loop_index;


// ランダムな文字生成(1文字)
char getRandomCharLower(void) {
    const char CHARS[] = "abcdefghijklmnopqrstuvwxyz";
    int index = random(0, (strlen(CHARS) - 1));
    char c = CHARS[index];
    return c;
}

// ランダムな文字生成(1文字)
char getRandomNumber(void) {
    const char CHARS[] = "0123456789";
    int index = random(0, (strlen(CHARS) - 1));
    char c = CHARS[index];
    return c;
}

// ランダムな文字列生成(文字数指定)
void getRandomCharsLower(int le, char *cbuf) {
    int i;
    for(i=0; i<le; i++){
        cbuf[i]  = getRandomCharLower();
    }
    cbuf[i] = '\0';
}

// ランダムな文字列生成(数字)
void getRandomNumbers(int le, char *cbuf) {
    int i;
    for(i=0; i<le; i++){
        cbuf[i]  = getRandomNumber();
    }
    cbuf[i] = '\0';
}

// crc32のハッシュ値を計算
int azcrc32(uint8_t* d, int len) {
	int i;
    uint32_t r = 0 ^ (-1);
    for (i=0; i<len; i++) {
        r = (r >> 8) ^ crc_table_crc32[(r ^ d[i]) & 0xFF];
    }
    return (r ^ (-1));
};


static void background_loop(void* arg) {
  while (true) {
    unsigned long start_time = millis();
    // サウンドを制御する定期処理
    sound_cls.loop_exec();
    // LED用定期処理
    // rgb_led_cls.rgb_led_loop_exec();
    unsigned long work_time = millis() - start_time;
    if (work_time < 20) { vTaskDelay(20 - work_time); }
  }
}

// 画面描画ループ
static void background_disp_loop(void* arg) {
  while (true) {
    unsigned long start_time = millis();
    if (!menu_mode_flag && aztool_mode_flag == 0 && disp_enable) lv_task_handler();
    unsigned long work_time = millis() - start_time;
    if (work_time < 30) { vTaskDelay(30 - work_time); }
  }
}



// コンストラクタ
AzCommon::AzCommon() {
}

// 共通処理の初期化
void AzCommon::common_start() {
    // M5Stack初期化
    M5.begin(true, true, true, true);
    M5.Axp.SetLed(false); // サイドの緑LEDを消す
    M5.Lcd.setRotation(0); // 画面を横向きにする
    // 乱数初期化
    randomSeed(millis());
    // ファイルシステム初期化
    if(!SPIFFS.begin(true)){
        ESP_LOGD(LOG_TAG, "SPIFFS begin error\n");
        return;
    }
    // WIFI 接続フラグ
    wifi_conn_flag = 0;
    // 押している最中のキーデータ初期化
    int i;
    for (i=0; i<PRESS_KEY_MAX; i++) {
        press_key_list[i].action_type = -1;
        press_key_list[i].key_num = -1;
        press_key_list[i].key_id = -1;
        press_key_list[i].layer_id = -1;
        press_key_list[i].press_time = -1;
        press_key_list[i].unpress_time = -1;
    }
    last_touch_x = -1;
    last_touch_y = -1;

    // ioエキスパンダピン
    ioxp_sda = -1;
    ioxp_scl = -1;
    ioxp_hz = 400000;
    // ioエキスパンダフラグ
    for (i=0; i<8; i++) {
      ioxp_status[i] = -1;
      ioxp_hash[i] = 0;
    }

    // macアドレス設定ステータス
    blemac_stat = 0;

    // マウスパッドステータス
    mouse_pad_status = 0;
    // マウスのスクロールボタンが押されているか
    mouse_scroll_flag = false;
    // リスタートフラグ
    restart_flag = -1;
    restart_index = -1;
    // メニューを表示しているかどうか
    menu_mode_flag = false;
    // aztoolで作業中かどうか
    aztool_mode_flag = 0;
    // LVGL表示インデックス
    lvgl_loop_index = 0;
    // ディスプレイの向き
    disp_rotation = 0;
    // ディスプレイ初期化フラグ
    disp_enable = false;
    // ソフトキーが押された時入る変数
    soft_click_layer = -1;
    soft_click_key = -1;
    // キーボードのステータス
    keyboard_status = 0;
    // 再生用バック処理
    xTaskCreatePinnedToCore(background_loop, "bgloop", 2048, NULL, 20, NULL, 0);
    // ディスプレイループ
    xTaskCreatePinnedToCore(background_disp_loop, "dploop", 2048, NULL, 20, NULL, 1);
}

// リスタート用ループ処理
void AzCommon::restart_loop() {
    // 3回ループ処理をしてからリスタート(ボタンを押してスグリスタートだと画面が固まったように見えるから)
    if (restart_flag >= 0) {
        restart_index++;
        if (restart_index > 3) {
            common_cls.change_mode(restart_flag);
        }
    }
}


// WIFI 接続
void AzCommon::wifi_connect() {
    // WIFI 接続
    int i;
    if (wifi_data_length <= 0) {
        ESP_LOGD(LOG_TAG, "wifi : not setting\r\n");
        wifi_conn_flag = 0;
        return;
    }
    // 液晶にWiFi接続中画面を表示する
    if (common_cls.on_tft_unit()) disp->view_wifi_conn();
    // WiFiに接続(一番電波が強いAPへ接続)
    for (i=0; i<wifi_data_length; i++) {
        wifiMulti.addAP(wifi_data[i].ssid, wifi_data[i].pass);
        ESP_LOGD(LOG_TAG, "wifi : [%S] [%S]", wifi_data[i].ssid, wifi_data[i].pass);
    }
    ESP_LOGD(LOG_TAG, "wifi : connect start\r\n");
    i = 0;
    wifi_conn_flag = 1;
    // while (WiFi.status() != WL_CONNECTED) {
    while (wifiMulti.run() != WL_CONNECTED) {
        ESP_LOGD(LOG_TAG, "wifi : try %D\r\n", i);
        delay(1000);
        i++;
        if (i > 20) {
            ESP_LOGD(LOG_TAG, "wifi : connect error\r\n");
            wifi_conn_flag = 0;
            break;
        }
    }
    if (wifi_conn_flag) {
        ESP_LOGD(LOG_TAG, "wifi : connect OK!\r\n");
    }
}

// wifiアクセスポイントのリストをJSONで取得
String AzCommon::get_wifi_ap_list_json() {
    String res = "{\"list\": [";
    int ssid_num;
    String auth_open;
    ssid_num = WiFi.scanNetworks();
    if (ssid_num == 0) {
        ESP_LOGD(LOG_TAG, "get_wifi_ap_list: no networks");
    } else {
        ESP_LOGD(LOG_TAG, "get_wifi_ap_list: %d\r\n", ssid_num);
        for (int i = 0; i < ssid_num; ++i) {
            auth_open = ((WiFi.encryptionType(i) == WIFI_AUTH_OPEN)? "true": "false");
            if (i > 0) res += ",";
            res += "{\"ssid\": \"" + WiFi.SSID(i) + "\", \"rssi\": \"" + WiFi.RSSI(i) + "\", \"auth_open\": " + auth_open + "}";
            delay(10);
        }
    }
    res += "]}";
    ESP_LOGD(LOG_TAG, "%S", res);
    return res;
}

// URLからドメイン名だけ抜き出す
void AzCommon::get_domain(char *url, char *domain_name) {
    int i = 0, j = 0, s = 0;
    while (url[i] > 0) {
        if (s == 0 && url[i] == '/') {
            s = 1;
        } else if (s == 1 && url[i] == '/') {
            s = 2;
        } else if (s == 2) {
            if (url[i] == '/' || url[i] == ':') {
                s = 3;
                domain_name[j] = 0x00;
                break;
            } else if (url[i] == '@') {
                j = 0;
                domain_name[j] = 0x00;
            } else {
                domain_name[j] = url[i];
                j++;
            }
        } else {
            s = 0;
        }
        i++;
    }
    domain_name[j] = 0x00;
}

// webリクエストを送信する(シンプルなGETリクエストのみ)
String AzCommon::send_webhook_simple(char *url) {
    int status_code;
    String res;
    // httpリクエスト用オブジェクト
    HTTPClient http;
    http.begin(url);
    // GET
    status_code = http.GET();
    ESP_LOGD(LOG_TAG, "http status: %D\r\n", status_code);
    if (status_code == HTTP_CODE_OK) {
        res = http.getString();
    } else {
        res = "";
    }
    http.end();
    return res;
}


// POSTでファイルの内容を送信する
String AzCommon::send_webhook_post_file(char *url, char *file_path) {
    if (!wifi_conn_flag) return "";
    status_led_mode = 3;
    // httpリクエスト用オブジェクト
    int status_code;
    HTTPClient_my http;
    String res_body;
    http.begin(url);
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");
    status_code = http.sendPostFile(file_path);
    if (status_code == HTTP_CODE_OK) {
        res_body = http.getString();
    } else {
        res_body = "";
    }
    http.end();
    return res_body;
}

// webリクエストを送信する
String AzCommon::send_webhook(char *setting_data) {
    ESP_LOGD(LOG_TAG, "send_webhook start: %S %D\r\n", setting_data, strlen(setting_data));
    int n = strlen(setting_data) + 1;
    char jchar[n];
    memcpy(jchar, setting_data, n);
    DynamicJsonDocument setting_doc(n + 512);
    deserializeJson(setting_doc, jchar);
    JsonObject prm = setting_doc.as<JsonObject>();
    String url = prm["url"].as<String>();
    int m = url.length() + 1;
    char url_char[m];
    url += "\0";
    url.toCharArray(url_char, m);
    if (url.startsWith("http://") || url.startsWith("https://")) {
        return http_request(url_char, prm);        
    } else {
        return String("url error");
    }
}

// HTTPリクエストを送信する
String AzCommon::http_request(char *url, const JsonObject &prm) {
    if (!wifi_conn_flag) return "";
    status_led_mode = 3;
    ESP_LOGD(LOG_TAG, "http : %S\r\n", url);
    char hkey[64];
    int i;
    int header_len = prm["header"].size();
    // httpリクエスト用オブジェクト
    HTTPClient http;
    http.begin(url);
    // ヘッダー送信
    for (i=0; i<header_len; i++) {
        String hk = prm["header"][i]["key"].as<String>();
        String hv = prm["header"][i]["value"].as<String>();
        hk.toCharArray(hkey, 64);
        hv.toCharArray(webhook_buf, WEBFOOK_BUF_SIZE);
        http.addHeader(hkey, webhook_buf);
    }
    // POSTデータがあればPOST
    String pd = prm["post"].as<String>();
    int status_code;
    if (pd.length() > 0) {
        // POST
        pd.toCharArray(webhook_buf, WEBFOOK_BUF_SIZE);
        status_code = http.POST(webhook_buf);
    } else {
        // GET
        status_code = http.GET();
    }
    ESP_LOGD(LOG_TAG, "http status: %D\r\n", status_code);
    int ot = prm["keyoutput"].as<signed int>();
    if (status_code == HTTP_CODE_OK) {
        String res_body = http.getString();
        http.end();
        status_led_mode = 1;
        if (ot == 0) {
            return "";
        } else if (ot == 1) {
            return String(status_code);
        } else if (ot == 2) {
            return res_body;
        }
        return "";
    } else {
        http.end();
        status_led_mode = 1;
        if (ot == 1) {
            return String(status_code);
        }
        String res_buf = "status code:" + String(status_code);
        return res_buf;
    }
}



// 区切り文字で分割する
int AzCommon::split(String data, char delimiter, String *dst){
    int index = 0;
    int arraySize = (sizeof(data)/sizeof((data)[0]));  
    int datalength = data.length();
    for (int i = 0; i < datalength; i++) {
        char tmp = data.charAt(i);
        if ( tmp == delimiter ) {
            index++;
            if ( index > (arraySize - 1)) return -1;
        }
        else dst[index] += tmp;
    }
    return (index + 1);
}

// レイヤー名、キー名から番号を抜き出す
int split_num(char *c) {
    // _の文字まで進める
    while (c[0] != 0x5F && c[0] != 0x00) {
        c++;
    }
    if (c[0] == 0x5F) c++;
    return String(c).toInt();
}

// JSONデータを読み込む
void AzCommon::load_setting_json() {
    // セッティングJSONを保持する領域
    SpiRamJsonDocument setting_doc(SETTING_JSON_BUF_SIZE);
    JsonObject setting_obj;

    // ファイルが無い場合はデフォルトファイル作成
    if (!SPIFFS.exists(SETTING_JSON_PATH)) {
        create_setting_json();
        ESP.restart(); // ESP32再起動
        return;
    }
    // ファイルオープン
    File json_file = SPIFFS.open(SETTING_JSON_PATH);
    if(!json_file){
        // ファイルが開けなかった場合はデフォルトファイル作り直して再起動
        delete_all(); // 全てのファイルを削除
        ESP.restart(); // ESP32再起動
        return;
    }
    // 読み込み＆パース
    DeserializationError err = deserializeJson(setting_doc, json_file);
    if (err) {
        delete_all(); // 全てのファイルを削除
        ESP.restart(); // ESP32再起動
        return;
    }
    json_file.close();
    // オブジェクトを保持
    setting_obj = setting_doc.as<JsonObject>();

    // キーボードタイプは必須なので項目が無ければ設定ファイル作り直し(設定ファイル壊れた時用)
    if (!setting_obj.containsKey("keyboard_type")) {
        delete_all(); // 全てのファイルを削除
        ESP.restart(); // ESP32再起動
        return;
    }

    // ディスプレイの向き
    disp_rotation = setting_obj["disp_rotation"].as<signed int>();

    // キーボードタイプの番号を取得する
    String ktype = setting_obj["keyboard_type"].as<String>();
    get_keyboard_type_int(setting_obj["keyboard_type"].as<String>());

    // キーボードの名前を取得する
    String keynamestr;
    keynamestr = setting_obj["keyboard_name"].as<String>();
    keynamestr.toCharArray(keyboard_name_str, 31);

    // キーボードのタイプがeep_dataと一致しなければ現在選択しているキーボードと、設定ファイルのキーボードが一致していないので
    // 現在選択しているキーボードのデフォルト設定ファイルを作成して再起動
    if (!ktype.equals(eep_data.keyboard_type)) {
        delete_all(); // 全てのファイルを削除
        ESP.restart(); // ESP32再起動
        return;
    }

    // HID 設定
    String hidstr;
    if (setting_obj.containsKey("vendorId")) {
        hidstr = setting_obj["vendorId"].as<String>();
        hid_vid = (uint16_t) strtol(&hidstr[2], NULL, 16);
    } else {
        hid_vid = BLE_HID_VID;
    }
    if (setting_obj.containsKey("productId")) {
        hidstr = setting_obj["productId"].as<String>();
        hid_pid = (uint16_t) strtol(&hidstr[2], NULL, 16);
    } else {
        hid_pid = BLE_HID_PID;
    }
    
    // デフォルトのレイヤー番号設定
    default_layer_no = setting_obj["default_layer"].as<signed int>();
    // 今選択してるレイヤーをデフォルトに
    select_layer_no = default_layer_no;
    // オプションタイプの番号を取得
    option_type_int = 0;
    if (setting_obj.containsKey("option_set") && setting_obj["option_set"].containsKey("type")) {
        get_option_type_int(setting_obj);
    }
    // オープニングムービー再生取得
    op_movie_flag = false;
    if (setting_obj["option_set"]["op_movie"].as<signed int>() > 0) op_movie_flag = true;
    // 入力ピン情報取得
    int i, j, k, m, n, o, p, r;
    direct_len = setting_obj["keyboard_pin"]["direct"].size();
    touch_len = setting_obj["keyboard_pin"]["touch"].size();
    ioxp_len = setting_obj["keyboard_pin"]["ioxp"].size();
    direct_list = new short[direct_len];
    for (i=0; i<direct_len; i++) {
        direct_list[i] = setting_obj["keyboard_pin"]["direct"][i].as<signed int>();
    }
    touch_list = new short[touch_len];
    for (i=0; i<touch_len; i++) {
        touch_list[i] = setting_obj["keyboard_pin"]["touch"][i].as<signed int>();
    }
    ioxp_list = new short[ioxp_len];
    for (i=0; i<ioxp_len; i++) {
        ioxp_list[i] = setting_obj["keyboard_pin"]["ioxp"][i].as<signed int>();
    }
    // IOエキスパンダピン
    if (setting_obj.containsKey("i2c_set") && setting_obj["i2c_set"].size() == 2) {
        ioxp_sda = setting_obj["i2c_set"][0].as<signed int>();
        ioxp_scl = setting_obj["i2c_set"][1].as<signed int>();
        ioxp_hz = setting_obj["i2c_set"][2].as<signed int>();
    } else {
        if (keyboard_type_int == 2) { // AZ-M5ortho
            ioxp_sda = 26;
            ioxp_scl = 14;
            ioxp_hz = 400000;
        } else if (keyboard_type_int == 3) { // えむごっち
            ioxp_sda = 26;
            ioxp_scl = 25;
            ioxp_hz = 400000;
        } else {
            ioxp_sda = -1;
            ioxp_scl = -1;
            ioxp_hz = 400000;
        }
    }
    // えむごっちで、25,26になってるヤツは逆なので勝手の元に戻す(昔の設定ファイルのままの人は逆で保存されちゃってる)
    if (keyboard_type_int == 3 && ioxp_sda == 25 && ioxp_scl == 26) {
        ioxp_sda = 26;
        ioxp_scl = 25;
    }

    // I2cオプションの設定
    int i2c_key_len = 0;
    if (setting_obj.containsKey("i2c_option") && setting_obj["i2c_option"].size()) {
        // 有効になっているオプションの数を数える
        k = setting_obj["i2c_option"].size();
        i2copt_len = 0;
        for (i=0; i<k; i++) {
            if (setting_obj["i2c_option"][i].containsKey("enable") &&
                    setting_obj["i2c_option"][i]["enable"].as<signed int>() == 1) {
                i2copt_len++;
            }
        }
        i2copt = new i2c_option[i2copt_len];
        // オプションのデータを取得
        j = 0;
        for (i=0; i<k; i++) {
            // 有効になっていないオプションは無視
            if (!setting_obj["i2c_option"][i].containsKey("enable") ||
                    setting_obj["i2c_option"][i]["enable"].as<signed int>() != 1) continue;
            // オプションのタイプ
            if (setting_obj["i2c_option"][i].containsKey("type")) {
                i2copt[i].opt_type = setting_obj["i2c_option"][i]["type"].as<signed int>();
            } else {
                i2copt[i].opt_type = 0;
            }
            // キーマッピング設定
            if (setting_obj["i2c_option"][i].containsKey("map") &&
                    setting_obj["i2c_option"][i]["map"].size() ) {
                i2copt[i].map_len = setting_obj["i2c_option"][i]["map"].size();
                i2copt[i].map = new short[i2copt[i].map_len];
                for (p=0; p<i2copt[i].map_len; p++) {
                    i2copt[i].map[p] = setting_obj["i2c_option"][i]["map"][p].as<signed int>();
                }
            } else {
                i2copt[i].map_len = 0;
            }
            i2c_key_len += i2copt[i].map_len;
            // キー設定の開始番号
            if (setting_obj["i2c_option"][i].containsKey("map_start")) {
                i2copt[i].map_start = setting_obj["i2c_option"][i]["map_start"].as<signed int>();
            } else {
                i2copt[i].map_start = 0;
            }
            // 使用するIOエキスパンダの情報
            if (setting_obj["i2c_option"][i].containsKey("ioxp") && setting_obj["i2c_option"][i]["ioxp"].size()) {
                i2copt[i].ioxp_len = setting_obj["i2c_option"][i]["ioxp"].size();
                i2copt[i].ioxp = new ioxp_option[i2copt[i].ioxp_len];
                for (n=0; n<i2copt[i].ioxp_len; n++) {
                    // IOエキスパンダのアドレス
                    i2copt[i].ioxp[n].addr = setting_obj["i2c_option"][i]["ioxp"][n]["addr"];
                    // row に設定しているピン
                    if (setting_obj["i2c_option"][i]["ioxp"][n].containsKey("row") &&
                            setting_obj["i2c_option"][i]["ioxp"][n]["row"].size() ) {
                        // row に設定されている配列を取得(0～7 以外を無視する)
                        i2copt[i].ioxp[n].row_len = setting_obj["i2c_option"][i]["ioxp"][n]["row"].size();
                        i2copt[i].ioxp[n].row = new uint8_t[i2copt[i].ioxp[n].row_len]; // row のピン番号
                        i2copt[i].ioxp[n].row_output = new uint16_t[i2copt[i].ioxp[n].row_len]; // マトリックスでrow write するデータ
                        i2copt[i].ioxp[n].row_mask = 0x00; // row write する時用のマスク
                        o = 0;
                        for (p=0; p<i2copt[i].ioxp[n].row_len; p++) {
                            r = setting_obj["i2c_option"][i]["ioxp"][n]["row"][p].as<signed int>();
                            // if (r < 0 || r >= 8) continue; // ポートA以外の場合は取得しない
                            i2copt[i].ioxp[n].row[o] = r;
                            o++;
                            i2copt[i].ioxp[n].row_mask |= 0x01 << r;
                        }
                        i2copt[i].ioxp[n].row_len = o;
                        // マトリックス出力する時用のデータ作成
                        for (p=0; p<o; p++) {
                            i2copt[i].ioxp[n].row_output[p] = i2copt[i].ioxp[n].row_mask & ~(0x01 << i2copt[i].ioxp[n].row[p]);
                        }
                    } else {
                        i2copt[i].ioxp[n].row_len = 0;
                    }
                    // col に設定しているピン
                    if (setting_obj["i2c_option"][i]["ioxp"][n].containsKey("col") &&
                            setting_obj["i2c_option"][i]["ioxp"][n]["col"].size() ) {
                        i2copt[i].ioxp[n].col_len = setting_obj["i2c_option"][i]["ioxp"][n]["col"].size();
                        i2copt[i].ioxp[n].col = new uint8_t[i2copt[i].ioxp[n].col_len];
                        for (p=0; p<i2copt[i].ioxp[n].col_len; p++) {
                            i2copt[i].ioxp[n].col[p] = setting_obj["i2c_option"][i]["ioxp"][n]["col"][p].as<signed int>();
                        }
                    } else {
                        i2copt[i].ioxp[n].col_len = 0;
                    }
                    // direct に設定しているピン
                    if (setting_obj["i2c_option"][i]["ioxp"][n].containsKey("direct") &&
                            setting_obj["i2c_option"][i]["ioxp"][n]["direct"].size() ) {
                        i2copt[i].ioxp[n].direct_len = setting_obj["i2c_option"][i]["ioxp"][n]["direct"].size();
                        i2copt[i].ioxp[n].direct = new uint8_t[i2copt[i].ioxp[n].direct_len];
                        for (p=0; p<i2copt[i].ioxp[n].direct_len; p++) {
                            i2copt[i].ioxp[n].direct[p] = setting_obj["i2c_option"][i]["ioxp"][n]["direct"][p].as<signed int>();
                        }
                    } else {
                        i2copt[i].ioxp[n].direct_len = 0;
                    }
                }
            } else {
                i2copt[i].ioxp_len = 0;
            }

        }


    } else {
        i2copt_len = 0;
    }
    
    // key_input_length = 16 * ioxp_len;
    // キーの設定を取得
    // まずは設定の数を取得
    layer_max = 0;
    setting_length = 0;
    soft_setting_length = 0;
    // key_max = (16 * ioxp_len) + direct_len + touch_len + i2c_key_len;
    key_max = (16 * ioxp_len) + direct_len + touch_len; // remap用
    this->get_keymap(setting_obj, "layers"); // 通常キー
    this->get_keymap(setting_obj, "soft_layers"); // ソフトウェアキー
    this->set_setting_remap(); // REMAPに送る用のデータ作成


    // wifiの設定読み出し
    String ssid, pass;
    wifi_data_length = setting_obj["wifi"].size(); // wifiの設定数
    wifi_data = new azsetting_wifi[wifi_data_length];
    for (i=0; i<wifi_data_length; i++) {
        ssid = setting_obj["wifi"][i]["ssid"].as<String>();
        pass = setting_obj["wifi"][i]["pass"].as<String>();
        m = ssid.length() + 1;
        wifi_data[i].ssid = new char[m];
        ssid.toCharArray(wifi_data[i].ssid, m);
        m = pass.length() + 1;
        wifi_data[i].pass = new char[m];
        pass.toCharArray(wifi_data[i].pass, m);
    }
    for (i=0; i<wifi_data_length; i++) {
        ESP_LOGD(LOG_TAG, "wifi setting [ %S , %S ]\n", wifi_data[i].ssid, wifi_data[i].pass );
    }

    // アクセスポイントのパスワード
    String ap_pass = setting_obj["ap"]["pass"].as<String>();
    m = ap_pass.length() + 1;
    ap_pass_char = new char[m];
    ap_pass.toCharArray(ap_pass_char, m);

    // 設定されているデフォルトレイヤー取得
    default_layer_no = setting_obj["default_layer"].as<signed int>();

    // キーボードの言語取得
    keyboard_language = setting_obj["keyboard_language"].as<signed int>();

    // RGBLED設定の取得
    rgb_pin = -1;
    if (setting_obj.containsKey("rgb_pin")) rgb_pin = setting_obj["rgb_pin"].as<signed int>();
    rgb_len = -1;
    if (setting_obj.containsKey("rgb_len")) rgb_len = setting_obj["rgb_len"].as<signed int>();
    matrix_row = -1;
    if (setting_obj.containsKey("matrix_row")) matrix_row = setting_obj["matrix_row"].as<signed int>();
    matrix_col = -1;
    if (setting_obj.containsKey("matrix_col")) matrix_col = setting_obj["matrix_col"].as<signed int>();

    led_num_length = setting_obj["led_num"].size();
    key_matrix_length = setting_obj["key_matrix"].size();
    if (led_num_length > 0) {
        led_num = new int8_t[led_num_length];
        for (i=0; i<led_num_length; i++) {
            led_num[i] = setting_obj["led_num"][i].as<signed int>();
        }
    }
    if (key_matrix_length > 0) {
        key_matrix = new int8_t[key_matrix_length];
        for (i=0; i<key_matrix_length; i++) {
            key_matrix[i] = setting_obj["key_matrix"][i].as<signed int>();
        }
    }

}

// キーマップ用に確保しているメモリを解放
void AzCommon::clear_keymap() {
    int i;
    setting_normal_input normal_input;
    for (i=0; i<setting_length; i++) {
        delete setting_press[i].key_name; // 表示名クリア
        if (setting_press[i].action_type == 1) { // 通常キー
            memcpy(&normal_input, setting_press[i].data, sizeof(setting_normal_input));
            delete[] normal_input.key;
            delete setting_press[i].data;
        } else if (setting_press[i].action_type == 2) { // テキスト入力
            delete[] setting_press[i].data;
        } else if (setting_press[i].action_type == 3) { // レイヤー切り替え
            delete setting_press[i].data;
        } else if (setting_press[i].action_type == 4) { // WEBフック
            delete[] setting_press[i].data;
        } else if (setting_press[i].action_type == 5) { // マウス移動
            delete setting_press[i].data;
        } else if (setting_press[i].action_type == 6) { // 暗記ボタン
            delete[] setting_press[i].data;
        } else if (setting_press[i].action_type == 7) { // LED設定ボタン
            delete setting_press[i].data;
        } else if (setting_press[i].action_type == 8) { // 打鍵設定ボタン
            delete setting_press[i].data;
        }
    }
    delete[] setting_press;
    // レイヤー名も解放
    for (i=0; i<layer_name_length; i++) {
        delete layer_name_list[i].layer_name;
    }
    delete[] layer_name_list;
}

// JSONデータからキーマップの情報を読み込む
void AzCommon::get_keymap(JsonObject setting_obj, char *key_name) {
    int i, j, k, l, m, at, s;
    char lkey[16];
    char kkey[16];
    uint16_t lnum, knum, slen, lmax;
    JsonObject::iterator it_l;
    JsonObject::iterator it_k;
    JsonObject layers, keys;
    JsonObject press_obj;
    String text_str;
    setting_normal_input normal_input;
    setting_layer_move layer_move_input;
    setting_mouse_move mouse_move_input;
    setting_key_press *setpt;
    layer_name_data *lndt;
    // 設定項目が存在しなければ何もしない
    if (!setting_obj.containsKey(key_name)) return;
    // まずはキー設定されている数を取得
    layers = setting_obj[key_name].as<JsonObject>();
    slen = 0;
    lmax = 0;
    for (it_l=layers.begin(); it_l!=layers.end(); ++it_l) {
        if (!setting_obj[key_name][it_l->key().c_str()].containsKey("keys")) continue;
        slen += setting_obj[key_name][it_l->key().c_str()]["keys"].size();
        lmax++;
    }
    ESP_LOGD(LOG_TAG, "setting total %D\n", slen);
    // キー設定が存在しなければ何もしない
    if (slen == 0) return;
    // 設定数分メモリ確保
    ESP_LOGD(LOG_TAG, "mmm: %D %D\n", heap_caps_get_free_size(MALLOC_CAP_32BIT), heap_caps_get_free_size(MALLOC_CAP_8BIT) );
    setpt = new setting_key_press[slen];
    ESP_LOGD(LOG_TAG, "mmm: %D %D\n", heap_caps_get_free_size(MALLOC_CAP_32BIT), heap_caps_get_free_size(MALLOC_CAP_8BIT) );
    // キー設定読み込み
    i = 0;
    l = 0;
    lndt = new layer_name_data[lmax];
    for (it_l=layers.begin(); it_l!=layers.end(); ++it_l) {
        sprintf(lkey, "%S", it_l->key().c_str());
        lnum = split_num(lkey);
        lndt[l].layer = lnum;
        if (setting_obj[key_name][lkey].containsKey("name")) {
            // レイヤー名があればレイヤー名取得
            text_str = setting_obj[key_name][lkey]["name"].as<String>();
            m = text_str.length() + 1;
            lndt[l].layer_name = new char[m];
            text_str.toCharArray(lndt[l].layer_name, m);
        } else {
            lndt[l].layer_name = new char[1];
            strcpy(lndt[l].layer_name, "");
        }
        l++;
        keys = setting_obj[key_name][lkey]["keys"].as<JsonObject>();
        for (it_k=keys.begin(); it_k!=keys.end(); ++it_k) {
            sprintf(kkey, "%S", it_k->key().c_str());
            knum = split_num(kkey);
            // ESP_LOGD(LOG_TAG, "layers %S %S [ %D %D ]\n", lkey, kkey, lnum, knum);
            press_obj = setting_obj[key_name][lkey]["keys"][kkey]["press"].as<JsonObject>();
            setpt[i].layer = lnum;
            setpt[i].key_num = knum;
            if (setting_obj[key_name][lkey]["keys"][kkey].containsKey("name")) {
                // 表示名があれば表示名取得
                text_str = setting_obj[key_name][lkey]["keys"][kkey]["name"].as<String>();
                m = text_str.length() + 1;
                setpt[i].key_name = new char[m];
                text_str.toCharArray(setpt[i].key_name, m);
            } else {
                // 表示名が無ければ空文字を設定
                setpt[i].key_name = new char[1];
                strcpy(setpt[i].key_name, "");
            }
            setpt[i].action_type = press_obj["action_type"].as<signed int>();
            if (setpt[i].action_type == 1) {
                // 通常入力
                normal_input.key_length = press_obj["key"].size();
                normal_input.key = new uint16_t[normal_input.key_length];
                for (j=0; j<normal_input.key_length; j++) {
                      normal_input.key[j] = press_obj["key"][j].as<signed int>();
                }
                if (press_obj.containsKey("repeat_interval")) {
                    normal_input.repeat_interval = press_obj["repeat_interval"].as<signed int>();
                } else {
                    normal_input.repeat_interval = 51;
                }
                if (press_obj.containsKey("hold")) {
                    normal_input.hold = press_obj["hold"].as<signed int>();
                } else {
                    normal_input.hold = 0;
                }
                setpt[i].data = (char *)new setting_normal_input;
                memcpy(setpt[i].data, &normal_input, sizeof(setting_normal_input));
            } else if (setpt[i].action_type == 2) {
                // テキスト入力
                text_str = press_obj["text"].as<String>();
                m = text_str.length() + 1;
                setpt[i].data = new char[m];
                text_str.toCharArray(setpt[i].data, m);
            } else if (setpt[i].action_type == 3) {
                // レイヤー切り替え
                layer_move_input.layer_id = press_obj["layer"].as<signed int>();
                layer_move_input.layer_type = press_obj["layer_type"].as<signed int>();
                if (layer_move_input.layer_type == 0) layer_move_input.layer_type = 0x51; // 切り替え方法の指定が無かった場合はMO(押している間切り替わる)
                setpt[i].data = (char *)new setting_layer_move;
                memcpy(setpt[i].data, &layer_move_input, sizeof(setting_layer_move));
            } else if (setpt[i].action_type == 4) {
                // WEBフック
                text_str = "";
                serializeJson(press_obj["webhook"], text_str);
                m = text_str.length() + 1;
                setpt[i].data = new char[m];
                text_str.toCharArray(setpt[i].data, m);
                
            } else if (setpt[i].action_type == 5) {
                // マウス移動
                mouse_move_input.x = press_obj["move"]["x"].as<signed int>();
                mouse_move_input.y = press_obj["move"]["y"].as<signed int>();
                mouse_move_input.wheel = press_obj["move"]["wheel"].as<signed int>();
                mouse_move_input.hWheel = press_obj["move"]["hWheel"].as<signed int>();
                mouse_move_input.speed = press_obj["move"]["speed"].as<signed int>();
                setpt[i].data = (char *)new setting_mouse_move;
                memcpy(setpt[i].data, &mouse_move_input, sizeof(setting_mouse_move));

            } else if (setpt[i].action_type == 6) {
                // 暗記ボタン
                text_str = press_obj["ankey_file"].as<String>();
                m = text_str.length() + 1;
                setpt[i].data = new char[m];
                text_str.toCharArray(setpt[i].data, m);

            } else if (setpt[i].action_type == 7) {
                // LED設定ボタン
                setpt[i].data = new char;
                *setpt[i].data = press_obj["led_setting_type"].as<signed int>();
                
            } else if (setpt[i].action_type == 8) {
                // 打鍵設定ボタン
                setpt[i].data = new char;
                *setpt[i].data = press_obj["dakagi_settype"].as<signed int>();

            }
            i++;
        }
    }
    if (strcmp(key_name, "layers") == 0) { // 通常のキー設定
        setting_press = setpt;
        setting_length = slen;
        layer_max = lmax;
        // レイヤー名取得
        layer_name_list = lndt;
        layer_name_length = lmax;

    } else if (strcmp(key_name, "soft_layers") == 0) { // ソフトウェアキー設定
        soft_setting_press = setpt;
        soft_setting_length = slen;
        // レイヤー名取得
        soft_layer_name_list = lndt;
        soft_layer_name_length = lmax;
    }
    // setting_press = setpt;
  
    // ログに出力して確認
    /*
    for (i=0; i<setting_length; i++) {
        if (setting_press[i].action_type == 1) {
            memcpy(&normal_input, setting_press[i].data, sizeof(setting_normal_input));
            ESP_LOGD(LOG_TAG, "setting_press %D %D %D %D [%D, %D]", i, setting_press[i].layer, setting_press[i].key_num, setting_press[i].action_type, normal_input.key_length, normal_input.repeat_interval);
            for (j=0; j<normal_input.key_length; j++) {
                ESP_LOGD(LOG_TAG, "setting_press %D ", normal_input.key[j]);
            }
        } else if (setting_press[i].action_type == 2) {
            ESP_LOGD(LOG_TAG, "setting_press %D %D %D %D [ %S ]\n", i, setting_press[i].layer, setting_press[i].key_num, setting_press[i].action_type, setting_press[i].data);
        } else if (setting_press[i].action_type == 3) {
            ESP_LOGD(LOG_TAG, "setting_press %D %D %D %D [ %D ]\n", i, setting_press[i].layer, setting_press[i].key_num, setting_press[i].action_type, *setting_press[i].data);
        } else if (setting_press[i].action_type == 4) {
            ESP_LOGD(LOG_TAG, "setting_press %D %D %D %D [ %S ]\n", i, setting_press[i].layer, setting_press[i].key_num, setting_press[i].action_type, setting_press[i].data);
        } else if (setting_press[i].action_type == 5) {
            memcpy(&mouse_move_input, setting_press[i].data, sizeof(setting_mouse_move));
            ESP_LOGD(LOG_TAG, "setting_press %D %D %D %D [ %D, %D, %D ]\n", i, setting_press[i].layer, setting_press[i].key_num, setting_press[i].action_type, mouse_move_input.x, mouse_move_input.y, mouse_move_input.speed);
        } else {
            ESP_LOGD(LOG_TAG, "setting_press %D %D %D %D\n", i, setting_press[i].layer, setting_press[i].key_num, setting_press[i].action_type);
        }
    }
    ESP_LOGD(LOG_TAG, "mmm: %D %D\n", heap_caps_get_free_size(MALLOC_CAP_32BIT), heap_caps_get_free_size(MALLOC_CAP_8BIT) );
    */
}

// REMAP に送る用のデータ作成
void AzCommon::set_setting_remap() {

    int i, j, k, m, at, s;
    setting_normal_input normal_input;
    setting_layer_move layer_move_input;
    setting_mouse_move mouse_move_input;
    // Remapに送る用のデータ作成
    // Serial.printf("layer_max: %d    key_max: %d\n", layer_max, key_max);
    int buf_length = layer_max * key_max * 2;
    setting_remap = new uint8_t[buf_length];
    // バッファ初期化
    for (i=0; i<buf_length; i++) {
        setting_remap[i] = 0x00;
    }
    // キーデータを格納
    for (i=0; i<setting_length; i++) {
        at = setting_press[i].action_type;
        m = (setting_press[i].layer * key_max * 2) + (setting_press[i].key_num * 2);
        if (m >= buf_length) continue; // 領域を超えているキー設定はオプション用なので無視
        if (at == 1) {
            // 通常キー
            memcpy(&normal_input, setting_press[i].data, sizeof(setting_normal_input));
            if (normal_input.key_length < 1) continue;
            s = normal_input.key[0];
            if (normal_input.hold) {
                // holdが定義されていたら tap/hold
                s = normal_input.hold << 8; // 上位8ビットはholdのキー
                if (normal_input.key_length) s |= normal_input.key[0] & 0xFF; // 下位8ビットはタップのキー
            } else if (s >= 0xE0 && s <= 0xE7 && normal_input.key_length > 1) {
                // 1キー目がモデファイアで複数キーが登録してある場合 同時押しと判定
                s = 0;
                for (j=0; j<normal_input.key_length; j++) {
                    k = normal_input.key[j];
                    if (k == 0xE0) { s |= 0x0100; } // 左Ctrl 
                    else if (k == 0xE1) { s |= 0x0200; } // 左Shift
                    else if (k == 0xE2) { s |= 0x0400; } // 左Alt
                    else if (k == 0xE3) { s |= 0x0800; } // 左GUI
                    else if (k == 0xE4) { s |= 0x1100; } // 右Ctrl
                    else if (k == 0xE5) { s |= 0x1200; } // 右Shift
                    else if (k == 0xE6) { s |= 0x1400; } // 右Alt
                    else if (k == 0xE7) { s |= 0x1800; } // 右GUI
                    else { s |= k & 0xFF; } // それ以外は通常キーとして下位8ビットに入れる
                }
            } else {
                // それ以外は通常キー
                if (s == 0x4001) s = 0xF4; // 左クリック
                if (s == 0x4002) s = 0xF5; // 右クリック
                if (s == 0x4004) s = 0xF6; // 中クリック
                if (s == 0x4005) s = 0xFD; // スクロールボタン
            }
            setting_remap[m] = (s >> 8) & 0xff;
            setting_remap[m + 1] = s & 0xff;
        } else if (at == 3) {
            // レイヤー切り替え
            memcpy(&layer_move_input, setting_press[i].data, sizeof(setting_layer_move));
            s = layer_move_input.layer_type; // 切り替えのタイプ
            if (s != 0x50 && s != 0x51 && s != 0x52 && s != 0x53 && s != 0x54 && s != 0x58) s = 0x51; // 不明な指定はMOと判定
            setting_remap[m] = s;
            setting_remap[m + 1] = layer_move_input.layer_id;
        } else if (at == 5) {
            // マウス移動
            memcpy(&mouse_move_input, setting_press[i].data, sizeof(setting_mouse_move));
            s = 0xF0;
            if (mouse_move_input.y < 0) s = 0xF0;
            if (mouse_move_input.y > 0) s = 0xF1;
            if (mouse_move_input.x < 0) s = 0xF2;
            if (mouse_move_input.x > 0) s = 0xF3;
            setting_remap[m] = (s >> 8) & 0xff;
            setting_remap[m + 1] = s & 0xff;
        }
    }
    /* REMAP用バッファデバッグ表示
    Serial.printf("remap_buf (%d):", buf_length);
    for (i=0; i<buf_length; i++) {
        Serial.printf(" %02x", setting_remap[i]);
    }
    Serial.printf("\n");
    */
}

// REMAPで受け取ったデータをJSONデータに書き込む
void AzCommon::remap_save_setting_json() {
    // セッティングJSONを保持する領域
    SpiRamJsonDocument setting_doc(SETTING_JSON_BUF_SIZE);
    JsonObject setting_obj;
    int i, j, k, m, h, l, s;
    int buf_length = layer_max * key_max * 2;
    JsonObject layer_obj;
    JsonObject keys_obj;
    JsonObject key_obj;
    JsonObject press_obj;
    JsonObject move_obj;
    JsonArray keyarray_obj;
    char lname[32];
    char kname[32];

    // ファイルオープン
    File open_file = SPIFFS.open(SETTING_JSON_PATH);
    if(!open_file) return;
    // 読み込み＆パース
    DeserializationError err = deserializeJson(setting_doc, open_file);
    if (err) return;
    open_file.close();

    // JSONオブジェクトを保持
    setting_obj = setting_doc.as<JsonObject>();

    // 入力モードは無変換の日本語キーボード固定
    setting_obj["keyboard_language"] = 0;
    keyboard_language = 0;

    // キーマップ情報の配列を作成
    setting_obj["layers"].clear();
    for (i=0; i<layer_max; i++) {
        sprintf(lname, "layer_%d", i);
        layer_obj = setting_obj["layers"].createNestedObject(lname);
        layer_obj["name"].set(lname);
        keys_obj = setting_obj["layers"][lname].createNestedObject("keys");
        for (j=0; j<key_max; j++) {
            m = (i * key_max * 2) + (j * 2);
            h = setting_remap[m];
            l = setting_remap[m + 1];
            k = (h << 8) | l;
            if (k == 0) continue;
            // Serial.printf("key: layer=%d key=%d mem=%d id=%d\n", i, j, m, k);
            sprintf(kname, "key_%d", j);
            key_obj = setting_obj["layers"][lname]["keys"].createNestedObject(kname);
            press_obj = setting_obj["layers"][lname]["keys"][kname].createNestedObject("press");
            press_obj["action_type"] = 0;
            if (k <= 0x00E7) {
                // 通常キー
                press_obj["action_type"] = 1; // 通常キー
                press_obj["repeat_interval"] = 51; // 連打無し
                keyarray_obj = setting_obj["layers"][lname]["keys"][kname]["press"].createNestedArray("key");
                keyarray_obj.add(k);
            } else if ((h >= 0x01 && h <= 0x0F) || (h >= 0x11 && h <= 0x1F)) { // 左モデファイア || 右モデファイア
                // モデファイア 同時押し
                press_obj["action_type"] = 1; // 通常キー
                press_obj["repeat_interval"] = 51; // 連打無し
                keyarray_obj = setting_obj["layers"][lname]["keys"][kname]["press"].createNestedArray("key");
                if ((h >> 4) == 0x00) { // 左モデファイア
                    if (h & 0x01) keyarray_obj.add(0xE0); // 左Ctrl
                    if (h & 0x02) keyarray_obj.add(0xE1); // 左Shift
                    if (h & 0x04) keyarray_obj.add(0xE2); // 左Alt
                    if (h & 0x08) keyarray_obj.add(0xE3); // 左GUI
                } else if ((h >> 4) == 0x01) { // 右モデファイア
                    if (h & 0x01) keyarray_obj.add(0xE4); // 右Ctrl
                    if (h & 0x02) keyarray_obj.add(0xE5); // 右Shift
                    if (h & 0x04) keyarray_obj.add(0xE6); // 右Alt
                    if (h & 0x08) keyarray_obj.add(0xE7); // 右GUI
                }
                keyarray_obj.add(l); // 下位8ビットを通常キー
            } else if ((h >= 0x61 && h <= 0x6F) || (h >= 0x71 && h <= 0x7F) || (h >= 0x41 && h <= 0x4F) || h == 0x56) { // 左モデファイア || 右モデファイア || レイヤー || swap?
                // Tap / Hold
                press_obj["action_type"] = 1; // 通常キー
                press_obj["repeat_interval"] = 51; // 連打無し
                press_obj["hold"] = h; // モデファイア | レイヤー定義
                keyarray_obj = setting_obj["layers"][lname]["keys"][kname]["press"].createNestedArray("key");
                keyarray_obj.add(l); // 下位8ビットを通常キー
            } else if ((h == 0x50 || h == 0x51 || h == 0x52 || h == 0x53 || h == 0x54 || h == 0x58) && l < layer_max) {
                // レイヤー切り替え
                press_obj["action_type"] = 3; // レイヤー切り替え
                press_obj["layer"] = l; // 切り替え先レイヤー
                press_obj["layer_type"] = h; // 切り替えの種類
            } else if (k == 0xF0 || k == 0xF1 || k == 0xF2 || k == 0xF3) { // マウス移動↑↓←→
                press_obj["action_type"] = 5; // マウス移動
                move_obj = setting_obj["layers"][lname]["keys"][kname]["press"].createNestedObject("move");
                if (k == 0xF0) { move_obj["x"] = 0; move_obj["y"] = -3; } // 上
                if (k == 0xF1) { move_obj["x"] = 0; move_obj["y"] = 3; } // 下
                if (k == 0xF2) { move_obj["x"] = -3; move_obj["y"] = 0; } // 左
                if (k == 0xF3) { move_obj["x"] = 3; move_obj["y"] = 0; } // 右
                move_obj["speed"] = 100; // 移動速度
            } else if (k == 0xF4 || k == 0xF5 || k == 0xF6 || k == 0xFD) { // マウス ボタン1(左クリック)
                press_obj["action_type"] = 1; // 通常キー
                press_obj["repeat_interval"] = 51; // 連打無し
                keyarray_obj = setting_obj["layers"][lname]["keys"][kname]["press"].createNestedArray("key");
                if (k == 0xF4) s = 0x4001; // 左クリック
                if (k == 0xF5) s = 0x4002; // 右クリック
                if (k == 0xF6) s = 0x4004; // 中クリック
                if (k == 0xFD) s = 0x4005; // スクロールボタン
                keyarray_obj.add(s);
            }
        }
    }

    // 変更した内容をファイルに保存
    File save_file = SPIFFS.open(SETTING_JSON_PATH, "w");
    if(!save_file) return;
    serializeJson(setting_doc, save_file);
    save_file.close();

    // メモリ上にあるキーマップを解放
    this->clear_keymap();

    // 新しいキーマップをメモリ上に入れる
    this->get_keymap(setting_obj, "layers");
    this->set_setting_remap(); // REMAPに送る用のデータ作成
}

// キーボードタイプの番号を取得する
void AzCommon::get_keyboard_type_int(String t) {
    if (t.equals("custom")) { keyboard_type_int = 1; } // カスタム
    else if (t.equals("az_m5ortho")) { keyboard_type_int = 2; } // AZ-M5ortho
	else if (t.equals("az_emugotch")) { keyboard_type_int = 3; } // えむごっち
    else { keyboard_type_int = 0; } // 不明なキーボード
}

// ユニットのタイプ番号を取得する
void AzCommon::get_option_type_int(JsonObject setting_obj) {
    String t = setting_obj["option_set"]["type"].as<String>();
    option_type_int = 0;
    if (t.equals("foot_m")) {
        // 踏みキー
        option_type_int = 1;
        if (setting_obj["option_set"]["inversion"].as<signed int>()) {
            foot_inversion = true;
        } else {
            foot_inversion = false;
        }
        
    } else if (t.equals("trackball_m")) {
        // トラックボール
        option_type_int = 2;
        // トラックボールの向きもここで取得
        if (setting_obj["option_set"].containsKey("trackball_direction")) {
            trackball_direction = setting_obj["option_set"]["trackball_direction"].as<signed int>();
        } else {
            trackball_direction = 0;
        }
        // トラックボールの速度も取得
        if (setting_obj["option_set"].containsKey("trackball_speed")) {
            trackball_speed = setting_obj["option_set"]["trackball_speed"].as<signed int>();
        } else {
            trackball_speed = 10;
        }
    } else if (t.equals("display_m")) {
        // AZ-Macro用 液晶モジュール
        option_type_int = 3;
    } else if (t.equals("display_66jp")) {
        // AZ-66JP用 液晶モジュール
        option_type_int = 4;
    }
}


// ファイルを開いてテキストをロードする
int AzCommon::read_file(char *file_path, String &read_data) {
    ESP_LOGD(LOG_TAG, "read file: %S\n", file_path);
    // ファイルが無ければエラー
    if (!SPIFFS.exists(file_path)) {
        ESP_LOGD(LOG_TAG, "read_file not file: %S\n", file_path);
        return 0;
    }
    // ファイルオープン
    File fp = SPIFFS.open(file_path, "r");
    if(!fp){
        ESP_LOGD(LOG_TAG, "read_file open error: %S\n", file_path);
        return 0;
    }
    // 読み込み
    read_data = fp.readString();
    fp.close();
    return 1;
}

// テキストをファイルに書き込む
int AzCommon::write_file(char *file_path, String &write_data) {
    // 書込みモードでファイルオープン
      ESP_LOGD(LOG_TAG, "write_file open path: %S\n", file_path);
    File fp = SPIFFS.open(file_path, FILE_WRITE);
    if(!fp){
        ESP_LOGD(LOG_TAG, "write_file open error: %S\n", file_path);
        return 0;
    }
    // 書込み
    ESP_LOGD(LOG_TAG, "save");
    if(!fp.print(write_data)){
        ESP_LOGD(LOG_TAG, "write_file print error");
        fp.close();
        return 0;
    }
    fp.close();
    ESP_LOGD(LOG_TAG, "write_file print ok");
    return 1;
}

// ファイルを削除
int AzCommon::remove_file(char *file_path) {
    if (SPIFFS.remove(file_path)) {
        return 1;
    } else {
        return 0;
    }
}

// マウスパッド設定読み込み
int AzCommon::mouse_pad_load() {
    // デフォルト値を入れておく
    mouse_pad_setting.mouse_type = 1;
    mouse_pad_setting.mouse_speed = 50;
    mouse_pad_setting.value_1 = 0;
    mouse_pad_setting.value_2 = 0;
    mouse_pad_status = 0;
    // ファイルが存在したら読み込み
    if (SPIFFS.exists(MOUSE_PAD_SETTING_PATH)) {
        File fp = SPIFFS.open(MOUSE_PAD_SETTING_PATH, "r");
        if (!fp) return 0;
        int i = fp.read((uint8_t *)&mouse_pad_setting, sizeof(mousepad_data_set));
        if (!i) return 0;
        fp.close();
    }
    mouse_pad_status = mouse_pad_setting.mouse_type;
    return 1;
}


// マウスパッド設定保存
int AzCommon::mouse_pad_save() {
    File fp = SPIFFS.open(MOUSE_PAD_SETTING_PATH, "w");
    if (!fp) return 0;
    int i = fp.write((uint8_t *)&mouse_pad_setting, sizeof(mousepad_data_set));
    if (!i) return 0;
    fp.close();
    return 1;
}


// 画面設定読み込み
int AzCommon::moniterset_load() {
    // デフォルト値を入れておく
    moniter_setting.st_brightness = 50;
    moniter_setting.menu_brightness = 50;
    moniter_setting.value_1 = 0;
    moniter_setting.value_2 = 0;
    // ファイルが存在したら読み込み
    if (SPIFFS.exists(MONITER_SETTING_PATH)) {
        File fp = SPIFFS.open(MONITER_SETTING_PATH, "r");
        if (!fp) return 0;
        int i = fp.read((uint8_t *)&moniter_setting, sizeof(moniterset_data_set));
        if (!i) return 0;
        fp.close();
    }
    return 1;
}

// 画面の明るさを設定 set_type= 0=待ち受け / 1=メニュー
void AzCommon::moniter_brightness(int set_type) {
    int s = 0;
    if (set_type == 1) {
        // メニューの明るさ設定取得
        if (moniter_setting.menu_brightness > 0) {
            s = 2500 + ((600 * moniter_setting.menu_brightness) / 100);
        }
        if (s < 2500) s = 2500;
    } else {
        // 待ち受けの明るさ設定取得
        if (moniter_setting.st_brightness > 0) {
            s = 2500 + ((600 * moniter_setting.st_brightness) / 100);
        }
    }
    if (s > 0) {
      M5.Axp.SetDCDC3(true);
      M5.Axp.SetLcdVoltage(s);
    } else {
        if (set_type == 0) M5.Axp.SetDCDC3(false); // 待ち受けの場合のみ バックライトOFF
        M5.Axp.SetLcdVoltage(2500);
    }
}


// 画面設定保存
int AzCommon::moniterset_save() {
    File fp = SPIFFS.open(MONITER_SETTING_PATH, "w");
    if (!fp) return 0;
    int i = fp.write((uint8_t *)&moniter_setting, sizeof(moniterset_data_set));
    if (!i) return 0;
    fp.close();
    return 1;
}

// アドレス１とアドレス２が同じかどうかチェック
bool AzCommon::addrcmp(uint8_t *addr1, uint8_t *addr2) {
    int i;
    for (i=0; i<6; i++) {
        if (addr1[i] != addr2[i]) return false;
    }
    return true;
}


// BLE用macアドレスリスト読み込み
void AzCommon::blemac_load() {
    int i;
    uint8_t addr[6];
    File fp;
    blemac_index = 0;
    // ファイルが無ければデータなしで終了
    if (!SPIFFS.exists(BLEMAC_ADDR_PATH)) {
        blemac_len = 0;
        return;
    }
    // ファイルから読み込み
    fp = SPIFFS.open(BLEMAC_ADDR_PATH, "r");
    // ファイルオープン失敗したらデータなしで終了
    if (!fp) {
        blemac_len = 0;
        return;
    }
    // ファイルサイズから保存件数を計算
    blemac_len = fp.size() / 6;
    // 格納用のメモリ確保
    blemac_list = new ble_mac_addr[blemac_len];
    // データ取得
    for (i=0; i<blemac_len; i++) {
        fp.read(blemac_list[i].addr, 6);
    }
    fp.close();
}

// BLE用macアドレス追加
void AzCommon::blemac_add(uint8_t *addr) {
    // ファイルに新らしいアドレス追記
    File fp = SPIFFS.open(BLEMAC_ADDR_PATH, "a");
    fp.write(addr, 6);
    fp.close();
    // メモリに確保しているリストを解放
    delete blemac_list;
    // 読み込みなおす
    blemac_load();
}


// デフォルトのsetting.jsonを生成する
bool AzCommon::create_setting_json() {
    // 書込みモードでファイルオープン
    File json_file = SPIFFS.open(SETTING_JSON_PATH, FILE_WRITE);
    if(!json_file){
        ESP_LOGD(LOG_TAG, "create_setting_json open error");
        return false;
    }
    // 書込み
    int i;
    if (strcmp(eep_data.keyboard_type, "az_emugotch") == 0) {
        i = json_file.print(setting_azemugotch_default_min_json_bin); // えむごっちのデフォルト設定
    } else if (strcmp(eep_data.keyboard_type, "az_m5ortho") == 0) {
        i = json_file.print(setting_azm5ortho_default_min_json_bin); // AZ-M5orthoのデフォルト設定
    } else if (strcmp(eep_data.keyboard_type, "az_m5orthow") == 0) {
        i = json_file.print(setting_azm5orthow_default_min_json_bin); // AZ-M5orthoのデフォルト設定
    }
    if(!i){
        ESP_LOGD(LOG_TAG, "create_setting_json print error");
        json_file.close();
        return false;
    }
    json_file.close();
    ESP_LOGD(LOG_TAG, "create_setting_json print ok");
    return true;
}

// I2C機器の初期化(戻り値：増えるキーの数)
int AzCommon::i2c_setup(int p, i2c_option *opt) {
    int i, j, k, m, x;
    int r = 0;
    int set_type[16];
    M5.Lcd.printf("i2c_setup: %d\n", p);
    if (opt->opt_type == 1) {
        // IOエキスパンダ
        for (i=0; i<opt->ioxp_len; i++) {
            x = opt->ioxp[i].addr - 32; // アドレス
            // Serial.printf("i2c_setup: %D %D %D\n", i, x, ioxp_status[x]);
            // まだ初期化されていないIOエキスパンダなら初期化
            if (ioxp_status[x] < 0) {
                ioxp_obj[x] = new Adafruit_MCP23X17();
                ioxp_status[x] = 0;
            }
            if (ioxp_status[x] < 1) {
                if (ioxp_obj[x]->begin_I2C(opt->ioxp[i].addr, &Wire)) {
                    M5.Lcd.printf("begin_I2C option  %D  %D OK\n", i, opt->ioxp[i].addr);
                    ioxp_hash[x] = 1;
                    ioxp_status[x] = 1;
                } else {
                    M5.Lcd.printf("begin_I2C option  %D  %D NG\n", i, opt->ioxp[i].addr);
                    delay(1000);
                    continue;
                }
            }
            // ピンのタイプデータをINPUT_PULLUPで初期化
            for (j=0; j<16; j++) {
                set_type[j] = INPUT_PULLUP;
            }
            // rowピンだけOUTPUTにする
            for (j=0; j<opt->ioxp[i].row_len; j++) {
                set_type[ opt->ioxp[i].row[j] ] = OUTPUT;
            }
            // ピン初期化
            for (j=0; j<16; j++) {
                ioxp_obj[x]->pinMode(j, set_type[j]);
            }
        }
        // キーの番号をmapデータに入れる
        // あとでキー設定の番号入れ替えをここでやる
        k = opt->map_start;
        for (i=0; i<opt->map_len; i++) {
            // 設定内容中の番号を付け替える
            for (j=0; j<setting_length; j++) {
                if (setting_press[j].key_num == p) {
                    // 元から同じ番号の設定が入っていたら消す
                    setting_press[j].key_num = -1;
                } else if (setting_press[j].key_num == k) {
                    // Serial.printf("map: %D %D %D [ %D -> %D ]\n", i, j, k, setting_press[j].key_num, p);
                    setting_press[j].key_num = p;
                }
            }
            p++;
            k++;
        }
    }
    return p;
}

// キーの入力ピンの初期化
void AzCommon::pin_setup() {
    
  int i, j, m, x;

  key_input_length = 0;

  // direct(スイッチ直接続)で定義されているピンを全てinputにする
  for (i=0; i<direct_len; i++) {
      pinMode(direct_list[i], INPUT_PULLUP);
  }
  key_input_length += direct_len;


  // I2C初期化
  if (ioxp_sda >= 0 && ioxp_scl >= 0) {

    if (Wire.begin(ioxp_sda, ioxp_scl)) {
        Wire.setClock(ioxp_hz);
        M5.Lcd.printf("Wire1 begin ok\n");
    } else {
        M5.Lcd.printf("Wire1 begin ng\n");
        delay(1000);
    }

    // IOエキスパンダ初期化
    M5.Lcd.printf("Adafruit_MCP23X17 %D\n", ioxp_len);
    for (i=0; i<ioxp_len; i++) {
        x = ioxp_list[i] - 32;
        if (ioxp_status[x] < 0) {
            ioxp_obj[x] = new Adafruit_MCP23X17();
            ioxp_status[x] = 0;
        }
        if (ioxp_status[x] < 1) {
            if (ioxp_obj[x]->begin_I2C(ioxp_list[i], &Wire)) {
                M5.Lcd.printf("begin_I2C %D  %D OK\n", i, ioxp_list[i]);
                ioxp_status[x] = 1;
                ioxp_hash[x] = 1;
            } else {
                M5.Lcd.printf("begin_I2C %D  %D NG\n", i, ioxp_list[i]);
                delay(1000);
                continue;
            }
        }
        delay(10);
        for (j = 0; j < 16; j++) {
            ioxp_obj[x]->pinMode(j, INPUT_PULLUP);
        }
        delay(50);
    }
    key_input_length += (ioxp_len * 16);
    

    // I2C接続のオプション初期化
    for (i=0; i<i2copt_len; i++) {
        key_input_length = i2c_setup(key_input_length, &i2copt[i]);
    }
    
  }

  this->key_count_total = 0; // キーボードを起動してから押されたキーの数総数
}


// キー入力ピン初期化のキーボード別の処理
void AzCommon::pin_setup_sub_process() {
    if (option_type_int == 1) {
        // 踏みキー
        pinMode(27, INPUT_PULLUP);
    }
}

// 指定したキーの入力設定オブジェクトを取得する
setting_key_press AzCommon::get_key_setting(int layer_id, int key_num) {
    int i;
    for (i=0; i<setting_length; i++) {
        if (setting_press[i].layer == layer_id && setting_press[i].key_num == key_num) return setting_press[i];
    }
    setting_key_press r;
    r.layer = -1;
    r.key_num = -1;
    r.action_type = -1;
    return r;
}

// 指定したキーの入力設定オブジェクトを取得する
setting_key_press AzCommon::get_soft_key_setting(int layer_id, int key_num) {
    int i;
    for (i=0; i<soft_setting_length; i++) {
        if (soft_setting_press[i].layer == layer_id && soft_setting_press[i].key_num == key_num) return soft_setting_press[i];
    }
    setting_key_press r;
    r.layer = -1;
    r.key_num = -1;
    r.action_type = -1;
    return r;
}

//データをEEPROMから読み込む。保存データが無い場合デフォルトにする。
void AzCommon::load_data() {
    // デフォルト値セット
    // 起動モード
    eep_data.boot_mode = 0;
    // データチェック文字列
    strcpy(eep_data.check, EEP_DATA_VERSION);
    // ユニークID
    getRandomNumbers(10, eep_data.uid);
    // キーボードの種類
    strcpy(eep_data.keyboard_type, "");
    File fp;
    if (!SPIFFS.exists(AZ_SYSTEM_FILE_PATH)) {
        // ファイルが無い場合デフォルト値でファイルを作成
        save_data();
        return;
    }
    // ファイルがあればデータ読み込み
    fp = SPIFFS.open(AZ_SYSTEM_FILE_PATH, "r");
    if(!fp){
        ESP_LOGD(LOG_TAG, "file open error\n");
        return;
    }
    if (fp.available()) {
        fp.read((uint8_t *)&eep_data, sizeof(mrom_data_set));
    }
    fp.close();
    // データのバージョンが変わっていたらファイルを消して再起動
    if (strcmp(eep_data.check, EEP_DATA_VERSION) != 0) {
        SPIFFS.remove(AZ_SYSTEM_FILE_PATH);
        SPIFFS.remove(SETTING_JSON_PATH);
        delay(300);
        ESP.restart(); // ESP32再起動
    }
}


// データをEEPROMに書き込む
void AzCommon::save_data() {
    //EEPROMに設定を保存する。
    File fp;
    strcpy(eep_data.check, EEP_DATA_VERSION);
    // ファイルに書き込み
    fp = SPIFFS.open(AZ_SYSTEM_FILE_PATH, "w");
    fp.write((uint8_t *)&eep_data, sizeof(mrom_data_set));
    fp.close();
    delay(200);
    ESP_LOGD(LOG_TAG, "save complete\r\n");
}

// 起動回数読み込み
void AzCommon::load_boot_count() {
    File fp;
    boot_count = 0;
    // ファイルが無い場合はデフォルトファイル作成
    if (SPIFFS.exists(BOOT_COUNT_PATH)) {
        // boot_count から読み込み
        fp = SPIFFS.open(BOOT_COUNT_PATH, "r");
        if(!fp){
            ESP_LOGD(LOG_TAG, "boot count file open error\n");
            return;
        }
        if (fp.available()) {
            fp.read((uint8_t *)&boot_count, 4);
        }
        fp.close();
    }
    // カウントアップ
    boot_count++;
    // ファイルに書き込み
    fp = SPIFFS.open(BOOT_COUNT_PATH, "w");
    fp.write((uint8_t *)&boot_count, 4);
    fp.close();
}


// ファイルから設定値を読み込み
void AzCommon::load_file_data(char *file_path, uint8_t *load_point, uint16_t load_size) {
    File fp;
    // ファイルが無い場合は何もしない
    if (!SPIFFS.exists(file_path)) return;
    // ファイル読み込み
    fp = SPIFFS.open(file_path, "r");
    if(!fp){
        ESP_LOGD(LOG_TAG, "file open error\n");
        return;
    }
    if (fp.available()) {
        fp.read(load_point, load_size);
    }
    fp.close();
}


// ファイルに設定値を書込み
void AzCommon::save_file_data(char *file_path, uint8_t *save_point, uint16_t save_size) {
    // ファイルに書き込み
    File fp;
    fp = SPIFFS.open(file_path, "w");
    fp.write(save_point, save_size);
    fp.close();
}


// 起動モードを変更してEEPROMに保存
void AzCommon::set_boot_mode(int set_mode) {
    eep_data.boot_mode = set_mode;
    save_data();
}

// モードを切り替えて再起動
void AzCommon::change_mode(int set_mode) {
    set_boot_mode(set_mode);
    ESP.restart(); // ESP32再起動
}

// I2C機器のキー状態を取得
int AzCommon::i2c_read(int p, i2c_option *opt, char *read_data) {
    int e, i, j, k, m, n, r, x;
    unsigned long start_time;
    unsigned long end_time;
    uint16_t rowput_mask;
    int rowput_len;
    uint16_t read_raw[32];
    Adafruit_MCP23X17 *ioxp;
    r = 0;
    e = 0;
    if (opt->opt_type == 1) {
        // IOエキスパンダ
        for (i=0; i<opt->ioxp_len; i++) {
            x = opt->ioxp[i].addr - 32; // アドレス
            // まだ初期化されていないIOエキスパンダなら無視
            if (ioxp_status[x] < 1) continue;
            // row と col があればマトリックス入力
            if (opt->ioxp[i].row_len > 0 && opt->ioxp[i].col_len > 0) {
                // まずrowでoutputするデータとマスクを用意
                rowput_len = opt->ioxp[i].row_len;
                rowput_mask = opt->ioxp[i].row_mask;
                ioxp = ioxp_obj[x];
                // rowのoutput分ループ
                for (j=0; j<rowput_len; j++) {
                    if (rowput_mask & 0xff00) { // ポートB
                        ioxp->writeGPIO((opt->ioxp[i].row_output[j] >> 8) & 0xff, 1); // ポートBに出力
                    }
                    if (rowput_mask & 0xff) { // ポートA
                        ioxp->writeGPIO(opt->ioxp[i].row_output[j] & 0xff, 0); // ポートAに出力
                    }
                    read_raw[e] = ioxp->readGPIOAB() | rowput_mask; // ポートA,B両方のデータを取得
                    e++;
                }
            } else {
                // col と row が無い場合はダイレクトのみ
                read_raw[e] = ioxp_obj[x]->readGPIOAB(); // ポートA,B両方のデータを取得
                e++;
            }
        }
        // マップデータ分入力を取得
        for (j=0; j<opt->map_len; j++) {
            n = opt->map[j];
            k = n / 16;
            m = n % 16;
            if (k >= e) k = e - 1; // マトリックスで取得した数より多い場合はdirectの指定なので一番最後に取得した情報で判定
            read_data[p] = (read_raw[k] & (0x01 << m))? 0: 1;
            p++;
            r++;
        }
    }
    return r;
}

// 現在のキーの入力状態を取得
void AzCommon::key_read(void) {
    unsigned long start_time;
    unsigned long end_time;
    int m[ioxp_len];
    int i, j, n, c, p, x;
    p = 0;
    // ダイレクト入力の取得
    for (i=0; i<direct_len; i++) {
        input_key[p] = !digitalRead(direct_list[i]);
        p++;
    }
    // IOエキスパンダ
    for (i=0; i<ioxp_len; i++) {
        x = ioxp_list[i] - 32;
        if (ioxp_status[x] == 1) {
            m[i] = ioxp_obj[x]->readGPIOAB();
            if (m[i] == 0) {
                // 全ピン読み込み無しだとIOエキスパンダの接続が切れたかリセットした可能性あり
                ioxp_status[x] = 2; // 一旦接続切れたステータスにする
                m[i] = 0xffff; // 全ボタン離した状態
            }
        } else if (ioxp_status[x] > 256) {
            // 接続が切れた状態の場合100周期に1回ピンの初期化を送信してステータスリセット
            for (j = 0; j < 16; j++) {
                ioxp_obj[x]->pinMode(j, INPUT_PULLUP);
            }
            ioxp_status[x] = 0; // ステータスを接続状態に戻す
            m[i] = 0xffff; // 全ボタン離した状態
        } else if (ioxp_status[x] > 1) {
            // IOエキスパンダ接続していない間カウントアップ
            ioxp_status[x]++;
            m[i] = 0xffff; // 全ボタン離した状態
        } else {
            m[i] = 0xFFFF; // 全てのボタン離した状態
        }
    }
    for (i=0; i<16; i++) {
        c = 1 << i;
        n = 0;
        for (j=0; j<ioxp_len; j++) {
            input_key[i + n + p] = (!(m[j] & c));
            n += 16;
        }
    }
    p += ioxp_len * 16;
    // I2Cオプション
    start_time = micros();
    for (i=0; i<i2copt_len; i++) {
        p += i2c_read(p, &i2copt[i], input_key);
    }
    end_time = micros();
    /*
    Serial.printf("%D [", key_input_length);
    for (i=0; i<key_input_length; i++) {
        Serial.printf(" %D", input_key[i]);
    }
    Serial.printf("] %D\n", (end_time - start_time));
    // delay(1000);
    /*
    M5.Lcd.fillScreen(BLACK);
    M5.Lcd.setCursor(0, 0, 2);
    for (i=0; i<key_input_length; i++) {
        // if ((i % 6) == 5) M5.Lcd.printf("\n");
        M5.Lcd.printf(" %D", i, input_key[i]);
    }
    */
    
}

// キーボード別の後処理
void AzCommon::key_read_sub_process(void) {
    if (option_type_int == 1) {
        // 踏みキー
        if (foot_inversion && digitalRead(27) == 0) {
            // ジャックが刺されている間はON/OFF を反転
            input_key[8] = ! input_key[8];
            input_key[9] = ! input_key[9];
        }
    }
}

// 今回の入力状態を保持
void AzCommon::key_old_copy(void) {
    int i;
    for (i=0; i<key_input_length; i++) {
        input_key_last[i] = input_key[i];
    }
}

// 液晶ユニットが付いているか
bool AzCommon::on_tft_unit(void) {
    if (option_type_int == 3 || option_type_int == 4) {
        return true;
    }
    return false;
}

// 全てのファイルを削除
void AzCommon::delete_all(void) {
    File dirp = SPIFFS.open("/");
    File filep = dirp.openNextFile();
    while(filep){
        SPIFFS.remove(filep.name());
        filep = dirp.openNextFile();
    }
}

// 指定した文字から始まるファイルすべて削除
void AzCommon::delete_indexof_all(String check_str) {
    File dirp = SPIFFS.open("/");
    File filep = dirp.openNextFile();
    String file_path;
    while(filep){
        file_path = String(filep.name());
        if (file_path.indexOf("/" + check_str) == 0) {
            SPIFFS.remove(file_path);
        }
        filep = dirp.openNextFile();
    }
}

// ファイル領域合計サイズを取得
int AzCommon::spiffs_total(void) {
    return SPIFFS.totalBytes();
}

// 使用しているファイル領域サイズを取得
int AzCommon::spiffs_used(void) {
    return SPIFFS.usedBytes();
}
