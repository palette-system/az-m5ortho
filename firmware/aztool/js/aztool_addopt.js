// オプション追加ツール


if (!window.aztool) aztool = {};

// オプション追加の入力データを保持する所
aztool.option_add = {};

// IOエキスパンダ設定のデフォルト設定
aztool.ioxp_default = {"addr": 32, "row":[], "col":[], "direct":[0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15]};

// キーの色
aztool.key_color = "#fff";
// 選択中のキーのカラー
aztool.select_key_color = "#ffb5b5";
// 設定が終わったキーのカラー
aztool.enable_key_color = "#b6fbdb";

// オプションを追加しているかどうかのフラグ
aztool.option_add_added_flag = false;

// オプション追加ライブラリ用初期化処理
aztool.addopt_start = function(div_id) {
    aztool.addopt_div_id = div_id;
    // aztool オプション追加ライブラリ用のHTML追加
    aztool.addopt_init_html();
    // モーダル登録
    aztool.klemdl = $('[data-remodal-id=kletxt_modal]').remodal();
};

aztool.addopt_init_html = function() {
    let html = `
    <div  style="max-width: 1200px;">
    <table><tr><td valign="top" align="center" style="width: 270px; background-color: #f8f8f8; padding: 20px 0;">
    <table>
    <tr><td align="center"><div id="stepbox_1" class="option_step">レイアウト設定</div></td></tr>
    <tr><td align="center"><div class="triangle_down"></div></td></tr>
    <tr><td align="center"><div id="stepbox_2" class="option_step">エキスパンダ設定</div></td></tr>
    <tr><td align="center"><div class="triangle_down"></div></td></tr>
    <tr><td align="center"><div id="stepbox_3" class="option_step">エキスパンダ確認</div></td></tr>
    <tr><td align="center"><div class="triangle_down"></div></td></tr>
    <tr><td align="center"><div id="stepbox_4" class="option_step">ボタンのマッピング</div></td></tr>
    <tr><td align="center"><div class="triangle_down"></div></td></tr>
    <tr><td align="center"><div id="stepbox_5" class="option_step">動作確認</div></td></tr>
    <tr><td align="center"><div class="triangle_down"></div></td></tr>
    <tr><td align="center"><div id="stepbox_6" class="option_step">保存</div></td></tr>
    <tr><td align="center"><div class="triangle_down"></div></td></tr>
    <tr><td align="center"><div id="stepbox_7" class="option_step">完了</div></td></tr>
    </table>

    </td><td valign="top" style="padding: 20px;">
    <div id='kle_view_box' style="border: solid 1px #888; width: 800px;height: 400px;overflow: hidden; background-color: #888;"></div>
    <div style="text-align: right; width: 800px;">
    <b style="font-size: 15px;">－</b>
    <input id="coef_input" type="range" name="speed" min="20" max="60" onChange="javascript:aztool.kle_view(aztool.option_add.kle, '#kle_view_box');">
    <b style="font-size: 15px;">＋</b>
    </div>
    <div id="option_setting_form">
    </div>


    </td></tr></table>

    </div>`;
    $("#" + aztool.addopt_div_id).html("");
    webhid.set_eztool_mode(1, function() { // eztoolモードON
        $("#" + aztool.addopt_div_id).html(html);
        aztool.step_max = 7;
        aztool.step_index = 0;
        aztool.option_add = {
            "id": "000000", // オプションごとのユニークなID
            "type": 1, // オプションのタイプ 1=IOエキスパンダ
            "enable": 1, // 有効かどうか 1=有効
            "kle": "", // KLEのJSONデータ
            "ioxp": [], // 繋げているIOエキスパンダの設定
            "map_start": 0, // キー設定の番号いくつからがこのオプションのキー設定か
            "map": [] // キーと読み込んだデータとのマッピング設定
        };
        aztool.option_add_layout_view();
    });
};

// レイアウト設定画面表示
aztool.option_add_layout_view = function() {
    let h = `
        <br>    
        <textarea id="kle_json_txt" style="width: 800px; height: 150px;"
        placeholder="KLEのレイアウトJSONを張り付けて下さい。"
        onChange="javascript:aztool.option_add_kle_change();"></textarea>
        <br><br>
        <div style="text-align: right; width: 800px;">
        <div id="can_btn" style="display: inline-block"></div>
        <div id="kle_json_btn" style="display: none;">　<a class="exec-button" onClick="javascript:aztool.option_add_expanda_view();">次へ</a></div>
        </div>`;
    $("#option_setting_form").html(h);
    $("#kle_json_txt").html(aztool.option_add.kle);
    // 既にオプションを追加済みであればキャンセルしたら完了ページに戻る
    if (aztool.option_add_added_flag) {
        $("#can_btn").html("<a class='cancel-button' onClick='javascript:aztool.option_add_complate();'>キャンセル</a>");
    } else {
        // オプション追加していなければトップメニューに戻る
        $("#can_btn").html("<a class='cancel-button' onClick='javascript:aztool.option_add_end(aztool.view_top_menu);'>キャンセル</a>");
    }
    aztool.update_step_box(1);
    aztool.option_add_kle_change();
};

// オプション追加終了
aztool.option_add_end = function(cb_func) {
    webhid.set_eztool_mode(0, function() { // eztoolモードOFF
        cb_func();
    });
}

aztool.option_add_kle_change = function() {
    let json_txt = $('#kle_json_txt').val();
    let r = aztool.kle_view(json_txt, '#kle_view_box', true);
    if (r) {
        aztool.option_add.kle = json_txt;
        $("#kle_json_btn").css({"display": "inline-block"});
    } else {
        $("#kle_json_btn").css({"display": "none"});
    }

};


// エキスパンダ設定画面表示
aztool.option_add_expanda_view = function() {
    let h = `
        ■ 使用するIOエキスパンダ<br>
        <div id="expanda_list" style="height: 120px; margin: 10px 0;"></div>
        <a class="exec-button" onClick="javascript:aztool.option_add_ioxp_add();" style="width: 300px;">IOエキスパンダを追加する</a>
        <br><br>
        <div style="text-align: right; width: 800px;">
        <a class="cancel-button" onClick="javascript:aztool.option_add_layout_view();">戻る</a>　
        <a class="exec-button" onClick="javascript:aztool.option_add_ioxp_check_view();">次へ</a>
        </div>`;
    $("#option_setting_form").html(h);
    aztool.option_add_ioxp_update();
    aztool.update_step_box(2);
};

// IOエキスパンダの追加
aztool.option_add_ioxp_add = function() {
    // 追加モーダルを表示
    pinstp.open(aztool.ioxp_default, {},
    function(btn, xdata) {
        // データチェック
        let i;
        if (btn != 1) return ""; // 決定以外は問題なし
        // 既に設定してあるアドレスの場合はエラー
        for (i in aztool.option_add.ioxp) {
            if (aztool.option_add.ioxp[i].addr == xdata.addr) return "既に設定されているアドレスです。";
        }
        return ""; // 問題無ければ空文字
    },function(btn, xdata) {
        // キャンセルが押された場合は何もしない
        if (btn == 0) return;
        aztool.option_add.ioxp.push(xdata); // 設定内容をIOエキスパンダリストに追加
        aztool.option_add_ioxp_sort(); // IOエキスパンダをアドレス順にソート
        aztool.option_add_ioxp_update(); // 表示を更新
    });
};

// ioエキスパンダの配列をソート
aztool.option_add_ioxp_sort = function() {
    aztool.option_add.ioxp.sort(function(a,b) {
        return (a.addr > b.addr)? 1: -1;
    });
};

// IOエキスパンダリストの表示を更新
aztool.option_add_ioxp_update = function() {
    let i, h = "", x;
    for (i in aztool.option_add.ioxp) {
        x = aztool.option_add.ioxp[i];
        h += "<div class='icon_ioxp' onClick='javascript:aztool.option_add_ioxp_click("+i+");'>";
        h += "<img src='./img/icon_ic.png'><br>";
        h += "0x" + x.addr.toString(16);
        h += "</div>";
    }
    // IOエキスパンダが１積内場合は追加して下さいメッセージ表示
    if (!h) {
        h = "<font style='color: #888;font-size: 12px;'>「IOエキスパンダを追加する」を押してIOエキスパンダを追加して下さい。</font>";
    }
    $("#expanda_list").html(h);
};

// IOエキスパンダアイコンをクリック
aztool.option_add_ioxp_click = function(ioxp_num) {
    // 編集中のIOエキスパンダの番号を控える
    aztool.option_add_ioxp_select = ioxp_num;
    pinstp.open(
        aztool.option_add.ioxp[ioxp_num],
        {"btn_delete": true},
        function(btn, xdata) {
            // データチェック
            let i;
            if (btn != 1) return ""; // 決定以外は問題なし
            // 既に設定してあるアドレスの場合はエラー
            for (i in aztool.option_add.ioxp) {
                if (i == aztool.option_add_ioxp_select) continue; // 自分は無視
                if (aztool.option_add.ioxp[i].addr == xdata.addr) return "既に設定されているアドレスです。";
            }
            return ""; // 問題無ければ空文字
        },function(btn, xdata) {
            // キャンセルが押された場合は何もしない
            if (btn == 0) return;
            if (btn == 1) { // 決定
                aztool.option_add.ioxp[aztool.option_add_ioxp_select] = xdata; // 設定データを更新
            }
            if (btn == 2) { // 削除
                aztool.option_add.ioxp.splice(aztool.option_add_ioxp_select, 1);
            }
            aztool.option_add_ioxp_sort(); // IOエキスパンダをアドレス順にソート
            aztool.option_add_ioxp_update(); // 表示を更新
        });
};

// IOエキスパンダ確認
aztool.option_add_ioxp_check_view = function() {
    let h = `
        キースイッチを押すとDataが緑色になる事を確認して下さい。<br>
        <div id="ioxp_check_info" style="height: 120px;"></div>
        <div style="text-align: right; width: 800px;">
        <a class="cancel-button" onClick="javascript:aztool.option_add_expanda_view();">戻る</a>
        <div id="ioxp_check_comp_btn" style="display: none;">　<a class="exec-button" onClick="javascript:aztool.option_add_btnmap_view();">次へ</a></div>
        </div>`;
    $("#option_setting_form").html(h);
    aztool.update_step_box(3);
    setTimeout(function() {
        aztool.ioxp_check_index = 0;
        aztool.ioxp_data = [];
        aztool.ioxp_lasttime = webhid.millis();
        aztool.ioxp_check_update_func = aztool.option_add_ioxp_check_update; // 画面更新用関数
        aztool.option_add_ioxp_health_check(3); // 入力取得が止まって無いか監視
        aztool.option_add_ioxp_check_exec(3); // 入力取得開始
    }, 1000);
};

// IOエキスパンダの接続を確認する
aztool.option_add_ioxp_check_exec = function(step_no) {
    let x = aztool.option_add.ioxp[aztool.ioxp_check_index];
    webhid.get_ioxp_key(x.addr - 32, x.row, function(d) {
        aztool.ioxp_lasttime = webhid.millis();
        aztool.ioxp_data[aztool.ioxp_check_index] = d;
        aztool.ioxp_check_index++;
        if (aztool.ioxp_check_index >= aztool.option_add.ioxp.length) aztool.ioxp_check_index = 0;
        let t = 10;
        if (aztool.ioxp_check_index == 0) {
            // 全エキスパンダの情報を取得したら
            aztool.ioxp_check_update_func(); // 画面の情報を更新
            t = 10; // ちょっと待つ
        }
        if (aztool.step_index != step_no) return; // 別のステップに移動したらチェックを止める
        setTimeout(function() { aztool.option_add_ioxp_check_exec(step_no); }, t); // 次のIOエキスパンダの情報を取得
    });
};

// IOエキスパンダチェックが止まって無いかチェック
aztool.option_add_ioxp_health_check = function(step_no) {
    let t = webhid.millis();
    // 最後に取得してから2秒以上経ってたら情報取得をもっかい実行
    if ((aztool.ioxp_lasttime + 1000) < t ) {
        aztool.ioxp_check_index = 0;
        aztool.ioxp_lasttime = webhid.millis();
        aztool.option_add_ioxp_check_exec(step_no); // 入力取得
    }
    if (aztool.step_index != step_no) return; // 別のステップに移動したらチェックを止める
    setTimeout(function () { aztool.option_add_ioxp_health_check(step_no); }, 1000); // 次のヘルスチェックを実行
};

// IOエキスパンダの入力データからキー入力配列を取得する
aztool.ioxp_data_to_array = function(ioxp_setting, get_data) {
    let r = ioxp_setting.row.length; // rowの数
    let a, b, i, j, m, s, x;
    let ma, mb; // Aポート用マスク、Bポート用マスク
    let d = []; // 返すデータ
    x = 0; // ダイレクトがあればダイレクトのマスク
    for (i=0; i<ioxp_setting.direct.length; i++) {
        x |= 0x01 << ioxp_setting.direct[i];
    }
    ma = x & 0xff;
    mb = (x >> 8) & 0xff;
    if (r) { // マトリックス取得の場合
        for (i=0; i<r; i++) {
            b = get_data[3 + (i * 2)] & ~mb; // ポートB
            a = get_data[4 + (i * 2)] & ~ma; // ポートA
            // ポートAを配列に
            for (j=0; j<8; j++) {
                m = 0x01 << j;
                s = (a & m)? 1: 0;
                d.push(s);
            }
            // ポートBを配列に
            for (j=0; j<8; j++) {
                m = 0x01 << j;
                s = (b & m)? 1: 0;
                d.push(s);
            }
        }
    }
    // ダイレクトがあればダイレクトの分も配列に入れる
    if (ioxp_setting.direct.length) {
        b = get_data[3] & mb; // ポートB
        a = get_data[4] & ma; // ポートA
        // ポートAを配列に
        for (j=0; j<8; j++) {
            m = 0x01 << j;
            s = (a & m)? 1: 0;
            d.push(s);
        }
        // ポートBを配列に
        for (j=0; j<8; j++) {
            m = 0x01 << j;
            s = (b & m)? 1: 0;
            d.push(s);
        }
    }
    return d;
};

// 画面の情報を更新
aztool.option_add_ioxp_check_update = function() {
    let m = aztool.option_add_key_stat_update();
    let ok_flag = true;
    for (i in m.status) {
        if (m.status[i] != 0) ok_flag = false;
    }

    if (ok_flag) {
        $("#ioxp_check_comp_btn").css({"display": "inline-block"});
    } else {
        $("#ioxp_check_comp_btn").css({"display": "none"});
    }
};

// ボタンのマッピング表示
aztool.option_add_btnmap_view = function() {
    let h = `
        <div id="btnmap_info">赤色になったキースイッチを押して下さい。</div>
        <div id="ioxp_check_info" style="height: 120px;"></div>
        <div style="text-align: right; width: 800px;">
        <a class="cancel-button" onClick="javascript:aztool.option_add_ioxp_check_view();">戻る</a>
        <div id="switch_comp_btn" style="display: none;">　<a class="exec-button" onClick="javascript:aztool.option_add_btncheck_view();">次へ</a></div>
        </div>`;
    $("#option_setting_form").html(h);
    aztool.update_step_box(4);
    setTimeout(function(){
        aztool.switch_check_index = 0; // 今何番目のスイッチをチェックしているか
        aztool.switch_last_set = -1; // 最後に押したスイッチの番号
        aztool.ioxp_check_index = 0;
        aztool.ioxp_data = [];
        aztool.map_data = []; // マッピングしたデータ
        aztool.ioxp_lasttime = webhid.millis();
        aztool.ioxp_check_update_func = aztool.option_add_keymap_update_func; // 画面更新用関数
        aztool.option_add_ioxp_health_check(4); // 入力取得が止まって無いか監視
        aztool.option_add_ioxp_check_exec(4); // 入力取得開始
    }, 700);
};

// キーの入力状態の表示を更新
aztool.option_add_key_stat_update = function() {
    let d, i, j, m, o, r, x;
    let h = "";
    m = {"status": [], "input": []};
    h += "<table>";
    for (i in aztool.ioxp_data) {
        d = aztool.ioxp_data[i]; // 受け取った情報
        x = aztool.option_add.ioxp[i]; // IOエキスパンダ設定
        r = d[2]; // rowの数
        if (r == 0) r = 1; // row無しの場合は1つ分だけデータを取る
        h += "<tr><td>";
        h += "Addr: 0x" + x.addr.toString(16) + "　";
        m.status.push(d[1]);
        if (d[1] == 0) { 
            h += "<b style='color: #7bdf48;'>接続</b>";
        } else if (d[1] == 1) {
            h += "<b style='color: #ff9a9a;'>使用中</b>";
        } else {
            h += "<b style='color: #ff9a9a;'>ERROR</b>";
        }
        o = aztool.ioxp_data_to_array(x, d);
        // m.input.concat(o);
        h += "　Data: ";
        h += "</td><td>";
        for (j=0; j<o.length; j++) {
            m.input.push(o[j]);
            h += (o[j])? "<div class='check_on'></div>": "<div class='check_off'></div>";
            if ((j % 16) == 15) h += "<br>";
        }
        h += "</td></tr>";
    }
    h += "</table>";
    $("#ioxp_check_info").html(h);
    return m;
};


// キーの入力状態を取得
aztool.option_add_keymap_update_func = function() {
    let dat = aztool.option_add_key_stat_update(); // キーのステータス表示をする
    let i, f = -1;
    // ボタンが押されてるキーを探す(最初の１つだけ取得)
    for (i=0; i<dat.input.length; i++) {
        if (dat.input[i]) {
            f = i;
            break;
        }
    }

    // キーが押されたら次のキー
    if (f >= 0 && aztool.switch_last_set != f && aztool.switch_check_index < aztool.switch_length) {
        aztool.switch_last_set = f;
        aztool.map_data.push(f);
        aztool.switch_check_index++;
        if (aztool.switch_check_index >= aztool.switch_length) {
            $("#btnmap_info").html("マッピングが完了しました。");
            $("#switch_comp_btn").css({"display": "inline-block"});
            console.log(aztool.map_data);
            aztool.option_add.map = aztool.map_data;
        }
    }
    if (f < 0) aztool.switch_last_set = -1;
    aztool.option_add_key_color_update();
};

// ボタンの色を更新
aztool.option_add_key_color_update = function() {
    let i;
    for (i=0; i<aztool.switch_length; i++) {
        if (i == aztool.switch_check_index) {
            $("#sw_" + i).css({"background-color": aztool.select_key_color}); // 読み取り中のキー
        } else if (i < aztool.switch_check_index) {
            $("#sw_" + i).css({"background-color": aztool.enable_key_color}); // 読み取りが終わったキー
        } else {
            $("#sw_" + i).css({"background-color": aztool.key_color}); // まだ読み込んでいないキー
        }
    }
};

// ボタンの入力チェック
aztool.option_add_btncheck_view = function() {
    let h = `
        <div id="btncheck_info">キースイッチを押して該当のスイッチの色が変わるのを確認して下さい。</div>
        <div id="ioxp_check_info" style="height: 120px;"></div>
        <div style="text-align: right; width: 800px;">
        <a class="cancel-button" onClick="javascript:aztool.option_add_btnmap_view();">戻る</a>
        　<a class="exec-button" onClick="javascript:aztool.option_add_save();">次へ</a>
        </div>`;
    $("#option_setting_form").html(h);
    aztool.update_step_box(5);
    setTimeout(function(){
        aztool.ioxp_check_index = 0;
        aztool.ioxp_data = [];
        aztool.ioxp_lasttime = webhid.millis();
        aztool.ioxp_check_update_func = aztool.option_add_map_check_update; // 画面更新用関数
        aztool.option_add_ioxp_health_check(5); // 入力取得が止まって無いか監視
        aztool.option_add_ioxp_check_exec(5); // 入力取得開始
    }, 700);
};

// マッピングの入力チェック
aztool.option_add_map_check_update = function() {
    let dat = aztool.option_add_key_stat_update(); // キーのステータス表示をする
    let i, x;
    for (i in aztool.option_add.map) {
        x = aztool.option_add.map[i];
        if (dat.input[x]) {
            $("#sw_" + i).css({"background-color": aztool.enable_key_color}); // 押しているキー
        } else {
            $("#sw_" + i).css({"background-color": aztool.key_color}); // 押していないキー
        }
    }
};

// キー設定番号で使われていない番号を探す(128ごと)
aztool.get_map_start_next = function() {
    let r = 128;
    // 他のオプションが無ければ128から
    if (!aztool.setting_json_data.i2c_option) return r;
    let i;
    for (i in aztool.setting_json_data.i2c_option) {
        while (aztool.setting_json_data.i2c_option[i].map_start >= r) r += 128;
    }
    return r;
};

// オプション保存
aztool.option_add_save = function() {
    // 保存中画面表示
    let h = `
        <div id="btncheck_info">設定内容を保存中</div>
        <div id="console_div" style="height: 120px;"></div>
        <div style="text-align: right; width: 800px;">
        <div id="switch_resave_btn" style="display: none;">　<a class="exec-button" onClick="javascript:aztool.option_add_save();">保存再実行</a></div>
        </div>`;
    $("#option_setting_form").html(h);
    aztool.update_step_box(6);
    // 保存前のデータ準備
    aztool.option_add.id = aztool.random_num(6); // オプションごとのユニークなID
    // KLE データをファイルに出力
    webhid.save_file(
        "/o" + aztool.option_add.id, // 保存ファイルパス
        aztool.option_add.kle, // 保存データ
        function (stat) {
            // 保存失敗
            if (stat != 0) {
                $("#switch_resave_btn").css({"display": "inline-block"});
                return;
            }
            // 保存成功したら配列に追加
            let set_data = {
                "id": aztool.option_add.id, // オプションごとのユニークなID
                "type": aztool.option_add.type, // オプションのタイプ
                "enable": 1, // 有効かどうか
                "ioxp": aztool.option_add.ioxp, // 繋げているIOエキスパンダの設定
                "map_start": aztool.get_map_start_next(), // キー設定番号の開始位置
                "map": aztool.option_add.map // キーと読み込んだデータとのマッピング設定
            };
            // オプション配列が無ければオプション配列作成
            if (!aztool.setting_json_data.i2c_option) aztool.setting_json_data.i2c_option = [];
            aztool.setting_json_data.i2c_option.push(set_data); // オプションにデータを追加
            // 設定JSON保存
            aztool.setting_json_save(function(stat) {
                // 保存失敗
                if (stat != 0) {
                    $("#switch_resave_btn").css({"display": "inline-block"});
                    return;
                }
                // 保存成功したら完了ページへ移動(一応画面が確認できるよう2秒くらい待ってから)
                setTimeout(aztool.option_add_complate, 2000);
            });
        }
    );
};

// オプション追加完了ページ
aztool.option_add_complate = function() {
    let h = `
        <div id="btncheck_info">オプションを追加しました。</div>
        <div id="ioxp_complate_info" style="height: 120px;"></div>
        <div style="text-align: right; width: 800px;">
        <a class="exec-button" onClick="javascript:aztool.option_add_restart();" style="width: 280px;">再起動してオプションを反映する</a>
        　<a class="exec-button" onClick="javascript:aztool.addopt_start(aztool.addopt_div_id);" style="width: 280px;">続けてオプションを追加する</a>
        </div>`;
    $("#option_setting_form").html(h);
    aztool.update_step_box(7);
    aztool.option_add_added_flag = true;
};

// 再起動してオプションを反映
aztool.option_add_restart = function () {
    aztool.option_add_added_flag = false; // オプション追加したよフラグリセット
    aztool.keyboard_restart(0); // キーボードを再起動
};

// kle JSONからキー配列を表示
aztool.kle_view = function(json_text, view_id, auto_resize, set_coef, set_prefix) {
    try {
      var json_data = aztool.kle_json_parse(json_text);
      aztool.serial_data = $serial.deserialize(json_data);
      if (!aztool.serial_data.keys.length) return false;
      var str="";
      let coef = parseInt($("#coef_input").val());
      if (auto_resize) coef = 60; // 最大は60
      if (set_coef) coef = set_coef;
      if (!set_prefix) set_prefix = "sw_";
      // 最大topとleftを取得
      let mtop = 0;
      let mleft = 0;
      let sx = 0, sy = 0;
      while (sx < 10 || sy < 10) {
        aztool.serial_data.keys.forEach(key => {
            if (mleft < (key.x + key.width)) mleft = (key.x + key.width);
            if (mtop < (key.y + key.height)) mtop = (key.y + key.height);
        });
        sx = Math.round(($(view_id).width() - (mleft * coef)) / 2);
        sy = Math.round(($(view_id).height() - (mtop * coef)) / 2);
        if (!auto_resize) break; // オートリサイズ設定が無ければサイズ調整しない
        if (sx < 10 || sy < 10) {
            if (coef <= 20) break; // 最小は5
            coef--; // 画面に納まりきらなければサイズを少し小さくしてもっかい計算
            $("#coef_input").val(coef)
        }
      }
      // HTMLを生成
      aztool.switch_length = 0;
      str += '<div style="position: relative;">\n';
      aztool.serial_data.keys.forEach(key => {
        if (key.rotation_angle) console.log(key);
        let wrapperStyle = ''
        wrapperStyle += 'user-select: none; position: absolute;'
        wrapperStyle += `transform: rotate(${key.rotation_angle}deg);`
        wrapperStyle += `transform-origin: ${((key.rotation_x * coef) + sx)}px ${((key.rotation_y * coef) + sy)}px;`
    
        let innerStyle = '';
        innerStyle += 'user-select: none; border-style: solid; border-radius: 3px;';
        innerStyle += 'border: solid 1px #444;';
        innerStyle += 'position: absolute;';
        innerStyle += 'background-color: '+aztool.key_color+';';
        innerStyle += `left: ${((key.x * coef) + sx)}px;`
        innerStyle += `top: ${((key.y * coef) + sy)}px;`
        innerStyle += `width: ${key.width * coef - 4}px;`
        innerStyle += `height: ${key.height * coef - 4}px;`
  
        const label = key.labels[0] || '';
  
        str += "<div style='" + wrapperStyle + "'>\n";
        // str += "<div id='sw_"+aztool.switch_length+"' style='" + innerStyle + "'><table cellpadding='0' cellspacing='0' style='user-select: none; width: 100%; height: 100%;font-size: 10px;'><tr><td align='center'>" + label + "</td></tr></table></div>\n";
        str += "<div id='"+set_prefix+aztool.switch_length+"' style='" + innerStyle + "'>&nbsp</div>\n";
        str += "</div>\n";
        aztool.switch_length++;
      });
      str += "</div>\n";
  
      $(view_id).html(str);
      $(view_id).css({"background-color": "#eee"});
      return aztool.serial_data;
    } catch(err) {
      window.err = err;
      $(view_id).html("error: " + err.message);
      return false;
    }
};
  
