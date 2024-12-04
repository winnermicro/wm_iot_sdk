/**
 * @file wm_drv_flash_ops.h
 *
 * @brief flash Driver Module
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

#ifndef __WM_DRV_FLASH_OPS_H__
#define __WM_DRV_FLASH_OPS_H__

#include "wm_drv_flash.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    int (*init)(wm_device_t *dev);
    int (*deinit)(wm_device_t *dev);
    int (*write_with_erase)(wm_device_t *dev, uint32_t offset, uint8_t *in_buf, uint32_t in_buf_len);
    int (*write)(wm_device_t *dev, uint32_t offset, uint8_t *in_buf, uint32_t in_buf_len);
    int (*read)(wm_device_t *dev, uint32_t offset, uint8_t *out_buf, uint32_t out_len);
    int (*get_info)(wm_device_t *dev, wm_drv_flash_info_t *info);
    int (*erase_region)(wm_device_t *dev, uint32_t offset, uint32_t erase_len);
    int (*erase_sector)(wm_device_t *dev, uint32_t sector_idx, uint32_t sector_count);
    int (*erase_chip)(wm_device_t *dev);
} wm_drv_flash_ops_t;

#ifdef __cplusplus
}
#endif

#endif /* __WM_DRV_FLASH_H__ */
