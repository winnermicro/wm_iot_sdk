/**
 * @file wm_nvs_handler.h
 *
 * @brief nvs handler
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

#ifndef __WM_NVS_HANDLER_H__
#define __WM_NVS_HANDLER_H__

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
typedef struct wm_nvs_handle_info_t {
    struct dl_list list;       /**< handle list         */
    wm_nvs_storage_t *storage; /**< storage information */
    uint8_t group_id;          /**< group id            */
    uint8_t valid;             /**< handle is valid     */
    wm_nvs_open_mode_t mode;   /**< open mode           */
} wm_nvs_handle_info_t;

int wm_nvs_handler_open(wm_nvs_storage_t *storage, uint8_t group_id, wm_nvs_open_mode_t mode, wm_nvs_handle_info_t **handle);
int wm_nvs_handler_close(wm_nvs_handle_info_t *handle);

#ifdef __cplusplus
}
#endif

#endif
