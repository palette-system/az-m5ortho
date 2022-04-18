
aztool = {};

aztool.step_max = 0;

aztool.option_add = {};

// IOエキスパンダ設定のデフォルト設定
aztool.ioxp_default = {"addr": 32, "row":[], "col":[], "direct":[0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15]};

aztool.init = function() {
    // webhidオブジェクト初期化
    webhid.init({"info_div": "console_div"});
    // ピン設定の初期化
    pinstp.init();
    // aztool 用のHTML追加
    aztool.init_html();
    // モーダル登録
    aztool.klemdl = $('[data-remodal-id=kletxt_modal]').remodal();
    aztool.klemdl.settings.closeOnOutsideClick = false;
    aztool.klemdl.settings.hashTracking = false;
};

aztool.init_html = function() {
    let html = `
    <div  style="max-width: 1200px;">
    <table><tr><td valign="top" align="center" style="width: 270px;">
    <table>
    <tr><td align="center"><div id="stepbox_1" class="option_step">レイアウト設定</div></td></tr>
    <tr><td align="center"><div class="triangle_down"></div></td></tr>
    <tr><td align="center"><div id="stepbox_2" class="option_step">エキスパンダ設定</div></td></tr>
    <tr><td align="center"><div class="triangle_down"></div></td></tr>
    <tr><td align="center"><div id="stepbox_3" class="option_step">ボタンのマッピング</div></td></tr>
    <tr><td align="center"><div class="triangle_down"></div></td></tr>
    <tr><td align="center"><div id="stepbox_4" class="option_step">動作チェック</div></td></tr>
    <tr><td align="center"><div class="triangle_down"></div></td></tr>
    </table>

    </td><td valign="top">
    <div id='kle_view_box' style="border: solid 1px #888; width: 800px;height: 400px;overflow: hidden;"></div>
    <div style="text-align: right; width: 800px;">
    <b style="font-size: 15px;">－</b>
    <input id="coef_input" type="range" name="speed" min="20" max="60" onChange="javascript:aztool.kle_view(aztool.option_add.kle, '#kle_view_box');">
    <b style="font-size: 15px;">＋</b>
    </div>
    <div id="option_setting_form">
    </div>


    </td></tr></table>

    </div>`;
    $("body").append(html);
    aztool.step_max = 4;
    aztool.option_add = {
        "kle": "",
        "ioxp": []
    };
    aztool.option_add_layout_view();
};

// レイアウト設定画面表示
aztool.option_add_layout_view = function() {
    let h = `
        KLEのレイアウトJSONを張り付けて下さい。<br>    
        <textarea id="kle_json_txt" style="width: 800px; height: 150px;"
        onChange="javascript:aztool.option_add.kle = $('#kle_json_txt').val(); aztool.kle_view(aztool.option_add.kle, '#kle_view_box', true);"></textarea>
        <br><br>
        <div style="text-align: right; width: 800px;">
        <a class="exec-button" onClick="javascript:aztool.option_add_expanda_view();">次へ</a>
        </div>`;
    $("#option_setting_form").html(h);
    $("#kle_json_txt").html(aztool.option_add.kle);
    aztool.update_step_box(1);
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
        <a class="exec-button" onClick="javascript:aztool.kle_view($('#kle_json_txt').val(), '#kle_view_box', true);">次へ</a>
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
        aztool.option_add_ioxp_update(); // 表示を更新
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
            aztool.option_add_ioxp_update(); // 表示を更新
        });
};


aztool.update_step_box = function(step_num) {
    let i, c;
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

aztool.eve_kle_text_ok = function(type_num) {
    aztool.klemdl.close();
};

// kle JSONからキー配列を表示
aztool.kle_view = function(json_data, view_id, auto_resize) {
    try {
      var tx = json_data.replace(/({|,|\s)([\w|\d]{1,3}):/g, "$1\"$2\":")
      var json_data = JSON.parse("[" + tx + "]");
      aztool.serial_data = $serial.deserialize(json_data);
      var str="";
      let coef = parseInt($("#coef_input").val());
      if (auto_resize) coef = 60; // 最大は60
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
        innerStyle += `left: ${((key.x * coef) + sx)}px;`
        innerStyle += `top: ${((key.y * coef) + sy)}px;`
        innerStyle += `width: ${key.width * coef - 4}px;`
        innerStyle += `height: ${key.height * coef - 4}px;`
  
        const label = key.labels[0] || '';
  
        str += "<div style='" + wrapperStyle + "'>\n";
        str += "<div style='" + innerStyle + "'><table cellpadding='0' cellspacing='0' style='user-select: none; width: 100%; height: 100%;font-size: 10px;'><tr><td align='center'>" + label + "</td></tr></table></div>\n";
        str += "</div>\n";
      });
      str += "</div>\n";
  
      $(view_id).html(str);
    } catch(err) {
      window.err = err;
      $(view_id).html("error: " + err.message);
    }
};
  
