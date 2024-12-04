/**
 * @file wm_drv_pwm.c
 *
 * @brief PWM DRV Module
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
#include "wm_hal_pwm.h"
#include "wm_osal.h"
#include "wm_soc.h"
#include "wm_soc_cfgs.h"
#include "wm_types.h"
#include "wm_error.h"
#include "wm_drv_dma.h"
#include "wm_drv_pwm.h"
#include "wm_drv_rcc.h"
#include "wm_drv_gpio.h"
#include "wm_drv_irq.h"
#include "wm_dt_hw.h"

#include "wm_drv_ops_pwm.c"

wm_device_t *wm_drv_pwm_init(const char *name)
{
    int ret               = WM_ERR_SUCCESS;
    wm_drv_pwm_ctx_t *ctx = NULL;
    wm_dt_hw_pwm_t *hw    = NULL;

    if (name == NULL) {
        WM_DRV_PWM_LOG_MARKER;
        return NULL;
    }

    wm_device_t *device = wm_dt_get_device_by_name(name);
    if (device == NULL) {
        WM_DRV_PWM_LOG_MARKER;
        return NULL;
    }

    if (device->drv || device->state != WM_DEV_ST_UNINIT) {
        WM_DRV_PWM_LOG_E("The driver %s has already been initialized.", name);
        return NULL;
    }

    device->state = WM_DEV_ST_INITING;

    if (device->ops == NULL || device->name == NULL || device->hw == NULL) {
        WM_DRV_PWM_LOG_MARKER;
        goto fail;
    }

    wm_drv_pwm_data_t *data = wm_os_internal_calloc(1, sizeof(*data));
    if (device == NULL) {
        WM_DRV_PWM_LOG_MARKER;
        goto fail;
    }

    device->drv = data;
    ctx         = &data->ctx;
    hw          = device->hw;

    ctx->dma_dev = wm_dt_get_device_by_name(hw->dma_device_name);

    ret = wm_os_internal_mutex_create(&ctx->lock);
    if (ret != WM_ERR_SUCCESS) {
        WM_DRV_PWM_LOG_MARKER;
        goto free_data;
    }

    ret = WM_DRV_PWM_OPS(device)->init(device);
    if (ret != WM_ERR_SUCCESS) {
        WM_DRV_PWM_LOG_MARKER;
        goto free_mutex;
    }

    device->state = WM_DEV_ST_INITED;

    return device;

free_mutex:
    wm_os_internal_mutex_delete(ctx->lock);
free_data:
    wm_os_internal_free(data);

fail:
    device->state = WM_DEV_ST_UNINIT;

    return NULL;
}

int wm_drv_pwm_deinit(wm_device_t *device)
{
    int ret = WM_ERR_SUCCESS;

    wm_drv_pwm_data_t *data = NULL;

    if (!WM_PWM_PRE_VALID()) {
        WM_DRV_PWM_LOG_MARKER;
        return WM_ERR_INVALID_PARAM;
    }

    if (!WM_DRV_PWM_TRY_LOCK()) {
        WM_DRV_PWM_LOG_MARKER;
        return WM_ERR_FAILED;
    }

    data = (wm_drv_pwm_data_t *)device->drv;

    ret = WM_DRV_PWM_OPS(device)->deinit(data);
    if (ret != WM_ERR_SUCCESS) {
        WM_DRV_PWM_LOG_MARKER;
        WM_DRV_PWM_UNLOCK();
        return WM_ERR_FAILED;
    }

    /* unlock mutex */
    WM_DRV_PWM_UNLOCK();

    /* free mutex */
    wm_os_internal_mutex_delete(data->ctx.lock);

    /* free data */
    memset(device->drv, 0x0, sizeof(wm_drv_pwm_data_t));
    wm_os_internal_free(device->drv);

    device->drv = NULL;

    device->state = WM_DEV_ST_UNINIT;

    WM_DRV_PWM_LOG_I("pwm driver de-initialized!");

    return ret;
}

int wm_drv_pwm_channel_init(wm_device_t *device, wm_drv_pwm_channel_cfg_t *cfg)
{
    int ret                 = WM_ERR_SUCCESS;
    wm_drv_pwm_data_t *data = NULL;

    if (!WM_PWM_PRE_VALID()) {
        WM_DRV_PWM_LOG_MARKER;
        return WM_ERR_INVALID_PARAM;
    }

    if (cfg == NULL || cfg->channel >= WM_PWM_CHANNEL_MAX) {
        WM_DRV_PWM_LOG_MARKER;
        return WM_ERR_INVALID_PARAM;
    }

    if (cfg->mode >= WM_PWM_MODE_MAX || cfg->int_type > WM_PWM_CAP_INT_DMA) {
        WM_DRV_PWM_LOG_MARKER;
        return WM_ERR_INVALID_PARAM;
    }

    if (cfg->cap_cache_cnt == 0 && cfg->mode == WM_PWM_IN_CAP) {
        WM_DRV_PWM_LOG_MARKER;
        return WM_ERR_INVALID_PARAM;
    }

    if (!WM_DRV_PWM_TRY_LOCK()) {
        return WM_ERR_FAILED;
    }

    data = device->drv;

    /* try deinit previous cfg of this channel if exist */
    WM_DRV_PWM_OPS(device)->channel_deinit(data, cfg->channel);

    ret = WM_DRV_PWM_OPS(device)->channel_init(device->drv, cfg);
    if (ret != WM_ERR_SUCCESS) {
        WM_DRV_PWM_LOG_MARKER;
        goto lerr;
    }

lerr:
    WM_DRV_PWM_UNLOCK();

    return ret;
}

int wm_drv_pwm_channel_deinit(wm_device_t *device, uint8_t channel)
{
    int ret = WM_ERR_SUCCESS;

    wm_drv_pwm_data_t *data = NULL;

    if (!WM_PWM_PRE_VALID()) {
        return WM_ERR_INVALID_PARAM;
    }

    if (!WM_DRV_PWM_TRY_LOCK()) {
        return WM_ERR_FAILED;
    }

    data = device->drv;

    ret = WM_DRV_PWM_OPS(device)->channel_deinit(data, channel);
    if (ret != WM_ERR_SUCCESS) {
        WM_DRV_PWM_LOG_MARKER;
        goto lerr;
    }

lerr:
    WM_DRV_PWM_UNLOCK();
    return ret;
}

int wm_drv_pwm_channel_start(wm_device_t *device, uint8_t channel)
{
    int ret = WM_ERR_SUCCESS;

    wm_drv_pwm_data_t *data = NULL;

    if (!WM_PWM_PRE_VALID()) {
        return WM_ERR_INVALID_PARAM;
    }

    if (!WM_DRV_PWM_TRY_LOCK()) {
        return WM_ERR_FAILED;
    }

    data = device->drv;

    ret = WM_DRV_PWM_OPS(device)->channel_start(data, channel);
    if (ret != WM_ERR_SUCCESS) {
        WM_DRV_PWM_LOG_MARKER;
        goto lerr;
    }

lerr:
    WM_DRV_PWM_UNLOCK();

    return ret;
}

int wm_drv_pwm_channel_stop(wm_device_t *device, uint8_t channel)
{
    int ret = WM_ERR_SUCCESS;

    wm_drv_pwm_data_t *data = NULL;

    if (!WM_PWM_PRE_VALID()) {
        return WM_ERR_INVALID_PARAM;
    }

    if (!WM_DRV_PWM_TRY_LOCK()) {
        return WM_ERR_FAILED;
    }

    data = device->drv;

    ret = WM_DRV_PWM_OPS(device)->channel_stop(data, channel);
    if (ret != WM_ERR_SUCCESS) {
        WM_DRV_PWM_LOG_MARKER;
        goto lerr;
    }
lerr:
    WM_DRV_PWM_UNLOCK();

    return ret;
}

int wm_drv_pwm_channels_start(wm_device_t *device)
{
    int ret = WM_ERR_SUCCESS;

    wm_drv_pwm_data_t *data = NULL;

    if (!WM_PWM_PRE_VALID()) {
        return WM_ERR_INVALID_PARAM;
    }

    if (!WM_DRV_PWM_TRY_LOCK()) {
        return WM_ERR_FAILED;
    }

    data = device->drv;

    ret = WM_DRV_PWM_OPS(device)->channel_start_all(data);
    if (ret != WM_ERR_SUCCESS) {
        WM_DRV_PWM_LOG_MARKER;
        goto lerr;
    }

lerr:
    WM_DRV_PWM_UNLOCK();

    return ret;
}

int wm_drv_pwm_channels_stop(wm_device_t *device)
{
    int ret = WM_ERR_SUCCESS;

    wm_drv_pwm_data_t *data = NULL;

    if (!WM_PWM_PRE_VALID()) {
        return WM_ERR_INVALID_PARAM;
    }

    if (!WM_DRV_PWM_TRY_LOCK()) {
        return WM_ERR_FAILED;
    }

    data = device->drv;

    ret = WM_DRV_PWM_OPS(device)->channel_stop_all(data);
    if (ret != WM_ERR_SUCCESS) {
        WM_DRV_PWM_LOG_MARKER;
        goto lerr;
    }
lerr:
    WM_DRV_PWM_UNLOCK();

    return ret;
}

int wm_drv_pwm_set_channel_freq(wm_device_t *device, uint8_t channel, uint32_t freq_hz)
{
    int ret = WM_ERR_SUCCESS;

    wm_drv_pwm_data_t *data = NULL;

    /* wm_drv_pwm_ioctl_args_t *ioctl_args = wm_os_internal_calloc(1, sizeof(*ioctl_args)); */
    wm_drv_pwm_ioctl_args_t ioctl_args = { 0 };

    if (!WM_PWM_PRE_VALID()) {
        return WM_ERR_INVALID_PARAM;
    }

    if (!WM_DRV_PWM_TRY_LOCK()) {
        return WM_ERR_FAILED;
    }

    data = device->drv;

    ioctl_args.channel   = channel;
    ioctl_args.u.freq_hz = freq_hz;

    ret = WM_DRV_PWM_OPS(device)->ioctl(data, WM_IOCTL_PWM_CMD_SET_FREQ, &ioctl_args);
    if (ret != WM_ERR_SUCCESS) {
        WM_DRV_PWM_LOG_MARKER;
        goto lerr;
    }

lerr:
    WM_DRV_PWM_UNLOCK();

    return ret;
}

int wm_drv_pwm_get_channel_freq(wm_device_t *device, uint8_t channel, uint32_t *freq_hz)
{
    int ret = WM_ERR_SUCCESS;

    wm_drv_pwm_data_t *data = NULL;

    wm_drv_pwm_ioctl_args_t ioctl_args = { 0 };

    if (freq_hz == NULL) {
        WM_DRV_PWM_LOG_MARKER;
        return WM_ERR_INVALID_PARAM;
    }

    if (!WM_PWM_PRE_VALID()) {
        return WM_ERR_INVALID_PARAM;
    }

    if (!WM_DRV_PWM_TRY_LOCK()) {
        return WM_ERR_FAILED;
    }

    ioctl_args.channel = channel;

    data = device->drv;

    ret = WM_DRV_PWM_OPS(device)->ioctl(data, WM_IOCTL_PWM_CMD_GET_FREQ, &ioctl_args);
    if (ret != WM_ERR_SUCCESS) {
        WM_DRV_PWM_LOG_MARKER;
        goto lerr;
    }

    *freq_hz = ioctl_args.u.freq_hz;

lerr:
    WM_DRV_PWM_UNLOCK();

    return ret;
}

int wm_drv_pwm_set_channel_period_clkdiv(wm_device_t *device, uint8_t channel, uint8_t period, uint16_t clkdiv)
{
    int ret = WM_ERR_SUCCESS;

    wm_drv_pwm_data_t *data = NULL;

    wm_drv_pwm_ioctl_args_t ioctl_args = { 0 };

    if (!WM_PWM_PRE_VALID()) {
        return WM_ERR_INVALID_PARAM;
    }

    if (!WM_DRV_PWM_TRY_LOCK()) {
        return WM_ERR_FAILED;
    }

    ioctl_args.channel                   = channel;
    ioctl_args.u.freq_cycle.period_cycle = period;
    ioctl_args.u.freq_cycle.clkdiv       = clkdiv;

    data = device->drv;

    ret = WM_DRV_PWM_OPS(device)->ioctl(data, WM_IOCTL_PWM_CMD_SET_FREQ_CYCLE, &ioctl_args);
    if (ret != WM_ERR_SUCCESS) {
        WM_DRV_PWM_LOG_MARKER;
        goto lerr;
    }

lerr:
    WM_DRV_PWM_UNLOCK();

    return ret;
}

int wm_drv_pwm_get_channel_period_clkdiv(wm_device_t *device, uint8_t channel, uint8_t *period, uint16_t *clkdiv)
{
    int ret = WM_ERR_SUCCESS;

    wm_drv_pwm_data_t *data = NULL;

    wm_drv_pwm_ioctl_args_t ioctl_args = { 0 };

    if (!WM_PWM_PRE_VALID()) {
        return WM_ERR_INVALID_PARAM;
    }

    if (!WM_DRV_PWM_TRY_LOCK()) {
        return WM_ERR_FAILED;
    }

    ioctl_args.channel = channel;

    data = device->drv;

    ret = WM_DRV_PWM_OPS(device)->ioctl(data, WM_IOCTL_PWM_CMD_GET_FREQ_CYCLE, &ioctl_args);
    if (ret != WM_ERR_SUCCESS) {
        WM_DRV_PWM_LOG_MARKER;
        goto lerr;
    }

    if (period) {
        *period = ioctl_args.u.freq_cycle.period_cycle;
    }

    if (clkdiv) {
        *clkdiv = ioctl_args.u.freq_cycle.clkdiv;
    }

lerr:
    WM_DRV_PWM_UNLOCK();

    return ret;
}

int wm_drv_pwm_set_channel_duty(wm_device_t *device, uint8_t channel, uint8_t duty_cycle)
{
    int ret = WM_ERR_SUCCESS;

    wm_drv_pwm_data_t *data = NULL;

    wm_drv_pwm_ioctl_args_t ioctl_args = { 0 };

    if (!WM_PWM_PRE_VALID()) {
        return WM_ERR_INVALID_PARAM;
    }

    if (!WM_DRV_PWM_TRY_LOCK()) {
        return WM_ERR_FAILED;
    }

    ioctl_args.channel      = channel;
    ioctl_args.u.duty_cycle = duty_cycle;

    data = device->drv;

    ret = WM_DRV_PWM_OPS(device)->ioctl(data, WM_IOCTL_PWM_CMD_SET_DUTY_CYCLE, &ioctl_args);
    if (ret != WM_ERR_SUCCESS) {
        WM_DRV_PWM_LOG_MARKER;
        goto lerr;
    }

lerr:
    WM_DRV_PWM_UNLOCK();

    return ret;
}

int wm_drv_pwm_get_channel_duty(wm_device_t *device, uint8_t channel, uint8_t *duty_cycle)
{
    int ret = WM_ERR_SUCCESS;

    wm_drv_pwm_data_t *data = NULL;

    wm_drv_pwm_ioctl_args_t ioctl_args = { 0 };

    if (duty_cycle == NULL) {
        WM_DRV_PWM_LOG_MARKER;
        return WM_ERR_INVALID_PARAM;
    }

    if (!WM_PWM_PRE_VALID()) {
        return WM_ERR_INVALID_PARAM;
    }

    if (!WM_DRV_PWM_TRY_LOCK()) {
        return WM_ERR_FAILED;
    }

    ioctl_args.channel = channel;

    data = device->drv;

    ret = WM_DRV_PWM_OPS(device)->ioctl(data, WM_IOCTL_PWM_CMD_GET_DUTY_CYCLE, &ioctl_args);
    if (ret != WM_ERR_SUCCESS) {
        WM_DRV_PWM_LOG_MARKER;
        goto lerr;
    }

    *duty_cycle = ioctl_args.u.duty_cycle;

lerr:
    WM_DRV_PWM_UNLOCK();

    return ret;
}

int wm_drv_pwm_set_channel_period_num(wm_device_t *device, uint8_t channel, uint8_t period_num)
{
    int ret = WM_ERR_SUCCESS;

    wm_drv_pwm_data_t *data = NULL;

    wm_drv_pwm_ioctl_args_t ioctl_args = { 0 };

    if (!WM_PWM_PRE_VALID()) {
        return WM_ERR_INVALID_PARAM;
    }

    if (!WM_DRV_PWM_TRY_LOCK()) {
        return WM_ERR_FAILED;
    }

    ioctl_args.channel      = channel;
    ioctl_args.u.period_num = period_num;

    data = device->drv;

    ret = WM_DRV_PWM_OPS(device)->ioctl(data, WM_IOCTL_PWM_CMD_SET_PERIOD_NUM, &ioctl_args);
    if (ret != WM_ERR_SUCCESS) {
        WM_DRV_PWM_LOG_MARKER;
        goto lerr;
    }

lerr:
    WM_DRV_PWM_UNLOCK();

    return ret;
}

int wm_drv_pwm_get_channel_period_num(wm_device_t *device, uint8_t channel, uint8_t *period_num)
{
    int ret = WM_ERR_SUCCESS;

    wm_drv_pwm_data_t *data = NULL;

    wm_drv_pwm_ioctl_args_t ioctl_args = { 0 };

    if (period_num == NULL) {
        WM_DRV_PWM_LOG_MARKER;
        return WM_ERR_INVALID_PARAM;
    }

    if (!WM_PWM_PRE_VALID()) {
        WM_DRV_PWM_LOG_MARKER;
        return WM_ERR_INVALID_PARAM;
    }

    if (!WM_DRV_PWM_TRY_LOCK()) {
        return WM_ERR_FAILED;
    }

    ioctl_args.channel = channel;

    data = device->drv;

    ret = WM_DRV_PWM_OPS(device)->ioctl(data, WM_IOCTL_PWM_CMD_GET_PERIOD_NUM, &ioctl_args);
    if (ret != WM_ERR_SUCCESS) {
        WM_DRV_PWM_LOG_MARKER;
        goto lerr;
    }

    *period_num = ioctl_args.u.period_num;

lerr:
    WM_DRV_PWM_UNLOCK();

    return ret;
}

int wm_drv_pwm_set_channel_period_int(wm_device_t *device, uint8_t channel, bool en)
{
    int ret = WM_ERR_SUCCESS;

    wm_drv_pwm_data_t *data = NULL;

    wm_drv_pwm_ioctl_args_t ioctl_args = { 0 };

    if (!WM_PWM_PRE_VALID()) {
        return WM_ERR_INVALID_PARAM;
    }

    if (!WM_DRV_PWM_TRY_LOCK()) {
        return WM_ERR_FAILED;
    }

    ioctl_args.channel      = channel;
    ioctl_args.u.period_int = en;

    data = device->drv;

    ret = WM_DRV_PWM_OPS(device)->ioctl(data, WM_IOCTL_PWM_CMD_SET_PERIOD_INT, &ioctl_args);
    if (ret != WM_ERR_SUCCESS) {
        WM_DRV_PWM_LOG_MARKER;
        goto lerr;
    }

lerr:
    WM_DRV_PWM_UNLOCK();

    return ret;
}

int wm_drv_pwm_get_channel_period_int(wm_device_t *device, uint8_t channel, bool *en)
{
    int ret = WM_ERR_SUCCESS;

    wm_drv_pwm_data_t *data = NULL;

    wm_drv_pwm_ioctl_args_t ioctl_args = { 0 };

    if (en == NULL) {
        WM_DRV_PWM_LOG_MARKER;
        return WM_ERR_INVALID_PARAM;
    }

    if (!WM_PWM_PRE_VALID()) {
        WM_DRV_PWM_LOG_MARKER;
        return WM_ERR_INVALID_PARAM;
    }

    if (!WM_DRV_PWM_TRY_LOCK()) {
        return WM_ERR_FAILED;
    }

    ioctl_args.channel = channel;

    data = device->drv;

    ret = WM_DRV_PWM_OPS(device)->ioctl(data, WM_IOCTL_PWM_CMD_GET_PERIOD_INT, &ioctl_args);
    if (ret != WM_ERR_SUCCESS) {
        WM_DRV_PWM_LOG_MARKER;
        goto lerr;
    }

    *en = ioctl_args.u.period_int;

lerr:
    WM_DRV_PWM_UNLOCK();

    return ret;
}

int wm_drv_pwm_set_channel_inverse(wm_device_t *device, uint8_t channel, bool en)
{
    int ret = WM_ERR_SUCCESS;

    wm_drv_pwm_data_t *data = NULL;

    wm_drv_pwm_ioctl_args_t ioctl_args = { 0 };

    if (!WM_PWM_PRE_VALID()) {
        return WM_ERR_INVALID_PARAM;
    }

    if (!WM_DRV_PWM_TRY_LOCK()) {
        return WM_ERR_FAILED;
    }

    ioctl_args.channel   = channel;
    ioctl_args.u.inverse = en;

    data = device->drv;

    ret = WM_DRV_PWM_OPS(device)->ioctl(data, WM_IOCTL_PWM_CMD_SET_INVERSE, &ioctl_args);
    if (ret != WM_ERR_SUCCESS) {
        WM_DRV_PWM_LOG_MARKER;
        goto lerr;
    }

lerr:
    WM_DRV_PWM_UNLOCK();

    return ret;
}

int wm_drv_pwm_get_channel_inverse(wm_device_t *device, uint8_t channel, bool *en)
{
    int ret = WM_ERR_SUCCESS;

    wm_drv_pwm_data_t *data = NULL;

    wm_drv_pwm_ioctl_args_t ioctl_args = { 0 };

    if (en == NULL) {
        WM_DRV_PWM_LOG_MARKER;
        return WM_ERR_INVALID_PARAM;
    }

    if (!WM_PWM_PRE_VALID()) {
        WM_DRV_PWM_LOG_MARKER;
        return WM_ERR_INVALID_PARAM;
    }

    if (!WM_DRV_PWM_TRY_LOCK()) {
        return WM_ERR_FAILED;
    }

    ioctl_args.channel = channel;

    data = device->drv;

    ret = WM_DRV_PWM_OPS(device)->ioctl(data, WM_IOCTL_PWM_CMD_GET_INVERSE, &ioctl_args);
    if (ret != WM_ERR_SUCCESS) {
        WM_DRV_PWM_LOG_MARKER;
        goto lerr;
    }

    *en = ioctl_args.u.inverse;

lerr:
    WM_DRV_PWM_UNLOCK();

    return ret;
}

int wm_drv_pwm_set_channel_brake_level(wm_device_t *device, uint8_t channel, bool en)
{
    int ret = WM_ERR_SUCCESS;

    wm_drv_pwm_data_t *data = NULL;

    wm_drv_pwm_ioctl_args_t ioctl_args = { 0 };

    if (!WM_PWM_PRE_VALID()) {
        return WM_ERR_INVALID_PARAM;
    }

    if (!WM_DRV_PWM_TRY_LOCK()) {
        return WM_ERR_FAILED;
    }

    ioctl_args.channel            = channel;
    ioctl_args.u.brake_level_high = en;

    data = device->drv;

    ret = WM_DRV_PWM_OPS(device)->ioctl(data, WM_IOCTL_PWM_CMD_SET_BRAKE_LEVEL, &ioctl_args);
    if (ret != WM_ERR_SUCCESS) {
        WM_DRV_PWM_LOG_MARKER;
        goto lerr;
    }

lerr:
    WM_DRV_PWM_UNLOCK();

    return ret;
}

int wm_drv_pwm_get_channel_brake_level(wm_device_t *device, uint8_t channel, bool *en)
{
    int ret = WM_ERR_SUCCESS;

    wm_drv_pwm_data_t *data = NULL;

    wm_drv_pwm_ioctl_args_t ioctl_args = { 0 };

    if (en == NULL) {
        WM_DRV_PWM_LOG_MARKER;
        return WM_ERR_INVALID_PARAM;
    }

    if (!WM_PWM_PRE_VALID()) {
        WM_DRV_PWM_LOG_MARKER;
        return WM_ERR_INVALID_PARAM;
    }

    if (!WM_DRV_PWM_TRY_LOCK()) {
        return WM_ERR_FAILED;
    }

    ioctl_args.channel = channel;

    data = device->drv;

    ret = WM_DRV_PWM_OPS(device)->ioctl(data, WM_IOCTL_PWM_CMD_GET_BRAKE_LEVEL, &ioctl_args);
    if (ret != WM_ERR_SUCCESS) {
        WM_DRV_PWM_LOG_MARKER;
        goto lerr;
    }

    *en = ioctl_args.u.brake_level_high;

lerr:
    WM_DRV_PWM_UNLOCK();

    return ret;
}

int wm_drv_pwm_set_channel_deadtime(wm_device_t *device, uint8_t channel, uint8_t cnt, uint8_t clkdiv)
{
    int ret = WM_ERR_SUCCESS;

    wm_drv_pwm_data_t *data = NULL;

    wm_drv_pwm_ioctl_args_t ioctl_args = { 0 };

    if (!WM_PWM_PRE_VALID()) {
        return WM_ERR_INVALID_PARAM;
    }

    if (!WM_DRV_PWM_TRY_LOCK()) {
        return WM_ERR_FAILED;
    }

    ioctl_args.channel           = channel;
    ioctl_args.u.deadtime.clkdiv = clkdiv;
    ioctl_args.u.deadtime.cnt    = cnt;

    data = device->drv;

    ret = WM_DRV_PWM_OPS(device)->ioctl(data, WM_IOCTL_PWM_CMD_SET_DEADTIME, &ioctl_args);
    if (ret != WM_ERR_SUCCESS) {
        WM_DRV_PWM_LOG_MARKER;
        goto lerr;
    }

lerr:
    WM_DRV_PWM_UNLOCK();

    return ret;
}

int wm_drv_pwm_get_channel_deadtime(wm_device_t *device, uint8_t channel, uint8_t *cnt, uint8_t *clkdiv)
{
    int ret = WM_ERR_SUCCESS;

    wm_drv_pwm_data_t *data = NULL;

    wm_drv_pwm_ioctl_args_t ioctl_args = { 0 };

    if (!WM_PWM_PRE_VALID()) {
        return WM_ERR_INVALID_PARAM;
    }

    if (!WM_DRV_PWM_TRY_LOCK()) {
        return WM_ERR_FAILED;
    }

    ioctl_args.channel = channel;

    data = device->drv;

    ret = WM_DRV_PWM_OPS(device)->ioctl(data, WM_IOCTL_PWM_CMD_GET_DEADTIME, &ioctl_args);
    if (ret != WM_ERR_SUCCESS) {
        WM_DRV_PWM_LOG_MARKER;
        goto lerr;
    }

    if (cnt) {
        *cnt = ioctl_args.u.deadtime.cnt;
    }

    if (clkdiv) {
        *clkdiv = ioctl_args.u.deadtime.clkdiv;
    }

lerr:
    WM_DRV_PWM_UNLOCK();

    return ret;
}

int wm_drv_pwm_set_channel_autoreload(wm_device_t *device, uint8_t channel, bool en)
{
    int ret = WM_ERR_SUCCESS;

    wm_drv_pwm_data_t *data = NULL;

    wm_drv_pwm_ioctl_args_t ioctl_args = { 0 };

    if (!WM_PWM_PRE_VALID()) {
        return WM_ERR_INVALID_PARAM;
    }

    if (!WM_DRV_PWM_TRY_LOCK()) {
        return WM_ERR_FAILED;
    }

    ioctl_args.channel      = channel;
    ioctl_args.u.autoreload = en;

    data = device->drv;

    ret = WM_DRV_PWM_OPS(device)->ioctl(data, WM_IOCTL_PWM_CMD_SET_AUTORELOAD, &ioctl_args);
    if (ret != WM_ERR_SUCCESS) {
        WM_DRV_PWM_LOG_MARKER;
        goto lerr;
    }

lerr:
    WM_DRV_PWM_UNLOCK();

    return ret;
}

int wm_drv_pwm_get_channel_autoreload(wm_device_t *device, uint8_t channel, bool *en)
{
    int ret = WM_ERR_SUCCESS;

    wm_drv_pwm_data_t *data = NULL;

    wm_drv_pwm_ioctl_args_t ioctl_args = { 0 };

    if (en == NULL) {
        WM_DRV_PWM_LOG_MARKER;
        return WM_ERR_INVALID_PARAM;
    }

    if (!WM_PWM_PRE_VALID()) {
        WM_DRV_PWM_LOG_MARKER;
        return WM_ERR_INVALID_PARAM;
    }

    if (!WM_DRV_PWM_TRY_LOCK()) {
        return WM_ERR_FAILED;
    }

    ioctl_args.channel = channel;

    data = device->drv;

    ret = WM_DRV_PWM_OPS(device)->ioctl(data, WM_IOCTL_PWM_CMD_GET_AUTORELOAD, &ioctl_args);
    if (ret != WM_ERR_SUCCESS) {
        WM_DRV_PWM_LOG_MARKER;
        goto lerr;
    }

    *en = ioctl_args.u.autoreload;

lerr:
    WM_DRV_PWM_UNLOCK();

    return ret;
}

int wm_drv_pwm_set_channel_capture_cb(wm_device_t *device, uint8_t channel, wm_drv_pwm_report_cb_t cb, void *arg)
{
    int ret = WM_ERR_SUCCESS;

    wm_drv_pwm_data_t *data = NULL;

    wm_drv_pwm_ioctl_args_t ioctl_args = { 0 };

    if (!WM_PWM_PRE_VALID()) {
        WM_DRV_PWM_LOG_MARKER;
        return WM_ERR_INVALID_PARAM;
    }

    if (!WM_DRV_PWM_TRY_LOCK()) {
        return WM_ERR_FAILED;
    }

    ioctl_args.channel            = channel;
    ioctl_args.u.report_cb.cb     = cb;
    ioctl_args.u.report_cb.cb_arg = arg;

    data = device->drv;

    ret = WM_DRV_PWM_OPS(device)->ioctl(data, WM_IOCTL_PWM_CMD_SET_CAP_CB, &ioctl_args);
    if (ret != WM_ERR_SUCCESS) {
        WM_DRV_PWM_LOG_MARKER;
        goto lerr;
    }

lerr:
    WM_DRV_PWM_UNLOCK();

    return ret;
}

int wm_drv_pwm_set_channel_output_cb(wm_device_t *device, uint8_t channel, wm_drv_pwm_report_cb_t cb, void *arg)
{
    int ret = WM_ERR_SUCCESS;

    wm_drv_pwm_data_t *data = NULL;

    wm_drv_pwm_ioctl_args_t ioctl_args = { 0 };

    if (!WM_PWM_PRE_VALID()) {
        WM_DRV_PWM_LOG_MARKER;
        return WM_ERR_INVALID_PARAM;
    }

    if (!WM_DRV_PWM_TRY_LOCK()) {
        return WM_ERR_FAILED;
    }

    ioctl_args.channel            = channel;
    ioctl_args.u.report_cb.cb     = cb;
    ioctl_args.u.report_cb.cb_arg = arg;

    data = device->drv;

    ret = WM_DRV_PWM_OPS(device)->ioctl(data, WM_IOCTL_PWM_CMD_SET_OUT_CB, &ioctl_args);
    if (ret != WM_ERR_SUCCESS) {
        WM_DRV_PWM_LOG_MARKER;
        goto lerr;
    }

lerr:
    WM_DRV_PWM_UNLOCK();

    return ret;
}

void wm_drv_pwm_show_cfg_regs(wm_device_t *device)
{
    int ret = WM_ERR_SUCCESS;

    wm_drv_pwm_data_t *data = NULL;

    wm_drv_pwm_ioctl_args_t ioctl_args = { 0 };

    WM_DRV_PWM_LOG_MARKER;

    if (!WM_PWM_PRE_VALID()) {
        WM_DRV_PWM_LOG_MARKER;
        return;
    }

    if (!WM_DRV_PWM_TRY_LOCK()) {
        return;
    }

    data = device->drv;

    ret = WM_DRV_PWM_OPS(device)->ioctl(data, WM_IOCTL_PWM_CMD_SHW_CFGS, &ioctl_args);
    if (ret != WM_ERR_SUCCESS) {
        WM_DRV_PWM_LOG_MARKER;
        goto lerr;
    }

lerr:
    WM_DRV_PWM_UNLOCK();
}
