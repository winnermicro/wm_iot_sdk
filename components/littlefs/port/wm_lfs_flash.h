/**
 * @file wm_lfs_flash.h
 *
 * @brief LittleFS Flash Block Device Module
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

#ifndef __WM_LFS_FLASH_H__
#define __WM_LFS_FLASH_H__

#include "lfs.h"
#include "lfs_util.h"

#ifdef __cplusplus
extern "C" {
#endif

// Block device specific tracing
#ifndef LFS_FLASH_TRACE
#ifdef LFS_FLASH_YES_TRACE
#define LFS_FLASH_TRACE(...) wm_log_debug(__VA_ARGS__)
#else
#define LFS_FLASH_TRACE(...)
#endif
#endif

/**
 * @defgroup WM_LFS_FLASH_Enumerations WM LFS FLASH Enumerations
 * @brief WinnerMicro LittleFS Flash Enumerations
 */

/**
 * @addtogroup WM_LFS_FLASH_Enumerations
 * @{
 */

/**
 * @brief Enumeration for the types of flash to use with LittleFS.
 *
 * This enumeration defines the available flash types for LittleFS.
 */
typedef enum {
    WM_LFS_FLASH_TYPE_INTERNAL = 0x00, /**< Internal flash storage */
    WM_LFS_FLASH_TYPE_EXTERNAL = 0x01, /**< External flash storage */
    WM_LFS_FLASH_TYPE_MAX
} wm_lfs_flash_type_t;

/**
 * @}
 */

/**
 * @defgroup WM_LFS_FLASH_Functions WM LFS FLASH Functions
 * @brief WinnerMicro LittleFS Flash Functions
 */

/**
 * @addtogroup WM_LFS_FLASH_Functions
 * @{
 */

/**
 * @brief Initialize flash block device and return littlefs config.
 *
 * This function initializes a flash block device for use with LittleFS and returns
 * a configuration structure that can be used to mount a LittleFS filesystem.
 *
 * @param[in] flash_type Type of flash to initialize (internal or external)
 *
 * @return Pointer to lfs_config structure if successful, NULL on failure
 */
struct lfs_config *lfs_flash_init(wm_lfs_flash_type_t flash_type);

/**
 * @brief Clean up flash block device resources.
 *
 * This function frees resources associated with a flash block device.
 *
 * @param[in] cfg Pointer to the lfs_config structure to clean up
 *
 * @return 0 on success, negative error code on failure
 */
int lfs_flash_deinit(const struct lfs_config *cfg);

/**
 * @}
 */

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* __WM_LFS_FLASH_H__ */
