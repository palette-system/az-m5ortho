'use strict';


// Adafruit_WebSerial_ESPTool に修正を加えたクラス
// ・転送速度変更
// ・ESPの再起動
// ・データを指定するだけでFlash書込み
class AzEspLoader extends EspLoader {

  // 非同期で動いている readLoop を止めるためのフラグ
  read_stop_fl = false;

  // 最後に設定した転送速度
  baud_last_set = 0;

  // ajax実行中の状態(0 = なし / 1 = ajaxリクエスト中 / 2 = 成功 / 3 = エラー)
  ajax_status = 0;

  // ajaxで取得したデータを格納する所
  download_data = {};

  // マックアドレスを見やすくする
  formatMacAddr(macAddr) {
    return macAddr.map(value => value.toString(16).toUpperCase().padStart(2, "0")).join(":");
  }

  // 配列をArrayBufferに変換する
  arrayToArraybuffer(array_data) {
    var i;
    var r = new ArrayBuffer(array_data.length);
    var d = new DataView(r);
    for (i=0; i<array_data.length; i++) {
      d.setUint8(i, array_data[i]);
    }
    return r;
  };

  // シリアルデータ受け取りループ処理
  // (reader と inputBuffer はesptoolライブラリのグローバル変数で固定のようです)
  readLoop(self) {
    let _this = this;
    if (self) _this = self;
    if (_this.read_stop_fl) {
      reader = null;
      return;
    }
    reader = port.readable.getReader();
    reader.read().then(function (res) {
      if (res.done) {
        reader.releaseLock();
        return;
      }
      inputBuffer = inputBuffer.concat(Array.from(res.value));
      reader.releaseLock();
      setTimeout(function() { _this.readLoop(_this); });
    });
  };

  // シリアル転送速度を変更(portをオープンしなおす方式)
  async setBaudrate(baud) {
    if (this._chipfamily == ESP8266) {
      this.logMsg("Baud rate can only change on ESP32 and ESP32-S2");
    } else {
      this.logMsg("Attempting to change baud rate to " + baud + "...");
      try {
        // stub takes the new baud rate and the old one
        let oldBaud = this.baud_last_set ? this.baud_last_set : ESP_ROM_BAUD; // 最後に設定した転送速度を取得、デフォルトは ESP_ROM_BAUD の値(115200)
        let buffer = struct.pack("<II", baud, oldBaud);
        await this.checkCommand(ESP_CHANGE_BAUDRATE, buffer);
        this.setPortBaudRate(baud);
        await this.sleep(50);

        // 読み込みループを一度止める
        this.read_stop_fl = true;
        await this.sleep(100);
        // ポートを閉じる
        if (reader) {
            await reader.cancel();
        }
        await port.close();
        await this.sleep(100);
        // ポートを開く
        await port.open({ baudRate: baud });
        outputStream = port.writable;
        inputStream = port.readable;
        outputStream.getWriter().releaseLock();
        await this.sleep(100);
        // 読み込みループ再開
        this.read_stop_fl = false;
        this.readLoop(); // シリアルデータ読み込みループ処理(port.closeした時にループを止めるのでもう一回実行)
        await this.sleep(100);
        inputBuffer = []; // openしなおした時にいろいろ飛んできているので一旦クリア
        // 最後に設定した転送速度を保持
        this.baud_last_set = baud;

        this.logMsg("Changed baud rate to " + baud);
      } catch (e) {
        throw("Unable to change the baud rate, please try setting the connection speed from " + baud + " to 115200 and reconnecting.");
      }
    }
  };

  // ESPを動作モードで再起動
  async espReboot() {
    // ENを押してリセットする
    await espTool.sleep(50);
    await port.setSignals({ dataTerminalReady: false, requestToSend: true });
    await espTool.sleep(50);
    await port.setSignals({ dataTerminalReady: false, requestToSend: false });
    await espTool.sleep(50);
  };

  // 指定したURLのファイルをダウンロードする
  ajaxArrayBuffer(src) {
    var _self = this;
    var xhr = new XMLHttpRequest();
    xhr.open("GET", src, true);
    xhr.responseType = "arraybuffer";
    xhr.onload = function(e) {
      if (xhr.status == 200) {
        _self.ajax_status = 2;
        _self.download_data[src] = xhr.response;
      } else {
        _self.ajax_status = 3;
        _self.logMsg("download status : " + xhr.status);
      }
    }
    xhr.send();
    _self.ajax_status = 1;
  };

  // ESPにファーム書込み
  // data_list = [ {"address": 書込み先アドレス, "data": 書込みデータ配列} ]
  // baud = 転送速度 (デフォルト: 115200)
  async writeFlash(data_list, baud) {
    try {
      if (!baud) baud = ESP_ROM_BAUD;
      this.logMsg("Connection start");
      this.baud_last_set = 0;
      this.read_stop_fl = false;
      await this.connect(); // 接続
      this.readLoop(); // シリアルデータ読み込みループ処理(裏でシリアルデータをバッファに入れ続ける)
      if (await this.sync()) { // SYNCコマンドを投げて情報取得
        this.logMsg("Connected to " + await this.chipName());
        this.logMsg("MAC Address: " + this.formatMacAddr(espTool.macAddr()));
        this.logMsg("chipType : " + await this.chipType());
        if (baud != ESP_ROM_BAUD) await this.setBaudrate(baud); // 115200、921600
      }
      var i, d, e;
      for (i in data_list) {
        if (!data_list[i].data || !data_list[i].address) continue;
        if (typeof data_list[i].data == "string") {
          // 文字列ならばURLと判断してURLのファイルを取得してArrayBufferにする
          this.logMsg("download start : " + data_list[i].data);
          if (!this.download_data[data_list[i].data]) this.ajaxArrayBuffer(data_list[i].data);
          e = 0;
          while (true) {
              if (this.download_data[data_list[i].data]) break;
              await this.sleep(200);
              if (this.ajax_status == 3) throw "download error : " + data_list[i].data; // ajax失敗
              e++;
              if (e > 900) throw "download timeout : " + data_list[i].data; // 3分待ってダウンロードできなければエラー
          }
          this.ajax_status = 0;
          this.logMsg("download end : " + data_list[i].data);
          d = this.download_data[data_list[i].data];
        } else if (Array.isArray(data_list[i].data)) {
          d = this.arrayToArraybuffer(data_list[i].data); // 配列ならArrayBufferに変換
        } else {
          d = data_list[i].data; // それ以外はArrayBufferが入って来たと判定
        }
        await this.flashData(d, data_list[i].address, i); // 書込み処理
      }
      // ESP32を動作モードで再起動
      await this.espReboot();
      // 読み込みループを一度止める
      this.read_stop_fl = true;
      await this.sleep(300);
      // 接続終了
      await this.disconnect();
      this.logMsg("finish");
    } catch(e) {
      this.logMsg(e);
      // 読み込みループを一度止める
      this.read_stop_fl = true;
      await this.sleep(100);
      // 接続終了
      await this.disconnect();
    }
  };


}

