

/**
 * @file wm_nvs_sector_manager.h
 *
 * @brief nvs sector manager module
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

#include <string.h>
#include <stdlib.h>

#include "wm_nvs.h"
#include "wm_nvs_hash.h"
#include "wm_nvs_item.h"
#include "wm_nvs_porting.h"
#include "wm_nvs_partition.h"
#include "wm_nvs_sector.h"

#ifndef __WM_NVS_SECTOR_MANAGER_H__
#define __WM_NVS_SECTOR_MANAGER_H__

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    struct dl_list active;    /**< using sector list      */
    struct dl_list idle;      /**< idle sector list       */
    wm_nvs_partition_t *pt;   /**< partition infomation   */
    wm_nvs_sector_t *sec_arr; /**< sector infomation list */
    uint32_t serial_number;   /**< next serial number     */

} wm_nvs_sector_manager_t;

int wm_nvs_sm_load(wm_nvs_sector_manager_t *sm, wm_nvs_partition_t *pt);
int wm_nvs_sm_unload(wm_nvs_sector_manager_t *sm);

int wm_nvs_sm_get_status(wm_nvs_sector_manager_t *sm, wm_nvs_status_t *status);
int wm_nvs_sm_request_sector(wm_nvs_sector_manager_t *sm, int need_size);

int wm_nvs_sm_find_item(wm_nvs_sector_manager_t *sm, uint8_t group_id, wm_nvs_type_t type, const char *key, int *item_index,
                        wm_nvs_sector_t **sector, wm_nvs_item_t *item, uint8_t seg_index, wm_nvs_seg_start_t seg_start);

int wm_nvs_sm_check_write_blob_size(wm_nvs_sector_manager_t *sm, size_t size);

inline static wm_nvs_sector_t *wm_nvs_sm_get_current_sector(wm_nvs_sector_manager_t *sm)
{
    return dl_list_last(&sm->active, wm_nvs_sector_t, list);
}

inline static int wm_nvs_sm_get_gc_size(wm_nvs_sector_t *sec)
{
    return (WM_NVS_ENTRY_COUNT - sec->used_slice + sec->droped_slice) * WM_NVS_SLICE_SIZE;
}

inline static int wm_nvs_sm_get_free_size(wm_nvs_sector_t *sec)
{
    return (WM_NVS_ENTRY_COUNT - sec->used_slice) * WM_NVS_SLICE_SIZE;
}

int wm_nvs_sm_get_status(wm_nvs_sector_manager_t *sm, wm_nvs_status_t *status);

#ifdef __cplusplus
}
#endif

#endif
