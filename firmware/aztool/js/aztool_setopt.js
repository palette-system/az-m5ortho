// I2Cオプション設定画面

if (!window.aztool) aztool = {};


// 設定を変更したか
aztool.setopt_changed = false;

// オプション設定メインページ表示
aztool.view_setopt = function() {
    let h = `
    <div  style="width: 1400px;">
    <table style="min-height: 700px;">
    <tr><td valign="top" style="width: 200px; background-color: #f8f8f8; padding: 10px; overflow-y: scroll;">
    <a class='leftmenu-button' onClick='javascript:aztool.addopt_start(\"main_box\");'>オプション追加</a><br>
    
    <div id="back_btn_box"></div>

    </td><td valign="top" style="padding: 20px;">
    <div id="opt_list"></div>
    <div id="setopt_info"></div>
    </td></tr></table>
    `;
    $("#main_box").html(h);
    if (aztool.setopt_changed) {
        $("#back_btn_box").html('<a class="leftmenu-button" onClick="javascript:aztool.setopt_save_back();">保存して戻る</a>');
    } else {
        $("#back_btn_box").html('<a class="leftmenu-button" onClick="javascript:aztool.view_top_menu();">戻る</a>');
    }
    // オプション設定が無ければ空データを作る
    if (!aztool.setting_json_data.i2c_option) aztool.setting_json_data.i2c_option = [];
    // オプションのリストを表示
    aztool.setopt_optlist_view();

};

// 登録されているオプションのリストを表示する
aztool.setopt_optlist_view = function() {
    let c, i, j, o, n;
    let h = "";
    for (i in aztool.setting_json_data.i2c_option) {
        o = aztool.setting_json_data.i2c_option[i];
        c = (o.enable)? "#f0fff2": "#ededed";
        n = (o.enable)? "0": "1";
        console.log(o);
        h += "<div style='width: 400px; background-color: "+c+"; border: solid 2px #aaa;display: inline-block; margin: 10px; padding: 20px;cursor: pointer;'";
        h += " onClick='javascript:aztool.setopt_opt_enable(\""+o.id+"\", "+n+");' >";
        h += "<table width='400'>";
        h += "<tr><td rowspan='2' width='200'><div id='ov_"+o.id+"' style='width: 170px; height: 200px; border: solid 1px #888'></div></td><td valign='top'>";
        h += "ID: " + o.id + "<br>";
        h += (o.enable)? "設定：有効<br>": "設定：無効<br>";
        h += "I2C： ";
        for (j in o.ioxp) {
            if (j > 0) h += " ,";
            h += aztool.to_hex(o.ioxp[j].addr);
        }
        h += "<br>";
        h += "map_start: " + o.map_start + "<br>";
        h += "キー数： " + o.map.length;
        h += "</td></tr>";
        h += "<tr><td valign='bottom' align='right'>";
        h += "<a class='exec-button' onClick='javascript:aztool.setopt_opt_remove("+o.id+");'>削除</a>";
        h += "</td></tr>";
        h += "</table>";
        h += "</div>";
    }
    $("#opt_list").html(h);
    for (i in aztool.setting_json_data.i2c_option) {
        o = aztool.setting_json_data.i2c_option[i];
        aztool.kle_view(aztool.i2c_option_data["o"+o.id], "#ov_"+o.id, false, 20, "sw_" + o.id + "_");
    }
};

// オプションの有効無効を切り替える
aztool.setopt_opt_enable = function(option_id, settype) {
    let i;
    console.log(option_id);
    for (i in aztool.setting_json_data.i2c_option) {
        console.log("ch: "+aztool.setting_json_data.i2c_option[i].id);
        if (aztool.setting_json_data.i2c_option[i].id == option_id) {
            aztool.setting_json_data.i2c_option[i].enable = settype;
            // 変更したよフラグを立てる
            aztool.setopt_changed = true;
        }
    }
    // オプションリスト表示
    aztool.view_setopt();
};

// オプションを削除
aztool.setopt_opt_remove = function(option_id) {
    $("#main_box").html("オプション "+option_id+" 削除中");
    // オプションのKLEのファイルを削除
    webhid.file_remove("/o" + option_id, function(stat) {
        let i, n, o;
        if (stat == 0 || stat == 1) { // 削除成功 or 元から存在しない
            // オプションリストから該当のリストを削除
            n = [];
            for (i in aztool.setting_json_data.i2c_option) {
                o = aztool.setting_json_data.i2c_option[i];
                if (o.id == option_id) { // 削除対象のオプション
                    aztool.setopt_keyset_remove(o); // キー設定を削除
                    aztool.setopt_layout_remove(o.id); // レイアウト情報削除
                    continue; // 削除対象のオプションは新しいリストに追加しない
                }
                n.push(o); // 対象外のオプションは新しいリストに追加
            }
            aztool.setting_json_data.i2c_option = n; // オプションリストを新しいリストにする
            // 変更したよフラグを立てる
            aztool.setopt_changed = true;
        }
        // オプションリスト表示
        aztool.view_setopt();
    });
};

// キー設定から対象のキー設定を削除
aztool.setopt_keyset_remove = function(option_set) {
    let i, k, l, kl, ms, me;
    let remove_list;
    // キー入力設定で削除する対象のリストを作成
    ms = option_set.map_start; // キー設定の開始ID
    me = ms + option_set.map.length; // キー設定の終わりのID
    remove_list = [];
    for (i=ms; i<me; i++) {
        remove_list.push("key_" + i);
    }
    // キー設定から該当のキー設定を削除
    for (l in aztool.setting_json_data.layers) { // 全レイヤー
        kl = Object.keys(aztool.setting_json_data.layers[l].keys);
        for (k in kl) { // 全キー
            if (remove_list.indexOf(kl[k]) >= 0) { // 削除リスト内にあるキー
                delete aztool.setting_json_data.layers[l].keys[ kl[k] ]; // 設定削除
            }
        }
    }
};

// レイアウトに表示位置データが残っていれば削除
aztool.setopt_layout_remove = function(option_id) {
    if (aztool.setting_json_data.layout && 
        aztool.setting_json_data.layout.position &&
        aztool.setting_json_data.layout.position["o_"+option_id]) {
            delete aztool.setting_json_data.layout.position["o_"+option_id];
    }
};

// 設定をファイルに保存してタイトルメニューに戻る
aztool.setopt_save_back = function() {
    // 設定を保存
    $("#main_box").html("保存中<br><div id='console_div'></div>");
    aztool.setting_json_save(function(stat) {
        // 保存失敗
        console.log("setting_json_save: " + stat);
        if (stat != 0) {
            aztool.view_setopt(); // オプション設定ページ再表示
            // $("#setmap_info").html("設定JSONの保存に失敗しました。");
            return;
        }
        aztool.setopt_changed = false;
        // トップメニューに戻る
        aztool.view_top_menu();
    });

};
