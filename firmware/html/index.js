// JavaScript 圧縮ツール
//   https://tm-webtools.com/Tools/jsMinify

// jquery もどき
function $(id) {
    return document.getElementById(id);
}
// 要素表示
function show(id, display_type) {
    if (!$(id)) return;
    if (display_type) {
        $(id).style.display = display_type;
    } else {
        $(id).style.display = "block";
    }
}
// 要素非表示
function hide(id) {
    if (!$(id)) return;
    $(id).style.display = "none";
}
// HTML表示
function set_html(id, html) {
    $(id).innerHTML = html;
}
// オブジェクトのクローン作製
function obj_clone(obj) {
    return JSON.parse(JSON.stringify(obj));
}
// オブジェクトの比較
function obj_eq(obj_a, obj_b) {
    return (JSON.stringify(obj_a) == JSON.stringify(obj_b));
}
// GETメソッド送信
function ajax(path, res_type, cb_func) {
    var req = new XMLHttpRequest();
    req.open("GET", path, true); // 第3パラメータ true で非同期
    req.responseType = res_type;
    req.setRequestHeader('Pragma', 'no-cache');
    req.setRequestHeader('Cache-Control', 'no-cache');
    req.setRequestHeader('If-Modified-Since', 'Thu, 01 Jun 1970 00:00:00 GMT');
    req.onreadystatechange = function() {
        if (req.readyState < 4) return;
        if(req.status == 200 ) {
            cb_func(true, req.response);
        } else {
            cb_func(false, req);
        }
    }
    req.send();
}

// POSTメソッド送信
function ajax_post(path, send_data, cb_func) {
    var req = new XMLHttpRequest();
    req.open("POST", path, true); // 第3パラメータ true で非同期
    req.setRequestHeader("Content-Type", "text/plain");
    req.setRequestHeader('Pragma', 'no-cache');
    req.setRequestHeader('Cache-Control', 'no-cache');
    req.setRequestHeader('If-Modified-Since', 'Thu, 01 Jun 1970 00:00:00 GMT');
    req.onreadystatechange = function() {
        if (req.readyState < 4) return;
        if(req.status == 200 ) {
            cb_func(true, req.responseText, req);
        } else {
            cb_func(false, req);
        }
    }
    req.send(send_data);
}


if (!window.mst) window.mst = {};


// 設定JSONデータ
mst.setting_data = {
    "ap": {"pass": ""},
    "wifi": [{"ssid": "", "pass": ""}],
    "status_pin": 13,
    "keyboard_language": 0,
    "keyboard_type": mst.key_pattern_default,
    "keyboard_pin": {
        "row": [],
        "col": [],
        "direct": [15]
    },
    "default_layer": 0,
    "layers": {
        "layer_0": {
            "name": "レイヤー0",
            "keys": {
                "key_0": {
                    "press": {
                        "action_type": 1,
                        "key": [4]
                    },
                    "long_press": {},
                    "long_press_time": 0
                }
            }
        }
    },
    "option_set": {"type": ""}
};

// ファームウェアのバージョン
mst.firmware_version = "none";

// 今編集中のレイヤー
mst.edit_layer = 0;

// セレクト実行時のコールバック関数保持する変数
mst.select_exec_cbfunc = null;

// 編集中のwifiデータ
mst.wifi_edit_kid = -1;
mst.wifi_edit_data = null;

// キー設定編集中のデータ
mst.key_edit_kid = -1;
mst.key_edit_data = null;

// 編集中のオプションデータ
mst.option_edit_data = null;

// ファームウェア選択されたファイルの情報
mst.firmware_file_data = null;

// ボタン接触確認中かどうか
mst.switch_check_mode = 0;

// 設定終了かどうか
mst.end_flag = false;

// 終了タイプ
mst.end_type = 0;

// 削除するファイルリスト
mst.delete_file_list = [];

// 初期処理
mst.init = function() {
    // スマホなら画面を90度回転
    if (mst.is_mobile()) mst.body_rotate();
    // ブラウザのボタン設定
    mst.browser_btn_control();
    // 読み込みメッセージ表示
    set_html("info_box", "読み込み中..");
    mst.view_box(["info_box"]);
    // ファームウェアのバージョン取得
    ajax("/firmware_version", "text", function(stat, res) {
        if (stat && res) {
            var r = JSON.parse(res);
            mst.firmware_version = r.v;
            mst.esp_type = parseInt(r.t);
        }
        // 設定JSON取得
        mst.get_setting_json();
    });
};

mst.popstate_func = function(e) {
    history.pushState(null, null,  location.href);
};
mst.popstate_func_ov = function(e) {
    updateBreadCrumbObservable();
};

mst.beforeunload_func = function(e) {
    e.preventDefault();
    return "別のページへ移動しますか？";
};

// ブラウザのボタン設定
mst.browser_btn_control = function() {
    // 戻るボタン無効化
    history.pushState(null, null,  location.href);
    window.addEventListener("popstate", mst.popstate_func, false);
    window.addEventListener("popstate", mst.popstate_func_ov, false);
    window.addEventListener("beforeunload", mst.beforeunload_func, false);
};

// ページ移動警告解除
mst.browser_btn_control_remove = function() {
    window.removeEventListener("popstate", mst.popstate_func, false);
    window.removeEventListener("popstate", mst.popstate_func_ov, false);
    window.removeEventListener("beforeunload", mst.beforeunload_func, false);
};

// 設定JSON取得
mst.get_setting_json = function() {
    // 設定JSON取得
    ajax("/get_setting_json", "text", function(stat, res) {
        if (!stat) {
            set_html("info_box", "キー設定 読み込み 失敗: /get_setting_json");
            mst.key_pattern = mst.key_pattern_list[ mst.key_pattern_default ];
            set_html("top_title", mst.key_pattern.name);
            mst.view_layer();
            return;
        }
        mst.setting_data = JSON.parse(res);
        set_html("info_box", "キー設定 読み込み 完了 ");
        // 必須項目が無い場合はここで追加
        if (!mst.setting_data.option_set) mst.setting_data.option_set = {"type": ""};
        // キーボードの設定を選ぶ
        if (mst.key_pattern_list[mst.setting_data.keyboard_type]) {
            mst.key_pattern = mst.key_pattern_list[mst.setting_data.keyboard_type];
        } else {
            mst.key_pattern = mst.key_pattern_list[ mst.key_pattern_default ];
        }
        // ユニットが設定されていればユニットの設定を反映
        if (mst.setting_data.option_set.type && mst.option_list[mst.setting_data.option_set.type]) {
            var optset = mst.option_list[mst.setting_data.option_set.type];
            var check_list = ["pin", "size", "keys", "parts"];
            var i;
            for (i in check_list) {
                if (optset[check_list[i]]) mst.key_pattern[check_list[i]] = optset[check_list[i]];
            }
        }
        // ページタイトルをキータイプにする
        set_html("top_title", mst.key_pattern.name);
        // デフォルトレイヤーを編集中にする
        mst.edit_layer = mst.setting_data.default_layer;
        // レイヤー表示
        mst.view_layer();
    });
};


// 設定JSON保存
mst.save_setting_json = function() {
    var key_pattern = mst.key_pattern_list[ mst.setting_data.keyboard_type ]
    mst.setting_data.keyboard_pin = key_pattern.pin;
    mst.setting_data.status_pin = key_pattern.status_pin;
    mst.setting_data.rgb_pin = key_pattern.rgb_pin;
    mst.setting_data.rgb_len = key_pattern.rgb_len;
    mst.setting_data.led_num = key_pattern.led_num;
    mst.setting_data.key_matrix = key_pattern.key_matrix;
    // ユニットが設定されていればユニットのpinを入れる
    if (mst.setting_data.option_set.type && mst.option_list[mst.setting_data.option_set.type] && mst.option_list[mst.setting_data.option_set.type].pin) {
        mst.setting_data.keyboard_pin = mst.option_list[mst.setting_data.option_set.type].pin;
    }
    // 使っていない暗記ファイル削除
    mst.delete_ankey_file(function() {
        // JSON保存
        api_path = "/upload_setting_json";
        if (mst.end_type == 2) api_path = "/upload_setting_json_reload";
        ajax_post(api_path, JSON.stringify(mst.setting_data), function(stat, res) {
            if (!stat) {
                set_html("info_box", "ページを閉じて下さい");
                mst.view_box(["info_box"]);
                return;
            }
            set_html("info_box", "終了中..");
        });
    });
};


// blobデータをキーボードに保存する
mst.file_send = function(url_path, file_name, blob_data, cb_func) {
    // フォームの情報を送信
    var fd = new FormData();
    fd.append("file", blob_data, file_name);
    var xhr = new XMLHttpRequest();
    xhr.open("POST", url_path, true);
    xhr.upload.addEventListener('progress', function(evt) {
        if (!evt.lengthComputable) return;
        console.log("progress: " + evt.loaded + " / " + evt.total);
    });
    var err_func = function() {
        cb_func(false, null);
    };
    xhr.addEventListener("error", err_func);
    xhr.upload.addEventListener("abort", err_func);
    xhr.upload.addEventListener("error", err_func);
    xhr.upload.addEventListener("timeout", err_func);
    xhr.onreadystatechange = function(e) {
        if (this.readyState == 4) {
            if (this.status == 200) {
                cb_func(true, this);
            } else {
                cb_func(false, null);
            }
        }
    };
    xhr.send(fd);
};

// 指定したファイルを削除する
mst.file_delete = function(file_name, cb_func) {
    ajax("delete_file_" + file_name, "text", cb_func);
};

mst.file_delete_list = function(file_list, cb_func) {
    if (file_list) {
        mst.delete_file_list = file_list;
    }
    console.log(mst.delete_file_list.length);
    if (!mst.delete_file_list.length) {
        cb_func();
        return;
    }
    var file_name = mst.delete_file_list.pop();
    ajax("delete_file_" + file_name, "text", function() {
        mst.file_delete_list(null, cb_func);
    });

};


// スマホ判定
mst.is_mobile = function() {
    return false;
    var ut = navigator.userAgent;
    if (ut.indexOf('iPhone') > 0 || ut.indexOf('iPod') > 0 || ut.indexOf('Android') > 0 && ut.indexOf('Mobile') > 0) {
        return true;
    } else if(ut.indexOf('iPad') > 0 || ut.indexOf('Android') > 0) {
        return true;
    }
    return false;
};

// 画面を90度回転
mst.body_rotate = function() {
    var bobj = $("body_box");
    var sh = bobj.offsetHeight;
    var sw = bobj.offsetWidth;
    var st = "rotate(90deg)";
    bobj.style.transformOrigin = "0 0 0";
    bobj.style.transform = st;
    bobj.style.MozTransform = st;
    bobj.style.msTransform = st;
    bobj.style.OTransform = st;
    bobj.style.top = 0;
    bobj.style.left = window.innerWidth + "px";
    bobj.style.width = (window.innerHeight - 50) + "px";
};

// 指定した枠を表示
mst.view_box = function(view_list) {
    var box_list = ["info_box", "select_box", "setting_box", "layer_box", "layer_menu", "key_img_box", "top_menu_box", "menu_box"];
    var i;
    for (i in box_list) {
        if (view_list.indexOf(box_list[i]) >= 0) {
            show(box_list[i]);
        } else {
            hide(box_list[i]);
        }
    }
    window.scrollTo(0, 0);
};

// キーコードから表示用の文字列を取得
mst.get_key_char = function(key_code, key_name) {
    var i;
    if (!key_name) key_name = "char";
    for (i in mst.key_list) {
        if (parseInt(mst.key_list[i].key) == key_code) {
            return mst.key_list[i][key_name];
        }
    }
    return "";
};

// レイヤー枠表示
mst.view_layer = function() {
    var s = "";
    s += "" +  mst.get_layer_name(mst.edit_layer) + "　";
    s += "<div style='display: inline-block;'>";
    s += "<div id='layer_menu'>";
    s += "<a href='#' class='update_button' onClick='javascript:mst.select_layer(); return false;'>▼</a>　";
    if (mst.edit_layer > 0) {
        s += "<a href='#' class='update_button' onClick='javascript:mst.delete_layer(); return false;'>×</a>　";
    }
    s += "<a href='#' class='update_button' onClick='javascript:mst.add_layer(); return false;'>＋</a>　";
    s += "</div>";
    s += "</div>";
    set_html("layer_box", s);
    mst.create_key_image(); // キーボード図形作成
    mst.create_key_btn(); // ボタン作成
    mst.view_box(["layer_box", "layer_menu", "key_img_box", "menu_box", "top_menu_box", "info_box"]);
};

// 編集中レイヤー選択
mst.select_layer = function() {
    var l = mst.get_layer_list();
    // 選択画面表示
    mst.select_exec(l, mst.edit_layer+"", function(select_key) {
        mst.edit_layer = parseInt(select_key);
        mst.view_layer();
    });
};

// レイヤー追加
mst.add_layer = function() {
    var k, n, m = 0;
    for (k in mst.setting_data.layers) {
        n = parseInt(k.substring(6));
        if (n > m) m = n;
    }
    m++;
    mst.setting_data.layers["layer_" + m] = {
        "name": "レイヤー" + m,
        "keys": {}
    };
    mst.edit_layer = m;
    mst.view_layer();
};

// レイヤー削除(選択中のレイヤーを削除)
mst.delete_layer = function() {
    if (mst.edit_layer > 0) {
        delete mst.setting_data.layers["layer_" + mst.edit_layer];
        mst.edit_layer = 0;
    }
    mst.view_layer();
};


// キータッチ用のオブジェクト作成
mst.create_key_btn = function() {
    var c, f, i, j, k, h, l, p, m, s, sc;
    // 画像の位置取得
    h = "";
    for (i in mst.key_pattern["keys"]) {
        k = mst.key_pattern.keys[i];
        p = mst.get_key_setting_num(k.id); // キーの設定
        s = "position: absolute;";
        s += "margin: -2px; border: 3px solid black; border-radius: 12px;overflow: hidden;";
        if (mst.switch_check_mode) {
            // キー動作確認中はグレー
            s += "background-color: rgba(80,80, 80, 0.8);";
        } else if (p.press.action_type == 0) {
            // 処理なし
            s += "background-color: rgb(255, 73, 73);";
        } else {
            // 処理あり
            s += "background-color: rgb(129, 255, 129);";
        }
        s += "top: " + k["y"] + "px; left: " + k["x"] + "px;";
        s += "width: " + k["width"] + "px; height: " + k["height"] + "px;";
        h += "<div id='key_" + i + "' style='" + s + "' onClick='javascript: mst.key_click("+i+");'>";
        sc = (k["width"] < 100)? 0.7: 1;
        h += "<table style='width: " + (k["width"]+20) + "px; height: " + (k["height"]+20) + "px;scale:"+sc+";margin: -10px;'><tr><td align='center'>";
        if (mst.switch_check_mode) {
            // キー動作確認中は番号を表示
            h += "<div id='key_top_"+i+"' style='color: white; font-size: 50px;'>"+k.id+"</div>";
        } else if (p.press.action_type == 0) {
            // 処理なし
            h += "<div id='key_top_"+i+"' style='color: black; font-size: 27px;line-height: 27px;'>None</div>";
        } else if (p.press.action_type == 1) {
            // 通常入力
            if (p.press.key.length <= 1) {
                c = mst.get_key_char(p.press.key[0]);
                f = (c.length > 3)? "30px": "43px";
                h += "<div id='key_top_"+i+"' style='color: black; font-size: "+f+";'>" + c + "</div>";
            } else {
                h += "<div id='key_top_"+i+"' style='color: black; font-size: 27px;line-height: 27px;'>";
                l = [];
                for (j in p.press.key) {
                    l.push(mst.get_key_char(p.press.key[j]));
                    console.log(j + " : " +p.press.key[j] + " : " + mst.get_key_char(p.press.key[j]));
                }
                console.log(l);
                h += l.join(" + ");
                h += "</div>";
            }
        } else if (p.press.action_type == 2) {
            // テキスト入力
            h += "<div id='key_top_"+i+"' style='color: black; font-size: 27px;'>text</div>";
        } else if (p.press.action_type == 3) {
            // レイヤー変更
            h += "<div id='key_top_"+i+"' style='color: black; font-size: 27px;'>" + mst.get_layer_name(p.press.layer) + "</div>";
        } else if (p.press.action_type == 4) {
            // WEBフック
            h += "<div id='key_top_"+i+"' style='color: black; font-size: 27px;'>WEB</div>";
        } else if (p.press.action_type == 5) {
            // マウス移動
            h += "<div id='key_top_"+i+"' style='color: black; font-size: 27px;'>MUS</div>";
        } else if (p.press.action_type == 6) {
            // 暗記ボタン
            h += "<div id='key_top_"+i+"' style='color: black; font-size: 27px;'>暗</div>";
        } else if (p.press.action_type == 7) {
            // LED設定ボタン
            if (p.press.led_setting_type == 0) c = "ONOF";
            if (p.press.led_setting_type == 1) c = "UP";
            if (p.press.led_setting_type == 2) c = "DOWN";
            if (p.press.led_setting_type == 3) c = "CRCH";
            if (p.press.led_setting_type == 4) c = "TYCH";
            h += "<div id='key_top_"+i+"' style='color: black; font-size: 23px;'>LED<br>"+c+"</div>";
        } else if (p.press.action_type == 8) {
            // 打鍵設定ボタン
            if (p.press.dakagi_settype == 0) c = "HM";
            if (p.press.dakagi_settype == 1) c = "QR";
            if (p.press.dakagi_settype == 2) c = "ATS";
            if (p.press.dakagi_settype == 3) c = "SAVE";
            h += "<div id='key_top_"+i+"' style='color: black; font-size: 23px;'>鍵<br>"+c+"</div>";
            
        }
        h += "</td></tr></table>";
        h += "</div>";
    }
    
    set_html("key_objs_box", h);
    
};

// キーボードの図形作成
mst.create_key_image = function() {
    var i, j, p;
    $("key_box").style.width = mst.key_pattern.size.width + "px";
    $("key_box").style.height = mst.key_pattern.size.height + "px";
    $("key_img_box").style.width = mst.key_pattern.size.width + "px";
    $("key_img_box").style.height = mst.key_pattern.size.height + "px";
    $("key_img").width = mst.key_pattern.size.width;
    $("key_img").height = mst.key_pattern.size.height;
    var cv = $("key_img").getContext("2d");
    // 描画するパーツ分ループ
    for (i in mst.key_pattern.parts) {
        p = mst.key_pattern.parts[i];
        if (p.type == "polygon") {
            // 表示タイプポリゴン
            if (p.line_width) cv.lineWidth = p.line_width; // 線の太さ
            if (p.line_color) cv.strokeStyle  = p.line_color; // 線の色
            if (p.fill_color) cv.fillStyle = p.fill_color; // 塗りつぶしの色
            cv.beginPath();
            for (j in p.path) {
                console.log(p.path[j]);
                if (j == 0) {
                    cv.moveTo(p.path[j][0], p.path[j][1]);
                } else {
                    cv.lineTo(p.path[j][0], p.path[j][1]);
                }
            }
            cv.closePath();
            if (p.fill) {
                cv.fill(); // 塗りつぶし
            } else {
                cv.stroke(); // 線だけ
            }
        }
    }
};

// 画像上のキーをクリックした時のイベント
mst.key_click = function(key_num) {
    // キー動作確認中は何もしない
    if (mst.switch_check_mode) return;
    // キーの設定画面表示
    var kid = mst.key_pattern.keys[key_num].id;
    mst.view_key_setting(kid);
    set_html("info_box", "");
};

// キーの設定を取得
mst.get_key_setting_num = function(key_id) {
    // 設定データが無ければデフォルトを設定
    if (!mst.setting_data.layers["layer_" + mst.edit_layer].keys["key_" + key_id]) {
        mst.setting_data.layers["layer_" + mst.edit_layer].keys["key_" + key_id] = {
            "press": {"action_type": 0}, // 処理なし
            "long_press": {},
            "long_press_time": 0
        };
    }
    // 設定データを返す
    return mst.setting_data.layers["layer_" + mst.edit_layer].keys["key_" + key_id]
};


// セレクトを選択した時の処理
mst.select_on_click = function(select_key) {
    mst.select_exec_cbfunc(select_key);
    set_html("select_box", "");
    hide("select_box");
    mst.select_exec_cbfunc = null;
};

// 選択枠表示
mst.select_exec = function(list_data, select_key, cb_func) {
    var i, o, sl, sli = -1, t, v;
    // コールバック関数を保持
    mst.select_exec_cbfunc = cb_func;
    var s = "";
    s += "<div id='lists' class='lists' style='width: "+(window.innerWidth - 100)+"px;'>";
    for (i in list_data) {
        o = list_data[i];
        if (o.key == select_key) {
            sl = "background-color: #34F464;";
            sli = i;
        } else {
            sl = "";
        }
        
        s += "<div id='opt_"+i+"' style='border-bottom: solid 1px #888;margin: 0; padding: 17px; font-size: 50px;display: block;"+sl+"' onClick='javascript:mst.select_on_click(\""+o.key+"\")'>"+o.value+"</div>";
    }
    s += "</div>";
    set_html("select_box", s);
    /*
    if (mst.is_mobile()) {
        $("lists").style.height = (window.innerWidth - 100) + "px";
        $("lists").style.width = (window.innerHeight - 50) + "px";
    } else {
        $("lists").style.height = (window.innerHeight - 180) + "px";
        $("lists").style.width = (window.innerWidth - 50) + "px";
    }
    */
    mst.view_box(["select_box"]);
    // スクロールの位置を選択してる項目の所へ移動
    if (sli >= 0) {
        v = $("opt_" + sli).getBoundingClientRect();
        window.scrollTo(0, v.top - 100);
    }
};



// レイヤーのリストを取得
mst.get_layer_list = function() {
    var v, n;
    var l = [];
    for (v in mst.setting_data.layers) {
        n = v.substring(6);
        l.push({"key": n, "value": mst.setting_data.layers[v].name});
    }
    return l;
};

// レイヤー番号からレイヤー名を取得
mst.get_layer_name = function(layer_no) {
    if (!mst.setting_data.layers["layer_" + layer_no]) return "";
    return mst.setting_data.layers["layer_" + layer_no].name;
};

// 入力タイプ選択(データが無い場合はここでデフォルトのデータを入れる)
mst.select_input_type = function() {
    var i, l = [];
    for (i in mst.input_type_list) {
        if (i == 6 && mst.esp_type == 0) continue; // 通常のESP32の場合暗記ボタンはスキップ
        if (i == 7 && mst.setting_data.rgb_pin < 0) continue; // RGB LEDが付いてないキーボードはスキップ
        // if (i == 8 && !mst.is_tft(mst.setting_data.option_set.type)) continue; // 打鍵設定は液晶が付いてないキーボードはスキップ
        if (i == 8 && mst.setting_data.option_set.type != "display_66jp") continue; // 66jpの液晶オプション以外はスキップ
        l.push({"key": i, "value": mst.input_type_list[i]});
    }
    mst.select_exec(l, mst.key_edit_data.press.action_type+"", function(select_key) {
        mst.key_edit_data.press.action_type = parseInt(select_key);
        if (mst.key_edit_data.press.action_type == 1) {
            // 通常入力
            if (!("key" in mst.key_edit_data.press)) {
                mst.key_edit_data.press.key = [4];
                mst.key_edit_data.press.repeat_interval = 51;
            }
        } else if (mst.key_edit_data.press.action_type == 2) {
            // テキスト入力
            if (!("text" in mst.key_edit_data.press)) mst.key_edit_data.press.text = "";
        } else if (mst.key_edit_data.press.action_type == 3) {
            // レイヤー切り替え
            if (!("layer" in mst.key_edit_data.press)) mst.key_edit_data.press.layer = 0;
        } else if (mst.key_edit_data.press.action_type == 4) {
            // WEBフック
            if (!("webhook" in mst.key_edit_data.press)) mst.key_edit_data.press.webhook = {"url": "http://palette-system.com/ct/", "header": [], "post": "", "keyoutput": 0};
        } else if (mst.key_edit_data.press.action_type == 5) {
            // マウス移動
            if (!("move" in mst.key_edit_data.press)) mst.key_edit_data.press.move = {"x": 0, "y": 0, "speed": 100};
        } else if (mst.key_edit_data.press.action_type == 6) {
            // 暗記ボタン
            if (!("ankey_file" in mst.key_edit_data.press)) {
                mst.key_edit_data.press.ankey_file = "A" + Math.random().toString(36).slice(-8);
            }
        } else if (mst.key_edit_data.press.action_type == 7) {
            // LED設定ボタン
            if (!("led_setting_type" in mst.key_edit_data.press)) {
                mst.key_edit_data.press.led_setting_type = 0;
            }
        } else if (mst.key_edit_data.press.action_type == 8) {
            // 打鍵設定ボタン
            if (!("dakagi_settype" in mst.key_edit_data.press)) {
                mst.key_edit_data.press.dakagi_settype = 0;
            }
        }
        mst.view_key_setting(mst.key_edit_kid);
    });
};


// 入力文字選択
mst.select_input_key = function(num) {
    mst.select_exec(mst.key_list, mst.key_edit_data.press.key[num]+"", function(select_key) {
        mst.key_edit_data.press.key[num] = parseInt(select_key);
        mst.view_key_setting(mst.key_edit_kid);
    });
};

// 切り替えレイヤーの選択
mst.select_change_layer = function() {
    mst.select_exec(mst.get_layer_list(), mst.key_edit_data.press.layer+"", function(select_key) {
        mst.key_edit_data.press.layer = parseInt(select_key);
        mst.view_key_setting(mst.key_edit_kid);
    });
};

// テキストに全角文字が入っていないかチェック
mst.check_ascii = function(elm){
    var txt = elm.value;
    for(i=0 ; i<txt.length ; i++){
        if(escape(txt.charAt(i)).length >= 4){
            alert("半角英数字を入力してください。");
            elm.value = "";
            return false;
        }
    }
    return true;
};

// テキスト内容変更イベント
mst.text_on_change = function() {
    var obj = $("key_text");
    mst.check_ascii(obj);
    mst.key_edit_data.press.text = obj.value;
};

// webフック用URLチェック
mst.web_hook_url_check = function(url) {
    // 入力が無い場合はOK
    if (!url.length) return true;
    // 頭7文字が http:// で始まらなければNG
    if (url.slice(0, 7) != "http://" && url.slice(0, 8) != "https://") {
        alert("http:// か https:// のみ指定できます");
        return false;
    }
    // それ以外はOK
    return true;
};

// URL内容変更イベント
mst.url_on_change = function() {
    var obj = $("url_text");
    var url = obj.value;
    // 入力チェックNGなら入力を元に戻す
    if (!mst.web_hook_url_check(url)) {
        obj.value = mst.key_edit_data.press.webhook.url;
        return;
    }
    mst.key_edit_data.press.webhook.url = url;
};

// URLがhttpsかどうか
mst.is_https_url = function(url) {
    if (url.length < 8) return false;
    if (url.slice(0, 8) == "https://") return true;
    return false;
};

// キー設定画面表示
mst.view_key_setting = function(key_id) {
    if (mst.key_edit_kid < 0) {
        mst.key_edit_kid = key_id;
        mst.key_edit_data = obj_clone(mst.get_key_setting_num(mst.key_edit_kid));
    }
    var pss = mst.key_edit_data.press; // 押された時アクションの設定
    var at = pss.action_type;
    var f, i, k, s = "", st;
    var hrst = "border: 1px solid #9a9fe3;";
    s += "<table cellpadding='20' style='min-width: 520px;'>";
    s += "<tr><td colspan='2'><b>キー番号：</b> <font style='font-size: 40px;'>" + mst.key_edit_kid + "</font></td></tr>";
    s += "<tr><td colspan='2' style='padding: 20px 0;'><hr style='"+hrst+"'></td></tr>";
    s += "<tr><td><b>動作：</b><br><font style='font-size: 40px;'>" + mst.input_type_list[at] + "</font></td><td align='right'>";
    s += "<a href='#' class='update_button' onClick='javascript:mst.select_input_type(); return false;'>変更</a>";
    s += "</td></tr>";
    if (at == 1) {
        // 通常入力
        for (i in pss.key) {
            k = mst.get_key_char(pss.key[i], "value");
            if (k.length < 12) { f = 40; } else { f = 30; }
            s += "<tr><td colspan='2' style='padding: 20px 0;'><hr style='"+hrst+"'></td></tr>";
            s += "<tr><td>";
            s += "<b>キー "+(parseInt(i)+1)+"：</b> <font style='font-size: "+f+"px;'>" + k + "</font>　";
            s += "</td><td align='right'>";
            s += "<a href='#' class='update_button' onClick='javascript:mst.select_input_key("+i+"); return false;'>変更</a>";
            if (pss.key.length > 1) {
                s += " <a href='#' class='update_button' onClick='javascript:mst.delete_input_key("+i+"); return false;'>×</a>";
            }
            s += "</td></tr>";
        }
        if (pss.key.length < 5) {
            s += "<tr><td colspan='2' style='padding: 20px 0;'><hr style='"+hrst+"'></td></tr>";
            s += "<tr><td colspan='2' align='right'>";
            s += "<br><a href='#' class='update_button' onClick='javascript:mst.add_input_key(); return false;'>入力キー追加</a>";
            s += "</td></tr>";
        }
        if (pss.repeat_interval === undefined) pss.repeat_interval = 51;
        console.log(pss);
        s += "<tr><td colspan='2' style='padding: 20px 0;'><hr style='"+hrst+"'></td></tr>";
        s += "<tr><td colspan='2'>";
        s += "<b>連打間隔：</b>　<font id='move_repeat_interval_val'></font><br><br>";
        s += "<center><input type='range' id='move_repeat_interval' name='move_repeat_interval' min='0' max='51' style='width: 420px;' value='"+pss.repeat_interval+"' onChange='javascript:mst.view_move_input(\"repeat_interval\");'></center>";
        s += "</td></tr>";
        
    } else if (at == 2) {
        // テキスト入力
        s += "<tr><td colspan='2' style='padding: 20px 0;'><hr style='"+hrst+"'></td></tr>";
        st = "font-size: 30px; width: 520px; border: 3px solid black;";
        s += "<tr><td colspan='2'>";
        s += "<b>テキスト：</b><br><input type='text' id='key_text' style='"+st+"' value='' onChange='javascript:mst.text_on_change();'>";
        s += "</td></tr>";
    } else if (at == 3) {
        // レイヤー切り替え
        s += "<tr><td colspan='2' style='padding: 20px 0;'><hr style='"+hrst+"'></td></tr>";
        s += "<tr><td>";
        s += "<b>レイヤー：</b><br><font style='font-size: 40px;'>"+mst.get_layer_name(pss.layer)+"</font></td><td align='right'>";
        s += "<a href='#' class='update_button' onClick='javascript:mst.select_change_layer(); return false;'>変更</a>";
        s += "</td></tr>";
        s += "<tr><td colspan='2'>";
        s += "<br><br><font style='font-size: 30px;'>※ このキーを押している間、指定したレイヤーの動作に切り替わります。</font><br><br>";
        s += "</td></tr>";
    } else if (at == 4) {
        // WEBフック
        s += "<tr><td colspan='2' style='padding: 20px 0;'><hr style='"+hrst+"'></td></tr>";
        st = "font-size: 30px; width: 520px; border: 3px solid black;";
        s += "<tr><td colspan='2'>";
        s += "<b>URL ：</b><br><input type='text' id='url_text' style='"+st+"' value='' onChange='javascript:mst.url_on_change();'>";
        s += "<br><br>";
        s += "<b>HTTPヘッダー ：</b><br>";
        for (i in pss.webhook.header) {
            s += "<p style='font-size: 24px; margin: 0; padding: 0; overflow: hidden; width: 520px;'>　" + pss.webhook.header[i].key + " : " + pss.webhook.header[i].value + "</p><br>";
            s += "<p style='text-align: right; margin: 0; padding: 6px;'><a href='#' class='update_button' onClick='javascript:mst.view_webhook_header_edit("+i+");return false;'>編集</a>　";
            s += "<a href='#' class='update_button' onClick='javascript:mst.delete_webfook_header("+i+");return false;'>削除</a></p>";
        }
        s += "<p style='text-align: right; margin: 0; padding: 30px 6px;'><a href='#' class='update_button' onClick='javascript:mst.add_webfook_header();return false;'>HTTPヘッダー追加</a></p>";
        s += "<b>POSTデータ ：</b><br>";
        s += "<textarea id='webhook_post_text' style='font-size: 30px; width: 520px; height: 200px; border: 3px solid black;'></textarea>";
        s += "<br><br>";
        s += "<b>キー動作 ：</b><br>";
        s += "<select id='webhook_keyoutput' style='font-size: 30px; border: 3px solid black;'>";
        s += "<option value='0'>無し</option>";
        s += "<option value='1'>ステータスコードを入力</option>";
        s += "<option value='2'>HTTPレスポンスを入力</option>";
        s += "</select>";
        s += "</td></tr>";
        if (!mst.setting_data.wifi.length) {
            s += "<tr><td colspan='2'>";
            s += "<font style='font-size: 30px;'>※ 有効にするにはWIFIの設定が必要です。</font><br>";
            s += "</td></tr>";
        }
        
    } else if (at == 5) {
        // マウス移動
        s += "<tr><td colspan='2' style='padding: 20px 0;'><hr style='"+hrst+"'></td></tr>";
        s += "<tr><td colspan='2'>";
        s += "<b>Ｘ：</b><font id='move_x_val'>"+pss.move.x+"</font><br><input type='range' id='move_x' name='move_x' min='-100' max='100' style='width: 400px;' value='"+pss.move.x+"' onChange='javascript:mst.view_move_input(\"x\");'><br><br>"
        s += "<b>Ｙ：</b><font id='move_y_val'>"+pss.move.y+"</font><br><input type='range' id='move_y' name='move_y' min='-100' max='100' style='width: 400px;' value='"+pss.move.y+"' onChange='javascript:mst.view_move_input(\"y\");'><br><br>"
        s += "<b>スピード：</b><font id='move_speed_val'>"+pss.move.speed+"</font><br><input type='range' id='move_speed' name='move_speed' min='0' max='100' style='width: 400px;' value='"+pss.move.speed+"' onChange='javascript:mst.view_move_input(\"speed\");'><br><br>"
        s += "</td></tr>";

    } else if (at == 6) {
        // 暗記ボタン

    } else if (at == 7) {
        // LED設定ボタン
        s += "<tr><td colspan='2'>";
        s += "<b>設定の種類 ：</b><br>";
        s += "<select id='led_setting_type' style='font-size: 30px; border: 3px solid black; width: 350px;'>";
        s += "<option value='0'>ON / OFF</option>";
        s += "<option value='1'>明るさUP</option>";
        s += "<option value='2'>明るさDOWN</option>";
        s += "<option value='3'>色変更</option>";
        s += "<option value='4'>光らせ方変更</option>";
        s += "</select>";
        s += "</td></tr>";

    } else if (at == 8) {
        // 打鍵設定ボタン
        s += "<tr><td colspan='2'>";
        s += "<b>設定の種類 ：</b><br>";
        s += "<select id='dakagi_settype' style='font-size: 30px; border: 3px solid black; width: 350px;'>";
        s += "<option value='0'>ヒートマップ表示</option>";
        s += "<option value='1'>QRコード</option>";
        s += "<option value='2'>自動保存設定</option>";
        s += "<option value='3'>打鍵数保存</option>";
        s += "</select>";
        s += "</td></tr>";

    }
    s += "</table>";
    s += "<br><br>";
    s += "<center>";
    s += "<a href='#' class='button' onClick='javascript:mst.key_setting_btn_click(1);return false;'>決定</a>　　";
    s += "<a href='#' class='button' onClick='javascript:mst.key_setting_btn_click(0);return false;'>キャンセル</a>";
    s += "</center>";
    s += "<br><br>";
    set_html("setting_box", s);
    // テキストは後からvalueに値を入れる
    if (at == 1) {
        mst.view_move_input("repeat_interval");
    } else if (at == 2) {
        $("key_text").value = pss.text;
    } else if (at == 4) {
        $("url_text").value = pss.webhook.url;
        $("webhook_post_text").value = pss.webhook.post;
        $("webhook_keyoutput").value = pss.webhook.keyoutput + "";
    } else if (at == 7) {
        $("led_setting_type").value = pss.led_setting_type + "";
    } else if (at == 8) {
        $("dakagi_settype").value = pss.dakagi_settype + "";
    }
    mst.view_box(["info_box", "setting_box", "layer_box"]);
};

// マウス移動のバー情報を画面に反映
mst.view_move_input = function(t) {
    var s = $("move_"+t).value;
    if (t == "repeat_interval") {
        if (s > 50) s = "連打無し";
    }
    set_html("move_"+t+"_val", s);
};


// WEBフックヘッダーを１つ削除
mst.delete_webfook_header = function(num) {
    var i, l = [];
    for (i in mst.key_edit_data.press.webhook.header) {
        if (i == num) continue;
        l.push(mst.key_edit_data.press.webhook.header[i]);
    }
    mst.key_edit_data.press.webhook.header = l;
    mst.view_key_setting(mst.key_edit_kid);
};


// WEBフックヘッダーを１つ追加
mst.add_webfook_header = function(num) {
    mst.key_edit_data.press.webhook.header.push({"key": "", "value": ""});
    mst.view_webhook_header_edit(mst.key_edit_data.press.webhook.header.length - 1);
};


// WEBフックヘッダーを編集するページを表示
mst.view_webhook_header_edit = function(num) {
    var s = "", st;
    var hd = mst.key_edit_data.press.webhook.header[num];
    mst.key_edit_data.press.webhook.url = $("url_text").value;
    mst.key_edit_data.press.webhook.post = $("webhook_post_text").value;
    mst.key_edit_data.press.webhook.keyoutput = parseInt($("webhook_keyoutput").value);
    st = "font-size: 30px; width: 520px; border: 3px solid black;";
    s += "<b style='font-size: 30px;'>HTTPヘッダー</b><br><br><br>";
    s += "<b>Key ：</b><br><input type='text' id='webheader_key' style='"+st+"' value=''>";
    s += "<br><br>";
    s += "<b>Value ：</b><br><input type='text' id='webheader_value' style='"+st+"' value=''>";
    s += "<br><br><br><br>";
    s += "<center>";
    s += "<a href='#' class='button' onClick='javascript:mst.webhook_header_end_btn("+num+", 1);return false;'>決定</a>　　";
    s += "<a href='#' class='button' onClick='javascript:mst.webhook_header_end_btn("+num+", 0);return false;'>キャンセル</a>";
    s += "</center>";
    s += "<br><br>";
    set_html("setting_box", s);
    mst.view_box(["info_box", "setting_box", "layer_box"]);
    $("webheader_key").value = hd.key;
    $("webheader_value").value = hd.value;
};

// WEBフックヘッダー決定ボタン
mst.webhook_header_end_btn = function(num, save_flag) {
    if (save_flag) {
        // 決定ボタンだった場合入力を反映
        mst.key_edit_data.press.webhook.header[num].key = $("webheader_key").value;
        mst.key_edit_data.press.webhook.header[num].value = $("webheader_value").value;
    }
    mst.view_key_setting(mst.key_edit_kid);
};

// 入力キーを１つ削除
mst.delete_input_key = function(num) {
    var i, l = [];
    for (i in mst.key_edit_data.press.key) {
        if (i == num) continue;
        l.push(mst.key_edit_data.press.key[i]);
    }
    mst.key_edit_data.press.key = l;
    mst.view_key_setting(mst.key_edit_kid);
};

// 入力キーを１つ追加
mst.add_input_key = function(num) {
    mst.key_edit_data.press.key.push(4);
    mst.select_input_key(mst.key_edit_data.press.key.length - 1);
};



// キー設定ボタンイベント(type_id : 0=キャンセル / 1=決定)
mst.key_setting_btn_click = function(type_id) {
    // 決定ならば設定データ更新
    var s;
    if (type_id == 1) {
        s = {"action_type": mst.key_edit_data.press.action_type};
        if (s.action_type == 1) { // 通常キー入力
            s.key =  mst.key_edit_data.press.key;
            s.repeat_interval = $("move_repeat_interval").value
        } else if (s.action_type == 2) { // テキスト入力
            s.text =  mst.key_edit_data.press.text;
        } else if (s.action_type == 3) { // レイヤー切り替え
            s.layer =  mst.key_edit_data.press.layer;
        } else if (s.action_type == 4) { // WEBフック
            mst.key_edit_data.press.webhook.url = $("url_text").value;
            mst.key_edit_data.press.webhook.post = $("webhook_post_text").value;
            mst.key_edit_data.press.webhook.keyoutput = parseInt($("webhook_keyoutput").value);
            s.webhook =  mst.key_edit_data.press.webhook;
        } else if (s.action_type == 5) { // マウス移動
            s.move = {"x": $("move_x").value, "y": $("move_y").value, "speed": $("move_speed").value};
        } else if (s.action_type == 6) { // 暗記ボタン
            s.ankey_file = mst.key_edit_data.press.ankey_file;
        } else if (s.action_type == 7) { // LED設定ボタン
            s.led_setting_type = parseInt($("led_setting_type").value);
        } else if (s.action_type == 8) { // 打鍵設定ボタン
            s.dakagi_settype = parseInt($("dakagi_settype").value);
        }
        mst.setting_data.layers["layer_" + mst.edit_layer].keys["key_" + mst.key_edit_kid].press = s;
    }
    mst.key_edit_kid = -1;
    mst.key_edit_data = null;
    mst.create_key_btn(); // キーのボタンオブジェクト作り直し
    set_html("info_box", "");
    mst.view_box(["info_box", "layer_box", "layer_menu", "key_img_box", "top_menu_box", "menu_box"]);
};

// 暗記ボタンとして設定されているかチェック
mst.check_ankey_file = function(file_name) {
    var i, j, p;
    for (i in mst.setting_data.layers) {
        layer_data = mst.setting_data.layers[i];
        for (j in layer_data["keys"]) {
            key_data = layer_data["keys"][j];
            p = key_data["press"];
            if (p["action_type"] != 6) continue; // 暗記ボタンで無ければ無視
            if (!p["ankey_file"]) continue; // 暗記ファイルの指定が無ければ無視
            if (p["ankey_file"] == file_name) return true; // 設定されている暗記ファイルだった
        }
    }
    return false;
}


// 使用していない暗記ボタン用ファイルを削除
mst.delete_ankey_file = function(cb_func) {
    // ファイルリスト取得
    ajax("/file_list", "text", function(stat, res) {
        if (!stat || !res) {
            cb_func(false);
            return;
        }
        var i;
        var file_list = JSON.parse(res);
        var delete_file_list = [];
        for (i in file_list["data"]) {
            file_path = file_list["data"][i];
            if (file_path.substr(0, 2) != "/A") continue; // 暗記ファイル以外は無視
            file_name = file_path.substr(1);
            // 使用していない暗記ファイルであれば削除
            if (!mst.check_ankey_file(file_name)) {
                // 削除リストに追加
                delete_file_list.push(file_name);
            }
            
        }
        mst.file_delete_list(delete_file_list, cb_func);
    });
};


// 終了ボタン end_type : 0 = 保存せずに終了 / 1 = 保存して終了 / 2 = 保存して設定モードで再起動
mst.end_setting = function(end_type) {
    var api_path;
    // 終了フラグを立てる
    mst.end_flag = true;
    mst.end_type = end_type;
    // ページ移動警告解除
    mst.browser_btn_control_remove();
    if (mst.end_type == 1 || mst.end_type == 2) {
        // 保存して終了
        set_html("info_box", "保存中.. ");
        mst.view_box(["info_box"]);
        mst.save_setting_json();
    } else {
        // 保存せずに終了
        set_html("info_box", "終了中.. ");
        ajax("/end_setting", "text", function(stat, res) {
            set_html("info_box", "ページを閉じて下さい");
            mst.view_box(["info_box"]);
        });
        mst.view_box(["info_box"]);
        setTimeout(function() {
            window.open('about:blank','_self').close();
        }, 1500);
    }
};


// wifi 設定画面表示(リスト表示)
mst.view_wifi_setting = function() {
    var i;
    var s = "";
    var hrst = "border: 1px solid #9a9fe3; margin: 40px 0;";
    s += "<b style='font-size: 30px;'>WIFI設定</b>";
    s += "<table style='min-width: 520px;'>";
    for (i in mst.setting_data.wifi) {
        s += "<tr><td colspan='2' style='padding: 12px 0;'><hr style='"+hrst+"'></td></tr>";
        s += "<tr><td><b style='font-size: 27px;'>" + mst.setting_data.wifi[i].ssid + "</b></td><td align='right'>";
        s += "<a href='#' class='button' onClick='javascript:mst.wifi_edit_btn("+i+");return false;'>編集</a>　";
        s += "<a href='#' class='button' onClick='javascript:mst.wifi_delete_btn("+i+");return false;'>削除</a>";
        s += "</td></tr>";
    }
    s += "<tr><td colspan='2' style='padding: 20px 0;'><hr style='"+hrst+"'></td></tr>";
    s += "<tr><td colspan='2' style='padding: 20px 0;' align='right'>";
    s += "<a href='#' class='button' onClick='javascript:mst.wifi_edit_btn(-1);return false;'>WIFI追加</a>";
    s += "</td></tr></table>";
    s += "<br><br><br>";
    s += "<center>";
    s += "<a href='#' class='button' onClick='javascript:mst.view_detail_setting();return false;'>完了</a>";
    s += "</center>";
    set_html("setting_box", s);
    set_html("info_box", "");
    mst.view_box(["info_box", "setting_box"]);
};

// wifi 編集ボタンクリック
mst.wifi_edit_btn = function(num) {
    // 追加の場合レコード追加
    if (num < 0) {
        num = mst.setting_data.wifi.length;
        mst.setting_data.wifi.push({"new_flag": true, "ssid": "", "pass": ""});
    }
    // 編集中のデータを指定されたデータにして設定画面表示
    mst.wifi_edit_kid = num;
    mst.wifi_edit_data = obj_clone(mst.setting_data.wifi[num]);
    mst.view_wifi_setting_one(mst.wifi_edit_data);
};

// wifi 削除ボタンクリック
mst.wifi_delete_btn = function(num) {
    mst.setting_data.wifi.splice(num, 1);
    mst.view_wifi_setting();
};

// wifi 設定画面表示(１つ分)
mst.view_wifi_setting_one = function(wifi_data) {
    var s = "";
    s += "<b style='font-size: 30px;'>WIFI設定</b><br><br><br>";
    s += "<b>ssid ： </b><br>";
    s += "<input type='text' id='wifi_ssid_text' style='font-size: 35px; border: 3px solid black; width: 380px;' value=''> ";
    s += "<a href='#' class='update_button' onClick='javascript:mst.wifi_search(); return false;'>▼</a>";
    s += "<br><br><br>";
    s += "<b>pass ： </b><br>";
    s += "<input type='password' id='wifi_pass_text' style='font-size: 35px; border: 3px solid black; width: 380px;' value=''>";
    // s += "<br><br><br>";
    // s += "<a href='#' class='update_button' onClick='javascript:mst.wifi_conn_check(); return false;'>接続確認</a> ";
    // s += "<font id='wifi_conn_check_info_box'></font>";
    s += "<br><br><br><br><br>";
    s += "<center><div id='wifi_setting_btn_box'>";
    s += "<a href='#' class='button' onClick='javascript:mst.wifi_setting_btn_click(1);return false;'>決定</a>　　";
    s += "<a href='#' class='button' onClick='javascript:mst.wifi_setting_btn_click(0);return false;'>キャンセル</a>";
    s += "</div></center>";
    set_html("setting_box", s);
    $("wifi_ssid_text").value = wifi_data.ssid;
    $("wifi_pass_text").value = wifi_data.pass;
    set_html("info_box", "");
    mst.view_box(["info_box", "setting_box"]);
};

// wifi 検索
mst.wifi_search = function() {
    mst.view_box(["info_box"]);
    set_html("info_box", "アクセスポイント 検索中 ...");
    ajax("/get_ap_list", "text", function(stat, res) {
        if (!stat) {
            mst.view_wifi_setting_one(mst.wifi_edit_data);
            set_html("info_box", "アクセスポイント検索 失敗");
            return;
        }
        var rdata = JSON.parse(res);
        if (!rdata["list"], !rdata.list.length) {
            mst.view_wifi_setting_one(mst.wifi_edit_data);
            set_html("info_box", "アクセスポイント検索 失敗");
            return;
        }
        set_html("info_box", "");
        var o, l = [];
        for (i in rdata.list) {
            o = (rdata.list[i].auth_open)? "(open)": "";
            l.push({"key": rdata.list[i].ssid, "value": rdata.list[i].ssid + " " + o});
        }
        mst.select_exec(l, mst.wifi_edit_data.ssid, function(select_key) {
            mst.wifi_edit_data.ssid = select_key;
            mst.view_wifi_setting_one(mst.wifi_edit_data);
        });
    });
};

// wifi接続確認
mst.wifi_conn_check = function() {
    var ssid = $("wifi_ssid_text").value;
    var pass = $("wifi_pass_text").value;
    if (!ssid.length) {
        set_html("wifi_conn_check_info_box", "ssidを指定してください");
        return;
    }
    set_html("wifi_conn_check_info_box", "接続中...");
    hide("wifi_setting_btn_box");
    ajax_post("/wifi_conn_check", ssid + "\n" + pass, function(stat, res) {
        if (!stat) {
            set_html("wifi_conn_check_info_box", "<font style='color: #FF9999;'><b>エラー</b></font>");
            show("wifi_setting_btn_box");
            return;
        }
        if (res == "OK") {
            set_html("wifi_conn_check_info_box", "<font style='color: #00FF99;'><b>OK</b></font>");
        } else {
            set_html("wifi_conn_check_info_box", "<font style='color: #FF9999;'><b>NG</b></font>");
        }
        show("wifi_setting_btn_box");
    });
};

// wifi設定完了
mst.wifi_setting_btn_click = function(save_flag) {
    var i;
    if (save_flag) {
        // 決定が押された時
        i = mst.wifi_edit_kid;
        mst.setting_data.wifi[i].ssid = $("wifi_ssid_text").value;
        mst.setting_data.wifi[i].pass = $("wifi_pass_text").value;
        if ("new_flag" in mst.setting_data.wifi[i]) delete mst.setting_data.wifi[i].new_flag;
    } else if (mst.wifi_edit_data.new_flag) {
        // キャンセルを押されたけど新しく追加したデータ場合追加したデータを削除
        mst.setting_data.wifi.splice(mst.wifi_edit_kid, 1);
    }
    mst.wifi_edit_kid = -1;
    mst.wifi_edit_data = null;
    mst.view_wifi_setting();
};

// キーボードの言語設定ページ表示
mst.view_language_setting = function(select_language) {
    var s = "";
    s += "<b style='font-size: 30px;'>日本語/US 切り替え</b><br><br><br><br>";
    if (!select_language) select_language = mst.setting_data.keyboard_language;
    s += "　　<b style='font-size: 32px;'>" + mst.key_language_list[select_language] + "</b>";
    s += "<div style='text-align: right;padding: 0 50px;'><a href='#' class='update_button' onClick='javascript:mst.keylanguage_select(\""+select_language+"\"); return false;'>変更</a></div>";
    s += "<br><br><br><br><br><br>";
    s += "<center><div id='wifi_setting_btn_box'>";
    s += "<a href='#' class='button' onClick='javascript:mst.keylanguage_setting_btn_click(1, \""+select_language+"\");return false;'>決定</a>　　";
    s += "<a href='#' class='button' onClick='javascript:mst.keylanguage_setting_btn_click(0, \""+select_language+"\");return false;'>キャンセル</a>";
    s += "</div></center>";
    set_html("setting_box", s);
    set_html("info_box", "");
    mst.view_box(["info_box", "setting_box"]);
};

// キーボード言語選択
mst.keylanguage_select = function(select_language) {
    var i;
    var l = [];
    for (i in mst.key_language_list) {
        l.push({"key": i, "value": mst.key_language_list[i]});
    }
    mst.select_exec(l, select_language, function(select_key) {
        mst.view_language_setting(select_key);
    });
};

// キーボード言語決定
mst.keylanguage_setting_btn_click = function(save_flag, select_language) {
    // キャンセルなら何もしない | 元の設定から変更が無ければ何もしない
    if (!save_flag || mst.setting_data.keyboard_language == select_language) {
        mst.view_detail_setting();
        return;
    }
    // 設定が変更されていて決定が押された時
    mst.setting_data.keyboard_language = select_language; // キーボード言語変更
    mst.view_detail_setting(); // 設定メニューに戻る
};

// キーボードの種類設定ページ表示
mst.view_keytype_setting = function(select_type) {
    var s = "";
    s += "<b style='font-size: 30px;'>キーボードの種類設定</b><br><br><br><br>";
    if (!select_type) select_type = mst.setting_data.keyboard_type;
    s += "　　<b style='font-size: 32px;'>" + mst.key_pattern_list[select_type].name + "</b>";
    // s += "<div style='text-align: right;padding: 10px 50px;'><a href='#' class='update_button' onClick='javascript:mst.keytype_select(\""+select_type+"\"); return false;'>変更</a></div>";
    s += "<br><br><br><br><br><br>";
    // タイプが変更されていたら再起動されるメッセージを出す
    if (mst.setting_data.keyboard_type != select_type) {
        s += "<div style='margin: 0 40px;'>※ キーボードがが変更されたので「決定」を押すと設定を保存して再起動します。</div>";
        s += "<br><br><br>";
    }
    s += "<center><div id='wifi_setting_btn_box'>";
    s += "<a href='#' class='button' onClick='javascript:mst.keytype_setting_btn_click(1, \""+select_type+"\");return false;'>決定</a>　　";
    s += "<a href='#' class='button' onClick='javascript:mst.keytype_setting_btn_click(0, \""+select_type+"\");return false;'>キャンセル</a>";
    s += "</div></center>";
    set_html("setting_box", s);
    set_html("info_box", "");
    mst.view_box(["info_box", "setting_box"]);
};

// キーボードタイプ選択
mst.keytype_select = function(select_type) {
    var i;
    var l = [];
    for (i in mst.key_pattern_list) {
        l.push({"key": i, "value": mst.key_pattern_list[i].name});
    }
    mst.select_exec(l, select_type, function(select_key) {
        mst.view_keytype_setting(select_key);
    });
};

// キーボードタイプ決定
mst.keytype_setting_btn_click = function(save_flag, select_type) {
    // キャンセルなら何もしない | 元の設定から変更が無ければ何もしない
    if (!save_flag || mst.setting_data.keyboard_type == select_type) {
        mst.view_detail_setting();
        return;
    }
    // 設定が変更されていて決定が押された時
    mst.setting_data.keyboard_type = select_type; // キーボードタイプ変更
    mst.setting_data.option_set = {"type": ""}; // ユニットもリセット
    // 設定ファイルを保存して設定モードで再起動
    mst.end_setting(2);
};


mst.view_option_setting = function(option_set) {
    var s = "";
    var hrst = "border: 1px solid #9a9fe3; margin: 40px 0;";
    s += "<b style='font-size: 30px;'>ユニット設定</b><br><br><br><br>";
    if (!option_set) mst.option_edit_data = obj_clone(mst.setting_data.option_set);
    if (mst.option_edit_data.type) {
        s += "<b>ユニット ：</b>　　<b style='font-size: 32px;'>" + mst.option_list[mst.option_edit_data.type].name + "</b>";
    } else {
        s += "<b>ユニット ：</b>　　<b style='font-size: 32px;'>なし</b>";
    }
    s += "<div style='text-align: right;padding: 10px 50px;'><a href='#' id='unich_btn' class='update_button' onClick='javascript:mst.option_type_select(); return false;'>変更</a></div>";
    s += "<br>";
    s += "<table style='min-width: 520px;'>";
    if (mst.option_edit_data.type == "foot_m") {
        // 踏みキー用ユニット設定
        var invstr = "しない";
        if (mst.option_edit_data.inversion == "1") invstr = "する";
        s += "<tr><td colspan='2' style='padding: 12px 0;'><hr style='"+hrst+"'></td></tr>";
        s += "<tr><td><b>入力反転：</b>　　<b style='font-size: 27px;'>"+invstr+"</b></td><td align='right'>";
        s += "<a href='#' id='rvch_btn' class='update_button' onClick='javascript:mst.foot_inv_edit_btn();return false;'>変更</a>";
        s += "</td></tr>";
    } else if (mst.option_edit_data.type == "trackball_m") {
        // AZ-Macro用トラックボール
        s += "<tr><td colspan='2' style='padding: 12px 0;'><hr style='"+hrst+"'></td></tr>";
        s += "<tr><td colspan='2'><b>トラックボール向き：</b>　　<select id='trackball_direction' style='font-size: 30px; border: 3px solid black;'>";
        s += "<option value='0'>　上　</option>";
        s += "<option value='1'>　右　</option>";
        s += "<option value='2'>　下　</option>";
        s += "<option value='3'>　左　</option>";
        s += "</select>";
        s += "</td></tr>";
        s += "<tr><td colspan='2' style='padding: 40px 0;'> </td></tr>";
        s += "<tr><td colspan='2'><b>カーソル移動速度：</b>";
        s += "<center>";
        s += "<table style='width: 420px;'><tr><td align='left'>ゆっくり</td><td align='right'>早い</td></tr></table>";
        s += "<input type='range' id='trackball_speed' name='trackball_speed' min='4' max='80' style='width: 420px;' value='"+mst.option_edit_data.trackball_speed+"' onChange='javascript:mst.view_move_input(\"repeat_interval\");'>";
        s += "</center>";
        s += "</td></tr>";
        
    } else if (mst.is_tft(mst.option_edit_data.type)) {
        // AZ-Macro,AZ-66JP用液晶
        var op_movie = "再生しない";
        if (mst.option_edit_data.op_movie == "1") op_movie = "再生する";
        s += "<tr><td colspan='2' style='padding: 12px 0;'><hr style='"+hrst+"'></td></tr>";
        s += "<tr><td><b>起動ムービー：</b>　　<b style='font-size: 27px;'>"+op_movie+"</b></td><td align='right'>";
        s += "<a href='#' id='stmv_btn' class='update_button' onClick='javascript:mst.op_movie_edit_btn();return false;'>変更</a>";
        s += "</td></tr>";
        // 待ち受け画像
        s += "<tr><td colspan='2' style='padding: 12px 0;'><hr style='"+hrst+"'></td></tr>";
        s += "<tr><td colspan='2'><b>待ち受け画像：</b><br>";
        s += "<input id='stimg_file' type='file' accept='image/*' onChange='javascript:mst.stimg_change(this);'><br>";
        s += "<canvas id='stimg_canvas' width='0' height='0'></canvas>";
        s += "</td></tr>";
        s += "<tr><td colspan='2' align='right'>";
        if (mst.option_edit_data.type == mst.setting_data.option_set.type) {
            s += "<a href='#' id='stimg_ch' class='update_button' onClick='javascript:mst.stimg_check();return false;'>確認</a>　";
        }
        s += "<a href='#' id='stimg_del' class='update_button' onClick='javascript:mst.stimg_delete();return false;'>削除</a>";
        s += "</td></tr>";
        
    }
    s += "</table>";
    s += "<br><br><br><br><br><br>";
    // タイプが変更されていたら再起動されるメッセージを出す
    if (mst.option_edit_data.type != mst.setting_data.option_set.type) {
        s += "<div style='margin: 0 40px;'>※ ユニットが変更されたので「決定」を押すと設定を保存して再起動します。</div>";
        s += "<br><br><br>";
    }
    s += "<div id='option_setting_info'></div>";
    s += "<center><div id='wifi_setting_btn_box'>";
    s += "<a href='#' id='uni_ok' class='button' onClick='javascript:mst.option_setting_btn_click(1);return false;'>決定</a>　　";
    s += "<a href='#' id='uni_can' class='button' onClick='javascript:mst.option_setting_btn_click(0);return false;'>キャンセル</a>";
    s += "</div></center>";
    set_html("setting_box", s);
    set_html("info_box", "");
    mst.view_box(["info_box", "setting_box"]);
    if (mst.option_edit_data.type == "trackball_m") {
        // トラックボールユニット
        $("trackball_direction").value = mst.option_edit_data.trackball_direction + "";
    } else if (mst.is_tft(mst.option_edit_data.type)) {
        // 液晶ユニット
        mst.view_imgdata("stimg_canvas", "stimg.dat");
    }
};

// 画像データを取得してきてキャンバスに表示
mst.view_imgdata = function(canvas_id, file_name) {
    ajax("read_file_" + file_name, "arraybuffer", function(stat, res) {
        if (!stat) {
            // ファイル無しなら何もしない
            return;
        }
        console.log(res);
        var img_data = new Uint8Array(res);
        console.log(img_data);
        var i = 0;
        var cl, ch, cr, cg, cb;
        var x = 0, y = 0;
        var max_size = mst.get_tft_size();
        var max_width = max_size.width;
        var max_height = max_size.height;
        // キャンバス準備
        var cvobj = $("stimg_canvas");
        cvobj.width = max_width;
        cvobj.height = max_height;
        var ctx = cvobj.getContext("2d");
        // データ分ループ
        while (i < img_data.length) {
            cl = img_data[i + 1];
            ch = img_data[i];
            cr = ch & 0xF8;
            cg = ((ch & 0x07) << 3 | (cl >> 5)) << 2;
            cb = (cl & 0x1F) << 3;
            ctx.fillStyle = "rgb("+cr+","+cg+","+cb+")" ;
            ctx.fillRect(x, y, 1, 1) ;
            x++;
            if (x >= max_width) { x = 0; y++; }
            i += 2;
        }
    });
};

// ユニットタイプ選択
mst.option_type_select = function() {
    var i;
    var l = [];
    l.push({"key": "", "value": "なし"});
    for (i in mst.option_list) {
        if (mst.option_list[i].keyboard_type.indexOf(mst.setting_data.keyboard_type) >= 0) {
            l.push({"key": i, "value": mst.option_list[i].name});
        }
    }
    select_type = "";
    if (mst.option_edit_data.type) select_type = mst.option_edit_data.type;
    mst.select_exec(l, select_type, function(select_key) {
        mst.option_edit_data.type = select_key;
        // 踏みキーで反転項目が無ければ追加
        if (select_key == "foot_m") {
            if (!("inversion" in mst.option_edit_data)) {
                mst.option_edit_data.inversion = "0";
            }
        }
        // トラックボールで向きが無ければ追加
        if (select_key == "trackball_m") {
            if (!("trackball_direction" in mst.option_edit_data)) {
                mst.option_edit_data.trackball_direction = "0"; // デフォルト上
            }
            if (!("trackball_speed" in mst.option_edit_data)) { // トラックボールカーソル移動速度
                mst.option_edit_data.trackball_speed = "20"; // デフォルト10
            }
        }
        // 液晶で起動ムービー再生が無ければ追加
        if (mst.is_tft(select_key)) {
            if (!("op_movie" in mst.option_edit_data)) {
                mst.option_edit_data.op_movie = "1";
            }
        }
        mst.view_option_setting(1);
    });
    
};

// 踏みキー入力反転選択
mst.foot_inv_edit_btn = function() {
    var l = [
        {"key": "0", "value": "しない"},
        {"key": "1", "value": "する"}
    ];
    mst.select_exec(l, mst.option_edit_data.inversion, function(select_key) {
        mst.option_edit_data.inversion = select_key;
        mst.view_option_setting(1);
    });
};

// 液晶の起動ムービー再生するかどうか
mst.op_movie_edit_btn = function() {
    var l = [
        {"key": "0", "value": "再生しない"},
        {"key": "1", "value": "再生する"}
    ];
    mst.select_exec(l, mst.option_edit_data.op_movie, function(select_key) {
        mst.option_edit_data.op_movie = select_key;
        mst.view_option_setting(1);
    });
};

// 液晶のオプションかどうか
mst.is_tft = function(type_str) {
    if (type_str == "display_m" || type_str == "display_66jp") return true;
    return false;
};

// 液晶のサイズを取得
mst.get_tft_size = function() {
    if (mst.setting_data.option_set.type == "display_m") {
        return {"width": 135, "height": 240};
    }
    if (mst.setting_data.option_set.type == "display_66jp") {
        return {"width": 240, "height": 135};
    }
    return {"width": 0, "height": 0};
};


// 待ち受け画像ファイル変更
mst.stimg_change = function(obj) {
    var set_file = obj.files[0];
    
    // 選択されたファイルが画像かどうか判定する
    // ここでは、jpeg形式とpng形式のみを画像をみなす
    if (set_file.type != "image/jpeg" && set_file.type != "image/png" && set_file.type != "image/bmp") {
      // 画像でない場合は何もせず終了する
      return;
    }
    // 画像をリサイズする
    var imgobj = new Image();
    var reader = new FileReader();
    reader.onload = function(e) {
      imgobj.onload = function() {

        var max_size = mst.get_tft_size();
        var max_width = max_size.width;
        var max_height = max_size.height;
        // 縮小後のサイズを計算する
        var width, height;
        var view_x, view_y;
        if((imgobj.width / imgobj.height) > (max_width / max_height)){
          // ヨコ長の画像は横サイズを定数にあわせる
          var ratio = imgobj.height/imgobj.width;
          width = max_width;
          height = max_width * ratio;
          view_x = 0;
          view_y = (max_height - height) / 2;
        } else {
          // タテ長の画像は縦のサイズを定数にあわせる
          var ratio = imgobj.width/imgobj.height;
          width = max_height * ratio;
          height = max_height;
          view_x = (max_width - width) / 2;
          view_y = 0;
        }

        // 縮小画像を描画するcanvasのサイズを上で算出した値に変更する
        var cvobj = $("stimg_canvas");
        cvobj.width = max_width;
        cvobj.height = max_height;
        
        
        var ctx = cvobj.getContext("2d");

        // canvasに既に描画されている画像があればそれを消す
        ctx.rect( 0, 0, max_width, max_height ) ;
        ctx.fillStyle = "#000000" ;
        ctx.fill() ;

        // canvasに縮小画像を描画する
        ctx.drawImage(imgobj,
          0, 0, imgobj.width, imgobj.height,
          view_x, view_y, width, height
        );
        
        // canvasからrgb565のデータを取得
        imgdata = ctx.getImageData(0, 0, max_width, max_height);
        console.log(imgdata.data.length);
        console.log(imgdata.data);
        var i = 0;
        var cr, cg, cb, ch, cl;
        var img_data = [];
        while (i < imgdata.data.length) {
            cr = imgdata.data[i] >> 3;
            cg = imgdata.data[i + 1] >> 2;
            cb = imgdata.data[i + 2] >> 3;
            ch = cr << 3 | cg >> 3;
            cl = (cg & 0x07) << 5 | cb;
            img_data.push(ch);
            img_data.push(cl);
            i += 4;
        }
        mst.option_edit_data.stimg_data = img_data;
        mst.option_edit_data.stimg_change = 1;
        
        // RGB565のデータでキャンバスに画像を描く(色が多少変わるため)
        i = 0;
        var x = 0, y = 0;
        while (i < img_data.length) {
            cl = img_data[i + 1];
            ch = img_data[i];
            cr = ch & 0xF8;
            cg = ((ch & 0x07) << 3 | (cl >> 5)) << 2;
            cb = (cl & 0x1F) << 3;
            ctx.fillStyle = "rgb("+cr+","+cg+","+cb+")" ;
            ctx.fillRect(x, y, 1, 1) ;
            x++;
            if (x >= max_width) { x = 0; y++; }
            i += 2;
        }

      }
      imgobj.src = e.target.result;
    }
    reader.readAsDataURL(set_file);
};

// 待ち受け画像確認(キーボードにデータを送って液晶に表示)
mst.stimg_check = function() {
    var uint8obj = new Uint8Array(mst.option_edit_data.stimg_data);
    var blobobj = new Blob([uint8obj]);
    set_html("option_setting_info", "画像を送信中です…");
    mst.file_send("/view_tft", "stimg.dat", blobobj, function(stat, res) {
        if (!stat) {
            set_html("option_setting_info", "画像の送信に失敗しました。");
        }
        set_html("option_setting_info", "");
    });
};

// 待ち受け画像削除
mst.stimg_delete = function() {
    var cvobj = $("stimg_canvas");
    cvobj.width = 0;
    cvobj.height = 0;
    mst.option_edit_data.stimg_data = [];
    mst.option_edit_data.stimg_change = 2;
};

// ユニット設定決定
mst.option_setting_btn_click = function(save_flag) {
    // キャンセルなら何もしない
    if (!save_flag) {
        mst.view_detail_setting();
        return;
    }
    var i, btn_list = ["unich_btn","rvch_btn","stmv_btn","stimg_file","uni_ok","uni_can","stimg_ch","stimg_del"];
    // 待ち受け画像が変更されていれば保存
    if ("stimg_change" in mst.option_edit_data) {
        for (i in btn_list) { hide(btn_list[i]); } // ボタン非表示
        if (mst.option_edit_data.stimg_change == 1) {
            // 画像が変更された
            var uint8obj = new Uint8Array(mst.option_edit_data.stimg_data);
            var blobobj = new Blob([uint8obj]);
            set_html("option_setting_info", "画像を保存中です…");
            mst.file_send("/file_save", "stimg.dat", blobobj, function(stat, res) {
                if (!stat) {
                    for (i in btn_list) { show(btn_list[i], "inline-block"); } // ボタン表示
                    set_html("option_setting_info", "画像を保存できませんでした");
                    return;
                }
                delete mst.option_edit_data.stimg_data;
                delete mst.option_edit_data.stimg_change;
                mst.option_setting_btn_click(1);
            });
            return;
        } else if (mst.option_edit_data.stimg_change == 2) {
            // 画像が削除された
            set_html("option_setting_info", "画像を削除中です…");
            mst.file_delete("stimg.dat", function(stat, res) {
                if (!stat) {
                    for (i in btn_list) { show(btn_list[i], "inline-block"); } // ボタン表示
                    set_html("option_setting_info", "画像を削除できませんでした");
                    return;
                }
                delete mst.option_edit_data.stimg_data;
                delete mst.option_edit_data.stimg_change;
                mst.option_setting_btn_click(1);
            });
            return;
        }
    }
    if (mst.option_edit_data.type == "trackball_m") {
        // トラックボールなら編集データに方向selectの値とカーソル移動速度を追加する
        mst.option_edit_data.trackball_direction = $("trackball_direction").value;
        mst.option_edit_data.trackball_speed = $("trackball_speed").value;
    }
    
    // タイプの変更が無ければ配列に反映するだけ
    if (mst.option_edit_data.type == mst.setting_data.option_set.type) {
        mst.setting_data.option_set = mst.option_edit_data;
        mst.view_detail_setting();
        return;
    }
    // ユニットタイプが変更されたら設定を保存して再起動
    for (i in btn_list) { hide(btn_list[i]); } // ボタン非表示
    set_html("option_setting_info", "本体を再起動中です…");
    mst.setting_data.option_set = mst.option_edit_data;
    mst.end_setting(2);
}


// 詳細設定表示
mst.view_detail_setting = function() {
    var s = "";
    var bs = "style='width: 450px;height: 70px; background: #78d3df; font-size: 35px; display: inline-block;'";
    s += "<b style='font-size: 30px;'>設定メニュー</b><br><br>";
    s += "<a href='#' class='update_button' "+bs+" onClick='javascript:mst.view_language_setting();return false;'>日本語/US 切り替え</a><br><br>";
    s += "<a href='#' class='update_button' "+bs+" onClick='javascript:mst.view_keytype_setting();return false;'>キーボードの種類</a><br><br>";
    s += "<a href='#' class='update_button' "+bs+" onClick='javascript:mst.view_option_setting();return false;'>ユニット設定</a><br><br>";
    s += "<a href='#' class='update_button' "+bs+" onClick='javascript:mst.view_switch_check();return false;'>スイッチ接触確認</a><br><br>";
    s += "<a href='#' class='update_button' "+bs+" onClick='javascript:mst.view_wifi_setting();return false;'>WIFI設定</a><br><br>";
    s += "<a href='#' class='update_button' "+bs+" onClick='javascript:mst.view_wifi_pass_setting();return false;'>ログイン設定</a><br><br>";
    s += "<a href='#' class='update_button' "+bs+" onClick='javascript:mst.view_init_setting();return false;'>設定の初期化</a><br><br>";
    s += "<a href='#' class='update_button' "+bs+" onClick='javascript:mst.firmware_setting();return false;'>ファームウェア</a><br><br>";
    s += "<br><br><br>";
    s += "<center>";
    s += "<a href='#' class='button' onClick='javascript:mst.view_layer();return false;'>完了</a>";
    s += "</center>";
    set_html("setting_box", s);
    set_html("info_box", "");
    mst.view_box(["info_box", "setting_box"]);
};

// ボタン接触確認表示
mst.view_switch_check = function() {
    // 接触確認中にする
    mst.switch_check_mode = 1;
    // 確認画面作成
    var s = "";
    s += "<center>";
    s += "<a href='#' class='button' onClick='javascript:mst.switch_check_mode = 0; mst.view_detail_setting();return false;'>完了</a>";
    s += "</center>";
    set_html("info_box", s);
    mst.create_key_btn();
    mst.view_box(["info_box", "key_img_box"]);
    
    // 接触状態取得のループを開始する
    mst.switch_check_loop();
};

// ボタン情報取得ループ
mst.switch_check_loop = function() {
    ajax("/get_key_status", "text", function(stat, res) {
        var i, k, s;
        if (stat && res.length) {
            for (i in mst.key_pattern["keys"]) {
                k = mst.key_pattern.keys[i];
                s = res.slice(parseInt(k.id), parseInt(k.id)+1); // キーの状態を取得
                if (s == "0") {
                    $("key_"+i).style.backgroundColor = "rgba(80, 80, 80, 0.8)";
                } else {
                    $("key_"+i).style.backgroundColor = "rgba(100, 100, 233, 0.8)";
                }
            }
        }
        // 接触確認中であれば再度アクセス
        if (mst.switch_check_mode) {
            setTimeout(mst.switch_check_loop, 20);
        }
    });
};

// WIFIパスワード設定画面表示
mst.view_wifi_pass_setting = function() {
    var s = "";
    s += "<b style='font-size: 30px;'>ログイン設定</b><br><br>";
    s += "<font style='font-size: 32px;'>このページへアクセスするWIFIのパスワードを設定します。</font><br><br><br>";
    s += "<b>pass ： </b><br>";
    s += "<input type='password' id='ap_pass_text' style='font-size: 35px; border: 3px solid black;' value='"+mst.setting_data.ap.pass+"'>";
    s += "<br><br><br><br>";
    s += "<font style='font-size: 32px;'>※ 8文字以上で有効になります。</font><br>";
    s += "<br><br><br>";
    s += "<center>";
    s += "<div id='firmware_back_btn'>";
    s += "<a href='#' class='button' onClick='javascript:mst.wifi_pass_setting(1);return false;'>決定</a>　　　";
    s += "<a href='#' class='button' onClick='javascript:mst.wifi_pass_setting(0);return false;'>キャンセル</a>";
    s += "</div>";
    s += "</center>";
    set_html("setting_box", s);
    set_html("info_box", "");
    mst.view_box(["info_box", "setting_box"]);
};

// パスワード設定ボタン
mst.wifi_pass_setting = function(save_flag) {
    if (save_flag) {
        mst.setting_data.ap.pass = $("ap_pass_text").value;
    }
    mst.view_detail_setting();
};

// ファームウェアのバージョンを表示用に成形
mst.firmware_view_convert = function(v) {
    if (v.length != 6) return v;
    var r = "";
    r += parseInt(v.slice(0,2));
    r += " . " + parseInt(v.slice(2,4));
    r += " . " + parseInt(v.slice(4,6));
    return r;
};

// ファームウェア設定画面表示
mst.firmware_setting = function() {
    var s = "";
    var fstyle = "font-size: 25px;border: 4px solid black;background-color: #e6f1ff;padding: 12px;width: 500px;";
    s += "<form method='POST' enctype='multipart/form-data' id='firmware_form'>";
    s += "<b style='font-size: 30px;'>ファームウェア</b><br><br>";
    s += "<b>バージョン：</b><br>　　<font style='font-size: 40px;'>" + mst.firmware_view_convert(mst.firmware_version) + "</font><br><br><br>";
    s += "<b>ファイル：</b><br>";
    s += "<input type='file' id='firmware_file' name='update' style='"+fstyle+"'><br><br>";
    s += "<div id='firmware_btn_box' style='text-align: right;'>";
    s += "<a href='#' class='update_button' onClick='javascript:mst.firmware_update();return false;'>アップデート</a><br><br>";
    s += "</div>";
    s += "<div id='firmware_info'></div>";
    s += "<br><br><br>";
    s += "<center>";
    s += "<div id='firmware_back_btn'>";
    s += "<a href='#' class='button' onClick='javascript:mst.view_detail_setting();return false;'>キャンセル</a>";
    s += "</div>";
    s += "</center>";
    s += "</form>";
    set_html("setting_box", s);
    set_html("info_box", "");
    // ファイルが選択された時のイベント
    mst.firmware_file_data = null;
    $("firmware_file").addEventListener('change', (evt) => {
        // 選択されたファイルの情報を保持
        mst.firmware_file_data = evt.target.files;
    });
    mst.view_box(["info_box", "setting_box"]);
};


// ファームウェア更新
mst.firmware_update = function() {
    if (!mst.firmware_file_data || !mst.firmware_file_data.length) {
        set_html("firmware_info", "bin ファイルを指定して下さい");
        return;
    }
    hide("firmware_back_btn");
    hide("firmware_btn_box");
    // フォームの情報を送信
    var fd = new FormData($("firmware_form"));
    var xhr = new XMLHttpRequest();
    xhr.open("POST", "/firmware_update", true);
    xhr.upload.addEventListener('progress', function(evt) {
        if (!evt.lengthComputable) return;
        var per = Math.round((evt.loaded / evt.total) * 100);
        if (per < 100) {
            set_html("firmware_info", "ファイル送信中： " + per + " %");
        } else {
            set_html("firmware_info", "ファームウェア更新中 ...");
        }
    });
    xhr.addEventListener("error", mst.firmware_update_error);
    xhr.upload.addEventListener("abort", mst.firmware_update_error);
    xhr.upload.addEventListener("error", mst.firmware_update_error);
    xhr.upload.addEventListener("timeout", mst.firmware_update_error);
    xhr.onreadystatechange = function(e) {
        if (this.readyState == 4 && this.status == 200) {
            mst.firmware_update_ok(this);
        } else {
            mst.firmware_update_error(null);
        }
    };
    xhr.send(fd);
};


// ファームウェア更新エラー時の処理
mst.firmware_update_error = function(err) {
    set_html("info_box", "<b>エラー</b><br>ページを閉じて下さい。");
    // ページ移動警告解除
    mst.browser_btn_control_remove();
    mst.view_box(["info_box"]);
};

// ファームウェア更新OK
mst.firmware_update_ok = function(xhr) {
    set_html("info_box", "<b>アップデート成功</b><br>ページを閉じて下さい。");
    // ページ移動警告解除
    mst.browser_btn_control_remove();
    mst.view_box(["info_box"]);
};

// 設定の初期化画面表示
mst.view_init_setting = function() {
    var s = "";
    s += "<b style='font-size: 30px;'>設定の初期化</b><br><br>";
    s += "<font style='font-size: 32px;'>全ての設定を出荷時の状態に戻します。<br><br><br>";
    s += "<font style='color: #ff6347;'>※ 初期化すると元に戻せません。</font></font><br>";
    s += "<br><br><br>";
    s += "<div id='init_setting_info'></div>";
    s += "<center>";
    s += "<div id='init_setting_menu'>";
    s += "<a href='#' class='button' onClick='javascript:mst.init_setting();return false;'>初期化する</a>　　　";
    s += "<a href='#' class='button' onClick='javascript:mst.view_detail_setting();return false;'>キャンセル</a>";
    s += "</div>";
    s += "</center><br>";
    set_html("setting_box", s);
    set_html("info_box", "");
    mst.view_box(["info_box", "setting_box"]);
};

// 設定の初期化実行
mst.init_setting = function() {
    hide("init_setting_menu");
    set_html("init_setting_info", "初期化中...");
    ajax("/init_setting_json", "text", function(stat, res) {
        // 失敗したら初期化画面のまま
        if (!stat) {
            set_html("init_setting_info", "エラー 初期化できませんでした");
            show("init_setting_menu");
            return;
        }
        // 成功したらキーボードモードで再起動
        set_html("init_setting_info", "初期化完了しました");
        // ページ移動警告解除
        mst.browser_btn_control_remove();
        setTimeout(function() {
            mst.end_setting(0);
        }, 1000);
    });
};


