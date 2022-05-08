// 共通設定データ

if (!window.aztool) aztool = {};

// 本体のkle
aztool.main_kle = {};

// えむごっちkle
aztool.main_kle["az_emugotch"] = `
    ["0", "1", "2"],
    ["3", "4", "5"]
`;

// レイヤー切り替えの種類
// https://qiita.com/chesscommands/items/3e7e02d025f261bf6134
aztool.layer_move_type_list = [
	{"key": 0x50, "value": "[TO] 切り替え後デフォルトになる"},
	{"key": 0x51, "value": "[MO] 押している間切り替わる"},
	{"key": 0x52, "value": "[DF] 切り替え後デフォルトになる"},
	{"key": 0x53, "value": "[TG] 切り替わったままになる"},
	{"key": 0x54, "value": "[OSL] 非対応"}, // 次のキーが入力されるまで指定したレイヤーに移動します。キーが入力されると元のレイヤーに戻ります。
	{"key": 0x58, "value": "[TT] 非対応"} // 単体で押すとMOと同じように押している間レイヤーに移動します。5回押すとTGキーと同じように移動したままになります。
];


