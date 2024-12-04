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

int wm_drv_i2s_ioctl(wm_device_t *device, wm_drv_i2s_ioctl_args_t *arg)
{
    int ret;
    if (!WM_I2S_PRE_VALID()) {
        return WM_ERR_INVALID_PARAM;
    }

    if (!WM_DRV_I2S_TRY_LOCK()) {
        return WM_ERR_FAILED;
    }

    ret = WM_DRV_I2S_OPS(device)->ioctl(device, arg);
    if (ret != WM_ERR_SUCCESS) {
        goto lerr;
    }

lerr:
    WM_DRV_I2S_UNLOCK();

    return ret;
}

#if WM_DRV_I2S_USE_WRITE_EXT_API
int wm_drv_i2s_write_async_ext(wm_device_t *device, void *buf, int len, i2s_free_buf_cb_t cb)
{
    int ret;
    if (!WM_I2S_PRE_VALID()) {
        return WM_ERR_INVALID_PARAM;
    }

    if (buf == NULL || len == 0) {
        WM_DRV_I2S_LOG_MARKER;
        return WM_ERR_FAILED;
    }

    if (!WM_DRV_I2S_TRY_LOCK()) {
        return WM_ERR_FAILED;
    }

    ret = WM_DRV_I2S_OPS(device)->write_async(device, buf, len, cb);
    if (ret != WM_ERR_SUCCESS) {
        goto lerr;
    }

lerr:
    WM_DRV_I2S_UNLOCK();

    return ret;
}
#endif /* WM_DRV_I2S_USE_WRITE_EXT_API */

int wm_drv_i2s_write_async(wm_device_t *device, void *buf, int len)
{
    int ret;
    if (!WM_I2S_PRE_VALID()) {
        return WM_ERR_INVALID_PARAM;
    }

    if (buf == NULL || len <= 0) {
        WM_DRV_I2S_LOG_MARKER;
        return WM_ERR_FAILED;
    }

    if (!WM_DRV_I2S_TRY_LOCK()) {
        return WM_ERR_FAILED;
    }

    ret = WM_DRV_I2S_OPS(device)->write_async(device, buf, len, NULL);
    if (ret != WM_ERR_SUCCESS) {
        goto lerr;
    }

lerr:
    WM_DRV_I2S_UNLOCK();

    return ret;
}

/* clear buffer cached and stop HW transfer */
int wm_drv_i2s_write_stop(wm_device_t *device)
{
    int ret;
    if (!WM_I2S_PRE_VALID()) {
        return WM_ERR_INVALID_PARAM;
    }

    if (!WM_DRV_I2S_TRY_LOCK()) {
        return WM_ERR_FAILED;
    }

    ret = WM_DRV_I2S_OPS(device)->write_stop(device);
    if (ret != WM_ERR_SUCCESS) {
        goto lerr;
    }

lerr:
    WM_DRV_I2S_UNLOCK();

    return ret;
}

int wm_drv_i2s_write_pause(wm_device_t *device)
{
    int ret;
    if (!WM_I2S_PRE_VALID()) {
        return WM_ERR_INVALID_PARAM;
    }

    if (!WM_DRV_I2S_TRY_LOCK()) {
        return WM_ERR_FAILED;
    }

    ret = WM_DRV_I2S_OPS(device)->write_pause(device);
    if (ret != WM_ERR_SUCCESS) {
        goto lerr;
    }

lerr:
    WM_DRV_I2S_UNLOCK();

    return WM_ERR_SUCCESS;
}

int wm_drv_i2s_write_resume(wm_device_t *device)
{
    int ret;
    if (!WM_I2S_PRE_VALID()) {
        return WM_ERR_INVALID_PARAM;
    }

    if (!WM_DRV_I2S_TRY_LOCK()) {
        return WM_ERR_FAILED;
    }

    ret = WM_DRV_I2S_OPS(device)->write_resume(device);
    if (ret != WM_ERR_SUCCESS) {
        goto lerr;
    }

lerr:
    WM_DRV_I2S_UNLOCK();

    return WM_ERR_SUCCESS;
}

int wm_drv_i2s_read_async(wm_device_t *device)
{
    int ret;
    if (!WM_I2S_PRE_VALID()) {
        return WM_ERR_INVALID_PARAM;
    }

    if (!WM_DRV_I2S_TRY_LOCK()) {
        return WM_ERR_FAILED;
    }

    ret = WM_DRV_I2S_OPS(device)->read_async(device);
    if (ret != WM_ERR_SUCCESS) {
        goto lerr;
    }

lerr:
    WM_DRV_I2S_UNLOCK();

    return ret;
}

int wm_drv_i2s_read_stop(wm_device_t *device)
{
    int ret;
    if (!WM_I2S_PRE_VALID()) {
        return WM_ERR_INVALID_PARAM;
    }

    if (!WM_DRV_I2S_TRY_LOCK()) {
        return WM_ERR_FAILED;
    }

    ret = WM_DRV_I2S_OPS(device)->read_stop(device);
    if (ret != WM_ERR_SUCCESS) {
        goto lerr;
    }

lerr:
    WM_DRV_I2S_UNLOCK();

    return ret;
}

int wm_drv_i2s_read_pause(wm_device_t *device)
{
    int ret;
    if (!WM_I2S_PRE_VALID()) {
        return WM_ERR_INVALID_PARAM;
    }

    if (!WM_DRV_I2S_TRY_LOCK()) {
        return WM_ERR_FAILED;
    }

    ret = WM_DRV_I2S_OPS(device)->read_pause(device);
    if (ret != WM_ERR_SUCCESS) {
        goto lerr;
    }

lerr:
    WM_DRV_I2S_UNLOCK();

    return ret;
}

int wm_drv_i2s_read_resume(wm_device_t *device)
{
    int ret;
    if (!WM_I2S_PRE_VALID()) {
        return WM_ERR_INVALID_PARAM;
    }

    if (!WM_DRV_I2S_TRY_LOCK()) {
        return WM_ERR_FAILED;
    }

    ret = WM_DRV_I2S_OPS(device)->read_resume(device);
    if (ret != WM_ERR_SUCCESS) {
        goto lerr;
    }

lerr:
    WM_DRV_I2S_UNLOCK();

    return ret;
}

int wm_drv_i2s_register_write_cb(wm_device_t *device, i2s_tx_done_cb_t cb)
{
    wm_drv_i2s_data_t *data = NULL;
    if (!WM_I2S_PRE_VALID()) {
        return WM_ERR_INVALID_PARAM;
    }

    if (!WM_DRV_I2S_TRY_LOCK()) {
        return WM_ERR_FAILED;
    }

    data = device->drv;

    wm_drv_i2s_ctx_t *ctx = &data->ctx;

    ctx->user_tx_cb = cb;

    WM_DRV_I2S_UNLOCK();

    return WM_ERR_SUCCESS;
}

int wm_drv_i2s_register_read_cb(wm_device_t *device, i2s_rx_ready_cb_t cb)
{
    wm_drv_i2s_data_t *data = NULL;
    if (!WM_I2S_PRE_VALID()) {
        return WM_ERR_INVALID_PARAM;
    }

    if (!WM_DRV_I2S_TRY_LOCK()) {
        return WM_ERR_FAILED;
    }

    data = device->drv;

    wm_drv_i2s_ctx_t *ctx = &data->ctx;

    ctx->user_rx_cb = cb;

    WM_DRV_I2S_UNLOCK();

    return WM_ERR_SUCCESS;
}

wm_device_t *wm_drv_i2s_init(const char *name, wm_drv_i2s_cfg_t *cfg)
{
    int ret               = WM_ERR_SUCCESS;
    wm_drv_i2s_ctx_t *ctx = NULL;

    if (name == NULL || cfg == NULL) {
        WM_DRV_I2S_LOG_MARKER;
        return NULL;
    }

    if (cfg->tx_ready_th > WM_DRV_I2S_NBC_TDEPTH) {
        WM_DRV_I2S_LOG_MARKER;
        return NULL;
    }

    if (cfg->rx_ready_th > WM_DRV_I2S_NBC_RDEPTH) {
        WM_DRV_I2S_LOG_MARKER;
        return NULL;
    }

    wm_device_t *device = wm_dt_get_device_by_name(name);
    if (device == NULL) {
        WM_DRV_I2S_LOG_MARKER;
        return NULL;
    }

    if (device->drv || device->state != WM_DEV_ST_UNINIT) {
        WM_DRV_I2S_LOG_E("The driver has already been initialized.");
        return NULL;
    }

    device->state = WM_DEV_ST_INITING;

    if (device->ops == NULL || device->name == NULL || device->hw == NULL) {
        device->state = WM_DEV_ST_UNINIT;
        return NULL;
    }

    wm_drv_i2s_data_t *data = wm_os_internal_calloc(1, sizeof(*data));
    if (device == NULL) {
        device->state = WM_DEV_ST_UNINIT;
        return NULL;
    }

    device->drv  = data;
    ctx          = &data->ctx;
    ctx->cfg     = *cfg;
    ctx->i2s_dev = device;

    ret = wm_os_internal_mutex_create(&ctx->lock);
    if (ret != WM_ERR_SUCCESS) {
        WM_DRV_I2S_LOG_MARKER;
        goto fail_mutex;
    }

    ret = WM_DRV_I2S_OPS(device)->init(device, &ctx->cfg);
    if (ret != WM_ERR_SUCCESS) {
        WM_DRV_I2S_LOG_MARKER;
        goto fail_ops;
    }

    device->state = WM_DEV_ST_INITED;

    return device;

fail_ops:
    wm_os_internal_mutex_delete(ctx->lock);

fail_mutex:
    memset(ctx, 0x0, sizeof(*ctx));
    wm_os_internal_free(data);

    device->drv   = NULL;
    device->state = WM_DEV_ST_UNINIT;

    return NULL;
}

int wm_drv_i2s_deinit(wm_device_t *device)
{
    int ret                 = WM_ERR_FAILED;
    wm_drv_i2s_data_t *data = device->drv;

    if (!WM_I2S_PRE_VALID()) {
        return WM_ERR_INVALID_PARAM;
    }

    if (!WM_DRV_I2S_TRY_LOCK()) {
        return WM_ERR_FAILED;
    }

    if (device->drv == NULL || device->state != WM_DEV_ST_INITED) {
        WM_DRV_I2S_LOG_E("The driver has not been initialized yet.");
        goto lerr;
    }

    ret = WM_DRV_I2S_OPS(device)->deinit(device);
    if (ret != WM_ERR_SUCCESS) {
        goto lerr;
    }

    /* unlock mutex */
    WM_DRV_I2S_UNLOCK();

    /* free mutex */
    wm_os_internal_mutex_delete(data->ctx.lock);

    /* free data */
    memset(device->drv, 0x0, sizeof(wm_drv_i2s_data_t));
    wm_os_internal_free(device->drv);

    device->drv   = NULL;
    device->state = WM_DEV_ST_UNINIT;

    WM_DRV_I2S_LOG_I("I2S driver de-initialized!");

    return WM_ERR_SUCCESS;

lerr:
    WM_DRV_I2S_UNLOCK();

    return ret;
}

void wm_drv_i2s_dump_cfgs(wm_device_t *device)
{
    extern int wm_hal_i2s_dump_desc(wm_hal_i2s_dev_t * dev);

    wm_hal_i2s_dev_t *dev   = NULL;
    wm_hal_i2s_cfg_t *cfg   = NULL;
    wm_drv_i2s_data_t *data = NULL;
    if (device == NULL) {
        return;
    }
    data = device->drv;
    dev  = &data->dev;
    cfg  = dev->cfg;
    if (cfg == NULL) {
        return;
    }

    wm_drv_i2s_ctx_t *ctx     = &data->ctx;
    wm_drv_i2s_rxbuf_t *rnode = ctx->rnbc.rx_ready_head;

    wm_log_info("cfg.irq_num      :%d", dev->irq_num);
    wm_log_info("cfg.reg_dev      :%p", dev->reg_dev);
    wm_log_info("cfg.cfg          :%p", dev->cfg);
    wm_log_info("cfg.dma_dev      :%p", dev->dma_dev);
    wm_log_info("cfg.mode         :%d", cfg->mode);
    wm_log_info("cfg.dir          :%d", cfg->dir);
    wm_log_info("cfg.std          :%d", cfg->std);
    wm_log_info("cfg.fmt          :%d", cfg->fmt);
    wm_log_info("cfg.ch_type      :%d", cfg->ctype);
    wm_log_info("cfg.xtype        :%d", cfg->xtype);
    wm_log_info("cfg.sample_rate  :%u", ctx->cfg.sample_rate_hz);
    wm_log_info("cfg.dma_ch_tx    :%d", cfg->dma_ch[WM_HAL_I2S_TR_TX]);
    wm_log_info("cfg.dma_ch_rx    :%d", cfg->dma_ch[WM_HAL_I2S_TR_RX]);
    wm_log_info("cfg.node_depth_tx:%d", cfg->desc_node_depth[WM_HAL_I2S_TR_TX]);
    wm_log_info("cfg.node_depth_rx:%d", cfg->desc_node_depth[WM_HAL_I2S_TR_RX]);
    wm_log_info("cfg.buf_size_rx  :%d", cfg->desc_rxbuf_size);
    wm_log_info("cfg.des_alloc_cb :%p", cfg->desc_calloc);
    wm_log_info("cfg.des_free_cb  :%p", cfg->desc_free);
    wm_log_info("cfg.drv_tx_cb    :%p", cfg->cb[WM_HAL_I2S_TR_TX]);
    wm_log_info("cfg.drv_rx_cb    :%p", cfg->cb[WM_HAL_I2S_TR_RX]);

    wm_log_info("tx DMA restart   :%d", dev->priv.desc_start_cnt[WM_HAL_I2S_TR_TX]);
    wm_log_info("tx total pkts    :%d", dev->priv.xfer_cnt[WM_HAL_I2S_TR_TX]);
    wm_log_info("tx desc push pkts:%d", dev->priv.desc_push_cnt[WM_HAL_I2S_TR_TX]);

    wm_log_info("tx pkt threshd   :%d", ctx->cfg.tx_ready_th);
    wm_log_info("tx valid cached  :%d", wm_drv_i2s_w800_trnb_valid_count(&ctx->tnbc.nbi, WM_DRV_I2S_NBC_TDEPTH));
    wm_log_info("tx ready cached  :%d", wm_drv_i2s_w800_trnb_ready_count(&ctx->tnbc.nbi, WM_DRV_I2S_NBC_TDEPTH));

    wm_log_info("rx DMA restart   :%d", dev->priv.desc_start_cnt[WM_HAL_I2S_TR_RX]);
    wm_log_info("rx pkt bufsize   :%d", ctx->cfg.rx_pkt_size);
    wm_log_info("rx pkt threshd   :%d", ctx->cfg.rx_ready_th);
    wm_log_info("rx buf online    :%d", wm_drv_i2s_w800_rnb_online_cnt(&ctx->rnbc));
    wm_log_info("rx buf oncache   :%d", ctx->rnbc.rx_buf_oncache);
    wm_log_info("rx total_bytes   :%llu", ctx->rnbc.rx_total_bytes);
    wm_log_info("rx total pkts    :%d", dev->priv.xfer_cnt[WM_HAL_I2S_TR_RX]);
    wm_log_info("rx desc push pkts:%d", dev->priv.desc_push_cnt[WM_HAL_I2S_TR_RX]);
    wm_log_info("rx flush total   :%d", dev->priv.rx_flush_total);
    wm_log_info("rx flush valid   :%d", dev->priv.rx_flush_valid);
    wm_log_info("rx flush bytes   :%d", dev->priv.rx_flush_bytes);

    for (; rnode; rnode = rnode->next) {
        wm_log_info("\tcached:%p, len:%d, magic:0x%08x", rnode->data, rnode->len, rnode->magic);
    }

    wm_hal_i2s_dump_desc(dev);
}
