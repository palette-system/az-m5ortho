#include <NimBLEServer.h>

#include "Arduino.h"
#include "ble_callbacks.h"

#if defined(CONFIG_ARDUHAL_ESP_LOG)
  #include "esp32-hal-log.h"
  #define LOG_TAG ""
#else
  #include "esp_log.h"
  static const char* LOG_TAG = "NimBLEDevice";
#endif


/* ====================================================================================================================== */
/** Characteristic コールバック クラス */
/* ====================================================================================================================== */

CharacteristicCallbacks::CharacteristicCallbacks(void) {
};

void CharacteristicCallbacks::onRead(NimBLECharacteristic* pCharacteristic){
    // Serial.print(pCharacteristic->getUUID().toString().c_str());
    // Serial.print(": onRead(), value: ");
    // Serial.println(pCharacteristic->getValue().c_str());
};

void CharacteristicCallbacks::onWrite(NimBLECharacteristic* pCharacteristic) {
    // Serial.print(pCharacteristic->getUUID().toString().c_str());
    // Serial.print(": onWrite(), value: ");
    // Serial.println(pCharacteristic->getValue().c_str());
};

void CharacteristicCallbacks::onNotify(NimBLECharacteristic* pCharacteristic) {
    // Serial.println("Sending notification to clients");
};

void CharacteristicCallbacks::onStatus(NimBLECharacteristic* pCharacteristic, Status status, int code) {
    String str = ("Notf/Ind stscode: ");
    str += status;
    str += ", retcode: ";
    str += code;
    str += ", "; 
    str += NimBLEUtils::returnCodeToString(code);
    //Serial.print(str);
};

void CharacteristicCallbacks::onSubscribe(NimBLECharacteristic* pCharacteristic, ble_gap_conn_desc* desc, uint16_t subValue) {
    String str = "Client ID: ";
    str += desc->conn_handle;
    str += " Address: ";
    str += std::string(NimBLEAddress(desc->peer_ota_addr)).c_str();
    if(subValue == 0) {
        str += " Unsubscribed to ";
    }else if(subValue == 1) {
        str += " Subscribed to notfications for ";
    } else if(subValue == 2) {
        str += " Subscribed to indications for ";
    } else if(subValue == 3) {
        str += " Subscribed to notifications and indications for ";
    }
    str += std::string(pCharacteristic->getUUID()).c_str();

    // Serial.println(str);
};

/* ====================================================================================================================== */
/** Descriptor コールバック クラス */
/* ====================================================================================================================== */

DescriptorCallbacks::DescriptorCallbacks(void) {
};

void DescriptorCallbacks::onWrite(NimBLEDescriptor* pDescriptor) {
    std::string dscVal((char*)pDescriptor->getValue(), pDescriptor->getLength());
    // Serial.print("Descriptor witten value:");
    // Serial.println(dscVal.c_str());
};

void DescriptorCallbacks::onRead(NimBLEDescriptor* pDescriptor) {
    // Serial.print(pDescriptor->getUUID().toString().c_str());
    // Serial.println(" Descriptor read");
};

/* ====================================================================================================================== */
/** コネクションクラス */
/* ====================================================================================================================== */

BleConnectionStatus::BleConnectionStatus(void) {
};

void BleConnectionStatus::onConnect(NimBLEServer* pServer)
{
  this->connected = true;
};

void BleConnectionStatus::onDisconnect(NimBLEServer* pServer)
{
  this->connected = false;
};

/* ====================================================================================================================== */
/** Output コールバック クラス */
/* ====================================================================================================================== */

KeyboardOutputCallbacks::KeyboardOutputCallbacks(void) {
}

void KeyboardOutputCallbacks::onWrite(NimBLECharacteristic* me) {
  uint8_t* value = (uint8_t*)(me->getValue().c_str());
  ESP_LOGI(LOG_TAG, "special keys: %d", *value);
}
