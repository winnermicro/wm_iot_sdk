/**
 * @file wm_drv_ops_sdh_spi.c
 *
 * @brief DRV_OPS_SDH_SPI Module
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

#include "wm_drv_sdh_spi.h"
#include "wm_drv_rcc.h"
#include "wm_drv_dma.h"
#include "wm_dt_hw.h"
#include "wm_drv_gpio.h"

/**
 *@brief Enable debug logs for the SDH_SPI driver
 */
#define WM_DRV_SDH_SPI_DEBUG 0
#if WM_DRV_SDH_SPI_DEBUG
#define LOG_TAG "DRV SPI"
#include "wm_log.h"
#define WM_DRV_SDH_SPI_LOG_D         wm_log_debug
#define WM_DRV_SDH_SPI_LOG_I         wm_log_info
#define WM_DRV_SDH_SPI_LOG_W         wm_log_warn
#define WM_DRV_SDH_SPI_LOG_E         wm_log_error
#define WM_DRV_SDH_SPI_LOG_DUMP      wm_log_dump
#define WM_DRV_SDH_SPI_FUNC_EXIT(fn) WM_DRV_SDH_SPI_LOG_D("%s exit", fn)
#define WM_DRV_SDH_SPI_FUNC_NAME     __func__
#else
#define WM_DRV_SDH_SPI_LOG_D(...)
#define WM_DRV_SDH_SPI_LOG_I(...)
#define WM_DRV_SDH_SPI_LOG_W(...)
#define WM_DRV_SDH_SPI_LOG_E(...)
#define WM_DRV_SDH_SPI_LOG_DUMP(...)
#define WM_DRV_SDH_SPI_FUNC_EXIT(fn)
#endif

/**
 * @brief Macro to lock the SDH_SPI driver context
 */
#define WM_DRV_SDH_SPI_LOCK(lock)                                                        \
    do {                                                                                 \
        if (!lock) {                                                                     \
            if (WM_ERR_SUCCESS != wm_os_internal_mutex_create(&lock)) {                  \
                WM_DRV_SDH_SPI_LOG_E("mutex create err!");                               \
                return WM_ERR_FAILED;                                                    \
            }                                                                            \
        }                                                                                \
        if (WM_ERR_SUCCESS != wm_os_internal_mutex_acquire(lock, WM_OS_WAIT_TIME_MAX)) { \
            WM_DRV_SDH_SPI_LOG_E("mutex acquire err!");                                  \
            return WM_ERR_FAILED;                                                        \
        }                                                                                \
    } while (0);

/**
 * @brief Macro to unlock the SDH_SPI driver context
 */
#define WM_DRV_SDH_SPI_UNLOCK(lock)         \
    do {                                    \
        if (!lock) {                        \
            return WM_ERR_NO_INITED;        \
        }                                   \
        wm_os_internal_mutex_release(lock); \
    } while (0);

/**
 * @brief Define timeout constants for the SDH_SPI driver operations
 */
#define WM_DRV_OPS_SDH_SPI_CMD_TIMEOUT      (10)       /**< Command operation timeout for SDH_SPI, in milliseconds */
#define WM_DRV_OPS_SDH_SPI_BLOCK_TIMEOUT    (100)      /**< Block transfer timeout for SDH_SPI, in milliseconds */
#define WM_DRV_OPS_SDH_SPI_READY_TIMEOUT    (500)      /**< Ready signal timeout for SDH_SPI, in milliseconds */
#define WM_DRV_OPS_SDH_SPI_DEFAULT_CLOCK_HZ (20000000) /**< Default clock frequency for SDH_SPI, in Hertz */
#define WM_DRV_OPS_SDH_SPI_MAX_BUFFER_SIZE  (65535)    /**< Maximum buffer size, in bytes */

static int wm_drv_ops_sdh_spi_init(wm_device_t *dev);
static int wm_drv_ops_sdh_spi_deinit(wm_device_t *dev);
static int wm_drv_ops_sdh_spi_transceive_sync(wm_device_t *dev, const wm_dt_hw_spim_dev_cfg_t *config, spim_transceive_t *desc,
                                              uint32_t ms_to_wait);
static int wm_drv_ops_sdh_spi_transceive_async(wm_device_t *dev, const wm_dt_hw_spim_dev_cfg_t *config, spim_transceive_t *desc,
                                               wm_spim_callback_t callback, void *usr_data);

/**
 * @brief Structure containing the SDH_SPI driver operations.
 */
typedef struct {
    int (*init)(wm_device_t *dev);   /**< Initialization function for the SDH_SPI device */
    int (*deinit)(wm_device_t *dev); /**< Deinitialization function for the SDH_SPI device */
    int (*transceive_sync)(wm_device_t *dev, const wm_dt_hw_spim_dev_cfg_t *config, spim_transceive_t *desc,
                           uint32_t ms_to_wait); /**< Synchronous transceive function */
    int (*transceive_async)(wm_device_t *dev, const wm_dt_hw_spim_dev_cfg_t *config, spim_transceive_t *desc,
                            wm_spim_callback_t callback, void *usr_data); /**< Asynchronous transceive function */
} wm_drv_sdh_spi_ops_t;

/**
 * @brief Structure representing the context of the SDH_SPI device.
 */
typedef struct {
    wm_os_mutex_t *mutex;        /**< Mutex for synchronization */
    wm_device_t *clock_dev;      /**< Clock device associated with the SDH_SPI device */
    wm_device_t *dma_dev;        /**< DMA device associated with the SDH_SPI device */
    wm_hal_sdh_evt_t evt;        /**< Event type for the SDH_SPI device */
    wm_spim_callback_t callback; /**< Callback function for asynchronous operations */
    void *usr_callback_data;     /**< User data for the callback function */
    uint8_t last_cs_num_op;      /**< Last used chip select GPIO pin number for SPI operations */
    bool is_tx_busy;             /**< Whether the SDH_SPI device is busy */
} wm_drv_sdh_spi_dev_ctx_t;

/**
 * @brief Structure representing the driver data for the SDH_SPI device.
 */
typedef struct {
    wm_hal_sdh_dev_t hal_sdh_dev;     /**< HAL SDH device structure */
    wm_drv_sdh_spi_dev_ctx_t drv_ctx; /**< Driver context for the SDH_SPI device */
} wm_drv_sdh_spi_drv_t;

static int wm_drv_sdh_spi_pin_cfg(wm_device_t *dev)
{
    int ret                    = WM_ERR_SUCCESS;
    wm_dt_hw_sdh_t *hw_sdh_spi = NULL;

    hw_sdh_spi = (wm_dt_hw_sdh_t *)dev->hw;

    wm_device_t *gpio_dev = wm_dt_get_device_by_name("gpio");
    if (gpio_dev == NULL || gpio_dev->state != WM_DEV_ST_INITED) {
        gpio_dev = wm_drv_gpio_init("gpio");
        if (gpio_dev == NULL || gpio_dev->state != WM_DEV_ST_INITED) {
            WM_DRV_SDH_SPI_LOG_E("gpio_dev init failed\n");
            return WM_ERR_FAILED;
        }
    }

    for (uint8_t i = 0; i < hw_sdh_spi->pin_cfg_count; i++) {
        ret = wm_drv_gpio_iomux_func_sel(hw_sdh_spi->pin_cfg[i].pin_num, hw_sdh_spi->pin_cfg[i].pin_mux);
        if (ret != WM_ERR_SUCCESS) {
            return ret;
        }
    }

    return ret;
}

static void wm_drv_sdh_spi_callback(void *hal_dev, wm_hal_sdh_evt_t *evt, void *user_data)
{
    wm_drv_sdh_spi_drv_t *drv_data = (wm_drv_sdh_spi_drv_t *)user_data;

    WM_DRV_SDH_SPI_LOG_D("sdspi callback: %d", evt->type);

    if (drv_data->drv_ctx.callback) {
        if (evt->type == WM_HAL_SDH_EVT_READ_DONE || evt->type == WM_HAL_SDH_EVT_WRITE_DONE) {
            drv_data->drv_ctx.callback(WM_ERR_SUCCESS, drv_data->drv_ctx.usr_callback_data);
        } else {
            drv_data->drv_ctx.callback(WM_ERR_FAILED, drv_data->drv_ctx.usr_callback_data);
        }
    }

    if (evt->is_dma_evt) {
        if (drv_data->hal_sdh_dev.dma_ch != 0xFF) {
            wm_drv_dma_release_ch(drv_data->drv_ctx.dma_dev, drv_data->hal_sdh_dev.dma_ch, WM_OS_WAIT_TIME_MAX);
            drv_data->hal_sdh_dev.dma_ch = 0xFF;
        }
    }

    //set CS as invalid, PULL up
    wm_drv_gpio_fast_write(drv_data->drv_ctx.last_cs_num_op, 1);
    drv_data->drv_ctx.is_tx_busy = false;
}

static int wm_drv_ops_sdh_spi_init(wm_device_t *dev)
{
    int ret                         = WM_ERR_SUCCESS;
    wm_drv_sdh_spi_drv_t *drv_data  = NULL;
    wm_dt_hw_sdh_t *hw_sdh_spi      = NULL;
    wm_dma_data_t *dma_data         = NULL;
    wm_drv_clock_ctx_t *rcc_drv_ctx = NULL;

    if (dev == NULL) {
        return WM_ERR_INVALID_PARAM;
    }

    if (dev->state == WM_DEV_ST_INITED) {
        return WM_ERR_ALREADY_INITED;
    }

    drv_data = (wm_drv_sdh_spi_drv_t *)wm_os_internal_malloc(sizeof(wm_drv_sdh_spi_drv_t));
    if (!drv_data) {
        return WM_ERR_NO_MEM;
    }
    memset(drv_data, 0x00, sizeof(wm_drv_sdh_spi_drv_t));

    hw_sdh_spi                  = (wm_dt_hw_sdh_t *)dev->hw;
    drv_data->drv_ctx.clock_dev = wm_dt_get_device_by_name(hw_sdh_spi->rcc_device_name);
    if (drv_data->drv_ctx.clock_dev == NULL) {
        wm_os_internal_free(drv_data);
        drv_data = NULL;
        return WM_ERR_FAILED;
    }
    drv_data->hal_sdh_dev.reg_base     = (wm_sdh_reg_t *)(hw_sdh_spi->reg_base);
    drv_data->hal_sdh_dev.irq_num      = hw_sdh_spi->irq_cfg.irq_num;
    drv_data->hal_sdh_dev.irq_priority = hw_sdh_spi->irq_cfg.irq_priority;
    drv_data->hal_sdh_dev.fhclk        = wm_drv_rcc_get_config_clock(drv_data->drv_ctx.clock_dev, WM_RCC_TYPE_CPU);

    drv_data->hal_sdh_dev.sdh_cfg.clock_hz  = WM_DRV_OPS_SDH_SPI_DEFAULT_CLOCK_HZ;
    drv_data->hal_sdh_dev.sdh_cfg.xfer_mode = WM_HAL_SDH_XFER_MODE_SPEED_HIGH;
    drv_data->hal_sdh_dev.sdh_cfg.bus_width = WM_SDH_BUS_WIDTH_1BIT;
    drv_data->hal_sdh_dev.sdh_cfg.port_mode = WM_HAL_SDH_PORT_MODE_SPI;
    drv_data->hal_sdh_dev.sdh_cfg.sd_mode   = WM_HAL_SDH_SD_MODE_SDIO;

    drv_data->drv_ctx.dma_dev = wm_dt_get_device_by_name(hw_sdh_spi->dma_device_name);
    if (drv_data->drv_ctx.clock_dev == NULL) {
        wm_os_internal_free(drv_data);
        drv_data = NULL;
        return WM_ERR_FAILED;
    }
    drv_data->drv_ctx.last_cs_num_op  = WM_GPIO_NUM_MAX;
    dma_data                          = (wm_dma_data_t *)drv_data->drv_ctx.dma_dev->drv;
    drv_data->hal_sdh_dev.dma_hal_dev = &dma_data->hal_dev;

    rcc_drv_ctx                       = (wm_drv_clock_ctx_t *)drv_data->drv_ctx.clock_dev->drv;
    drv_data->hal_sdh_dev.rcc_hal_dev = &rcc_drv_ctx->hal_dev;

    drv_data->hal_sdh_dev.dma_ch = 0xFF;
    dev->drv                     = drv_data;

    WM_DRV_SDH_SPI_LOCK(drv_data->drv_ctx.mutex);

    ret = wm_drv_clock_enable(drv_data->drv_ctx.clock_dev, WM_RCC_SDIOM_GATE_EN);
    if (ret != WM_ERR_SUCCESS) {
        goto exit;
    }

    ret = wm_hal_sdh_init(&drv_data->hal_sdh_dev);
    if (ret != WM_ERR_SUCCESS) {
        goto exit;
    }

    ret = wm_drv_sdh_spi_pin_cfg(dev);
    if (ret != WM_ERR_SUCCESS) {
        goto exit;
    }

    ret = wm_hal_sdh_tx_register_callback(&drv_data->hal_sdh_dev, wm_drv_sdh_spi_callback, drv_data);
    if (ret != WM_ERR_SUCCESS) {
        WM_DRV_SDH_SPI_LOG_E("register tx callback err");
        goto exit;
    }

    ret = wm_hal_sdh_rx_register_callback(&drv_data->hal_sdh_dev, wm_drv_sdh_spi_callback, drv_data);
    if (ret != WM_ERR_SUCCESS) {
        WM_DRV_SDH_SPI_LOG_E("register rx callback err");
        goto exit;
    }

    dev->state = WM_DEV_ST_INITED;
    WM_DRV_SDH_SPI_LOG_D("wm_drv_ops_sdh_spi_init successfully");

exit:
    WM_DRV_SDH_SPI_UNLOCK(drv_data->drv_ctx.mutex);
    if (ret != WM_ERR_SUCCESS && dev->drv != NULL) {
        wm_os_internal_free(dev->drv);
        dev->drv = NULL;
    }

    return ret;
}

static int wm_drv_ops_sdh_spi_deinit(wm_device_t *dev)
{
    int ret                        = WM_ERR_SUCCESS;
    wm_drv_sdh_spi_drv_t *drv_data = NULL;
    wm_dt_hw_sdh_t *hw_sdh_spi     = NULL;

    if (dev == NULL) {
        return WM_ERR_INVALID_PARAM;
    }

    if (dev->state != WM_DEV_ST_INITED) {
        return WM_ERR_NO_INITED;
    }

    hw_sdh_spi = (wm_dt_hw_sdh_t *)dev->hw;
    drv_data   = (wm_drv_sdh_spi_drv_t *)dev->drv;

    WM_DRV_SDH_SPI_LOCK(drv_data->drv_ctx.mutex);

    for (uint8_t i = 0; i < hw_sdh_spi->pin_cfg_count; i++) {
        ret = wm_drv_gpio_iomux_func_sel(hw_sdh_spi->pin_cfg[i].pin_num, WM_GPIO_IOMUX_FUN5);
        if (ret != WM_ERR_SUCCESS) {
            goto exit;
        }
    }

    ret = wm_hal_sdh_deinit(&drv_data->hal_sdh_dev);
    if (ret != WM_ERR_SUCCESS) {
        goto exit;
    }

    ret = wm_drv_clock_disable(drv_data->drv_ctx.clock_dev, WM_RCC_SDIOM_GATE_EN);
    if (ret != WM_ERR_SUCCESS) {
        goto exit;
    }

    if (dev->drv != NULL) {
        wm_os_internal_free(dev->drv);
        dev->drv = NULL;
    }

    dev->state = WM_DEV_ST_UNINIT;

exit:
    WM_DRV_SDH_SPI_UNLOCK(drv_data->drv_ctx.mutex);

    return ret;
}

static int wm_drv_ops_sdh_spi_transceive_sync(wm_device_t *dev, const wm_dt_hw_spim_dev_cfg_t *config, spim_transceive_t *desc,
                                              uint32_t ms_to_wait)
{
    int ret                        = WM_ERR_SUCCESS;
    wm_drv_sdh_spi_drv_t *drv_data = NULL;
    wm_hal_sdh_xfer_desc_t xfer    = { 0 };

    if (dev == NULL || config == NULL || desc == NULL) {
        return WM_ERR_INVALID_PARAM;
    }

    drv_data = (wm_drv_sdh_spi_drv_t *)dev->drv;
    if (drv_data == NULL) {
        return WM_ERR_INVALID_PARAM;
    }

    if (desc->tx_buf == NULL && desc->rx_buf == NULL) {
        return WM_ERR_INVALID_PARAM;
    }

    if (config->mode != 0) {
        return WM_ERR_INVALID_PARAM;
    }

    if (desc->tx_len == 0 && desc->rx_len == 0) {
        return WM_ERR_INVALID_PARAM;
    }

    if (desc->tx_len > WM_DRV_OPS_SDH_SPI_MAX_BUFFER_SIZE || desc->rx_len > WM_DRV_OPS_SDH_SPI_MAX_BUFFER_SIZE) {
        return WM_ERR_INVALID_PARAM;
    }

    WM_DRV_SDH_SPI_LOCK(drv_data->drv_ctx.mutex);

    if (drv_data->drv_ctx.is_tx_busy) {
        ret = WM_ERR_BUSY;
        goto exit;
    }

    ret = wm_hal_sdh_set_clock(&drv_data->hal_sdh_dev, config->freq);
    if (ret != WM_ERR_SUCCESS) {
        goto exit;
    }

    if (desc->tx_len > 0) {
        xfer.spi_xfer_desc.mode = WM_HAL_SDH_SPI_TX;

        xfer.spi_xfer_desc.spi_data.tx.buf     = desc->tx_buf;
        xfer.spi_xfer_desc.spi_data.tx.len     = desc->tx_len;
        xfer.spi_xfer_desc.spi_data.tx.timeout = ms_to_wait;
    } else if (desc->rx_len > 0) {
        xfer.spi_xfer_desc.mode = WM_HAL_SDH_SPI_RX;

        xfer.spi_xfer_desc.spi_data.rx.buf     = desc->rx_buf;
        xfer.spi_xfer_desc.spi_data.rx.len     = desc->rx_len;
        xfer.spi_xfer_desc.spi_data.rx.timeout = ms_to_wait;
    }

    //select cs as valid, PULL down
    if (drv_data->drv_ctx.last_cs_num_op != config->pin_cs.pin_num) {
        ret = wm_drv_gpio_iomux_func_sel(config->pin_cs.pin_num, config->pin_cs.pin_mux);
        if (ret != WM_ERR_SUCCESS) {
            goto exit;
        }
        ret = wm_drv_gpio_set_dir(config->pin_cs.pin_num, config->pin_cs.pin_dir);
        if (ret != WM_ERR_SUCCESS) {
            goto exit;
        }
        ret = wm_drv_gpio_set_pullmode(config->pin_cs.pin_num, config->pin_cs.pin_pupd);
        if (ret != WM_ERR_SUCCESS) {
            goto exit;
        }
        drv_data->drv_ctx.last_cs_num_op = config->pin_cs.pin_num;
    }
    wm_drv_gpio_fast_write(config->pin_cs.pin_num, 0);
    ret = wm_hal_sdh_xfer_polling(&drv_data->hal_sdh_dev, &xfer);
    //set CS as invalid, PULL up
    wm_drv_gpio_fast_write(config->pin_cs.pin_num, 1);

exit:
    WM_DRV_SDH_SPI_UNLOCK(drv_data->drv_ctx.mutex);

    return ret;
}

static int wm_drv_ops_sdh_spi_transceive_async(wm_device_t *dev, const wm_dt_hw_spim_dev_cfg_t *config, spim_transceive_t *desc,
                                               wm_spim_callback_t callback, void *usr_data)
{
    int ret                        = WM_ERR_SUCCESS;
    wm_drv_sdh_spi_drv_t *drv_data = NULL;
    wm_hal_sdh_xfer_desc_t xfer    = { 0 };
    wm_drv_dma_status_t dma_status = 0;

    if (dev == NULL || config == NULL || desc == NULL) {
        return WM_ERR_INVALID_PARAM;
    }

    drv_data = (wm_drv_sdh_spi_drv_t *)dev->drv;
    if (drv_data == NULL) {
        return WM_ERR_INVALID_PARAM;
    }

    if (desc->tx_buf == NULL && desc->rx_buf == NULL) {
        return WM_ERR_INVALID_PARAM;
    }

    if (config->mode != 0) {
        return WM_ERR_INVALID_PARAM;
    }

    if (desc->tx_len == 0 && desc->rx_len == 0) {
        return WM_ERR_INVALID_PARAM;
    }

    if (desc->tx_len > WM_DRV_OPS_SDH_SPI_MAX_BUFFER_SIZE || desc->rx_len > WM_DRV_OPS_SDH_SPI_MAX_BUFFER_SIZE) {
        return WM_ERR_INVALID_PARAM;
    }

    if (desc->tx_len % 4 != 0 || desc->rx_len % 4 != 0) { //must 4 byte algin
        return WM_ERR_INVALID_PARAM;
    }

    WM_DRV_SDH_SPI_LOCK(drv_data->drv_ctx.mutex);

    if (drv_data->drv_ctx.is_tx_busy) {
        ret = WM_ERR_BUSY;
        goto exit;
    }
    drv_data->drv_ctx.is_tx_busy = true;

    ret = wm_hal_sdh_set_clock(&drv_data->hal_sdh_dev, config->freq);
    if (ret != WM_ERR_SUCCESS) {
        goto exit;
    }

    if (desc->tx_len > 0) {
        xfer.spi_xfer_desc.mode = WM_HAL_SDH_SPI_TX;

        xfer.spi_xfer_desc.spi_data.tx.buf = desc->tx_buf;
        xfer.spi_xfer_desc.spi_data.tx.len = desc->tx_len;
    } else if (desc->rx_len > 0) {
        xfer.spi_xfer_desc.mode = WM_HAL_SDH_SPI_RX;

        xfer.spi_xfer_desc.spi_data.rx.buf = desc->rx_buf;
        xfer.spi_xfer_desc.spi_data.rx.len = desc->rx_len;
    }

    drv_data->drv_ctx.callback          = callback;
    drv_data->drv_ctx.usr_callback_data = usr_data;

    if (drv_data->hal_sdh_dev.dma_ch == 0xFF) {
        dma_status = wm_drv_dma_request_ch(drv_data->drv_ctx.dma_dev, &drv_data->hal_sdh_dev.dma_ch, 0);
        if (dma_status != WM_DRV_DMA_STATUS_SUCCESS) {
            ret = WM_ERR_FAILED;
            goto exit;
        }
    }

    //select cs as valid, PULL down
    if (drv_data->drv_ctx.last_cs_num_op != config->pin_cs.pin_num) {
        ret = wm_drv_gpio_iomux_func_sel(config->pin_cs.pin_num, config->pin_cs.pin_mux);
        if (ret != WM_ERR_SUCCESS) {
            goto exit;
        }
        ret = wm_drv_gpio_set_dir(config->pin_cs.pin_num, config->pin_cs.pin_dir);
        if (ret != WM_ERR_SUCCESS) {
            goto exit;
        }
        ret = wm_drv_gpio_set_pullmode(config->pin_cs.pin_num, config->pin_cs.pin_pupd);
        if (ret != WM_ERR_SUCCESS) {
            goto exit;
        }
        drv_data->drv_ctx.last_cs_num_op = config->pin_cs.pin_num;
    }
    wm_drv_gpio_fast_write(config->pin_cs.pin_num, 0);

    ret = wm_hal_sdh_xfer_dma(&drv_data->hal_sdh_dev, &xfer);
    if (ret != WM_ERR_SUCCESS) {
        WM_DRV_SDH_SPI_LOG_E("xfer err");
        goto exit;
    }

exit:
    if (ret != WM_ERR_SUCCESS) {
        drv_data->drv_ctx.is_tx_busy = false;
    }
    WM_DRV_SDH_SPI_UNLOCK(drv_data->drv_ctx.mutex);

    return ret;
}

/**
 * @brief Constant structure containing the SDH_SPI driver operations
 */
const wm_drv_sdh_spi_ops_t wm_drv_sdh_spi_ops = {
    .init             = wm_drv_ops_sdh_spi_init,
    .deinit           = wm_drv_ops_sdh_spi_deinit,
    .transceive_sync  = wm_drv_ops_sdh_spi_transceive_sync,
    .transceive_async = wm_drv_ops_sdh_spi_transceive_async,
};