# Wifi 設定メニュー

<br><br>


## 設定画面の開き方
<table>
  <tr>
    <td><img src="/images/azm5ortho/mon_9.jpg" width="500"></td>
    <td valign="top">
      キーボードを設定モードで再起動します。<br>
      設定モードでの再起動の方法はこちら<br>
       → <a href="https://github.com/palette-system/az-m5ortho/tree/main/docs/azm5ortho/moniter_setting#%E5%90%84%E7%94%BB%E9%9D%A2%E3%81%AE%E8%AA%AC%E6%98%8E" target="_blank">https://github.com/palette-system/az-m5ortho/tree/main/docs/azm5ortho/moniter_setting/</a>
    </td>
  </tr>
  <tr>
    <td><img src="/images/azm5ortho/setting_1.jpg" width="500"></td>
    <td valign="top">
      この状態でPCやスマートフォンからWiFiのアクセスポイントを検索すると「<b>AZ-Keyboard-****</b>」があるので接続します。
    </td>
  </tr>
  <tr>
    <td><img src="/images/azm5ortho/setting_2.jpg" width="500"></td>
    <td valign="top">
      WiFiに接続できるとブラウザが立ち上がり、自動的に設定画面が開きます。
    </td>
  </tr>
</table>

<br><br>


## メイン画面と キー設定画面の説明
<table>
  <tr>
    <td><img src="/images/setting/setting_01.png" width="500"></td>
    <td valign="top">
      <b>メイン画面</b><br><br>
      １．現在編集しているキーボードの名前が表示されます。<br>
      <br>
      ２．設定メニューを開きます。詳細については設定メニューの説明に記述します。<br>
      <br>
      ３．現在選択しているレイヤーが表示されます。<br>
      　▼　→　選択しているレイヤーを切り替えられます。<br>
      　名前変更　→　レイヤーの表示名を変更できます。<br>
      　追加　→　レイヤーを追加できます。<br>
      　×　→　選択中のレイヤーを削除できます。(レイヤー０は削除できません)<br>
      <br>
      ４．現在キーに割り当てている設定が表示されます。キーをクリックするとキーの設定ができます。<br>
      <br>
      ５．変更したキー設定をキーボードに保存して設定モードを終了します。<br>
      <br>
      ６．変更を保存せずに設定モードを終了します。
    </td>
  </tr>
  <tr>
    <td><img src="/images/setting/setting_02.png" width="500"></td>
    <td valign="top">
      <b>通常キー入力</b><br><br>
      １．現在編集しているキーの番号が表示されます。<br>
      <br>
      ２．キーを押した時の動作が表示されます。<br>
      　<b>変更</b> をクリックすると変更できます。<br>
      <br>
      ３．入力するキーが表示されます。<br>
      　<b>変更</b> をクリックすると変更できます。<br>
      　<b>×</b> をクリックすると削除できます。<br>
      <br>
      ４．入力するキーを増やせます。最大5キーまで増やせます。<br>
      <br>
      ５．長押しのモディファイアを設定できます。<br>
      　長押ししている間ここで登録したモディファイアが押され、短押しされた時③④で設定したキーが入力されます。<br>
      <br>
      ６．連打入力を設定できます。押している間③④で登録したキーを連打します。<br>
      　スライドが左に行くほど連打の周期が早くなります。<br>
      　スライドを一番右に持って行くと連打無しです。<br>
      <br>
      ７．設定した内容をキーに登録します。<br>
      <br>
      ８．設定内容を登録せずにメイン画面に戻ります。<br>
    </td>
  </tr>
  <tr>
    <td><img src="/images/setting/setting_03.png" width="500"></td>
    <td valign="top">
      <b>テキスト入力</b><br><br>
      １．入力させたいテキストを登録できます。
      　半角英数字、記号のみで全角文字は登録できません。<br>
    </td>
  </tr>
  <tr>
    <td><img src="/images/setting/setting_04.png" width="500"></td>
    <td valign="top">
      <b>レイヤー切り替え</b><br><br>
      １．キーを押した時に切り替えたいレイヤーを設定します。<br>
      　<b>変更</b> をクリックすると変更できます。<br>
      ２．切り替え方を選択できます。<br>
      　<b>[TO]</b>　切り替え後デフォルトになる<br>
      　<b>[MO]</b>　押している間切り替わる<br>
      　<b>[DF]</b>　切り替え後デフォルトになる<br>
      　<b>[TG]</b>　切り替わったままになる<br>
      　<b>[OSL]</b>　非対応<br>
      　<b>[TT]</b>　非対応<br>
    </td>
  </tr>
  <tr>
    <td><img src="/images/setting/setting_05.png" width="500"></td>
    <td valign="top">
      <b>WEBフック入力</b><br><br>
      キーを押した時にHTTPリクエストを送信します。<br><br>
      １．開くページの URL を設定できます。<br>
      <br>
      ２．HTTPリクエストヘッダーを追加する事ができます。複数指定できます。<br>
      　<b>HTTP ヘッダー追加</b> ヘッダーを新しく追加できます。<br>
      　<b>編集</b> ヘッダーを編集できます。<br>
      　<b>削除</b> ヘッダーを削除できます。<br>
      <br>
      ３．POST で任意のデータを送信できます。<br>
      　キーバリュー、JSON など自分でフォーマットして指定して下さい。<br>
      　この欄を空にすると GET リクエストになります。<br>
      <br>
      ４．HTTP 取得した後のキー動作を下記から指定できます。<br>
      　動作無し<br>
      　結果のステータスコードをキー入力<br>
      　HTTP レスポンスをキー入力<br>
      <br><br><br>
      ※ WIFI 設定がしてある必要があります。
      ※ HTTP レスポンスは 1024 バイトまで取得します。
      ※ HTTP レスポンス内に全角文字が含まれていると入力動作が不安定になります。
    </td>
  </tr>
  <tr>
    <td><img src="/images/setting/setting_06.png" width="500"></td>
    <td valign="top">
      <b>マウス移動</b><br><br>
      キーを押している間マウスを移動します。<br><br>
      １．横移動する距離を設定します。<br>
      　マイナスを指定すると左、プラスを指定すると右に移動します。<br>
      ２．縦移動する距離を設定します。<br>
      　マイナスを指定すると上、プラスを指定すると下に移動します。<br>
      ３．移動する速度を設定します。数値が高いほど早く動きます<br>
    </td>
  </tr>
</table>

<br><br>

## 設定メニュー画面
<table>
  <tr>
    <td><img src="/images/setting/setting_07.png" width="500"></td>
    <td valign="top">
      <b>設定メニュー</b><br><br>
      <b>ソフトウェアキー</b><br>
      　M5StackCore2画面に表示する仮想キーの設定をします。<br>
      <br>
      <b>日本語/US 切り替え</b><br>
      　HID送信するキーコードのタイプを変更します。<br>
      <br>
      <b>キーボードの種類</b><br>
      　M5StackCore2に接続しているキーボードの種類を確認できます。<br>
      <br>
      <b>モニタ設定</b><br>
      　M5StackCore2画面に表示する待ち受け画像などの設定をします。<br>
      <br>
      <b>サウンド設定</b><br>
      　打鍵した時に再生するWAVファイルの登録ができます。<br>
      <br>
      <b>ユニット設定</b><br>
      　準備中です。<br>
      <br>
      <b>スイッチ接触確認</b><br>
      　キーが押されているかチェックできます。<br>
      <br>
      <b>WIFI設定</b><br>
      　M5StackCore2をWifiに接続するためのSSID,PASSの設定をします。<br>
      <br>
      <b>ログイン設定</b><br>
      　この管理ページへログインするためのパスワードを設定します。<br>
      <br>
      <b>設定の初期化</b><br>
      　M5StackCore2に保存した設定の内容をリセットします。<br>
      <br>
      <b>ファームウェア</b><br>
      　使用しているファームウェアのバージョンを確認します。<br>
    </td>
  </tr>
  <tr>
    <td><img src="/images/setting/setting_08.png" width="500"></td>
    <td valign="top">
      <b>ソフトウェアキー 画面</b><br><br>
      M5StackCore2画面に表示する仮想キーの設定をします<br>
      最大で6キーまで登録できます。<br>
      <br>
      <b>button*</b><br>
      　クリックすると設定しているボタンを押した時の動作を変更できます<br>
      <br>
      <b>削除</b><br>
      　ボタンを削除できます。<br>
      <br>
      <b>キー追加</b><br>
      　仮想キーのボタンを追加できます。<br>
    </td>
  </tr>
  <tr>
    <td><img src="/images/setting/setting_09.png" width="500"></td>
    <td valign="top">
      <b>日本語/US 切り替え 画面</b><br><br>
      HID送信するキーコードのタイプを変更します<br>
      <br>
      <b>変更</b><br>
      　クリックするとタイプを変更できます<br>
    </td>
  </tr>
  <tr>
    <td><img src="/images/setting/setting_10.png" width="500"></td>
    <td valign="top">
      <b>モニター設定 画面</b><br><br>
      M5StackCore2画面の設定を変更します。<br>
      <br>
      <b>待ち受け画像</b><br>
      　キーボード起動中に表示する画像を設定できます。<br>
      　ファイルはBMP、JPEG、PNGが指定できます。<br>
      　サイズは320×240(縦長時は240×320)に収まるように小さくされます。<br>
      <br>
      <b>確認</b><br>
      　選択した画像をM5StackCore2のモニタに表示します。(少し時間が掛かります)<br>
      <br>
      <b>削除</b><br>
      　設定している画像を削除します。<br>
      　何も設定されていない場合デフォルトの待ち受けが表示されます。<br>
      <br>
      <b>決定</b><br>
      　設定した画像をM5StackCore2に保存してメニューに戻ります。<br>
      <br>
      <b>キャンセル</b><br>
      　メニューに戻ります。<br>
    </td>
  </tr>
  <tr>
    <td><img src="/images/setting/setting_11.png" width="500"></td>
    <td valign="top">
      <b>サウンド設定 画面</b><br><br>
      打鍵時のカスタム音の設定を変更します。<br>
      <br>
      <b>カスタム音(デフォルト)</b><br>
      　打鍵音設定に”カスタム音”を設定して打鍵した時に再生されるWAVファイルを設定します。<br>
      <br>
      <b>カスタム音(エンター)</b><br>
      　打鍵音設定に”カスタム音”を設定してエンターを押した時に再生されるWAVファイルを設定します。<br>
      <br>
      <b>決定</b><br>
      　設定したWAVファイルをM5StackCore2に保存してメニューに戻ります。<br>
      <br>
      <b>キャンセル</b><br>
      　メニューに戻ります。<br>
      <br>
      <br>
      ※ WAVファイルのビットレートは自動認識するのでお好みの音質でWAVファイルを作成して下さい。<br>
      ※ 2チャンネル入ったファイルでもモノラルで再生されます。<br>
      ※ ファイルサイズは500KBまでです。<br>
    </td>
  </tr>
  <tr>
    <td><img src="/images/setting/setting_12.png" width="500"></td>
    <td valign="top">
      <b>WIFI設定 画面</b><br><br>
      キーボード本体が WIFI に接続するための設定ができます。<br>
      設定は複数設定可能で、一番電波が強いアクセスポイントに接続しに行きます。<br>
      この設定をするとキーボードモードで起動した時に WIFI に接続しに行くので起動が遅くなります。<br>
      また、WIFI に常時接続した状態なので電池の消費が早いです。<br>
      WEB フックなどを使用する予定が無い場合は設定しない事をオススメします。<br>
      <br>
      <b>編集</b><br>
      　登録した WIFI 設定を編集します。<br>
      <br>
      <b>削除</b><br>
      　登録した WIFI 設定を削除します。<br>
      <br>
      <b>WIFI 追加</b><br>
      　新しく WIFI 設定を登録します。<br>
      <br>
      <b>完了</b><br>
      　メニューに戻ります。<br>
    </td>
  </tr>
  <tr>
    <td><img src="/images/setting/setting_13.png" width="500"></td>
    <td valign="top">
      <b>WIFI アクセスポイント設定 画面</b><br><br>
      <br>
      <b>ssid</b><br>
      　▼をクリックするとアクセスポイントを探して選択できます。<br>
      <br>
      <b>pass</b><br>
      　アクセスポイントにログインするためのパスワードを設定します。<br>
      <br>
      <b>決定</b><br>
      　アクセスポイントの情報を登録してWIFI設定画面に戻ります。<br>
      <br>
      <b>キャンセル</b><br>
      　WIFI設定画面に戻ります。<br>
    </td>
  </tr>
  <tr>
    <td><img src="/images/setting/setting_14.png" width="500"></td>
    <td valign="top">
      <b>ログイン設定 画面</b><br><br>
      設定画面に入るための WIFI パスワードを設定します。<br>
      ここで指定したパスワードを忘れると設定画面に入れなくなるので注意して下さい。<br>
      <br>
      <b>pass</b><br>
      　ログインするためのパスワードを設定します。<br>
      <br>
      <b>決定</b><br>
      　パスワードを設定してメニューに戻ります。<br>
      <br>
      <b>キャンセル</b><br>
      　メニューに戻ります。<br>
    </td>
  </tr>
  <tr>
    <td><img src="/images/setting/setting_15.png" width="500"></td>
    <td valign="top">
      <b>設定の初期化 画面</b><br><br>
      キーマップ、登録した画像、サウンド設定などの全ての設定をリセットします。<br>
      <br>
      <b>初期化する</b><br>
      　リセットしてM5StackCore2を再起動します。<br>
      <br>
      <b>キャンセル</b><br>
      　メニューに戻ります。<br>
    </td>
  </tr>
  <tr>
    <td><img src="/images/setting/setting_16.png" width="500"></td>
    <td valign="top">
      <b>ファームウェア 画面</b><br><br>
      現在動作しているファームウェアのバージョン確認と、ファームウェアのアップデートができます。<br>
      <b>キャンセル</b><br>
      　メニューに戻ります。<br>
      <br>
      ※ この機能はAZ-Macro用の物でM5StackCore2ではUSB経由でファームウェアのアップデートをして下さい。
    </td>
  </tr>
</table>

