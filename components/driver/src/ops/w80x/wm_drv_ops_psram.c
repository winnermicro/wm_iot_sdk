/**
 * @file wm_drv_ops_psram.c
 *
 * @brief psram ops Module
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

#include "wmsdk_config.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "wm_types.h"
#include "wm_error.h"
#include "wm_attr.h"

#include "wm_osal.h"

#include "wm_irq.h"
#include "wm_drv_rcc.h"
#include "wm_drv_gpio.h"
#include "wm_drv_irq.h"
#include "wm_hal_psram.h"
#include "wm_drv_psram.h"
#include "wm_dt_hw.h"
#include "wm_drv_dma.h"

/**
 * @brief minimum size for DMA copy
 */
#define PSRAM_MIN_DMA_SIZE        1024

/**
  * @brief  DMA using mask in attribute status
  */
#define PSRAM_STATUS_DMA_USING    (1 << 0)

/**
  * @brief  invalid DMA  handle
  */
#define PSRAM_INVALID_DMA_CHANNEL 0xFF

/**
  * @brief  mutex lock for api
  */
#define WM_DRV_PSRAM_LOCK(ctx)                                                                   \
    do {                                                                                         \
        if (wm_os_internal_mutex_acquire((ctx)->mutex, WM_OS_WAIT_TIME_MAX) != WM_ERR_SUCCESS) { \
            return WM_ERR_FAILED;                                                                \
        }                                                                                        \
    } while (0)

/**
  * @brief  mutex unlock for api
  */
#define WM_DRV_PSRAM_UNLOCK(ctx) wm_os_internal_mutex_release((ctx)->mutex)

/**
 * @brief Local api types
 */
typedef struct {
    int (*init)(wm_device_t *device);
    int (*deinit)(wm_device_t *device);

    int (*register_callback)(wm_device_t *device, wm_drv_psram_callback_t callback, void *user_data);
    int (*ioctrl)(wm_device_t *device, int cmd, uint32_t param, uint32_t *arg);
    int (*get_size)(wm_device_t *device, uint32_t *size);
    int (*memcpy_dma)(wm_device_t *device, void *dst, const void *src, uint32_t size);
} wm_drv_psram_ops_t;

/**
  * @brief  driver context struct
  */
typedef struct {
    wm_os_mutex_t *mutex;             /**< mutex for lock api  */
    wm_drv_psram_callback_t callback; /**< callback            */
    void *user_data;                  /**< user data           */
    wm_device_t *dma_dev;             /**< dma device          */
    uint8_t dma_ch;                   /**< tx dma  channel     */
    uint8_t status;                   /**< status              */
} wm_drv_psram_ctx_t;

/**
  * @brief  driver data struct
  */
typedef struct {
    wm_hal_psram_dev_t hal_dev; /**< hal device     */
    wm_drv_psram_ctx_t ctx;     /**< driver context */
} wm_drv_psram_data_t;

/**
  * @brief  PSRAM device struct
  */
typedef struct {
    char *name;               /**< device name                       */
    wm_dt_hw_psram_t *hw;     /**< hardware info, @ref wm_dt_hw_psram_t */
    wm_drv_psram_ops_t *ops;  /**< device operation interface        */
    wm_drv_psram_data_t *drv; /**< driver context data               */
    wm_device_status_t state; /**< device state                      */
    void *priv;               /**< user private data                 */
} wm_drv_psram_dev_t;

static int wm_drv_psram_pin_config(wm_dt_hw_pin_cfg_t *pin_cfg, int count)
{
    wm_gpio_num_t pin;

    for (int i = 0; i < count; i++) {
        pin = pin_cfg[i].pin_num;
        wm_drv_gpio_iomux_func_sel(pin, pin_cfg[i].pin_mux);
    }

    return WM_ERR_SUCCESS;
}

static void wm_drv_psram_dma_done_callback(wm_hal_psram_dev_t *device, void *user_data)
{
    wm_drv_psram_dev_t *dev = (wm_drv_psram_dev_t *)user_data;
    wm_drv_psram_ctx_t *ctx = &dev->drv->ctx;

    if (ctx->callback) {
        ctx->callback((wm_device_t *)dev, ctx->user_data);
    }

    if (ctx->dma_ch != PSRAM_INVALID_DMA_CHANNEL) {
        /*release dma channel in callback will fail when DMA mutex occupied by other tasks*/
        if (wm_drv_dma_release_ch(ctx->dma_dev, ctx->dma_ch, 0) == WM_ERR_SUCCESS) {
            ctx->dma_ch = PSRAM_INVALID_DMA_CHANNEL;
        }
    }

    ctx->status &= ~(PSRAM_STATUS_DMA_USING);
}

static int psram_init(wm_device_t *device)
{
    wm_drv_psram_dev_t *dev = (wm_drv_psram_dev_t *)device;
    wm_hal_psram_dev_t *hal;
    wm_drv_psram_ctx_t *ctx;
    wm_device_t *rcc;

    if (dev->state == WM_DEV_ST_INITED) {
        /*init before*/
        return WM_ERR_ALREADY_INITED;
    }

    dev->drv = calloc(1, sizeof(wm_drv_psram_data_t));
    if (!dev->drv) {
        return WM_ERR_NO_MEM;
    }

    hal = &(dev->drv->hal_dev);
    ctx = &(dev->drv->ctx);

    /* create mutex for api calling*/
    if (wm_os_internal_mutex_create(&ctx->mutex) != WM_OS_STATUS_SUCCESS) {
        free(dev->drv);
        return WM_ERR_FAILED;
    }

    /*get dma device*/
    ctx->dma_dev = wm_dt_get_device_by_name(dev->hw->dma_device_name);
    ctx->dma_ch  = PSRAM_INVALID_DMA_CHANNEL;

    /* config SIO0~3,SCLK,CS pin*/
    wm_drv_psram_pin_config(dev->hw->pin_cfg, dev->hw->pin_cfg_count);

    /* enable psram clock*/
    rcc = wm_dt_get_device_by_name(dev->hw->rcc_device_name);
    wm_drv_clock_enable(rcc, WM_RCC_QSRAM_GATE_EN);

    /* init hal dev attribute */
    hal->reg       = (wm_psram_reg_t *)(dev->hw->reg_base);
    hal->ref_clock = wm_drv_rcc_get_config_clock(rcc, WM_RCC_TYPE_CPU) * UNIT_MHZ;
    hal->clock_hz  = dev->hw->psram_cfg.clock_hz;
    hal->quad      = dev->hw->psram_cfg.spi_mode;

    /* init hal psram */
    if (wm_hal_psram_init(hal) != WM_ERR_SUCCESS) {
        wm_os_internal_recursive_mutex_delete(ctx->mutex);
        free(dev->drv);
        dev->drv = NULL;
        return WM_ERR_FAILED;
    }

    wm_hal_psram_register_callback(hal, wm_drv_psram_dma_done_callback, dev);

    dev->state = WM_DEV_ST_INITED;

    return WM_ERR_SUCCESS;
}

static int psram_deinit(wm_device_t *device)
{
    wm_drv_psram_dev_t *dev = (wm_drv_psram_dev_t *)device;
    wm_drv_psram_ctx_t *ctx;

    if (dev->state == WM_DEV_ST_INITED && dev->drv) {
        ctx = &dev->drv->ctx;

        WM_DRV_PSRAM_LOCK(ctx);

        /*invalid state*/
        dev->state = WM_DEV_ST_INITING;

        /*wait copy end*/
        while (ctx->status & PSRAM_STATUS_DMA_USING) {
            wm_os_internal_time_delay(1);
        }

        /*release DMA channel*/
        if (ctx->dma_ch != PSRAM_INVALID_DMA_CHANNEL) {
            wm_drv_dma_release_ch(ctx->dma_dev, ctx->dma_ch, WM_OS_WAIT_TIME_MAX);
            ctx->dma_ch = PSRAM_INVALID_DMA_CHANNEL;
        }

        /*deinit hal psram*/
        wm_hal_psram_deinit(&dev->drv->hal_dev);

        /*disable clock*/
        wm_drv_clock_disable(wm_dt_get_device_by_name(dev->hw->rcc_device_name), WM_RCC_QSRAM_GATE_EN);

        WM_DRV_PSRAM_UNLOCK(ctx);

        /*delete mutex*/
        wm_os_internal_recursive_mutex_delete(ctx->mutex);

        free(dev->drv);
        dev->drv   = NULL;
        dev->state = WM_DEV_ST_UNINIT;

        return WM_ERR_SUCCESS;
    } else {
        return WM_ERR_FAILED;
    }
}

static int psram_register_callback(wm_device_t *device, wm_drv_psram_callback_t callback, void *user_data)
{
    wm_drv_psram_ctx_t *ctx;
    wm_drv_psram_dev_t *dev;

    dev = (wm_drv_psram_dev_t *)device;
    if (dev->state != WM_DEV_ST_INITED) {
        return WM_ERR_NO_INITED;
    }
    ctx = &dev->drv->ctx;

    WM_DRV_PSRAM_LOCK(ctx);

    /*wait copy end*/
    while (ctx->status & PSRAM_STATUS_DMA_USING) {
        wm_os_internal_time_delay(1);
    }

    ctx->callback  = callback;
    ctx->user_data = user_data;
    WM_DRV_PSRAM_UNLOCK(ctx);

    return WM_ERR_SUCCESS;
}

int psram_ioctrl(wm_device_t *device, int cmd, uint32_t param, uint32_t *arg)
{
    int ret;
    wm_drv_psram_dev_t *dev;
    wm_drv_psram_ctx_t *ctx;

    dev = (wm_drv_psram_dev_t *)device;
    if (dev->state != WM_DEV_ST_INITED) {
        return WM_ERR_NO_INITED;
    }

    WM_DRV_PSRAM_LOCK(&dev->drv->ctx);

    ctx = &dev->drv->ctx;

    /*wait copy end*/
    while (ctx->status & PSRAM_STATUS_DMA_USING) {
        wm_os_internal_time_delay(1);
    }

    ret = wm_hal_psram_ioctrl(&dev->drv->hal_dev, cmd, param, arg);
    WM_DRV_PSRAM_UNLOCK(&dev->drv->ctx);

    return ret;
}

static int psram_get_size(wm_device_t *device, uint32_t *size)
{
    int ret;
    wm_drv_psram_dev_t *dev;

    dev = (wm_drv_psram_dev_t *)device;

    if (dev->state != WM_DEV_ST_INITED) {
        return WM_ERR_NO_INITED;
    }

    if (!(size)) {
        return WM_ERR_INVALID_PARAM;
    }

    WM_DRV_PSRAM_LOCK(&dev->drv->ctx);
    ret = wm_hal_psram_get_size(&dev->drv->hal_dev, size);
    WM_DRV_PSRAM_UNLOCK(&dev->drv->ctx);

    return ret;
}

static int psram_memcpy_dma(wm_device_t *device, void *dst, const void *src, uint32_t size)
{
    wm_hal_psram_dma_t config;

    int ret;
    wm_drv_psram_dev_t *dev = (wm_drv_psram_dev_t *)device;
    wm_drv_psram_ctx_t *ctx;
    uint32_t start_bound;
    uint32_t end_bound;

    if (dev->state != WM_DEV_ST_INITED) {
        return WM_ERR_NO_INITED;
    }

    if (!(dst && src && size)) {
        return WM_ERR_INVALID_PARAM;
    }

    start_bound = CONFIG_PSRAM_BASE_ADDR;
    end_bound   = CONFIG_PSRAM_BASE_ADDR + dev->drv->hal_dev.size;

    /*check dst addr is in range*/
    if (((uint32_t)dst) >= start_bound && ((uint32_t)dst) < end_bound && ((uint32_t)dst) + size > end_bound) {
        return WM_ERR_INVALID_PARAM;
    }

    /*check src addr is in range*/
    if (((uint32_t)src) >= start_bound && ((uint32_t)src) < end_bound && ((uint32_t)src) + size > end_bound) {
        return WM_ERR_INVALID_PARAM;
    }

    ctx = &dev->drv->ctx;

    WM_DRV_PSRAM_LOCK(ctx);

    if (ctx->status & PSRAM_STATUS_DMA_USING) {
        /*DMA is using, Do not use cpu copy at the same time*/
        ret = WM_ERR_BUSY;
        WM_DRV_PSRAM_UNLOCK(ctx);
        return ret;
    }

    ctx->status |= PSRAM_STATUS_DMA_USING;

    /*call wm_drv_dma_release_ch in callback may fail, so the last dma channel maybe valid*/
    if (ctx->dma_dev && !(((uint32_t)src) & 3) && !(((uint32_t)dst) & 3) && size >= PSRAM_MIN_DMA_SIZE &&
        (ctx->dma_ch != PSRAM_INVALID_DMA_CHANNEL || wm_drv_dma_request_ch(ctx->dma_dev, &ctx->dma_ch, 0) == WM_ERR_SUCCESS)) {
        /*Get hal dma device*/
        config.dma_dev = &((wm_dma_data_t *)ctx->dma_dev->drv)->hal_dev;
        config.dma_ch  = ctx->dma_ch;

        /*copy data by DMA */
        ret = wm_hal_psram_memcpy(&dev->drv->hal_dev, dst, src, size, &config);

    } else {
        /*copy data by CPU*/
        memcpy(dst, src, size);

        /*notify user copy end.*/
        if (ctx->callback) {
            ctx->callback((wm_device_t *)dev, ctx->user_data);
        }
        ctx->status &= ~(PSRAM_STATUS_DMA_USING);
        ret = WM_ERR_SUCCESS;
    }

    WM_DRV_PSRAM_UNLOCK(ctx);

    return ret;
}

/*internal interface ops*/
const wm_drv_psram_ops_t wm_drv_psram_ops = {
    .init              = psram_init,
    .deinit            = psram_deinit,
    .register_callback = psram_register_callback,
    .get_size          = psram_get_size,
    .ioctrl            = psram_ioctrl,
    .memcpy_dma        = psram_memcpy_dma,
};
