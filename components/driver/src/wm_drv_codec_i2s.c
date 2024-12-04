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

#define WM_DRV_CODEC_OPS_CHECK(ops, name) ((ops) && (ops)->name)

wm_device_t *wm_drv_codec_i2s_init(const char *name, wm_drv_codec_i2s_cfg_t *cfg)
{
    wm_drv_codec_i2s_ops_t *ops = NULL;
    wm_device_t *codec_device   = NULL;

    /* check input parameters */
    if (name == NULL || cfg == NULL) {
        return NULL;
    }

    /* check codec configuration in DT */
    codec_device = wm_dt_get_device_by_name(name);
    if (codec_device == NULL) {
        return NULL;
    }

    /* check if codec has been initialized already */
    if (codec_device->state != WM_DEV_ST_UNINIT) {
        return NULL;
    }

    if (codec_device->drv != NULL) {
        return NULL;
    }

    /* set to init on going */
    codec_device->state = WM_DEV_ST_INITING;
    ops                 = codec_device->ops;

    /* ops check */
    if (!WM_DRV_CODEC_OPS_CHECK(ops, init)) {
        goto fail;
    }

    if (WM_ERR_SUCCESS != ops->init(codec_device, cfg)) {
        goto fail;
    }

    codec_device->state = WM_DEV_ST_INITED;

    return codec_device;

fail:
    codec_device->state = WM_DEV_ST_UNINIT;

    return NULL;
}

int wm_drv_codec_i2s_deinit(wm_device_t *device)
{
    wm_drv_codec_i2s_ops_t *ops = NULL;

    if (device == NULL || device->state != WM_DEV_ST_INITED) {
        return WM_ERR_NO_INITED;
    }

    ops = device->ops;

    /* ops check */
    if (WM_DRV_CODEC_OPS_CHECK(ops, prev_ops)) {
        if (WM_ERR_SUCCESS != ops->prev_ops(device)) {
            return WM_ERR_NOT_ALLOWED;
        }
    }

    /* ops check */
    if (!WM_DRV_CODEC_OPS_CHECK(ops, deinit)) {
        return WM_ERR_NOT_ALLOWED;
    }

    if (WM_ERR_SUCCESS != ops->deinit(device)) {
        return WM_ERR_FAILED;
    }

    device->state = WM_DEV_ST_UNINIT;

    return WM_ERR_SUCCESS;
}

int wm_drv_codec_i2s_start(wm_device_t *device)
{
    int ret, ret_post = WM_ERR_SUCCESS;

    wm_drv_codec_i2s_ops_t *ops = NULL;

    if (device == NULL || device->state != WM_DEV_ST_INITED) {
        return WM_ERR_NO_INITED;
    }

    ops = device->ops;

    /* ops check */
    if (WM_DRV_CODEC_OPS_CHECK(ops, prev_ops)) {
        if (WM_ERR_SUCCESS != ops->prev_ops(device)) {
            return WM_ERR_NOT_ALLOWED;
        }
    }

    /* ops check */
    if (!WM_DRV_CODEC_OPS_CHECK(ops, start)) {
        return WM_ERR_NOT_ALLOWED;
    }

    ret = ops->start(device);

    if (WM_DRV_CODEC_OPS_CHECK(ops, post_ops)) {
        ret_post = ops->post_ops(device);
    }

    return ret == WM_ERR_SUCCESS ? ret_post : ret;
}

int wm_drv_codec_i2s_stop(wm_device_t *device)
{
    int ret, ret_post = WM_ERR_SUCCESS;

    wm_drv_codec_i2s_ops_t *ops = NULL;

    if (device == NULL || device->state != WM_DEV_ST_INITED) {
        return WM_ERR_NO_INITED;
    }

    ops = device->ops;

    /* ops check */
    if (WM_DRV_CODEC_OPS_CHECK(ops, prev_ops)) {
        if (WM_ERR_SUCCESS != ops->prev_ops(device)) {
            return WM_ERR_NOT_ALLOWED;
        }
    }

    /* ops check */
    if (!WM_DRV_CODEC_OPS_CHECK(ops, stop)) {
        return WM_ERR_NOT_ALLOWED;
    }

    ret = ops->stop(device);

    if (WM_DRV_CODEC_OPS_CHECK(ops, post_ops)) {
        ret_post = ops->post_ops(device);
    }

    return ret == WM_ERR_SUCCESS ? ret_post : ret;
}

int wm_drv_codec_i2s_ioctl(wm_device_t *device, wm_drv_codec_i2s_ioctl_arg_t *arg)
{
    int ret, ret_post = WM_ERR_SUCCESS;

    wm_drv_codec_i2s_ops_t *ops = NULL;

    if (device == NULL || device->state != WM_DEV_ST_INITED) {
        return WM_ERR_NO_INITED;
    }

    ops = device->ops;

    /* ops check */
    if (WM_DRV_CODEC_OPS_CHECK(ops, prev_ops)) {
        if (WM_ERR_SUCCESS != ops->prev_ops(device)) {
            return WM_ERR_NOT_ALLOWED;
        }
    }

    /* ops check */
    if (!WM_DRV_CODEC_OPS_CHECK(ops, ioctl)) {
        return WM_ERR_NOT_ALLOWED;
    }

    ret = ops->ioctl(device, arg);

    if (WM_DRV_CODEC_OPS_CHECK(ops, post_ops)) {
        ret_post = ops->post_ops(device);
    }

    return ret == WM_ERR_SUCCESS ? ret_post : ret;
}

int wm_drv_codec_i2s_dumps(wm_device_t *device)
{
    int ret, ret_post = WM_ERR_SUCCESS;

    wm_drv_codec_i2s_ops_t *ops = NULL;

    if (device == NULL || device->state != WM_DEV_ST_INITED) {
        return WM_ERR_NO_INITED;
    }

    ops = device->ops;

    /* ops check */
    if (WM_DRV_CODEC_OPS_CHECK(ops, prev_ops)) {
        if (WM_ERR_SUCCESS != ops->prev_ops(device)) {
            return WM_ERR_NOT_ALLOWED;
        }
    }

    /* ops check */
    if (!WM_DRV_CODEC_OPS_CHECK(ops, dumps)) {
        return WM_ERR_NOT_ALLOWED;
    }

    ret = ops->dumps(device);

    if (WM_DRV_CODEC_OPS_CHECK(ops, post_ops)) {
        ret_post = ops->post_ops(device);
    }

    return ret == WM_ERR_SUCCESS ? ret_post : ret;
}
