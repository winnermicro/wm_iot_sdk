/**
 * @file wm_diskio.c
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

#include <string.h>
#include "wm_diskio.h"
#include "wm_drv_rtc.h"
#include "wm_error.h"
#include "wm_component.h"

#define LOG_TAG "diskio"
#include "wm_log.h"

#define wm_diskio_init   disk_initialize
#define wm_diskio_status disk_status
#define wm_diskio_read   disk_read
#define wm_diskio_write  disk_write
#define wm_diskio_ioctl  disk_ioctl

wm_device_t *diskio_dev[FF_VOLUMES]              = { NULL };
static wm_diskio_ops_t *s_diskio_ops[FF_VOLUMES] = { NULL };

#if FF_MULTI_PARTITION
PARTITION VolToPart[FF_VOLUMES] = { 0 };
#endif

#if CONFIG_ATCMD_FATFS_SDMMC_DISK
wm_diskio_driver_num_t at_disk_num = WM_DISKIO_DRIVER_NUM_SDMMC;
#elif CONFIG_ATCMD_FATFS_INTERNAL_FLASH_DISK
wm_diskio_driver_num_t at_disk_num = WM_DISKIO_DRIVER_NUM_INTERNAL_FLASH;
#elif CONFIG_ATCMD_FATFS_EXTERNAL_FLASH_DISK
wm_diskio_driver_num_t at_disk_num = WM_DISKIO_DRIVER_NUM_EXTERNAL_FLASH;
#endif

DRESULT wm_diskio_register(BYTE pdrv, const wm_diskio_ops_t *ops)
{
    if (pdrv >= FF_VOLUMES || ops == NULL) {
        return RES_PARERR;
    }

    if (s_diskio_ops[pdrv]) {
        wm_diskio_ops_t *p = s_diskio_ops[pdrv];
        s_diskio_ops[pdrv] = NULL;
        free(p);
    }

    wm_diskio_ops_t *p = (wm_diskio_ops_t *)malloc(sizeof(wm_diskio_ops_t));
    if (p == NULL) {
        return RES_ERROR;
    }

    memcpy(p, ops, sizeof(wm_diskio_ops_t));
    s_diskio_ops[pdrv] = p;

#if FF_MULTI_PARTITION
    VolToPart[pdrv].pd = pdrv;
    VolToPart[pdrv].pt = 0;
#endif

    return RES_OK;
}

DSTATUS wm_diskio_init(BYTE pdrv)
{
    if (pdrv >= FF_VOLUMES || s_diskio_ops[pdrv] == NULL || s_diskio_ops[pdrv]->init == NULL) {
        return STA_NOINIT;
    }

    return s_diskio_ops[pdrv]->init(pdrv);
}

DSTATUS wm_diskio_status(BYTE pdrv)
{
    if (pdrv >= FF_VOLUMES || s_diskio_ops[pdrv] == NULL || s_diskio_ops[pdrv]->status == NULL) {
        return STA_NOINIT;
    }

    return s_diskio_ops[pdrv]->status(pdrv);
}

DRESULT wm_diskio_read(BYTE pdrv, BYTE *buff, LBA_t sector, UINT count)
{
    if (pdrv >= FF_VOLUMES || s_diskio_ops[pdrv] == NULL || s_diskio_ops[pdrv]->read == NULL) {
        return RES_PARERR;
    }

    return s_diskio_ops[pdrv]->read(pdrv, buff, sector, count);
}

DRESULT wm_diskio_write(BYTE pdrv, const BYTE *buff, LBA_t sector, UINT count)
{
    if (pdrv >= FF_VOLUMES || s_diskio_ops[pdrv] == NULL || s_diskio_ops[pdrv]->write == NULL) {
        return RES_PARERR;
    }

    return s_diskio_ops[pdrv]->write(pdrv, buff, sector, count);
}

DRESULT wm_diskio_ioctl(BYTE pdrv, BYTE cmd, void *buff)
{
    if (pdrv >= FF_VOLUMES || s_diskio_ops[pdrv] == NULL || s_diskio_ops[pdrv]->ioctl == NULL) {
        return RES_PARERR;
    }

    return s_diskio_ops[pdrv]->ioctl(pdrv, cmd, buff);
}

DWORD get_fattime(void)
{
    struct tm tm                = { 0 };
    static wm_device_t *rtc_dev = NULL;
    int ret                     = WM_ERR_SUCCESS;

    if (rtc_dev == NULL) {
        rtc_dev = wm_drv_rtc_init("rtc");
    }

    if (rtc_dev == NULL) {
        rtc_dev = wm_dt_get_device_by_name("rtc");
    }

    ret = wm_drv_rtc_get_time(rtc_dev, &tm);
    if (ret != WM_ERR_SUCCESS) {
        return 0;
    }

    return ((DWORD)(tm.tm_year - 80) << 25) | ((DWORD)(tm.tm_mon + 1) << 21) | ((DWORD)tm.tm_mday << 16) |
           (WORD)(tm.tm_hour << 11) | (WORD)(tm.tm_min << 5) | (WORD)(tm.tm_sec >> 1);
}

int wm_diskio_componen_init(void)
{
    int ret     = 0;
    DRESULT res = FR_OK;

#if CONFIG_FATFS_SDMMC_DISK_ENABLE
    res = wm_diskio_sdmmc_register(WM_DISKIO_DRIVER_NUM_SDMMC);
    if (res != RES_OK) {
        wm_log_error("Failed to register SDMMC driver.");
        ret = WM_ERR_FAILED;
    }
#endif

#if CONFIG_FATFS_INTERNAL_FLASH_DISK_ENABLE
    res = wm_diskio_flash_register(WM_DISKIO_DRIVER_NUM_INTERNAL_FLASH);
    if (res != RES_OK) {
        wm_log_error("Failed to register internal flash driver.");
        ret = WM_ERR_FAILED;
    }
#endif

#if CONFIG_FATFS_EXTERNAL_FLASH_DISK_ENABLE
    res = wm_diskio_flash_register(WM_DISKIO_DRIVER_NUM_EXTERNAL_FLASH);
    if (res != RES_OK) {
        wm_log_error("Failed to register external flash driver.");
        ret = WM_ERR_FAILED;
    }
#endif

    return ret;
}

WM_COMPONEN_INIT_3(wm_diskio_componen_init);
