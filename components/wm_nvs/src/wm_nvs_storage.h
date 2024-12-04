/**
 * @file wm_nvs_storage.h
 *
 * @brief nvs storage
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

#ifndef __WM_NVS_STORAGE_H__
#define __WM_NVS_STORAGE_H__

#include <stdint.h>

#include "wm_nvs.h"
#include "wm_nvs_porting.h"
#include "wm_nvs_partition.h"
#include "wm_nvs_sector_manager.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    struct dl_list list;               /**< group link node  */
    char name[WM_NVS_MAX_KEY_LEN + 1]; /**< group name       */
    uint8_t id;                        /**< group id         */
} wm_nvs_group_t;

/**
  * @brief  nvs storage structure
  */
typedef struct {
    struct dl_list list;        /**< patition storage list      */
    wm_nvs_partition_t pt;      /**< nvs partition information  */
    wm_nvs_sector_manager_t sm; /**< sector manager information */
    struct dl_list group_list;  /**< group list                 */
} wm_nvs_storage_t;

int wm_nvs_storage_init(wm_nvs_partition_t *pt, wm_nvs_storage_t **storage);
int wm_nvs_storage_deinit(wm_nvs_storage_t *storage);

int wm_nvs_storage_open_group(wm_nvs_storage_t *storage, const char *group, bool create_new, uint8_t *group_id);
int wm_nvs_storage_del_group(wm_nvs_storage_t *storage, uint8_t group_id);

int wm_nvs_storage_write_item(wm_nvs_storage_t *storage, uint8_t group_id, wm_nvs_type_t type, const char *key,
                              const void *data, size_t size);
int wm_nvs_storage_read_item(wm_nvs_storage_t *storage, uint8_t group_id, wm_nvs_type_t type, const char *key, void *data,
                             size_t *size);
int wm_nvs_storage_del_item(wm_nvs_storage_t *storage, uint8_t group_id, wm_nvs_type_t type, const char *key);

int wm_nvs_storage_find_key(wm_nvs_storage_t *storage, uint8_t group_id, const char *key, wm_nvs_item_t *item);

wm_nvs_group_t *wm_nvs_storage_find_group_by_name(wm_nvs_storage_t *storage, const char *group_name);

wm_nvs_group_t *wm_nvs_storage_find_group_by_id(wm_nvs_storage_t *storage, uint8_t group_id);

#ifdef __cplusplus
}
#endif

#endif
