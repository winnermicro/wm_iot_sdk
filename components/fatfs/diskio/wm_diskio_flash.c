/**
 * @file wm_diskio_flash.c
 *
 * @brief DISKIO Flash Module
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

#include "wm_diskio_flash.h"
#include "wm_drv_flash.h"
#include "wm_partition_table.h"
#include "wm_error.h"

#define LOG_TAG "diskio_flash"
#include "wm_log.h"

#define FATFS_FLASH_SECTOR_SIZE (512)

#if CONFIG_FATFS_INTERNAL_FLASH_DISK_ENABLE
static DRESULT wm_diskio_internal_flash_check_partition(const char *partition_name, uint32_t sector, uint32_t count,
                                                        uint32_t *addr, uint32_t *size)
{
    wm_partition_item_t partition;
    int ret = wm_partition_table_find(partition_name, &partition);
    if (ret != WM_ERR_SUCCESS) {
        wm_log_error("Partition not found: %s", partition_name);
        return RES_ERROR;
    }

    *addr = partition.offset + (sector * FATFS_FLASH_SECTOR_SIZE);
    *size = count * FATFS_FLASH_SECTOR_SIZE;

    if (*addr + *size > partition.offset + partition.size) {
        wm_log_error("Operation exceeds partition size");
        return RES_ERROR;
    }

    return RES_OK;
}
#endif

#if CONFIG_FATFS_EXTERNAL_FLASH_DISK_ENABLE
static DRESULT wm_diskio_external_flash_check(wm_device_t *dev, uint32_t sector, uint32_t count, uint32_t *addr, uint32_t *size)
{
    wm_drv_flash_info_t flash_info;
    uint32_t available_size = 0;
    int ret                 = wm_drv_flash_get_device_info(dev, &flash_info);
    if (ret != WM_ERR_SUCCESS) {
        wm_log_error("Failed to get flash info");
        return RES_ERROR;
    }

    *addr = CONFIG_FATFS_EXTERNAL_FLASH_START_ADDRESS + sector * FATFS_FLASH_SECTOR_SIZE;
    *size = count * FATFS_FLASH_SECTOR_SIZE;

    available_size = (flash_info.flash_size - CONFIG_FATFS_EXTERNAL_FLASH_START_ADDRESS) < CONFIG_FATFS_EXTERNAL_FLASH_SIZE ?
                         (flash_info.flash_size - CONFIG_FATFS_EXTERNAL_FLASH_START_ADDRESS) :
                         CONFIG_FATFS_EXTERNAL_FLASH_SIZE;

    if (*addr + *size > CONFIG_FATFS_EXTERNAL_FLASH_START_ADDRESS + available_size) {
        wm_log_error("Operation exceeds flash size");
        return RES_ERROR;
    }

    return RES_OK;
}
#endif
static DSTATUS wm_diskio_flash_init(BYTE pdrv)
{
    DSTATUS status = 0;

    if (diskio_dev[pdrv] == NULL) {
#if CONFIG_FATFS_INTERNAL_FLASH_DISK_ENABLE
        if (pdrv == WM_DISKIO_DRIVER_NUM_INTERNAL_FLASH) {
            diskio_dev[pdrv] = wm_dt_get_device_by_name("iflash");
            if (diskio_dev[pdrv] == NULL) {
                wm_log_error("Internal flash init failed");
                status = STA_NODISK;
            }
        }
#endif
#if CONFIG_FATFS_EXTERNAL_FLASH_DISK_ENABLE
        if (pdrv == WM_DISKIO_DRIVER_NUM_EXTERNAL_FLASH) {
            diskio_dev[pdrv] = wm_dt_get_device_by_name(CONFIG_FATFS_EXTERNAL_FLASH_DEVICE_NAME);
            if (diskio_dev[pdrv] != NULL && diskio_dev[pdrv]->state != WM_DEV_ST_INITED) {
                diskio_dev[pdrv] = wm_drv_flash_init(CONFIG_FATFS_EXTERNAL_FLASH_DEVICE_NAME);
            }
            if (diskio_dev[pdrv] == NULL || diskio_dev[pdrv]->state != WM_DEV_ST_INITED) {
                wm_log_error("External flash init failed");
                status = STA_NODISK;
            }
        }
#endif
    }

    return status;
}

static DSTATUS wm_diskio_flash_status(BYTE pdrv)
{
    DSTATUS status   = 0;
    wm_device_t *dev = diskio_dev[pdrv];

    if (dev == NULL || dev->state != WM_DEV_ST_INITED) {
        wm_log_error("Flash device not ready");
        return STA_NOINIT;
    }

    return status;
}

static DRESULT wm_diskio_flash_read(BYTE pdrv, BYTE *buff, DWORD sector, UINT count)
{
    DRESULT result   = RES_OK;
    int ret          = WM_ERR_SUCCESS;
    wm_device_t *dev = diskio_dev[pdrv];
    uint32_t read_addr, read_size;

    if (dev == NULL || dev->state != WM_DEV_ST_INITED) {
        wm_log_error("Flash device not ready for read");
        return RES_NOTRDY;
    }

#if CONFIG_FATFS_INTERNAL_FLASH_DISK_ENABLE
    if (pdrv == WM_DISKIO_DRIVER_NUM_INTERNAL_FLASH) {
        result = wm_diskio_internal_flash_check_partition(CONFIG_FATFS_INTERNAL_FLASH_PARTITION_NAME, sector, count, &read_addr,
                                                          &read_size);
        if (result != RES_OK) {
            return result;
        }
    }
#endif
#if CONFIG_FATFS_EXTERNAL_FLASH_DISK_ENABLE
    if (pdrv == WM_DISKIO_DRIVER_NUM_EXTERNAL_FLASH) {
        result = wm_diskio_external_flash_check(dev, sector, count, &read_addr, &read_size);
        if (result != RES_OK) {
            return result;
        }
    }
#endif
    ret = wm_drv_flash_read(dev, read_addr, buff, read_size);
    if (ret != WM_ERR_SUCCESS) {
        wm_log_error("Flash read failed: %d", ret);
        return RES_ERROR;
    }

    return result;
}

static DRESULT wm_diskio_flash_write(BYTE pdrv, const BYTE *buff, DWORD sector, UINT count)
{
    DRESULT result   = RES_OK;
    int ret          = WM_ERR_SUCCESS;
    wm_device_t *dev = diskio_dev[pdrv];
    uint32_t write_addr, write_size;

    if (dev == NULL || dev->state != WM_DEV_ST_INITED) {
        wm_log_error("Flash device not ready for write");
        return RES_NOTRDY;
    }

#if CONFIG_FATFS_INTERNAL_FLASH_DISK_ENABLE
    if (pdrv == WM_DISKIO_DRIVER_NUM_INTERNAL_FLASH) {
        result = wm_diskio_internal_flash_check_partition(CONFIG_FATFS_INTERNAL_FLASH_PARTITION_NAME, sector, count,
                                                          &write_addr, &write_size);
        if (result != RES_OK) {
            return result;
        }
    }
#endif
#if CONFIG_FATFS_EXTERNAL_FLASH_DISK_ENABLE
    if (pdrv == WM_DISKIO_DRIVER_NUM_EXTERNAL_FLASH) {
        result = wm_diskio_external_flash_check(dev, sector, count, &write_addr, &write_size);
        if (result != RES_OK) {
            return result;
        }
    }
#endif

    ret = wm_drv_flash_write_with_erase(dev, write_addr, (uint8_t *)buff, write_size);
    if (ret != WM_ERR_SUCCESS) {
        wm_log_error("Flash write failed: %d", ret);
        return RES_ERROR;
    }

    return result;
}

static DRESULT wm_diskio_flash_ioctl(BYTE pdrv, BYTE cmd, void *buff)
{
    int ret          = WM_ERR_SUCCESS;
    wm_device_t *dev = diskio_dev[pdrv];

    if (dev == NULL || dev->state != WM_DEV_ST_INITED) {
        wm_log_error("Flash device not ready for IOCTL");
        return RES_NOTRDY;
    }

    switch (cmd) {
        case CTRL_SYNC:
            return RES_OK;
        case GET_SECTOR_COUNT:
#if CONFIG_FATFS_INTERNAL_FLASH_DISK_ENABLE
            if (pdrv == WM_DISKIO_DRIVER_NUM_INTERNAL_FLASH) {
                wm_partition_item_t partition;
                ret = wm_partition_table_find(CONFIG_FATFS_INTERNAL_FLASH_PARTITION_NAME, &partition);
                if (ret != WM_ERR_SUCCESS) {
                    wm_log_error("Partition not found: %s", CONFIG_FATFS_INTERNAL_FLASH_PARTITION_NAME);
                    return RES_ERROR;
                }
                *((DWORD *)buff) = partition.size / FATFS_FLASH_SECTOR_SIZE;
            }
#endif
#if CONFIG_FATFS_EXTERNAL_FLASH_DISK_ENABLE
            if (pdrv == WM_DISKIO_DRIVER_NUM_EXTERNAL_FLASH) {
                uint32_t available_size = 0;
                wm_drv_flash_info_t flash_info;
                ret = wm_drv_flash_get_device_info(dev, &flash_info);
                if (ret != WM_ERR_SUCCESS) {
                    wm_log_error("Failed to get flash info");
                    return RES_ERROR;
                }
                available_size =
                    (flash_info.flash_size - CONFIG_FATFS_EXTERNAL_FLASH_START_ADDRESS) < CONFIG_FATFS_EXTERNAL_FLASH_SIZE ?
                        (flash_info.flash_size - CONFIG_FATFS_EXTERNAL_FLASH_START_ADDRESS) :
                        CONFIG_FATFS_EXTERNAL_FLASH_SIZE;
                *((DWORD *)buff) = available_size / FATFS_FLASH_SECTOR_SIZE;
            }
#endif
            return RES_OK;
        case GET_SECTOR_SIZE:
            *((WORD *)buff) = FATFS_FLASH_SECTOR_SIZE;
            return RES_OK;
        case GET_BLOCK_SIZE:
            return RES_ERROR;
#if FF_USE_TRIM
        case CTRL_TRIM:
            wm_log_error("TRIM command is not supported.");
            return RES_ERROR;
#endif
        default:
            wm_log_error("Unsupported IOCTL command");
            return RES_ERROR;
    }

    return RES_ERROR;
}

DRESULT wm_diskio_flash_register(BYTE pdrv)
{
    DRESULT result      = RES_OK;
    wm_diskio_ops_t ops = { .init   = &wm_diskio_flash_init,
                            .status = &wm_diskio_flash_status,
                            .read   = &wm_diskio_flash_read,
                            .write  = &wm_diskio_flash_write,
                            .ioctl  = &wm_diskio_flash_ioctl };

    result = wm_diskio_register(pdrv, &ops);

    return result;
}
