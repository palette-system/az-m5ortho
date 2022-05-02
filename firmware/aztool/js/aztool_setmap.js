// キーマップ設定


if (!window.aztool) aztool = {};

// 今選択中のレイヤー番号
aztool.setmap_select_layer = "";

aztool.setmap_layer_keys = [];

// 設定操作中フラグ
aztool.setmap_stat = 0; // 0=何もしてない / 1=一括設定中 / 2=1キー設定中 / 3=レイヤー設定

// キーマップ設定画面表示
aztool.view_setmap = function() {
    let h = `
    <div id='key_layout_box' style='width: 1000px; height: 500px;overflow: hidden; border: solid 1px black; text-align: left;'></div>
    <br><br>
    <div id="setmap_info"></div>
    <br>
    <a href='#' onClick='javascript:aztool.setmap_all_set(0);'>一括設定</a><br><br>

    <a href='#' onClick='javascript:aztool.setmap_layer_set();'>レイヤー設定</a><br><br>

    <a href='#' onClick='javascript:aztool.setmap_save();'>設定を保存して再起動</a><br><br>

    <a href='#' onClick='javascript:aztool.view_top_menu();'>もどる</a><br>
    `;
    $("#main_box").html(h);
    // レイヤーのリスト取得
    aztool.setmap_get_layer_list();
    // 選択中のレイヤーをデフォルトにする
    aztool.setmap_select_layer = "layer_" + aztool.setting_json_data.default_layer;
    // キーのレイアウト表示
    aztool.view_key_layout();
    // ボタンに設定されている文字を表示
    aztool.setmap_key_string_update();
};

// モーダル用HTML登録
aztool.setmap_init = function() {
    // モーダル用HTML登録
    let html = `
        <div class="remodal azmodal" data-remodal-id="setmap_layer_modal" 
                data-remodal-options="hashTracking: false, closeOnOutsideClick: false"
                style="max-width: 1200px; width: 1200px; min-height: 600px;">
            <!-- クローズボタン -->
            <!-- button data-remodal-action="close" class="remodal-close"></button> -->
            <table style="display: inline-block; margin: 50px 0;"><tr><td valign="top">
            <font style="font-size: 14px;">レイヤーリスト</font><br>
            <ul id="layer_list" class="layer_list_box"></ul>
            </td><td valign="bottom">
            <div class="list_side_btn" onClick="javascript: aztool.setmap_layer_add_btn_click();">＋追加</div><br>
            <div class="list_side_btn" onClick="javascript: aztool.setmap_layer_delete_btn_click();">－削除</div><br>
            </td><td valign="top" style="width: 500px;">
            <div id="layer_edit_form">
            <table style="font-size: 14px; height: 180px;">
            <tr><td>レイヤー番号</td><td><input id="layer_num_edit" type="text" class="layer_input_box" value="0"></td></tr>
            <tr><td>レイヤー名</td><td><input id="layer_name_edit" type="text" class="layer_input_box" value="レイヤー１"></td></tr>
            </table>
            <br><br>
            <div style="text-align: center;">
            <div id="layer_edit_form_info"></div>
            <a id="pin_set_ok" class="exec-button" onClick="javascript:aztool.setmap_layer_default_layer_set();" style="width: 240px;">デフォルトのレイヤーにする</a>　
            <a id="pin_set_ok" class="exec-button" onClick="javascript:aztool.setmap_layer_form_save();">反映</a>
            </div>
            </div>
            </td></tr></table>
            <div style="text-align: right; width: 1050px;margin: 0 0 20px 0;">
            <a id="pin_set_ok" class="exec-button" onClick="javascript:aztool.setmap_layer_edit_close(1);">決定</a>　
            <a id="pin_set_cancel" class="cancel-button" onClick="javascript:aztool.setmap_layer_edit_close(0);">キャンセル</a>
        </div>

    </div>`;
    $("body").append(html);
    // モーダル登録
    aztool.setmap_layer_mdl = $('[data-remodal-id=setmap_layer_modal]').remodal();
    aztool.setmap_layer_mdl.settings.closeOnOutsideClick = false;
    aztool.setmap_layer_mdl.settings.hashTracking = false;
    // リストの操作登録
    $('#layer_list').sortable({
        "distance": 5,
        "axis": "y",
        "opacity": 0.5,
        "update": function(event, ui) {
            // 並び順が変更された
            // 編集中データの並び替え
            let ll = $('#layer_list').sortable("toArray"); // リストの並び順取得
            let i, k, s, r = {};
            for (i in ll) {
                s = ll[i].split("_");
                k = s[1] + "_" + s[2];
                r[k] = aztool.setmap_layer_edit_data[k];
            }
            aztool.setmap_layer_edit_data = r;
        }
    });
};

// オプションが有効か
aztool.on_i2coption = function(option_data) {
    // 有効でない場合
    if (option_data.enable == 0) return false;
    // それ以外は有効
    return true;
};

// キーレイアウトを表示
aztool.view_key_layout = function() {
    let h = "";
    let i, j, o, s;
    // キー配列を表示する枠を表示
    h += "<div id='odiv_0' style='position: relative; top: 250px;'></div>"; // 本体のキー配列用
    for (i in aztool.setting_json_data.i2c_option) {
        o = aztool.setting_json_data.i2c_option[i];
        if (!aztool.on_i2coption(o)) continue; // 有効でないオプションは無視
        h += "<div id='odiv_"+o.id+"' style='position: relative; top: 250px;'></div>"; // オプションのキー配列用
    }
    h += "<div id='layer_title_info' class='layer_title'>レイヤー名</div>";
    $("#key_layout_box").html(h);
    // キー配列を表示
    aztool.key_layout_data = [];
    // 本体のキー配列を表示
    aztool.key_layout_data.push({
        "option": {"id": 0, "map_start": 0},
        "kle": aztool.kle_view(aztool.main_kle[aztool.setting_json_data.keyboard_type], "#odiv_0", false, 40, "sw_0_")
    });
    // オプションのキー配列を表示
    for (i in aztool.setting_json_data.i2c_option) {
        o = aztool.setting_json_data.i2c_option[i];
        if (!aztool.on_i2coption(o)) continue; // 有効でないオプションは無視
        aztool.key_layout_data.push({
            "option": aztool.setting_json_data.i2c_option[i],
            "kle": aztool.kle_view(aztool.i2c_option_data["o"+o.id], "#odiv_"+o.id, false, 40, "sw_" + o.id + "_")
        });
    }
    // それぞれのオプションをドラッグで移動できるようにする
    for (i in aztool.key_layout_data) {
        o = aztool.key_layout_data[i].option;
        $("#odiv_"+o.id).draggable({
            "stop": function() { // ドラッグ終了
                // ドラッグが終わった時間を記録(クリックイベントとかぶらないよう時間で判定)
                aztool.setmap_dragg_last_time = aztool.millis();
            }
        });
    }
    // ボタンのクリックイベント
    for (i in aztool.key_layout_data) { // kleのデータループ
        s = aztool.key_layout_data[i].kle;
        o = aztool.key_layout_data[i].option;
        for (j in s.keys) { // kle のキー分ループ
            // ボタンにクリックイベント登録
            $("#sw_"+o.id+"_"+j).click(function(e) {
                let t;
                // ドラッグ処理から10ミリ秒以下ならばドラッグ処理でのクリックなので何もしない
                if ((aztool.millis() - aztool.setmap_dragg_last_time) < 10) {
                    return;
                }
                // クリックされたdivのid取得
                t = (e.target.id)? e.target.id: e.currentTarget.id;
                // クリックしたボタンのキー設定
                aztool.setmap_key_setting_one(t);
            });
        }
    }
    // レイヤー名のドロップダウン表示非表示イベント
    $("#layer_title_info").hover(function(){
        $("#layer_menu_list").show();
    }, function(){
        $("#layer_menu_list").hide();
    });
};

// １ボタン設定
aztool.setmap_key_setting_one = function(target_id) {
    // 他の設定操作中であれば何もしない
    if (aztool.setmap_stat != 0) return;
    // １ボタン設定開始
    aztool.setmap_stat = 2;
    aztool.setmap_setting_one_target = target_id;
    // 該当のキーの色を選択中にする
    $("#"+target_id).css({"background-color": aztool.select_key_color});
    // info 表示
    $("#setmap_info").html("設定したいキーを押して下さい。");
    // キーダウンイベント登録
    document.body.addEventListener("keydown", aztool.setmap_key_setting_one_keydown, false);
};

// １ボタン設定でキーが押された
aztool.setmap_key_setting_one_keydown = function(e) {
    let input_key = aztool.get_key_data(3, e.keyCode); // 押されたキーの情報を取得
    let target_data = aztool.setmap_setting_one_target.split("_"); // setmap_setting_one_target に sw_0_0 が入ってる
    let target_option = parseInt(target_data[1]);
    let target_key = parseInt(target_data[2]);
    let i, option_set;
    for (i in aztool.setting_json_data.i2c_option) {
        if (aztool.setting_json_data.i2c_option[i].id == target_option) {
            option_set = aztool.setting_json_data.i2c_option[i];
        }
    }
    let key_id = option_set.map_start + target_key;
    let k = aztool.setmap_select_layer; // 選択中のレイヤーのキー名
    // 入力データを設定
    aztool.setting_json_data.layers[k].keys["key_"+key_id] = aztool.setmap_create_one_key_data(input_key);
    // ボタンの文字と色を更新
    aztool.setmap_key_string_update();
    // キーダウンイベント削除
    document.body.removeEventListener("keydown", aztool.setmap_key_setting_one_keydown, false);
    // １ボタン設定終了
    aztool.setmap_stat = 0;
};

// 選択してるレイヤーの設定をボタンに表示する
aztool.setmap_key_string_update = function() {
    let d, h, i, j, k, l, o, s;
    let str;
    l = aztool.setmap_select_layer; // 選択中のレイヤーのキー名
    d = aztool.setting_json_data.layers[l]; // 選択レイヤーの設定されている情報
    // ボタンに文字を表示
    for (i in aztool.key_layout_data) { // kleのデータループ
        s = aztool.key_layout_data[i].kle;
        o = aztool.key_layout_data[i].option;
        for (j in s.keys) { // kle のキー分ループ
            str = "&nbsp;"; // 表示文字のデフォルトは空
            k = o.map_start + parseInt(j); // 設定JSON上で設定されてるkey_{n}の番号
            if (d.keys["key_" + k]) { // 設定がある
                str = aztool.setmap_get_key_string(d.keys["key_" + k]);
            }
            // 設定文字を反映
            $("#sw_"+o.id+"_"+j).html(str); // ボタンに文字を表示
            $("#sw_"+o.id+"_"+j).css({"background-color": aztool.key_color}); // ボタンの色もデフォルトに
        }
    }
    // レイヤー名の更新
    h = "<div id='layer_menu_top' class='layer_title_name'>" + d.name + "</div>";
    if (Object.keys(aztool.setting_json_data.layers).length > 1) { // レイヤーが2個以上あればドロップダウンリスト追加
        h += "<div id='layer_menu_list' class='layer_title_menu'>";
        h += "<ul>";
        for (k in aztool.setting_json_data.layers) {
            s = (k == l)? " style='background-color: #aed4ff;'": ""; // 選択中のレイヤーは色を変える
            o = (k == "layer_" + aztool.setting_json_data.default_layer)? " <font style='font-size: 11px;'>デフォルト</font>": ""; // デフォルトのレイヤーはデフォルト文字追加
            h += "<li id='layer_menu_item_"+k+"'"+s+">"+aztool.setting_json_data.layers[k].name+ o +"</li>";
        }
        h += "</ul>";
        h += "</div>";
    }
    $("#layer_title_info").html(h);
    if (Object.keys(aztool.setting_json_data.layers).length > 1) { // レイヤーが2個以上あればドロップダウン動作追加
        for (k in aztool.setting_json_data.layers) {
            $("#layer_menu_item_"+k).click(function(e){
                let tid = (e.target.id)? e.target.id: e.currentTarget.id;
                let s = tid.split("_");
                aztool.setmap_select_layer = s[3] + "_" + s[4];
                aztool.setmap_key_string_update();
            });
        }
    }
};

// キーデータから表示用の文字取得
aztool.setmap_get_key_string = function(key_data) {
    let i, d;
    let k = key_data.press;
    let r = "&nbsp;";
    if (k.action_type == 1) {
        // 通常入力
        r = "";
        for (i in k.key) {
            if (parseInt(i)) r += " + "
            d = aztool.get_key_data(2, k.key[i]); // hid コードからキーデータ取得
            r += d.str;
        }
    }
    return "<table cellpadding='0' cellspacing='0' style='user-select: none; width: 100%; height: 100%;font-size: 10px;'><tr><td align='center'>" + r + "</td></tr></table>";
};

// レイヤーのリスト取得
aztool.setmap_get_layer_list = function() {
    aztool.setmap_layer_keys = Object.keys(aztool.setting_json_data.layers);
};

// キーマップ一括登録開始
aztool.setmap_all_set = function() {
    // 他の設定動作中であれば無視
    if (aztool.setmap_stat != 0) return;
    // 一括登録スタート
    aztool.setmap_stat = 1;
    // 設定インデックスリセット
    aztool.setmap_all_index = 0;
    // 設定データを入れる所
    aztool.setmap_all_set_data = {};
    // キーダウンイベント登録
    document.body.addEventListener("keydown", aztool.setmap_all_set_keydown, false);
    // キーに色を付ける
    aztool.setmap_all_set_view();
    // info表示
    $("#setmap_info").html("登録したいキーを押して下さい。");

};

// 一括設定の表示処理
aztool.setmap_all_set_view = function() {
    let i, j, o, p, s;
    p = 0;
    for (i in aztool.key_layout_data) {
        s = aztool.key_layout_data[i].kle;
        o = aztool.key_layout_data[i].option;
        for (j in s.keys) {
            if (p == aztool.setmap_all_index) {
                $("#sw_"+o.id+"_"+j).css({"background-color": aztool.select_key_color}); // 選択して欲しいキー
            } else if (p < aztool.setmap_all_index) {
                $("#sw_"+o.id+"_"+j).css({"background-color": aztool.enable_key_color}); // 選択が終わったキー
            } else {
                $("#sw_"+o.id+"_"+j).css({"background-color": aztool.key_color}); // まだ読み込んでいないキー
            }
            p++;
        }
    }
};

// 1キーの入力データ作成
aztool.setmap_create_one_key_data = function(keycode_data) {
    return {
        "press":{
            "action_type":1,
            "repeat_interval":51,
            "key":[keycode_data.hid]
        }
    };
};

// キーマップ一括登録表示
aztool.setmap_all_set_keydown = function(e) {
    let f, i, j, k, o, p, s, str;
    let input_key = aztool.get_key_data(3, e.keyCode); // 押されたキーの情報を取得
    console.log(input_key);
    p = 0;
    f = false;
    for (i in aztool.key_layout_data) {
        s = aztool.key_layout_data[i].kle;
        o = aztool.key_layout_data[i].option;
        for (j in s.keys) {
            if (p == aztool.setmap_all_index) {
                k = o.map_start + parseInt(j);
                aztool.setmap_all_set_data["key_" + k] = aztool.setmap_create_one_key_data(input_key); // 入力データを追加
                str = aztool.setmap_get_key_string(aztool.setmap_all_set_data["key_" + k]); // 表示用の文字取得
                $("#sw_"+o.id+"_"+j).html(str); // ボタンに文字を表示
                str = p + " - " + k + " - " + input_key.str + "<br>";
                str += "<a href='#' onClick='javascript:aztool.setmap_all_set_keydown_back();'>ひとつ前のボタンに戻る</a><br>";
                str += "<a href='#' onClick='javascript:aztool.setmap_all_set_keydown_cancel();'>キャンセル</a><br><br>";
                $("#setmap_info").html(str); // インフォメーションに設定内容を表示
                f = true;
            }
            p++;
        }
    }
    aztool.setmap_all_index++;
    // キーに色を付ける
    aztool.setmap_all_set_view();
    // 全てのキー設定完了
    if (p <= aztool.setmap_all_index) {
        console.log(aztool.setmap_all_set_data);
        // 一括登録終了
        aztool.setmap_stat = 0;
        $("#setmap_info").html("完了");
        // キーダウンイベント削除
        document.body.removeEventListener("keydown", aztool.setmap_all_set_keydown, false);
        // 設定データに追加
        k = aztool.setmap_select_layer; // 選択中のレイヤーのキー名
        aztool.setting_json_data.layers[k].keys = aztool.setmap_all_set_data; // 設定データのレイヤーを変更
        // ボタンの文字と色を更新
        aztool.setmap_key_string_update();
    }
    // キーの入力をキャンセル
    e.stopPropagation();
    e.preventDefault();
    return false;
};

// ひとつ前のボタンに戻る
aztool.setmap_all_set_keydown_back = function() {
    aztool.setmap_all_index--;
    // キーに色を付ける
    aztool.setmap_all_set_view();
};

// 一括登録キャンセル
aztool.setmap_all_set_keydown_cancel = function() {
        // 一括登録終了
        aztool.setmap_stat = 0;
        $("#setmap_info").html("");
        // キーダウンイベント削除
        document.body.removeEventListener("keydown", aztool.setmap_all_set_keydown, false);
        // ボタンの文字と色を更新
        aztool.setmap_key_string_update();
};

// レイヤー設定
aztool.setmap_layer_set = function() {
    // レイヤー編集用のデータ用意
    aztool.setmap_layer_edit_data = aztool.crone(aztool.setting_json_data.layers);
    // 選択中のレイヤー
    aztool.setmap_layer_edit_select = "";
    // デフォルトのレイヤー
    aztool.setmap_layer_edit_default = aztool.setting_json_data.default_layer;
    // レイヤーリストを更新
    aztool.setmap_layer_list_update();
    // レイヤーモーダルを開く
    aztool.setmap_layer_mdl.open();

};

// レイヤーリストを更新
aztool.setmap_layer_list_update = function() {
    let d, k, h = "", s;
    // $("#layer_list").destroy();
    // リストアイテムを生成しなおす
    for (k in aztool.setmap_layer_edit_data) {
        s = k.split("_");
        d = (aztool.setmap_layer_edit_default == parseInt(s[1]))? " <font style='color: #a2a2a2;font-size: 12px;margin: 5px;'>デフォルト</font>": "";
        h += "<li id='list_"+k+"'>" + aztool.setmap_layer_edit_data[k].name + d + "</li>";
    }
    $("#layer_list").html(h);
    // アイテムクリックイベント登録
    $("#layer_list li").click(aztool.setmap_layer_item_click);
    // 選択状態の確認
    if (aztool.setmap_layer_edit_select) {
        // 選択してるアイテムがあれば選択状態にしてフォームも表示
        aztool.setmap_layer_item_click({"target": {"id": "list_" + aztool.setmap_layer_edit_select}});
    } else {
        // 選択してるアイテムが無ければ編集フォーム非表示
        $("#layer_edit_form").hide();

    }
};

// レイヤーアイテムクリック
aztool.setmap_layer_item_click = function(e) {
    let tid = (e.target.id)? e.target.id: e.currentTarget.id;
    let s = tid.split("_");
    let lid = s[1] + "_" + s[2];
    // 選択レイヤーにキーを設定(既に選択中のレイヤーなら選択を外す)
    aztool.setmap_layer_edit_select = (aztool.setmap_layer_edit_select == lid)? "": lid;
    // アイテムの色を変える
    let k;
    for (k in aztool.setmap_layer_edit_data) {
        if (k == aztool.setmap_layer_edit_select) {
            $("#list_" + k).css({"background-color": "#aed4ff"});
        } else {
            $("#list_" + k).css({"background-color": "#e1efff"});
        }
    }
    // フォームに内容を入れる
    if (aztool.setmap_layer_edit_select == "") {
        // 選択が無ければフォーム表示
        $("#layer_edit_form").hide();
    } else {
        $("#layer_num_edit").val(s[2]);
        k = (aztool.setmap_layer_edit_select)? aztool.setmap_layer_edit_data[aztool.setmap_layer_edit_select].name: "";
        $("#layer_name_edit").val(k);
        $("#layer_edit_form_info").html("");
        // フォーム表示
        $("#layer_edit_form").show();
    }

};

// レイヤー追加
aztool.setmap_layer_add_btn_click = function() {
    let k, m = -1;
    // 空いてるレイヤ番号を探す
    for (k=0; k<100; k++) {
        if (!aztool.setmap_layer_edit_data["layer_"+k]) {
            m = k;
            break;
        }
    }
    // 空いてるレイヤーが無ければ追加しない
    if (m < 0) return;
    // 空のレイヤーを追加
    aztool.setmap_layer_edit_data["layer_" + m] = {"name": "レイヤー" + m, "keys": {}};
    // レイヤーリストを更新
    aztool.setmap_layer_list_update();
    // セレクトレイヤーを追加したレイヤーにする
    aztool.setmap_layer_item_click({"target": {"id": "list_layer_" + m}});
};

// レイヤー削除
aztool.setmap_layer_delete_btn_click = function() {
    if (!aztool.setmap_layer_edit_select) return; // 選択レイヤーが無い
    // レイヤーが1つしかない場合は消さない
    if (Object.keys(aztool.setmap_layer_edit_data).length <= 1) return;
    // デフォルトレイヤーかどうか
    let dlf = (aztool.setmap_layer_edit_select == "layer_" + aztool.setmap_layer_edit_default);
    // レイヤーの削除
    delete aztool.setmap_layer_edit_data[aztool.setmap_layer_edit_select];
    aztool.setmap_layer_edit_select = "";
    // デフォルトレイヤーの削除だった場合1番目をデフォルトレイヤーにする
    let k, s;
    for (k in aztool.setmap_layer_edit_data) {
        s = k.split("_");
        aztool.setmap_layer_edit_default = parseInt(s[1]);
        break;
    }
    // リストの表示を更新
    aztool.setmap_layer_list_update();
};

// 選択中のレイヤーをデフォルトのレイヤーにする
aztool.setmap_layer_default_layer_set = function() {
    // 選択中のレイヤーをデフォルトレイヤーにする
    let s = aztool.setmap_layer_edit_select.split("_");
    aztool.setmap_layer_edit_default = parseInt(s[1]);
    // 選択レイヤーをクリア
    aztool.setmap_layer_edit_select = "";
    // リストの表示を更新
    aztool.setmap_layer_list_update();
};

// レイヤーの情報を反映をクリック
aztool.setmap_layer_form_save = function() {
    // レイヤーの番号が数値かどうか
    let lno = $("#layer_num_edit").val();
    if (!aztool.is_num(lno)) {
        $("#layer_edit_form_info").html("レイヤー番号は数字を入力して下さい。");
        return;
    }
    // レイヤー番号の最大値は99
    let lno_num = parseInt(lno);
    if (lno_num > 99) {
        $("#layer_edit_form_info").html("レイヤー番号は99以下を入力して下さい。");
        return;
    }
    // レイヤー名の文字列は12文字まで
    let lname = $("#layer_name_edit").val();
    if (lname.length > 12) {
        $("#layer_edit_form_info").html("レイヤー名が長すぎます。");
        return;
    }
    // レイヤーの番号が変わっていた場合
    let lid_new = "layer_" + lno;
    if (aztool.setmap_layer_edit_select != lid_new) {
        // 既に存在する番号だった
        if (aztool.setmap_layer_edit_data[lid_new]) {
            $("#layer_edit_form_info").html("既に使用されているレイヤー番号です。");
            return;
        }
        // 新しいレイヤーIDにデータをコピーして古いレイヤーIDを削除
        aztool.setmap_layer_edit_data[lid_new] = aztool.setmap_layer_edit_data[aztool.setmap_layer_edit_select];
        delete aztool.setmap_layer_edit_data[aztool.setmap_layer_edit_select];
        // デフォルトのレイヤーだった場合デフォルトレイヤの番号も変更
        if (aztool.setmap_layer_edit_select == "layer_" + aztool.setmap_layer_edit_default) {
            aztool.setmap_layer_edit_default = lno_num;
        }
    }
    // レイヤー名を更新
    aztool.setmap_layer_edit_data[lid_new].name = lname;
    // 選択レイヤーをクリア
    aztool.setmap_layer_edit_select = "";
    // リストの表示を更新
    aztool.setmap_layer_list_update();
};

// レイヤー設定閉じる動作 end_type = 1.保存 0.キャンセル
aztool.setmap_layer_edit_close = function(end_type) {
    if (end_type) {
        // 保存なら編集配列を元データに反映
        aztool.setting_json_data.layers = aztool.setmap_layer_edit_data; // レイヤーデータ
        aztool.setting_json_data.default_layer = aztool.setmap_layer_edit_default; // デフォルトレイヤー
    }
    // レイヤーモーダルを閉じる
    aztool.setmap_layer_mdl.close();
    // キー配列の表示を描きなおす
    aztool.view_setmap();
};

// 設定を保存して再起動
aztool.setmap_save = function() {
    // 設定を保存
    $("#main_box").html("保存中<br><div id='console_div'></div>");
    aztool.setting_json_save(function(stat) {
        // 保存失敗
        if (stat != 0) {
            aztool.view_setmap(); // キー設定ページ再表示
            $("#setmap_info").html("設定JSONの保存に失敗しました。");
            return;
        }
        $("#main_box").html("保存完了。再起動します。");
        // 2秒ほど待ってからキーボード再起動
        setTimeout(function() {
            aztool.keyboard_restart(0); // キーボードを再起動
        }, 2000);
    });
};
