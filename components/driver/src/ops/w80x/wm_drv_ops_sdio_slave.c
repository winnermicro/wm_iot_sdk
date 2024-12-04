/**
 * @file wm_drv_ops_sdio_slave.c
 *
 * @brief DRV_OPS_SDIO_SLAVE Module
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

#include "wm_drv_sdio_slave.h"
#include "wm_drv_rcc.h"
#include "wm_drv_gpio.h"
#include "wm_dt_hw.h"
#include "wm_osal.h"
#include "freertos/FreeRTOS.h"

/**
 * @brief Enable debug logs for SDIO_SLAVE driver
 */
#define WM_DRV_SDIO_SLAVE_DEBUG 0
#if WM_DRV_SDIO_SLAVE_DEBUG
#define LOG_TAG "DRV SDIO SLAVE"
#include "wm_log.h"
#define WM_DRV_SDIO_SLAVE_LOG_D(fmt, ...)    wm_log_debug("[%s:%d] " fmt, __FUNCTION__, __LINE__, ##__VA_ARGS__)
#define WM_DRV_SDIO_SLAVE_LOG_I(fmt, ...)    wm_log_info("[%s:%d] " fmt, __FUNCTION__, __LINE__, ##__VA_ARGS__)
#define WM_DRV_SDIO_SLAVE_LOG_W(fmt, ...)    wm_log_warn("[%s:%d] " fmt, __FUNCTION__, __LINE__, ##__VA_ARGS__)
#define WM_DRV_SDIO_SLAVE_LOG_E(fmt, ...)    wm_log_error("[%s:%d] " fmt, __FUNCTION__, __LINE__, ##__VA_ARGS__)
#define WM_DRV_SDIO_SLAVE_LOG_DUMP(fmt, ...) wm_log_dump("[%s:%d] " fmt, __FUNCTION__, __LINE__, ##__VA_ARGS__)
#define WM_DRV_SDIO_SLAVE_FUNC_EXIT(fn)      WM_DRV_SDIO_SLAVE_LOG_D("%s exit", fn)
#define WM_DRV_SDIO_SLAVE_FUNC_NAME          __func__
#else
#define WM_DRV_SDIO_SLAVE_LOG_D(...)
#define WM_DRV_SDIO_SLAVE_LOG_I(...)
#define WM_DRV_SDIO_SLAVE_LOG_W(...)
#define WM_DRV_SDIO_SLAVE_LOG_E(...)
#define WM_DRV_SDIO_SLAVE_LOG_DUMP(...)
#define WM_DRV_SDIO_SLAVE_FUNC_EXIT(fn)
#endif /**
 * @brief Lock the SDIO_SLAVE driver for thread-safe operations
 */
#define WM_DRV_SDIO_SLAVE_LOCK(lock)                                                     \
    do {                                                                                 \
        if (!lock) {                                                                     \
            if (WM_ERR_SUCCESS != wm_os_internal_mutex_create(&lock)) {                  \
                WM_DRV_SDIO_SLAVE_LOG_E("mutex create err!");                            \
                return WM_ERR_FAILED;                                                    \
            }                                                                            \
        }                                                                                \
        if (WM_ERR_SUCCESS != wm_os_internal_mutex_acquire(lock, WM_OS_WAIT_TIME_MAX)) { \
            WM_DRV_SDIO_SLAVE_LOG_E("mutex acquire err!");                               \
            return WM_ERR_FAILED;                                                        \
        }                                                                                \
    } while (0);

/**
 * @brief Unlock the SDIO_SLAVE driver after thread-safe operations
 */
#define WM_DRV_SDIO_SLAVE_UNLOCK(lock)      \
    do {                                    \
        if (!lock) {                        \
            return WM_ERR_NO_INITED;        \
        }                                   \
        wm_os_internal_mutex_release(lock); \
    } while (0);

/**
 * @brief Maximum size of SDIO slave transmit buffer in bytes
 */
#define WM_DRV_SDIO_SLAVE_TX_DATA_MAX_SIZE (8192) /**< Maximum size of SDIO slave transmit buffer in bytes */
#define WM_DRV_SDIO_SLAVE_RX_DATA_MAX_SIZE (4096) /**< Maximum size of SDIO slave receive buffer in bytes */
#define WM_DRV_SDIO_SLAVE_CMD_MAX_SIZE     (4096) /**< Maximum size of SDIO slave command buffer in bytes */
#define WM_DRV_SDIO_SLAVE_CIS_MAX_SIZE     (4096) /**< Maximum size of SDIO slave CIS buffer in bytes */

/**
 * @brief SDIO event flag definitions
 */
#define SDIO_EVENT_TX_DONE                 (1 << 0) /**< SDIO transmission complete event flag */
#define SDIO_EVENT_RX_DONE                 (1 << 1) /**< SDIO receive complete event flag */
#define SDIO_EVENT_RX_CMD_DONE             (1 << 2) /**< SDIO command receive complete event flag */

/**
 * @brief SDIO slave driver operations structure
 */
typedef struct {
    int (*init)(wm_device_t *dev);   /**< Initialize SDIO slave device */
    int (*deinit)(wm_device_t *dev); /**< Deinitialize SDIO slave device */
    int (*register_callback)(wm_device_t *dev, wm_drv_sdio_slave_callback_t cb,
                             void *priv);         /**< Register callback function for SDIO slave events */
    int (*unregister_callback)(wm_device_t *dev); /**< Unregister callback function for SDIO slave events */
    int (*read_cis_reg)(wm_device_t *dev, wm_drv_sdio_slave_func_t func, uint32_t pos,
                        uint32_t *reg_val); /**< Read register value */
    int (*write_cis_reg)(wm_device_t *dev, wm_drv_sdio_slave_func_t func, uint32_t pos,
                         uint32_t reg_val);                                            /**< Write register value */
    int (*tx_data_sync)(wm_device_t *dev, void *addr, uint32_t len, uint32_t timeout); /**< Send data synchronously */
    int (*tx_data_async)(wm_device_t *dev, void *addr, uint32_t len);                  /**< Send data asynchronously */
    int (*rx_data)(wm_device_t *dev, void **addr, uint32_t timeout); /**< Receive data from SDIO slave device */
    int (*rx_cmd)(wm_device_t *dev, void **addr, uint32_t timeout);  /**< Receive command from SDIO slave device */
} wm_drv_sdio_slave_ops_t;

typedef struct {
    wm_drv_sdio_slave_callback_t cb;
    void *priv;
} wm_drv_sdio_slave_cb_info_t;

/**
 * @brief Structure representing the device context for SDIO_SLAVE driver
 */
typedef struct {
    wm_os_mutex_t *mutex;                   /**< Mutex for thread-safe access */
    wm_hal_sdio_slave_buf_desc_t *buf_desc; /**< Buffer descriptor for SDIO slave */
    wm_drv_sdio_slave_cb_info_t cb_info;    /**< Callback information */
    wm_os_event_t *event;                   /**< Event group for TX/RX/CMD events */
    void *rx_buf_addr;                      /**< RX buffer address */
    void *rx_cmd_addr;                      /**< RX command buffer address */
} wm_drv_sdio_slave_dev_ctx_t;

/**
 * @brief SDIO_SLAVE driver data structure
 */
typedef struct {
    wm_hal_sdio_slave_dev_t hal_sdio_slave_dev; /**< HAL device structure */
    wm_drv_sdio_slave_dev_ctx_t drv_ctx;        /**< Driver context for the SDIO_SLAVE driver */
} wm_drv_sdio_slave_drv_t;

static int wm_drv_ops_sdio_slave_init(wm_device_t *dev);
static int wm_drv_ops_sdio_slave_deinit(wm_device_t *dev);
static int wm_drv_ops_sdio_slave_register_callback(wm_device_t *dev, wm_drv_sdio_slave_callback_t cb, void *priv);
static int wm_drv_ops_sdio_slave_unregister_callback(wm_device_t *dev);
static int wm_drv_ops_sdio_slave_read_cis_reg(wm_device_t *dev, wm_drv_sdio_slave_func_t func, uint32_t pos, uint32_t *reg_val);
static int wm_drv_ops_sdio_slave_write_cis_reg(wm_device_t *dev, wm_drv_sdio_slave_func_t func, uint32_t pos, uint32_t reg_val);
static int wm_drv_ops_sdio_slave_tx_data_sync(wm_device_t *dev, void *addr, uint32_t len, uint32_t timeout);
static int wm_drv_ops_sdio_slave_tx_data_async(wm_device_t *dev, void *addr, uint32_t len);
static int wm_drv_ops_sdio_slave_rx_data(wm_device_t *dev, void **addr, uint32_t timeout);
static int wm_drv_ops_sdio_slave_rx_cmd(wm_device_t *dev, void **addr, uint32_t timeout);

static void wm_drv_sdio_slave_callback(void *hal_dev, wm_hal_sdio_slave_event_t event, void *addr, void *priv)
{
    wm_device_t *dev = (wm_device_t *)priv;
    if (dev == NULL) {
        WM_DRV_SDIO_SLAVE_LOG_E("Invalid device pointer");
        return;
    }
    wm_drv_sdio_slave_drv_t *drv_data = (wm_drv_sdio_slave_drv_t *)dev->drv;
    if (drv_data == NULL) {
        WM_DRV_SDIO_SLAVE_LOG_E("Invalid driver data pointer");
        return;
    }

    switch (event) {
        case WM_HAL_SDIO_SLAVE_EVENT_TX:
            WM_DRV_SDIO_SLAVE_LOG_D("TX event received");
            wm_os_internal_event_put(drv_data->drv_ctx.event, SDIO_EVENT_TX_DONE);
            if (drv_data->drv_ctx.cb_info.cb) {
                drv_data->drv_ctx.cb_info.cb(dev, WM_DRV_SDIO_SLAVE_EVENT_TX, (void *)addr, drv_data->drv_ctx.cb_info.priv);
            }
            break;
        case WM_HAL_SDIO_SLAVE_EVENT_RX:
            WM_DRV_SDIO_SLAVE_LOG_D("RX event received");
            wm_os_internal_event_put(drv_data->drv_ctx.event, SDIO_EVENT_RX_DONE);
            drv_data->drv_ctx.rx_buf_addr = addr;
            if (drv_data->drv_ctx.cb_info.cb) {
                drv_data->drv_ctx.cb_info.cb(dev, WM_DRV_SDIO_SLAVE_EVENT_RX, (void *)addr, drv_data->drv_ctx.cb_info.priv);
            }
            break;
        case WM_HAL_SDIO_SLAVE_EVENT_TX_CMD:
            WM_DRV_SDIO_SLAVE_LOG_D("UP RX CMD event received");
            if (drv_data->drv_ctx.cb_info.cb) {
                drv_data->drv_ctx.cb_info.cb(dev, WM_DRV_SDIO_SLAVE_EVENT_TX_CMD, (void *)addr, drv_data->drv_ctx.cb_info.priv);
            }
            break;
        case WM_HAL_SDIO_SLAVE_EVENT_RX_CMD:
            WM_DRV_SDIO_SLAVE_LOG_D("DOWN TX CMD event received");
            wm_os_internal_event_put(drv_data->drv_ctx.event, SDIO_EVENT_RX_CMD_DONE);
            drv_data->drv_ctx.rx_cmd_addr = addr;
            if (drv_data->drv_ctx.cb_info.cb) {
                drv_data->drv_ctx.cb_info.cb(dev, WM_DRV_SDIO_SLAVE_EVENT_RX_CMD, (void *)addr, drv_data->drv_ctx.cb_info.priv);
            }
            break;
        default:
            WM_DRV_SDIO_SLAVE_LOG_W("Unknown event received: %d", event);
            break;
    }
}

static int wm_drv_sdio_slave_pin_cfg(wm_device_t *dev)
{
    int ret                              = WM_ERR_SUCCESS;
    wm_dt_hw_sdio_slave_t *hw_sdio_slave = NULL;

    if (dev == NULL) {
        WM_DRV_SDIO_SLAVE_LOG_E("Invalid device pointer");
        return WM_ERR_INVALID_PARAM;
    }

    hw_sdio_slave = (wm_dt_hw_sdio_slave_t *)dev->hw;
    if (hw_sdio_slave == NULL) {
        WM_DRV_SDIO_SLAVE_LOG_E("Invalid hardware configuration pointer");
        return WM_ERR_INVALID_PARAM;
    }

    for (uint8_t i = 0; i < hw_sdio_slave->pin_cfg_count; i++) {
        ret = wm_drv_gpio_iomux_func_sel(hw_sdio_slave->pin_cfg[i].pin_num, hw_sdio_slave->pin_cfg[i].pin_mux);
        if (ret != WM_ERR_SUCCESS) {
            WM_DRV_SDIO_SLAVE_LOG_E("Failed to configure pin %d", hw_sdio_slave->pin_cfg[i].pin_num);
            return ret;
        }
    }

    WM_DRV_SDIO_SLAVE_LOG_I("Pin configuration completed successfully");
    return ret;
}

static int wm_drv_sdio_slave_buf_desc_init(wm_hal_sdio_slave_buf_desc_t **buf_desc)
{
    int ret = WM_ERR_SUCCESS;

    if (buf_desc == NULL) {
        WM_DRV_SDIO_SLAVE_LOG_E("Invalid buffer descriptor pointer");
        return WM_ERR_INVALID_PARAM;
    }

    if (WM_DRV_SDIO_SLAVE_TX_BUF_SIZE * WM_DRV_SDIO_SLAVE_TX_BD_NUM > WM_DRV_SDIO_SLAVE_TX_DATA_MAX_SIZE ||
        WM_DRV_SDIO_SLAVE_RX_BUF_SIZE * WM_DRV_SDIO_SLAVE_RX_BD_NUM > WM_DRV_SDIO_SLAVE_RX_DATA_MAX_SIZE ||
        WM_DRV_SDIO_SLAVE_CIS_SIZE > WM_DRV_SDIO_SLAVE_CIS_MAX_SIZE ||
        WM_DRV_SDIO_SLAVE_CMD_RX_BUF_SIZE > WM_DRV_SDIO_SLAVE_CMD_MAX_SIZE) {
        WM_DRV_SDIO_SLAVE_LOG_E("Buffer size exceeds maximum limit");
        return WM_ERR_INVALID_PARAM;
    }

    *buf_desc = (wm_hal_sdio_slave_buf_desc_t *)wm_heap_caps_alloc(sizeof(wm_hal_sdio_slave_buf_desc_t), WM_HEAP_CAP_SHARED);
    if (*buf_desc == NULL) {
        WM_DRV_SDIO_SLAVE_LOG_E("Failed to allocate buffer descriptor");
        return WM_ERR_NO_MEM;
    }
    memset(*buf_desc, 0, sizeof(wm_hal_sdio_slave_buf_desc_t));

    (*buf_desc)->f0_cis_addr = (void *)wm_heap_caps_alloc(WM_DRV_SDIO_SLAVE_CIS_SIZE, WM_HEAP_CAP_SHARED);
    if ((*buf_desc)->f0_cis_addr == NULL) {
        WM_DRV_SDIO_SLAVE_LOG_E("Failed to allocate F0 CIS buffer");
        goto err_f0_cis;
    }
    (*buf_desc)->f0_cis_size = WM_DRV_SDIO_SLAVE_CIS_SIZE;
    memset((*buf_desc)->f0_cis_addr, 0, WM_DRV_SDIO_SLAVE_CIS_SIZE);

    (*buf_desc)->f1_cis_addr = (void *)wm_heap_caps_alloc(WM_DRV_SDIO_SLAVE_CIS_SIZE, WM_HEAP_CAP_SHARED);
    if ((*buf_desc)->f1_cis_addr == NULL) {
        WM_DRV_SDIO_SLAVE_LOG_E("Failed to allocate F1 CIS buffer");
        goto err_f1_cis;
    }
    (*buf_desc)->f1_cis_size = WM_DRV_SDIO_SLAVE_CIS_SIZE;
    memset((*buf_desc)->f1_cis_addr, 0, WM_DRV_SDIO_SLAVE_CIS_SIZE);

    (*buf_desc)->cmd_rxbuf_addr = (void *)wm_heap_caps_alloc(WM_DRV_SDIO_SLAVE_CMD_RX_BUF_SIZE, WM_HEAP_CAP_SHARED);
    if ((*buf_desc)->cmd_rxbuf_addr == NULL) {
        WM_DRV_SDIO_SLAVE_LOG_E("Failed to allocate command receive buffer");
        goto err_cmd_rxbuf;
    }
    (*buf_desc)->cmd_rxbuf_size = WM_DRV_SDIO_SLAVE_CMD_RX_BUF_SIZE;
    memset((*buf_desc)->cmd_rxbuf_addr, 0, WM_DRV_SDIO_SLAVE_CMD_RX_BUF_SIZE);

    /* Create TX buffer descriptor chain */
    (*buf_desc)->txbd = (wm_hal_sdio_wrapper_txbd_t *)wm_heap_caps_alloc(
        WM_DRV_SDIO_SLAVE_TX_BD_NUM * sizeof(wm_hal_sdio_wrapper_txbd_t), WM_HEAP_CAP_SHARED);
    if ((*buf_desc)->txbd == NULL) {
        WM_DRV_SDIO_SLAVE_LOG_E("Failed to allocate TX buffer descriptors");
        goto err_txbd;
    }
    memset((*buf_desc)->txbd, 0, WM_DRV_SDIO_SLAVE_TX_BD_NUM * sizeof(wm_hal_sdio_wrapper_txbd_t));

    /* Link TX buffer descriptors in a circular chain */
    for (int i = 0; i < WM_DRV_SDIO_SLAVE_TX_BD_NUM; i++) {
        if (i == WM_DRV_SDIO_SLAVE_TX_BD_NUM - 1) {
            /* Last BD points back to first BD */
            (*buf_desc)->txbd[i].next = (wm_hal_sdio_wrapper_txbd_t *)&(*buf_desc)->txbd[0];
        } else {
            /* Link to next BD */
            (*buf_desc)->txbd[i].next = (wm_hal_sdio_wrapper_txbd_t *)&(*buf_desc)->txbd[i + 1];
        }
    }
    (*buf_desc)->tx_buf_len  = WM_DRV_SDIO_SLAVE_TX_BUF_SIZE;
    (*buf_desc)->tx_link_num = WM_DRV_SDIO_SLAVE_TX_BD_NUM;

    /* Create RX buffer descriptor chain */
    (*buf_desc)->rxbd = (wm_hal_sdio_wrapper_rxbd_t *)wm_heap_caps_alloc(
        WM_DRV_SDIO_SLAVE_RX_BD_NUM * sizeof(wm_hal_sdio_wrapper_rxbd_t), WM_HEAP_CAP_SHARED);
    if ((*buf_desc)->rxbd == NULL) {
        WM_DRV_SDIO_SLAVE_LOG_E("Failed to allocate RX buffer descriptors");
        goto err_rxbd;
    }
    memset((*buf_desc)->rxbd, 0, WM_DRV_SDIO_SLAVE_RX_BD_NUM * sizeof(wm_hal_sdio_wrapper_rxbd_t));

    /* Link RX buffer descriptors in a circular chain */
    void *rxbuf = wm_heap_caps_alloc(WM_DRV_SDIO_SLAVE_RX_BUF_SIZE * WM_DRV_SDIO_SLAVE_RX_BD_NUM, WM_HEAP_CAP_SHARED);
    if (rxbuf == NULL) {
        WM_DRV_SDIO_SLAVE_LOG_E("Failed to allocate RX buffers");
        goto err_rxbuf;
    }
    memset(rxbuf, 0, WM_DRV_SDIO_SLAVE_RX_BUF_SIZE * WM_DRV_SDIO_SLAVE_RX_BD_NUM);

    for (int i = 0; i < WM_DRV_SDIO_SLAVE_RX_BD_NUM; i++) {
        (*buf_desc)->rxbd[i].valid      = 1;
        (*buf_desc)->rxbd[i].rxbuf_addr = (uint32_t)((uint8_t *)rxbuf + i * WM_DRV_SDIO_SLAVE_RX_BUF_SIZE);

        if (i == WM_DRV_SDIO_SLAVE_RX_BD_NUM - 1) {
            /* Last BD points back to first BD */
            (*buf_desc)->rxbd[i].next = (wm_hal_sdio_wrapper_rxbd_t *)&(*buf_desc)->rxbd[0];
        } else {
            /* Link to next BD */
            (*buf_desc)->rxbd[i].next = (wm_hal_sdio_wrapper_rxbd_t *)&(*buf_desc)->rxbd[i + 1];
        }
    }
    (*buf_desc)->rx_buf_len  = WM_DRV_SDIO_SLAVE_RX_BUF_SIZE;
    (*buf_desc)->rx_link_num = WM_DRV_SDIO_SLAVE_RX_BD_NUM;

    WM_DRV_SDIO_SLAVE_LOG_I("Buffer descriptor initialization completed successfully");
    return ret;

err_rxbuf:
    if ((*buf_desc)->rxbd[0].rxbuf_addr != 0) {
        wm_heap_caps_free((void *)((*buf_desc)->rxbd[0].rxbuf_addr));
    }
    if ((*buf_desc)->rxbd != NULL) {
        wm_heap_caps_free((*buf_desc)->rxbd);
        (*buf_desc)->rxbd = NULL;
    }
err_rxbd:
    if ((*buf_desc)->txbd != NULL) {
        wm_heap_caps_free((*buf_desc)->txbd);
        (*buf_desc)->txbd = NULL;
    }
err_txbd:
    if ((*buf_desc)->cmd_rxbuf_addr != NULL) {
        wm_heap_caps_free((*buf_desc)->cmd_rxbuf_addr);
        (*buf_desc)->cmd_rxbuf_addr = NULL;
    }
err_cmd_rxbuf:
    if ((*buf_desc)->f1_cis_addr != NULL) {
        wm_heap_caps_free((*buf_desc)->f1_cis_addr);
        (*buf_desc)->f1_cis_addr = NULL;
    }
err_f1_cis:
    if ((*buf_desc)->f0_cis_addr != NULL) {
        wm_heap_caps_free((*buf_desc)->f0_cis_addr);
        (*buf_desc)->f0_cis_addr = NULL;
    }
err_f0_cis:
    if (*buf_desc != NULL) {
        wm_heap_caps_free(*buf_desc);
        *buf_desc = NULL;
    }

    WM_DRV_SDIO_SLAVE_LOG_E("Buffer descriptor initialization failed");
    return WM_ERR_NO_MEM;
}

static int wm_drv_sdio_slave_buf_desc_deinit(wm_hal_sdio_slave_buf_desc_t **buf_desc)
{
    if (buf_desc == NULL || *buf_desc == NULL) {
        WM_DRV_SDIO_SLAVE_LOG_E("Invalid buffer descriptor pointer");
        return WM_ERR_INVALID_PARAM;
    }

    /* Free RX buffer descriptors and buffers */
    if ((*buf_desc)->rxbd != NULL) {
        if ((*buf_desc)->rxbd[0].rxbuf_addr != 0) {
            wm_heap_caps_free((void *)((*buf_desc)->rxbd[0].rxbuf_addr));
        }
        wm_heap_caps_free((*buf_desc)->rxbd);
        (*buf_desc)->rxbd = NULL;
    }

    /* Free TX buffer descriptors */
    if ((*buf_desc)->txbd != NULL) {
        wm_heap_caps_free((*buf_desc)->txbd);
        (*buf_desc)->txbd = NULL;
    }

    /* Free command receive buffer */
    if ((*buf_desc)->cmd_rxbuf_addr != NULL) {
        wm_heap_caps_free((*buf_desc)->cmd_rxbuf_addr);
        (*buf_desc)->cmd_rxbuf_addr = NULL;
    }

    /* Free CIS buffers */
    if ((*buf_desc)->f1_cis_addr != NULL) {
        wm_heap_caps_free((*buf_desc)->f1_cis_addr);
        (*buf_desc)->f1_cis_addr = NULL;
    }

    if ((*buf_desc)->f0_cis_addr != NULL) {
        wm_heap_caps_free((*buf_desc)->f0_cis_addr);
        (*buf_desc)->f0_cis_addr = NULL;
    }

    /* Free buffer descriptor structure */
    wm_heap_caps_free(*buf_desc);
    *buf_desc = NULL;

    WM_DRV_SDIO_SLAVE_LOG_I("Buffer descriptor deinitialization completed successfully");
    return WM_ERR_SUCCESS;
}

static int wm_drv_ops_sdio_slave_init(wm_device_t *dev)
{
    int ret                                = WM_ERR_SUCCESS;
    wm_drv_sdio_slave_drv_t *drv_data      = NULL;
    wm_hal_sdio_slave_buf_desc_t *buf_desc = NULL;
    wm_dt_hw_sdio_slave_t *hw_sdio_slave   = NULL;

    if (dev == NULL) {
        WM_DRV_SDIO_SLAVE_LOG_E("Invalid device pointer");
        return WM_ERR_INVALID_PARAM;
    }

    drv_data = (wm_drv_sdio_slave_drv_t *)wm_os_internal_malloc(sizeof(wm_drv_sdio_slave_drv_t));
    if (!drv_data) {
        return WM_ERR_NO_MEM;
    }
    memset(drv_data, 0x00, sizeof(wm_drv_sdio_slave_drv_t));

    WM_DRV_SDIO_SLAVE_LOCK(drv_data->drv_ctx.mutex);

    hw_sdio_slave = (wm_dt_hw_sdio_slave_t *)dev->hw;
    if (hw_sdio_slave == NULL) {
        WM_DRV_SDIO_SLAVE_LOG_E("Invalid hardware configuration pointer");
        ret = WM_ERR_INVALID_PARAM;
        goto exit;
    }

    wm_device_t *rcc_dev = wm_dt_get_device_by_name(hw_sdio_slave->rcc_device_name);
    if (rcc_dev == NULL) {
        WM_DRV_SDIO_SLAVE_LOG_E("Failed to get RCC device");
        ret = WM_ERR_INVALID_PARAM;
        goto exit;
    }

    drv_data->hal_sdio_slave_dev.sdio_slave_reg_base     = (wm_sdio_slave_reg_t *)hw_sdio_slave->sdio_slave_reg_base;
    drv_data->hal_sdio_slave_dev.wrapper_reg_base        = (wm_sdio_wrapper_reg_t *)hw_sdio_slave->wrapper_reg_base;
    drv_data->hal_sdio_slave_dev.irq_num                 = hw_sdio_slave->irq_cfg.irq_num;
    drv_data->hal_sdio_slave_dev.irq_priority            = hw_sdio_slave->irq_cfg.irq_priority;
    drv_data->hal_sdio_slave_dev.sysclk                  = wm_drv_rcc_get_config_clock(rcc_dev, WM_RCC_TYPE_CPU);
    drv_data->hal_sdio_slave_dev.sdio_slave_cfg.ccr_rev  = WM_HAL_SDIO_SLAVE_CCCR_REV_1_20;
    drv_data->hal_sdio_slave_dev.sdio_slave_cfg.sdio_rev = WM_HAL_SDIO_SLAVE_SDIO_REV_2_00;
    drv_data->hal_sdio_slave_dev.sdio_slave_cfg.sd_rev   = WM_HAL_SDIO_SLAVE_SD_REV_2_00;
    dev->drv                                             = drv_data;

    ret = wm_drv_sdio_slave_pin_cfg(dev);
    if (ret != WM_ERR_SUCCESS) {
        WM_DRV_SDIO_SLAVE_LOG_E("Pin configuration failed");
        goto exit;
    }

    if (drv_data->drv_ctx.buf_desc == NULL) {
        ret = wm_drv_sdio_slave_buf_desc_init(&buf_desc);
        if (ret != WM_ERR_SUCCESS) {
            WM_DRV_SDIO_SLAVE_LOG_E("Buffer descriptor initialization failed");
            goto exit;
        }
        drv_data->drv_ctx.buf_desc = buf_desc;
    }

    ret = wm_hal_sdio_slave_register_buf(&drv_data->hal_sdio_slave_dev, drv_data->drv_ctx.buf_desc);
    if (ret != WM_ERR_SUCCESS) {
        WM_DRV_SDIO_SLAVE_LOG_E("Failed to register buffer");
        goto exit;
    }

    ret = wm_hal_sdio_slave_register_callback(&drv_data->hal_sdio_slave_dev,
                                              (wm_hal_sdio_slave_callback_t)wm_drv_sdio_slave_callback, dev);
    if (ret != WM_ERR_SUCCESS) {
        WM_DRV_SDIO_SLAVE_LOG_E("Failed to register callback");
        goto exit;
    }

    ret = wm_hal_sdio_slave_init(&drv_data->hal_sdio_slave_dev);
    if (ret != WM_ERR_SUCCESS) {
        WM_DRV_SDIO_SLAVE_LOG_E("HAL initialization failed");
        goto exit;
    }

    if (wm_os_internal_event_create(&drv_data->drv_ctx.event) != WM_OS_STATUS_SUCCESS) {
        WM_DRV_SDIO_SLAVE_LOG_E("Failed to create event group");
        ret = WM_ERR_FAILED;
        goto exit;
    }

    dev->state = WM_DEV_ST_INITED;

exit:
    WM_DRV_SDIO_SLAVE_UNLOCK(drv_data->drv_ctx.mutex);
    if (ret != WM_ERR_SUCCESS && dev->drv != NULL) {
        wm_os_internal_free(dev->drv);
        dev->drv = NULL;
        WM_DRV_SDIO_SLAVE_LOG_E("Driver initialization failed");
    } else {
        WM_DRV_SDIO_SLAVE_LOG_I("Driver initialization completed successfully");
    }

    return ret;
}

static int wm_drv_ops_sdio_slave_deinit(wm_device_t *dev)
{
    int ret                              = WM_ERR_SUCCESS;
    wm_drv_sdio_slave_drv_t *drv_data    = NULL;
    wm_dt_hw_sdio_slave_t *hw_sdio_slave = NULL;

    if (dev == NULL) {
        WM_DRV_SDIO_SLAVE_LOG_E("Invalid device pointer");
        return WM_ERR_INVALID_PARAM;
    }

    drv_data = (wm_drv_sdio_slave_drv_t *)dev->drv;
    if (drv_data == NULL) {
        WM_DRV_SDIO_SLAVE_LOG_E("Invalid driver data pointer");
        return WM_ERR_INVALID_PARAM;
    }

    hw_sdio_slave = (wm_dt_hw_sdio_slave_t *)dev->hw;
    if (hw_sdio_slave == NULL) {
        WM_DRV_SDIO_SLAVE_LOG_E("Invalid hardware configuration pointer");
        return WM_ERR_INVALID_PARAM;
    }

    WM_DRV_SDIO_SLAVE_LOCK(drv_data->drv_ctx.mutex);

    if (drv_data->drv_ctx.event) {
        if (wm_os_internal_event_delete(drv_data->drv_ctx.event) != WM_OS_STATUS_SUCCESS) {
            WM_DRV_SDIO_SLAVE_LOG_E("Failed to delete event group");
            ret = WM_ERR_FAILED;
            goto exit;
        }
        drv_data->drv_ctx.event = NULL;
    }

    ret = wm_hal_sdio_slave_deinit(&drv_data->hal_sdio_slave_dev);
    if (ret != WM_ERR_SUCCESS) {
        WM_DRV_SDIO_SLAVE_LOG_E("HAL deinitialization failed");
        goto exit;
    }

    ret = wm_hal_sdio_slave_unregister_callback(&drv_data->hal_sdio_slave_dev);
    if (ret != WM_ERR_SUCCESS) {
        WM_DRV_SDIO_SLAVE_LOG_E("Failed to unregister callback");
        goto exit;
    }

    ret = wm_hal_sdio_slave_unregister_buf(&drv_data->hal_sdio_slave_dev);
    if (ret != WM_ERR_SUCCESS) {
        WM_DRV_SDIO_SLAVE_LOG_E("Failed to unregister buffer");
        goto exit;
    }

    ret = wm_drv_sdio_slave_buf_desc_deinit(&drv_data->drv_ctx.buf_desc);
    if (ret != WM_ERR_SUCCESS) {
        WM_DRV_SDIO_SLAVE_LOG_E("Buffer descriptor deinitialization failed");
        goto exit;
    }
    drv_data->drv_ctx.buf_desc = NULL;

    for (uint8_t i = 0; i < hw_sdio_slave->pin_cfg_count; i++) {
        ret = wm_drv_gpio_iomux_func_sel(hw_sdio_slave->pin_cfg[i].pin_num, WM_GPIO_IOMUX_FUN5);
        if (ret != WM_ERR_SUCCESS) {
            WM_DRV_SDIO_SLAVE_LOG_E("Failed to reset pin %d", hw_sdio_slave->pin_cfg[i].pin_num);
            goto exit;
        }
    }

    dev->state = WM_DEV_ST_UNINIT;

exit:
    WM_DRV_SDIO_SLAVE_UNLOCK(drv_data->drv_ctx.mutex);

    if (ret == WM_ERR_SUCCESS) {
        WM_DRV_SDIO_SLAVE_LOG_I("Driver deinitialization completed successfully");
        wm_os_internal_free(dev->drv);
        dev->drv = NULL;
    } else {
        WM_DRV_SDIO_SLAVE_LOG_E("Driver deinitialization failed");
    }

    return ret;
}

static int wm_drv_ops_sdio_slave_register_callback(wm_device_t *dev, wm_drv_sdio_slave_callback_t cb, void *priv)
{
    int ret                           = WM_ERR_SUCCESS;
    wm_drv_sdio_slave_drv_t *drv_data = NULL;

    WM_DRV_SDIO_SLAVE_LOG_D("Registering callback");

    if (dev == NULL) {
        WM_DRV_SDIO_SLAVE_LOG_E("Invalid device pointer");
        return WM_ERR_INVALID_PARAM;
    }

    drv_data = (wm_drv_sdio_slave_drv_t *)dev->drv;
    if (drv_data == NULL) {
        WM_DRV_SDIO_SLAVE_LOG_E("Invalid driver data pointer");
        return WM_ERR_INVALID_PARAM;
    }

    drv_data->drv_ctx.cb_info.cb   = cb;
    drv_data->drv_ctx.cb_info.priv = priv;

    WM_DRV_SDIO_SLAVE_LOG_D("Callback registered successfully");
    return ret;
}

static int wm_drv_ops_sdio_slave_unregister_callback(wm_device_t *dev)
{
    int ret                           = WM_ERR_SUCCESS;
    wm_drv_sdio_slave_drv_t *drv_data = NULL;

    WM_DRV_SDIO_SLAVE_LOG_D("Unregistering callback");

    if (dev == NULL) {
        WM_DRV_SDIO_SLAVE_LOG_E("Invalid device pointer");
        return WM_ERR_INVALID_PARAM;
    }

    drv_data = (wm_drv_sdio_slave_drv_t *)dev->drv;
    if (drv_data == NULL) {
        WM_DRV_SDIO_SLAVE_LOG_E("Invalid driver data pointer");
        return WM_ERR_INVALID_PARAM;
    }

    drv_data->drv_ctx.cb_info.cb   = NULL;
    drv_data->drv_ctx.cb_info.priv = NULL;

    WM_DRV_SDIO_SLAVE_LOG_D("Callback unregistered successfully");
    return ret;
}

static int wm_drv_ops_sdio_slave_read_cis_reg(wm_device_t *dev, wm_drv_sdio_slave_func_t func, uint32_t pos, uint32_t *reg_val)
{
    int ret                           = WM_ERR_SUCCESS;
    wm_drv_sdio_slave_drv_t *drv_data = NULL;

    WM_DRV_SDIO_SLAVE_LOG_D("Reading register");

    if (dev == NULL || reg_val == NULL) {
        WM_DRV_SDIO_SLAVE_LOG_E("Invalid parameters");
        return WM_ERR_INVALID_PARAM;
    }

    drv_data = (wm_drv_sdio_slave_drv_t *)dev->drv;
    if (drv_data == NULL) {
        WM_DRV_SDIO_SLAVE_LOG_E("Invalid driver data pointer");
        return WM_ERR_INVALID_PARAM;
    }

    WM_DRV_SDIO_SLAVE_LOCK(drv_data->drv_ctx.mutex);
    ret = wm_hal_sdio_slave_read_cis_reg(&drv_data->hal_sdio_slave_dev, func, pos, reg_val);
    WM_DRV_SDIO_SLAVE_UNLOCK(drv_data->drv_ctx.mutex);

    if (ret != WM_ERR_SUCCESS) {
        WM_DRV_SDIO_SLAVE_LOG_E("Failed to read register");
        return ret;
    }

    return ret;
}

static int wm_drv_ops_sdio_slave_write_cis_reg(wm_device_t *dev, wm_drv_sdio_slave_func_t func, uint32_t pos, uint32_t reg_val)
{
    int ret                           = WM_ERR_SUCCESS;
    wm_drv_sdio_slave_drv_t *drv_data = NULL;

    WM_DRV_SDIO_SLAVE_LOG_D("Writing register");

    if (dev == NULL) {
        WM_DRV_SDIO_SLAVE_LOG_E("Invalid parameters");
        return WM_ERR_INVALID_PARAM;
    }

    drv_data = (wm_drv_sdio_slave_drv_t *)dev->drv;
    if (drv_data == NULL) {
        WM_DRV_SDIO_SLAVE_LOG_E("Invalid driver data pointer");
        return WM_ERR_INVALID_PARAM;
    }

    WM_DRV_SDIO_SLAVE_LOCK(drv_data->drv_ctx.mutex);
    ret = wm_hal_sdio_slave_write_cis_reg(&drv_data->hal_sdio_slave_dev, func, pos, reg_val);
    WM_DRV_SDIO_SLAVE_UNLOCK(drv_data->drv_ctx.mutex);

    if (ret != WM_ERR_SUCCESS) {
        WM_DRV_SDIO_SLAVE_LOG_E("Failed to write register");
        return ret;
    }

    return ret;
}

static int wm_drv_ops_sdio_slave_tx_data_sync(wm_device_t *dev, void *addr, uint32_t len, uint32_t timeout)
{
    int ret                           = WM_ERR_SUCCESS;
    wm_drv_sdio_slave_drv_t *drv_data = NULL;
    uint32_t event_bits;

    WM_DRV_SDIO_SLAVE_LOG_D("Sending data synchronously, len: %u", len);

    if (dev == NULL) {
        WM_DRV_SDIO_SLAVE_LOG_E("Invalid device pointer");
        return WM_ERR_INVALID_PARAM;
    }

    drv_data = (wm_drv_sdio_slave_drv_t *)dev->drv;
    if (drv_data == NULL) {
        WM_DRV_SDIO_SLAVE_LOG_E("Invalid driver data pointer");
        return WM_ERR_INVALID_PARAM;
    }

    WM_DRV_SDIO_SLAVE_LOCK(drv_data->drv_ctx.mutex);
    ret = wm_hal_sdio_slave_tx_dma(&drv_data->hal_sdio_slave_dev, addr, len);
    if (ret != WM_ERR_SUCCESS) {
        WM_DRV_SDIO_SLAVE_LOG_E("TX DMA failed");
        goto exit;
    }

    wm_os_internal_event_get(drv_data->drv_ctx.event, SDIO_EVENT_TX_DONE, &event_bits, true, pdMS_TO_TICKS(timeout));
    if (!(event_bits & SDIO_EVENT_TX_DONE)) {
        WM_DRV_SDIO_SLAVE_LOG_E("TX timeout");
        ret = WM_ERR_TIMEOUT;
        goto exit;
    }
    wm_os_internal_event_clear_bits(drv_data->drv_ctx.event, SDIO_EVENT_TX_DONE);

exit:
    WM_DRV_SDIO_SLAVE_UNLOCK(drv_data->drv_ctx.mutex);

    return ret;
}

static int wm_drv_ops_sdio_slave_tx_data_async(wm_device_t *dev, void *addr, uint32_t len)
{
    int ret                           = WM_ERR_SUCCESS;
    wm_drv_sdio_slave_drv_t *drv_data = NULL;

    WM_DRV_SDIO_SLAVE_LOG_D("Sending data asynchronously, len: %u", len);

    if (dev == NULL || addr == NULL) {
        WM_DRV_SDIO_SLAVE_LOG_E("Invalid parameters");
        return WM_ERR_INVALID_PARAM;
    }

    drv_data = (wm_drv_sdio_slave_drv_t *)dev->drv;
    if (drv_data == NULL) {
        WM_DRV_SDIO_SLAVE_LOG_E("Invalid driver data pointer");
        return WM_ERR_INVALID_PARAM;
    }

    WM_DRV_SDIO_SLAVE_LOCK(drv_data->drv_ctx.mutex);
    ret = wm_hal_sdio_slave_tx_dma(&drv_data->hal_sdio_slave_dev, addr, len);
    if (ret != WM_ERR_SUCCESS) {
        WM_DRV_SDIO_SLAVE_LOG_E("TX DMA failed");
        goto exit;
    }

exit:
    WM_DRV_SDIO_SLAVE_UNLOCK(drv_data->drv_ctx.mutex);

    if (ret == WM_ERR_SUCCESS) {
        WM_DRV_SDIO_SLAVE_LOG_D("Data sent successfully");
    }
    return ret;
}

static int wm_drv_ops_sdio_slave_rx_data(wm_device_t *dev, void **addr, uint32_t timeout)
{
    int ret                           = WM_ERR_SUCCESS;
    wm_drv_sdio_slave_drv_t *drv_data = NULL;
    uint32_t event_bits;

    WM_DRV_SDIO_SLAVE_LOG_D("Receiving data");

    if (dev == NULL || addr == NULL) {
        WM_DRV_SDIO_SLAVE_LOG_E("Invalid parameters");
        return WM_ERR_INVALID_PARAM;
    }

    drv_data = (wm_drv_sdio_slave_drv_t *)dev->drv;
    if (drv_data == NULL) {
        WM_DRV_SDIO_SLAVE_LOG_E("Invalid driver data pointer");
        return WM_ERR_INVALID_PARAM;
    }

    wm_os_internal_event_get(drv_data->drv_ctx.event, SDIO_EVENT_RX_DONE, &event_bits, true, pdMS_TO_TICKS(timeout));
    if (!(event_bits & SDIO_EVENT_RX_DONE)) {
        WM_DRV_SDIO_SLAVE_LOG_E("RX timeout");
        return WM_ERR_TIMEOUT;
    }
    wm_os_internal_event_clear_bits(drv_data->drv_ctx.event, SDIO_EVENT_RX_DONE);

    *addr = drv_data->drv_ctx.rx_buf_addr;

    WM_DRV_SDIO_SLAVE_LOG_D("Data received successfully");
    return ret;
}

static int wm_drv_ops_sdio_slave_rx_cmd(wm_device_t *dev, void **addr, uint32_t timeout)
{
    int ret                           = WM_ERR_SUCCESS;
    wm_drv_sdio_slave_drv_t *drv_data = NULL;
    uint32_t event_bits;

    WM_DRV_SDIO_SLAVE_LOG_D("Receiving data");

    if (dev == NULL || addr == NULL) {
        WM_DRV_SDIO_SLAVE_LOG_E("Invalid parameters");
        return WM_ERR_INVALID_PARAM;
    }

    drv_data = (wm_drv_sdio_slave_drv_t *)dev->drv;
    if (drv_data == NULL) {
        WM_DRV_SDIO_SLAVE_LOG_E("Invalid driver data pointer");
        return WM_ERR_INVALID_PARAM;
    }

    wm_os_internal_event_get(drv_data->drv_ctx.event, SDIO_EVENT_RX_CMD_DONE, &event_bits, true, pdMS_TO_TICKS(timeout));
    if (!(event_bits & SDIO_EVENT_RX_CMD_DONE)) {
        WM_DRV_SDIO_SLAVE_LOG_E("RX CMD timeout");
        return WM_ERR_TIMEOUT;
    }
    wm_os_internal_event_clear_bits(drv_data->drv_ctx.event, SDIO_EVENT_RX_CMD_DONE);

    *addr = drv_data->drv_ctx.rx_cmd_addr;

    WM_DRV_SDIO_SLAVE_LOG_D("Data received successfully");
    return ret;
}

/**
 * @brief Constant definition for the SDH_SDIO_SLAVE driver operations
 */
const wm_drv_sdio_slave_ops_t wm_drv_sdio_slave_ops = {
    .init                = wm_drv_ops_sdio_slave_init,
    .deinit              = wm_drv_ops_sdio_slave_deinit,
    .register_callback   = wm_drv_ops_sdio_slave_register_callback,
    .unregister_callback = wm_drv_ops_sdio_slave_unregister_callback,
    .read_cis_reg        = wm_drv_ops_sdio_slave_read_cis_reg,
    .write_cis_reg       = wm_drv_ops_sdio_slave_write_cis_reg,
    .tx_data_sync        = wm_drv_ops_sdio_slave_tx_data_sync,
    .tx_data_async       = wm_drv_ops_sdio_slave_tx_data_async,
    .rx_data             = wm_drv_ops_sdio_slave_rx_data,
    .rx_cmd              = wm_drv_ops_sdio_slave_rx_cmd,
};
