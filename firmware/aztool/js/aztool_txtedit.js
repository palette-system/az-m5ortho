// テキスト編集ツール

if (!window.aztool) aztool = {};

// 開いているファイルのファイルパス
aztool.txtedit_file_path = "";

// 開いているデータのテキスト(元のデータ)
aztool.txtedit_text = "";
aztool.txtedit_text_size = 0;

// 開いているファイルの現在のテキストデータ（変種中のデータ）
aztool.txtedit_text_edit = "";

// 終了時のコールバック
aztool.txtedit_cb = function() {};

// 指定したファイルを開く
aztool.txtedit_open = function(file_path, cb_func) {
    aztool.view_load_page(); // ロード画面表示
    aztool.txtedit_file_path = file_path; // 開くファイルのパス
    aztool.txtedit_text = ""; // 読み込んだテキスト
    aztool.txtedit_cb = cb_func; // コールバック関数
    if (!aztool.txtedit_cb) aztool.txtedit_cb = function() {};
    webhid.get_file(file_path, function(stat, load_data) {
        // 読み込み失敗
        if (stat != 0) {
            alert(file_path + " を開けませんでした。");
            aztool.txtedit_cb(stat, 0);
            return;
        }
        // 読み込み成功したらデータを受けとる
        let txt = webhid.arr2str(load_data);
        aztool.txtedit_text = txt;
        aztool.txtedit_text_size = load_data.length;
        aztool.txtedit_text_edit = txt;
        console.log(aztool.txtedit_text);
        // 編集画面表示
        aztool.txtedit_editpage_view();
    });

};

// 編集画面表示
aztool.txtedit_editpage_view = function() {
    let h = "";
    h = `
    ファイルパス： ${aztool.txtedit_file_path}　サイズ： ${aztool.txtedit_text_size}<br>
    <textarea id='txtedit_txt' style='width: 900; height: 400px;'></textarea><br>
    <div id='console_div'></div><br>
    <div style="text-align: right; width: 800px;">
    <a class="cancel-button" onClick="javascript:aztool.txtedit_cb(0, 0);">キャンセル</a>
    　<a class="exec-button" onClick="javascript:aztool.txtedit_save();">保存</a>
    </div>`;
    $("#main_box").html(h);
    $("#txtedit_txt").html(aztool.txtedit_text_edit);
};

// 編集したデータを保存
aztool.txtedit_save = function() {
    // データに変更があるかチェック
    let txt = $("#txtedit_txt").val();
    if (aztool.txtedit_text == txt) {
        // 変更が無ければ何もせずに閉じる
        aztool.txtedit_cb(0, 0);
        return;
    }
    // 変更があれば保存
    webhid.save_file(
        aztool.txtedit_file_path, // 保存するファイルパス
        txt, // 保存する内容
        function(stat) {
            // エラー
            if (stat != 0) return;
            // 保存成功コールバック実行
            aztool.txtedit_cb(0, 1);
        }
    );
};
