// AZ-60JIS �p�ݒ�


// �����N�����̃f�t�H���g�ݒ�
#include "setting_az66jp_default.h"
#include "../image/common.h"
#include "../image/led_setting.h"
#include "../image/az_66jp_img.h"



// �L�[�{�[�h�̒�`
#define  KEYBOARD_AZ66JP

// ESP32�̎�� (0 = ESP32 / 1 = ESP32-WROVER)
#define  AZESP32_TYPE  1

// �������ɕێ�����L�[�̐�(���������m�ۂ���T�C�Y)
#define KEY_INPUT_MAX  70

// ���C���[�؂�ւ������������e��
#define PRESS_KEY_MAX 16

// �}�E�X�ړ��{�^�������������e��
#define PRESS_MOUSE_MAX 4

// WIFI�A�N�Z�X�|�C���g�̖��O
#define WIFI_AP_SSI_NAME    "AZ-66JP"

// Bluetooth �Ō����������\������閼�O
#define BLUETOOTH_SEARCH_NAME    "az-66jp"

// EEPROM �ǂݍ��ݎ��̃T�C�Y
#define EEPROM_BUF_SIZE   256

// WEB�t�b�N�p�̃o�b�t�@�T�C�Y
#define WEBFOOK_BUF_SIZE 512

// �ݒ�JSON�̃o�b�t�@�T�C�Y
#define SETTING_JSON_BUF_SIZE 30720

// �ݒ胂�[�h�ɓ��邽�߂̃L�[�ԍ�
#define SETTING_MODE_KEY 0

// �ËL�{�^���ňËL�ł��鐔
#define ANKEY_DATA_MAX_LENGTH  512

