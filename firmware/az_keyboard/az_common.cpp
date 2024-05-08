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

// i2c OLEDクラス
Oled *oled_cls;

// メインOLEDの設定
short oled_main_width = -1;
short oled_main_height = -1;
short oled_main_addr = -1;

// I2Cライブラリ用クラス
Wirelib wirelib_cls = Wirelib();

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

// キースキャンループの待ち時間
short loop_delay;

// ディスプレイの向き
uint8_t disp_rotation;

// キーボードタイプの番号
int keyboard_type_int;

// キーボードの名前
char keyboard_name_str[32];

// キーボードの言語(日本語=0/ US=1 / 日本語(US記号) = 2)
uint8_t keyboard_language;

// トラックボールの方向
uint8_t trackball_direction;

// トラックボールのカーソル移動速度
uint8_t trackball_speed;

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

// ホールセンサーの範囲
short hall_range_min;
short hall_range_max;

// holdの設定
uint8_t hold_type;
uint8_t hold_time;

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
short col_len;
short row_len;
short direct_len;
short touch_len;
short hall_len;
short *col_list;
short *row_list;
short *direct_list;
short *touch_list;
short *hall_list;
short *hall_offset;

short *ioxp_list;
short ioxp_sda;
short ioxp_scl;
int ioxp_hz;
short ioxp_status[8];
int ioxp_hash[8];

short ioxp_len;

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

// サウンド定期処理
static void background_loop(void* arg) {
  while (true) {
    unsigned long start_time = millis();
    // サウンドを制御する定期処理
    sound_cls.loop_exec();
    unsigned long work_time = millis() - start_time;
    if (work_time < 3) { vTaskDelay(3 - work_time); }
  }
}

// 画面、Neopixel定期処理
static void background_disp_loop(void* arg) {
  while (true) {
    unsigned long start_time = millis();
    // 画面定期処理
    if (!menu_mode_flag && aztool_mode_flag == 0 && disp_enable) lv_task_handler();
    // Neopixel定期処理
    rgb_led_cls.rgb_led_loop_exec();
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
        press_key_list[i].press_type = -1;
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
    if (setting_obj.containsKey("keyboard_name")) {
        keynamestr = setting_obj["keyboard_name"].as<String>();
        keynamestr.toCharArray(keyboard_name_str, 31);
    } else {
        // 設定が無い場合はデフォルト
        sprintf(keyboard_name_str, "az_keyboard");
    }

    // キースキャンループの待ち時間
    if (setting_obj.containsKey("loop_delay")) {
        loop_delay = setting_obj["loop_delay"].as<signed int>();
    } else {
        loop_delay = LOOP_DELAY_DEFAULT;
    }

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
    // hold 設定読み込み
    hold_type = 0;
    hold_time = 30;
    if (setting_obj.containsKey("hold")) {
        if (setting_obj["hold"].containsKey("type")) {
            hold_type = setting_obj["hold"]["type"].as<signed int>();
        }
        if (setting_obj["hold"].containsKey("time")) {
            hold_time = setting_obj["hold"]["time"].as<signed int>();
        }
    }
    // ホールセンサーのアナログ値読み取り範囲
    if (setting_obj.containsKey("hall_range_min")) {
        hall_range_min = setting_obj["hall_range_min"].as<signed int>();
    } else {
        hall_range_min = HALL_RANGE_MIN_DEFAULT;
    }
    if (setting_obj.containsKey("hall_range_max")) {
        hall_range_max = setting_obj["hall_range_max"].as<signed int>();
    } else {
        hall_range_max = HALL_RANGE_MAX_DEFAULT;
    }
    // オープニングムービー再生取得
    op_movie_flag = false;
    if (setting_obj["option_set"]["op_movie"].as<signed int>() > 0) op_movie_flag = true;
    // 入力ピン情報取得
    int i, j, k, m, n, o, p, r;
    col_len = setting_obj["keyboard_pin"]["col"].size();
    row_len = setting_obj["keyboard_pin"]["row"].size();
    direct_len = setting_obj["keyboard_pin"]["direct"].size();
    touch_len = setting_obj["keyboard_pin"]["touch"].size();
    ioxp_len = setting_obj["keyboard_pin"]["ioxp"].size();
    col_list = new short[col_len];
    for (i=0; i<col_len; i++) {
        col_list[i] = setting_obj["keyboard_pin"]["col"][i].as<signed int>();
    }
    row_list = new short[row_len];
    for (i=0; i<row_len; i++) {
        row_list[i] = setting_obj["keyboard_pin"]["row"][i].as<signed int>();
    }
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

    // 磁気スイッチ入力ピン情報取得
    if (setting_obj["keyboard_pin"].containsKey("hall")) {
        hall_len = setting_obj["keyboard_pin"]["hall"].size();
        hall_list = new short[hall_len]; // ホールセンサーにつながっているピン
        input_key_analog = new uint8_t[hall_len]; // 現在のアナログ値
        analog_stroke_most = new char[hall_len]; // キーを最も押し込んだ時のアナログ値
        for (i=0; i<hall_len; i++) {
            hall_list[i] = setting_obj["keyboard_pin"]["hall"][i].as<signed int>();
            input_key_analog[i] = 0;
            analog_stroke_most[i] = 0;
        }
    } else {
        hall_len = 0;
    }

    // IOエキスパンダピン
    if (setting_obj.containsKey("i2c_set") && setting_obj["i2c_set"].size() == 3) {
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
    i2c_map i2cmap_obj;
    i2c_ioxp i2cioxp_obj;
    i2c_rotary i2crotary_obj;
    i2c_pim447 i2cpim447_obj;
    i2c_azxp i2cazxp_obj;
    int opt_type;
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
                opt_type = setting_obj["i2c_option"][i]["type"].as<signed int>();
            } else {
                opt_type = 0;
            }
            i2copt[j].opt_type = opt_type & 0xff;
            // マッピング情報の読み込み
            if (opt_type == 1 || opt_type == 2 || opt_type == 3 || opt_type == 4 || opt_type == 5) { // 1 = IOエキスパンダ（MCP23017）/ 2 = Tiny202 ロータリーエンコーダ
                // キーマッピング設定
                if (setting_obj["i2c_option"][i].containsKey("map") &&
                        setting_obj["i2c_option"][i]["map"].size() ) {
                    i2cmap_obj.map_len = setting_obj["i2c_option"][i]["map"].size();
                    i2cmap_obj.map = new short[i2cmap_obj.map_len];
                    for (p=0; p<i2cmap_obj.map_len; p++) {
                        i2cmap_obj.map[p] = setting_obj["i2c_option"][i]["map"][p].as<signed int>();
                    }
                } else {
                    i2cmap_obj.map_len = 0;
                }
                // キー設定の開始番号
                if (setting_obj["i2c_option"][i].containsKey("map_start")) {
                    i2cmap_obj.map_start = setting_obj["i2c_option"][i]["map_start"].as<signed int>();
                } else {
                    i2cmap_obj.map_start = 0;
                }
                i2copt[j].i2cmap = new i2c_map;
                memcpy(i2copt[j].i2cmap, &i2cmap_obj, sizeof(i2c_map));
            }
            // オプション別のデータ読み込み
            if (opt_type == 1) { // 1 = IOエキスパンダ（MCP23017）
                // 使用するIOエキスパンダの情報
                if (setting_obj["i2c_option"][i].containsKey("ioxp") && setting_obj["i2c_option"][i]["ioxp"].size()) {
                    i2cioxp_obj.ioxp_len = setting_obj["i2c_option"][i]["ioxp"].size();
                    i2cioxp_obj.ioxp = new ioxp_option[i2cioxp_obj.ioxp_len];
                    for (n=0; n<i2cioxp_obj.ioxp_len; n++) {
                        // IOエキスパンダのアドレス
                        i2cioxp_obj.ioxp[n].addr = setting_obj["i2c_option"][i]["ioxp"][n]["addr"];
                        // row に設定しているピン
                        if (setting_obj["i2c_option"][i]["ioxp"][n].containsKey("row") &&
                                setting_obj["i2c_option"][i]["ioxp"][n]["row"].size() ) {
                            // row に設定されている配列を取得(0～7 以外を無視する)
                            i2cioxp_obj.ioxp[n].row_len = setting_obj["i2c_option"][i]["ioxp"][n]["row"].size();
                            i2cioxp_obj.ioxp[n].row = new uint8_t[i2cioxp_obj.ioxp[n].row_len]; // row のピン番号
                            i2cioxp_obj.ioxp[n].row_output = new uint16_t[i2cioxp_obj.ioxp[n].row_len]; // マトリックスでrow write するデータ
                            i2cioxp_obj.ioxp[n].row_mask = 0x00; // row write する時用のマスク
                            o = 0;
                            for (p=0; p<i2cioxp_obj.ioxp[n].row_len; p++) {
                                r = setting_obj["i2c_option"][i]["ioxp"][n]["row"][p].as<signed int>();
                                // if (r < 0 || r >= 8) continue; // ポートA以外の場合は取得しない
                                i2cioxp_obj.ioxp[n].row[o] = r;
                                o++;
                                i2cioxp_obj.ioxp[n].row_mask |= 0x01 << r;
                            }
                            i2cioxp_obj.ioxp[n].row_len = o;
                            // マトリックス出力する時用のデータ作成
                            for (p=0; p<o; p++) {
                                i2cioxp_obj.ioxp[n].row_output[p] = i2cioxp_obj.ioxp[n].row_mask & ~(0x01 << i2cioxp_obj.ioxp[n].row[p]);
                            }
                        } else {
                            i2cioxp_obj.ioxp[n].row_len = 0;
                        }
                        // col に設定しているピン
                        if (setting_obj["i2c_option"][i]["ioxp"][n].containsKey("col") &&
                                setting_obj["i2c_option"][i]["ioxp"][n]["col"].size() ) {
                            i2cioxp_obj.ioxp[n].col_len = setting_obj["i2c_option"][i]["ioxp"][n]["col"].size();
                            i2cioxp_obj.ioxp[n].col = new uint8_t[i2cioxp_obj.ioxp[n].col_len];
                            for (p=0; p<i2cioxp_obj.ioxp[n].col_len; p++) {
                                i2cioxp_obj.ioxp[n].col[p] = setting_obj["i2c_option"][i]["ioxp"][n]["col"][p].as<signed int>();
                            }
                        } else {
                            i2cioxp_obj.ioxp[n].col_len = 0;
                        }
                        // direct に設定しているピン
                        if (setting_obj["i2c_option"][i]["ioxp"][n].containsKey("direct") &&
                                setting_obj["i2c_option"][i]["ioxp"][n]["direct"].size() ) {
                            i2cioxp_obj.ioxp[n].direct_len = setting_obj["i2c_option"][i]["ioxp"][n]["direct"].size();
                            i2cioxp_obj.ioxp[n].direct = new uint8_t[i2cioxp_obj.ioxp[n].direct_len];
                            for (p=0; p<i2cioxp_obj.ioxp[n].direct_len; p++) {
                                i2cioxp_obj.ioxp[n].direct[p] = setting_obj["i2c_option"][i]["ioxp"][n]["direct"][p].as<signed int>();
                            }
                        } else {
                            i2cioxp_obj.ioxp[n].direct_len = 0;
                        }
                    }
                } else {
                    i2cioxp_obj.ioxp_len = 0;
                }
                i2copt[j].data = (uint8_t *)new i2c_ioxp;
                memcpy(i2copt[j].data, &i2cioxp_obj, sizeof(i2c_ioxp));
                j++;

            } else if (opt_type == 2) { // 2 = Tiny202 ロータリーエンコーダ
                
                // 使用するIOエキスパンダの情報
                if (setting_obj["i2c_option"][i].containsKey("rotary") && setting_obj["i2c_option"][i]["rotary"].size()) {
                    i2crotary_obj.rotary_len = setting_obj["i2c_option"][i]["rotary"].size();
                    i2crotary_obj.rotary = new uint8_t[i2crotary_obj.rotary_len];
                    for (n=0; n<i2crotary_obj.rotary_len; n++) {
                        i2crotary_obj.rotary[n] = setting_obj["i2c_option"][i]["rotary"][n].as<signed int>();
                    }
                } else {
                    i2crotary_obj.rotary_len = 0;
                }
                i2copt[j].data = (uint8_t *)new i2c_rotary;
                memcpy(i2copt[j].data, &i2crotary_obj, sizeof(i2c_rotary));
                j++;

            } else if (opt_type == 3 || opt_type == 4) { // 3 = 1U トラックボール PIM447  // 4 = フリック入力
                // アドレス
                if (setting_obj["i2c_option"][i].containsKey("addr")) {
                    i2cpim447_obj.addr = setting_obj["i2c_option"][i]["addr"].as<signed int>();
                } else {
                    i2cpim447_obj.addr = 0;
                }
                // マウスの移動速度
                if (setting_obj["i2c_option"][i].containsKey("speed")) {
                    i2cpim447_obj.speed = setting_obj["i2c_option"][i]["speed"].as<signed int>();
                } else {
                    i2cpim447_obj.speed = 0;
                }
                // トラックボールの方向
                if (setting_obj["i2c_option"][i].containsKey("rotate")) {
                    i2cpim447_obj.rotate = setting_obj["i2c_option"][i]["rotate"].as<signed int>();
                } else {
                    i2cpim447_obj.rotate = 0;
                }
                i2copt[j].data = (uint8_t *)new i2c_pim447;
                memcpy(i2copt[j].data, &i2cpim447_obj, sizeof(i2c_pim447));
                j++;

            } else if (opt_type == 5) { // 5 = AZ-Expander
                m = setting_obj["i2c_option"][i]["setting"].size();
                if (m > 18) m = 18;
                for (n=0; n<m; n++) {
                    i2cazxp_obj.setting[n] = setting_obj["i2c_option"][i]["setting"][n].as<signed int>();
                }
                i2copt[j].data = (uint8_t *)new i2c_azxp;
                memcpy(i2copt[j].data, &i2cazxp_obj, sizeof(i2c_azxp));
                j++;

            } else if (opt_type == 6) { // 6 = OLED(メイン)
                oled_main_width = -1;
                oled_main_height = -1;
                oled_main_addr = -1;
                // 設定内容を取得
                if (setting_obj["i2c_option"][i].containsKey("width")) {
                    oled_main_width = setting_obj["i2c_option"][i]["width"].as<signed int>();
                }
                if (setting_obj["i2c_option"][i].containsKey("height")) {
                    oled_main_height = setting_obj["i2c_option"][i]["height"].as<signed int>();
                }
                if (setting_obj["i2c_option"][i].containsKey("addr")) {
                    oled_main_addr = setting_obj["i2c_option"][i]["addr"].as<signed int>();
                }
                j++;

            }

        }


    } else {
        i2copt_len = 0;
    }
    
    // key_input_length = 16 * ioxp_len;
    // キーの設定を取得
    // まずは設定の数を取得
    setting_length = 0;
    soft_setting_length = 0;
    this->get_keymap(setting_obj, "layers"); // 通常キー
    this->get_keymap(setting_obj, "soft_layers"); // ソフトウェアキー


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
    if (setting_obj.containsKey("keyboard_language")) {
        keyboard_language = setting_obj["keyboard_language"].as<signed int>();
    } else {
        keyboard_language = 0;
    }

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
        } else if (setting_press[i].action_type == 9) { // holdボタン
        } else if (setting_press[i].action_type == 10) { // アナログマウス移動
            delete setting_press[i].data;
        } else if (setting_press[i].action_type == 11) { // Nubkey位置調節
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
    int i, l, m;
    char lkey[16];
    char kkey[16];
    uint16_t lnum, knum, slen, lmax;
    String text_str;
    JsonObject::iterator it_l;
    JsonObject::iterator it_k;
    JsonObject layers, keys, press_json;
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
    // Serial.printf("setting total %D\n", slen);
    // 設定数分メモリ確保
    // Serial.printf("setting_key_press:start: %D %D\n", heap_caps_get_free_size(MALLOC_CAP_32BIT), heap_caps_get_free_size(MALLOC_CAP_8BIT) );
    setpt = new setting_key_press[slen];
    // Serial.printf("setting_key_press:end: %D %D\n", heap_caps_get_free_size(MALLOC_CAP_32BIT), heap_caps_get_free_size(MALLOC_CAP_8BIT) );
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
            // Serial.printf("get_keymap: %S %S [ %D %D ]\n", lkey, kkey, lnum, knum);
            // Serial.printf("mem: %D %D\n", heap_caps_get_free_size(MALLOC_CAP_32BIT), heap_caps_get_free_size(MALLOC_CAP_8BIT) );
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
            press_json = setting_obj[key_name][lkey]["keys"][kkey]["press"].as<JsonObject>();
            this->get_keymap_one(press_json, &setpt[i], lnum, knum);
            setpt[i].sub_press_flag = false;
            if (setting_obj[key_name][lkey]["keys"][kkey].containsKey("sub")) {
                setpt[i].sub_press_flag = true;
                setpt[i].sub_press = new setting_key_press;
                press_json = setting_obj[key_name][lkey]["keys"][kkey]["sub"].as<JsonObject>();
                this->get_keymap_one(press_json, (setting_key_press *)setpt[i].sub_press, lnum, knum);
            }
            i++;
        }
    }
    if (strcmp(key_name, "layers") == 0) { // 通常のキー設定
        setting_press = setpt;
        setting_length = slen;
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

}

// JSONデータからキーマップの情報を読み込む(1キー分)
void AzCommon::get_keymap_one(JsonObject json_obj, setting_key_press *press_obj, uint16_t lnum, uint16_t knum) {
    int j, k, m, at, s;
    String text_str;
    setting_normal_input normal_input;
    setting_layer_move layer_move_input;
    setting_mouse_move mouse_move_input;

    press_obj->layer = lnum; // 対象レイヤー
    press_obj->key_num = knum; // 対象キーID
    // アクチュエーションポイント
    if (json_obj.containsKey("act")) {
        press_obj->actuation_type = json_obj["act"].as<signed int>();
    } else {
        press_obj->actuation_type = ACTUATION_TYPE_DEFAULT;
    }
    // アクチュエーションポイント
    if (json_obj.containsKey("acp")) {
        press_obj->actuation_point = json_obj["acp"].as<signed int>();
    } else {
        press_obj->actuation_point = ACTUATION_POINT_DEFAULT;
    }
    // ラピットトリガー
    if (json_obj.containsKey("rap")) {
        press_obj->rapid_trigger = json_obj["rap"].as<signed int>();
    } else {
        press_obj->rapid_trigger = RAPID_TRIGGER_DEFAULT;
    }
    // ボタンの動作
    press_obj->action_type = json_obj["action_type"].as<signed int>();
    if (press_obj->action_type == 1) {
        // 通常入力
        normal_input.key_length = json_obj["key"].size();
        normal_input.key = new uint16_t[normal_input.key_length];
        for (j=0; j<normal_input.key_length; j++) {
                normal_input.key[j] = json_obj["key"][j].as<signed int>();
        }
        // 連打設定
        if (json_obj.containsKey("repeat_interval")) {
            normal_input.repeat_interval = json_obj["repeat_interval"].as<signed int>();
        } else {
            normal_input.repeat_interval = 51;
        }
        // ホールド設定
        if (json_obj.containsKey("hold")) {
            normal_input.hold = json_obj["hold"].as<signed int>();
        } else {
            normal_input.hold = 0;
        }
        press_obj->data = (char *)new setting_normal_input;
        memcpy(press_obj->data, &normal_input, sizeof(setting_normal_input));
    } else if (press_obj->action_type == 2) {
        // テキスト入力
        text_str = json_obj["text"].as<String>();
        m = text_str.length() + 1;
        press_obj->data = new char[m];
        text_str.toCharArray(press_obj->data, m);
    } else if (press_obj->action_type == 3) {
        // レイヤー切り替え
        layer_move_input.layer_id = json_obj["layer"].as<signed int>();
        layer_move_input.layer_type = json_obj["layer_type"].as<signed int>();
        if (layer_move_input.layer_type == 0) layer_move_input.layer_type = 0x51; // 切り替え方法の指定が無かった場合はMO(押している間切り替わる)
        press_obj->data = (char *)new setting_layer_move;
        memcpy(press_obj->data, &layer_move_input, sizeof(setting_layer_move));
    } else if (press_obj->action_type == 4) {
        // WEBフック
        text_str = "";
        serializeJson(json_obj["webhook"], text_str);
        m = text_str.length() + 1;
        press_obj->data = new char[m];
        text_str.toCharArray(press_obj->data, m);
        
    } else if (press_obj->action_type == 5 || press_obj->action_type == 10) {
        // 5.マウス移動 10.アナログマウス移動
        mouse_move_input.x = json_obj["move"]["x"].as<signed int>();
        mouse_move_input.y = json_obj["move"]["y"].as<signed int>();
        mouse_move_input.wheel = json_obj["move"]["wheel"].as<signed int>();
        mouse_move_input.hWheel = json_obj["move"]["hWheel"].as<signed int>();
        mouse_move_input.speed = json_obj["move"]["speed"].as<signed int>();
        press_obj->data = (char *)new setting_mouse_move;
        memcpy(press_obj->data, &mouse_move_input, sizeof(setting_mouse_move));

    } else if (press_obj->action_type == 6) {
        // 暗記ボタン

    } else if (press_obj->action_type == 7) {
        // LED設定ボタン
        press_obj->data = new char;
        *press_obj->data = json_obj["led_setting_type"].as<signed int>();
        
    } else if (press_obj->action_type == 8) {
        // 打鍵設定ボタン
        press_obj->data = new char;
        *press_obj->data = json_obj["dakagi_settype"].as<signed int>();

    }
}

// キーボードタイプの番号を取得する
void AzCommon::get_keyboard_type_int(String t) {
    if (t.equals("custom")) { keyboard_type_int = 1; } // カスタム
    else if (t.equals("az_m5ortho")) { keyboard_type_int = 2; } // AZ-M5ortho
	else if (t.equals("az_emugotch")) { keyboard_type_int = 3; } // えむごっち
    else { keyboard_type_int = 0; } // 不明なキーボード
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
    i2c_map i2cmap_obj;
    i2c_ioxp i2cioxp_obj;
    i2c_pim447 i2cpim447_obj;
    i2c_azxp i2cazxp_obj;
    // Serial.printf("i2c_setup: opt_type %D\n", opt->opt_type);
    if (opt->opt_type == 1) {
        // IOエキスパンダ
        memcpy(&i2cioxp_obj, opt->data, sizeof(i2c_ioxp));
        for (i=0; i<i2cioxp_obj.ioxp_len; i++) {
            x = i2cioxp_obj.ioxp[i].addr - 32; // アドレス
            // Serial.printf("i2c_setup: %D %D %D\n", i, x, ioxp_status[x]);
            // まだ初期化されていないIOエキスパンダなら初期化
            if (ioxp_status[x] < 0) {
                ioxp_obj[x] = new Adafruit_MCP23X17();
                ioxp_status[x] = 0;
            }
            if (ioxp_status[x] < 1) {
            // Serial.printf("i2c_setup: begin %D\n", i2cioxp_obj.ioxp[i].addr);
                if (ioxp_obj[x]->begin_I2C(i2cioxp_obj.ioxp[i].addr, &Wire)) {
                    ioxp_hash[x] = 1;
                    ioxp_status[x] = 1;
                } else {
                    // 初期化失敗
                    continue;
                }
            }
            // ピンのタイプデータをINPUT_PULLUPで初期化
            for (j=0; j<16; j++) {
                set_type[j] = INPUT_PULLUP;
            }
            // rowピンだけOUTPUTにする
            for (j=0; j<i2cioxp_obj.ioxp[i].row_len; j++) {
                set_type[ i2cioxp_obj.ioxp[i].row[j] ] = OUTPUT;
            }
            // ピン初期化
            for (j=0; j<16; j++) {
            // Serial.printf("i2c_setup: pinMode %D %D\n", j, set_type[j]);
                ioxp_obj[x]->pinMode(j, set_type[j]);
            }
        }
    } else if (opt->opt_type == 2) {
        // ATTiny202 ロータリーエンコーダー
        // 初期化特になし

    } else if (opt->opt_type == 3) {
        // 1U トラックボール PIM447
        memcpy(&i2cpim447_obj, opt->data, sizeof(i2c_pim447));
        wirelib_cls.set_az1uball_read_type(i2cpim447_obj.addr, 1); // 早く動かすとマウスの移動速度も速くする

    } else if (opt->opt_type == 4) {
        // 1U トラックボール PIM447 スクロール
        memcpy(&i2cpim447_obj, opt->data, sizeof(i2c_pim447));
        wirelib_cls.set_az1uball_read_type(i2cpim447_obj.addr, 0); // 早く動かしてもマウスの移動速度は変えない

    } else if (opt->opt_type == 5) {
        // AZ-Expander
        memcpy(&i2cazxp_obj, opt->data, sizeof(i2c_azxp));
        // 設定を送信
        wirelib_cls.send_azxp_setting(i2cazxp_obj.setting[0], (uint8_t *)&i2cazxp_obj.setting);
        delay(100); // 変更が入った場合AZエクスパンダが再起動するのでちょっと待つ
        // キー数とかを取得
        i2cazxp_obj.key_info = wirelib_cls.read_key_info(i2cazxp_obj.setting[0]);
        // 設定データのメモリに反映
        memcpy(opt->data, &i2cazxp_obj, sizeof(i2c_azxp));


    }
    // マッピングに合わせてキー番号を付けなおす
    if (opt->opt_type == 1 || opt->opt_type == 2 || opt->opt_type == 3 || opt->opt_type == 4 || opt->opt_type == 5) {
        // キーの番号をmapデータに入れる
        // あとでキー設定の番号入れ替えをここでやる
        memcpy(&i2cmap_obj, opt->i2cmap, sizeof(i2c_map));
        k = i2cmap_obj.map_start;
        for (i=0; i<i2cmap_obj.map_len; i++) {
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
    // output ピン設定 (colで定義されているピンを全てoutputにする)
    int c, i, j, m, x;
    int mx, my;
    int offset_buf[10][hall_len];
    File fp;

    for (i=0; i<col_len; i++) {
        if ((col_list[i] != 1 && col_list[i] != 3)) pinMode(col_list[i], OUTPUT_OPEN_DRAIN);
    }
    // row で定義されているピンを全てinputにする
    for (i=0; i<row_len; i++) {
        if ((row_list[i] != 1 && row_list[i] != 3)) pinMode(row_list[i], INPUT_PULLUP);
    }
    // direct(スイッチ直接続)で定義されているピンを全てinputにする
    for (i=0; i<direct_len; i++) {
        pinMode(direct_list[i], INPUT_PULLUP);
    }
    // 磁気スイッチピンは全てinputにする
    for (i=0; i<hall_len; i++) {
        pinMode(hall_list[i], INPUT);
    }
    // 磁気スイッチの現在の高さを初期位置にする
    if (hall_len) {
        hall_offset = new short[hall_len];
        // 全ピン時間を空けながら10回アナログ値を取得
        for (c=0; c<10; c++) {
            for (i=0; i<hall_len; i++) {
                offset_buf[c][i] = analogRead(hall_list[i]);
            }
            delay(10);
        }
        // 10回の平均をオフセットに設定
        for (i=0; i<hall_len; i++) {
            x = 0;
            for (c=0; c<10; c++) {
                x += offset_buf[c][i];
            }
            hall_offset[i] = x / 10;
        }
    }

    // キー数の計算
    key_input_length = (col_len * row_len) + direct_len + touch_len + hall_len;

    // I2C初期化
    if (ioxp_sda >= 0 && ioxp_scl >= 0) {

        // I2Cクラス初期化
        if (Wire.begin(ioxp_sda, ioxp_scl)) {
            Wire.setClock(ioxp_hz);
            M5.Lcd.printf("Wire1 begin ok\n");
        } else {
            M5.Lcd.printf("Wire1 begin ng\n");
            delay(1000);
        }

       // メインOLED初期化
        if (oled_main_addr > 0 && oled_main_width > 0 && oled_main_height > 0) {
            oled_cls = new Oled();
            oled_cls->begin(oled_main_width, oled_main_height, oled_main_addr, ioxp_hz);
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

    // 動作電圧チェック用ピン
    // power_read_pin = 36;

    input_key = new char[key_input_length];
    input_key_last = new char[key_input_length];
    key_count = new uint16_t[key_input_length];
    key_point = new short[key_input_length];
    ESP_LOGD(LOG_TAG, "key length : %D\r\n", key_input_length);
    // リセット
    for (i=0; i<key_input_length; i++) {
        this->input_key[i] = 0; // 今回のキースキャンデータ
        this->input_key_last[i] = 0; // 前回のキースキャンデータ
        this->key_count[i] = 0; // 打鍵数
        this->key_point[i] = -1; // キーごとの設定ID
    }
    this->key_count_total = 0;
}

// レイヤーが存在するか確認
bool AzCommon::layers_exists(int layer_no) {
    int i;
    for (i=0; i<setting_length; i++) {
        if (setting_press[i].layer == layer_no) return true;
    }
    return false;
}

// 指定したレイヤーを選択する
void AzCommon::layer_set(int layer_no) {
    int i;
    select_layer_no = layer_no;
    // 設定リストをリセット
    for (i=0; i<key_input_length; i++) {
        key_point[i] = -1;
    }
    // 現在のレイヤーの各キーの設定IDを取得
    for (i=0; i<setting_length; i++) {
        if (setting_press[i].layer == select_layer_no) {
            key_point[setting_press[i].key_num] = i;
        }
    }

}

// 指定したキーの入力設定オブジェクトを取得する
setting_key_press AzCommon::get_key_setting(int layer_id, int key_num, short press_type) {
    int i;
    setting_key_press r;
    r.layer = -1;
    r.key_num = -1;
    r.action_type = -1;
    r.actuation_type = ACTUATION_TYPE_DEFAULT;
    r.actuation_point = ACTUATION_POINT_DEFAULT;
    r.rapid_trigger = RAPID_TRIGGER_DEFAULT;
    // 現在のレイヤーであれば key_point に入れてあった設定を返す
    if (select_layer_no == layer_id) {
        if (key_point[key_num] < 0) return r;
        if (press_type == 0) {
            // 通常キー入力 (or 2段入力 2段目)
            return setting_press[key_point[key_num]];
        } else if (press_type == 1) {
            // 2段入力 1段目
            if (!setting_press[key_point[key_num]].sub_press_flag) return r; // 1段目設定が無かった
            return *(setting_key_press *)setting_press[key_point[key_num]].sub_press;
        }
    }
    // 現在のレイヤーでなければキー設定から指定されたキーを探す
    for (i=0; i<setting_length; i++) {
        if (setting_press[i].layer == layer_id && setting_press[i].key_num == key_num) return setting_press[i];
    }
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
    int e, i, j, k, m, n, r, x, y;
    unsigned long start_time;
    unsigned long end_time;
    uint16_t rowput_mask;
    int rowput_len;
    int read_data_bit = 8;
    uint16_t read_raw[32];
    Adafruit_MCP23X17 *ioxp;
    i2c_map i2cmap_obj;
    i2c_ioxp i2cioxp_obj;
    i2c_rotary i2crotary_obj;
    i2c_pim447 i2cpim447_obj;
    tracktall_pim447_data pim447_data_obj;
    i2c_azxp i2cazxp_obj;
    azxp_key_data azxp_key_data_obj;
    r = 0;
    e = 0;
    if (opt->opt_type == 1) {
        // IOエキスパンダ
        read_data_bit = 16;
        memcpy(&i2cioxp_obj, opt->data, sizeof(i2c_ioxp));
        memcpy(&i2cmap_obj, opt->i2cmap, sizeof(i2c_map));
        for (i=0; i<i2cioxp_obj.ioxp_len; i++) {
            x = i2cioxp_obj.ioxp[i].addr - 32; // アドレス
            // まだ初期化されていないIOエキスパンダなら無視
            if (ioxp_status[x] < 1) continue;
            // row と col があればマトリックス入力
            if (i2cioxp_obj.ioxp[i].row_len > 0 && i2cioxp_obj.ioxp[i].col_len > 0) {
                // まずrowでoutputするデータとマスクを用意
                rowput_len = i2cioxp_obj.ioxp[i].row_len;
                rowput_mask = i2cioxp_obj.ioxp[i].row_mask;
                ioxp = ioxp_obj[x];
                // rowのoutput分ループ
                for (j=0; j<rowput_len; j++) {
                    if (rowput_mask & 0xff00) { // ポートB
                        ioxp->writeGPIO((i2cioxp_obj.ioxp[i].row_output[j] >> 8) & 0xff, 1); // ポートBに出力
                    }
                    if (rowput_mask & 0xff) { // ポートA
                        ioxp->writeGPIO(i2cioxp_obj.ioxp[i].row_output[j] & 0xff, 0); // ポートAに出力
                    }
                    read_raw[e] = ~ioxp->readGPIOAB() | rowput_mask; // ポートA,B両方のデータを取得
                    e++;
                }
            } else {
                // col と row が無い場合はダイレクトのみ
                read_raw[e] = ~ioxp_obj[x]->readGPIOAB(); // ポートA,B両方のデータを取得
                e++;
            }
        }
    } else if (opt->opt_type == 2) {
        // ATTiny202 ロータリーエンコーダー
        memcpy(&i2crotary_obj, opt->data, sizeof(i2c_rotary));
        memcpy(&i2cmap_obj, opt->i2cmap, sizeof(i2c_map));
        for (i=0; i<i2crotary_obj.rotary_len; i++) {
            read_raw[e] = wirelib_cls.read_rotary(i2crotary_obj.rotary[i]);
            e++;
        }

    } else if (opt->opt_type == 3 || opt->opt_type == 4) {
        // 1U トラックボール PIM447
        memcpy(&i2cpim447_obj, opt->data, sizeof(i2c_pim447));
        memcpy(&i2cmap_obj, opt->i2cmap, sizeof(i2c_map));
        pim447_data_obj = wirelib_cls.read_trackball_pim447(i2cpim447_obj.addr); // 入力情報取得
        // トラックボール操作があればマウス移動リストに追加
        if (i2cpim447_obj.rotate == 1) { // 右が上
            x = (pim447_data_obj.down - pim447_data_obj.up);
            y = (pim447_data_obj.left - pim447_data_obj.right);
        } else if (i2cpim447_obj.rotate == 2) { // 下が上
            x = (pim447_data_obj.left - pim447_data_obj.right);
            y = (pim447_data_obj.up - pim447_data_obj.down);
        } else if (i2cpim447_obj.rotate == 3) { // 左が上
            x = (pim447_data_obj.up - pim447_data_obj.down);
            y = (pim447_data_obj.right - pim447_data_obj.left);
        } else { // それ以外は上が上
            x = (pim447_data_obj.right - pim447_data_obj.left);
            y = (pim447_data_obj.down - pim447_data_obj.up);
        }
        if (x != 0 || y != 0) {
            if (x > 127) x = 127;
            if (x < -127) x = -127;
            if (y > 127) y = 127;
            if (y < -127) y = -127;
            if (opt->opt_type == 3) {
                // トラックボール
                if (mouse_scroll_flag) {
                    m = (y == 0)? 0: (y > 0)? 1: -1;
                    n = (x == 0)? 0: (x > 0)? 1: -1;
                    press_mouse_list_push(0x2000, 5, 0, 0, m, n, 100); // action_type : 5 = マウス移動
                } else {
                    press_mouse_list_push(0x2000, 5, x, y, 0, 0, i2cpim447_obj.speed); // action_type : 5 = マウス移動
                }
            } else if (opt->opt_type == 4) {
                // フリック入力
                read_raw[e] = 0;
                if (x < -2 && abs(x) > abs(y)) {
                    read_raw[e] = 1; // 左
                } else if (x > 2 && abs(x) > abs(y)) {
                    read_raw[e] = 2; // 右
                } else if (y < -2 && abs(y) > abs(x)) {
                    read_raw[e] = 4; // 上
                } else if (y > 2 && abs(y) > abs(x)) {
                    read_raw[e] = 8; // 下
                }
                e++;
            }
        }
        // キー入力(クリック)取得
        read_raw[e] = pim447_data_obj.click;
        e++;

    } else if (opt->opt_type == 5) {
        // AZエクスパンダ
        read_data_bit = 8;
        memcpy(&i2cazxp_obj, opt->data, sizeof(i2c_azxp));
        memcpy(&i2cmap_obj, opt->i2cmap, sizeof(i2c_map));
        // キー入力を取得
        azxp_key_data_obj = wirelib_cls.read_azxp_key(i2cazxp_obj.setting[0], i2cazxp_obj.key_info);
        for (i=0; i<i2cazxp_obj.key_info.key_byte; i++) {
            read_raw[e] = azxp_key_data_obj.key_input[i]; // 読み込んだデータをチェック用の変数へ入れる
            e++;
        }

    }
    // 読み込んだデータからキー入力を取得
    if (opt->opt_type == 1 || opt->opt_type == 2 || opt->opt_type == 3 || opt->opt_type == 4 || opt->opt_type == 5) {
        // マップデータ分入力を取得
        for (j=0; j<i2cmap_obj.map_len; j++) {
            n = i2cmap_obj.map[j];
            k = n / read_data_bit;
            m = n % read_data_bit;
            if (k >= e) k = e - 1; // マトリックスで取得した数より多い場合はdirectの指定なので一番最後に取得した情報で判定
            read_data[p] = (read_raw[k] & (0x01 << m))? 1: 0;
            p++;
            r++;
        }
    }
    return r;
}

// 現在のキーの入力状態を取得
void AzCommon::key_read(void) {
    int a, c, i, j, m, n, x, s;
    int act, acp, acpt, rap;
    int o[ioxp_len];
    setting_key_press *k;
    n = 0;
    // ダイレクト入力の取得
    for (i=0; i<direct_len; i++) {
        input_key[n] = !digitalRead(direct_list[i]);
        n++;
    }
    // タッチ入力の取得
    for (i=0; i<touch_len; i++) {
        if (touchRead(touch_list[i]) < 25) {
            input_key[n] = 1;
        } else {
            input_key[n] = 0;
        }
        n++;
    }
    // 磁気スイッチの取得
    for (i=0; i<hall_len; i++) {
        // 設定からアクチュエーションポイント、ラピットトリガー取得
        if (key_point[n] >= 0) {
            k = &setting_press[key_point[n]]; // キーの設定取得
            act = k->action_type;
            acpt = k->actuation_type;
            acp = k->actuation_point;
            rap = k->rapid_trigger;
        } else {
            // 設定がなければデフォルト値
            act = 0;
            acpt = ACTUATION_TYPE_DEFAULT;
            acp = ACTUATION_POINT_DEFAULT;
            rap = RAPID_TRIGGER_DEFAULT;
        }
        // 現在のアナログ値取得
        a = analogRead(hall_list[i]);
        m = map(a, hall_offset[i] + hall_range_min, hall_offset[i] + hall_range_max, 0, 255);
        if (m > 255) m = 255;
        if (m < 0) m = 0;
        input_key_analog[i] = m;
        if (acpt == 0) {
            // 静的なアクチュエーションポイントとラピットトリガー
            // 固定位置で判定
            if (input_key_analog[i] > acp) { // アクチュエーションポイントを超えたらON
                input_key[n] = 1;
            } else if (input_key_analog[i] < rap) { // ラピットトリガーを下回ったらOFF
                input_key[n] = 0;
            } else { // 中間地点にいる場合は前のステータスを引き継ぐ
                input_key[n] = input_key_last[n];
            }

        } else if (acpt == 1) {
            // 動的なアクチュエーションポイントとラピットトリガー
            // (移動距離で判定)
            if (input_key_last[n] == 0) { // 前回が未入力
                if (input_key_analog[i] > (analog_stroke_most[i] + acp) || input_key_analog[i] > 240) {
                    // アクチュエーションポイントを超えたらON
                    input_key[n] = 1;
                    analog_stroke_most[i] = input_key_analog[i]; // ONになった位置を保持
                } else {
                    input_key[n] = 0; // アクチュエーションポイント超えるまではOFFのまま
                    // 離されたらOFFになった位置を更新する
                    if (analog_stroke_most[i] > input_key_analog[i]) {
                        analog_stroke_most[i] = input_key_analog[i];
                    }

                }
            } else if (input_key_last[n] == 1) { // 前回がON
                if (input_key_analog[i] < (analog_stroke_most[i] - rap) || input_key_analog[i] < 12) { // 最も押し込んだ位置からラピットトリガー分戻ったらリセット
                    input_key[n] = 0;
                    analog_stroke_most[i] = input_key_analog[i]; // OFFになった位置を保持
                } else {
                    input_key[n] = 1; // ラピットトリガーを下回るまではONのまま
                    // 深く押し込まれたらONになった位置を更新する
                    if (analog_stroke_most[i] < input_key_analog[i]) {
                        analog_stroke_most[i] = input_key_analog[i];
                    }
                }
            } else {
                input_key[n] = 0;
            }

        } else if (acpt == 2) {
            // 2段階入力
            if (input_key_last[n] == 0) { // 前回が未入力
                if (input_key_analog[i] > 210) {
                    // 2段目まで押し込まれたら2段目にする
                    input_key[n] = 3; // 2段目ON
                    analog_stroke_most[i] = 0; // カウンタリセット

                } else if (input_key_analog[i] > 40) {
                    // 1段目の深さの場合
                    analog_stroke_most[i]++; // 超えたよ数をカウントしていき、5回を超えたらONにする(素早い入力の時は1段目を飛ばすため)
                    if (analog_stroke_most[i] > 5) {
                        input_key[n] = 2; // 1段目ON
                    } else {
                        input_key[n] = 0;
                    }
                } else {
                    // 浅い位置にいればカウントもリセット
                    input_key[n] = 0;
                    analog_stroke_most[i] = 0;
                }
            } else if (input_key_last[n] == 2) { // 前回が1段目ON
                if (input_key_analog[i] > 210) {
                    // 2段目の深さまで押し込まれた
                    input_key[n] = 3; // 2段目ON
                    analog_stroke_most[i] = 0; // カウンタリセット
                } else if (input_key_analog[n] < 30) {
                    // 浅い位置に戻った
                    input_key[n] = 0; // OFF
                    analog_stroke_most[i] = 0; // カウンタリセット
                } else {
                    input_key[n] = 2;
                    
                }
                
            } else if (input_key_last[n] == 3) { // 前回が2段目ON
                if (input_key_analog[i] < 30) {
                    // 浅い位置に戻った
                    input_key[n] = 0; // OFF
                    analog_stroke_most[i] = 0; // カウンタリセット

                } else if (input_key_analog[i] < 200) {
                    // 2段目まで戻った
                    analog_stroke_most[i]++; // 戻ったよ数をカウントしていき、5回を超えたらONにする(素早い入力の時は1段目を飛ばすため)
                    if (analog_stroke_most[i] > 5) {
                        input_key[n] = 2; // 2段目ON　＋　1段目OFF
                        analog_stroke_most[i] = 0; // カウンタリセット
                    } else {
                        input_key[n] = 3;
                    }
                } else {
                    // 深い位置
                    input_key[n] = 3;
                    analog_stroke_most[i] = 0; // カウンタリセット
                }
            }

        }
        n++;
    }
    // マトリックス入力の取得
    for (i=0; i<col_len; i++) {
        // 対象のcolピンのみ lowにする
        for (j=0; j<col_len; j++) {
            if (i == j) { s = 0; } else { s = 1; }
            if ((col_list[j] != 1 && col_list[j] != 3)) digitalWrite(col_list[j], s);
        }
        delayMicroseconds(50);
        // row の分キー入力チェック
        for (j=0; j<row_len; j++) {
            input_key[n] = !digitalRead(row_list[j]);
            n++;
        }
    }
    // IOエキスパンダ
    for (i=0; i<ioxp_len; i++) {
        x = ioxp_list[i] - 32;
        if (ioxp_status[x] == 1) {
            o[i] = ioxp_obj[x]->readGPIOAB();
            if (o[i] == 0) {
                // 全ピン読み込み無しだとIOエキスパンダの接続が切れたかリセットした可能性あり
                ioxp_status[x] = 2; // 一旦接続切れたステータスにする
                o[i] = 0xffff; // 全ボタン離した状態
            }
        } else if (ioxp_status[x] > 256) {
            // 接続が切れた状態の場合100周期に1回ピンの初期化を送信してステータスリセット
            for (j = 0; j < 16; j++) {
                ioxp_obj[x]->pinMode(j, INPUT_PULLUP);
            }
            ioxp_status[x] = 0; // ステータスを接続状態に戻す
            o[i] = 0xffff; // 全ボタン離した状態
        } else if (ioxp_status[x] > 1) {
            // IOエキスパンダ接続していない間カウントアップ
            ioxp_status[x]++;
            o[i] = 0xffff; // 全ボタン離した状態
        } else {
            o[i] = 0xFFFF; // 全てのボタン離した状態
        }
    }
    for (i=0; i<16; i++) {
        c = 1 << i;
        s = 0;
        for (j=0; j<ioxp_len; j++) {
            input_key[i + s + n] = (!(o[j] & c));
            s += 16;
        }
    }
    n += ioxp_len * 16;
    // I2Cオプション
    for (i=0; i<i2copt_len; i++) {
        n += i2c_read(n, &i2copt[i], input_key);
    }
}


// 今回の入力状態を保持
void AzCommon::key_old_copy(void) {
    int i;
    for (i=0; i<key_input_length; i++) {
        input_key_last[i] = input_key[i];
    }
}

// 全てのファイルを削除
void AzCommon::delete_all(void) {
    SPIFFS.format();
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

// マウス移動リストを空にする
void AzCommon::press_mouse_list_clean() {
    int i;
    for (i=0; i<PRESS_MOUSE_MAX; i++) {
        press_mouse_list[i].key_num = -1;
        press_mouse_list[i].action_type = 0;
        press_mouse_list[i].move_x = 0;
        press_mouse_list[i].move_y = 0;
        press_mouse_list[i].move_speed = 0;
        press_mouse_list[i].move_index = 0;
    }
}


// マウス移動リストに追加
void AzCommon::press_mouse_list_push(int key_num, short action_type, short move_x, short move_y, short move_wheel, short move_hWheel, short move_speed) {
    int i;
    for (i=0; i<PRESS_MOUSE_MAX; i++) {
        // データが入っていれば次
        if (press_mouse_list[i].key_num >= 0) continue;
        // 空いていればデータを入れる
        press_mouse_list[i].key_num = key_num;
        press_mouse_list[i].action_type = action_type;
        press_mouse_list[i].move_x = move_x;
        press_mouse_list[i].move_y = move_y;
        press_mouse_list[i].move_wheel = move_wheel * -1;
        press_mouse_list[i].move_hWheel = move_hWheel;
        press_mouse_list[i].move_speed = move_speed;
        press_mouse_list[i].move_index = 0;
        break;
    }
}


// マウス移動リストから削除
void AzCommon::press_mouse_list_remove(int key_num) {
    int i;
    for (i=0; i<PRESS_MOUSE_MAX; i++) {
        // 該当のキーで無ければ何もしない
        if (press_mouse_list[i].key_num != key_num) continue;
        // 該当のキーのデータ削除
        press_mouse_list[i].key_num = -1;
        press_mouse_list[i].move_x = 0;
        press_mouse_list[i].move_y = 0;
        press_mouse_list[i].move_speed = 0;
        press_mouse_list[i].move_index = 0;
    }
}
