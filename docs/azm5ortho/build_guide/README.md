# AZ-M5ortho 組み立て手順

<br><br>


## 使う道具
<table>
  <tr>
    <td><img src="/images/azm5ortho/dougu.jpg" width="400"></td>
    <td>
      カッター<br>
      プラスドライバー（中）<br>
      ピンセット<br>
      ニッパー<br>
      はんだ<br>
      はんだごて
    </td>
  </tr>
</table>

<br><br>


## カラーLEDのはんだ付け
<table>
  <tr>
    <td><img src="/images/azm5ortho/muki_2.jpg" width="400"></td>
    <td valign="top">
      <b>向き合わせ</b><br>
      PCBについている三角マークと、LEDの足の角が欠けている方向を一緒にして下さい。<br>
      向きを間違えると他のLEDも含め動作しなくなるので注意して下さい。
    </td>
  </tr>
  <tr>
    <td><img src="/images/azm5ortho/muki_3.jpg" width="400"></td>
    <td valign="top">
      8ヶ所LEDの穴の枠が黒く塗られている所があり、こちらはLEDを表向きにしてはんだします。<br>
      こちらもPCBの三角マークと、LEDの足の角が欠けている方向を一緒にして下さい。
    </td>
  </tr>
</table>

<br><br>

## スイッチ用PCBソケットのはんだ付け
<table>
  <tr>
    <td><img src="/images/azm5ortho/muki_1.jpg" width="400"></td>
    <td valign="top">
      ソケット用の穴にソケットを差し込みはんだ付けします。
    </td>
  </tr>
</table>

<br><br>

## ピンヘッダのはんだ付け
<table>
  <tr>
    <td><img src="/images/azm5ortho/muki_6.jpg" width="400"></td>
    <td valign="top">
      ニッパーなどでピンヘッダを幅15ピンでカットし、PCBの表側から差し込み、裏側裏側をはんだ付けします。<br>
      ※ この位置が重要でピンヘッダが斜めに刺さっていたりズレたりするとM5StackCore2の位置がズレます。<br>
      ※ 心配な方はM5StackCore2にピンヘッダを差し込んだ状態ではんだ付けするとズレずにはんだ付けできます。
    </td>
  </tr>
</table>

<br><br>

## PHコネクタのはんだ付け
<table>
  <tr>
    <td><img src="/images/azm5ortho/muki_4.jpg" width="400"><br><img src="/images/azm5ortho/muki_5.jpg" width="400"></td>
    <td valign="top">
      PHコネクタも表側から差し込み裏側をはんだ付けします。<br>
      ※ 誤って表裏逆にさすと＋、－が逆になってしまうので気を付けて下さい。
    </td>
  </tr>
</table>

<br><br>

## ねじ止め
<table>
  <tr>
    <td><img src="/images/azm5ortho/kumi_1.jpg" width="400"></td>
    <td valign="top">
      各プレートの保護シートをはがします。<br>
      その後トッププレートに六角ナット固定用の穴が開いているので、六角ナットを差し込みます。
    </td>
  </tr>
  <tr>
    <td><img src="/images/azm5ortho/kumi_2.jpg" width="400"></td>
    <td valign="top">
      トッププレート → PCB → ミドルプレート → ボトムプレート とはさみ、裏側からネジでとめます。
    </td>
  </tr>
  <tr>
    <td><img src="/images/azm5ortho/kumi_3.jpg" width="400"></td>
    <td valign="top">
      左右6ヶ所ずつ、合計12ヶ所とめて下さい。
    </td>
  </tr>
</table>

<br><br>

## M5StackCore2 の取り付け
<table>
  <tr>
    <td><img src="/images/azm5ortho/kumi_4.jpg" width="400"></td>
    <td valign="top">
      M5StackCore2 の背面のコネクタ部分のフタを外します。<br>
      2か所穴が開いているのでそこへピンセット等を差し込みテコの原理で外します。
    </td>
  </tr>
  <tr>
    <td><img src="/images/azm5ortho/kumi_5.jpg" width="400"><br><img src="/images/azm5ortho/kumi_6.jpg" width="400"></td>
    <td valign="top">
      六角レンチで裏面4ヶ所のネジも外します。<br>
      外すとフタにバッテリーが両面テープで固定されているのでフタからバッテリーをはがして下さい。
    </td>
  </tr>
  <tr>
    <td><img src="/images/azm5ortho/kumi_7.jpg" width="400"></td>
    <td valign="top">
      M5StackCore2 のピンソケット部分に AZ-M5ortho のピンヘッダが刺さるように差し込み、裏面からネジどめします。<br>
      ネジは左側２つを短いネジ、右側２つを長いネジでとめます。
    </td>
  </tr>
</table>

<br><br>





これで組み立て完了です。<br>
後はキースイッチを差し込んで、キーキャップを取り付けて下さい。<br>
<br>

<img src="/images/azm5ortho/kumi_8.jpg" width="1000"><br>

バッテリーは標準の物を紹介しましたが、PH2ピンのコネクタにささるリチウムバッテリに変更する事も可能です。<br>
変更する場合は400mAh以上の容量のバッテリーをご利用するのと、標準のバッテリーは抜いてご利用ください。<br>

<br>

