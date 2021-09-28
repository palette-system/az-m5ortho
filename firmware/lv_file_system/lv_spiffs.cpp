// LVGL �t�@�C���V�X�e�����g�p���� ESP32��SPIFFS��ɂ���摜�t�@�C����\�����邽�߂̃t�@�C���V�X�e���\�[�X
// �r���܂ł�������ǂ������(1��ڂ̉摜�\���͂��܂��s�����ǂQ��ډ摜��\�����鎞�u(*fp) = file;�v��ESP32��������)
// File �̍\�����ς���� lv_spiffs_file_t �ɍ���Ȃ��Ȃ����̂ł́H
// 1��ڂ�Open���āAOpen����Ă����2��ڈȍ~�͉������Ȃ��݂����ȃ\�[�X�����邯�ǂ��������ESP�������邵�摜�t�@�C��������
// 2��ڈȍ~Open����Ă����Close���悤�Ƃ�����Close�ŗ�����BSeek��0�w�����������Seek�ŗ�����
// ���������\���I��������close����Ȃ��ƃ_���Ȃ̂ł́H
// �����Ƃ��Ă�
// 1��� Open->read(4byte)->close->open->seek(4)->read(16k���炢)->seek.....(�ǂݍ��݂Ȃ���摜���\��
// 2��� Open������
// 
// �Q�l�ɂ����y�[�W
// https://forum.lvgl.io/t/file-system-callback-questions/3452  <- �R�R�̃\�[�X�R�s�y����
// https://docs.lvgl.io/latest/en/html/overview/file-system.html
// https://docs.lvgl.io/latest/en/html/widgets/img.html
// https://lvgl.io/tools/imageconverter
// https://github.com/lvgl/lvgl/issues/1453
// https://github.com/Xinyuan-LilyGO/Twatch-example/blob/master/setup.cpp
// 
// lvgl�̉摜bin�t�@�C���̓�4�o�C�g�ɓ����Ă�摜�T�C�Y�̃f�[�^�\��
// b0 = 0x04;
// b1 = ((x & 0x3f) << 2);
// b2 = ((y & 0x07) << 5) | ((x & 0x07c0) >> 6);
// b3 = ((y & 0x07f8) >> 3);
// 
// img�I�u�W�F�N�g�̍���
//	lv_obj_t * icon = lv_img_create(lv_scr_act(), NULL);
//	lv_img_set_src(icon, "S:logo.bin");
//	lv_obj_align(icon, NULL, LV_ALIGN_CENTER, 0, 0);
// 
// �e�X�g�Ŏg���Ă��摜�t�@�C���̍���
// #include "logo_img.h"
// File fp = SPIFFS.open("/logo.bin", FILE_WRITE);
// fp.write((uint8_t *)&logo_bin_bin, 17124);
// fp.close();


#include "lv_spiffs.h"

lv_fs_res_t open_SPIFFS_file(struct _lv_fs_drv_t * drv, void * file_p, const char * path, lv_fs_mode_t mode) 
{
    (void) drv; /*Unused*/

    char filename[32];
    snprintf_P(filename, sizeof(filename), PSTR("/%s"), path);    

    File file = SPIFFS.open(filename, mode == LV_FS_MODE_WR ? FILE_WRITE : FILE_READ);

    if(!file) {
        return LV_FS_RES_NOT_EX;

    } else if(file.isDirectory()) {
        return LV_FS_RES_UNKNOWN;

    } else {
        lv_spiffs_file_t * fp = (lv_spiffs_file_t *)file_p; /*Just avoid the confusing casings*/
        if (fp != NULL) (*fp) = file;
        return LV_FS_RES_OK;
    }}

lv_fs_res_t read_SPIFFS_file(struct _lv_fs_drv_t *drv, void *file_p, void *buf, uint32_t btr, uint32_t *br)
{
    (void) drv; /*Unused*/

ESP_LOGD(LOG_TAG, "read_SPIFFS_file: read start %D %D", file_p, btr );
    lv_spiffs_file_t *fp = (lv_spiffs_file_t *)file_p;
    lv_spiffs_file_t file = *fp;

    if (!file) {
ESP_LOGD(LOG_TAG, "read_SPIFFS_file: read end A");
        return LV_FS_RES_NOT_EX;

    } else {
ESP_LOGD(LOG_TAG, "read_SPIFFS_file: read end B");
        *br = (uint32_t)file.readBytes((char *)buf, btr);
        return LV_FS_RES_OK;
    }    
}

lv_fs_res_t close_SPIFFS_file(struct _lv_fs_drv_t *drv, void *file_p)
{
    (void) drv; /*Unused*/

ESP_LOGD(LOG_TAG, "close_SPIFFS_file: close start %D", file_p );
    lv_spiffs_file_t file = *(lv_spiffs_file_t *)file_p;

    if (!file) {
ESP_LOGD(LOG_TAG, "close_SPIFFS_file: close end A" );
        return LV_FS_RES_NOT_EX;

    } else if(file.isDirectory()) {
ESP_LOGD(LOG_TAG, "close_SPIFFS_file: close end B" );
        return LV_FS_RES_UNKNOWN;

    } else {
ESP_LOGD(LOG_TAG, "close_SPIFFS_file: close end C" );
        file.close();
        return LV_FS_RES_OK;
    }    
}

lv_fs_res_t seek_SPIFFS_file(lv_fs_drv_t *drv, void *file_p, uint32_t pos)
{
    (void) drv; /*Unused*/

ESP_LOGD(LOG_TAG, "seek_SPIFFS_file: seek start %D, %D", file_p, pos );
    lv_spiffs_file_t file = *(lv_spiffs_file_t *)file_p;

    if (!file) {
ESP_LOGD(LOG_TAG, "seek_SPIFFS_file: seek end A");
        return LV_FS_RES_NOT_EX;

    } else {
ESP_LOGD(LOG_TAG, "seek_SPIFFS_file: seek end B");
        file.seek(pos, SeekSet);
        return LV_FS_RES_OK;
    }
}

lv_fs_res_t tell_SPIFFS_file(lv_fs_drv_t *drv, void *file_p, uint32_t *pos_p)
{
    (void) drv; /*Unused*/

ESP_LOGD(LOG_TAG, "tell_SPIFFS_file: tell start %D", file_p );
    lv_spiffs_file_t file = *(lv_spiffs_file_t *)file_p;

    if (!file) {
ESP_LOGD(LOG_TAG, "tell_SPIFFS_file: tell end A");
        return LV_FS_RES_NOT_EX;

    } else {
ESP_LOGD(LOG_TAG, "tell_SPIFFS_file: tell end B");
        *pos_p = (uint32_t)file.position();
        return LV_FS_RES_OK;
    }    
}

bool init_file_system_driver() 
{
    lv_fs_drv_t drv;
    lv_fs_drv_init(&drv);                     /*Basic initialization*/

    drv.letter = 'S';                         /*An uppercase letter to identify the drive */
    drv.file_size = sizeof(lv_spiffs_file_t);   /*Size required to store a file object*/
    drv.rddir_size = sizeof(lv_spiffs_dir_t);   /*Size required to store a directory object (used by dir_open/close/read)*/
    drv.ready_cb = NULL; //my_ready_cb;               /*Callback to tell if the drive is ready to use */
    drv.open_cb = open_SPIFFS_file;                 /*Callback to open a file */
    drv.close_cb = close_SPIFFS_file;                /*Callback to close a file */
    drv.read_cb = read_SPIFFS_file;                 /*Callback to read a file */
    drv.write_cb = NULL; //my_write_cb;               /*Callback to write a file */
    drv.seek_cb = seek_SPIFFS_file;                 /*Callback to seek in a file (Move cursor) */
    drv.tell_cb = tell_SPIFFS_file;                 /*Callback to tell the cursor position  */
    drv.trunc_cb = NULL; //my_trunc_cb;               /*Callback to delete a file */
    drv.size_cb = NULL; //my_size_cb;                 /*Callback to tell a file's size */
    drv.rename_cb = NULL; //my_rename_cb;             /*Callback to rename a file */

    drv.dir_open_cb = NULL; //my_dir_open_cb;         /*Callback to open directory to read its content */
    drv.dir_read_cb = NULL; //my_dir_read_cb;         /*Callback to read a directory's content */
    drv.dir_close_cb = NULL; //my_dir_close_cb;       /*Callback to close a directory */

    drv.free_space_cb = NULL; //my_free_space_cb;     /*Callback to tell free space on the drive */

    // drv.user_data = my_user_data;             /*Any custom data if required*/

    lv_fs_drv_register(&drv);                 /*Finally register the drive*/


    return true;
}
