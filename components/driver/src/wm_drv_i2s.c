/**
 * @file wm_drv_i2s.c
 *
 * @brief DRV_I2S Module
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
#include "wm_drv_i2s.h"
#include "wm_drv_rcc.h"
#include "wm_drv_gpio.h"
#include "wm_drv_irq.h"
#include "wm_dt_hw.h"

#include "wm_drv_ops_i2s.c"

#define WM_DRV_I2S_LOCK(device)                                                                    \
    do {                                                                                           \
        if (i2s_mutex_lock(((wm_drv_i2s_data_t *)((device)->drv))->ctx.mutex) != WM_ERR_SUCCESS) { \
            return WM_ERR_FAILED;                                                                  \
        }                                                                                          \
    } while (0)

#define WM_DRV_I2S_UNLOCK(device) wm_os_internal_mutex_release(((wm_drv_i2s_data_t *)(device)->drv)->ctx.mutex)

static int i2s_mutex_lock(wm_os_mutex_t *mutex)
{
    if (wm_os_internal_get_isr_count() > 0) {
        wm_log_debug("can't call in interrupt context");
        return WM_ERR_NO_SUPPORT;
    }

    return wm_os_internal_mutex_acquire(mutex, WM_OS_WAIT_TIME_MAX);
}

int wm_drv_i2s_ioctl(wm_device_t *device, wm_drv_i2s_ioctl_cmd_t cmd, bool enable, uint32_t arg)
{
    int err;

    if (!(device && device->drv)) {
        return WM_ERR_INVALID_PARAM;
    }

    WM_DRV_I2S_LOCK(device);

    err = ((wm_drv_i2s_ops_t *)(device->ops))->ioctl(device, cmd, enable, arg);

    WM_DRV_I2S_UNLOCK(device);

    return err;
}

int wm_drv_i2s_set_format(wm_device_t *device, uint32_t sample_rate_hz, enum wm_i2s_bits bits, enum wm_i2s_chan_type channel)
{
    int err;

    if (!(device && device->drv)) {
        return WM_ERR_INVALID_PARAM;
    }

    WM_DRV_I2S_LOCK(device);

    err = ((wm_drv_i2s_ops_t *)(device->ops))->set_format(device, sample_rate_hz, bits, channel);

    WM_DRV_I2S_UNLOCK(device);

    return err;
}

int wm_drv_i2s_set_mclk(wm_device_t *device, uint32_t mclk_hz)
{
    int err;

    if (!(device && device->drv)) {
        return WM_ERR_INVALID_PARAM;
    }

    WM_DRV_I2S_LOCK(device);

    err = ((wm_drv_i2s_ops_t *)(device->ops))->set_mclk(device, mclk_hz);

    WM_DRV_I2S_UNLOCK(device);

    return err;
}

int wm_drv_i2s_write_async(wm_device_t *device, void *buf, int len)
{
    int err;

    if (!(device && device->drv)) {
        return WM_ERR_INVALID_PARAM;
    }

    WM_DRV_I2S_LOCK(device);

    err = ((wm_drv_i2s_ops_t *)(device->ops))->write_async(device, buf, len);

    WM_DRV_I2S_UNLOCK(device);

    return err;
}

int wm_drv_i2s_write_stop(wm_device_t *device)
{
    int err;

    if (!(device && device->drv)) {
        return WM_ERR_INVALID_PARAM;
    }

    WM_DRV_I2S_LOCK(device);

    err = ((wm_drv_i2s_ops_t *)(device->ops))->write_stop(device);

    WM_DRV_I2S_UNLOCK(device);

    return err;
}

int wm_drv_i2s_write_pause(wm_device_t *device)
{
    int err;

    if (!(device && device->drv)) {
        return WM_ERR_INVALID_PARAM;
    }

    WM_DRV_I2S_LOCK(device);

    err = ((wm_drv_i2s_ops_t *)(device->ops))->write_pause(device);

    WM_DRV_I2S_UNLOCK(device);

    return err;
}

int wm_drv_i2s_write_resume(wm_device_t *device)
{
    int err;

    if (!(device && device->drv)) {
        return WM_ERR_INVALID_PARAM;
    }

    WM_DRV_I2S_LOCK(device);

    err = ((wm_drv_i2s_ops_t *)(device->ops))->write_resume(device);

    WM_DRV_I2S_UNLOCK(device);

    return err;
}

int wm_drv_i2s_read_async(wm_device_t *device, void *buf, int len)
{
    int err;

    if (!(device && device->drv)) {
        return WM_ERR_INVALID_PARAM;
    }

    WM_DRV_I2S_LOCK(device);

    err = ((wm_drv_i2s_ops_t *)(device->ops))->read_async(device, buf, len);

    WM_DRV_I2S_UNLOCK(device);

    return err;
}

int wm_drv_i2s_read_stop(wm_device_t *device)
{
    int err;

    if (!(device && device->drv)) {
        return WM_ERR_INVALID_PARAM;
    }

    WM_DRV_I2S_LOCK(device);

    err = ((wm_drv_i2s_ops_t *)(device->ops))->read_stop(device);

    WM_DRV_I2S_UNLOCK(device);

    return err;
}

int wm_drv_i2s_read_pause(wm_device_t *device)
{
    int err;

    if (!(device && device->drv)) {
        return WM_ERR_INVALID_PARAM;
    }

    WM_DRV_I2S_LOCK(device);

    err = ((wm_drv_i2s_ops_t *)(device->ops))->read_pause(device);

    WM_DRV_I2S_UNLOCK(device);

    return err;
}

int wm_drv_i2s_read_resume(wm_device_t *device)
{
    int err;

    if (!(device && device->drv)) {
        return WM_ERR_INVALID_PARAM;
    }

    WM_DRV_I2S_LOCK(device);

    err = ((wm_drv_i2s_ops_t *)(device->ops))->read_resume(device);

    WM_DRV_I2S_UNLOCK(device);

    return err;
}

int wm_drv_i2s_register_write_cb(wm_device_t *device, wm_drv_i2s_callback_t callback)
{
    if (!(device && device->drv)) {
        return WM_ERR_INVALID_PARAM;
    }

    WM_DRV_I2S_LOCK(device);

    ((wm_drv_i2s_data_t *)(device->drv))->ctx.tx_callback = callback;

    WM_DRV_I2S_UNLOCK(device);

    return WM_ERR_SUCCESS;
}

int wm_drv_i2s_register_read_cb(wm_device_t *device, wm_drv_i2s_callback_t callback)
{
    if (!(device && device->drv)) {
        return WM_ERR_INVALID_PARAM;
    }

    WM_DRV_I2S_LOCK(device);

    ((wm_drv_i2s_data_t *)(device->drv))->ctx.rx_callback = callback;

    WM_DRV_I2S_UNLOCK(device);

    return WM_ERR_SUCCESS;
}

wm_device_t *wm_drv_i2s_init(const char *name, wm_drv_i2s_cfg_t *cfg)
{
    int err                 = WM_ERR_SUCCESS;
    wm_device_t *device     = NULL;
    wm_drv_i2s_ctx_t *ctx   = NULL;
    wm_drv_i2s_data_t *data = NULL;

    if (name == NULL || cfg == NULL) {
        wm_log_debug("init:bad param");
        return NULL;
    }

    device = wm_dt_get_device_by_name(name);
    if (device == NULL) {
        wm_log_debug("init: no such device");
        return NULL;
    }

    if (device->drv || device->state != WM_DEV_ST_UNINIT) {
        wm_log_debug("already initialized.");
        return NULL;
    }

    device->state = WM_DEV_ST_INITING;

    if (device->ops == NULL || device->name == NULL || device->hw == NULL) {
        device->state = WM_DEV_ST_UNINIT;
        return NULL;
    }

    data = WM_DRV_I2S_CALLOC(1, sizeof(*data));
    if (data == NULL) {
        device->state = WM_DEV_ST_UNINIT;
        return NULL;
    }

    device->drv = data;
    ctx         = &data->ctx;
    ctx->cfg    = *cfg;

    err = wm_os_internal_mutex_create(&ctx->mutex);
    if (err != WM_ERR_SUCCESS) {
        goto fail_mutex;
    }

    err = ((wm_drv_i2s_ops_t *)(device->ops))->init(device, &ctx->cfg);
    if (err != WM_ERR_SUCCESS) {
        goto fail_init;
    }

    device->state = WM_DEV_ST_INITED;

    return device;

fail_init:
    wm_os_internal_mutex_delete(ctx->mutex);

fail_mutex:

    WM_DRV_I2S_FREE(data);
    device->drv   = NULL;
    device->state = WM_DEV_ST_UNINIT;

    return NULL;
}

int wm_drv_i2s_deinit(wm_device_t *device)
{
    int err                 = WM_ERR_FAILED;
    wm_drv_i2s_data_t *data = device->drv;

    if (!(device && device->drv)) {
        wm_log_debug("bad param.");
        return WM_ERR_INVALID_PARAM;
    }

    if (device->state != WM_DEV_ST_INITED) {
        wm_log_debug("not init.");
        return WM_ERR_FAILED;
    }

    WM_DRV_I2S_LOCK(device);

    err = ((wm_drv_i2s_ops_t *)(device->ops))->deinit(device);

    /* unlock mutex */
    WM_DRV_I2S_UNLOCK(device);

    /* delete mutex */
    wm_os_internal_mutex_delete(data->ctx.mutex);

    /* free data */
    WM_DRV_I2S_FREE(device->drv);

    device->drv   = NULL;
    device->state = WM_DEV_ST_UNINIT;

    wm_log_debug("I2S driver de-initialized!");

    return err;
}

int wm_drv_i2s_dump_info(wm_device_t *device)
{
    wm_hal_i2s_dev_t *dev    = NULL;
    wm_drv_i2s_cfg_t *cfg    = NULL;
    wm_drv_i2s_format_t *fmt = NULL;
    wm_drv_i2s_data_t *data  = NULL;
    wm_drv_i2s_ctx_t *ctx    = NULL;

    if (!(device && device->drv)) {
        return WM_ERR_INVALID_PARAM;
    }

    if (device->state != WM_DEV_ST_INITED) {
        wm_log_debug("not init.");
        return WM_ERR_FAILED;
    }

    WM_DRV_I2S_LOCK(device);

    data = device->drv;
    dev  = &data->dev;
    ctx  = &data->ctx;
    cfg  = &ctx->cfg;
    fmt  = &ctx->fmt;

    wm_log_info("cfg.mode         :%d", cfg->mode);
    wm_log_info("cfg.dir          :%d", cfg->dir);
    wm_log_info("cfg.std          :%d", cfg->std);
    wm_log_info("fmt.bits          :%d", fmt->bits);
    wm_log_info("fmt.ch_type      :%d", fmt->ctype);
    wm_log_info("cfg.xtype        :%d", cfg->xtype);
    wm_log_info("fmt.sample_rate  :%u", fmt->sample_rate_hz);
    wm_log_info("cfg.rx_pkt_size  :%u", ctx->cfg.rx_pkt_size);
    wm_log_info("cfg.rx_pkt_num  :%u", ctx->cfg.rx_pkt_num);
    wm_log_info("cfg.tx_pkt_num  :%u", ctx->cfg.tx_pkt_num);

    wm_log_info("ctx.mclk_hz  :%u", ctx->mclk_hz);
    wm_log_info("ctx.bitrates  :%u", ctx->bitrates);
    wm_log_info("ctx.bclk_div  :%u", ctx->bclk_div);
    wm_log_info("ctx.mclk_div  :%u", ctx->mclk_div);

    wm_hal_i2s_dump_info(dev);

    WM_DRV_I2S_UNLOCK(device);

    return WM_ERR_SUCCESS;
}
