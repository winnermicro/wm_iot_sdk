/**
 * @file wm_diskio_sdmmc.h
 *
 * @brief DISKIO_SDMMC Module
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

#ifndef __WM_DISKIO_SDMMC_H__
#define __WM_DISKIO_SDMMC_H__

#include "wm_diskio.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup WM_DISKIO_SDMMC_Type_Definitions WM DISKIO_SDMMC Type Definitions
 * @brief WinnerMicro DISKIO_SDMMC Type Definitions
 */

/**
 * @addtogroup WM_DISKIO_SDMMC_Type_Definitions
 * @{
 */

/**
 * @brief Registers the SDMMC disk I/O operations.
 *
 * This function registers the SDMMC-specific disk I/O operations with the DISKIO module.
 * It sets up the necessary functions for initializing the SDMMC device, checking its status,
 * reading from and writing to the device, and performing I/O control operations.
 *
 * @param[in] pdrv Drive number (0-FF_VOLUMES)
 *
 * @return
 *    - RES_OK Success
 *    - RES_PARERR Parameter error
 *    - others: failed
 */
DRESULT wm_diskio_sdmmc_register(BYTE pdrv);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* __WM_DISKIO_SDMMC_H__ */
