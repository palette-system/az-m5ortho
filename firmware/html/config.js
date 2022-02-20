

if (!window.mst) window.mst = {};

// キーボード画像上のキーの位置
mst.key_pattern = {};

// キーボードの言語リスト
mst.key_language_list = [
	"日本語 キーボード",
	"US キーボード",
	"日本語 キーボード(US記号)"
];

// キーボードのタイプデフォルト
mst.key_pattern_default = "az_m5egg";

// 各キーボード別の設定
if (!mst.key_pattern_list) mst.key_pattern_list = {};

// AZ-M5ortho
mst.key_pattern_list["az_m5ortho"] = {
	"name": "AZ M5ortho",
	"keyboard_pin": {
		"direct": [],
		"touch": [],
		"ioxp": [32, 33, 34, 35]
	},
	"ioxp_pin": [26, 14],
	"status_pin": -1,
	"rgb_pin": 27,
	"rgb_len": 59,
	"led_num": [
		23, 20,  9,  6, 22, 21,  8,  7, //  0 -  7
		 4, 10, 18, 24,  3, 11, 17, 25, //  8 - 15
		27, 14, 13,  0, 26, 15, 12,  1, // 16 - 23
		28, 29, 30, -1, -1, -1, -1, -1, // 24 - 31
		32, 43, 46, 57, 31, 44, 45, 58, // 32 - 39
		-1, -1, -1, -1, -1, -1, -1, -1, // 40 - 47
		34, 40, 48, 54, 33, 41, 47, 55, // 48 - 55
		52, 49, 38, 35, 51, 50, 37, 36  // 56 - 63
	],
	"key_matrix": [
		 7,  3,  8, 12, 23, 19, -1, -1, -1, 39, 35, 55, 51, 56, 60,
		 6,  2,  9, 13, 22, 18, -1, -1, -1, 38, 34, 54, 50, 57, 61,
		 5,  1, 10, 14, 21, 17, -1, -1, -1, 37, 33, 53, 49, 58, 62,
		 4,  0, 11, 15, 20, 16, 24, 25, 26, 36, 32, 52, 48, 59, 63
	],
	"matrix_row": 4,
	"matrix_col": 15,
	"size": {"width": 1355, "height": 385},
	"keys": [
		{"id": 7 , "x": 8, "y": 8, "width": 66, "height": 66 },
		{"id": 3 , "x": 98, "y": 8 , "width": 66, "height": 66 },
		{"id": 8 , "x": 189, "y": 8 , "width": 66, "height": 66 },
		{"id":12 , "x": 279, "y": 8 , "width": 66, "height": 66 },
		{"id":23 , "x": 370, "y": 8 , "width": 66, "height": 66 },
		{"id":19 , "x": 460, "y": 8 , "width": 66, "height": 66 },
		{"id":39 , "x": 824, "y": 8 , "width": 66, "height": 66 },
		{"id":35 , "x": 914, "y": 8 , "width": 66, "height": 66 },
		{"id":55 , "x": 1005, "y": 8 , "width": 66, "height": 66 },
		{"id":51 , "x": 1096, "y": 8 , "width": 66, "height": 66 },
		{"id":56 , "x": 1186, "y": 8 , "width": 66, "height": 66 },
		{"id":60 , "x": 1277, "y": 8 , "width": 66, "height": 66 },

		{"id": 6 , "x": 8, "y": 96, "width": 66, "height": 66 },
		{"id": 2 , "x": 98, "y":  96, "width": 66, "height": 66 },
		{"id": 9 , "x": 189, "y":  96, "width": 66, "height": 66 },
		{"id":13 , "x": 279, "y":  96, "width": 66, "height": 66 },
		{"id":22 , "x": 370, "y":  96, "width": 66, "height": 66 },
		{"id":18 , "x": 460, "y":  96, "width": 66, "height": 66 },
		{"id":38 , "x": 824, "y":  96, "width": 66, "height": 66 },
		{"id":34 , "x": 914, "y":  96, "width": 66, "height": 66 },
		{"id":54 , "x": 1005, "y":  96, "width": 66, "height": 66 },
		{"id":50 , "x": 1096, "y":  96, "width": 66, "height": 66 },
		{"id":57 , "x": 1186, "y":  96, "width": 66, "height": 66 },
		{"id":61 , "x": 1277, "y":  96, "width": 66, "height": 66 },

		{"id": 5 , "x": 8, "y": 187, "width": 66, "height": 66 },
		{"id": 1 , "x": 98, "y":  187, "width": 66, "height": 66 },
		{"id":10 , "x": 189, "y":  187, "width": 66, "height": 66 },
		{"id":14 , "x": 279, "y":  187, "width": 66, "height": 66 },
		{"id":21 , "x": 370, "y":  187, "width": 66, "height": 66 },
		{"id":17 , "x": 460, "y":  187, "width": 66, "height": 66 },
		{"id":37 , "x": 824, "y":  187, "width": 66, "height": 66 },
		{"id":33 , "x": 914, "y":  187, "width": 66, "height": 66 },
		{"id":53 , "x": 1005, "y":  187, "width": 66, "height": 66 },
		{"id":49 , "x": 1096, "y":  187, "width": 66, "height": 66 },
		{"id":58 , "x": 1186, "y":  187, "width": 66, "height": 66 },
		{"id":62 , "x": 1277, "y":  187, "width": 66, "height": 66 },

		{"id": 4 , "x": 8, "y": 278, "width": 66, "height": 66 },
		{"id": 0 , "x": 98, "y":  278, "width": 66, "height": 66 },
		{"id":11 , "x": 189, "y":  278, "width": 66, "height": 66 },
		{"id":15 , "x": 279, "y":  278, "width": 66, "height": 66 },
		{"id":20 , "x": 370, "y":  278, "width": 66, "height": 66 },
		{"id":16 , "x": 460, "y":  278, "width": 66, "height": 66 },
		{"id":24 , "x": 551, "y":  278, "width": 66, "height": 66 },
		{"id":25 , "x": 642, "y":  278, "width": 66, "height": 66 },
		{"id":26 , "x": 733, "y":  278, "width": 66, "height": 66 },
		{"id":36 , "x": 824, "y":  278, "width": 66, "height": 66 },
		{"id":32 , "x": 914, "y":  278, "width": 66, "height": 66 },
		{"id":52 , "x": 1005, "y":  278, "width": 66, "height": 66 },
		{"id":48 , "x": 1096, "y":  278, "width": 66, "height": 66 },
		{"id":59 , "x": 1186, "y":  278, "width": 66, "height": 66 },
		{"id":63 , "x": 1277, "y":  278, "width": 66, "height": 66 }


	],
	"parts": [
		{
			"type": "polygon",
			"fill_color": "#666",
			"fill": true, // 塗りつぶし
			"path": [[2,2], [1353,2], [1353,364], [2,364]]
		},
		{
			"type": "polygon",
			"line_width": 4,
			"line_color": "black",
			"path": [[2,2], [1353,2], [1353,364], [2,364]]
		},
	]
};


// AZ M5egg
mst.key_pattern_list["az_m5egg"] = {
	"name": "AZ M5egg",
	"keyboard_pin": {
		"row": [],
		"col": [],
		"direct": [34, 19, 27, 13, 36, 35],
		"touch": []
	},
	"ioxp_pin": [25, 26],
	"status_pin": -1,
	"rgb_pin": 14,
	"rgb_len": 10,
	"led_num": [0, 1, 2, 3, 4, 5],
	"key_matrix": [0, 1, 2, 3, 4, 5],
	"size": {"width": 460, "height": 340},
	"keys": [
		{"id": 0, "x": 11, "y": 35, "width": 115, "height": 115 },
		{"id": 1, "x": 163, "y": 35, "width": 115, "height": 115 },
		{"id": 2, "x": 317, "y": 35, "width": 115, "height": 115 },
		{"id": 3, "x": 11, "y": 188, "width": 115, "height": 115 },
		{"id": 4, "x": 163, "y": 188, "width": 115, "height": 115 },
		{"id": 5, "x": 317, "y": 188, "width": 115, "height": 115 },
	],
	"parts": [
		{
			"type": "polygon",
			"fill_color": "#666",
			"fill": true, // 塗りつぶし
			"path": [[2,2], [446,2], [446,338], [2,338]]
		},
		{
			"type": "polygon",
			"line_width": 4,
			"line_color": "black",
			"path": [[2,2], [446,2], [446,338], [2,338]]
		},
	]
};


// オプションのリスト
mst.option_list = {};




// キーのボタンリスト
mst.key_list = [
	{"key": "16385", "char": "LCk", "value": "Left Click"},
	{"key": "16386", "char": "RCk", "value": "Right Click"},
	{"key": "16388", "char": "MCk", "value": "Middle Click"},
	{"key": "16389", "char": "MScl", "value": "Mouse Scroll"},

	{"key": "224", "char": "LCtrl", "value": "Left Ctrl"},
	{"key": "225", "char": "LShift", "value": "Left Shift"},
	{"key": "226", "char": "LAlt", "value": "Left Alt"},
	{"key": "227", "char": "LGUI", "value": "Left GUI"},
	{"key": "228", "char": "RCtrl", "value": "Right Ctrl"},
	{"key": "229", "char": "RShift", "value": "Right Shift"},
	{"key": "230", "char": "RAlt", "value": "Right Alt"},
	{"key": "231", "char": "RGUI", "value": "Right GUI"},
	{"key": "82", "char": "↑", "value": "↑"},
	{"key": "81", "char": "↓", "value": "↓"},
	{"key": "80", "char": "←", "value": "←"},
	{"key": "79", "char": "→", "value": "→"},

	{"key": "42", "char": "BS", "value": "BackSpace"},
	{"key": "43", "char": "TAB", "value": "Tab"},
	{"key": "40", "char": "ENT", "value": "Enter"},
	{"key": "41", "char": "ESC", "value": "Esc"},
	{"key": "73", "char": "INS", "value": "Insert"},
	{"key": "74", "char": "HOME", "value": "Home"},
	{"key": "76", "char": "DEL", "value": "Delete"},
	{"key": "77", "char": "END", "value": "End"},
	{"key": "57", "char": "CPL", "value": "CaplLock"},
	{"key": "70", "char": "PrtSc", "value": "PrtSc"},
	{"key": "71", "char": "SL", "value": "ScrollLock"},
	{"key": "72", "char": "Pause", "value": "Pause"},
	{"key": "75", "char": "PgUp", "value": "PageUp"},
	{"key": "78", "char": "PgDw", "value": "PageDown"},
	{"key": "136", "char": "H/K", "value": "Hira / Kana"},

	{"key": "8193", "char": "EJC", "value": "Eject"},
	{"key": "8194", "char": "MNX", "value": "Media Next"},
	{"key": "8195", "char": "MPT", "value": "Media Previous"},
	{"key": "8196", "char": "MST", "value": "Media Stop"},
	{"key": "8197", "char": "MPL", "value": "Media Play / Pause"},
	{"key": "8198", "char": "MMT", "value": "Media Mute"},
	{"key": "8199", "char": "MVI", "value": "Media Volume +"},
	{"key": "8200", "char": "MVD", "value": "Media Volume -"},

	{"key": "138", "char": "変換", "value": "変換"},
	{"key": "139", "char": "無変換", "value": "無変換"},
	{"key": "144", "char": "Kana", "value": "Kana(mac)"},
	{"key": "145", "char": "Eisu", "value": "Eisu(mac)"},
	{"key": "101", "char": "App", "value": "App"},
	{"key": "102", "char": "Power", "value": "Power"},
	{"key": "58", "char": "F1", "value": "F1"},
	{"key": "59", "char": "F2", "value": "F2"},
	{"key": "60", "char": "F3", "value": "F3"},
	{"key": "61", "char": "F4", "value": "F4"},
	{"key": "62", "char": "F5", "value": "F5"},
	{"key": "63", "char": "F6", "value": "F6"},
	{"key": "64", "char": "F7", "value": "F7"},
	{"key": "65", "char": "F8", "value": "F8"},
	{"key": "66", "char": "F9", "value": "F9"},
	{"key": "67", "char": "F10", "value": "F10"},
	{"key": "68", "char": "F11", "value": "F11"},
	{"key": "69", "char": "F12", "value": "F12"},

	{"key": "44", "char": "SP", "value": "Space"},
	{"key": "4126", "char": "!", "value": "!"}, // 30 + shift
	{"key": "4148", "char": "*", "value": "*"}, // 52 + shift
	{"key": "4128", "char": "#", "value": "#"}, // 32 + shift
	{"key": "4129", "char": "$", "value": "$"}, // 33 + shift
	{"key": "4130", "char": "%", "value": "%"}, // 34 + shift
	{"key": "4132", "char": "'", "value": "'"}, // 36 + shift
	{"key": "52", "char": ":", "value": ":"},
	{"key": "4134", "char": ")", "value": ")"}, // 38 + shift
	{"key": "4133", "char": "(", "value": "("}, // 37 + shift
	{"key": "4142", "char": "~", "value": "~"}, // 46 + shift
	{"key": "4143", "char": "`", "value": "`"}, // 47 + shift
	{"key": "54", "char": ",", "value": ","},
	{"key": "45", "char": "-", "value": "-"},
	{"key": "4231", "char": "_", "value": "_"}, // 135 + shift
	{"key": "55", "char": ".", "value": "."},
	{"key": "56", "char": "/", "value": "/"},
	{"key": "4147", "char": "+", "value": "+"}, // 51 + shift
	{"key": "51", "char": ";", "value": ";"},
	{"key": "4150", "char": "<", "value": "<"}, // 54 + shift
	{"key": "46", "char": "^", "value": "^"},
	{"key": "4151", "char": ">", "value": ">"}, // 55 + shift
	{"key": "4152", "char": "?", "value": "?"}, // 56 + shift
	{"key": "4127", "char": "\"", "value": "\""}, // 31 + shift
	{"key": "47", "char": "@", "value": "@"},
	{"key": "50", "char": "]", "value": "]"},
	{"key": "4233", "char": "|", "value": "|"}, // 137 + shift
	{"key": "48", "char": "[", "value": "["},
	{"key": "4144", "char": "{", "value": "{"}, // 48 + shift
	{"key": "4146", "char": "}", "value": "}"}, // 50 + shift
	{"key": "4131", "char": "&", "value": "&"}, // 35 + shift
	{"key": "4141", "char": "=", "value": "="}, // 45 + shift
	{"key": "135", "char": "<span style='font-family: Arial;'>\\</span>", "value": "<span style='font-family: Arial;'>\\</span>"},
	{"key": "137", "char": "&yen;", "value": "&yen;"},
	{"key": "53", "char": "e/j", "value": "半角/全角"},

	{"key": "39", "char": "0", "value": "0"},
	{"key": "30", "char": "1", "value": "1"},
	{"key": "31", "char": "2", "value": "2"},
	{"key": "32", "char": "3", "value": "3"},
	{"key": "33", "char": "4", "value": "4"},
	{"key": "34", "char": "5", "value": "5"},
	{"key": "35", "char": "6", "value": "6"},
	{"key": "36", "char": "7", "value": "7"},
	{"key": "37", "char": "8", "value": "8"},
	{"key": "38", "char": "9", "value": "9"},

	{"key": "4",  "char": "a", "value": "a"},
	{"key": "5",  "char": "b", "value": "b"},
	{"key": "6",  "char": "c", "value": "c"},
	{"key": "7", "char": "d", "value": "d"},
	{"key": "8", "char": "e", "value": "e"},
	{"key": "9", "char": "f", "value": "f"},
	{"key": "10", "char": "g", "value": "g"},
	{"key": "11", "char": "h", "value": "h"},
	{"key": "12", "char": "i", "value": "i"},
	{"key": "13", "char": "j", "value": "j"},
	{"key": "14", "char": "k", "value": "k"},
	{"key": "15", "char": "l", "value": "l"},
	{"key": "16", "char": "m", "value": "m"},
	{"key": "17", "char": "n", "value": "n"},
	{"key": "18", "char": "o", "value": "o"},
	{"key": "19", "char": "p", "value": "p"},
	{"key": "20", "char": "q", "value": "q"},
	{"key": "21", "char": "r", "value": "r"},
	{"key": "22", "char": "s", "value": "s"},
	{"key": "23", "char": "t", "value": "t"},
	{"key": "24", "char": "u", "value": "u"},
	{"key": "25", "char": "v", "value": "v"},
	{"key": "26", "char": "w", "value": "w"},
	{"key": "27", "char": "x", "value": "x"},
	{"key": "28", "char": "y", "value": "y"},
	{"key": "29", "char": "z", "value": "z"}
];

// 長押しキーリスト
mst.hold_list = [
	{"key": 0x00, "value": "無し"},
	{"key": 0x61, "value": "左 Ctrl"},
	{"key": 0x62, "value": "左 Shift"},
	{"key": 0x64, "value": "左 Alt"},
	{"key": 0x68, "value": "左 GUI"},
	{"key": 0x63, "value": "左 Ctrl+Shift"},
	{"key": 0x65, "value": "左 Ctrl+Alt"},
	{"key": 0x66, "value": "左 Shift+Alt"},
	{"key": 0x67, "value": "左 Ctrl+Shift+Alt"},
	{"key": 0x69, "value": "左 Ctrl+GUI"},
	{"key": 0x6A, "value": "左 Shift+GUI"},
	{"key": 0x6B, "value": "左 Ctrl+Shift+GUI"},
	{"key": 0x6C, "value": "左 Alt+GUI"},
	{"key": 0x6D, "value": "左 Ctrl+Alt+GUI"},
	{"key": 0x6E, "value": "左 Shift+Alt+GUI"},
	{"key": 0x6F, "value": "左 Ctrl+Shift+Alt+GUI"},
	{"key": 0x71, "value": "右 Ctrl"},
	{"key": 0x72, "value": "右 Shift"},
	{"key": 0x74, "value": "右 Alt"},
	{"key": 0x78, "value": "右 GUI"},
	{"key": 0x73, "value": "右 Ctrl+Shift"},
	{"key": 0x75, "value": "右 Ctrl+Alt"},
	{"key": 0x76, "value": "右 Shift+Alt"},
	{"key": 0x77, "value": "右 Ctrl+Shift+Alt"},
	{"key": 0x79, "value": "右 Ctrl+GUI"},
	{"key": 0x7A, "value": "右 Shift+GUI"},
	{"key": 0x7B, "value": "右 Ctrl+Shift+GUI"},
	{"key": 0x7C, "value": "右 Alt+GUI"},
	{"key": 0x7D, "value": "右 Ctrl+Alt+GUI"},
	{"key": 0x7E, "value": "右 Shift+Alt+GUI"},
	{"key": 0x7F, "value": "右 Ctrl+Shift+Alt+GUI"}
];

// 入力タイプのリスト
mst.input_type_list = {
	"0": "動作無し",
	"1": "通常キー入力",
	"2": "テキスト入力",
	"3": "レイヤー切り替え",
	"4": "WEBフック",
	"5": "マウス移動",
	"6": "暗記ボタン",
	"7": "LED設定ボタン",
	"8": "打鍵設定ボタン"
};

// レイヤー切り替えの種類
mst.layer_move_type_list = [
	{"key": 0x50, "value": "[TO]切り替え後デフォルトになる"},
	{"key": 0x51, "value": "[MO]押している間切り替わる"},
	{"key": 0x52, "value": "[DF]切り替え後デフォルトになる"},
	{"key": 0x53, "value": "[TG]切り替わったままになる"},
	{"key": 0x54, "value": "[OSL]非対応"},
	{"key": 0x58, "value": "[TT]非対応"}
];

// サウンドタイプの種類
mst.sound_type_list = [
	"なし",
	"カスタム音",
	"type 1",
	"type 2",
	"type 3",
	"type 4",
	"type 5",
	"type 6",
	"type 7",
	"type 8"
];

