/**
 * @file wm_drv_dma.c
 *
 * @brief dma driver layer implementation
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

#include <stdio.h>
#include <string.h>
#include "wm_drv_dma.h"
#include "wm_dt_hw.h"
#include "wm_drv_rcc.h"
#include "wm_drv_ops_dma.c"
#define LOG_TAG "dma"
#include "wm_log.h"

//#define WM_DRV_DMA_lOG_EN
#ifdef WM_DRV_DMA_lOG_EN
#define WM_DRV_DMA_LOG_D         wm_log_debug
#define WM_DRV_DMA_LOG_I         wm_log_info
#define WM_DRV_DMA_LOG_W         wm_log_warn
#define WM_DRV_DMA_LOG_E         wm_log_error
#define WM_DRV_DMA_LOG_DUMP      wm_log_dump
#define WM_DRV_DMA_FUNC_EXIT(fn) WM_DRV_DMA_LOG_D("%s exit", fn)
#else
#define WM_DRV_DMA_LOG_D(...)    ((void)0)
#define WM_DRV_DMA_LOG_I(...)    ((void)0)
#define WM_DRV_DMA_LOG_W(...)    ((void)0)
#define WM_DRV_DMA_LOG_E(...)    ((void)0)
#define WM_DRV_DMA_LOG_DUMP(...) ((void)0)
#define WM_DRV_DMA_FUNC_EXIT(fn) WM_DRV_DMA_LOG_D("%s exit", fn)
#endif
#define WM_DRV_DMA_FUNC_NAME  __func__
#define WM_DRV_DMA_DUMP_WIDTH (0xf)
#define WM_DRV_DMA_NAME       "dma"

WM_DRV_DMA_STATIC wm_drv_dma_status_t wm_w800_driver_dma_unregister_callback(wm_dma_data_t *dma_data)
{
    wm_drv_dma_status_t ret = WM_DRV_DMA_STATUS_SUCCESS;

    for (uint8_t ch = WM_DMA_CH_0; ch < WM_DMA_CH_MAX; ch++) {
        if (wm_hal_dma_unregister_callback(&dma_data->hal_dev, ch) != WM_HAL_DMA_STATUS_SUCCESS) {
            ret = WM_DRV_DMA_STATUS_FAILED;
        }
    }

    return ret;
}

wm_device_t *wm_drv_dma_init(const char *dma_name)
{
    wm_device_t *dev      = NULL;
    wm_drv_dma_ops_t *ops = NULL;

    if (!strcmp(dma_name, WM_DRV_DMA_NAME)) {
        dev = wm_dt_get_device_by_name(dma_name);
        if (!dev) {
            WM_DRV_DMA_LOG_E("get dma dev error ");
            return NULL;
        }

        if (dev->state == WM_DEV_ST_INITED) {
            WM_DRV_DMA_LOG_W("dma already inited");
            return dev;
        }

        dev->state = WM_DEV_ST_UNINIT;
        ops        = dev->ops;

        if (ops == NULL || ops->init == NULL) {
            WM_DRV_DMA_LOG_E("dma ops err ");
            return NULL;
        }

        if (ops->init(dev) != WM_DRV_DMA_STATUS_SUCCESS) {
            WM_DRV_DMA_LOG_E("ops init fail");
            return NULL;
        }

        WM_DRV_DMA_FUNC_EXIT(WM_DRV_DMA_FUNC_NAME);
        return dev;
    } else {
        return NULL;
    }
}

wm_drv_dma_status_t wm_drv_dma_deinit(wm_device_t *dev)
{
    wm_drv_dma_status_t ret = WM_DRV_DMA_STATUS_SUCCESS;
    wm_dma_data_t *dma_data = NULL;
    wm_drv_dma_ops_t *ops   = NULL;

    if (dev == NULL) {
        WM_DRV_DMA_LOG_W("dma not inited");
        ret = WM_DRV_DMA_STATUS_ERROR;
        goto dma_deinit_ext;
    }

    if (dev->state == WM_DEV_ST_UNINIT) {
        WM_DRV_DMA_LOG_W("dma already deinited");
        goto dma_deinit_ext;
    }

    ops      = (wm_drv_dma_ops_t *)dev->ops;
    dma_data = (wm_dma_data_t *)dev->drv;

    if (ops != NULL && ops->deinit != NULL && dma_data) {
        wm_w800_driver_dma_unregister_callback(dma_data);
        ops->deinit(dma_data);

        if (dma_data->drv_ctx.lock) {
            WM_DRV_DMA_DEL_LOCK(dma_data->drv_ctx.lock);
            dma_data->drv_ctx.lock = NULL;
        }

        WM_DRV_DMA_FREE(dma_data);

        dev->drv   = NULL;
        dev->state = WM_DEV_ST_UNINIT;

        WM_DRV_DMA_FUNC_EXIT(WM_DRV_DMA_FUNC_NAME);
    } else {
        WM_DRV_DMA_LOG_E("dma init error or not init");
        ret = WM_DRV_DMA_STATUS_ERROR;
    }
dma_deinit_ext:
    WM_DRV_DMA_LOG_E("dma driver deinit done for dma dev:0x%x with ret:%d", dev, ret);
    return ret;
}

wm_drv_dma_status_t wm_drv_dma_request_ch(wm_device_t *dev, uint8_t *ch, uint32_t wait_time)
{
    if (dev) {
        wm_dma_data_t *dma_data = (wm_dma_data_t *)dev->drv;
        wm_drv_dma_ops_t *ops   = (wm_drv_dma_ops_t *)dev->ops;
        wm_drv_dma_status_t ret = WM_DRV_DMA_STATUS_SUCCESS;

        if (!ops || !ops->request_ch || !dma_data->drv_ctx.lock || !dma_data || !ch) {
            WM_DRV_DMA_LOG_W("dma param error");
            ret = WM_DRV_DMA_STATUS_FAILED;
        } else {
            if (WM_DRV_DMA_GET_LOCK(dma_data->drv_ctx.lock, wait_time) != WM_OS_STATUS_SUCCESS) {
                ret = WM_DRV_DMA_STATUS_TIMEOUT;
            } else {
                ret = ops->request_ch(dma_data, ch);
                WM_DRV_DMA_REL_LOCK(dma_data->drv_ctx.lock);
            }

            WM_DRV_DMA_LOG_I("alloc dma ch:%d with err code:%d", *ch, ret);
        }
        return ret;
    } else {
        return WM_DRV_DMA_STATUS_FAILED;
    }
}

wm_drv_dma_status_t wm_drv_dma_release_ch(wm_device_t *dev, uint8_t ch, uint32_t wait_time)
{
    if (dev) {
        wm_dma_data_t *dma_data = (wm_dma_data_t *)dev->drv;
        wm_drv_dma_ops_t *ops   = (wm_drv_dma_ops_t *)dev->ops;
        wm_drv_dma_status_t ret = WM_DRV_DMA_STATUS_SUCCESS;

        if (!ops || !ops->release_ch || !dma_data->drv_ctx.lock || !dma_data) {
            WM_DRV_DMA_LOG_W("dma already deinited");
            ret = WM_DRV_DMA_STATUS_FAILED;
        } else {
            ret = ops->release_ch(dma_data, ch);
            WM_DRV_DMA_LOG_I("release dma ch:%d with err code:%d", ch, ret);
        }

        return ret;
    } else {
        return WM_DRV_DMA_STATUS_FAILED;
    }
}

wm_drv_dma_status_t wm_drv_dma_register_callback(wm_device_t *dev, wm_dma_ch_t ch, wm_drv_dma_ch_callback_t dma_callback,
                                                 void *user_data)
{
    if (dev) {
        wm_dma_data_t *dma_data = (wm_dma_data_t *)dev->drv;
        wm_drv_dma_status_t ret = WM_DRV_DMA_STATUS_SUCCESS;

        if (!(ch >= WM_DMA_CH_0 && ch < WM_DMA_CH_MAX) || !dma_data || !dma_callback) {
            WM_DRV_DMA_LOG_W("dma already deinited");
            ret = WM_DRV_DMA_STATUS_FAILED;
        } else {
            dma_data->drv_ctx.user_ch_cb[ch].cb      = dma_callback;
            dma_data->drv_ctx.user_ch_cb[ch].cb_priv = user_data;
            WM_DRV_DMA_LOG_I("dma register callback ch:%d with err code:%d", ch, ret);
        }

        return ret;
    } else {
        return WM_DRV_DMA_STATUS_FAILED;
    }
}

WM_HAL_DMA_STATIC void wm_drv_dma_user_isr_dummy(wm_dma_ch_t ch, uint32_t sts, void *user_data)
{
    WM_DRV_DMA_LOG_W("user not regiser cb for ch:%d", ch);
}

wm_drv_dma_status_t wm_drv_dma_unregister_callback(wm_device_t *dev, wm_dma_ch_t ch)
{
    if (dev) {
        wm_dma_data_t *dma_data = (wm_dma_data_t *)dev->drv;
        wm_drv_dma_status_t ret = WM_DRV_DMA_STATUS_SUCCESS;

        if (!(ch >= WM_DMA_CH_0 && ch < WM_DMA_CH_MAX) || !dma_data) {
            WM_DRV_DMA_LOG_W("dma already deinited");
            ret = WM_DRV_DMA_STATUS_FAILED;
        } else {
            dma_data->drv_ctx.user_ch_cb[ch].cb      = wm_drv_dma_user_isr_dummy;
            dma_data->drv_ctx.user_ch_cb[ch].cb_priv = wm_drv_dma_user_isr_dummy;

            WM_DRV_DMA_LOG_I("dma unregister callback ch:%d with err code:%d", ch, ret);
        }

        return ret;
    } else {
        return WM_DRV_DMA_STATUS_FAILED;
    }
}

wm_drv_dma_status_t wm_drv_dma_config(wm_device_t *dev, wm_drv_dma_desc_t *dma_desc)
{
    if (dev) {
        wm_dma_data_t *dma_data = (wm_dma_data_t *)dev->drv;
        wm_drv_dma_ops_t *ops   = (wm_drv_dma_ops_t *)dev->ops;
        wm_drv_dma_status_t ret = WM_DRV_DMA_STATUS_SUCCESS;

        if (!ops || !ops->config_dma || !dma_desc || !dma_data) {
            WM_DRV_DMA_LOG_W("dma already deinited");
            ret = WM_DRV_DMA_STATUS_FAILED;
        } else {
            ret = ops->config_dma(dma_data, dma_desc);

            WM_DRV_DMA_LOG_I("config dma with err code:%d", ret);
        }

        return ret;
    } else {
        return WM_DRV_DMA_STATUS_FAILED;
    }
}

wm_drv_dma_status_t wm_drv_dma_start(wm_device_t *dev, wm_dma_ch_t ch)
{
    if (dev) {
        wm_dma_data_t *dma_data = (wm_dma_data_t *)dev->drv;
        wm_drv_dma_ops_t *ops   = (wm_drv_dma_ops_t *)dev->ops;
        wm_drv_dma_status_t ret = WM_DRV_DMA_STATUS_SUCCESS;

        if (!(ch >= WM_DMA_CH_0 && ch < WM_DMA_CH_MAX) || !ops || !ops->start_dma || !dma_data) {
            WM_DRV_DMA_LOG_W("dma already deinited");
            ret = WM_DRV_DMA_STATUS_FAILED;
        } else {
            ret = ops->start_dma(dma_data, ch);

            WM_DRV_DMA_LOG_I("start dma ch:%d with err code:%d", ch, ret);
        }

        return ret;
    } else {
        return WM_DRV_DMA_STATUS_FAILED;
    }
}

wm_drv_dma_status_t wm_drv_dma_stop(wm_device_t *dev, wm_dma_ch_t ch)
{
    if (dev) {
        wm_dma_data_t *dma_data = (wm_dma_data_t *)dev->drv;
        wm_drv_dma_ops_t *ops   = (wm_drv_dma_ops_t *)dev->ops;
        wm_drv_dma_status_t ret = WM_DRV_DMA_STATUS_SUCCESS;

        if (!(ch >= WM_DMA_CH_0 && ch < WM_DMA_CH_MAX) || !ops || !ops->stop_dma || !dma_data) {
            WM_DRV_DMA_LOG_W("dma already deinited");
            ret = WM_DRV_DMA_STATUS_FAILED;
        } else {
            ret = ops->stop_dma(dma_data, ch);

            WM_DRV_DMA_LOG_I("stop dma ch:%d with err code:%d", ch, ret);
        }

        return ret;
    } else {
        return WM_DRV_DMA_STATUS_FAILED;
    }
}

wm_drv_dma_status_t wm_drv_dma_get_status(wm_device_t *dev, wm_dma_ch_t ch, wm_dma_sts_info_t *dma_sts)
{
    if (dev) {
        wm_dma_data_t *dma_data = (wm_dma_data_t *)dev->drv;
        wm_drv_dma_ops_t *ops   = (wm_drv_dma_ops_t *)dev->ops;
        wm_drv_dma_status_t ret = WM_DRV_DMA_STATUS_SUCCESS;

        if (!(ch >= WM_DMA_CH_0 && ch < WM_DMA_CH_MAX) || !ops || !ops->get_status || !dma_data || !dma_sts) {
            WM_DRV_DMA_LOG_W("dma already deinited");
            ret = WM_DRV_DMA_STATUS_FAILED;
        } else {
            ret = ops->get_status(dma_data, ch, dma_sts);
            WM_DRV_DMA_LOG_I("get dma status:0x%x err code:%d", dma_sts, ret);
        }

        return ret;
    } else {
        return WM_DRV_DMA_STATUS_FAILED;
    }
}

wm_drv_dma_status_t wm_drv_dma_transfer(wm_device_t *dma_dev, wm_dma_ch_t ch, wm_drv_dma_xfer_cfg_t *dma_xfer)
{
    if (dma_dev) {
        wm_drv_dma_desc_t dma_desc = { 0 };
        wm_drv_dma_status_t ret    = WM_DRV_DMA_STATUS_SUCCESS;
        wm_dma_data_t *dma_data    = (wm_dma_data_t *)dma_dev->drv;

        if (!(ch >= WM_DMA_CH_0 && ch < WM_DMA_CH_MAX) || !dma_data || !dma_xfer) {
            WM_DRV_DMA_LOG_W("dma already deinited");
            ret = WM_DRV_DMA_STATUS_FAILED;
        } else {
            dma_desc.ctrl.ch = ch;
            if (WM_ERR_SUCCESS != (ret = wm_drv_dma_stop(dma_dev, dma_desc.ctrl.ch))) {
                wm_log_error("dma stop err");
                return ret;
            }

            /* config dma */
            dma_desc.src                = dma_xfer->src;
            dma_desc.dest               = dma_xfer->dest;
            dma_desc.ctrl.len           = dma_xfer->len;
            dma_desc.ctrl.src_inc_mode  = WM_DRV_DMA_ADDR_INC;
            dma_desc.ctrl.dest_inc_mode = WM_DRV_DMA_ADDR_INC;
            dma_desc.ctrl.int_en        = WM_DRV_DMA_CH_INT_ENABLE;
            dma_desc.ctrl.auto_reload   = dma_xfer->auto_reload;
            dma_desc.ctrl.dma_mode      = WM_DRV_DMA_NORMAL_MODE;
            dma_desc.ctrl.req_src       = dma_xfer->req_src;

            if (WM_ERR_SUCCESS != (ret = wm_drv_dma_config(dma_dev, &dma_desc))) {
                wm_log_error("dma cfg err");
                return ret;
            }

            /* start dma */
            if (WM_ERR_SUCCESS != (ret = wm_drv_dma_start(dma_dev, dma_desc.ctrl.ch))) {
                wm_log_error("dma start err");
                return ret;
            }
        }

        return ret;
    } else {
        return WM_DRV_DMA_STATUS_FAILED;
    }
}
