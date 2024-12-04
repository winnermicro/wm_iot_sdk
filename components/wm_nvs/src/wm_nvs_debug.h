/**
 * @file wm_nvs_debug.h
 *
 * @brief nvs debug
 *
 */

/**
 *  Copyright 2022-2024 Beijing WinnerMicroelectronics Co.,Ltd.
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */

#ifndef __WM_NVS_DEBUG_H__
#define __WM_NVS_DEBUG_H__

#include <stdint.h>
#include "wmsdk_config.h"
#include "wm_types.h"
#include "wm_nvs.h"

#ifdef __cplusplus
extern "C" {
#endif

#define WM_NVS_TRACE_POWER_OFF(type, stage) wm_nvs_port_power_off(type, stage)

/*power off type */
enum {
    WM_NVS_TRACE_TYPE_WRITE,
    WM_NVS_TRACE_TYPE_BLOB,
    WM_NVS_TRACE_TYPE_GC,
};

/*power off stage for normal write type*/
enum {
    WM_NVS_TRACE_BEFORE_WRITE_DATA,
    WM_NVS_TRACE_BEFPRE_ERASE_OLD,
};

/*power off stage for BLOB type*/
enum {
    WM_NVS_TRACE_AFTER_WRITE_A_SEG,
    WM_NVS_TRACE_AFTER_WRITE_ALL_SEG,
    WM_NVS_TRACE_AFTER_MODIFY_NEW,
    WM_NVS_TRACE_AFTER_ERASE_OLD_DESC,
};

/*power off stage for garbage collection*/
enum {
    WM_NVS_TRACE_GC_1_NEW_SECTOR,
    WM_NVS_TRACE_GC_2_SET_OLD_DELETING,
    WM_NVS_TRACE_GC_3_COPY,
    WM_NVS_TRACE_GC_4_ERASE_OLD,
};

int wm_nvs_debug_print_sector(const char *partition_name, int index, int size);

int wm_nvs_debug_print_status(const char *partition_name);

int wm_nvs_debug_print_storage(int storage_detail, int sec_detail);

int wm_nvs_debug_print_handle(void);

void wm_nvs_debug_print_value(const char *key, wm_nvs_type_t type, const void *p, size_t length);

void wm_nvs_debug_cmd(int argc, char *argv[]);

void wm_nvs_port_power_off(int type, int stage);

int wm_nvs_debug_nvs_open(const char *partition, const char *group, wm_nvs_open_mode_t mode);

int wm_nvs_debug_nvs_close(int id);

int wm_nvs_debug_clear_partiton_handle(const char *partition);

wm_nvs_handle_t wm_nvs_debug_nvs_get_handle(int id);

bool wm_nvs_debug_nvs_check_handle_id(int id);

int wm_nvs_debug_cmd_get_item(int id, const char *key);

int wm_nvs_debug_set_item_with_size(int id, const char *key, const char *str_size, wm_nvs_type_t type);

int wm_nvs_debug_set_base_item(int id, const char *key, const char *type, const char *value);

int wm_nvs_debug_del(int id, const char *key);

int wm_nvs_debug_del_group(int id);

#ifdef __cplusplus
}
#endif

#endif
