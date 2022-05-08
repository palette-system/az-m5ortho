// キーの動作を設定するモーダル

if (!window.aztool) aztool = {};

// 何番目のキーを選んでいるか
aztool.keyact_key_list_num = -1;

// モーダル用のHTML生成
aztool.keyact_init = function() {
    // モーダル用HTML登録
    let html = `
        <!-- レイヤー設定用モーダル -->
        <div class="remodal azmodal" data-remodal-id="keyact_modal" 
                data-remodal-options="hashTracking: false, closeOnOutsideClick: false"
                style="max-width: 1200px; width: 1200px; min-height: 600px;padding: 50px 180px;">
            <div style="text-align: left;margin: 0 0 20px 0;">
            <select id="key_action_type_select" style='width: 300px; padding: 10px; font-size: 20px;' onChange='javascript:aztool.keyact_acttype_change();'>
            <option value="0">動作無し</option>
            <option value="1">通常キー入力</option>
            <option value="2">テキスト入力</option>
            <option value="3">レイヤー切り替え</option>
            <option value="4">WEBフック</option>
            <option value="5">マウス移動</option>
            </select>
            </div>
            <div id="key_action_main_box" style="text-align: left; width: 870px; height: 400px; margin: 20px 0;overflow-x: hidden; overflow-y: auto;"></div>
            <div style="text-align: right;margin: 0 0 20px 0;">
            <a id="pin_set_ok" class="exec-button" onClick="javascript:aztool.keyact_close(1);">決定</a>　
            <a id="pin_set_cancel" class="cancel-button" onClick="javascript:aztool.keyact_close(0);">キャンセル</a>
        </div>

    </div>`;
    $("body").append(html);
    // モーダル登録
    aztool.keyact_mdl = $('[data-remodal-id=keyact_modal]').remodal();
    aztool.keyact_mdl.settings.closeOnOutsideClick = false;
    aztool.keyact_mdl.settings.hashTracking = false;

};

// 動作タイプ変更した時のイベント
aztool.keyact_acttype_change = function() {
    let k, s;
    // 動作タイプ変更
    aztool.keyact_edit_key.press.action_type = $("#key_action_type_select").val();
    // デフォルトのデータを入れる
    if (aztool.keyact_edit_key.press.action_type == 1) {
        // 通常入力
        if (!aztool.keyact_edit_key.press.key) {
            aztool.keyact_edit_key.press.key = [];
        }

    } else if (aztool.keyact_edit_key.press.action_type == 2) {
        // テキスト入力
        if (!aztool.keyact_edit_key.press.text) {
            aztool.keyact_edit_key.press.text = "";
        }

    } else if (aztool.keyact_edit_key.press.action_type == 3) {
        // レイヤー切り替え
        if (!aztool.keyact_edit_key.press.layer) {
            aztool.keyact_edit_key.press.layer = aztool.setting_json_data.default_layer; // デフォルトのれいやー
        }
        if (!aztool.keyact_edit_key.press.layer_type) {
            aztool.keyact_edit_key.press.layer_type = 0x51;
        }

    } else if (aztool.keyact_edit_key.press.action_type == 4) {
        // WEBフック
        if (!aztool.keyact_edit_key.press.webhook) {
            aztool.keyact_edit_key.press.webhook = {
                "url":"",
                "header":[],
                "post":"",
                "keyoutput":1
            };
        }
    
    } else if (aztool.keyact_edit_key.press.action_type == 5) {
        // マウス移動
        if (!aztool.keyact_edit_key.press.move) {
            aztool.keyact_edit_key.press.move = {"x": "0", "y": "0", "speed": "50"};
        }

    }
    // 入力フォーム表示
    aztool.keyact_form_view();
};

// キーの動作を設定するモーダルを開く
aztool.keyact_open = function(key_id) {
    let k = aztool.setmap_select_layer; // 選択中のレイヤーのキー名
    // 設定中のキーID
    aztool.keyact_key_id = key_id;
    // 設定データを取得
    if (aztool.setting_json_data.layers[k].keys[aztool.keyact_key_id]) {
        aztool.keyact_edit_key = aztool.crone(aztool.setting_json_data.layers[k].keys[aztool.keyact_key_id]);
    } else {
        // データが無ければ空データを入れる
        aztool.keyact_edit_key = {"press": {"action_type": 0}};
    }
    // 入力フォーム表示
    aztool.keyact_form_view();
    // モーダルを開く
    aztool.keyact_mdl.open();
};

// キーの動作を設定するモーダルを閉じる
aztool.keyact_close = function(save_flag) {
    let k = aztool.setmap_select_layer; // 選択中のレイヤーのキー名
    let t;
    if (save_flag) { // 変更内容を反映
        if (aztool.keyact_edit_key.press.action_type == 2) {
            // テキスト入力
            t = $("#key_action_form_text").val();
            if (!aztool.is_han(t)) {
                alert("英数字で入力してください。");
                return;
            }
            aztool.keyact_edit_key.press.text = t;

        } else if (aztool.keyact_edit_key.press.action_type == 3) {
            // レイヤー切り替え
            aztool.keyact_edit_key.press.layer = $("#key_action_form_layer").val();
            aztool.keyact_edit_key.press.layer_type = $("#key_action_form_layer_type").val();

        } else if (aztool.keyact_edit_key.press.action_type == 4) {
            // WEBフック
            aztool.keyact_edit_key.press.webhook = {
                "url": $("#key_action_form_url").val(),
                "header": aztool.keyact_form_webheader_get_data(),
                "post": $("#webhook_post").val(),
                "keyoutput": parseInt($("#webhook_keyoutput").val())
            };

        } else if (aztool.keyact_edit_key.press.action_type == 5) {
            // マウス移動
            aztool.keyact_edit_key.press.move = {
                "x": $("#keyact_mouse_x").val(),
                "y": $("#keyact_mouse_y").val(),
                "speed": $("#keyact_mouse_speed").val()
            };

        }
        // 編集データを反映
        aztool.setting_json_data.layers[k].keys[aztool.keyact_key_id] = aztool.keyact_edit_key;
        // ボタンの文字と色を更新
        aztool.setmap_key_string_update();
    }
    aztool.keyact_mdl.close();
};

// 入力フォームを表示
aztool.keyact_form_view = function() {
    let press = aztool.keyact_edit_key.press;
    // 入力の種類
    $("#key_action_type_select").val(press.action_type);
    // フォーム作成
    if (press.action_type == 0) {
        // 動作無し
        $("#key_action_main_box").html("");

    } else if (press.action_type == 1) {
        // 通常入力
        aztool.keyact_form_normal_view();

    } else if (press.action_type == 2) {
        // テキスト入力
        aztool.keyact_form_text_view();
        
    } else if (press.action_type == 3) {
        // レイヤー切り替え
        aztool.keyact_form_layer_view();

    } else if (press.action_type == 4) {
        // WEBフック
        aztool.keyact_form_webhook_view();

    } else if (press.action_type == 5) {
        // マウス移動
        aztool.keyact_form_mouse_view();

    } else {
        // 不明な入力はフォームクリア
        $("#key_action_main_box").html("");
    }

};

// 通常入力フォーム表示
aztool.keyact_form_normal_view = function() {
    let h = "";
    let c, d, i, k, s;
    let press = aztool.keyact_edit_key.press;
    // 通常入力
    h += "<div class='keyaction_form_title'>入力するコード</div>";
    k = [];
    for (i in press.key) {
        if (!press.key[i]) continue; // 0は無視
        k.push(press.key[i]);
    }
    press.key = aztool.crone(k);
    while (k.length < 5) k.push(0);
    for (i in k) {
        d = aztool.get_key_data(2, k[i]);
        c = (d.hid)? "#e7fbff": "#f7f7f7";
        s = (d.hid)? d.str: "";
        h += "<div id='kbn_"+i+"' class='keyaction_key_btn' style='background-color: "+c+";' onClick='javascript:aztool.keyact_key_list_open("+i+", "+d.hid+");'>"+s+"</div>";
    }
    h += "<div id='keyact_form_key_list' style='width: 900px; height: 400px;display: none; position: absolute; overflow-y: scroll; background-color: #f5f5f5;border: solid 2px #888;box-shadow: 3px 3px 4px #aaa;'>";
    h += "</div>";
    h += "<br><br><br>";
    h += "<div class='keyaction_form_title'>長押しで入力するコード　（長押しと短押しで入力を分ける場合）</div>";
    h += "<div id='kbh_0' class='keyaction_key_btn' style='background-color: #f7f7f7;' onClick='javascript:aztool.keyact_key_hold_open();'></div>";
    h += "<div id='keyact_form_hold_list' style='width: 900px; height: 250px;display: none; position: absolute; overflow-y: scroll; background-color: #f5f5f5;border: solid 2px #888;box-shadow: 3px 3px 4px #aaa;'>";
    $("#key_action_main_box").html(h);
    aztool.keyact_key_hold_btn_color_update();
};

// 選択しているボタンの色を変える
aztool.keyact_btn_color_update = function(list_num) {
    let i;
    // 編集しているボタンの色を変える
    for (i=0; i<5; i++) {
        if (i == list_num) {
            $("#kbn_"+i).css({"background-color": "#d3ebff"}); // 編集中
        } else if (i < aztool.keyact_edit_key.press.key.length) {
            $("#kbn_"+i).css({"background-color": "#e7fbff"}); // 登録データがある
        } else {
            $("#kbn_"+i).css({"background-color": "#f7f7f7"}); // 登録データがない
        }
    }
};

// hold 用の表示名前を取得
aztool.keyact_get_host_str = function(hold_id) {
    let t = hold_id >> 4;
    let n = hold_id & 0x0F;
    if (t == 0x06) { // 左モディファイ
        if (n & 0x01) return "*Ctrl";
        if (n & 0x02) return "*Shift";
        if (n & 0x04) return "*Alt";
        if (n & 0x08) return "*GUI";
    } else if (t == 0x07) { //  右モデファイ
        if (n & 0x01) return "Ctrl*";
        if (n & 0x02) return "Shift*";
        if (n & 0x04) return "Alt*";
        if (n & 0x08) return "GUI*";
    } else if (t == 0x04) { // レイヤー
        return aztool.setmap_get_layer_name(n); // レイヤー名取得
    }
    return "";
}

// コード選択用の枠表示 
aztool.keyact_key_list_open = function(list_num, hid_id) {
    let b, c, d, h, i, j, k, n;
    let code_list = [];
    // 長押し設定中であれば長押し側は閉じる
    if (aztool.keyact_key_list_num == 5) {
        aztool.keyact_key_list_num = -1;
        // ボタンの色を更新
        aztool.keyact_key_hold_btn_color_update();
        // 選択枠を消す
        $("#keyact_form_hold_list").hide();
    }
    // 既に選んでいる最中で、同じ所をクリックされたら閉じる
    if (aztool.keyact_key_list_num >= 0 && aztool.keyact_key_list_num == list_num) {
        // 選んでいた番号をリセット
        aztool.keyact_key_list_num = -1;
        // ボタンの色を更新
        aztool.keyact_btn_color_update(-1);
        // 選択枠を消す
        $("#keyact_form_key_list").hide();
        return;
    }
    aztool.keyact_key_list_num = list_num; // 何番目のデータを編集中か
    // ボタンの色を更新
    aztool.keyact_btn_color_update(list_num);
    // 選択枠を表示
    h = "";
    h += "<table style='padding: 10px;'>";
    for (i in aztool.key_category) {
        k = aztool.key_category[i];
        h += "<tr><td><b>"+k.category+"</b></td><td>";
        h += "<tr><td>";
        h += "<table>";
        for (j in k.list) {
            h += "<tr><td valign='top' style='padding: 15px 10px;white-space:nowrap;'>"+k.list[j].name+"</td><td style='padding: 10px 0;'>";
            for (n in k.list[j].list) {
                c = k.list[j].list[n]; // HIDの番号
                d = aztool.get_key_data(2, c); // キーコードの情報
                b = (c && c == hid_id)? "#d3ebff": "#fff"; // 登録されてるIDは色を付ける
                h += "<div id='ka_"+c+"' style='padding: 6px;margin: 4px; width: 30px; height: 30px;display: inline-block;border: solid 1px #b9b9b9; background-color: "+b+";font-size: 12px;text-align: center;overflow: hidden; cursor: pointer;'>"+d.str+"</div>";
                code_list.push(c);
            }
            h += "</td></tr>";
        }
        h += "</table>";
        h += "</td></tr>";
    }
    h += "</table>";
    $("#keyact_form_key_list").html(h);
    $("#keyact_form_key_list").show();
    // クリックした時のイベント登録
    for (i in code_list) {
        $("#ka_"+code_list[i]).click(function(e) {
            let t = (e.target.id)? e.target.id: e.currentTarget.id; // ka_{hid} が入る
            let s = t.split("_");
            if (aztool.keyact_edit_key.press.key.length > aztool.keyact_key_list_num) {
                // 既に設定されていたIDの変更
                aztool.keyact_edit_key.press.key[aztool.keyact_key_list_num] = parseInt(s[1]);
            } else {
                // 設定されていなかった所の変更はリストに追加
                if (aztool.keyact_edit_key.press.key.length < 5) { // 登録数は5件まで
                    aztool.keyact_edit_key.press.key.push(parseInt(s[1]));
                }
            }
            // 選択が終わったら選んでいた番号をリセット
            aztool.keyact_key_list_num = -1;
            // 再度フォームを表示
            aztool.keyact_form_view();
        });
    }
};

// hold設定ボタンの色と文字を更新
aztool.keyact_key_hold_btn_color_update = function() {
    // ボタンの色を更新
    let h = aztool.keyact_edit_key.press.hold;
    let c = (h)? "#e7fbff": "#f7f7f7"; // データが有る/無いの色
    $("#kbh_0").css({"background-color": c});
    // ボタンの表示文字更新
    let n = (h)? aztool.keyact_get_host_str(h): "";
    $("#kbh_0").html(n);
};

// ホールドボタン用リストオープン
aztool.keyact_key_hold_open = function() {

    let b, c, h, i, n;
    // 既に選んでいる最中で、同じ所をクリックされたら閉じる
    if (aztool.keyact_key_list_num >= 0) {
        if (aztool.keyact_key_list_num == 5) { // 自分が開かれてて自分が押された
            // 選んでいた番号をリセット
            aztool.keyact_key_list_num = -1;
            // ボタンの色を更新
            aztool.keyact_key_hold_btn_color_update();
            // 選択枠を消す
            $("#keyact_form_hold_list").hide();
        } // 他のが開かれてた場合は何もしない
        return;
    }
    aztool.keyact_key_list_num = 5; // 何番目のデータを編集中か
    // ボタンの色を編集中に
    $("#kbh_0").css({"background-color": "#d3ebff"}); // 編集中
    // 選択枠を表示
    let mlist = [
        {"hold": 0x61, "str": "*Ctrl"},
        {"hold": 0x62, "str": "*Shift"},
        {"hold": 0x64, "str": "*Alt"},
        {"hold": 0x68, "str": "*GUI"},
        {"hold": 0x71, "str": "Ctrl*"},
        {"hold": 0x72, "str": "Shift*"},
        {"hold": 0x74, "str": "Alt*"},
        {"hold": 0x78, "str": "GUI*"}
    ];
    h = "";
    h += "<table>";
    h += "<tr><td valign='top' style='padding: 15px 10px;white-space:nowrap;'>削除</td><td style='padding: 10px 0;'>";
    h += "<div id='ka_0' class='keyaction_stg_btn' style='background-color: #fff;' onClick='javascript:aztool.keyact_key_hold_code_click(0);'>✖</div>";
    h += "</td></tr>";
    h += "<tr><td valign='top' style='padding: 15px 10px;white-space:nowrap;'>Mod</td><td style='padding: 10px 0;'>";
    // b = (c && c == hid_id)? "#d3ebff": "#fff"; // 登録されてるIDは色を付ける
    for (i in mlist) {
        b = (aztool.keyact_edit_key.press.hold == mlist[i].hold)? "#d3ebff": "#fff"; // 登録されてるIDは色を付ける
        h += "<div id='ka_"+mlist[i].hold+"' class='keyaction_stg_btn' style='background-color: "+b+";' onClick='javascript:aztool.keyact_key_hold_code_click("+mlist[i].hold+");'>"+mlist[i].str+"</div>";
    }
    h += "</td></tr>";
    h += "<tr><td valign='top' style='padding: 15px 10px;white-space:nowrap;'>レイヤ<br>切替</td><td style='padding: 10px 0;'>";
    for (i=0; i<16; i++) {
        n = aztool.setmap_get_layer_name(i); // レイヤー名取得
        if (!n) continue;
        c = 0x40 + i;
        b = (aztool.keyact_edit_key.press.hold == c)? "#d3ebff": "#fff"; // 選択していれば色を変える
        h += "<div id='ka_"+c+"' class='keyaction_stg_btn' style='background-color: "+b+"; width: 120px;' onClick='javascript:aztool.keyact_key_hold_code_click("+c+");'>"+n+"</div>";
    }
    h += "</td></tr>";
    h += "</table>";
    $("#keyact_form_hold_list").html(h);
    $("#keyact_form_hold_list").show();
};

// ホールド設定クリック
aztool.keyact_key_hold_code_click = function(hold_id) {
    // ホールドの設定を更新
    if (hold_id) {
        aztool.keyact_edit_key.press.hold = hold_id;
    } else {
        delete aztool.keyact_edit_key.press.hold;
    }
    // 選んでいた番号をリセット
    aztool.keyact_key_list_num = -1;
    // ボタンの色と文字を更新
    aztool.keyact_key_hold_btn_color_update();
    // 選択枠を消す
    $("#keyact_form_hold_list").hide();

};

// テキスト入力フォーム
aztool.keyact_form_text_view = function() {
    let h = "";
    let press = aztool.keyact_edit_key.press;
    h += "<div class='keyaction_form_title'>入力する文字</div>";
    h += "<input type='text' id='key_action_form_text' value='"+press.text+"' style='font-size: 20; line-height: 30px; padding: 6px 20px; width: 600px; margin: 10px 0;'><br>";
    h += "<font style='font-size: 16px;'>※ 英数字で入力して下さい。</font>";
    $("#key_action_main_box").html(h);
};

// レイヤー切り替えフォーム
aztool.keyact_form_layer_view = function() {
    let c, i, n, h = "", s;
    let press = aztool.keyact_edit_key.press;
    h += "<div class='keyaction_form_title'>切替えるレイヤー</div>";
    h += "<select id='key_action_form_layer' style='padding: 10px; font-size: 20px; width: 500px;'>";
    for (i in aztool.setting_json_data.layers) {
        s = i.split("_");
        n = aztool.setmap_get_layer_name(s[1]);
        c = (press.layer == parseInt(s[1]))? " selected": "";
        h += "<option value='"+s[1]+"' "+c+">"+n+"</option>";
    }
    h += "</select><br><br><br>";
    h += "<div class='keyaction_form_title'>切替え方</div>";
    h += "<select id='key_action_form_layer_type' style='padding: 10px; font-size: 20px; width: 500px;'>";
    for (i in aztool.layer_move_type_list) {
        s = aztool.layer_move_type_list[i];
        c = (press.layer_type == s.key)? " selected": "";
        h += "<option value='"+s.key+"' "+c+">"+s.value+"</option>";
    }
    h += "</select><br>";
    $("#key_action_main_box").html(h);
};

// WEBフックフォーム
aztool.keyact_form_webhook_view = function() {
    let i, h = "";
    let webhook = aztool.keyact_edit_key.press.webhook;
    h += "<div class='keyaction_form_title'>WEBフックする URL</div>";
    h += "<input type='text' id='key_action_form_url' value='' style='font-size: 20; line-height: 30px; padding: 6px 20px; width: 600px; margin: 10px 0;'><br>";
    h += "<br><br>";
    h += "<div class='keyaction_form_title'>送信するHTTPヘッダ</div>";
    // h += "<div style='width: 800px; height: 200px; border: solid 1px #888; overflow-y: scroll; overflow-x: hidden; background-color: #f5f5f5;'></div>";
    h += "<div id='webheader_box' style='max-height: 200px; overflow-y: auto; width: 800px; background-color: #f9f9f9;'></div>";
    h += "<a id='' class='exec-button' onClick='javascript:aztool.keyact_form_webheader_add();'>ヘッダ追加</a><br>";
    h += "<br><br>";
    h += "<div class='keyaction_form_title'>送信するPOSTデータ</div>";
    h += "<textarea id='webhook_post' style='width: 800px; height: 100px; border: solid 1px #888; overflow-y: scroll; overflow-x: auto; background-color: #fdfdfd;font-size: 18px;'></textarea><br>";
    h += "<br><br>";
    h += "<div class='keyaction_form_title'>WEBフック後のキー入力</div>";
    h += "<select id='webhook_keyoutput' style='font-size: 18px; padding: 6px 20px; width: 300px;'>";
    h += "<option value='0'>無し</option>";
    h += "<option value='1'>ステータスコードを入力</option>";
    h += "<option value='2'>HTTPレスポンスを入力</option>";
    h += "</select>";
    h += "<br><br><br><br>";
    $("#key_action_main_box").html(h);
    $("#key_action_form_url").val(webhook.url);
    $("#webhook_post").val(webhook.post);
    $("#webhook_keyoutput").val(webhook.keyoutput + "");
    for (i in webhook.header) {
        // フォームを追加
        aztool.keyact_form_webheader_add();
        $("#webhead_key_"+i).val( webhook.header[i].key );
        $("#webhead_val_"+i).val( webhook.header[i].value );
    }

};

// WEBフック用ヘッダ入力フォーム追加
aztool.keyact_form_webheader_add = function() {
    let i;
    let h = "";
    // フォームのhtmlを生成
    for (i=0; i<16; i++) { // 最大で16個
        if ($("#webhead_"+i).length) continue; // 既にフォームがあれば次
        h += "<div id='webhead_"+i+"'>";
        h += "<input type='text' id='webhead_key_"+i+"' placeholder='key' style='font-size: 18px; padding: 2px 8px; margin: 4px; width: 200px;'>：";
        h += "<input type='text' id='webhead_val_"+i+"' placeholder='value' style='font-size: 18px; padding: 2px 8px; margin: 4px; width: 400px;'>";
        h += "　<input type='button' value='✖' style='cursor: pointer;' onClick='javascript:aztool.keyact_form_webheader_remove("+i+");'>";
        h += "</div>";
        break;
    }
    if (h) $("#webheader_box").append(h);
};

// WEBフック用ヘッダ入力フォーム削除
aztool.keyact_form_webheader_remove = function(n) {
    let i, j;
    for (i=0; i<16; i++) {
        if (i < n) continue; // 削除対象より前のフォームは変更なし
        if (!$("#webhead_"+i).length) continue; // フォームが無ければ何もしない
        j = i + 1;
        if (!$("#webhead_"+j).length) {
            // 次のフォームが無い(最後のフォームなので削除)
            $("#webhead_"+i).remove();
        } else {
            // 次のフォームがあれば次のフォームの入力内容を自分にコピー
            $("#webhead_key_"+i).val( $("#webhead_key_"+j).val() );
            $("#webhead_val_"+i).val( $("#webhead_val_"+j).val() );
        }
    }
};

// WEBフック用ヘッダ入力フォームの入力内容を配列にして取得
aztool.keyact_form_webheader_get_data = function() {
    let i, k, v;
    let r = [];
    for (i=0; i<16; i++) {
        if (!$("#webhead_"+i).length) continue; // フォームが無ければ何もしない
        k = $("#webhead_key_"+i).val();
        v = $("#webhead_val_"+i).val();
        if (!k.length) continue; // キーの入力が空なら無視
        r.push({"key": k, "value": v});
    }
    return r;
};

// マウス移動設定フォーム表示
aztool.keyact_form_mouse_view = function() {
    let h = "";
    let press = aztool.keyact_edit_key.press;
    h += "<div class='keyaction_form_title'>移動</div>";
    h += "<div style='margin: 20px 40px;'>";
    h += "<b>Ｘ：</b><font id='keyact_mouse_x_val'>"+press.move.x+"</font><br><input type='range' id='keyact_mouse_x' min='-100' max='100' style='width: 400px;' value='"+press.move.x+"' onChange='javascript:aztool.keyact_mouse_move(\"x\");'><br><br>"
    h += "<br>";
    h += "<b>Ｙ：</b><font id='keyact_mouse_y_val'>"+press.move.y+"</font><br><input type='range' id='keyact_mouse_y' min='-100' max='100' style='width: 400px;' value='"+press.move.y+"' onChange='javascript:aztool.keyact_mouse_move(\"y\");'><br><br>"
    h += "<br>";
    h += "<b>スピード：</b><font id='keyact_mouse_speed_val'>"+press.move.speed+"</font><br><input type='range' id='keyact_mouse_speed' min='0' max='100' style='width: 400px;' value='"+press.move.speed+"' onChange='javascript:aztool.keyact_mouse_move(\"speed\");'>"
    h += "</div>";
    $("#key_action_main_box").html(h);
};

aztool.keyact_mouse_move = function(chtype) {
    $("#keyact_mouse_"+chtype+"_val").html( $("#keyact_mouse_"+chtype).val() );
};