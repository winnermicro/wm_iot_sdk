/**
 * @file wm_nvs_hash.h
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

#ifndef __WM_NVS_HASH_H__
#define __WM_NVS_HASH_H__

#include <stdint.h>
#include "wmsdk_config.h"
#include "wm_types.h"

#include "wm_nvs.h"
#include "wm_nvs_porting.h"
#include "wm_nvs_item.h"

#ifdef __cplusplus
extern "C" {
#endif

#define WM_NVS_HASH_INVALID -1

/**
  * @brief  nvs hash table node
  */
typedef union {
    struct {
        uint32_t id   : 8;  /**< slice id in sector */
        uint32_t hash : 24; /**< hash code          */
    };
    struct {
        int8_t index; /**< look for index     */
        uint8_t resv[3];
    };
} wm_nvs_hash_node_t;

/**
  * @brief  nvs hash table information
  */
typedef struct {
    wm_nvs_hash_node_t *hash_table; /**< hash table array */
    uint8_t count;                  /**< item entry num   */
    uint8_t size;                   /**< hash table size  */
} wm_nvs_hash_t;

int wm_nvs_hash_init(wm_nvs_hash_t *h);
int wm_nvs_hash_append(wm_nvs_hash_t *h, const wm_nvs_item_t *item, size_t index);
bool wm_nvs_hash_erase(wm_nvs_hash_t *h, const size_t index);
int wm_nvs_hash_find(wm_nvs_hash_t *h, size_t start, const wm_nvs_item_t *item);
void wm_nvs_hash_clear(wm_nvs_hash_t *h);

#ifdef __cplusplus
}
#endif

#endif
