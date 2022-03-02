# 初期設定

<br><br>

## 対応しているキーボード
- えむごっち
- AZ-M5ortho

<br><br>

## ドライバのインストール
<table>
  <tr>
    <td><img src="/images/azm5ortho/init_2.jpg" width="800"></td>
    <td>
      まずはM5StackCore2のドライバをインストールします。<br>
      (既にM5Stackの開発環境が整っている場合は必要ありません。)<br><br>
      <a href="https://shop.m5stack.com/pages/download" target="_blank"> https://shop.m5stack.com/pages/download</a> にアクセスしお使いのOSに合った 「 <b>CP2104 Driver</b> 」をダウンロードします。
    </td>
  </tr>
  <tr>
    <td><img src="/images/azm5ortho/init_3.jpg" width="800"></td>
    <td>
      利用しているPCに合ったファイルを実行してドライバをインストールして下さい。<br>
      （私の場合はWindows10、64bitCPU のため「 CP210xVCPInstaller_x64_v6.7.0.0.exe 」でした。）
    </td>
  </tr>
</table>

<br><br>


## ファームウェアの書込み
<table>
  <tr>
    <td><img src="/images/azm5ortho/init_4.jpg" width="600"></td>
    <td valign="top">
      ドライバのインストールが終わったらM5StackCore2をPCにUSBでつなぎます。<br><br>
      その後 <b>Chrome</b> で <a href="https://azkey.jp/azm5ortho/" target="_blank">https://azkey.jp/azm5ortho/</a> を開き、ファームウェア欄の「 <b>Version　0.0.1　書込み</b> 」をクリックします。<br>
      必ずChromeで開いて下さい。
    </td>
  </tr>
  <tr>
    <td><img src="/images/azm5ortho/init_5.jpg" width="600"></td>
    <td valign="top">
      ドライバが正しくインストールされていればシリアルポートのデバイスが出てくるので、選択して「接続」をクリックします。
    </td>
  </tr>
  <tr>
    <td><img src="/images/azm5ortho/init_6.jpg" width="600"></td>
    <td valign="top">
      接続されるとボタンの下に書込みステータスが表示され、書き込みが始まります。<br><br>
      ※ 書込みが失敗する場合はChromeを再起動したり、M5StackCore2のUSBを抜き差ししてもう一度お試しください。
    </td>
  </tr>
  <tr>
    <td><img src="/images/azm5ortho/init_7.jpg" width="600"></td>
    <td valign="top">
     <b> finish</b> と表示されたら書込み完了です。<br>
     自動的にM5StackCore2が再起動するので、起動するまでお待ちください。<br><br>
     ※ 一番最初に書き込む場合は再起動後ROM内の初期化が行われるため1分半ほど起動に時間がかかります。画面が暗いままですがそのままお待ちください。
    </td>
  </tr>
</table>

<br><br>

## 接続しているキーボードの選択
<table>
  <tr>
    <td><img src="/images/azm5ortho/mon_1.jpg" width="600"></td>
    <td valign="top">
      初回起動時のみ現在接続しているキーボードの選択画面が表示されます。<br>
      接続しているキーボード(<b>AZ-M5ortho</b>)を選択し「 決定 」をタップしてください。
    </td>
  </tr>
  <tr>
    <td><img src="/images/azm5ortho/mon_3.jpg" width="600"><br><img src="/images/azm5ortho/init_8.jpg" width="600"></td>
    <td valign="top">
      M5StackCore2が再起動されパレットシステムのロゴが表示されたら初期設定完了です。<br><br>
      この状態でPCからBluetooth機器を検索するとキーボードが出てきます。
    </td>
  </tr>
</table>

<br>

