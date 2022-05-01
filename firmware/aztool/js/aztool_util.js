// 共通関数

if (!window.aztool) aztool = {};

// ランダムな数字を生成
aztool.random_num = function(n) {
    let r = [];
    while (n > r.length) {
        r.push(Math.floor( Math.random() * 10 ));
    }
    return r.join("");
};

// ミリ秒の時間を取得
aztool.millis = function() {
    let d = new Date();
    return d.getTime();
};

// KLEのJSON文字列をJS配列に変換
aztool.kle_json_parse = function(json_data) {
    var tx = json_data.replace(/({|,|\s)([\w|\d]{1,3}):/g, "$1\"$2\":")
    return JSON.parse("[" + tx + "]");
};

// キーコード(JS)からキーコードデータを取得
aztool.get_key_data = function(c, s) {
    let i, k;
    for (i in aztool.keycode) {
        k = aztool.keycode[i];
        if (k[c] == s) {
            return {
                "str": (k[1])? k[1]: k[0],
                "hid": k[2],
                "ascii": k[3]
            };
        }
    }
    return {"str": "", "hid": 0, "ascii": 0};
};

// オブジェクトのクローンを作成(function などはクローンされないので注意)
aztool.crone = function(obj) {
    return JSON.parse(JSON.stringify(obj));
};

// 渡された文字列が数字のみかどうか
aztool.is_num = function(str) {
    return (str.match(/[^0-9]+/))? false: true;
};

