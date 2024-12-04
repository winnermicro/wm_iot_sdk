/**
 * @file wm_nvs_handler.c
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

#include "wm_nvs_handler.h"

int wm_nvs_handler_open(wm_nvs_storage_t *storage, uint8_t group_id, wm_nvs_open_mode_t mode, wm_nvs_handle_info_t **handle)
{
    wm_nvs_handle_info_t *ctx = WM_NVS_CALLOC(1, sizeof(*ctx));
    if (ctx) {
        dl_list_init(&ctx->list);
        ctx->storage  = storage;
        ctx->group_id = group_id;
        ctx->mode     = mode;
        ctx->valid    = 1;
        *handle       = ctx;
        return WM_NVS_ERR_OK;
    } else {
        return WM_NVS_ERR_NO_MEM;
    }
}

int wm_nvs_handler_close(wm_nvs_handle_info_t *handle)
{
    WM_NVS_FREE(handle);
    return WM_NVS_ERR_OK;
}
