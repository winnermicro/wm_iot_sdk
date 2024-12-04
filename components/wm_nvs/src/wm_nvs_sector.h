

/**
 * @file wm_nvs_sector.h
 *
 * @brief nvs sector module
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

#include "wm_nvs_porting.h"
#include "wm_nvs.h"
#include "wm_nvs_hash.h"
#include "wm_nvs_item.h"
#include "wm_nvs_partition.h"

#ifndef __WM_NVS_SECTOR_H__
#define __WM_NVS_SECTOR_H__

#ifdef __cplusplus
extern "C" {
#endif

#ifndef CONFIG_NVS_VER_NUM
#define CONFIG_NVS_VER_NUM 1
#endif

#define WM_NVS_SECTOR_MAGIC 0x4D57

/**
  * @brief  nvs sector state
  */
typedef enum {
    WM_NVS_SECTOR_STATE_UNINIT    = 0xff, /* 1111 1111 uninit   */
    WM_NVS_SECTOR_STATE_USING     = 0xfe, /* 1111 1110 using    */
    WM_NVS_SECTOR_STATE_FULL      = 0xfc, /* 1111 1100 full     */
    WM_NVS_SECTOR_STATE_DELETTING = 0xf8, /* 1111 1000 deleting */
    WM_NVS_SECTOR_STATE_CRASH     = 0xf0, /* 1111 0000 crashed  */
    WM_NVS_SECTOR_STATE_INVALID   = 0x00, /* 0000 0000 invalid  */
} wm_nvs_sector_state_t;

/**
  * @brief  nvs sector header structure
  */
typedef struct {
    uint16_t magic;         /**< sector header magic  */
    uint8_t state;          /**< sector state         */
    uint8_t reserve_1;      /**< sector reserve1      */
    uint32_t crc32;         /**< sector crc32         */
    uint32_t serial_number; /**< sector serial number */
    uint8_t version;        /**< sector version       */
    uint8_t reserve_2[19];  /**< sector reserve2      */
} wm_nvs_sector_header_t;

/**
  * @brief  nvs sector manager information
  */
typedef struct {
    struct dl_list list; /* for list manager */
    uint8_t state;       /* sector state     */
    uint8_t version;     /* sector version   */

    uint8_t next_free_slice; /* next free slice id       */
    uint8_t used_slice;      /* used num : using + droped*/
    uint8_t droped_slice;    /* erased num               */

    uint32_t address;       /* offset address from partition start position */
    uint32_t serial_number; /* sector serial number */
    wm_nvs_hash_t hash;     /* hash list            */
    wm_nvs_partition_t *pt; /* partition info       */
} wm_nvs_sector_t;

int wm_nvs_sector_init(wm_nvs_sector_t *sec);

int wm_nvs_sector_set_state(wm_nvs_sector_t *sec, wm_nvs_sector_state_t state);

int wm_nvs_sector_erase(wm_nvs_sector_t *sec);

/*index start from 0. not include header */
int wm_nvs_sector_erase_item(wm_nvs_sector_t *sec, int index, wm_nvs_item_t *item, bool erase_hash);

int wm_nvs_sector_load(wm_nvs_partition_t *pt, wm_nvs_sector_t *sec, int index);

int wm_nvs_sector_write_item_data(wm_nvs_sector_t *sec, wm_nvs_item_t *pitem, const void *extra_data, size_t len,
                                  int entry_cnt);

int wm_nvs_sector_write_item(wm_nvs_sector_t *sec, uint8_t group_id, wm_nvs_type_t type, const char *key, const void *data,
                             size_t size, uint8_t seg_id);

int wm_nvs_sector_read_item_data(wm_nvs_sector_t *sec, int found_slice_index, wm_nvs_item_t *item, void *data, size_t size);

int wm_nvs_sector_find_item(wm_nvs_sector_t *sec, uint8_t group_id, wm_nvs_type_t type, const char *key, int *item_index,
                            wm_nvs_item_t *item, uint8_t seg_index, wm_nvs_seg_start_t seg_start);

int wm_nvs_sector_copy(wm_nvs_sector_t *dst, wm_nvs_sector_t *src);

#ifdef __cplusplus
}
#endif

#endif
