/**
 * @file wm_nvs_item.h
 *
 * @brief nvs item
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

#ifndef __WM_NVS_ITEM_H__
#define __WM_NVS_ITEM_H__

#include <stdint.h>
#include "wmsdk_config.h"
#include "wm_types.h"

#include "wm_nvs.h"
#include "wm_nvs_porting.h"

#ifdef __cplusplus
extern "C" {
#endif

#define WM_NVS_SLICE_SIZE                32
#define WM_NVS_SLICE_NUM                 (WM_NVS_SECTION_SIZE / 32)
#define WM_NVS_ENTRY_COUNT               (WM_NVS_SLICE_NUM - 1)

#define WM_NVS_SECTOR_STATE_OFF_SET      2

#define WM_NVS_SINGLE_ITEM_MAX_DATA_SIZE ((WM_NVS_ENTRY_COUNT - 1) * WM_NVS_SLICE_SIZE)
#define WM_NVS_ENTRY_DATA_OFFSET         WM_NVS_SLICE_SIZE

#define WM_NVS_GROUP_ID_MAX              0x3f                /* group id max     */
#define WM_NVS_GROUP_ID_ANY              WM_NVS_GROUP_ID_MAX /* invlaid group id */
#define WM_NVS_GROUP_ITSELF_ID           0x0                 /* id for group itself  */

#define WM_NVS_SEG_ID_ANY                0xff /* Not use as valid seg id    */
#define WM_NVS_SEG_NUM_MAX               127

/**
  * @brief  nvs type internal
  */
#define WM_NVS_TYPE_BLOB_SEG             WM_NVS_TYPE_MAX       /**< BLOB data segment */
#define WM_NVS_TYPE_ANY_WITHOUT_SEG      (WM_NVS_TYPE_MAX + 1) /**< Type any and not exclude blog segment */

/**
  * @brief  nvs item state
  */
typedef enum {
    WM_NVS_ITEM_STATE_UNUSED = 0xff, /* 1111 1111 unused   */
    WM_NVS_ITEM_STATE_USING  = 0xfe, /* 1111 1110 using    */
    WM_NVS_ITEM_STATE_DROPED = 0xfc, /* 1111 1100 droped   */
} wm_nvs_item_state_t;

typedef enum {
    WM_NVS_SEG_START_VER_0 = 0x0,  /* start from 0     */
    WM_NVS_SEG_START_VER_1 = 0x80, /* start from 0x80  */
    WM_NVS_SEG_START_ANY   = 0xff,
} wm_nvs_seg_start_t;

typedef struct {
    uint8_t state;
    char name[WM_NVS_MAX_KEY_LEN]; /**< item name     */

    struct {
        uint8_t group_id : 6; /**< group id        */
        uint8_t recv1    : 2; /**< reseve          */
        uint8_t seg_id;       /**< seg id          */
        uint16_t type   : 4;  /**< type            */
        uint16_t length : 12; /**< length          */
    };

    uint32_t crc_item; /**< crc32 for head  */

    union {
        struct {               /*blob desc                     */
            uint32_t all_size; /**< for blob segs all size     */
            uint8_t seg_count; /**< for blob desc      */
            uint8_t seg_start; /**< for seg start      */
            uint16_t resv2;
        };

        struct {               /**< string(len > 8) or blob data   */
            uint32_t crc_data; /**< for data                       */
            uint32_t resv3;
        };
        uint8_t data[8]; /**< primitive data or (string <= 8) */
    };
} wm_nvs_item_t;

#define WM_NVS_ITEM_CRC_LEN (WM_NVS_SLICE_SIZE - 1)

uint32_t wm_nvs_item_crc_hash(const wm_nvs_item_t *item);
uint32_t wm_nvs_item_crc_head(const wm_nvs_item_t *item);

int wm_nvs_item_get_span(wm_nvs_item_t *item);
void wm_nvs_item_print_item_head(wm_nvs_item_t *item);

/*
Init item, Not init crc and data field
*/
int wm_nvs_item_init(wm_nvs_item_t *item, uint8_t state, uint8_t gid, wm_nvs_type_t type, const char *key, const void *data,
                     int size, uint8_t seg_id);

#ifdef __cplusplus
}
#endif

#endif
