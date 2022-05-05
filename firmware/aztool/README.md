# 設定ツール

使い方

- M5StackCore2のドライバ　CP2104 Driver　のインストール

　　　　https://shop.m5stack.com/pages/download

- このリポジトリをクローンする

- このディレクトリにある　writer.html　を Chrome で開いて　M5StackCore2　にツール対応のファームウェアを書き込む

　　　※　M5StackCore2とはUSBで接続する

- M5StackCore2とBluetoothのペアリングをする

　　　※　ペアリング済みだった人はペアリング設定を削除してもう一度ペアリングしなおす

- このディレクトリにある　index.html　を　Chrome　で開いて設定ツールを使う

# オプションを追加する流れ

- M5StackCore2の電源が入ってない状態でオプションになる基板をTRRSで接続する

- M5StackCore2の電源を入れる

- 設定ツールを開いて「オプション設定」→「オプション追加」からオプションを追加する

- 追加が終わるとM5StackCore2の再起動をして設定画面から追い出されるので、再起動が終わったらまた設定画面から接続する。

　　※　再接続する時は一度ページをリロードした方が安全（なんか前のキャッシュが残ってたりしてて時々変な動きする時がある）

- 「キーマップ設定」を開くと登録したオプションが表示されていて、キー設定が出来る

- キー設定が終わったら「保存して再起動」すると設定した内容が保存されて使えるようになってる

# おまけ

「設定JSON編集」で設定に使用しているJSONを直接操作できる。

VID/PID、I2Cのピン、キーボードのピンなどキーボードで設定してる内容はだいたいこのJSONで設定してます。

| キー | 内容 |
| --- | --- |
| ap.pass | 設定モード(WIFI)で起動した時のWIFIに接続するパスワード |
| wifi | M5StackCore2がWIFIにアクセスするためのAPとパスワード |
| vendorId<br>productId | ベンダーID<br>プロジェクトID |
| rgb_pin<br>rgb_len | NeoPixcell へ接続してるピン<br>何個接続してるか |
| led_num<br>key_matrix<br>matrix_row<br>matrix_col | NeoPixcell 表示に使う用の設定 |
| disp_rotation | 画面の向き |
| keyboard_language | 日本語入力か英語入力か（廃止したい） |
| keyboard_name | キーボード名 |
| keyboard_type | キーボードの種類コード<br>この値で今どのキーボードとして動いてるか判定 |
| keyboard_pin | 本体のキースイッチに設定しているピン<br>direct - GPIOから直にスイッチ<br>touch - 未使用<br>ioxp - エキスパンダのアドレス(全てdirect) |
| ioxp_pin | I2C接続するピン |
| default_layer | キーボード起動時のレイヤーの番号 |
| layers | キー入力の設定、キー名がレイヤーID |
| layers[n].name | レイヤーの名前 |
| layers[n].keys | レイヤーに登録されているキー設定<br>キー名がキーのID |
| layers[n].keys[k].press | キーが押された時の動作 |
| layers[n].keys[k].press.action_type | 動作のタイプ |
| layers[n].keys[k].press.key | 送信するHIDコード(５個まで同時押し可) |
| option_set | 付けているオプション設定(AZ-Macro用) |
| i2c_option | 設定したI2Cオプションの設定 |
| i2c_option[n].id | オプションの識別番号 |
| i2c_option[n].type | オプションの種類<br>今は 1:エキスパンダ のみ |
| i2c_option[n].enable | オプションが有効かどうか（無効だと入力しても反応しない） |
| i2c_option[n].ioxp | オプションで使用するエキスパンダ(複数)<br>と、エキスパンダのピン設定 |
| i2c_option[n].map_start | キー設定のIDとマッピング開始する番号 |
| i2c_option[n].map | エキスパンダの入力データの何番目がキーになるかのマッピング |
| layout | キーマップ設定でキー配列をドラッグで移動した表示位置 |
