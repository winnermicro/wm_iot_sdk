/**
 * @file wm_drv_i2s_codec.c
 *
 * @brief DRV_I2S_CODEC Module
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

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "wm_dt.h"
#include "wm_irq.h"
#include "wm_list.h"
#include "wm_hal_i2s.h"
#include "wm_osal.h"
#include "wm_soc.h"
#include "wm_soc_cfgs.h"
#include "wm_types.h"
#include "wm_error.h"
#include "wm_drv_dma.h"
#include "wm_drv_codec_i2s.h"
#include "wm_drv_rcc.h"
#include "wm_drv_gpio.h"
#include "wm_drv_irq.h"
#include "wm_dt_hw.h"

#define LOG_TAG "codec_i2s"
#include "wm_log.h"

#define WM_DRV_CODEC_LOG wm_log_error

/**
  * @brief  mutex lock for api
  */
#define WM_DRV_CODEC_I2S_LOCK(mutex)                                                                                   \
    do {                                                                                                               \
        if (wm_os_internal_recursive_mutex_acquire((wm_os_mutex_t *)(mutex), WM_OS_WAIT_TIME_MAX) != WM_ERR_SUCCESS) { \
            return WM_ERR_FAILED;                                                                                      \
        }                                                                                                              \
    } while (0)

/**
  * @brief  mutex unlock for api
  */
#define WM_DRV_CODEC_I2S_UNLOCK(mutex) wm_os_internal_recursive_mutex_release((wm_os_mutex_t *)(mutex))

wm_device_t *wm_drv_codec_i2s_init(const char *name, wm_drv_codec_i2s_cfg_t *cfg)
{
    wm_device_t *dev            = NULL;
    wm_drv_codec_i2s_ctx_t *ctx = NULL;

    int err;

    if (!(name && cfg && (dev = wm_dt_get_device_by_name(name)))) {
        /*bad param*/
        WM_DRV_CODEC_LOG("bad param");
        return NULL;
    }

    if (dev->state != WM_DEV_ST_UNINIT) {
        /*bad status*/
        WM_DRV_CODEC_LOG("bad state=%d", dev->state);
        return NULL;
    }

    dev->drv = calloc(1, sizeof(wm_drv_codec_i2s_ctx_t));
    if (!dev->drv) {
        return NULL;
    }

    dev->state = WM_DEV_ST_INITING;

    ctx = (wm_drv_codec_i2s_ctx_t *)dev->drv;

    if (wm_os_internal_recursive_mutex_create(&ctx->mutex) == WM_OS_STATUS_SUCCESS) {
        err = ((wm_drv_codec_i2s_ops_t *)dev->ops)->init(dev, cfg);

        if (err == WM_ERR_SUCCESS) {
            /*init success*/
            dev->state = WM_DEV_ST_INITED;
            return dev;
        } else {
            WM_DRV_CODEC_LOG("init fail err=%d", err);
        }
    }

    /*init fail*/
    if (ctx->mutex) {
        wm_os_internal_recursive_mutex_delete(ctx->mutex);
    }

    if (dev->drv) {
        free(dev->drv);
        dev->drv = NULL;
    }

    dev->state = WM_DEV_ST_UNINIT;

    return NULL;
}

int wm_drv_codec_i2s_deinit(wm_device_t *device)
{
    int err                     = WM_ERR_FAILED;
    wm_drv_codec_i2s_ctx_t *ctx = NULL;

    if (!(device && device->state == WM_DEV_ST_INITED && device->drv)) {
        WM_DRV_CODEC_LOG("deinit bad state state=%d", device->state);
        return WM_ERR_INVALID_PARAM;
    }

    ctx = (wm_drv_codec_i2s_ctx_t *)device->drv;

    WM_DRV_CODEC_I2S_LOCK(ctx->mutex);

    if (device->state == WM_DEV_ST_INITED) {
        device->state = WM_DEV_ST_UNKNOWN;

        err = ((wm_drv_codec_i2s_ops_t *)device->ops)->deinit(device);

        if (ctx->mutex) {
            wm_os_internal_recursive_mutex_delete(ctx->mutex);
            ctx->mutex = NULL;
        }

        if (device->drv) {
            free(device->drv);
            device->drv = NULL;
        }
    }

    device->state = WM_DEV_ST_UNINIT;

    return err;
}

int wm_drv_codec_i2s_set_format(wm_device_t *device, uint32_t sample_rate_hz, enum wm_i2s_bits bits,
                                enum wm_i2s_chan_type channel)
{
    int err;
    wm_drv_codec_i2s_ctx_t *ctx = NULL;

    if (!(device && device->state == WM_DEV_ST_INITED && device->drv)) {
        return WM_ERR_INVALID_PARAM;
    }

    ctx = (wm_drv_codec_i2s_ctx_t *)device->drv;

    WM_DRV_CODEC_I2S_LOCK(ctx->mutex);

    err = ((wm_drv_codec_i2s_ops_t *)device->ops)->set_format(device, sample_rate_hz, bits, channel);

    WM_DRV_CODEC_I2S_UNLOCK(ctx->mutex);

    return err;
}

int wm_drv_codec_i2s_start(wm_device_t *device)
{
    int err;
    wm_drv_codec_i2s_ctx_t *ctx = NULL;

    if (!(device && device->state == WM_DEV_ST_INITED && device->drv)) {
        return WM_ERR_INVALID_PARAM;
    }

    ctx = (wm_drv_codec_i2s_ctx_t *)device->drv;

    WM_DRV_CODEC_I2S_LOCK(ctx->mutex);

    err = ((wm_drv_codec_i2s_ops_t *)device->ops)->start(device);

    WM_DRV_CODEC_I2S_UNLOCK(ctx->mutex);

    return err;
}

int wm_drv_codec_i2s_stop(wm_device_t *device)
{
    int err;
    wm_drv_codec_i2s_ctx_t *ctx = NULL;

    ctx = (wm_drv_codec_i2s_ctx_t *)device->drv;

    if (!(device && device->state == WM_DEV_ST_INITED && device->drv)) {
        return WM_ERR_INVALID_PARAM;
    }

    WM_DRV_CODEC_I2S_LOCK(ctx->mutex);

    err = ((wm_drv_codec_i2s_ops_t *)device->ops)->stop(device);

    WM_DRV_CODEC_I2S_UNLOCK(ctx->mutex);

    return err;
}

int wm_drv_codec_i2s_set_mute(wm_device_t *device, bool mute)
{
    int err;
    wm_drv_codec_i2s_ctx_t *ctx = NULL;

    ctx = (wm_drv_codec_i2s_ctx_t *)device->drv;

    if (!(device && device->state == WM_DEV_ST_INITED && device->drv)) {
        return WM_ERR_INVALID_PARAM;
    }

    WM_DRV_CODEC_I2S_LOCK(ctx->mutex);

    err = ((wm_drv_codec_i2s_ops_t *)device->ops)->set_mute(device, mute);

    WM_DRV_CODEC_I2S_UNLOCK(ctx->mutex);

    return err;
}

int wm_drv_codec_i2s_set_volume(wm_device_t *device, int vol)
{
    int err;
    wm_drv_codec_i2s_ctx_t *ctx = NULL;

    ctx = (wm_drv_codec_i2s_ctx_t *)device->drv;

    if (!(device && device->state == WM_DEV_ST_INITED && device->drv && vol >= 0 && vol <= 100)) {
        return WM_ERR_INVALID_PARAM;
    }

    WM_DRV_CODEC_I2S_LOCK(ctx->mutex);

    err = ((wm_drv_codec_i2s_ops_t *)device->ops)->set_volume(device, vol);

    WM_DRV_CODEC_I2S_UNLOCK(ctx->mutex);

    return err;
}

int wm_drv_codec_i2s_set_mic_mute(wm_device_t *device, bool mute)
{
    int err;
    wm_drv_codec_i2s_ctx_t *ctx = NULL;

    ctx = (wm_drv_codec_i2s_ctx_t *)device->drv;

    if (!(device && device->state == WM_DEV_ST_INITED && device->drv)) {
        return WM_ERR_INVALID_PARAM;
    }

    WM_DRV_CODEC_I2S_LOCK(ctx->mutex);

    err = ((wm_drv_codec_i2s_ops_t *)device->ops)->set_mic_mute(device, mute);

    WM_DRV_CODEC_I2S_UNLOCK(ctx->mutex);

    return err;
}

int wm_drv_codec_i2s_set_mic_volume(wm_device_t *device, int vol)
{
    int err;
    wm_drv_codec_i2s_ctx_t *ctx = NULL;

    if (!(device && device->state == WM_DEV_ST_INITED && device->drv && vol >= 0 && vol <= 100)) {
        return WM_ERR_INVALID_PARAM;
    }

    ctx = (wm_drv_codec_i2s_ctx_t *)device->drv;

    WM_DRV_CODEC_I2S_LOCK(ctx->mutex);

    err = ((wm_drv_codec_i2s_ops_t *)device->ops)->set_mic_volume(device, vol);

    WM_DRV_CODEC_I2S_UNLOCK(ctx->mutex);

    return err;
}

int wm_drv_codec_i2s_set_reg(wm_device_t *device, int reg, int value)
{
    int err;
    wm_drv_codec_i2s_ctx_t *ctx = NULL;

    if (!(device && device->state == WM_DEV_ST_INITED && device->drv)) {
        return WM_ERR_INVALID_PARAM;
    }

    ctx = (wm_drv_codec_i2s_ctx_t *)device->drv;

    WM_DRV_CODEC_I2S_LOCK(ctx->mutex);

    err = ((wm_drv_codec_i2s_ops_t *)device->ops)->set_reg(device, reg, value);

    WM_DRV_CODEC_I2S_UNLOCK(ctx->mutex);

    return err;
}

int wm_drv_codec_i2s_get_reg(wm_device_t *device, int reg, int *value)
{
    int err;
    wm_drv_codec_i2s_ctx_t *ctx = NULL;

    if (!(device && device->state == WM_DEV_ST_INITED && device->drv && value)) {
        return WM_ERR_INVALID_PARAM;
    }

    ctx = (wm_drv_codec_i2s_ctx_t *)device->drv;

    WM_DRV_CODEC_I2S_LOCK(ctx->mutex);

    err = ((wm_drv_codec_i2s_ops_t *)device->ops)->get_reg(device, reg, value);

    WM_DRV_CODEC_I2S_UNLOCK(ctx->mutex);

    return err;
}

int wm_drv_codec_i2s_dump(wm_device_t *device)
{
    int err;
    wm_drv_codec_i2s_ctx_t *ctx = NULL;

    if (!(device && device->state == WM_DEV_ST_INITED && device->drv)) {
        return WM_ERR_INVALID_PARAM;
    }

    ctx = (wm_drv_codec_i2s_ctx_t *)device->drv;

    WM_DRV_CODEC_I2S_LOCK(ctx->mutex);

    err = ((wm_drv_codec_i2s_ops_t *)device->ops)->dump(device);

    WM_DRV_CODEC_I2S_UNLOCK(ctx->mutex);

    return err;
}
