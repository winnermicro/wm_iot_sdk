/**
 * @file wm_nvs_pt.h
 *
 * @brief nvs partition
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

#ifndef __WM_NVS_PARTITION_H__
#define __WM_NVS_PARTITION_H__

#include <stdint.h>

#include "wmsdk_config.h"
#include "wm_types.h"
#include "wm_utils.h"
#include "wm_nvs_porting.h"
#include "wm_nvs_item.h"

#ifdef __cplusplus
extern "C" {
#endif

#define WM_NVS_SECTION_SIZE 0x1000
#define WM_NVS_FLASH_DEVICE "iflash"

typedef struct {
    void *dev;                             /**< flash device        */
    char name[WM_NVS_PARTITION_NAME_SIZE]; /**< partition name      */
    uint32_t offset;                       /**< partition offset    */
    uint16_t sec_num;                      /**< sector num          */
    uint16_t sec_size;                     /**< sector size         */
    uint32_t size;                         /**< all partitoin size  */
    uint8_t encrypted;                     /**< partition encrypted */
    uint8_t readonly;                      /**< partition readonly  */
} wm_nvs_partition_t;

/*init partition*/
int wm_nvs_pt_init(const char *partition_name, wm_nvs_partition_t *pt);

/*erase all the partition*/
int wm_nvs_pt_erase_all(const char *partition_name);

/*erase partition sector*/
int wm_nvs_pt_erase(wm_nvs_partition_t *pt, uint32_t address);

/*read with encryption if configured*/
int wm_nvs_pt_read(wm_nvs_partition_t *pt, uint32_t address, void *data, uint32_t length);

/*write with encryption if configured*/
int wm_nvs_pt_write(wm_nvs_partition_t *pt, uint32_t address, const void *data, uint32_t length);

/*read without encryption*/
int wm_nvs_pt_read_raw(wm_nvs_partition_t *pt, uint32_t address, void *data, uint32_t length);

/*read without encryption*/
int wm_nvs_pt_write_raw(wm_nvs_partition_t *pt, uint32_t address, const void *data, uint32_t length);

/*read item, The first 16 bytes are encrypted, and the last 16 bytes are not encrypted*/
int wm_nvs_pt_read_item(wm_nvs_partition_t *pt, uint32_t address, wm_nvs_item_t *item);

/*write item, The first 16 bytes are encrypted, and the last 16 bytes are not encrypted*/
int wm_nvs_pt_write_item(wm_nvs_partition_t *pt, uint32_t address, const wm_nvs_item_t *item);

#ifdef __cplusplus
}
#endif

#endif
