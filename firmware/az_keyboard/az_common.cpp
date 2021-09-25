#include "az_common.h"


// キーが押された時の設定
uint16_t setting_length;
setting_key_press *setting_press;
// wifi設定
uint8_t wifi_data_length;
setting_wifi *wifi_data;
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


// ステータスLED今0-9
int status_led_bit = 0;

// ステータスLED表示モード
int status_led_mode;

// マウスパッドステータス
int8_t mouse_pad_status;

// マウスパッド操作方法設定
int8_t mouse_pad_setting;

// 液晶表示用オブジェクト
Display *disp;

// rgb_led制御用クラス
Neopixel rgb_led_cls = Neopixel();

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

// キーボードタイプの番号
int keyboard_type_int;

// キーボードの名前
const char *keyboard_name_str;

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

// リスタート用のフラグ
int8_t restart_flag;
int8_t restart_index;

// 設定メニューを表示しているかどうか
bool menu_mode_flag;

// オールクリア送信フラグ
int press_key_all_clear;

// eepromのデータ
mrom_data_set eep_data;

// 起動回数
uint32_t boot_count;

// 打鍵数を自動保存するかどうか
uint8_t key_count_auto_save;

// 入力用ピン情報
short direct_len;
short touch_len;
short *direct_list;
short *touch_list;
short ioxp_len;
short *ioxp_list;

// バッテリーオブジェクト
AXP192 power;

// I/Oエキスパンダ用
Adafruit_MCP23X17 *iomcp;

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

// コンストラクタ
AzCommon::AzCommon() {
}

// 共通処理の初期化
void AzCommon::common_start() {
    // M5Stack初期化
    M5.begin(true, true, true, true);
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
        press_key_list[i].unpress_time = -1;
    }
    last_touch_x = -1;
    last_touch_y = -1;

    // マウスパッドステータス
    mouse_pad_status = 0;
    // マウスパッド操作設定ステータス
    mouse_pad_setting = 0;
    // リスタートフラグ
    restart_flag = -1;
    restart_index = -1;
    // メニューを表示しているかどうか
    menu_mode_flag = false;
    // LVGL表示インデックス
    lvgl_loop_index = 0;
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
        if (!create_setting_json()) return;
    }
    // ファイルオープン
    File json_file = SPIFFS.open(SETTING_JSON_PATH);
    if(!json_file){
        // ファイルが開けなかった場合はデフォルトファイル作り直して再起動
        ESP_LOGD(LOG_TAG, "json file open error\n");
        create_setting_json();
        delay(1000);
        ESP.restart(); // ESP32再起動
        return;
    }
    // 読み込み＆パース
    DeserializationError err = deserializeJson(setting_doc, json_file);
    if (err) {
        ESP_LOGD(LOG_TAG, "load_setting_json deserializeJson error\n");
        create_setting_json();
        delay(1000);
        ESP.restart(); // ESP32再起動
        return;
    }
    json_file.close();
    // オブジェクトを保持
    setting_obj = setting_doc.as<JsonObject>();

    // キーボードタイプは必須なので項目が無ければ設定ファイル作り直し(設定ファイル壊れた時用)
    if (!setting_obj.containsKey("keyboard_type")) {
        ESP_LOGD(LOG_TAG, "json not keyboard_type error\n");
        create_setting_json();
        delay(1000);
        ESP.restart(); // ESP32再起動
        return;
    }

    // キーボードタイプの番号を取得する
    String ktype = setting_obj["keyboard_type"].as<String>();
    get_keyboard_type_int(setting_obj["keyboard_type"].as<String>());

    // キーボードの名前を取得する
    keyboard_name_str = setting_obj["keyboard_name"].as<const char*>();

    // キーボードのタイプがeep_dataと一致しなければ現在選択しているキーボードと、設定ファイルのキーボードが一致していないので
    // 現在選択しているキーボードのデフォルト設定ファイルを作成して再起動
    if (!ktype.equals(eep_data.keyboard_type)) {
        create_setting_json();
        delay(1000);
        ESP.restart(); // ESP32再起動
        return;
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
    int i, j, m;
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
    // キーの設定を取得
    // まずは設定の数を取得
    char lkey[16];
    char kkey[16];
    int lnum, knum;
    JsonObject::iterator it_l;
    JsonObject::iterator it_k;
    JsonObject layers, keys;
    JsonObject press_obj;
    String text_str;
    setting_normal_input normal_input;
    setting_mouse_move mouse_move_input;
    // まずはキー設定されている数を取得
    layers = setting_obj["layers"].as<JsonObject>();
    setting_length = 0;
    for (it_l=layers.begin(); it_l!=layers.end(); ++it_l) {
        setting_length += setting_obj["layers"][it_l->key().c_str()]["keys"].size();
    }
    ESP_LOGD(LOG_TAG, "setting total %D\n", setting_length);
    // 設定数分メモリ確保
    ESP_LOGD(LOG_TAG, "mmm: %D %D\n", heap_caps_get_free_size(MALLOC_CAP_32BIT), heap_caps_get_free_size(MALLOC_CAP_8BIT) );
    setting_press = new setting_key_press[setting_length];
    ESP_LOGD(LOG_TAG, "mmm: %D %D\n", heap_caps_get_free_size(MALLOC_CAP_32BIT), heap_caps_get_free_size(MALLOC_CAP_8BIT) );
    // キー設定読み込み
    i = 0;
    for (it_l=layers.begin(); it_l!=layers.end(); ++it_l) {
        sprintf(lkey, "%S", it_l->key().c_str());
        lnum = split_num(lkey);
        keys = setting_obj["layers"][lkey]["keys"].as<JsonObject>();
        for (it_k=keys.begin(); it_k!=keys.end(); ++it_k) {
            sprintf(kkey, "%S", it_k->key().c_str());
            knum = split_num(kkey);
            // ESP_LOGD(LOG_TAG, "layers %S %S [ %D %D ]\n", lkey, kkey, lnum, knum);
            press_obj = setting_obj["layers"][lkey]["keys"][kkey]["press"].as<JsonObject>();
            setting_press[i].layer = lnum;
            setting_press[i].key_num = knum;
            setting_press[i].action_type = press_obj["action_type"].as<signed int>();
            if (setting_press[i].action_type == 1) {
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
                setting_press[i].data = (char *)new setting_normal_input;
                memcpy(setting_press[i].data, &normal_input, sizeof(setting_normal_input));
            } else if (setting_press[i].action_type == 2) {
                // テキスト入力
                text_str = press_obj["text"].as<String>();
                m = text_str.length() + 1;
                setting_press[i].data = new char[m];
                text_str.toCharArray(setting_press[i].data, m);
            } else if (setting_press[i].action_type == 3) {
                // レイヤー切り替え
                setting_press[i].data = new char;
                *setting_press[i].data = press_obj["layer"].as<signed int>();
            } else if (setting_press[i].action_type == 4) {
                // WEBフック
                text_str = "";
                serializeJson(press_obj["webhook"], text_str);
                m = text_str.length() + 1;
                setting_press[i].data = new char[m];
                text_str.toCharArray(setting_press[i].data, m);
                
            } else if (setting_press[i].action_type == 5) {
                // マウス移動
                mouse_move_input.x = press_obj["move"]["x"].as<signed int>();
                mouse_move_input.y = press_obj["move"]["y"].as<signed int>();
                mouse_move_input.speed = press_obj["move"]["speed"].as<signed int>();
                setting_press[i].data = (char *)new setting_mouse_move;
                memcpy(setting_press[i].data, &mouse_move_input, sizeof(setting_mouse_move));

            } else if (setting_press[i].action_type == 6) {
                // 暗記ボタン
                text_str = press_obj["ankey_file"].as<String>();
                m = text_str.length() + 1;
                setting_press[i].data = new char[m];
                text_str.toCharArray(setting_press[i].data, m);

            } else if (setting_press[i].action_type == 7) {
                // LED設定ボタン
                setting_press[i].data = new char;
                *setting_press[i].data = press_obj["led_setting_type"].as<signed int>();
                
            } else if (setting_press[i].action_type == 8) {
                // 打鍵設定ボタン
                setting_press[i].data = new char;
                *setting_press[i].data = press_obj["dakagi_settype"].as<signed int>();

            }
            i++;
        }
    }
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

    // wifiの設定読み出し
    String ssid, pass;
    wifi_data_length = setting_obj["wifi"].size(); // wifiの設定数
    wifi_data = new setting_wifi[wifi_data_length];
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
    if (led_num_length > 0 && key_matrix_length > 0) {
        led_num = new int8_t[led_num_length];
        for (i=0; i<led_num_length; i++) {
            led_num[i] = setting_obj["led_num"][i].as<signed int>();
        }
        key_matrix = new int8_t[key_matrix_length];
        for (i=0; i<key_matrix_length; i++) {
            key_matrix[i] = setting_obj["key_matrix"][i].as<signed int>();
        }
    }

}


// キーボードタイプの番号を取得する
void AzCommon::get_keyboard_type_int(String t) {
    if (t.equals("custom")) { keyboard_type_int = 1; } // カスタム
    else if (t.equals("az_m5ortho")) { keyboard_type_int = 2; } // AZ-M5ortho
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
    if (strcmp(eep_data.keyboard_type, "az_m5macro") == 0) {
        i = json_file.print(setting_azm5macro_default_min_json_bin); // AZ-M5orthoのデフォルト設定
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

// キーの入力ピンの初期化
void AzCommon::pin_setup() {
    
  // I2C初期化
  if (Wire.begin(26, 14)) {
      Wire.setClock(1700000);
      M5.Lcd.printf("Wire1 begin ok\n");
  } else {
      M5.Lcd.printf("Wire1 begin ng\n");
  }

  // IOエキスパンダ初期化
  int i, j;
  M5.Lcd.printf("Adafruit_MCP23X17 %D\n", ioxp_len);
  iomcp = new Adafruit_MCP23X17[ioxp_len];
  for (i=0; i<ioxp_len; i++) {
      if (iomcp[i].begin_I2C(ioxp_list[i], &Wire)) {
          M5.Lcd.printf("begin_I2C %D  %D OK\n", i, ioxp_list[i]);
      } else {
          M5.Lcd.printf("begin_I2C %D %D NG\n", i, ioxp_list[i]);
          return;
      }
      for (j = 0; j < 16; j++) {
          iomcp[i].pinMode(j, INPUT_PULLUP);
      }
  }

  key_input_length = 52;
  this->key_count_total = 0;
}


// キー入力ピン初期化のキーボード別の処理
void AzCommon::pin_setup_sub_process() {
    if (option_type_int == 1) {
        // 踏みキー
        pinMode(27, INPUT_PULLUP);
    }
}

// レイヤーが存在するか確認
bool AzCommon::layers_exists(int layer_no) {
    int i;
    for (i=0; i<setting_length; i++) {
        if (setting_press[i].layer == layer_no) return true;
    }
    return false;
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


//データをEEPROMから読み込む。保存データが無い場合デフォルトにする。
void AzCommon::load_data() {
    // デフォルト値セット
    // 起動モード
    eep_data.boot_mode = 0;
    // データチェック文字列
    strcpy(eep_data.check, EEP_DATA_VERSION);
    // WIFIアクセスポイントの名前
    char b[16];
    getRandomNumbers(4, b);
    sprintf(eep_data.ap_ssid, "AZ-Keyboard-%S", b);
    // ユニークID
    getRandomNumbers(10, eep_data.uid);
    // キーボードの種類
    strcpy(eep_data.keyboard_type, "");
    File fp;
    if (!SPIFFS.exists(SYSTEM_FILE_PATH)) {
        // ファイルが無い場合デフォルト値でファイルを作成
        save_data();
        return;
    }
    // ファイルがあればデータ読み込み
    fp = SPIFFS.open(SYSTEM_FILE_PATH, "r");
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
        SPIFFS.remove(SYSTEM_FILE_PATH);
        ESP.restart(); // ESP32再起動
    }
}


// データをEEPROMに書き込む
void AzCommon::save_data() {
    //EEPROMに設定を保存する。
    File fp;
    strcpy(eep_data.check, EEP_DATA_VERSION);
    ESP_LOGD(LOG_TAG, "save eeprom boot mode: %D", eep_data.boot_mode);
    ESP_LOGD(LOG_TAG, "save eeprom check sum: %S", eep_data.check);
    ESP_LOGD(LOG_TAG, "keyboard_type: %S", eep_data.keyboard_type);
    ESP_LOGD(LOG_TAG, "data sizeof: %D", sizeof(mrom_data_set));
    // ファイルに書き込み
    fp = SPIFFS.open(SYSTEM_FILE_PATH, "w");
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


// 現在のキーの入力状態を取得
void AzCommon::key_read(void) {
    unsigned long start_time;
    unsigned long end_time;
    int m[ioxp_len];
    int i, j, n, c;
    bool idata[72];
    char convert_map[] = {
      37, 25, 13, 1, 36, 24, 12, 0, // 0 - 7
      2, 14, 26, 38, 3, 15, 27, 39, // 8 - 15
      41, 29, 17, 5, 40, 28, 16, 4, // 16 - 23
      42, 43, 44, // 24 - 26
      -1, -1, -1, -1, -1, // 27 - 31
      46, 31, 19, 7, 45, 30, 18, 6, // 32 - 39
      -1, -1, -1, -1, -1, -1, -1, -1, // 40 - 47
      48, 33, 21, 9, 47, 32, 20, 8, // 48 - 55
      10, 22, 34, 49, 11, 23, 35, 50 // 56 - 63
      };
    for (i=0; i<70; i++) idata[i] = false;
    start_time = millis();
    for (i=0; i<ioxp_len; i++) m[i] = iomcp[i].readGPIOAB();
    for (i=0; i<16; i++) {
        c = 1 << i;
        n = 0;
        for (j=0; j<ioxp_len; j++) {
            idata[i + n] = (!(m[j] & c));
            n += 16;
        }
    }
    end_time = millis();
    for (i=0; i<70; i++) {
        if (convert_map[i] < 0) continue;
        if (idata[i]) {
            input_key[convert_map[i]] = 1;
        } else {
            input_key[convert_map[i]] = 0;
        }
    }
    // M5.Lcd.printf("\n%D", end_time - start_time);


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
