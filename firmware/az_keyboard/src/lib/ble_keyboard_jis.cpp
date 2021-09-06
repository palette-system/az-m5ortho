#include <NimBLEDevice.h>
#include <NimBLEUtils.h>
#include <NimBLEServer.h>
#include "HIDTypes.h"
#include <driver/adc.h>
#include "sdkconfig.h"

#include "ble_keyboard_jis.h"
#include "ble_callbacks.h"

// コールバッククラス
static DescriptorCallbacks dscCallbacks;
static CharacteristicCallbacks chrCallbacks;

// コンストラクタ
BleKeyboardJIS::BleKeyboardJIS(std::string deviceName, std::string deviceManufacturer)
{
  this->deviceName = deviceName;
  this->deviceManufacturer = deviceManufacturer;
  this->_MouseButtons = 0x00;
  this->batteryLevel = 100;
  this->connectionStatus = new BleConnectionStatus();
  this->keyboard_language = 0;
  this->_hidReportDescriptor = (uint8_t *)_hidReportDescriptorDefault;
  this->_hidReportDescriptorSize = sizeof(_hidReportDescriptorDefault);
  this->_asciimap = (unsigned short *)_asciimap_default;
  this->_codemap_us = (unsigned short *)_codemap_us_default;
  
};

// 日本語キーボード、USキーボードを設定
void BleKeyboardJIS::set_keyboard_language(uint8_t kl)
{
  this->keyboard_language = kl;
};

// HID report map 設定
// 例： bleKeyboard.set_report_map((uint8_t *)_hidReportDescriptorDefault, sizeof(_hidReportDescriptorDefault));
void BleKeyboardJIS::set_report_map(uint8_t * report_map, unsigned short report_size)
{
  this->_hidReportDescriptor = report_map;
  this->_hidReportDescriptorSize = report_size;
};

// BLEキーボードとして処理開始
void BleKeyboardJIS::begin(void)
{
  xTaskCreate(this->taskServer, "server", 20000, (void *)this, 5, NULL); // BLE HID 開始処理
};

// BLEキーボード終了
void BleKeyboardJIS::end(void)
{
};

// 接続中かどうかを返す
bool BleKeyboardJIS::isConnected(void)
{
  return this->connectionStatus->connected;
};

// BLE HID 開始処理
void BleKeyboardJIS::taskServer(void* pvParameter)
{

   BleKeyboardJIS *bleKeyboardInstance = (BleKeyboardJIS *) pvParameter; //static_cast<BleKeyboard *>(pvParameter);

    /** sets device name */
    NimBLEDevice::init(bleKeyboardInstance->deviceName);
    NimBLEDevice::setPower(ESP_PWR_LVL_P9); /** +9db */
    NimBLEDevice::setSecurityAuth(BLE_SM_PAIR_AUTHREQ_BOND);

    bleKeyboardInstance->pServer = NimBLEDevice::createServer();
    bleKeyboardInstance->pServer->setCallbacks(bleKeyboardInstance->connectionStatus);

    //DeviceInfoService
    bleKeyboardInstance->pDeviceInfoService = bleKeyboardInstance->pServer->createService("180A"); // <-デバイスインフォのUUID
    bleKeyboardInstance->pPnpCharacteristic = bleKeyboardInstance->pDeviceInfoService->createCharacteristic("2A50", NIMBLE_PROPERTY::READ);
    uint8_t sig = 0x02;
    uint16_t vid = 0xe502;
    uint16_t pid = 0xa111;
    uint16_t version = 0x0210; 
    uint8_t pnp[] = { sig, (uint8_t) (vid >> 8), (uint8_t) vid, (uint8_t) (pid >> 8), (uint8_t) pid, (uint8_t) (version >> 8), (uint8_t) version };
    bleKeyboardInstance->pPnpCharacteristic->setValue(pnp, sizeof(pnp));
    bleKeyboardInstance->pPnpCharacteristic->setCallbacks(&chrCallbacks);

    //DeviceInfoService-Manufacturer
    bleKeyboardInstance->pManufacturerCharacteristic = bleKeyboardInstance->pDeviceInfoService->createCharacteristic("2A29", NIMBLE_PROPERTY::READ); // 0x2a29 = メーカー名
    bleKeyboardInstance->pManufacturerCharacteristic->setValue(bleKeyboardInstance->deviceManufacturer);
    bleKeyboardInstance->pManufacturerCharacteristic->setCallbacks(&chrCallbacks);

    //HidService
    bleKeyboardInstance->pHidService = bleKeyboardInstance->pServer->createService(NimBLEUUID("1812"), 40);

    //HidService-hidInfo
    bleKeyboardInstance->pHidInfoCharacteristic = bleKeyboardInstance->pHidService->createCharacteristic("2A4A", NIMBLE_PROPERTY::READ);// HID Information 会社名？とか？あと何かのフラグ
    uint8_t country = 0x00;
    uint8_t flags = 0x01;
    uint8_t info[] = { 0x11, 0x1, country, flags };
    bleKeyboardInstance->pHidInfoCharacteristic->setValue(info, sizeof(info));
    bleKeyboardInstance->pHidInfoCharacteristic->setCallbacks(&chrCallbacks);

    //HidService-reportMap
    ESP_LOGD(LOG_TAG, "reportMap set: start");
    ESP_LOGD(LOG_TAG, "reportMap set: size %D", bleKeyboardInstance->_hidReportDescriptorSize);
    bleKeyboardInstance->pReportMapCharacteristic = bleKeyboardInstance->pHidService->createCharacteristic("2A4B",NIMBLE_PROPERTY::READ); // HID Report Map (ここでHIDのいろいろ設定
    bleKeyboardInstance->pReportMapCharacteristic->setValue((uint8_t*)bleKeyboardInstance->_hidReportDescriptor, bleKeyboardInstance->_hidReportDescriptorSize);
    bleKeyboardInstance->pReportMapCharacteristic->setCallbacks(&chrCallbacks);

    //HidService-HidControl
    bleKeyboardInstance->pHidControlCharacteristic = bleKeyboardInstance->pHidService->createCharacteristic("2A4C", NIMBLE_PROPERTY::WRITE_NR);// HID Control Point
    bleKeyboardInstance->pHidControlCharacteristic->setCallbacks(&chrCallbacks);

    //HidService-protocolMode
    bleKeyboardInstance->pProtocolModeCharacteristic = bleKeyboardInstance->pHidService->createCharacteristic("2A4E",NIMBLE_PROPERTY::WRITE_NR | NIMBLE_PROPERTY::READ); // Protocol Mode
    const uint8_t pMode[] = { 0x01 }; // 0: Boot Protocol 1: Rport Protocol
    bleKeyboardInstance->pProtocolModeCharacteristic->setValue((uint8_t*) pMode, 1);
    bleKeyboardInstance->pProtocolModeCharacteristic->setCallbacks(&chrCallbacks);

    //HidService-input
    bleKeyboardInstance->pInputCharacteristic = bleKeyboardInstance->pHidService->createCharacteristic("2A4D", NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::NOTIFY | NIMBLE_PROPERTY::READ_ENC | NIMBLE_PROPERTY::WRITE_ENC); // Report
    bleKeyboardInstance->pDesc1 = bleKeyboardInstance->pInputCharacteristic->createDescriptor( "2908", NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::NOTIFY | NIMBLE_PROPERTY::READ_ENC | NIMBLE_PROPERTY::WRITE_ENC, 20); // Report Reference
    uint8_t desc1_val[] = { 0x01, 0x01 }; // Report ID 1 を Input に設定
    bleKeyboardInstance->pDesc1->setValue((uint8_t*) desc1_val, 2);
    bleKeyboardInstance->pDesc1->setCallbacks(&dscCallbacks);


    // HidService-output
    bleKeyboardInstance->pOutputCharacteristic = bleKeyboardInstance->pHidService->createCharacteristic("2A4D", NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::WRITE | NIMBLE_PROPERTY::WRITE_NR | NIMBLE_PROPERTY::READ_ENC | NIMBLE_PROPERTY::WRITE_ENC);
    bleKeyboardInstance->pOutputCharacteristic->setCallbacks(new KeyboardOutputCallbacks());
    bleKeyboardInstance->pDesc2 = bleKeyboardInstance->pOutputCharacteristic->createDescriptor("2908", NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::NOTIFY | NIMBLE_PROPERTY::READ_ENC | NIMBLE_PROPERTY::WRITE_ENC, 20);
    uint8_t desc1_val2[] = { 0x01, 0x02}; // Report ID 1 を Output に設定
    bleKeyboardInstance->pDesc2->setValue((uint8_t*) desc1_val2, 2);


    //HidService-input2 media port
    bleKeyboardInstance->pInputCharacteristic2 = bleKeyboardInstance->pHidService->createCharacteristic("2A4D", NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::NOTIFY | NIMBLE_PROPERTY::READ_ENC | NIMBLE_PROPERTY::WRITE_ENC); // Report
    bleKeyboardInstance->pDesc3 = bleKeyboardInstance->pInputCharacteristic2->createDescriptor("2908", NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::NOTIFY | NIMBLE_PROPERTY::READ_ENC | NIMBLE_PROPERTY::WRITE_ENC, 20);
    uint8_t desc1_val3[] = { 0x02, 0x01 }; // Report ID 2 を Input に設定
    bleKeyboardInstance->pDesc3->setValue((uint8_t*) desc1_val3, 2);
    bleKeyboardInstance->pDesc3->setCallbacks(&dscCallbacks);


    //HidService-input3 media port
    bleKeyboardInstance->pInputCharacteristic3 = bleKeyboardInstance->pHidService->createCharacteristic("2A4D", NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::NOTIFY | NIMBLE_PROPERTY::READ_ENC | NIMBLE_PROPERTY::WRITE_ENC); // Report
    bleKeyboardInstance->pDesc4 = bleKeyboardInstance->pInputCharacteristic3->createDescriptor("2908", NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::NOTIFY | NIMBLE_PROPERTY::READ_ENC | NIMBLE_PROPERTY::WRITE_ENC, 20);
    uint8_t desc1_val4[] = { 0x03, 0x01 }; // Report ID 3 を Input に設定
    bleKeyboardInstance->pDesc4->setValue((uint8_t*) desc1_val4, 2);
    bleKeyboardInstance->pDesc4->setCallbacks(&dscCallbacks);

    
    //BatteryService
    bleKeyboardInstance->pBatteryService = bleKeyboardInstance->pServer->createService("180F");
    bleKeyboardInstance->pBatteryLevelCharacteristic = bleKeyboardInstance->pBatteryService->createCharacteristic("2A19", NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::NOTIFY);
    bleKeyboardInstance->pBatteryLevelDescriptor = (NimBLE2904*)bleKeyboardInstance->pBatteryLevelCharacteristic->createDescriptor("2904"); 
    bleKeyboardInstance->pBatteryLevelDescriptor->setFormat(NimBLE2904::FORMAT_UTF8);
    bleKeyboardInstance->pBatteryLevelDescriptor->setNamespace(1);
    bleKeyboardInstance->pBatteryLevelDescriptor->setUnit(0x27ad);
    bleKeyboardInstance->pBatteryLevelDescriptor->setCallbacks(&dscCallbacks);


    /** Start the services when finished creating all Characteristics and Descriptors */
    bleKeyboardInstance->pDeviceInfoService->start();
    bleKeyboardInstance->pHidService->start();
    bleKeyboardInstance->pBatteryService->start();

    bleKeyboardInstance->pAdvertising = NimBLEDevice::getAdvertising();
    bleKeyboardInstance->pAdvertising->setAppearance(HID_KEYBOARD); //HID_KEYBOARD / HID_MOUSE
    bleKeyboardInstance->pAdvertising->addServiceUUID(bleKeyboardInstance->pHidService->getUUID());
    bleKeyboardInstance->pAdvertising->setScanResponse(true);
    bleKeyboardInstance->pAdvertising->start();

    // bleKeyboardInstance->inputKeyboard = pInputCharacteristic;
    // bleKeyboardInstance->inputMediaKeys = pInputCharacteristic2;
    // bleKeyboardInstance->outputKeyboard = pOutputCharacteristic;

    ESP_LOGD(LOG_TAG, "Advertising started!");
    vTaskDelay(portMAX_DELAY); //delay(portMAX_DELAY);
};

unsigned short BleKeyboardJIS::modifiers_press(unsigned short k) {
  if (k & JIS_SHIFT) { // shift
    this->_keyReport.modifiers |= 0x02; // the left shift modifier
    k &= 0xFF;
  }
  if (k == 224) this->_keyReport.modifiers |= 0x01; // LEFT Ctrl
  if (k == 228) this->_keyReport.modifiers |= 0x10; // RIGHT Ctrl
  if (k == 225) this->_keyReport.modifiers |= 0x02; // LEFT Shift
  if (k == 229) this->_keyReport.modifiers |= 0x20; // RIGHT Shift
  if (k == 226) this->_keyReport.modifiers |= 0x04; // LEFT Alt
  if (k == 230) this->_keyReport.modifiers |= 0x40; // RIGHT Alt
  if (k == 227) this->_keyReport.modifiers |= 0x08; // LEFT GUI
  if (k == 231) this->_keyReport.modifiers |= 0x80; // RIGHT GUI
  return k;
};


unsigned short BleKeyboardJIS::modifiers_release(unsigned short k) {
  if (k & JIS_SHIFT) { // shift
    this->_keyReport.modifiers &= ~(0x02);  // the left shift modifier
    k &= 0xFF;
  }
  if (k == 224) this->_keyReport.modifiers &= ~(0x01); // LEFT Ctrl
  if (k == 228) this->_keyReport.modifiers &= ~(0x10); // RIGHT Ctrl
  if (k == 225) this->_keyReport.modifiers &= ~(0x02); // LEFT Shift
  if (k == 229) this->_keyReport.modifiers &= ~(0x20); // RIGHT Shift
  if (k == 226) this->_keyReport.modifiers &= ~(0x04); // LEFT Alt
  if (k == 230) this->_keyReport.modifiers &= ~(0x40); // RIGHT Alt
  if (k == 227) this->_keyReport.modifiers &= ~(0x08); // LEFT GUI
  if (k == 231) this->_keyReport.modifiers &= ~(0x80); // RIGHT GUI
  return k;
};

// Shiftを離す
void BleKeyboardJIS::shift_release() {
  int i;
  this->_keyReport.modifiers &= ~(0x22);
  for (i=0; i<6; i++) {
    if (this->_keyReport.keys[i] == 225 || this->_keyReport.keys[i] == 229) {
      this->_keyReport.keys[i] = 0;
    }
  }
}

unsigned short BleKeyboardJIS::modifiers_media_press(unsigned short k) {
  if (k == 8193) { // Eject
    this->_mediaKeyReport[0] |= 0x01;
    this->sendReport(&this->_mediaKeyReport);
    return 1;
  } else if (k == 8194) { // Media Next
    this->_mediaKeyReport[0] |= 0x02;
    this->sendReport(&this->_mediaKeyReport);
    return 1;
  } else if (k == 8195) { // Media Previous
    this->_mediaKeyReport[0] |= 0x04;
    this->sendReport(&this->_mediaKeyReport);
    return 1;
  } else if (k == 8196) { // Media Stop
    this->_mediaKeyReport[0] |= 0x08;
    this->sendReport(&this->_mediaKeyReport);
    return 1;
  } else if (k == 8197) { // Media play / pause
    this->_mediaKeyReport[0] |= 0x10;
    this->sendReport(&this->_mediaKeyReport);
    return 1;
  } else if (k == 8198) { // Media Mute
    this->_mediaKeyReport[0] |= 0x20;
    this->sendReport(&this->_mediaKeyReport);
    return 1;
  } else if (k == 8199) { // Media volume +
    this->_mediaKeyReport[0] |= 0x40;
    this->sendReport(&this->_mediaKeyReport);
    return 1;
  } else if (k == 8200) { // Media volume -
    this->_mediaKeyReport[0] |= 0x80;
    this->sendReport(&this->_mediaKeyReport);
    return 1;
  }
  return 0;
};

unsigned short BleKeyboardJIS::modifiers_media_release(unsigned short k) {
  if (k == 8193) { // Eject
    this->_mediaKeyReport[0] &= ~(0x01);
    this->sendReport(&this->_mediaKeyReport);
    return 1;
  } else if (k == 8194) { // Media Next
    this->_mediaKeyReport[0] &= ~(0x02);
    this->sendReport(&this->_mediaKeyReport);
    return 1;
  } else if (k == 8195) { // Media Previous
    this->_mediaKeyReport[0] &= ~(0x04);
    this->sendReport(&this->_mediaKeyReport);
    return 1;
  } else if (k == 8196) { // Media Stop
    this->_mediaKeyReport[0] &= ~(0x08);
    this->sendReport(&this->_mediaKeyReport);
    return 1;
  } else if (k == 8197) { // Media play / pause
    this->_mediaKeyReport[0] &= ~(0x10);
    this->sendReport(&this->_mediaKeyReport);
    return 1;
  } else if (k == 8198) { // Media Mute
    this->_mediaKeyReport[0] &= ~(0x20);
    this->sendReport(&this->_mediaKeyReport);
    return 1;
  } else if (k == 8199) { // Media volume +
    this->_mediaKeyReport[0] &= ~(0x40);
    this->sendReport(&this->_mediaKeyReport);
    return 1;
  } else if (k == 8200) { // Media volume -
    this->_mediaKeyReport[0] &= ~(0x80);
    this->sendReport(&this->_mediaKeyReport);
    return 1;
  }
  return 0;
};

unsigned short BleKeyboardJIS::code_convert(unsigned short k)
{
  short i = 0;
  if (this->keyboard_language == 1) {
    // US キーボード
    while (this->_codemap_us[i]) {
      if (this->_codemap_us[i] == k) {
        return this->_codemap_us[i + 1];
      }
      i += 2;
    }
  } else if (this->keyboard_language == 2) {
    // 日本語キーボード(US記号)
    if (!this->onShift()) return k; // Shift が押されていなければそのまま
    if (k == 31) { this->shift_release(); return 47; } // 2 -> @ (Shiftを離す)
    if (k == 35) { this->shift_release(); return 46; } // 6 -> ^ (Shiftを離す)
    if (k == 36) { return 4131; } // 7 -> &
    if (k == 37) { return 4148; } // 8 -> *
    if (k == 38) { return 4133; } // 9 -> (
    if (k == 39) { return 4134; } // 0 -> )
    if (k == 45) { return 4231; } // - -> _
    if (k == 4141) { return 4147; } // = -> +
    if (k == 51) { this->shift_release(); return 52; } // ; -> : (Shiftを離す)
    if (k == 4132) { return 4127; } // ' -> "
    if (k == 4143) { return 4142; } // ` -> ~
    if (k == 135) { return 4233; } // \ -> |
  }
  return k;
};

void BleKeyboardJIS::sendReport(KeyReport* keys)
{
  if (this->isConnected())
  {
    this->pInputCharacteristic->setValue((uint8_t*)keys, sizeof(KeyReport));
    this->pInputCharacteristic->notify();
  }
};

void BleKeyboardJIS::sendReport(MediaKeyReport* keys)
{
  if (this->isConnected())
  {
    this->pInputCharacteristic2->setValue((uint8_t*)keys, sizeof(MediaKeyReport));
    this->pInputCharacteristic2->notify();
  }
};

void BleKeyboardJIS::mouse_click(uint8_t b)
{
    this->_MouseButtons = b;
    this->mouse_move(0,0,0,0);
    delay(10);
    this->_MouseButtons = 0x00;
    this->mouse_move(0,0,0,0);
};


void BleKeyboardJIS::mouse_press(uint8_t b)
{
    this->_MouseButtons |= b;
    this->mouse_move(0,0,0,0);
};

void BleKeyboardJIS::mouse_release(uint8_t b)
{
    this->_MouseButtons &= ~(b);
    this->mouse_move(0,0,0,0);
};

void BleKeyboardJIS::mouse_move(signed char x, signed char y, signed char wheel, signed char hWheel)
{
    if (this->isConnected()) {
        uint8_t m[5];
        m[0] = this->_MouseButtons;
        m[1] = x;
        m[2] = y;
        m[3] = wheel;
        m[4] = hWheel;
        this->pInputCharacteristic3->setValue(m, 5);
        this->pInputCharacteristic3->notify();
    }
};

size_t BleKeyboardJIS::press_raw(unsigned short k)
{
  uint8_t i;
  unsigned short kk;
  // メディアキー
  if (modifiers_media_press(k)) return 1;
  kk = this->code_convert(k);
  ESP_LOGD(LOG_TAG, "press_raw: %D", kk);
  kk = this->modifiers_press(kk);
  ESP_LOGD(LOG_TAG, "press_raw modifiers: %D", this->_keyReport.modifiers);
  if (this->_keyReport.keys[0] != kk && this->_keyReport.keys[1] != kk &&
    this->_keyReport.keys[2] != kk && this->_keyReport.keys[3] != kk &&
    this->_keyReport.keys[4] != kk && this->_keyReport.keys[5] != kk) {

    for (i=0; i<6; i++) {
      if (this->_keyReport.keys[i] == 0x00) {
        this->_keyReport.keys[i] = kk;
        break;
      }
    }
    if (i == 6) {
      ESP_LOGD(LOG_TAG, "press_raw error: %D", kk);
      return 0;
    }
  }
  this->sendReport(&_keyReport);
  return 1;
};

size_t BleKeyboardJIS::press_set(uint8_t k)
{
  uint8_t i;
  unsigned short kk;
  kk = this->code_convert(this->_asciimap[k]);
  if (!kk) {
    ESP_LOGD(LOG_TAG, "press_set error: %D", k);
    return 0;
  }
  this->_keyReport.modifiers = 0x00;
  kk = this->modifiers_press(kk);
  this->_keyReport.keys[0] = kk;
  this->_keyReport.keys[1] = 0x00;
  this->_keyReport.keys[2] = 0x00;
  this->_keyReport.keys[3] = 0x00;
  this->_keyReport.keys[4] = 0x00;
  this->_keyReport.keys[5] = 0x00;

  this->sendReport(&this->_keyReport);
  return 1;
};

size_t BleKeyboardJIS::release_raw(unsigned short k)
{
  uint8_t i;
  unsigned short kk;
  // メディアキー
  if (modifiers_media_release(k)) return 1;
  kk = this->code_convert(k);
  kk = this->modifiers_release(kk);

  // Test the key report to see if k is present.  Clear it if it exists.
  // Check all positions in case the key is present more than once (which it shouldn't be)
  for (i=0; i<6; i++) {
    if (0 != kk && this->_keyReport.keys[i] == kk) {
      this->_keyReport.keys[i] = 0x00;
    }
  }

  this->sendReport(&this->_keyReport);
  return 1;
};

void BleKeyboardJIS::releaseAll(void)
{
  this->_keyReport.keys[0] = 0;
  this->_keyReport.keys[1] = 0;
  this->_keyReport.keys[2] = 0;
  this->_keyReport.keys[3] = 0;
  this->_keyReport.keys[4] = 0;
  this->_keyReport.keys[5] = 0;
  this->_keyReport.modifiers = 0;
  this->_mediaKeyReport[0] = 0;
  this->_mediaKeyReport[1] = 0;
  this->sendReport(&this->_keyReport);
  this->sendReport(&this->_mediaKeyReport);
};

// Shiftが押されている状態かどうか(物理的に)
bool BleKeyboardJIS::onShift()
{
  int i;
  for (i=0; i<PRESS_KEY_MAX; i++) {
    if (press_key_list[i].key_num < 0) continue; // 押されたよデータ無ければ無視
    if (press_key_list[i].unpress_time > 0) continue; // 離したよカウントが始まっていたら押していないので無視
    if (press_key_list[i].key_id == 225 || press_key_list[i].key_id == 229) return true; // ShiftコードならばShiftが押されている
  }
  return false;
}
