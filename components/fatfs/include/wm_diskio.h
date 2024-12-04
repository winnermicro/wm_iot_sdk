/**
 * @file wm_diskio.h
 *
 * @brief DISKIO Module
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

#ifndef __WM_DISKIO_H__
#define __WM_DISKIO_H__

#include "ff.h"
#include "ffconf.h"
#include "diskio.h"
#include "wm_dt.h"
#include "wm_diskio_sdmmc.h"
#include "wm_diskio_flash.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief External array of device pointers for managing disk I/O operations.
 *
 * This array holds pointers to device structures that are used to manage disk I/O operations.
 * Each entry in the array corresponds to a different logical drive, allowing for multiple
 * physical or virtual drives to be managed simultaneously.
 */
extern wm_device_t *diskio_dev[FF_VOLUMES];

/**
 * @defgroup WM_DISKIO_Enumerations WM DISKIO Enumerations
 * @brief WinnerMicro DISKIO Enumerations
 */

/**
 * @addtogroup WM_DISKIO_Enumerations
 * @{
 */

/**
 * @brief Enumeration for the names of disk I/O drivers.
 *
 * This enumeration defines the names of the available disk I/O drivers.
 * It is used to identify the driver type when registering or initializing disk I/O operations.
 */
typedef enum {
    WM_DISKIO_DRIVER_NUM_SDMMC          = 0x00, /**< Identifier for the SDMMC disk I/O driver */
    WM_DISKIO_DRIVER_NUM_INTERNAL_FLASH = 0x01, /**< Identifier for the internal flash disk I/O driver */
    WM_DISKIO_DRIVER_NUM_EXTERNAL_FLASH = 0x02, /**< Identifier for the external flash disk I/O driver */
    WM_DISKIO_DRIVER_NUM_MAX
} wm_diskio_driver_num_t;

/**
 * @}
 */

/**
 * @defgroup WM_DISKIO_Structures WM DISKIO Structures
 * @brief WinnerMicro DISKIO Structures
 */

/**
 * @addtogroup WM_DISKIO_Structures
 * @{
 */

/**
 * @brief Structure defining the disk I/O operations.
 *
 * This structure defines a set of functions that must be implemented to handle disk I/O operations.
 * These functions include initialization, status checking, reading, writing, and control commands.
 */
typedef struct {
    DSTATUS (*init)(BYTE pdrv);                                                  /**< Initializes the disk. */
    DSTATUS (*status)(BYTE pdrv);                                                /**< Checks the disk status.*/
    DRESULT (*read)(BYTE pdrv, BYTE *buff, LBA_t sector, UINT count);            /**< Reads data from the disk.*/
    DRESULT (*write)(BYTE pdrv, const BYTE *data_buf, LBA_t sector, UINT count); /**< Writes data to the disk.*/
    DRESULT (*ioctl)(BYTE pdrv, BYTE cmd, void *buff);                           /**< Executes disk control commands.*/
} wm_diskio_ops_t;

/**
 * @}
 */

/**
 * @brief The disk number for ATCMD.
 *
 * This variable holds the disk number for ATCMD.
 */
extern wm_diskio_driver_num_t at_disk_num;

/**
 * @defgroup WM_DISKIO_Functions WM DISKIO Functions
 * @brief WinnerMicro DISKIO Functions
 */

/**
 * @addtogroup WM_DISKIO_Functions
 * @{
 */

/**
 * @brief Registers disk I/O operation functions.
 *
 * This function is used to register disk I/O operation functions associated with a specific drive.
 *
 * @param[in] pdrv Drive number (0-FF_VOLUMES)
 * @param[in] ops Pointer to the disk I/O operation functions
 *
 * @return
 *    - RES_OK Success
 *    - RES_PARERR Parameter error
 *    - others: failed
 */
DRESULT wm_diskio_register(BYTE pdrv, const wm_diskio_ops_t *ops);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* __WM_DISKIO_H__ */
