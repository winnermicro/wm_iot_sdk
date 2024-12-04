/**
 * @file wm_nvs_partition_manager.h
 *
 * @brief nvs partition manager
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

#ifndef __WM_NVS_PARTITION_MANAGER_H__
#define __WM_NVS_PARTITION_MANAGER_H__

#include <stdint.h>
#include "wmsdk_config.h"
#include "wm_types.h"

#include "wm_nvs.h"
#include "wm_nvs_porting.h"
#include "wm_nvs_sector.h"
#include "wm_nvs_item.h"
#include "wm_nvs_hash.h"
#include "wm_nvs_storage.h"
#include "wm_nvs_handler.h"
#include "wm_nvs_iterator.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
  * @note  ptm is the abbreviation of partition manager
  */

/**
  * @brief  nvs partition init, add partition to list
  */
int wm_nvs_ptm_init(const char *partition_name);

/**
  * @brief  nvs partition deinit, remove partition from list
  */
int wm_nvs_ptm_deinit(const char *partition_name);

/**
  * @brief  is nvs partition manager init
  */
int wm_nvs_ptm_is_in_using(void);

/**
  * @brief  open handle
  */
int wm_nvs_ptm_open_handle(const char *partition_name, const char *group, wm_nvs_open_mode_t mode,
                           wm_nvs_handle_info_t **handle);

/**
  * @brief  close handle
  */
int wm_nvs_ptm_close_handle(wm_nvs_handle_info_t *handle);

/**
  * @brief  is handle valid
  */
bool wm_nvs_ptm_is_handle_valid(wm_nvs_handle_info_t *handle);

/**
  * @brief  find parition storage by parition name
  */
wm_nvs_storage_t *wm_nvs_ptm_find_storage(const char *partition_name);

struct dl_list *wm_nvs_get_storage_list(void);
struct dl_list *wm_nvs_get_handle_list(void);

int wm_nvs_ptm_get_status(const char *partition_name, wm_nvs_status_t *status);

#ifdef __cplusplus
}
#endif

#endif
