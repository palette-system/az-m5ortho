var webhid = {};


// �ڑ�����HID�f�o�C�X
webhid.device = null;

// �f�[�^�����[�h���鎞��step�v�Z�p
webhid.load_index = 0;

// �f�[�^�����[�h���鎞��C�ɉ����R�[�h�󂯎�邩
webhid.load_step = 16;

// �f�[�^�����[�h����e��
webhid.load_length = 0;

// ���[�h�����f�[�^
webhid.load_data = [];

// ���[�h���̃n�b�V�����v�Z����p�̃f�[�^
webhid.load_hash = [];

// �Ō�Ƀ��[�h��������
webhid.last_load_time = 0;

// �Ō�Ƀ��[�h�����|�C���g
webhid.last_load_point = 0;

// �Ō�ɑ������n�b�V���l
webhid.last_send_hash = 0;

// ���[�h���̃t�@�C���p�X
webhid.load_file_path = "";

// �ۑ��f�[�^���M�̃C���f�b�N�X
webhid.save_index = 0;

// �ۑ��f�[�^����C�ɉ����R�[�h���M���邩
webhid.save_step = 0;

// �ۑ��f�[�^�𑗐M����f�[�^�̊J�n�ʒu
webhid.save_seek = 0;

// �ۑ�����f�[�^
webhid.save_data = [];

// �ۑ����̃n�b�V�����v�Z����p�̃f�[�^
webhid.save_hash = [];

// �ۑ����̃t�@�C���p�X
webhid.save_file_path = "";

// HID �� usage ID
webhid.hid_usage = 0x61;
webhid.hid_usage_page = 0xff60;

// HID�ڑ����N�G�X�g�p�p�����[�^
webhid.hid_request_prm = {filters: [{"usagePage": webhid.hid_usage_page, "usage": webhid.hid_usage}]};

// ����M����|�[�g�̔ԍ�
webhid.raw_report_id = 0;

// �C���t�H��\������DIV
webhid.info_div = "";

// �R�}���hID
webhid.command_id = {
    "file_load_start": 0x30, // �t�@�C���ǂݍ��݊J�n
    "file_load_data": 0x31, // �t�@�C���f�[�^�擾
    "file_save_start": 0x32, // �t�@�C���ۑ��J�n
    "file_save_data": 0x33, // �t�@�C���ۑ��f�[�^���M
    "file_save_complate": 0x34, // �t�@�C���ۑ�����
    "none": 0x00 // �󑗐M
};

// ������
webhid.init = function(opt) {
    // �I�v�V�������󂯎��
    let k;
    for (k in opt) {
        webhid[k] = opt[k];
    }
    // crc�e�[�u���쐬
    webhid.crc_table = webhid.get_crctable();
    // �ڑ��C�x���g�o�^
    navigator.hid.addEventListener("connect", webhid.handle_connect);
    // �ڑ��I���C�x���g�o�^
    navigator.hid.addEventListener("disconnect", webhid.handle_disconnect);
};

// �R�}���h�𑗐M
webhid.send_command = function(arr) {
    while (arr.length < webhid.raw_report_id.out_size) arr.push(0x00); // 0�l��
    let cmd = new Uint8Array(arr);
    return webhid.device.sendReport(webhid.raw_report_id.out, cmd);
};

// �~���b�̎��Ԃ��擾
webhid.millis = function() {
    let d = new Date();
    return d.getTime();
};

// �������uint8Array�ɂ���
webhid.str2arr = function(str) {
    return new TextEncoder().encode(str);
};

// uint8Array�𕶎���ɂ���
webhid.arr2str = function(arr) {
    return  new TextDecoder().decode(new Uint8Array(arr))
};

// �����񂩂ǂ����̔���
webhid.is_string = function(v) {
    return (typeof v === "string" || v instanceof String);
}

// crc32 �v�Z�p��crc�e�[�u���擾
webhid.get_crctable = function(){
    var c;
    var crcTable = [];
    for(var n =0; n < 256; n++){
        c = n;
        for(var k =0; k < 8; k++){
            c = ((c&1) ? (0xEDB88320 ^ (c >>> 1)) : (c >>> 1));
        }
        crcTable[n] = c;
    }
    return crcTable;
}

// crc32 �n�b�V���v�Z
webhid.crc32 = function(arr) {
    var crc = 0 ^ (-1);
    for (var i = 0; i < arr.length; i++ ) {
        crc = (crc >>> 8) ^ webhid.crc_table[(crc ^ arr[i]) & 0xFF];
    }
    return Math.floor(crc ^ (-1));
};

// �C���t�H���[�V�����\��
webhid.view_info = function(msg) {
    if (!webhid.info_div) {
        console.log("info: " + msg);
        return;
    }
    $("#" + webhid.info_div).html(msg);
};

// HID�ڑ��������Ăяo�����C�x���g
webhid.handle_connect = function(e) {
    console.log("Device connected: " + e.device.productName);
};

// HID�ڑ��I���������ɌĂяo�����C�x���g
webhid.handle_disconnect = function(e) {
    webhid.device = null; // �f�o�C�X
    webhid.raw_report_id = 0; // ����M���Ă������|�[�g�ԍ�
    console.log("Device disconnected: " + e.device.productName);
};

// �f�[�^���󂯎�������̃C�x���g
webhid.handle_input_report = function(e) {
    // �S�Ẵ|�[�g�̃C���v�b�g�ł��̃C�x���g����������̂�raw�ȊO�̃|�[�g�̃C�x���g�͖���
    if (e.reportId != webhid.raw_report_id.in) return;
    // �f�[�^��Uint8Array�ɂ���
    let get_data = new Uint8Array(e.data.buffer);
    // console.log("get");
    // console.log(get_data);
    let cmd_type = get_data[0];
    let cmd;
    let l, i, j, h, p, s;
    if (cmd_type == webhid.command_id.file_load_start) {
        // �t�@�C���ǂݍ��݊J�n(�t�@�C���L���Ɨe�ʂ��A���ė���)
        if (!get_data[1]) { // �t�@�C��������
            webhid.view_info("�t�@�C�������݂��܂���ł����B");
            return;
        }
        // �e�ʎ擾
        webhid.load_length = (get_data[2] << 24) + (get_data[3] << 16) + (get_data[4] << 8) + get_data[5];
        // ��̃��[�h�f�[�^���쐬(�����Ƀf�[�^�����Ă���)
        webhid.load_data = [];
        for (i=0; i<webhid.load_length; i++) webhid.load_data.push(0x00);
        // �f�[�^�擾�R�}���h�쐬
        cmd = [webhid.command_id.file_load_data, webhid.load_step, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00];
        // �R�}���h���M
        webhid.load_index = 0;
        webhid.load_hash = [];
        webhid.last_load_time = webhid.millis(); // �Ō�ɃR�}���h�𓊂�������
        webhid.last_load_point = 0; // �Ō�Ƀ��N�G�X�g�����|�C���g
        webhid.last_send_hash = 0;
        webhid.send_command(cmd).then(() => {
            webhid.view_info("loading... [ 0 / "+webhid.load_length+" ]");
        });
        // ���[�h�Ď������J�n
        webhid.file_load_check();

    } else if (cmd_type == webhid.command_id.file_load_data) {
        // �t�@�C���ǂݍ���(�f�[�^�擾)
        i = 0;
        s = get_data.length - 4; // �󂯎�����f�[�^�̃T�C�Y�擾-4�̓R�}���h�ƊJ�n�ʒu��
        p = (get_data[1] << 16) + (get_data[2] << 8) + get_data[3]; // �󂯎�����f�[�^�̊J�n�ʒu�擾
        // console.log(p + " / " + webhid.load_length);
        // 1�o�C�g���f�[�^�擾
        while (i < s) {
            webhid.load_data[p + i] = get_data[i + 4];
            webhid.load_hash.push(get_data[i + 4]);
            i++;
            if ((p + i) >= webhid.load_length) break; // �t�@�C���e�ʕ��S���ǂݍ��񂾂�I��
        }
        webhid.load_index++; // step�����Z
        if (webhid.load_index >= webhid.load_step && (p + i) < webhid.load_length) {
            // step�I����āA�܂��f�[�^�S���ǂ߂Ė�����Ύ��̃f�[�^�擾�R�}���h�𑗐M
            webhid.last_send_hash = webhid.crc32(webhid.load_hash);
            webhid.load_index = 0; // step�����Z�b�g
            webhid.load_hash = []; // �n�b�V���f�[�^�����Z�b�g
            m = p + i; // ���ǂݍ��ފJ�n�ʒu
            // �f�[�^�v���R�}���h�쐬
            cmd = [
                webhid.command_id.file_load_data,
                webhid.load_step,
                ((m >> 16) & 0xff), // �ǂݍ��݊J�n�ʒu 1
                ((m >> 8) & 0xff),  // �ǂݍ��݊J�n�ʒu 2
                (m & 0xff),         // �ǂݍ��݊J�n�ʒu 3
                ((webhid.last_send_hash >> 24) & 0xff), // �n�b�V��1
                ((webhid.last_send_hash >> 16) & 0xff), // �n�b�V��2
                ((webhid.last_send_hash >> 8) & 0xff),  // �n�b�V��3
                (webhid.last_send_hash & 0xff) ];       // �n�b�V��4
            // �R�}���h���M
            webhid.last_load_time = webhid.millis(); // �Ō�ɃR�}���h�𓊂�������
            webhid.last_load_point = m; // �Ō�Ƀ��N�G�X�g�����|�C���g
            webhid.send_command(cmd).then(() => {
                webhid.view_info("loading...  "+m+" / "+webhid.load_length+" ");
            });
        } else if ((p + i) >= webhid.load_length) {
            // �Ō�܂Ŏ擾�ł����犮��
            webhid.load_file_path = "";
            let str = webhid.arr2str(webhid.load_data); // �z����e�L�X�g�ɕϊ�
            webhid.view_info("<textarea id='json_area' style='width: 800px; height:300px;'>"+str+"</textarea>");
        }
        
    } else if (cmd_type == webhid.command_id.file_save_data) {
        // �t�@�C���ۑ��̃f�[�^�v��
        webhid.save_step = get_data[1]; // ���M����X�e�b�v�����擾
        s = webhid.save_seek;
        webhid.save_seek = (get_data[2] << 24) + (get_data[3] << 16) + (get_data[4] << 8) + get_data[5]; // �n���f�[�^�̊J�n�ʒu�擾
        p = (get_data[6] << 24) + (get_data[7] << 16) + (get_data[8] << 8) + get_data[9]; // �O�񑗂����f�[�^�̃n�b�V��(M5Stack�����v�Z�����l)
        h = webhid.crc32(webhid.save_hash);
        // console.log(webhid.save_hash);
        if (h == p) {
            console.log("OK : [" + h + "  " + p + "] [" + webhid.save_seek + " ]");
        } else {
            console.error("NG : [" + h + "  " + p + "] [" + webhid.save_seek + " -> "+s+"]");
            webhid.save_seek = s;
        }
        webhid.save_index = 0; // �X�e�b�v�ʒu��0��
        webhid.save_hash = [];
        webhid.last_save_time = webhid.millis(); // �Ō�ɃR�}���h�𓊂�������
        webhid.send_save_data(); // �ۑ��f�[�^�̑��M

    } else if (cmd_type == webhid.command_id.file_save_complate) {
        // �t�@�C���ۑ������ʒm
        webhid.save_data = []; // �ۑ��f�[�^�N���A
        webhid.save_file_path = ""; // �ۑ��t�@�C�����N���A
        webhid.view_info("save complate!");
        
    }
    
};

// �ۑ��f�[�^�̑��M
webhid.send_save_data = function() {
    let data_len = webhid.raw_report_id.out_size - 5; // 1�X�e�b�v�ő���f�[�^�̒���
    // �R�}���h�쐬
    let cmd = [
        webhid.command_id.file_save_data,
        webhid.save_index,
        ((webhid.save_seek >> 16) & 0xff),
        ((webhid.save_seek >> 8) & 0xff),
        (webhid.save_seek & 0xff)
    ];
    // �R�}���h�Ƀf�[�^���i�[
    let i = webhid.save_seek + (data_len * webhid.save_index);
    while (cmd.length < webhid.raw_report_id.out_size) {
        if (i >= webhid.save_data.length) {
            cmd.push(0x00);
        } else {
            cmd.push(webhid.save_data[i]);
            webhid.save_hash.push(webhid.save_data[i]);
            i++;
        }
    }
    let p = (webhid.save_seek + (webhid.save_index * (webhid.raw_report_id.out_size - 5)));
    if (p > webhid.save_data.length) p = webhid.save_data.length;
    webhid.view_info("saving ...  " + p + " / " + webhid.save_data.length);
    // �R�}���h���M
    // console.log("send");
    // console.log(cmd);
    webhid.send_command(cmd).then(() => {
        webhid.save_index++;
        if (webhid.save_index < webhid.save_step) {
            setTimeout(webhid.send_save_data, 0);
        }
    });
};

// ���[�h���~�܂��Ė������Ď�
webhid.file_load_check = function() {
    // ���[�h���������Ă�����Ď��I��
    if (!webhid.load_file_path) return;
    let t = webhid.millis() - webhid.last_load_time; // �Ō�ɑ����Ă���ǂꂭ�炢���Ԃ���������
    if (t > 2000) { // 2�b�ȏ�f�[�^���󂯎��Ȃ���΂������f�[�^�v���R�}���h�𑗐M
        // �f�[�^�v���R�}���h�쐬
        if (webhid.load_step > 1) webhid.load_step--; // ��x�ɑ���f�[�^�������炷
        webhid.load_index = 0; // step�����Z�b�g
        webhid.load_hash = []; // �n�b�V���f�[�^�����Z�b�g
        let cmd = [
            webhid.command_id.file_load_data,
            webhid.load_step,
            ((webhid.last_load_point >> 16) & 0xff),
            ((webhid.last_load_point >> 8) & 0xff),
            (webhid.last_load_point & 0xff),
            0x00, 0x00, 0x00, 0x00 ]; // �n�b�V���l0�Ńn�b�V���`�F�b�N�֌W�Ȃ��w�肵���A�h���X�̃f�[�^��v���ł���
        // �R�}���h���M
        webhid.last_load_time = webhid.millis(); // �Ō�ɃR�}���h�𓊂�������
        webhid.send_command(cmd).then(() => {
            webhid.view_info("loading... [ "+webhid.last_load_point+" / "+webhid.load_length+" ]");
        });
    }
    // 1�b�����Ƀ`�F�b�N
    setTimeout(webhid.file_load_check, 1000);
};

// �ۑ����~�܂��Ă��Ȃ����Ď�
webhid.file_save_check = function() {
    // �ۑ����������Ă�����Ď��I��
    if (!webhid.save_file_path) return;
    let t = webhid.millis() - webhid.last_save_time; // �Ō�ɑ����Ă���ǂꂭ�炢���Ԃ���������
    if (t > 2000) { // 2�b�ȏ�f�[�^���󂯎��Ȃ���΂������f�[�^���M
        console.log("save command resend: " + webhid.save_seek);
        webhid.save_index = 0; // �X�e�b�v�ʒu��0��
        webhid.save_hash = [];
        webhid.last_save_time = webhid.millis(); // �Ō�ɃR�}���h�𓊂�������
        webhid.send_save_data(); // �ۑ��f�[�^�̑��M
    }
    // 1�b�����Ƀ`�F�b�N
    setTimeout(webhid.file_save_check, 1000);
}

// HID�@��֐ڑ�
webhid.connect = function() {
    // �f�o�C�X�̑I��
    navigator.hid.requestDevice(webhid.hid_request_prm).then((devices) => {
        if (!devices.length || !devices[0] || !devices[0].collections) {
            webhid.view_info("�@�킪������܂���ł����B");
            return;
        }
        // �ڑ����J�n
        devices[0].open().then(() => {
            // ����M���郌�|�[�g�ԍ��擾
            let r = webhid.get_report_id(devices[0]);
            if (!r.in || !r.out) {
                webhid.view_info("�ڑ��ł��܂���ł����B");
                return;
            }
            webhid.raw_report_id = r;
            // �ڑ�����
            webhid.device = devices[0];
            // �f�[�^���󂯎�������̃C�x���g�o�^
            webhid.device.addEventListener("inputreport", webhid.handle_input_report);
        });
    });
};

// raw�f�[�^�𑗎�M����|�[�gID���擾����
webhid.get_report_id = function(d) {
    let i;
    let r = {"in": 0, "in_size": 0, "out": 0, "out_size": 0};
    // �f�o�C�X�̏�񂪖������0
    if (!d.collections || !d.collections.length) return r;
    // �Y���̃|�[�g�ԍ���T��
    for (i in d.collections) {
        if (d.collections[i].usage == webhid.hid_usage && d.collections[i].usagePage == webhid.hid_usage_page) {
            r.in = d.collections[1].inputReports[0].reportId; // ���̓|�[�gID
            r.in_size = d.collections[1].inputReports[0].items[0].reportCount; // ���̓|�[�g�̃T�C�Y(32byte)
            r.out = d.collections[1].outputReports[0].reportId; // �o�̓|�[�gID
            r.out_size = d.collections[1].outputReports[0].items[0].reportCount; // �o�̓|�[�g�̃T�C�Y(32bytp)
            break;
        }
    }
    // �Y������|�[�g��Ԃ�(�������0)
    return r;
};

// �t�@�C���̃f�[�^���擾����
webhid.get_file = function(file_path) {
    // �R�}���h���쐬
    webhid.load_file_path = file_path;
    let file_path_arr = webhid.str2arr(file_path);
    let cmd = [webhid.command_id.file_load_start];
    let i;
    for (i=0; i<file_path_arr.length; i++) {
        cmd.push(file_path_arr[i]);
    }
    if (cmd.length > 30) {
        webhid.view_info("�t�@�C�������������܂��B [ "+file_path+" ]");
        return;
    }
    // �R�}���h���M
    webhid.send_command(cmd).then(() => {
        webhid.view_info("loading ...");
    });
};

// �t�@�C���Ƀf�[�^����������
webhid.save_file = function(file_path, file_data) {
    // �ۑ�����f�[�^���擾(uint8array�ŕێ�)
    webhid.save_data = (webhid.is_string(file_data))? webhid.str2arr(file_data): new Uint8Array(file_data);
    webhid.save_hash = [];
    webhid.save_seek = 0;
    webhid.save_file_path = file_path;
    webhid.last_save_time = webhid.millis(); // �Ō�ɃR�}���h�𓊂�������
    let file_path_arr = webhid.str2arr(file_path); // �t�@�C���p�X��uint8Array�ɕϊ�
    let data_len = webhid.save_data.length; // �ۑ�����t�@�C���̗e��
    // �R�}���h�쐬
    let cmd = [
        webhid.command_id.file_save_start,
        ((data_len >> 24) & 0xff),
        ((data_len >> 16) & 0xff),
        ((data_len >> 8) & 0xff),
        (data_len & 0xff),
        4, // �ۑ��f�[�^���M�̃X�e�b�v���w��
    ];
    let i;
    for (i=0; i<file_path_arr.length; i++) {
        cmd.push(file_path_arr[i]);
    }
    if (cmd.length > 30) {
        webhid.view_info("�t�@�C�������������܂��B [ "+file_path+" ]");
        return;
    }
    // �R�}���h���M
    webhid.send_command(cmd).then(() => {
        webhid.view_info("saving ...");
    });
    // �ۑ��f�[�^���M���~�܂�Ȃ����`�F�b�N�J�n
    webhid.file_save_check();
};


