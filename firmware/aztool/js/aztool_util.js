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

aztool.kle_json_parse = function(json_data) {
    var tx = json_data.replace(/({|,|\s)([\w|\d]{1,3}):/g, "$1\"$2\":")
    return JSON.parse("[" + tx + "]");
};
