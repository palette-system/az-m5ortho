#include "Arduino.h"
#include "dakey.h"


// �R���X�g���N�^
Dakey::Dakey() {
}


// �Ō����N���X������
void Dakey::begin() {
	this->last_save_key_count = -1;
	this->last_save_time = 0;
	// �Ƃ肠�����ߋ����̑Ō����͖���폜
	common_cls.delete_indexof_all("D_");
	// �Ō�����ۑ�(�ŏ���0��ۑ�)(Wifi�ɂȂ����Ă��Ď����ۑ�ON�Ȃ��)
	if (wifi_conn_flag && key_count_auto_save) {
		this->save_dakey(0);
	}
}


// �Ō��������ۑ����邩�ǂ����̐ݒ��ύX
void Dakey::set_auto_save_change() {
	if (key_count_auto_save) {
		// OFF�ɂ���
		key_count_auto_save = 0;
	} else {
		// ON�ɂ���
		key_count_auto_save = 1;
		// ���݂̑Ō�����ۑ�
		this->save_dakey(0);
	}
	common_cls.save_file_data(KEY_COUNT_AUTO_SAVE_PATH, (uint8_t *)&key_count_auto_save, 1);
	disp->view_dakey_auto_save();
}


// ���݂̑Ō�����ۑ�����
void Dakey::save_dakey(uint8_t view_flag) {
	char save_path[32];
	String date_now;
	int i;
	// WiFi�ɂȂ����Ė�����Ή������Ȃ�
	if (!wifi_conn_flag) {
		// WiFi�ɐڑ����ĉ������G���[�\��
		// if (common_cls.on_tft_unit()) {
		//	disp->view_error_wifi_conn();
		// }
		return;
	}
	// �t�@�C���̋󂫗e�ʂ�10kb����������牽�����Ȃ�
	int size_free = common_cls.spiffs_total() - common_cls.spiffs_used();
	if (size_free < 10240) {
		return;
	}
	// WEB���猻�ݓ������擾
	date_now = common_cls.send_webhook_simple("http://azkey.jp/api/date.php");
	// �t�@�C���ɏ�������
	File fp;
	sprintf(save_path, "/D_%06D", boot_count);
	if (SPIFFS.exists(save_path)){
		// �t�@�C��������ΒǋL
		fp = SPIFFS.open(save_path, "a");
	} else {
		// �t�@�C����������ΐV�K�쐬
		fp = SPIFFS.open(save_path, "w");
		fp.printf("uid=%S&boot=%D&data=", eep_data.uid, boot_count);
	}
	if (!fp) {
		// �t�@�C�������ݎ��s
		return;
	}
	// ������������
	fp.print(date_now);
	if (common_cls.key_count_total == 0) {
		// �N�����čŏ��̏�����
		fp.print(",0");
	} else if (this->last_save_key_count == common_cls.key_count_total) {
		// �Ō�ɏ������񂾑Ō������瑝���ĂȂ�
		fp.print(",1");
	} else {
		// �Ō�����������
		fp.print(",2,");
		// �Ō��f�[�^��URL�ɒǉ�����
		for (i=0; i<key_input_length; i++) {
			fp.printf("%04x", common_cls.key_count[i]);
		}
	}
	fp.print("\n");
	fp.close();
	this->last_save_key_count = common_cls.key_count_total;
	this->last_save_time = millis();
	// �Ō�����ۑ����܂�����\��
	if (view_flag) disp->view_dakey_save_comp();
	
}


// �������
void Dakey::loop_exec() {
	// �����ۑ�����
	if (wifi_conn_flag && // Wifi�Ɍq�����Ă���
		key_count_auto_save && // �����ۑ�ON
		(this->last_save_time + AUTO_SAVE_INTERVAL) < millis() ) { // �Ō�ɕۑ��������Ԃ�����ȏ㎞�Ԃ��o������
		this->save_dakey(1);
	}
}


