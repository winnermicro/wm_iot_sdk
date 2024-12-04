/**
 * @file wm_nvs_hash.c
 *
 * @brief nvs hash
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

#define LOG_TAG "nvs_hash"
#include "wm_nvs_porting.h"
#include "wm_nvs.h"
#include "wm_nvs_item.h"
#include "wm_nvs_sector.h"

#define WM_NVS_HASH_ENLARGE_STEP 10

int wm_nvs_hash_init(wm_nvs_hash_t *h)
{
    memset(h, 0, sizeof(*h));
    return WM_NVS_ERR_OK;
}

int wm_nvs_hash_append(wm_nvs_hash_t *h, const wm_nvs_item_t *item, size_t index)
{
    int new_num;

    /*full*/
    if (h->count >= WM_NVS_ENTRY_COUNT) {
        return WM_NVS_ERR_NO_SPACE;
    }

    /*need enlarge*/
    if (h->size <= h->count) {
        new_num = h->size + WM_NVS_HASH_ENLARGE_STEP;
        WM_NVS_LOGV("count=%d,(old_size-->new_size) = (%u -->%u)", h->count, h->size, new_num);
        h->hash_table = WM_NVS_REALLOC(h->hash_table, new_num * sizeof(wm_nvs_hash_node_t));
        if (!h->hash_table) {
            return WM_NVS_ERR_NO_MEM;
        }
        h->size = new_num;
    }

    /*add to last*/
    h->hash_table[h->count].hash = wm_nvs_item_crc_hash(item);
    h->hash_table[h->count].id   = (uint8_t)index;

    WM_NVS_LOGD("insert %.*s --> %d", WM_NVS_MAX_KEY_LEN, item->name, h->count);

    h->count++;

    return WM_NVS_ERR_OK;
}

bool wm_nvs_hash_erase(wm_nvs_hash_t *h, const size_t index)
{
    int i;

    for (i = 0; i < h->count; i++) {
        if (h->hash_table[i].index == index) {
            /*Not the last one*/
            if (i != h->count - 1) {
                /* Move the following items to the front , keep the index order*/
                memcpy(&h->hash_table[i], &h->hash_table[i + 1], (h->count - i - 1) * sizeof(wm_nvs_hash_node_t));
            }

            h->count--;

            WM_NVS_LOGD("erase %d", i);

            return true;
        }
    }

    return false;
}

int wm_nvs_hash_find(wm_nvs_hash_t *h, size_t start, const wm_nvs_item_t *item)
{
    int i;
    uint32_t crc;

    crc = wm_nvs_item_crc_hash(item);

    for (i = 0; i < h->count; i++) {
        if (h->hash_table[i].index >= start && h->hash_table[i].index != WM_NVS_HASH_INVALID &&
            h->hash_table[i].hash == (crc & 0xffffff)) {
            WM_NVS_LOGD("found %.*s at %d", WM_NVS_MAX_KEY_LEN, item->name, i);
            return h->hash_table[i].index;
        }
    }

    return WM_NVS_HASH_INVALID;
}

void wm_nvs_hash_clear(wm_nvs_hash_t *h)
{
    WM_NVS_LOGD("clear");

    if (h->hash_table) {
        WM_NVS_FREE(h->hash_table);
        memset(h, 0, sizeof(*h));
    }
    return;
}
