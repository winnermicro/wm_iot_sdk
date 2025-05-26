/**
 * @file wm_nvs_port.c
 *
 * @brief nvs port
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
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdint.h>

#include "wm_partition_table.h"
#include "wm_osal.h"
#include "wm_dt.h"
#include "wm_drv_flash.h"
#include "wm_drv_crc.h"

#define LOG_TAG "nvs_port"
#include "wm_nvs_porting.h"
#include "wm_nvs.h"

static wm_os_mutex_t *g_nvs_mutex = NULL; /**< global mutex  */
static wm_device_t *g_crc_dev     = NULL; /**< crc device    */

int wm_nvs_port_init(void)
{
    int ret = WM_NVS_ERR_OK;

    if (!g_nvs_mutex) {
        g_crc_dev = wm_drv_crc_init("crc");

        ret = wm_os_internal_mutex_create((wm_os_mutex_t **)&g_nvs_mutex);
        WM_NVS_LOGD("nvs port init, ret=%d,g_nvs_mutex=%p", ret, g_nvs_mutex);
    }

    return ret;
}

int wm_nvs_port_deinit(void)
{
    if (g_nvs_mutex) {
        wm_os_internal_mutex_delete((wm_os_mutex_t *)g_nvs_mutex);
        g_nvs_mutex = NULL;
        WM_NVS_LOGD("nvs port deinit");
    }

    return WM_NVS_ERR_OK;
}

int wm_nvs_port_is_init(void)
{
    return ((g_nvs_mutex && g_crc_dev) ? true : false);
}

int wm_nvs_port_mutex_lock(void)
{
    if (!g_nvs_mutex) {
        return WM_NVS_ERR_NOT_INIT;
    }

    if (wm_os_internal_get_isr_count() > 0) {
        return WM_NVS_ERR_CALL_IN_ISR;
    }

    return wm_os_internal_mutex_acquire((wm_os_mutex_t *)g_nvs_mutex, WM_OS_WAIT_TIME_MAX);
}

int wm_nvs_port_mutex_unlock(void)
{
    return wm_os_internal_mutex_release((wm_os_mutex_t *)g_nvs_mutex);
}

uint32_t wm_nvs_port_crc32(uint32_t crc, const void *buf, uint32_t len)
{
    wm_drv_crc_cfg_t ctx;
    uint32_t crcvalue = crc;

    if (g_crc_dev) {
        wm_drv_crc_cfg(g_crc_dev, &ctx, crc, WM_GPSEC_CRC32, WM_GPSEC_CRC_OUT_IN_REVERSE);

        wm_drv_crc_update(g_crc_dev, &ctx, (unsigned char *)buf, len);

        wm_drv_crc_final(g_crc_dev, &ctx, &crcvalue);
    }

    return crcvalue;
}

__attribute__((weak)) void wm_nvs_port_power_off(int type, int stage)
{
    ((void)type);
    ((void)stage);
}
