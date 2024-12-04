/**
 * @file wm_nvs_iterator.h
 *
 * @brief nvs iterator
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

#ifndef __WM_NVS_ITERATOR_H__
#define __WM_NVS_ITERATOR_H__

#include <stdint.h>
#include "wmsdk_config.h"
#include "wm_types.h"
#include "wm_list.h"

#include "wm_nvs.h"
#include "wm_nvs_porting.h"
#include "wm_nvs_storage.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
  * @brief  nvs open handle information structure
  */
typedef struct {
    wm_nvs_storage_t *storage; /**< storage info          */
    wm_nvs_sector_t *sector;   /**< current sector        */
    int slice_index;           /**< current slice index   */
    uint8_t group_id;          /**< group id for look up  */
    wm_nvs_type_t type;        /**< item type for look up */
    wm_nvs_entry_t info;       /**< item information      */
} wm_nvs_iterator_info_t;

int wm_nvs_iterator_find(const char *partition_name, const char *group, wm_nvs_type_t type, wm_nvs_iterator_t *output_iterator);

int wm_nvs_iterator_find_by_handle(wm_nvs_handle_t handle, wm_nvs_type_t type, wm_nvs_iterator_t *output_iterator);

int wm_nvs_iterator_next(wm_nvs_iterator_t *iterator);

int wm_nvs_iterator_info(wm_nvs_iterator_t iterator, wm_nvs_entry_t *info);

int wm_nvs_iterator_data(wm_nvs_iterator_t iterator, void *data, size_t *data_len);

int wm_nvs_iterator_release(wm_nvs_iterator_t iterator);

int wm_nvs_iterator_print(const char *partition_name, const char *group_name);

#ifdef __cplusplus
}
#endif

#endif
