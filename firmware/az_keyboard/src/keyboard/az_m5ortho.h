// AZ-M5ortho �p�ݒ�

// �����N�����̃f�t�H���g�ݒ�
#include "setting_m5ortho_default.h"
#include "../image/common.h"
#include "../image/az_m5ortho_img.h"

// �L�[�{�[�h�̒�`
#define KEYBOARD_AZM5ORTHO

// ESP32�̎�� (0 = ESP32 / 1 = ESP32-WROVER)
#define  AZESP32_TYPE  1

// M5Stack�̉�ʂ̌���(0 = �c�� / 1 = ����)
#define  DISPLAY_ROTATION  0

// �������ɕێ�����L�[�̐�(���������m�ۂ���T�C�Y)
#define KEY_INPUT_MAX  55

// ���C���[�؂�ւ������������e��
#define PRESS_KEY_MAX 16

// �}�E�X�ړ��{�^�������������e��
#define PRESS_MOUSE_MAX 4

// WIFI�A�N�Z�X�|�C���g�̖��O
#define WIFI_AP_SSI_NAME    "AZ-M5ortho"

// Bluetooth �Ō����������\������閼�O
#define BLUETOOTH_SEARCH_NAME    "az-m5ortho"

// EEPROM �ǂݍ��ݎ��̃T�C�Y
#define EEPROM_BUF_SIZE   256

// WEB�t�b�N�p�̃o�b�t�@�T�C�Y
#define WEBFOOK_BUF_SIZE 512

// �ݒ�JSON�̃o�b�t�@�T�C�Y
#define SETTING_JSON_BUF_SIZE 131072

// �ݒ胂�[�h�ɓ��邽�߂̃L�[�ԍ�
#define SETTING_MODE_KEY 11

// �ËL�{�^���ňËL�ł��鐔
#define ANKEY_DATA_MAX_LENGTH  32
