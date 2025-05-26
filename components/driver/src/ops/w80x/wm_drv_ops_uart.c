/**
 * @file wm_drv_ops_uart.c
 *
 * @brief uart Module
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
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "wm_irq.h"
#include "wm_drv_rcc.h"
#include "wm_drv_gpio.h"
#include "wm_drv_irq.h"
#include "wm_drv_dma.h"
#include "wm_hal_uart.h"
#include "wm_drv_uart.h"
#include "wm_dt_hw.h"
#define LOG_TAG "drv_uart"
#include "wm_log.h"

/**
 * @brief Debug switch
 */
#define WM_DRV_UART_DEBUG                 0

/**
  * @brief  TX buffer number
  */
#define WM_DRV_UART_TX_BUF_NUM            4

/**
  * @brief  RX buffer number
  */
#define WM_DRV_UART_RX_BUF_NUM            WM_HAL_RX_RECV_BUF_NUM

/**
  * @brief  Use tx polling without tx buffer
  */
#define WM_DRV_USE_TX_POLLING_WITHOUT_BUF 0

/**
  * @brief  Uart mutex lock for api
  */
#define WM_DRV_UART_LOCK(ctx)                                                                              \
    do {                                                                                                   \
        if (wm_os_internal_recursive_mutex_acquire((ctx)->mutex, WM_OS_WAIT_TIME_MAX) != WM_ERR_SUCCESS) { \
            return WM_ERR_FAILED;                                                                          \
        }                                                                                                  \
    } while (0)

/**
  * @brief  Uart mutex unlock for api
  */
#define WM_DRV_UART_UNLOCK(ctx) wm_os_internal_recursive_mutex_release((ctx)->mutex)

/**
 * @brief uart driver log macros
 */
#if WM_DRV_UART_DEBUG
#define WM_DRV_UART_LOGI wm_log_info
#else
#define WM_DRV_UART_LOGI(...) ((void)0)
#endif
#define WM_DRV_UART_LOGE         WM_DRV_UART_LOGI
#define WM_DRV_UART_TRACE_FAIL() WM_DRV_UART_LOGE("err line %d", __LINE__)

#define WM_DRV_UART_MIN(a, b)    ((a) > (b) ? (b) : (a))

/**
 * @brief Uart local api types
 */
typedef struct {
    int (*init)(wm_device_t *dev, uint32_t rx_buf_size, uint32_t tx_buf_size);
    int (*deinit)(wm_device_t *dev);
    int (*ioctrl)(wm_device_t *dev, int cmd, uint32_t param, void *arg);
    int (*register_callback)(wm_device_t *dev, wm_drv_uart_callback_t cb, void *user_data);

    int (*write)(wm_device_t *dev, const uint8_t *buf, uint32_t size);
    int (*read)(wm_device_t *dev, uint8_t *buf, uint32_t size, uint32_t timeout);

    int (*receive_data)(wm_device_t *dev, uint8_t **buf, uint32_t *size);
    int (*release_data)(wm_device_t *dev, const uint8_t *buf, uint32_t size);

    int (*set_config)(wm_device_t *dev, wm_drv_uart_config_t *config);
    int (*get_config)(wm_device_t *dev, wm_drv_uart_config_t *config);
    int (*set_baudrate)(wm_device_t *dev, uint32_t baudrate);
    int (*get_baudrate)(wm_device_t *dev, uint32_t *baudrate);
    int (*set_data_bits)(wm_device_t *dev, wm_uart_data_bits_t data_bits);
    int (*get_data_bits)(wm_device_t *dev, wm_uart_data_bits_t *data_bits);
    int (*set_stop_bits)(wm_device_t *dev, wm_uart_stop_bits_t stop_bits);
    int (*get_stop_bits)(wm_device_t *dev, wm_uart_stop_bits_t *stop_bits);
    int (*set_parity)(wm_device_t *dev, wm_uart_parity_t parity);
    int (*get_parity)(wm_device_t *dev, wm_uart_parity_t *parity);
    int (*set_flow_ctrl)(wm_device_t *dev, wm_uart_flowctrl_t flow_ctrl);
    int (*get_flow_ctrl)(wm_device_t *dev, wm_uart_flowctrl_t *flow_ctrl);
} wm_drv_uart_ops_t;

/**
  * @brief  RX or TX buffer node struct
  */
typedef struct wm_drv_uart_buf_node_s {
    struct wm_drv_uart_buf_node_s *next;
    uint8_t *buf;                       /**< buffer for rx or tx           */
    int size;                           /**< buffer size                   */
    union {                             /**< union for tx or tx            */
        struct {                        /**< for rx                        */
            uint32_t data_holding : 1;  /**< have data wait to read        */
            uint32_t receiving    : 1;  /**< is being used to receive data */
            uint32_t timeout_flag : 1;  /**< rx fifo timeout event flag    */
            uint32_t reserved_3   : 29; /**< reserved                      */
        };
        struct {                       /**< for tx                        */
            uint32_t sending     : 1;  /**< have data in sending          */
            uint32_t date_size   : 16; /**< write offset in current node  */
            uint32_t reserved_17 : 15; /**< reserved                      */
        };
        uint32_t status;
    };
} wm_drv_uart_buf_node_t;

/**
  * @brief  RX or TX manager struct
  */
typedef struct {
    void *buf_address;              /**< memory address                    */
    wm_drv_uart_buf_node_t *r;      /**< read data pointer                 */
    wm_drv_uart_buf_node_t *w;      /**< chain with buffer for receive     */
    wm_drv_uart_buf_node_t *append; /**< append buffer pointer             */
    union {                         /**< union for tx or rx                */
        struct {                    /**< for rx                            */
            int end_offset;         /**< rx timeout offset                 */
            int read_offset;        /**< rx read offset                    */
        };
        struct {          /**< for tx           */
            int writting; /**< tx writting flag */
        };
    };
} wm_drv_uart_buf_manager_t;

/**
  * @brief  Uart driver context struct
  */
typedef struct {
    wm_os_mutex_t *mutex;         /**< mutex for lock api       */
    wm_os_mutex_t *tx_mutex;      /**< mutex for lock tx        */
    wm_os_mutex_t *rx_mutex;      /**< mutex for lock rx        */
    wm_os_sem_t *rx_sem;          /**< mutex for read function  */
    wm_os_sem_t *tx_sem;          /**< mutex for write function */
    wm_drv_uart_buf_manager_t tx; /**< tx buffer manager        */
    wm_drv_uart_buf_manager_t rx; /**< rx buffer manager        */
    wm_drv_uart_callback_t cb;    /**< user callback            */
    void *user_data;              /**< user callback  data      */

    int (*tx_api)(wm_hal_uart_dev_t *dev, const uint8_t *data, uint32_t size); /**< tx api, hal tx dma/it */
    int (*rx_api)(wm_hal_uart_dev_t *dev, uint8_t *buffer, uint32_t size);     /**< rx api, hal rx dma/it */
} wm_drv_uart_ctx_t;

/**
  * @brief  Uart driver data struct
  */
typedef struct {
    wm_hal_uart_dev_t hal_dev; /**< hal device     */
    wm_drv_uart_ctx_t ctx;     /**< driver context */
} wm_drv_uart_data_t;

/**
  * @brief  Uart device struct
  */
typedef struct {
    char *name;               /**< device name                          */
    wm_dt_hw_uart_t *hw;      /**< hardware info, @ref wm_dt_hw_uart_t  */
    wm_drv_uart_ops_t *ops;   /**< device operation interface           */
    wm_drv_uart_data_t *drv;  /**< driver context data                  */
    wm_device_status_t state; /**< device state                         */
    void *priv;               /**< user private data                    */
} wm_drv_uart_dev_t;

static int wm_drv_uart_pin_config(wm_dt_hw_pin_cfg_t *pin_cfg, int count)
{
    wm_gpio_num_t pin;

    for (int i = 0; i < count; i++) {
        pin = pin_cfg[i].pin_num;
        WM_DRV_UART_LOGI("config pin %d, %d %d\n", pin, pin_cfg[i].pin_mux, pin_cfg[i].pin_pupd);
        wm_drv_gpio_iomux_func_sel(pin, pin_cfg[i].pin_mux);
    }

    return WM_ERR_SUCCESS;
}

static void wm_drv_uart_evt_callback(void *hal_dev, wm_hal_uart_evt_t *evt, void *user_data)
{
    wm_drv_uart_dev_t *dev = (wm_drv_uart_dev_t *)user_data;
    wm_drv_uart_ctx_t *ctx = &dev->drv->ctx;
    wm_drv_uart_evt_t event;
    int data_offset;

    switch (evt->type) {
        case WM_HAL_UART_TX_DONE:
        {
            wm_drv_uart_buf_manager_t *tbm = &ctx->tx;

            WM_DRV_UART_LOGE("tx_done --\n");

            if (tbm->buf_address) {
                wm_drv_uart_buf_node_t *read   = tbm->r;
                wm_drv_uart_buf_node_t *append = tbm->append;

                WM_DRV_UART_LOGE("read=%p,sending=%d,append.size=%d,tbm->writting=%d\n", read, read->sending, append->size,
                                 tbm->writting);

                /*send ok!*/
                if (read->sending) {
                    WM_DRV_UART_LOGE("sent %d %d\n", evt->tx_data_len, read->date_size);
                    read->status = 0;
                    tbm->r       = read->next;
                }

                /*have data, append to hal uart*/
                if (append->date_size > 0 && !tbm->writting) {
                    WM_DRV_UART_LOGI("write done, append. %p, len=%u\n", append, append->date_size);

                    /*call wm_hal_uart_tx_dma or wm_hal_uart_tx_it*/
                    if (dev->drv->ctx.tx_api(&dev->drv->hal_dev, append->buf, append->date_size) == WM_ERR_SUCCESS) {
                        append->sending = 1;
                        if (tbm->w == append) {
                            tbm->w = tbm->w->next;
                        }
                        tbm->append = append->next;
                    } else {
                        WM_DRV_UART_TRACE_FAIL();
                    }
                }
            }

            /*signal to write function*/
            wm_os_internal_sem_release(ctx->tx_sem);

            /*notify user*/
            if (ctx->cb) {
                event.type = WM_DRV_UART_TX_DONE;
                event.len  = evt->tx_data_len;
                ctx->cb((wm_device_t *)dev, &event, ctx->user_data);
            }
            break;
        }
        case WM_HAL_UART_RX_READY:
        {
            wm_drv_uart_buf_node_t *write = ctx->rx.w;

            WM_DRV_UART_LOGE("rx cb: chain=%p,buf=[%p,%p],len=%d,timeout_flag=%d\n", write, write->buf, evt->rx_data,
                             evt->rx_len, evt->rx_timeout_flag);

            if (write->receiving) {
                /*change receiving status, evt->rx_len == 0 if receive at the end of buffer.*/
                write->data_holding = 1;
                write->timeout_flag = evt->rx_timeout_flag;

                data_offset = (evt->rx_data - write->buf) + evt->rx_len;
                if (data_offset == write->size) {
                    write->receiving = 0; /*receive end.*/
                    WM_DRV_UART_LOGE("--> cb set %p receing-->0\n", write);
                    ctx->rx.end_offset = 0;
                    write              = write->next;
                    WM_DRV_UART_LOGE("rx cb : cur node done\n");
                } else {
                    ctx->rx.end_offset = data_offset;
                }

                ctx->rx.w = write;
                /*signal to read function*/
                wm_os_internal_sem_release(ctx->rx_sem);

                /*notify user*/
                if (ctx->cb) {
                    event.type = WM_DRV_UART_RX_READY;
                    event.len  = evt->rx_len;
                    ctx->cb((wm_device_t *)dev, &event, ctx->user_data);
                }
            } else {
                WM_DRV_UART_TRACE_FAIL();
            }

            break;
        }
        case WM_HAL_UART_RX_ERROR:
        {
            if (ctx->cb) {
                event.type  = WM_DRV_UART_RX_ERROR;
                event.error = (wm_drv_uart_evt_error_t)evt->error;
                ctx->cb((wm_device_t *)dev, &event, ctx->user_data);
            }
            break;
        }
        default:
        {
            break;
        }
    }
}

/*
get the port from name
*/
static int wm_drv_uart_get_port(wm_drv_uart_dev_t *dev)
{
    int port = 0;
    int len  = strlen(dev->name);

    for (int i = len - 1; i >= 0; i--) {
        if (!(dev->name[i] >= '0' && dev->name[i] <= '9')) {
            port = atoi(&dev->name[i + 1]);
            break;
        }
    }
    return port;
}

static int wm_drv_uart_get_def_cfg(wm_dt_hw_uart_t *hw, wm_hal_uart_config_t *cfg)
{
    cfg->baudrate  = hw->uart_cfg.baudrate;
    cfg->data_bits = hw->uart_cfg.data_bits;
    cfg->stop_bits = hw->uart_cfg.stop_bits;
    cfg->parity    = hw->uart_cfg.parity;
    cfg->flow_ctrl = hw->uart_cfg.flow_ctrl;

    return WM_ERR_SUCCESS;
}

wm_hal_uart_dev_t *uart_get_hal_dev(wm_device_t *device)
{
    wm_drv_uart_dev_t *dev = (wm_drv_uart_dev_t *)device;
    return &dev->drv->hal_dev;
}

static int wm_drv_uart_init_dma(wm_drv_uart_dev_t *dev)
{
    wm_hal_uart_dma_t dma_info = { NULL, NULL, 0, 0 };
    uint8_t ch;
    wm_device_t *dma_dev;
    wm_drv_uart_ctx_t *ctx = &dev->drv->ctx;

    ctx->tx_api = wm_hal_uart_tx_it;
    ctx->rx_api = wm_hal_uart_rx_it;

    if (!dev->hw->dma_device_name) {
        return WM_ERR_SUCCESS;
    }

    dma_dev = wm_dt_get_device_by_name(dev->hw->dma_device_name);
    wm_drv_dma_init(dev->hw->dma_device_name);

    if (dma_dev) {
        if (wm_drv_dma_request_ch(dma_dev, &ch, 0) == WM_ERR_SUCCESS) {
            dma_info.tx_dma    = &((wm_dma_data_t *)dma_dev->drv)->hal_dev;
            dma_info.tx_dma_ch = ch;
            WM_DRV_UART_LOGI("dma tx ch=%d\n", (int)ch);
        } else {
            WM_DRV_UART_LOGE("request tx dma fail\n");
        }

        if (wm_drv_dma_request_ch(dma_dev, &ch, 0) == WM_ERR_SUCCESS) {
            dma_info.rx_dma    = &((wm_dma_data_t *)dma_dev->drv)->hal_dev;
            dma_info.rx_dma_ch = ch;
            WM_DRV_UART_LOGI("dma rx ch=%d\n", (int)ch);
        } else {
            WM_DRV_UART_LOGE("request rx dma fail\n");
        }

        if (wm_hal_uart_set_dma(&dev->drv->hal_dev, &dma_info) == WM_ERR_SUCCESS) {
            WM_DRV_UART_LOGI("set tx rx OK\n");
            if (dma_info.tx_dma) {
                ctx->tx_api = wm_hal_uart_tx_dma;
            }
            if (dma_info.rx_dma) {
                ctx->rx_api = wm_hal_uart_rx_dma;
            }
        } else {
            WM_DRV_UART_LOGE("set tx rx FAIL\n");
            if (dma_info.tx_dma) {
                wm_drv_dma_release_ch(dma_dev, dma_info.tx_dma_ch, WM_OS_WAIT_TIME_MAX);
            }
            if (dma_info.rx_dma) {
                wm_drv_dma_release_ch(dma_dev, dma_info.rx_dma_ch, WM_OS_WAIT_TIME_MAX);
            }
        }

        return WM_ERR_SUCCESS;
    } else {
        WM_DRV_UART_TRACE_FAIL();
        return WM_ERR_FAILED;
    }
}

static int wm_drv_uart_deinit_dma(wm_drv_uart_dev_t *dev)
{
    wm_hal_uart_dev_t *hal_dev = &dev->drv->hal_dev;

    wm_device_t *dma_dev;

    if (dev->hw->dma_device_name) {
        dma_dev = wm_dt_get_device_by_name(dev->hw->dma_device_name);

        if (hal_dev->tx_dma) {
            wm_drv_dma_release_ch(dma_dev, hal_dev->tx_dma_ch, WM_OS_WAIT_TIME_MAX);
        }

        if (hal_dev->rx_dma) {
            wm_drv_dma_release_ch(dma_dev, hal_dev->rx_dma_ch, WM_OS_WAIT_TIME_MAX);
        }
    }

    return WM_ERR_SUCCESS;
}

static int uart_init_buf_manager(wm_drv_uart_buf_manager_t *bm, uint32_t num, uint32_t buf_size)
{
    int size       = (num * sizeof(wm_drv_uart_buf_node_t)) + buf_size;
    int block_size = buf_size / num;
    wm_drv_uart_buf_node_t *pnode;
    uint8_t *p;

    if (buf_size == 0) {
        return WM_ERR_INVALID_PARAM;
    }

    pnode = malloc(size);
    if (!pnode) {
        return WM_ERR_FAILED;
    }

    bm->buf_address = pnode;

    /*skip nodes head and point to buffer*/
    p = (uint8_t *)(pnode + num);

    WM_DRV_UART_LOGE("buf=%p,size=%d\n", p, block_size);

    /*init nodes and link as ring*/
    for (int i = 0; i < num; i++) {
        WM_DRV_UART_LOGI("node%p, buf=%p\n", pnode + i, p);

        pnode[i].status = 0;
        pnode[i].buf    = p;
        pnode[i].size   = block_size;
        if (i != num - 1) {
            pnode[i].next = &pnode[i + 1];
        } else {
            pnode[i].next = &pnode[0];
        }
        p += block_size;
    }

    bm->w           = pnode;
    bm->r           = pnode;
    bm->append      = pnode;
    bm->end_offset  = 0;
    bm->read_offset = 0;

    return WM_ERR_SUCCESS;
}

static void uart_deinit_buf_manager(wm_drv_uart_buf_manager_t *bm)
{
    if (bm->buf_address) {
        free(bm->buf_address);
        bm->buf_address = NULL;
    }
}

static void uart_start_receive(wm_drv_uart_data_t *drv)
{
    wm_drv_uart_buf_node_t *append = drv->ctx.rx.append;

    while (!append->receiving) {
        append->status    = 0;
        append->receiving = 1;
        if (drv->ctx.rx_api(&drv->hal_dev, append->buf, append->size) != WM_ERR_SUCCESS) {
            append->receiving = 0;
            break;
        } else {
            append = append->next;
        }
    }

    drv->ctx.rx.append = append;
}

static int uart_init(wm_device_t *device, uint32_t rx_buf_size, uint32_t tx_buf_size)
{
    int ret = WM_ERR_INVALID_PARAM;

    wm_drv_uart_dev_t *dev = (wm_drv_uart_dev_t *)device;
    wm_dt_hw_uart_t *hw    = dev->hw;
    wm_drv_uart_data_t *drv;
    wm_drv_uart_ctx_t *ctx;
    wm_hal_uart_dev_t *hal;
    wm_hal_uart_config_t cfg;
    uint32_t port;
    uint32_t APB;

    if (dev->state == WM_DEV_ST_INITED) {
        WM_DRV_UART_LOGI("init before\n");
        return WM_ERR_ALREADY_INITED;
    }

    WM_DRV_UART_LOGI("start init\n");

    port = wm_drv_uart_get_port(dev);
    if (port < 0 || port >= WM_UART_PORT_NUM) {
        WM_DRV_UART_TRACE_FAIL();
        return WM_ERR_INVALID_PARAM;
    }

    if (!hw->rcc_device_name) {
        return WM_ERR_INVALID_PARAM;
    }

    if (!((tx_buf_size == 0 || (tx_buf_size >= WM_DRV_UART_TX_MIN_BUF_SIZE && tx_buf_size < WM_DRV_UART_BUF_BLOCK_MAX_SIZE)) &&
          (rx_buf_size >= WM_DRV_UART_RX_MIN_BUF_SIZE && rx_buf_size < WM_DRV_UART_BUF_BLOCK_MAX_SIZE))) {
        WM_DRV_UART_TRACE_FAIL();
        return WM_ERR_INVALID_PARAM;
    }

    drv = malloc(sizeof(*drv));
    if (!drv) {
        WM_DRV_UART_TRACE_FAIL();
        return WM_ERR_NO_MEM;
    }

    memset(drv, 0, sizeof(*drv));
    dev->drv = drv;
    hal      = &drv->hal_dev;
    ctx      = &drv->ctx;

    if (wm_os_internal_recursive_mutex_create(&ctx->mutex) != WM_OS_STATUS_SUCCESS) {
        free(drv);
        WM_DRV_UART_TRACE_FAIL();
        return WM_ERR_FAILED;
    }

    ret = WM_ERR_FAILED;

    if (wm_os_internal_recursive_mutex_create(&ctx->tx_mutex) != WM_OS_STATUS_SUCCESS) {
        goto FAIL;
    }

    if (wm_os_internal_recursive_mutex_create(&ctx->rx_mutex) != WM_OS_STATUS_SUCCESS) {
        goto FAIL;
    }

    if (tx_buf_size > 0 && uart_init_buf_manager(&ctx->tx, WM_DRV_UART_TX_BUF_NUM, tx_buf_size) != WM_ERR_SUCCESS) {
        goto FAIL;
    }

    if (uart_init_buf_manager(&ctx->rx, WM_DRV_UART_RX_BUF_NUM, rx_buf_size)) {
        goto FAIL;
    }

    if (wm_os_internal_sem_create(&ctx->tx_sem, 0) != WM_OS_STATUS_SUCCESS) {
        goto FAIL;
    }

    if (wm_os_internal_sem_create(&ctx->rx_sem, 0) != WM_OS_STATUS_SUCCESS) {
        goto FAIL;
    }

    wm_drv_uart_pin_config(dev->hw->pin_cfg, dev->hw->pin_cfg_count);

    wm_drv_clock_enable(wm_dt_get_device_by_name(hw->rcc_device_name), (1 << (port + 1)));

    APB = wm_drv_rcc_get_config_clock(wm_dt_get_device_by_name(hw->rcc_device_name), WM_RCC_TYPE_APB) * UNIT_MHZ;

    hal->reg          = (wm_uart_reg_t *)(hw->reg_base);
    hal->port         = (uint8_t)port;
    hal->irq_num      = hw->irq_cfg.irq_num;
    hal->irq_priority = hw->irq_cfg.irq_priority;
    hal->ref_clock    = APB;

    wm_drv_uart_get_def_cfg(hw, &cfg);

    ret = wm_hal_uart_init(hal, &cfg);
    if (ret != WM_ERR_SUCCESS) {
        WM_DRV_UART_TRACE_FAIL();
        goto FAIL;
    }

    wm_hal_uart_register_tx_callback(hal, wm_drv_uart_evt_callback, dev);
    wm_hal_uart_register_rx_callback(hal, wm_drv_uart_evt_callback, dev);

    wm_drv_uart_init_dma(dev);
    uart_start_receive(drv);

    dev->state = WM_DEV_ST_INITED;

    WM_DRV_UART_LOGI("uart init ok\n");

    return WM_ERR_SUCCESS;

FAIL:

    if (ctx->rx_sem) {
        wm_os_internal_sem_delete(ctx->rx_sem);
    }

    if (ctx->tx_sem) {
        wm_os_internal_sem_delete(ctx->tx_sem);
    }

    if (ctx->tx_mutex) {
        wm_os_internal_recursive_mutex_delete(ctx->tx_mutex);
    }

    if (ctx->rx_mutex) {
        wm_os_internal_recursive_mutex_delete(ctx->rx_mutex);
    }

    wm_os_internal_recursive_mutex_delete(ctx->mutex);

    uart_deinit_buf_manager(&ctx->tx);
    uart_deinit_buf_manager(&ctx->rx);

    free(drv);
    dev->drv = NULL;

    WM_DRV_UART_LOGI("uart init FAIL\n");

    return ret;
}

static int uart_deinit(wm_device_t *device)
{
    wm_drv_uart_dev_t *dev = (wm_drv_uart_dev_t *)device;
    wm_drv_uart_ctx_t *ctx;
    wm_hal_uart_dev_t *hal;
    wm_drv_uart_data_t *drv;
    wm_dt_hw_uart_t *hw = dev->hw;

    if (dev->state == WM_DEV_ST_INITED && dev->drv) {
        drv = dev->drv;
        ctx = &drv->ctx;
        hal = &drv->hal_dev;

        WM_DRV_UART_LOCK(ctx);

        if (wm_os_internal_recursive_mutex_acquire(ctx->tx_mutex, 250) != WM_ERR_SUCCESS) {
            WM_DRV_UART_UNLOCK(ctx);
            return WM_ERR_FAILED;
        }

        if (wm_os_internal_recursive_mutex_acquire(ctx->rx_mutex, 250) != WM_ERR_SUCCESS) {
            wm_os_internal_recursive_mutex_release(ctx->tx_mutex);
            WM_DRV_UART_UNLOCK(ctx);
            return WM_ERR_FAILED;
        }

        wm_hal_uart_deinit(hal);

        wm_drv_uart_deinit_dma(dev);

        wm_drv_clock_disable(wm_dt_get_device_by_name(hw->rcc_device_name), (1 << (hal->port + 1)));

        if (ctx->tx_sem) {
            wm_os_internal_sem_delete(ctx->tx_sem);
        }

        if (ctx->rx_sem) {
            wm_os_internal_sem_delete(ctx->rx_sem);
        }

        if (ctx->tx_mutex) {
            wm_os_internal_recursive_mutex_delete(ctx->tx_mutex);
        }

        if (ctx->rx_mutex) {
            wm_os_internal_recursive_mutex_delete(ctx->rx_mutex);
        }

        dev->drv = NULL;

        uart_deinit_buf_manager(&ctx->tx);
        uart_deinit_buf_manager(&ctx->rx);

        WM_DRV_UART_UNLOCK(ctx);

        if (ctx->mutex) {
            wm_os_internal_recursive_mutex_delete(ctx->mutex);
        }

        free(drv);

        dev->state = WM_DEV_ST_UNINIT;
        return WM_ERR_SUCCESS;
    } else {
        return WM_ERR_FAILED;
    }
}

/*
TODO: When there are new set and get commands in the future, they can be added to ioctrl
*/
static int uart_ioctrl(wm_device_t *device, int cmd, uint32_t param, void *arg)
{
    int ret = WM_ERR_INVALID_PARAM;
    return ret;
}

int uart_register_callback(wm_device_t *device, wm_drv_uart_callback_t cb, void *user_data)
{
    wm_drv_uart_dev_t *dev = (wm_drv_uart_dev_t *)device;
    wm_drv_uart_ctx_t *ctx;

    WM_DRV_UART_CHECK_PARAM(dev && dev->drv);

    ctx = &dev->drv->ctx;

    WM_DRV_UART_LOCK(ctx);

    ctx->cb        = cb;
    ctx->user_data = user_data;

    WM_DRV_UART_UNLOCK(ctx);

    return WM_ERR_SUCCESS;
}

static int uart_read_from_buffer(wm_drv_uart_data_t *drv, uint8_t *buf, uint32_t size, int *rx_fifo_timeout)
{
    wm_drv_uart_buf_manager_t *rbm = &drv->ctx.rx;
    wm_drv_uart_buf_node_t *read   = rbm->r;
    wm_drv_uart_buf_node_t *append = rbm->append;
    int data_size;
    int need_size;
    int copy_len;
    int count = 0;

    *rx_fifo_timeout = 0;

    WM_DRV_UART_LOGI("read start: size=%u\n", size);

    while (count < size) {
        /*check have data*/
        WM_DRV_UART_LOGI("read status:holding=%d,receiving=%d,timeout=%d\n", read->data_holding, read->receiving,
                         read->timeout_flag);
        if (read->data_holding) {
            if (read->receiving) {
                /* Receiving , hal use this node at the same time, process rx fifo timeout*/
                data_size = rbm->end_offset - rbm->read_offset;
                WM_DRV_UART_LOGI("read timeout process,data_size=%d,%d;%d\n", rbm->read_offset, rbm->end_offset, data_size);

                if (data_size > 0) {
                    *rx_fifo_timeout = read->timeout_flag;
                    WM_DRV_UART_LOGI("read timeout copy\n");

                    need_size = size - count;
                    copy_len  = WM_DRV_UART_MIN(need_size, data_size);

                    memcpy(buf + count, read->buf + rbm->read_offset, copy_len);
                    count += copy_len;
                    rbm->read_offset += copy_len;

                    if (copy_len == data_size) {
                        /*clear timeout flag*/
                        read->timeout_flag = 0;
                    }
                    WM_DRV_UART_LOGI("read,break for rx fifo timeout\n");
                    break; /*rx fifo timeout, so return*/
                } else {
                    WM_DRV_UART_LOGI("read , read before.\n");
                    *rx_fifo_timeout = read->timeout_flag;
                    break;
                }
            } else {
                /* Receive done , hal not use any more*/
                data_size = read->size - rbm->read_offset;
                need_size = size - count;

                WM_DRV_UART_LOGI("read done process data_size=%d,need=%d\n", data_size, need_size);

                copy_len = WM_DRV_UART_MIN(need_size, data_size);
                memcpy(buf + count, read->buf + rbm->read_offset, copy_len);
                count += copy_len;

                if (data_size <= need_size) {
                    /*read all the node data, may be need read the next node*/
                    WM_DRV_UART_LOGI("read all\n");
                    rbm->read_offset = 0;

                    /*have fifo timeout*/
                    *rx_fifo_timeout = read->timeout_flag;

                    /*go to next node*/
                    read   = read->next;
                    rbm->r = read;

                    /*append buf to hal*/
                    append->status    = 0;
                    append->receiving = 1;

                    WM_DRV_UART_LOGI("read set %p receing-->1\n", append);

                    /*call wm_hal_uart_rx_dma or wm_hal_uart_rx_it*/
                    if (drv->ctx.rx_api(&drv->hal_dev, append->buf, append->size) != WM_ERR_SUCCESS) {
                        append->receiving = 0;
                        WM_DRV_UART_TRACE_FAIL();
                        WM_DRV_UART_LOGI("read set %p receing-->0\n", append);
                    } else {
                        append      = append->next;
                        rbm->append = append;
                    }
                } else {
                    /*enough reading, the node has data left*/
                    WM_DRV_UART_LOGI("read enough\n");
                    rbm->read_offset += need_size;
                }
            }
        } else {
            /*no data in current node.*/
            break;
        }
    }
    WM_DRV_UART_LOGI("read end=%d\n", count);
    return count;
}

static int uart_receive_from_buffer(wm_drv_uart_data_t *drv, uint8_t **buf, uint32_t *size)
{
    wm_drv_uart_buf_manager_t *rbm = &drv->ctx.rx;
    wm_drv_uart_buf_node_t *read   = rbm->r;
    int data_size;
    int ret = WM_ERR_FAILED;

    if (read->data_holding) {
        if (read->receiving) {
            data_size = rbm->end_offset - rbm->read_offset;
        } else {
            data_size = read->size - rbm->read_offset;
        }

        if (data_size > 0) {
            *buf  = read->buf + rbm->read_offset;
            *size = data_size;
            ret   = WM_ERR_SUCCESS;
            WM_DRV_UART_LOGI("rcv done process,read offset=%d, data_size=%d,receiving=%d\n", rbm->read_offset, data_size,
                             read->receiving);
        }
    } else {
        /*no data in current node.*/
        WM_DRV_UART_LOGI("rcv no data\n");
    }

    return ret;
}

static int uart_release_to_buffer(wm_drv_uart_data_t *drv, const uint8_t *buf, uint32_t size)
{
    wm_drv_uart_buf_manager_t *rbm = &drv->ctx.rx;
    wm_drv_uart_buf_node_t *read   = rbm->r;
    wm_drv_uart_buf_node_t *append = rbm->append;

    int ret = WM_ERR_FAILED;

    WM_DRV_UART_LOGI("release start: size=%u\n", size);

    /*check have data*/
    WM_DRV_UART_LOGI("release status:holding=%d,receiving=%d,timeout=%d\n", read->data_holding, read->receiving,
                     read->timeout_flag);

    if (read->data_holding) {
        read->timeout_flag = 0;
        rbm->read_offset += size;

        ret = WM_ERR_SUCCESS;

        if (rbm->read_offset == read->size) {
            WM_DRV_UART_LOGI("release to the end of node.\n");
            /*release all the node data*/
            rbm->read_offset = 0;

            /*go to next*/
            read   = read->next;
            rbm->r = read;

            /*append buf to hal*/
            append->status    = 0;
            append->receiving = 1;

            WM_DRV_UART_LOGE("----> release set %p receing-->1\n", append);

            /*call wm_hal_uart_rx_dma or wm_hal_uart_rx_it*/
            if (drv->ctx.rx_api(&drv->hal_dev, append->buf, append->size) != WM_ERR_SUCCESS) {
                append->receiving = 0;
                WM_DRV_UART_TRACE_FAIL();
                WM_DRV_UART_LOGE("------> release set %p receing-->0\n", append);
            } else {
                rbm->append = append->next;
            }

            ret = WM_ERR_SUCCESS;
        }
    } else {
        WM_DRV_UART_TRACE_FAIL();
    }

    return ret;
}

static int uart_write_to_buffer(wm_drv_uart_data_t *drv, const uint8_t *data, uint32_t size)
{
    wm_drv_uart_buf_manager_t *tbm = &drv->ctx.tx;
    wm_drv_uart_buf_node_t *write  = tbm->w;
    wm_drv_uart_buf_node_t *append = tbm->append;

    int copy_len;
    int count = 0;

    int buf_size;
    int need_size;

    WM_DRV_UART_LOGI("wb start: size=%u\n", size);

    /*prevent appending buffer at tx done */
    tbm->writting = 1;

    while (count < size) {
        /*check have data*/
        WM_DRV_UART_LOGI("wb:sending=%d, offset=%d\n", write->sending, write->date_size);
        if (!write->sending && (buf_size = write->size - write->date_size) > 0) {
            need_size = size - count;
            copy_len  = WM_DRV_UART_MIN(need_size, buf_size);
            memcpy(write->buf + write->date_size, data + count, copy_len);

            WM_DRV_UART_LOGI("wb %p:copy, offset=%d, len=%d\n", write, write->date_size, copy_len);
            write->date_size += copy_len;
            count += copy_len;

            if (write->date_size == write->size) {
                write  = write->next;
                tbm->w = write;
            }
        } else {
            WM_DRV_UART_LOGI("wb: buffer full write=%p\n", write);
            break;
        }
    }

    /*hal is idle and have data to write*/
    if (append == tbm->r && append->date_size > 0) {
        WM_DRV_UART_LOGI("start write append. %p, len=%u\n", append, append->date_size);
        wm_drv_uart_buf_node_t *cur = append;

        append->sending = 1;
        if (tbm->w == append) {
            tbm->w = tbm->w->next;
        }
        tbm->append = append->next;

        tbm->writting = 0;
        /*call wm_hal_uart_tx_dma or wm_hal_uart_tx_it*/
        if (drv->ctx.tx_api(&drv->hal_dev, cur->buf, cur->date_size) != WM_ERR_SUCCESS) {
            WM_DRV_UART_TRACE_FAIL();
        }
    }

    tbm->writting = 0;

    WM_DRV_UART_LOGI("write %d to buffer\n", count);
    return count;
}

static int uart_write(wm_device_t *device, const uint8_t *data, uint32_t size)
{
    wm_drv_uart_dev_t *dev = (wm_drv_uart_dev_t *)device;
    wm_drv_uart_ctx_t *ctx;
    int count = 0;
    int cur_write_len;
    int isr_count;

    WM_DRV_UART_CHECK_PARAM(dev && dev->drv && data && size > 0);

    ctx = &dev->drv->ctx;

    isr_count = wm_os_internal_get_isr_count();

    if (isr_count > 0 || xTaskGetSchedulerState() != taskSCHEDULER_RUNNING) {
        /* In interrupt, mutex can't protect resource any more, so use polling mode;
           Use polling if the scheduler is not running. */
        wm_hal_uart_tx_polling(&dev->drv->hal_dev, data, size);
        count = size;
    } else {
        wm_os_internal_recursive_mutex_acquire(ctx->tx_mutex, WM_OS_WAIT_TIME_MAX);

        if (ctx->tx.buf_address) {
            /* buffer mode,copy to buffer*/
            while (count < size) {
                wm_os_internal_sem_reset(ctx->tx_sem);
                cur_write_len = uart_write_to_buffer(dev->drv, data + count, size - count);
                if (cur_write_len > 0) {
                    count += cur_write_len;
                }

                WM_DRV_UART_LOGI("write %d/%d to buffer\n", count, size);

                if (count < size) {
                    if (wm_os_internal_sem_acquire(ctx->tx_sem, WM_OS_WAIT_TIME_MAX) != WM_OS_STATUS_SUCCESS) {
                        break;
                    }
                }
            }
        } else {
            /* non buffer mode, write directly*/
#if WM_DRV_USE_TX_POLLING_WITHOUT_BUF
            wm_hal_uart_tx_polling(&dev->drv->hal_dev, data, size);
#else
            wm_os_internal_sem_reset(ctx->tx_sem);
            /*call wm_hal_uart_tx_dma or wm_hal_uart_tx_it*/
            if (ctx->tx_api(&dev->drv->hal_dev, data, size) == WM_ERR_SUCCESS) {
                wm_os_internal_sem_acquire(ctx->tx_sem, WM_OS_WAIT_TIME_MAX);
            } else {
                WM_DRV_UART_TRACE_FAIL();
            }
#endif
            count = size;
        }

        wm_os_internal_recursive_mutex_release(ctx->tx_mutex);
    }
    return count;
}

static int uart_read(wm_device_t *device, uint8_t *buf, uint32_t size, uint32_t timeout)
{
    wm_drv_uart_dev_t *dev = (wm_drv_uart_dev_t *)device;
    wm_drv_uart_ctx_t *ctx;

    int count           = 0;
    int rx_fifo_timeout = 0;
    int cur_read_len;
    uint32_t start_ms;
    uint32_t delta_ms;

    WM_DRV_UART_CHECK_PARAM(dev && dev->drv && buf && size > 0);

    ctx = &dev->drv->ctx;

    if (wm_os_internal_recursive_mutex_acquire(ctx->rx_mutex, WM_OS_WAIT_TIME_MAX) != WM_ERR_SUCCESS) {
        return WM_ERR_FAILED;
    }

    start_ms = wm_os_internal_get_time_ms();

    while (count < size) {
        /*read data from buffer*/
        WM_DRV_UART_LOGI("reading: count=%d,size=%u\n", count, size);

        wm_os_internal_sem_reset(ctx->rx_sem);
        cur_read_len = uart_read_from_buffer(dev->drv, buf + count, size - count, &rx_fifo_timeout);
        if (cur_read_len >= 0) {
            count += cur_read_len;
            if (rx_fifo_timeout) {
                WM_DRV_UART_LOGI("read timeout: count=%d\n", count);
                break;
            }
        }

        /*check and wait */
        delta_ms = wm_os_internal_get_time_ms() - start_ms;
        if (count < size && delta_ms < timeout) {
            WM_DRV_UART_LOGI("reading wait next: need=%d, wait=0x%x\n", size - count, timeout - delta_ms);
            if (wm_os_internal_sem_acquire(ctx->rx_sem, pdMS_TO_TICKS(timeout - delta_ms)) == WM_OS_STATUS_ERROR) {
                break;
            }
        } else {
            WM_DRV_UART_LOGI("read OK or timeout: need=%d\n", size - count);
            break;
        }
    }

    wm_os_internal_recursive_mutex_release(ctx->rx_mutex);

    WM_DRV_UART_LOGI("read end: count=%d\n", count);

    return count;
}

static int uart_receive_data(wm_device_t *device, uint8_t **buf, uint32_t *size)
{
    wm_drv_uart_dev_t *dev = (wm_drv_uart_dev_t *)device;
    wm_drv_uart_ctx_t *ctx;
    int ret;

    WM_DRV_UART_CHECK_PARAM(dev && dev->drv && buf && size);

    ctx = &dev->drv->ctx;

    if (wm_os_internal_recursive_mutex_acquire(ctx->rx_mutex, WM_OS_WAIT_TIME_MAX) != WM_ERR_SUCCESS) {
        return WM_ERR_FAILED;
    }

    ret = uart_receive_from_buffer(dev->drv, buf, size);

    wm_os_internal_recursive_mutex_release(ctx->rx_mutex);

    return ret;
}

static int uart_release_data(wm_device_t *device, const uint8_t *buf, uint32_t size)
{
    wm_drv_uart_dev_t *dev = (wm_drv_uart_dev_t *)device;
    wm_drv_uart_ctx_t *ctx;
    int ret;

    WM_DRV_UART_CHECK_PARAM(dev && dev->drv && buf && size);

    ctx = &dev->drv->ctx;

    if (wm_os_internal_recursive_mutex_acquire(ctx->rx_mutex, WM_OS_WAIT_TIME_MAX) != WM_ERR_SUCCESS) {
        return WM_ERR_FAILED;
    }

    ret = uart_release_to_buffer(dev->drv, buf, size);

    wm_os_internal_recursive_mutex_release(ctx->rx_mutex);

    return ret;
}

static int uart_set_config(wm_device_t *device, wm_drv_uart_config_t *config)
{
    wm_drv_uart_dev_t *dev = (wm_drv_uart_dev_t *)device;
    wm_drv_uart_ctx_t *ctx;
    int ret;

    WM_DRV_UART_CHECK_PARAM(dev->drv && config);

    ctx = &dev->drv->ctx;

    WM_DRV_UART_LOCK(ctx);

    ret = wm_hal_uart_set_config(&dev->drv->hal_dev, (wm_hal_uart_config_t *)config);

    WM_DRV_UART_UNLOCK(ctx);

    return ret;
}

static int uart_get_config(wm_device_t *device, wm_drv_uart_config_t *config)
{
    wm_drv_uart_dev_t *dev = (wm_drv_uart_dev_t *)device;
    wm_drv_uart_ctx_t *ctx;
    int ret;

    WM_DRV_UART_CHECK_PARAM(dev->drv && config);

    ctx = &dev->drv->ctx;
    WM_DRV_UART_LOCK(ctx);

    ret = wm_hal_uart_get_config(&dev->drv->hal_dev, (wm_hal_uart_config_t *)config);

    WM_DRV_UART_UNLOCK(ctx);

    return ret;
}

static int uart_set_baudrate(wm_device_t *device, uint32_t baudrate)
{
    wm_drv_uart_dev_t *dev = (wm_drv_uart_dev_t *)device;
    wm_drv_uart_ctx_t *ctx;
    int ret;

    WM_DRV_UART_CHECK_PARAM(dev->drv);

    ctx = &dev->drv->ctx;
    WM_DRV_UART_LOCK(ctx);

    ret = wm_hal_uart_set_baudrate(&dev->drv->hal_dev, baudrate);

    WM_DRV_UART_UNLOCK(ctx);

    return ret;
}

static int uart_get_baudrate(wm_device_t *device, uint32_t *baudrate)
{
    wm_drv_uart_dev_t *dev = (wm_drv_uart_dev_t *)device;
    wm_drv_uart_ctx_t *ctx;
    int ret;

    WM_DRV_UART_CHECK_PARAM(dev->drv && baudrate);

    ctx = &dev->drv->ctx;
    WM_DRV_UART_LOCK(ctx);

    ret = wm_hal_uart_get_baudrate(&dev->drv->hal_dev, baudrate);

    WM_DRV_UART_UNLOCK(ctx);

    return ret;
}

static int uart_set_data_bits(wm_device_t *device, wm_uart_data_bits_t data_bits)
{
    wm_drv_uart_dev_t *dev = (wm_drv_uart_dev_t *)device;
    wm_drv_uart_ctx_t *ctx;
    int ret;

    WM_DRV_UART_CHECK_PARAM(dev->drv);

    ctx = &dev->drv->ctx;
    WM_DRV_UART_LOCK(ctx);

    ret = wm_hal_uart_set_data_bits(&dev->drv->hal_dev, data_bits);

    WM_DRV_UART_UNLOCK(ctx);

    return ret;
}

static int uart_get_data_bits(wm_device_t *device, wm_uart_data_bits_t *data_bits)
{
    wm_drv_uart_dev_t *dev = (wm_drv_uart_dev_t *)device;
    wm_drv_uart_ctx_t *ctx;
    int ret;

    WM_DRV_UART_CHECK_PARAM(dev->drv && data_bits);

    ctx = &dev->drv->ctx;

    WM_DRV_UART_LOCK(ctx);

    ret = wm_hal_uart_get_data_bits(&dev->drv->hal_dev, data_bits);

    WM_DRV_UART_UNLOCK(ctx);

    return ret;
}

static int uart_set_stop_bits(wm_device_t *device, wm_uart_stop_bits_t stop_bits)
{
    wm_drv_uart_dev_t *dev = (wm_drv_uart_dev_t *)device;
    wm_drv_uart_ctx_t *ctx;
    int ret;

    WM_DRV_UART_CHECK_PARAM(dev->drv);

    ctx = &dev->drv->ctx;

    WM_DRV_UART_LOCK(ctx);

    ret = wm_hal_uart_set_stop_bits(&dev->drv->hal_dev, stop_bits);

    WM_DRV_UART_UNLOCK(ctx);

    return ret;
}

static int uart_get_stop_bits(wm_device_t *device, wm_uart_stop_bits_t *stop_bits)
{
    wm_drv_uart_dev_t *dev = (wm_drv_uart_dev_t *)device;
    wm_drv_uart_ctx_t *ctx;
    int ret;

    WM_DRV_UART_CHECK_PARAM(dev->drv && stop_bits);

    ctx = &dev->drv->ctx;

    WM_DRV_UART_LOCK(ctx);

    ret = wm_hal_uart_get_stop_bits(&dev->drv->hal_dev, stop_bits);

    WM_DRV_UART_UNLOCK(ctx);

    return ret;
}

static int uart_set_parity(wm_device_t *device, wm_uart_parity_t parity)
{
    wm_drv_uart_dev_t *dev = (wm_drv_uart_dev_t *)device;
    wm_drv_uart_ctx_t *ctx;
    int ret;

    WM_DRV_UART_CHECK_PARAM(dev->drv);

    ctx = &dev->drv->ctx;

    WM_DRV_UART_LOCK(ctx);

    ret = wm_hal_uart_set_parity(&dev->drv->hal_dev, parity);

    WM_DRV_UART_UNLOCK(ctx);

    return ret;
}

static int uart_get_parit(wm_device_t *device, wm_uart_parity_t *parity)
{
    wm_drv_uart_dev_t *dev = (wm_drv_uart_dev_t *)device;
    wm_drv_uart_ctx_t *ctx;
    int ret;

    WM_DRV_UART_CHECK_PARAM(dev->drv && parity);

    ctx = &dev->drv->ctx;

    WM_DRV_UART_LOCK(ctx);

    ret = wm_hal_uart_get_parity(&dev->drv->hal_dev, parity);

    WM_DRV_UART_UNLOCK(ctx);

    return ret;
}

static int uart_set_flow_ctrl(wm_device_t *device, wm_uart_flowctrl_t flow_ctrl)
{
    wm_drv_uart_dev_t *dev = (wm_drv_uart_dev_t *)device;
    wm_drv_uart_ctx_t *ctx;
    int ret;

    WM_DRV_UART_CHECK_PARAM(dev->drv);

    ctx = &dev->drv->ctx;

    WM_DRV_UART_LOCK(ctx);

    ret = wm_hal_uart_set_flow_ctrl(&dev->drv->hal_dev, flow_ctrl);

    WM_DRV_UART_UNLOCK(ctx);

    return ret;
}

static int uart_get_flow_ctrl(wm_device_t *device, wm_uart_flowctrl_t *flow_ctrl)
{
    wm_drv_uart_dev_t *dev = (wm_drv_uart_dev_t *)device;
    wm_drv_uart_ctx_t *ctx;
    int ret;

    WM_DRV_UART_CHECK_PARAM(dev->drv && flow_ctrl);

    ctx = &dev->drv->ctx;

    WM_DRV_UART_LOCK(ctx);

    ret = wm_hal_uart_get_flow_ctrl(&dev->drv->hal_dev, flow_ctrl);

    WM_DRV_UART_UNLOCK(ctx);

    return ret;
}

const wm_drv_uart_ops_t wm_drv_uart_ops = {
    .init              = uart_init,
    .deinit            = uart_deinit,
    .ioctrl            = uart_ioctrl,
    .register_callback = uart_register_callback,

    .write        = uart_write,
    .read         = uart_read,
    .receive_data = uart_receive_data,
    .release_data = uart_release_data,

    .set_config    = uart_set_config,
    .get_config    = uart_get_config,
    .set_baudrate  = uart_set_baudrate,
    .get_baudrate  = uart_get_baudrate,
    .set_data_bits = uart_set_data_bits,
    .get_data_bits = uart_get_data_bits,
    .set_stop_bits = uart_set_stop_bits,
    .get_stop_bits = uart_get_stop_bits,
    .set_parity    = uart_set_parity,
    .get_parity    = uart_get_parit,
    .set_flow_ctrl = uart_set_flow_ctrl,
    .get_flow_ctrl = uart_get_flow_ctrl,
};
