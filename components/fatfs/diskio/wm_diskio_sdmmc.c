/**
 * @file wm_diskio_sdmmc.c
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

#include "wm_diskio_sdmmc.h"
#include "wm_drv_sdh_sdmmc.h"

#define LOG_TAG "diskio_sdmmc"
#include "wm_log.h"

static DSTATUS wm_diskio_sdmmc_init(BYTE pdrv)
{
    DSTATUS status = 0;

    if (diskio_dev[pdrv] == NULL) {
        /* Initialize sdmmc */
        wm_log_info("Initializing sdmmc device...");
        diskio_dev[pdrv] = wm_drv_sdh_sdmmc_init("sdmmc");
        if (diskio_dev[pdrv] == NULL) {
            wm_log_error("Failed to initialize sdmmc device.");
            status = STA_NODISK;
        }
    }

    return status;
}

static DSTATUS wm_diskio_sdmmc_status(BYTE pdrv)
{
    DSTATUS status   = 0;
    wm_device_t *dev = diskio_dev[pdrv];

    if (dev == NULL || dev->state != WM_DEV_ST_INITED) {
        wm_log_error("SDMMC device is not initialized or not present.");
        return STA_NOINIT;
    }

    return status;
}

static DRESULT wm_diskio_sdmmc_read(BYTE pdrv, BYTE *buff, DWORD sector, UINT count)
{
    DRESULT result                         = RES_OK;
    int ret                                = WM_ERR_SUCCESS;
    wm_device_t *dev                       = diskio_dev[pdrv];
    BYTE *aligned_buff                     = NULL;
    wm_drv_sdh_sdmmc_card_info_t card_info = { 0 };

    if (dev == NULL || dev->state != WM_DEV_ST_INITED) {
        wm_log_error("SDMMC device is not ready for read operation.");
        return RES_NOTRDY;
    }

    if ((uintptr_t)buff % 4 != 0) {
        ret = wm_drv_sdh_sdmmc_get_card_info(dev, &card_info);
        if (ret != WM_ERR_SUCCESS) {
            wm_log_error("Failed to get SDMMC card information.");
            return RES_ERROR;
        }
        aligned_buff = (BYTE *)malloc(count * card_info.block_size);
        if (aligned_buff == NULL) {
            wm_log_error("Failed to allocate aligned buffer for read operation.");
            return RES_ERROR;
        }
    } else {
        aligned_buff = buff;
    }

    ret = wm_drv_sdh_sdmmc_read_blocks(dev, aligned_buff, sector, count);
    if (ret != WM_ERR_SUCCESS) {
        wm_log_error("sdmmc read blocks failed: %d", ret);
        if (aligned_buff != buff) {
            free(aligned_buff);
        }
        return RES_ERROR;
    }

    if (aligned_buff != buff) {
        memcpy((void *)buff, aligned_buff, count * card_info.block_size);
        free(aligned_buff);
    }

    return result;
}

static DRESULT wm_diskio_sdmmc_write(BYTE pdrv, const BYTE *buff, DWORD sector, UINT count)
{
    DRESULT result                         = RES_OK;
    int ret                                = WM_ERR_SUCCESS;
    wm_device_t *dev                       = diskio_dev[pdrv];
    BYTE *aligned_buff                     = NULL;
    wm_drv_sdh_sdmmc_card_info_t card_info = { 0 };

    if (dev == NULL || dev->state != WM_DEV_ST_INITED) {
        wm_log_error("SDMMC device is not ready for write operation.");
        return RES_NOTRDY;
    }

    if ((uintptr_t)buff % 4 != 0) {
        ret = wm_drv_sdh_sdmmc_get_card_info(dev, &card_info);
        if (ret != WM_ERR_SUCCESS) {
            wm_log_error("Failed to get SDMMC card information.");
            return RES_ERROR;
        }
        aligned_buff = (BYTE *)malloc(count * card_info.block_size);
        if (aligned_buff == NULL) {
            wm_log_error("Failed to allocate aligned buffer for write operation.");
            return RES_ERROR;
        }
        memcpy(aligned_buff, buff, count * card_info.block_size);
    } else {
        aligned_buff = (BYTE *)buff;
    }

    ret = wm_drv_sdh_sdmmc_write_blocks(dev, aligned_buff, sector, count);
    if (ret != WM_ERR_SUCCESS) {
        wm_log_error("sdmmc write blocks failed: %d", ret);
        if (aligned_buff != buff) {
            free(aligned_buff);
        }
        return RES_ERROR;
    }

    if (aligned_buff != buff) {
        free(aligned_buff);
    }

    return result;
}

static DRESULT wm_diskio_sdmmc_ioctl(BYTE pdrv, BYTE cmd, void *buff)
{
    int ret                                = WM_ERR_SUCCESS;
    wm_device_t *dev                       = diskio_dev[pdrv];
    wm_drv_sdh_sdmmc_card_info_t card_info = { 0 };

    if (dev == NULL || dev->state != WM_DEV_ST_INITED) {
        wm_log_error("SDMMC device is not ready for IOCTL operation.");
        return RES_NOTRDY;
    }

    ret = wm_drv_sdh_sdmmc_get_card_info(dev, &card_info);
    if (ret != WM_ERR_SUCCESS) {
        wm_log_error("Failed to get SDMMC card information.");
        return RES_ERROR;
    }

    switch (cmd) {
        case CTRL_SYNC:
            return RES_OK;
        case GET_SECTOR_COUNT:
            *((DWORD *)buff) = card_info.capacity / card_info.block_size;
            return RES_OK;
        case GET_SECTOR_SIZE:
            *((WORD *)buff) = card_info.block_size;
            return RES_OK;
        case GET_BLOCK_SIZE:
            return RES_ERROR;
#if FF_USE_TRIM
        case CTRL_TRIM:
            wm_log_error("TRIM command is not supported.");
            return RES_ERROR;
#endif
        default:
            wm_log_error("Unsupported IOCTL command.");
            return RES_ERROR;
    }

    return RES_ERROR;
}

DRESULT wm_diskio_sdmmc_register(BYTE pdrv)
{
    DRESULT result      = RES_OK;
    wm_diskio_ops_t ops = { .init   = &wm_diskio_sdmmc_init,
                            .status = &wm_diskio_sdmmc_status,
                            .read   = &wm_diskio_sdmmc_read,
                            .write  = &wm_diskio_sdmmc_write,
                            .ioctl  = &wm_diskio_sdmmc_ioctl };

    result = wm_diskio_register(pdrv, &ops);

    return result;
}
