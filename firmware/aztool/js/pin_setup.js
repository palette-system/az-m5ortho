
let pinstp = {};

// モーダル閉じた後のコールバック
pinstp.cb_func = null;

// 受け取ったIOエキスパンダデータ
pinstp.ioxp_option = {};

// 動作オプション
pinstp.option = {};

pinstp.zero = function(n, len){
    return n.toString().padStart(len, "0");
};

// ピン設定初期化
pinstp.init = function() {
    // モーダル用HTML登録
    pinstp.init_html();
    // モーダル登録
    pinstp.mdl = $('[data-remodal-id=pinset_modal]').remodal();
    pinstp.mdl.settings.closeOnOutsideClick = false;
    pinstp.mdl.settings.hashTracking = false;

};

// モーダル用HTML登録
pinstp.init_html = function() {
    let html = `
        <div class="remodal azmodal" data-remodal-id="pinset_modal" 
                data-remodal-options="hashTracking: false, closeOnOutsideClick: false"
                style="max-width: 1200px;">
            <!-- クローズボタン -->
            <!-- button data-remodal-action="close" class="remodal-close"></button> -->

        <table>
            <tr>
                <td style="text-align: center; width: 500px;">
                    <font style="font-size:24;"><b>アドレス：</b></font>　
                    <select id="addr_select" class="addrsl" onChange="javascript:pinstp.ic_update();">
                        <option value="0" class="addrsl">　0x20　</option>
                        <option value="1" class="addrsl">　0x21　</option>
                        <option value="2" class="addrsl">　0x22　</option>
                        <option value="3" class="addrsl">　0x23　</option>
                        <option value="4" class="addrsl">　0x24　</option>
                        <option value="5" class="addrsl">　0x25　</option>
                        <option value="6" class="addrsl">　0x26　</option>
                        <option value="7" class="addrsl">　0x27　</option>
                    </select>
                    <br><br>
                    <b style="font-size: 24px;">mcp23017</b><br>
                    <table cellpadding="0" cellspacing="0" style="display: inline-block; width: 400px;">
                        <tr><td style="height: 20px;">　</td><td rowspan="16" style="text-align: center;width: 180px;"><img src="./img/mcp23017.png"></td><td>　</td></tr>
                        <tr><td class="pbox" id="pin_01" align="right"></td><td class="pbox" id="pin_28"></td></tr>
                        <tr><td class="pbox" id="pin_02" align="right"></td><td class="pbox" id="pin_27"></td></tr>
                        <tr><td class="pbox" id="pin_03" align="right"></td><td class="pbox" id="pin_26"></td></tr>
                        <tr><td class="pbox" id="pin_04" align="right"></td><td class="pbox" id="pin_25"></td></tr>
                        <tr><td class="pbox" id="pin_05" align="right"></td><td class="pbox" id="pin_24"></td></tr>
                        <tr><td class="pbox" id="pin_06" align="right"></td><td class="pbox" id="pin_23"></td></tr>
                        <tr><td class="pbox" id="pin_07" align="right"></td><td class="pbox" id="pin_22"></td></tr>
                        <tr><td class="pbox" id="pin_08" align="right"></td><td class="pbox" id="pin_21"></td></tr>
                        <tr><td class="pbox" id="pin_09" align="right"></td><td class="pbox" id="pin_20"></td></tr>
                        <tr><td class="pbox" id="pin_10" align="right"></td><td class="pbox" id="pin_19"></td></tr>
                        <tr><td class="pbox" id="pin_11" align="right"></td><td class="pbox" id="pin_18"></td></tr>
                        <tr><td class="pbox" id="pin_12" align="right"></td><td class="pbox" id="pin_17"></td></tr>
                        <tr><td class="pbox" id="pin_13" align="right"></td><td class="pbox" id="pin_16"></td></tr>
                        <tr><td class="pbox" id="pin_14" align="right"></td><td class="pbox" id="pin_15"></td></tr>
                        <tr><td>　</td><td>　</td></tr>
                    </table>
                    
                </td>
                <td style="width: 20px;"></td>
                <td>
                    <div id="pin_setting_right"></div>
                    <div id="pin_setting_info">　</div>
                    <div style="text-align: right; width: 580px;margin:0;">
                        <a id="pin_set_ok" class="exec-button" onClick="javascript:pinstp.eve_ok_click(1);">決定</a>　
                        <div id="pin_set_del_box" style="display: none;"><a id="pin_set_del" class="exec-button" onClick="javascript:pinstp.eve_ok_click(2);">削除</a>　</div>
                        <a id="pin_set_cancel" class="cancel-button" onClick="javascript:pinstp.eve_ok_click(0);">キャンセル</a>
                    </div>
                </td>
            </tr>
        </table>
    </div>`;
    $("body").append(html);
    // 設定枠HTML初期化
    pinstp.set_pin_setting_html();

};

// 設定枠HTML初期化
pinstp.set_pin_setting_html = function() {
    $("#pin_setting_right").html(`<b>Direct</b><br>
    <ul id="list_direct" class="iopinbox sortable ioxp_pintype_box"></ul><br>
    <b>Row</b><br>
    <ul id="list_row" class="iopinbox sortable ioxp_pintype_box"></ul><br>
    <b>Col</b><br>
    <ul id="list_col" class="iopinbox sortable ioxp_pintype_box"></ul>`);
}

// ドラッグイベントの登録
pinstp.init_sortable = function() {
    var ph, pw;
    $('.sortable').selectable({
        cancel: '.sort-handle, .ui-selected',
        filter: '> li'
    }).sortable({
        opacity: 0.9,
        items: "> li",
        handle: 'div, .sort-handle, .ui-selected',
        connectWith: ".iopinbox",
        //ヘルパー生成時（ドラッグ時）処理
        helper: function(e, item) { // item はドラッグした1つだけ入って来る
            if ( ! item.hasClass('ui-selected') ) {
                item.parent().children('.ui-selected').removeClass('ui-selected');
                item.addClass('ui-selected');
            }
            var selected = item.parent().children('.ui-selected').clone(); // selectable で選ばれたヤツを全部コピー
            //placeholder用の高さを取得しておく
            ph = item.outerHeight() * selected.length;
            pw = (item.outerWidth() + 20) * selected.length; // ドラッグしたアイテムのサイズ×選択したアイテムの数
            // ↑ 本当は選択した要素の部分の高さと幅を取得したいけど縦に複数選択した場合などの高さ、幅の取得方法が分からない
            console.log(item);
            console.log("ph: " + ph + "    pw: " + pw);
            item.data('multidrag', selected).siblings('.ui-selected').remove(); // ？
            console.log(selected);
            return $("<li style='background-color: transparent; width: "+pw+"px;' />").append(selected); // 移動で持って行くDOMにselectable で選ばれたヤツを突っ込んで返す
        },
        //ドラッグした時にplaceholderの高さを選択要素分取る
        start: function(e, ui) {
            console.log(ph);
            console.log(ui.placeholder);
            // ui.placeholder.css({'height':ph});
            // ui.placeholder.css({"height": 0, "width": 0});
        },
        //ドロップ時処理
        stop: function(e, ui) { // ui.item はドラッグした1つだけ入って来る
            var selected = ui.item.data("multidrag"); // 渡されたアイテムに突っ込んだ選択したアイテムを取得(選択したアイテムの中にドラッグしたアイテムのクローンも入ってる)
            ui.item.after(selected); // 受け取ったアイテム(ドラッグしたアイテム)の隣に選択したアイテムを並べる
            ui.item.remove(); // 受け取ったアイテム(ドラッグしたアイテム)事態は削除(じゃないとドラッグしたアイテムが２つになる)
            // 選択を解除
            selected.removeClass("ui-selected");
            // アイテムをピン名順にソート
            let sort_func = function(a, b){
                return ((parseInt($(a).attr("id").split("_")[1]) > parseInt($(b).attr("id").split("_")[1]))? 1 : -1);
            };
            $("#list_direct").html($("ul#list_direct li").sort(sort_func));
            $("#list_row").html($("ul#list_row li").sort(sort_func));
            $("#list_col").html($("ul#list_col li").sort(sort_func));
            // ピン情報更新
            pinstp.ic_update();
        }
    });

    pinstp.ic_update(); // ピン情報更新
};

// ピン情報の表示
pinstp.vset = function(obj_num, set_txt, bgcolor) {
    let obj_id = "#pin_" + obj_num;
    let i = parseInt(obj_num);
    let txt = "";
    $(obj_id).css({"background-color": bgcolor});
    if (i <= 14) {
        txt = set_txt + "<font style='font-size: 14px;color: #666;font-weight: normal;'> ." + obj_num + "</font>";
    } else {
        txt = "<font style='font-size: 14px;color: #666;font-weight: normal;'>" + obj_num + ". </font>" + set_txt;
    }
    $(obj_id).html(txt);
};

// ピン情報の表示更新
pinstp.ic_update = function() {

    // gpioとicのピン番号のマッピング
    let pim_map = {
        "gpio_00": "21",
        "gpio_01": "22",
        "gpio_02": "23",
        "gpio_03": "24",
        "gpio_04": "25",
        "gpio_05": "26",
        "gpio_06": "27",
        "gpio_07": "28",
        "gpio_08": "01",
        "gpio_09": "02",
        "gpio_10": "03",
        "gpio_11": "04",
        "gpio_12": "05",
        "gpio_13": "06",
        "gpio_14": "07",
        "gpio_15": "08"
    };
    // direct row col
    let i, k;
    let directs = $("#list_direct").contents();
    let rows = $("#list_row").contents();
    let cols = $("#list_col").contents();
    for (i in directs) {
        if (!directs[i].id || !pim_map[directs[i].id]) continue;
        k = pim_map[directs[i].id];
        pinstp.vset(k, "Direct "+i, "#e2ffd6");
    }
    for (i in rows) {
        if (!rows[i].id || !pim_map[rows[i].id]) continue;
        k = pim_map[rows[i].id];
        pinstp.vset(k, "Row "+i, "#ffd6f6");
    }
    for (i in cols) {
        if (!cols[i].id || !pim_map[cols[i].id]) continue;
        k = pim_map[cols[i].id];
        pinstp.vset(k, "Col "+i, "#fff8ad");
    }

    // 固定ピン
    pinstp.vset("12", "SCL", "#d9fbfd");
    pinstp.vset("13", "SDA", "#d9fbfd");
    pinstp.vset("09", "VCC", "#ff8081");
    pinstp.vset("10", "GND", "#a4c5f9");
    pinstp.vset("11", "NC", "#e0e0e0");
    pinstp.vset("14", "NC", "#e0e0e0");
    pinstp.vset("18", "VCC", "#ff8081");
    pinstp.vset("19", "NC", "#e0e0e0");
    pinstp.vset("20", "NC", "#e0e0e0");

    // アドレス
    let addr = parseInt($("#addr_select").val());
    let addr_pin = {"15": 0x01, "16": 0x02, "17": 0x04};
    for (k in addr_pin) {
        if (addr & addr_pin[k]) {
            pinstp.vset(k, "VCC", "#ff8081");
        } else {
            pinstp.vset(k, "GND", "#a4c5f9");
        }
    }
};

pinstp.li_html = function(n) {
    let s = pinstp.zero(n, 2);
    if (n < 8) {
        return `<li id="gpio_` + s + `" class="ioxp_pin_box_a"><div class="ioxp_pin_box_div">A` + n + `</div></li>`;
    } else {
        return `<li id="gpio_` + s + `" class="ioxp_pin_box_b"><div class="ioxp_pin_box_div">B` + n + `</div></li>`;
    }
};

// OK、キャンセルのクリックイベント(click_type = 0 キャンセル / 1 決定)
pinstp.eve_ok_click = function(click_type) {
    let c, i, r = {};
    // キャンセルならそのまま閉じる
    if (click_type == 0) {
        pinstp.cb_func(click_type, {});
        pinstp.mdl.close();
        return;
    }
    // アドレス
    r.addr = parseInt($("#addr_select").val()) + 32;
    // direct
    let directs = $("#list_direct").contents();
    r.direct = [];
    for (i in directs) {
        if (!directs[i].id) continue;
        r.direct.push(parseInt(directs[i].id.split("_")[1]));
    }
    // row
    let rows = $("#list_row").contents();
    r.row = [];
    for (i in rows) {
        if (!rows[i].id) continue;
        r.row.push(parseInt(rows[i].id.split("_")[1]));
    }
    // col
    let cols = $("#list_col").contents();
    r.col = [];
    for (i in cols) {
        if (!cols[i].id) continue;
        r.col.push(parseInt(cols[i].id.split("_")[1]));
    }
    console.log(r);
    // データチェックで問題があればメッセージ表示
    c = pinstp.check_func(click_type, r);
    if (c) {
        $("#pin_setting_info").html(c);
        return;
    }
    // コールバックを実行してモーダルを閉じる
    pinstp.cb_func(click_type, r);
    pinstp.mdl.close();
    return;

};

// IOエキスパンダ設定のモーダルを開く
// check_func = 決定前のチェック関数(戻り値に文字が帰って来るとエラーと判定する)
// cb_func = OK後のコールバック
pinstp.open = function(ioxp_data, opt, check_func, cb_func) {
    // オプション受け取り
    pinstp.option = opt;
    // エキスパンダ設定受け取り
    pinstp.ioxp_option = ioxp_data;
    // チェック関数受け取り
    if (!check_func) check_func = function() {};
    pinstp.check_func = check_func;
    // コールバック受け取り
    if (!cb_func) cb_func = function() {};
    pinstp.cb_func = cb_func;
    // アドレス設定
    $("#addr_select").val(ioxp_data.addr - 32);
    let i, h;
    // 設定枠HTML初期化
    pinstp.set_pin_setting_html();
    // direct 設定
    h = "";
    for (i in ioxp_data.direct) h += pinstp.li_html(ioxp_data.direct[i]);
    $("#list_direct").html(h);
    // row 設定
    h = "";
    for (i in ioxp_data.row) h += pinstp.li_html(ioxp_data.row[i]);
    $("#list_row").html(h);
    // col 設定
    h = "";
    for (i in ioxp_data.col) h += pinstp.li_html(ioxp_data.col[i]);
    $("#list_col").html(h);
    // 削除ボタンフラグがあれば削除ボタンも表示
    if (pinstp.option.btn_delete) {
        console.log("delete_btn: view");
        $("#pin_set_del_box").css("display", "inline-block"); // 表示
    } else {
        console.log("delete_btn: hidden");
        $("#pin_set_del_box").css("display", "none"); // 非表示
    }
    // infoの表示は空にする
    $("#pin_setting_info").html("　");
    // ドラッグ動作設定
    pinstp.init_sortable();
    // モーダルを開く
    pinstp.mdl.open();
};
