
/**
 * @file wm_nvs_item->c
 *
 * @brief nvs nvs item module
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

#define LOG_TAG "nvs_sec"
#include "wm_nvs_porting.h"
#include "wm_nvs.h"
#include "wm_nvs_item.h"

/*for valid item*/
int wm_nvs_item_get_span(wm_nvs_item_t *item)
{
    int span = 1;

    if (item->length > 8) {
        /*extra data length*/
        span += (item->length + (WM_NVS_SLICE_SIZE - 1)) / WM_NVS_SLICE_SIZE;
    }

    return span;
}

uint32_t wm_nvs_item_crc_hash(const wm_nvs_item_t *item)
{
    return wm_nvs_port_crc32(UINT32_MAX, &item->name, sizeof(item->name) + 2);
}

uint32_t wm_nvs_item_crc_head(const wm_nvs_item_t *item)
{
    uint32_t crc;

    crc = wm_nvs_port_crc32(UINT32_MAX, &item->name, sizeof(item->name) + 4);
    crc = wm_nvs_port_crc32(crc, &item->data, 8);

    return crc;
}

/*can't init blob descriptor*/
int wm_nvs_item_init(wm_nvs_item_t *item, uint8_t state, uint8_t group_id, wm_nvs_type_t type, const char *key,
                     const void *data, int size, uint8_t seg_id)
{
    int key_len = strnlen(key, WM_NVS_MAX_KEY_LEN);

    memset(item, 0xff, sizeof(*item));

    item->state    = state;
    item->group_id = group_id;
    item->type     = type;
    item->length   = size;
    item->seg_id   = seg_id;
    item->recv1    = 0;

    memcpy(item->name, key, key_len);

    if (key_len < WM_NVS_MAX_KEY_LEN) {
        item->name[key_len] = 0;
    }

    /*
    init data area if the data is Not NULL
    */
    if (data) {
        if (size > 8) {
            /*multi entry item*/
            item->crc_data = wm_nvs_port_crc32(UINT32_MAX, data, size);
            item->crc_item = wm_nvs_item_crc_head(item);

        } else {
            /*one entry item*/
            memcpy(item->data, data, size);
            item->crc_item = wm_nvs_item_crc_head(item);
        }
    }

    return WM_NVS_ERR_OK;
}

void wm_nvs_item_print_item_head(wm_nvs_item_t *item)
{
    WM_NVS_LOGD("state=0x%x", item->state);
    WM_NVS_LOGD("group_id=0x%x", item->group_id);
    WM_NVS_LOGD("seg_id=0x%x", item->seg_id);
    WM_NVS_LOGD("type=0x%x", item->type);
    WM_NVS_LOGD("length=0x%x", item->length);

    if (item->type == WM_NVS_TYPE_BLOB) {
        WM_NVS_LOGD("all_size=0x%x", item->all_size);
        WM_NVS_LOGD("seg_count=0x%x", item->seg_count);
        WM_NVS_LOGD("seg_start=0x%x", item->seg_start);
    }

    return;
}
