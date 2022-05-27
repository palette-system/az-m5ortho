
// モーダルのオプション
window.REMODAL_GLOBALS = {
    "closeOnOutsideClick": false,
    "hashTracking": false
};

if (!window.aztool) aztool = {};

// 設定JSONファイルのパス
aztool.setting_json_path = "/setting.json";

// 設定JSONのデータ
aztool.setting_json_txt = ""; // テキスト
aztool.setting_json_data = {}; // データ

// ロード用I2C用のインデックス
aztool.i2c_load_index = 0;
aztool.i2c_option_data = {};

aztool.step_max = 0;
aztool.step_index = 0; // 今のステップ

// aztool初期化
aztool.init = function() {
    // webhidオブジェクト初期化
    webhid.disconnect_func = aztool.hid_disconn_func; // 接続が切れた時のイベント
    webhid.init({"info_div": "console_div"});
    // ピン設定の初期化
    pinstp.init();
    // キー設定ページの初期化
    aztool.setmap_init();
    // キー動作設定用のモーダル初期化
    aztool.keyact_init();
    // 接続ページを表示
    aztool.view_connect_top();
};


// キーボードに接続
aztool.connect = function() {
    // コネクション開始
    webhid.connect(function(stat) {
        // コネクション失敗
        if (stat != 0) {
            aztool.view_connect_top("接続できませんでした。"); // 接続ページを表示
            return;
        }
        // aztool.addopt_start("main_box"); // オプション追加
        // 接続成功したら設定JSON読み込み
        aztool.load_setting_json();
    });
};

// 接続が切れた時に呼び出される
aztool.hid_disconn_func = function(e) {
    // 接続ページを表示
    aztool.view_connect_top("切断しました " + aztool.to_hex(e.device.productId, 4) + " : " + aztool.to_hex(e.device.vendorId, 4));
};

// 設定JSONの読み込み
aztool.load_setting_json = function() {
    aztool.view_load_page(); // ロード画面表示
    webhid.get_file(aztool.setting_json_path, function(stat, load_data) {
        // 読み込み失敗
        if (stat != 0) return;
        // 読み込み成功したらデータを受けとる
        let txt = webhid.arr2str(load_data);
        aztool.setting_json_txt = txt; // 設定データテキスト
        aztool.setting_json_data = JSON.parse(txt); // 設定データパース
        console.log(aztool.setting_json_txt);
        console.log(aztool.setting_json_data);
        // 別で読み込みが必要なi2cオプションのデータをロード
        aztool.i2c_option_data = {};
        aztool.i2c_load_index = 0;
        aztool.load_i2c_data();
    });
};

// i2cデータのロード
aztool.load_i2c_data = function() {
    // i2cオプションのロード終わったらメニューを表示
    if (!aztool.setting_json_data.i2c_option || // i2cの設定が無い
        aztool.i2c_load_index >= aztool.setting_json_data.i2c_option.length) { // 全てロード完了
            aztool.view_top_menu();
            return;
    }
    // i2cのデータをロード
    let o = aztool.setting_json_data.i2c_option[ aztool.i2c_load_index ];
    let t;
    // IOエキスパンダ
    if (o.type == 1) {
        // kleのJSONロード
        console.log("get_file: /o" + o.id);
        webhid.get_file("/o" + o.id, function(stat, load_data) {
            if (stat != 0) {
                // 読み込み失敗 空のデータを入れる
                aztool.i2c_option_data[ "o" + o.id ] = "[]";
            } else {
                // kleJSON取得
                aztool.i2c_option_data[ "o" + o.id ] = webhid.arr2str(load_data);
            }
            // 次のオプションを取得
            aztool.i2c_load_index++;
            aztool.load_i2c_data();
        });
        return;
    }
    // 不明なオプションタイプ
    aztool.i2c_load_index++;
    aztool.load_i2c_data();
};

// データのロードページ表示
aztool.view_load_page = function() {
    $("#main_box").html("<div id='console_div'></div>");
};

// コネクションページ表示
aztool.view_connect_top = function(msg) {
    let h = "";
    h += "<input type='button' value='キーボードに接続' onClick='javascript:aztool.connect();'>";
    if (msg) {
        h += "<br>" + msg;
    }
    $("#main_box").html(h);
};

// トップメニューの表示
aztool.view_top_menu = function() {
    let k = aztool.setting_json_data;
    let h = "";
    let d, x;
    h += "<table><tr>";
    h += "<td>";
    h += "vendorId / productId： " + k.vendorId + " / " + k.productId + "<br>";
    h += "キーボード名：" + k.keyboard_name + "<br>";
    x = (k.keyboard_pin.ioxp)? k.keyboard_pin.ioxp.join(","): "";
    d = (k.keyboard_pin.direct)? k.keyboard_pin.direct.join(","): "";
    h += "キーピン： direct="+d+"　ioxp="+x+"<br>";
    console.log(k);
    if (k.i2c_set && k.i2c_set.length == 3) {
        h += "I2Cピン：  SDA= " + k.i2c_set[0] + " / SCL= " + k.i2c_set[1] + " / " + k.i2c_set[1] + " Hz<br>";
    }
    h += "</td></tr></table>";
    // h += "<div id='key_layout_box' style='width: 1000px; height: 500px;overflow: hidden; border: solid 1px black; text-align: left;'></div>";
    h += "<br><br>";
    h += "<a href='#' onClick='javascript:aztool.view_setmap();'>キーマップ設定</a><br>";
    h += "<a href='#' onClick='javascript:aztool.view_setopt();'>オプション設定</a><br>";
    h += "<a href='#' onClick='javascript:aztool.edit_setting_json();'>設定JSON編集</a><br>";
    $("#main_box").html(h);
    // キー配列を表示
    // aztool.view_key_layout();
};

// キーボードを再起動
aztool.keyboard_restart = function(boot_type) {
    $("#main_box").html("キーボードを再起動します。");
    webhid.m5_restart(0); // キーボードモードで再起動
};

// 設定JSON編集
aztool.edit_setting_json = function() {
    aztool.txtedit_open(aztool.setting_json_path, function(stat, save_flag) {
        // 変更されていればキーボードを再起動
        if (save_flag) {
            aztool.keyboard_restart(0); // キーボードモードで再起動
            return;
        }
        aztool.view_top_menu();
    });
};

// 設定配列に反映した内容をJSONにして保存
aztool.setting_json_save = function(cb_func) {
    // 設定JSONデータ作成
    aztool.setting_json_txt = JSON.stringify(aztool.setting_json_data);
    // 保存
    webhid.save_file(
        aztool.setting_json_path, // 保存先
        aztool.setting_json_txt, // 保存データ
        cb_func);
};

// オプションのステップ信仰の表示を更新する
aztool.update_step_box = function(step_num) {
    let i, c;
    aztool.step_index = step_num;
    for (i=1; i<=aztool.step_max; i++) {
        c = "option_step";
        if (step_num == i) {
            c += " step_selected";
        } else if (step_num > i) {
            c += " step_ended";
        }
        $("#stepbox_" + i).attr("class", c);
    }
};

