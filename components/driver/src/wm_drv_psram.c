/**
 * @file wm_drv_psram.c
 *
 * @brief driver psram master Module
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

#include "wm_drv_psram.h"
#undef LOG_TAG
#define LOG_TAG "drv_psram"
#include "wm_log.h"

#include "wm_drv_ops_psram.c"

wm_device_t *wm_drv_psram_init(const char *device_name)
{
    wm_device_t *dev = (device_name ? wm_dt_get_device_by_name(device_name) : NULL);
    if (!dev || !dev->ops) {
        return NULL;
    }
    return ((wm_drv_psram_ops_t *)dev->ops)->init(dev) == WM_ERR_SUCCESS ? dev : NULL;
}

int wm_drv_psram_deinit(wm_device_t *device)
{
    if (!device || !device->ops) {
        return WM_ERR_INVALID_PARAM;
    }
    return ((wm_drv_psram_ops_t *)device->ops)->deinit(device);
}

int wm_drv_psram_register_callback(wm_device_t *device, wm_drv_psram_callback_t callback, void *user_data)
{
    if (!device || !device->ops) {
        return WM_ERR_INVALID_PARAM;
    }
    return ((wm_drv_psram_ops_t *)device->ops)->register_callback(device, callback, user_data);
}

int wm_drv_psram_get_size(wm_device_t *device, uint32_t *size)
{
    if (!device || !device->ops) {
        return WM_ERR_INVALID_PARAM;
    }
    return ((wm_drv_psram_ops_t *)device->ops)->get_size(device, size);
}

int wm_drv_psram_ioctrl(wm_device_t *device, int cmd, uint32_t param, uint32_t *arg)
{
    if (!device || !device->ops) {
        return WM_ERR_INVALID_PARAM;
    }
    return ((wm_drv_psram_ops_t *)device->ops)->ioctrl(device, cmd, param, arg);
}

int wm_drv_psram_memcpy_dma(wm_device_t *device, void *dst, const void *src, uint32_t size)
{
    if (!device || !device->ops) {
        return WM_ERR_INVALID_PARAM;
    }
    return ((wm_drv_psram_ops_t *)device->ops)->memcpy_dma(device, dst, src, size);
}
