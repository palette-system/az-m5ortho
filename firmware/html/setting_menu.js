if (!window.mst) window.mst = {};

// ソフトウェアキー設定
mst.view_softkey_setting = function() {
    var i, k, n;
    var ly, ky, vc;
    var bs = "style='width: 320px;height: 70px; background: #78d3df; font-size: 35px; display: inline-block;'";
    var ds = "style='width: 80px;height: 70px; background: #78d3df; font-size: 35px; display: inline-block;'";
    var s = "";
    s += "<b style='font-size: 30px;'>ソフトウェアキー</b><br><br><br><br>";
    vc = 0;
    for (k in mst.setting_data.soft_layers) {
        ly = mst.setting_data.soft_layers[k];
        // s += ly.name + "<br>";
        for (i in ly.keys) {
            ky = ly.keys[i];
            n = (ky.name)? ky.name: i;
            s += "<a href='#' class='update_button' "+bs+" onClick='javascript:mst.soft_key_click(\""+k+"\",\""+i+"\");return false;'>"+n+"</a>　";
            s += "<a href='#' class='update_button' "+ds+" onClick='javascript:mst.soft_key_delete(\""+k+"\",\""+i+"\");return false;'>削除</a>";
            s += "<br><br>";
            vc++;
        }
    }
    s += "<div id='soft_key_add_div' style='text-align: right;'>";
    if (vc < 6) {
        s += "<a href='#' class='update_button' onClick='javascript:mst.add_soft_key(0, -1);return false;'>キー追加</a><br><br>";
    }
    s += "</div>";
    s += "<br><br><br><br><br><br>";
    s += "<center><div id='softkey_btn'>";
    s += "<a href='#' class='button' onClick='javascript:mst.view_detail_setting();return false;'>完了</a>";
    s += "</div></center>";
    mst.edit_soft_layer = -1;
    mst.key_edit_kid = -1;
    mst.key_edit_type = 1;
    set_html("setting_box", s);
    set_html("info_box", "");
    mst.view_box(["info_box", "setting_box"]);
};

// 画像上のキーをクリックした時のイベント
mst.soft_key_click = function(layer_key, key_key) {
    // キーの設定画面表示
    mst.key_edit_type = 1;
    mst.edit_soft_layer = mst.str_id2num(layer_key);
    mst.key_edit_kid = mst.str_id2num(key_key);
    mst.key_edit_data = obj_clone(mst.setting_data.soft_layers[layer_key].keys[key_key]);
    mst.view_key_setting(mst.key_edit_kid);
    set_html("info_box", "");
};


// ソフトウェアキー追加
mst.add_soft_key = function(layer_id, key_id) {
    var i;
    var ly;
    // レイヤーが無ければ作成
    if (!mst.setting_data.soft_layers["layer_" + layer_id]) {
        mst.setting_data.soft_layers["layer_" + layer_id] = {"name": "レイヤー"+layer_id, "keys": {}};
    }
    ly = mst.setting_data.soft_layers["layer_" + layer_id];
    // キーIDの指定があれば空いていれば空データを作って編集
    if (key_id >= 0 && !ly.keys["key_"+key_id]) {
        ly.keys["key_"+key_id] = {
            "name": "button" + key_id, // 表示名
            "press": {"action_type": 0} // 処理なし
        };
        mst.soft_key_click("layer_"+layer_id, "key_"+key_id);
        return;
    }
    // キーIDの指定が無いか、設定済みだったら空いている一番低いIDに空データを作って編集
    i = 0;
    while (true) {
        if (ly.keys["key_"+i]) {
            i++;
            continue;
        }
        ly.keys["key_"+i] = {
            "name": "button" + i, // 表示名
            "press": {"action_type": 0} // 処理なし
        };
        mst.soft_key_click("layer_"+layer_id, "key_"+i);
        break;
    }
};

// ソフトウェアキー削除
mst.soft_key_delete = function(layer_key, key_key) {
    var i, k, r = {};
    i = 0;
    for (k in mst.setting_data.soft_layers[layer_key].keys) {
        if (k == key_key) continue;
        r["key_"+i] = mst.setting_data.soft_layers[layer_key].keys[k];
        i++;
    }
    mst.setting_data.soft_layers[layer_key].keys = r;
    mst.view_softkey_setting();
};