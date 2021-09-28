#ifndef lvspiffs_h
#define lvspiffs_h

#include <lvgl.h>

#include "FS.h"
#include "SPIFFS.h"

typedef File lv_spiffs_file_t;
typedef File lv_spiffs_dir_t;


lv_fs_res_t open_SPIFFS_file(struct _lv_fs_drv_t * drv, void * file_p, const char * path, lv_fs_mode_t mode);
lv_fs_res_t read_SPIFFS_file(struct _lv_fs_drv_t *drv, void *file_p, void *buf, uint32_t btr, uint32_t *br);
lv_fs_res_t close_SPIFFS_file(struct _lv_fs_drv_t *drv, void *file_p);
lv_fs_res_t seek_SPIFFS_file(lv_fs_drv_t *drv, void *file_p, uint32_t pos);
lv_fs_res_t tell_SPIFFS_file(lv_fs_drv_t *drv, void *file_p, uint32_t *pos_p);
bool init_file_system_driver();


#endif

