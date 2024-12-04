/**
 * @file wm_hal_uart.c
 *
 * @brief hal uart Module
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
#include <stdlib.h>

#include "wm_types.h"
#include "wm_error.h"
#include "wm_soc.h"
#include "wm_ll_uart.h"
#include "wm_hal_uart.h"
#include "wm_hal_irq.h"
#include "wm_hal_dma.h"
#include "wm_log.h"
#include "wm_hal_gpio.h"
#undef LOG_TAG
#define LOG_TAG           "hal_uart"

/**
 * @brief Debug switch
 */
#define WM_HAL_UART_DEBUG 0

/**
 * @brief uart log macros
 */
#if WM_HAL_UART_DEBUG
#define WM_HAL_UART_LOGI wm_log_info
#else
#define WM_HAL_UART_LOGI(...) ((void)0)
#endif

#define WM_HAL_UART_LOGE         WM_HAL_UART_LOGI
#define WM_HAL_UART_TRACE_FAIL() WM_HAL_UART_LOGE("err line %d", __LINE__)

/**
 * @brief Baudrate calculate macro.
 */
#define WM_HAL_UART_CAL_BAUDRATE(APB, baudrate) \
    ((APB) / (16 * (baudrate)) - 1) | ((((APB) % ((baudrate) * 16)) * 16 / ((baudrate) * 16)) << 16)

/**
 * @brief Check uart params
 */
#define WM_HAL_UART_CHECK_PARAM(p)   \
    if (!(p)) {                      \
        WM_HAL_UART_TRACE_FAIL();    \
        return WM_ERR_INVALID_PARAM; \
    }

/**
 * @brief Uart ALL IRQ mask
 */
#define WM_HAL_UART_ALL_IRQ_MASK 0x3ff

/**
 * @brief Uart TX intrrupt mode IRQ mask
 */
#define WM_HAL_TX_IT_MASKS       (WM_LL_UART_TX_FIFO_EMPTY | WM_LL_UART_TX_FIFO_TRIG_LEVEL)

/**
 * @brief Uart RX intrrupt mode IRQ mask
 */
#define WM_HAL_RX_IT_MASKS                                                                                           \
    (WM_LL_UART_RX_FIFO_TRIG_LEVEL | WM_LL_UART_RX_DATA_TIMEOUT | WM_LL_UART_BREAK_DETECT | WM_LL_UART_FRAME_ERROR | \
     WM_LL_UART_PATIRY_ERROR | WM_LL_UART_RXFIFO_OVER_ERROR)

/**
 * @brief Uart RXerror IRQ mask
 */
#define WM_HAL_RX_ERR_MASKS \
    (WM_LL_UART_BREAK_DETECT | WM_LL_UART_FRAME_ERROR | WM_LL_UART_PATIRY_ERROR | WM_LL_UART_RXFIFO_OVER_ERROR)

/**
 * @brief DMA empty flag for receive
 */
#define WM_UART_DMA_STATUS_BUF_EMPTY (1UL << 0x1f)

/**
 * @brief Uart RX error index
 */
enum {
    WM_HAL_UART_STATISTICS_BREAK,
    WM_HAL_UART_STATISTICS_FRAME_ERR,
    WM_HAL_UART_STATISTICS_PARITY_ERR,
    WM_HAL_UART_STATISTICS_OVERRUN,
    WM_HAL_UART_STATISTICS_BUFFER_FULL,
    WM_HAL_UART_STATISTICS_RECV_RST,
    WM_HAL_UART_STATISTICS_ERR_NUM,
};

/**
 * @brief Uart TX fifo trigger level
 */
enum {
    TX_FIFO_TIRGGER_LEVLE_EMPTY   = 0,
    TX_FIFO_TIRGGER_LEVLE_4BYTES  = 1,
    TX_FIFO_TIRGGER_LEVLE_8BYTES  = 2,
    TX_FIFO_TIRGGER_LEVLE_16BYTES = 3,
};

/**
 * @brief Uart RX fifo trigger level
 */
enum {
    RX_FIFO_TIRGGER_LEVLE_1BYTE   = 0,
    RX_FIFO_TIRGGER_LEVLE_4BYTES  = 1,
    RX_FIFO_TIRGGER_LEVLE_8BYTES  = 2,
    RX_FIFO_TIRGGER_LEVLE_16BYTES = 3,
};

/*save 2~5 uart dev context for the same irq handler*/
static wm_hal_uart_dev_t *g_uart_dev[WM_UART_PORT_NUM];
static int8_t g_irq25_num = 0;
static int8_t g_dma_port  = -1;

typedef struct {
    wm_hal_uart_config_t config;        /**< uart config               */
    wm_hal_uart_callback_t rx_callback; /**< rx callback               */
    wm_hal_uart_callback_t tx_callback; /**< tx callback               */
    void *rx_user_data;                 /**< rx user data              */
    void *tx_user_data;                 /**< tx user data              */
    uint32_t tx_size;                   /**< TX buffer size,DMA use    */
    uint8_t *tx_data;                   /**< TX buffer ,IT use         */
    uint32_t tx_offset;                 /**< TX send offset,IT use     */
    wm_hal_dma_desc_t *r;               /**< DMA receiving chain read  */
    wm_hal_dma_desc_t *w;               /**< DMA receiving chain write */
    uint32_t rx_read_offset;            /**< RX read offset            */
    uint32_t rx_timeout_offset;         /**< RX fifo timeout offset    */

    wm_hal_dma_desc_t rx_desc[WM_HAL_RX_RECV_BUF_NUM]; /**< DMA desc or rx it nodes   */

    struct {                        /**< uart status               */
        uint32_t timeout_flag : 1;  /**< timeout flag              */
        uint32_t reserved     : 31; /**< reserved                  */
    };
#ifdef CONFIG_UART_STATISTICS_ENABLE
    uint32_t error[WM_HAL_UART_STATISTICS_ERR_NUM];
    uint64_t send_cnt;
    uint64_t recv_cnt;
#endif
} wm_hal_uart_ctx_t;

static void wm_hal_uart_dma_rx_timeout(wm_hal_uart_dev_t *dev);
static void wm_hal_uart_stop_rx_dma(wm_hal_uart_dev_t *dev);
static void wm_hal_uart_stop_tx_dma(wm_hal_uart_dev_t *dev);
static void wm_hal_uart_rx_it_receive(wm_hal_uart_dev_t *dev, uint32_t rx_fifo_timeout);
static int wm_hal_uart_rx_init_chain(wm_hal_uart_dev_t *dev);

static int wm_hal_uart_check_baudrate(uint32_t baudrate)
{
    switch (baudrate) {
        case WM_UART_BAUDRATE_B600:
        case WM_UART_BAUDRATE_B1200:
        case WM_UART_BAUDRATE_B1800:
        case WM_UART_BAUDRATE_B2400:
        case WM_UART_BAUDRATE_B4800:
        case WM_UART_BAUDRATE_B9600:
        case WM_UART_BAUDRATE_B19200:
        case WM_UART_BAUDRATE_B38400:
        case WM_UART_BAUDRATE_B57600:
        case WM_UART_BAUDRATE_B115200:
        case WM_UART_BAUDRATE_B230400:
        case WM_UART_BAUDRATE_B460800:
        case WM_UART_BAUDRATE_B921600:
        case WM_UART_BAUDRATE_B1000000:
        case WM_UART_BAUDRATE_B1250000:
        case WM_UART_BAUDRATE_B1500000:
        case WM_UART_BAUDRATE_B2000000:
        {
            break;
        }
        default:
        {
            return WM_ERR_INVALID_PARAM;
        }
    }
    return WM_ERR_SUCCESS;
}

/*uart0 and uart1 irq proccess*/
static void wm_hal_uart_irq_handler(wm_irq_no_t irq, void *arg)
{
    wm_hal_uart_dev_t *dev = (wm_hal_uart_dev_t *)arg;
    wm_uart_reg_t *reg     = dev->reg;
    wm_hal_uart_ctx_t *ctx = dev->ctx;
    uint32_t intr_src;

    /*get interrupt and clear interrupt*/
    intr_src         = reg->int_src.val;
    reg->int_src.val = intr_src;

    if (!intr_src) {
        return;
    }

    WM_HAL_UART_LOGI("line %d,int=0x%x\n", __LINE__, intr_src);

    /*RX*/
    if (intr_src & (WM_LL_UART_RX_FIFO_TRIG_LEVEL | WM_LL_UART_RX_DATA_TIMEOUT)) {
        if (!dev->rx_dma) {
            /*1 for rx it*/
            wm_hal_uart_rx_it_receive(dev, (intr_src & WM_LL_UART_RX_DATA_TIMEOUT));
        }

        /*RX timeout*/
        if ((intr_src & WM_LL_UART_RX_DATA_TIMEOUT)) {
            WM_HAL_UART_LOGI("timeout\n");
            /*2 for rx polling*/
            ctx->timeout_flag = 1;

            /*3 for rx dma*/
            if (dev->rx_dma && wm_ll_uart_is_rx_dma_enabled(reg)) {
                wm_hal_uart_dma_rx_timeout(dev);
            }
        }
    }

    /*TX*/
    if ((intr_src & (WM_HAL_TX_IT_MASKS)) && !dev->tx_dma && ctx->tx_offset <= ctx->tx_size) {
        int tx_fifo_free = WM_UART_HW_FIFO_NUM - wm_ll_uart_get_txfifo_count(dev->reg);
        int need_write   = ctx->tx_size - ctx->tx_offset;

        if (tx_fifo_free > need_write) {
            tx_fifo_free = need_write;
        }

#ifdef CONFIG_UART_STATISTICS_ENABLE
        ctx->send_cnt += tx_fifo_free;
#endif

        /*write data to tx hardware fifo*/
        while (tx_fifo_free-- > 0) {
            wm_ll_uart_putchar(dev->reg, ctx->tx_data[ctx->tx_offset++]);
        }

        if (ctx->tx_size == ctx->tx_offset) {
            /*send end, notify user*/
            wm_hal_uart_evt_t evt = {
                .type        = WM_HAL_UART_TX_DONE,
                .tx_data_len = ctx->tx_size,
            };

            /*clear tx_size for next use.*/
            ctx->tx_size = 0;
            if (ctx->tx_callback) {
                ctx->tx_callback(dev, &evt, ctx->tx_user_data);
            }
        }
    }

    /*CTS*/
    if ((intr_src & WM_LL_UART_CTS_CHANGE) && (ctx->config.flow_ctrl & WM_UART_FLOW_CTRL_CTS)) {
        WM_HAL_UART_LOGI("enable tx %d %d\n", dev->port, wm_ll_uart_is_cts_enabled(reg));
        wm_ll_uart_enable_tx(reg, wm_ll_uart_is_cts_enabled(reg));
    }

    /*RX Error*/
    if ((intr_src & (WM_HAL_RX_ERR_MASKS)) && ctx->rx_callback) {
        wm_hal_uart_evt_t evt = { .type = WM_HAL_UART_RX_ERROR };

        if (intr_src & WM_LL_UART_BREAK_DETECT) {
            evt.error = WM_HAL_UART_BREAK;
        } else if (intr_src & WM_LL_UART_FRAME_ERROR) {
            evt.error = WM_HAL_UART_ERROR_FRAMING;
        } else if (intr_src & WM_LL_UART_PATIRY_ERROR) {
            evt.error = WM_HAL_UART_ERROR_PARITY;
        } else if (intr_src & WM_LL_UART_RXFIFO_OVER_ERROR) {
            evt.error = WM_HAL_UART_ERROR_OVERRUN;
            /*When RX overrun, the RX buffer needs to be cleared,
            otherwise the interrupts will come continuously*/
            wm_ll_uart_reset_rx_fifo(dev->reg);
            WM_HAL_UART_LOGI("rx overrun");
        }
        ctx->rx_callback(dev, &evt, ctx->rx_user_data);

#ifdef CONFIG_UART_STATISTICS_ENABLE
        if (intr_src & WM_LL_UART_BREAK_DETECT) {
            ctx->error[WM_HAL_UART_STATISTICS_BREAK]++;

        } else if (intr_src & WM_LL_UART_FRAME_ERROR) {
            ctx->error[WM_HAL_UART_STATISTICS_FRAME_ERR]++;

        } else if (intr_src & WM_LL_UART_PATIRY_ERROR) {
            ctx->error[WM_HAL_UART_STATISTICS_PARITY_ERR]++;

        } else if (intr_src & WM_LL_UART_RXFIFO_OVER_ERROR) {
            ctx->error[WM_HAL_UART_STATISTICS_OVERRUN]++;
        }
#endif
    }
}

/*uart 2 3 4 5 irq handler*/
static void wm_hal_uart2_5_irq_handler(wm_irq_no_t irq, void *arg)
{
    for (int i = 2; i < sizeof(g_uart_dev) / sizeof(g_uart_dev[0]); i++) {
        if (g_uart_dev[i]) {
            wm_hal_uart_irq_handler(irq, g_uart_dev[i]);
        }
    }
}

static void wm_hal_uart_add_irq(wm_hal_uart_dev_t *dev, wm_irq_no_t irq_num)
{
    if (dev->port >= WM_UART_PORT_2 && dev->port < WM_UART_PORT_NUM) {
        g_irq25_num++;

        /*uart 2~5 use same irq, only process first attaching*/
        if (g_irq25_num != 1) {
            return;
        }
        wm_hal_irq_attach_sw_vector(irq_num, wm_hal_uart2_5_irq_handler, dev);
    } else {
        wm_hal_irq_attach_sw_vector(irq_num, wm_hal_uart_irq_handler, dev);
    }
    WM_HAL_UART_LOGI("attach %d\n", irq_num);

    wm_hal_irq_enable(irq_num);
    wm_hal_irq_clear_pending(irq_num);
}

static void wm_hal_uart_remove_irq(wm_hal_uart_dev_t *dev, wm_irq_no_t irq_num)
{
    if (dev->port >= WM_UART_PORT_2 && dev->port < WM_UART_PORT_NUM) {
        g_irq25_num--;
        /*uart 2~5 use same irq, do not detach if other port use irq*/
        if (g_irq25_num != 0) {
            return;
        }
    }
    WM_HAL_UART_LOGI("dettach %d\n", irq_num);
    wm_hal_irq_disable(irq_num);
    wm_hal_irq_detach_sw_vector(irq_num);
}

int wm_hal_uart_init(wm_hal_uart_dev_t *dev, wm_hal_uart_config_t *config)
{
    wm_uart_reg_t *reg = NULL;

    /*check uart dev*/
    if (!(dev && dev->port >= 0 && dev->port < WM_UART_PORT_NUM && dev->reg && dev->ref_clock > 0 && config)) {
        WM_HAL_UART_TRACE_FAIL();
        return WM_ERR_INVALID_PARAM;
    }

    /*init before*/
    if (g_uart_dev[dev->port]) {
        return WM_ERR_ALREADY_INITED;
    }

    wm_hal_uart_ctx_t *ctx = calloc(1, sizeof(*ctx));
    if (!ctx) {
        return WM_ERR_NO_MEM;
    }

    reg      = dev->reg;
    dev->ctx = ctx;

    /* clear registers*/
    wm_ll_uart_set_line_ctrl(reg, 0);
    wm_ll_uart_set_flow_ctrl(reg, 0);
    wm_ll_uart_set_dma_ctrl(reg, 0);
    wm_ll_uart_set_fifo_ctrl(reg, 0);

    /* clear and enable all interrupt */
    wm_ll_uart_set_int_source(reg, 0xFFFFFFFF);

    /* set user configs */
    if (wm_hal_uart_set_config(dev, config) != WM_ERR_SUCCESS) {
        WM_HAL_UART_TRACE_FAIL();
        free(ctx);
        return WM_ERR_INVALID_PARAM;
    }

    /* rx dma ctrl */
    wm_ll_uart_set_rxfifo_timeout(reg, 4); /*4 package time*/
    wm_ll_uart_enable_rxfifo_timeout(reg, 1);

    /* fifo ctrl: tx fifo and rx */
    wm_ll_uart_set_txfifo_trigger_level(reg, TX_FIFO_TIRGGER_LEVLE_4BYTES);
    wm_ll_uart_set_rxfifo_trigger_level(reg, RX_FIFO_TIRGGER_LEVLE_16BYTES);
    wm_ll_uart_reset_tx_fifo(reg);
    wm_ll_uart_reset_rx_fifo(reg);

    /* line ctrl : enable rx tx */
    wm_ll_uart_enable_rx(reg, 0);
    wm_ll_uart_enable_tx(reg, 0);
    wm_ll_uart_enable_rx(reg, 1);
    wm_ll_uart_enable_tx(reg, 1);

    /*enable rx timeout irq*/
    wm_ll_uart_enable_irq(reg, WM_LL_UART_RX_DATA_TIMEOUT);

    /*register uart irq*/
    wm_hal_uart_add_irq(dev, dev->irq_num);

    g_uart_dev[dev->port] = dev;

    WM_HAL_UART_LOGI("hal uart init ok!\n");

    return WM_ERR_SUCCESS;
}

int wm_hal_uart_deinit(wm_hal_uart_dev_t *dev)
{
    int ret = WM_ERR_INVALID_PARAM;

    if (!dev) {
        return ret;
    }

    if (dev->ctx) {
        wm_hal_uart_remove_irq(dev, dev->irq_num);

        /*disable all irq*/
        wm_ll_uart_disable_irq(dev->reg, WM_HAL_UART_ALL_IRQ_MASK);

        if (dev->rx_dma) {
            g_dma_port = -1;
            wm_hal_uart_stop_rx_dma(dev);
        }

        if (dev->tx_dma) {
            g_dma_port = -1;
            wm_hal_uart_stop_tx_dma(dev);
        }

        wm_ll_uart_enable_tx(dev->reg, 0);
        wm_ll_uart_enable_rx(dev->reg, 0);

        if (dev->ctx) {
            free(dev->ctx);
            dev->ctx = NULL;
        }

        g_uart_dev[dev->port] = NULL;
        ret                   = WM_ERR_SUCCESS;
    } else {
        ret = WM_ERR_FAILED;
    }

    return ret;
}

static void wm_hal_uart_init_dma_desc(wm_hal_dma_desc_t *desc, uint32_t uart_ch, uint32_t dma_ch, uint32_t src, uint32_t dst,
                                      uint32_t len, int rx_chain)
{
    memset(desc, 0, sizeof(*desc));

    desc->src_addr  = src;
    desc->dest_addr = dst;

    if (rx_chain) {
        /*rx chain*/
        desc->ctrl.chain.len           = len;
        desc->ctrl.chain.src_addr_inc  = WM_DMA_ADDR_FIXED;
        desc->ctrl.chain.dest_addr_inc = WM_DMA_ADDR_INC;
        desc->ctrl.chain.data_unit     = WM_DMA_XFER_UNIT_BYTE;
        desc->ctrl.chain.burst_size    = WM_DMA_BURST_1UNIT;
    } else {
        /*tx non chain*/
        desc->ctrl.nonchain.len           = len;
        desc->ctrl.nonchain.auto_reload   = WM_DMA_RELOAD_DISABLE;
        desc->ctrl.nonchain.src_addr_inc  = WM_DMA_ADDR_INC;
        desc->ctrl.nonchain.dest_addr_inc = WM_DMA_ADDR_FIXED;
        desc->ctrl.nonchain.data_unit     = WM_DMA_XFER_UNIT_BYTE;
        desc->ctrl.nonchain.burst_size    = WM_DMA_BURST_1UNIT;
    }

    desc->extend_ctrl.ch           = dma_ch;
    desc->extend_ctrl.int_en       = WM_DMA_CH_EN_CTRL_ENABLE;
    desc->extend_ctrl.int_type     = WM_DMA_INT_XFER_DONE;
    desc->extend_ctrl.mode         = WM_DMA_HW_MODE;
    desc->extend_ctrl.uart_dma_ch  = uart_ch;
    desc->extend_ctrl.warp_mode_en = WM_DMA_WARP_CTRL_DISABLE;

    if (rx_chain) {
        desc->extend_ctrl.req_sel       = WM_DMA_UART_RX_REQ;
        desc->extend_ctrl.chain_mode    = WM_DMA_CHAIN_MODE_LIST;
        desc->extend_ctrl.chain_mode_en = WM_DMA_CHAIN_MODE_ENABLE;
    } else {
        desc->extend_ctrl.req_sel    = WM_DMA_UART_TX_REQ;
        desc->extend_ctrl.chain_mode = WM_DMA_CHAIN_MODE_NORMAL;
    }
}

static int wm_hal_uart_rx_init_chain(wm_hal_uart_dev_t *dev)
{
    wm_hal_uart_ctx_t *ctx = dev->ctx;
    uint32_t hw_addr       = (uint32_t)wm_ll_uart_get_rx_window_addr(dev->reg);
    wm_hal_dma_desc_t *p   = &ctx->rx_desc[0];

    ctx->w = p;
    ctx->r = p;

    for (int i = 0; i < WM_HAL_RX_RECV_BUF_NUM; i++) {
        WM_HAL_UART_LOGI("hal init rx chain=%p,rx_dma_ch=%d\n", &p[i], dev->rx_dma_ch);
        wm_hal_uart_init_dma_desc(&p[i], dev->port, dev->rx_dma_ch, hw_addr, 0, 0, 1);
        if (i < WM_HAL_RX_RECV_BUF_NUM - 1) {
            p[i].next = &p[i + 1];
        } else {
            p[i].next = &p[0];
        }
    }
    return WM_ERR_SUCCESS;
}

int wm_hal_uart_set_dma(wm_hal_uart_dev_t *dev, wm_hal_uart_dma_t *dma)
{
    int ret = WM_ERR_INVALID_PARAM;

    if (dev && dma) {
        /*WM800 only one uart port can use DMA*/
        if (g_dma_port == -1) {
            dev->rx_dma    = dma->rx_dma;
            dev->tx_dma    = dma->tx_dma;
            dev->rx_dma_ch = dma->rx_dma_ch;
            dev->tx_dma_ch = dma->tx_dma_ch;
            g_dma_port     = dev->port;
            ret            = WM_ERR_SUCCESS;
        } else {
            ret = WM_ERR_FAILED;
        }
    }

    return ret;
}

static void wm_hal_uart_stop_tx_dma(wm_hal_uart_dev_t *dev)
{
    wm_hal_dma_stop(dev->tx_dma, dev->tx_dma_ch);
    wm_ll_uart_enable_tx_dma(dev->reg, 0);
    wm_hal_dma_unregister_callback(dev->tx_dma, dev->tx_dma_ch);

    ((wm_hal_uart_ctx_t *)dev)->tx_size = 0;
}

static void wm_hal_uart_dma_tx_done_callback(wm_dma_ch_t ch, uint32_t sts, void *user_data)
{
    wm_hal_uart_dev_t *dev = user_data;
    wm_hal_uart_ctx_t *ctx = dev->ctx;
    wm_hal_uart_evt_t evt  = {
         .type        = WM_HAL_UART_TX_DONE,
         .tx_data_len = ctx->tx_size,
    };

    /*clear tx xfer*/
    ctx->tx_size = 0;

    /*notify user*/
    if (ctx->tx_callback) {
        ctx->tx_callback(dev, &evt, ctx->tx_user_data);
    }
}

static void wm_hal_uart_dma_rx_done_callback(wm_dma_ch_t ch, uint32_t sts, void *user_data)
{
    wm_hal_uart_dev_t *dev  = user_data;
    wm_hal_uart_ctx_t *ctx  = dev->ctx;
    wm_hal_dma_desc_t *read = ctx->r;
    wm_hal_uart_evt_t evt;

    while (!read->vld && read->dest_addr) {
        evt.type            = WM_HAL_UART_RX_READY;
        evt.rx_timeout_flag = 0;
        evt.rx_data         = ((uint8_t *)(read->dest_addr)) + ctx->rx_timeout_offset;
        evt.rx_len          = read->ctrl.chain.len - ctx->rx_timeout_offset;

#ifdef CONFIG_UART_STATISTICS_ENABLE
        ctx->recv_cnt += evt.rx_len;
#endif

        WM_HAL_UART_LOGE("HAL DMA done: node_size=%u,off=%u,evt.rx_len=%d\n", read->ctrl.chain.len, ctx->rx_timeout_offset,
                         evt.rx_len);

        /*reset timeout offset*/
        ctx->rx_timeout_offset = 0;

        /*remove buffer*/
        read->dest_addr = 0;

        /*In extreme cases, receiving two blocks will be interrupted once,so need process next block*/
        read = read->next;

        if (ctx->rx_callback) {
            /*notify use rx ready*/
            ctx->rx_callback(dev, &evt, ctx->rx_user_data);
        }

        WM_HAL_UART_LOGE("HAL DMA go to next\n");
    }
    ctx->r = read;
}

static int wm_hal_uart_start_rx_dma(wm_hal_uart_dev_t *dev, wm_hal_dma_desc_t *tail)
{
    wm_hal_dma_register_callback(dev->rx_dma, dev->rx_dma_ch, wm_hal_uart_dma_rx_done_callback, dev);
    wm_hal_dma_config(dev->rx_dma, tail);

    /*need restart*/
    wm_ll_uart_set_rxfifo_trigger_level(dev->reg, 0); /**< 1 bytes         */
    wm_ll_uart_enable_irq(dev->reg, WM_HAL_RX_ERR_MASKS | WM_LL_UART_RX_DATA_TIMEOUT);

    /*Close rx to ensure that DMA is started successfully*/
    wm_ll_uart_enable_rx_dma(dev->reg, 0); /**< disable uart rx dma */
    wm_ll_uart_enable_rx(dev->reg, 0);     /**< disable uart rx */

    /*clear rxfifo, otherwise the dma may not work*/
    wm_ll_uart_reset_rx_fifo(dev->reg);

    /*W800 need clear dam reqest, Otherwise, an extra byte may be received*/
    wm_hal_dma_clr_req(dev->rx_dma, WM_DMA_UART_RX_REQ);

    /*start dma*/
    wm_hal_dma_start(dev->rx_dma, dev->rx_dma_ch);

    wm_ll_uart_enable_rx_dma(dev->reg, 1); /**< enable uart rx dma */
    wm_ll_uart_enable_rx(dev->reg, 1);     /**< enable uart rx     */

    return WM_ERR_SUCCESS;
}

static void wm_hal_uart_stop_rx_dma(wm_hal_uart_dev_t *dev)
{
    /*stop rx dma*/
    wm_hal_dma_stop(dev->rx_dma, dev->rx_dma_ch);
    wm_ll_uart_enable_rx_dma(dev->reg, 0);
    wm_hal_dma_unregister_callback(dev->rx_dma, dev->rx_dma_ch);
}

void wm_hal_uart_get_status(wm_hal_uart_dev_t *dev)
{
    wm_dma_sts_info_t status;

    wm_hal_dma_get_status(dev->rx_dma, dev->rx_dma_ch, &status);

    WM_HAL_UART_LOGI("HAL RFO: offset=%d,xfer_cnt=%d,fifo_cnt=%d,dma_run=%d\n",
                     ((wm_hal_uart_ctx_t *)dev->ctx)->rx_timeout_offset, status.xfer_cnt, wm_ll_uart_get_rxfifo_count(dev->reg),
                     status.sts);
}

static void wm_hal_uart_dma_rx_timeout(wm_hal_uart_dev_t *dev)
{
    wm_hal_uart_ctx_t *ctx  = dev->ctx;
    wm_hal_dma_desc_t *read = ctx->r;
    wm_hal_uart_evt_t evt;
    wm_dma_sts_info_t status;

    /*get dma receive cnt int current buffer.*/
    wm_hal_dma_get_status(dev->rx_dma, dev->rx_dma_ch, &status);

    WM_HAL_UART_LOGI("HAL RFO: pre_offset=%d,xfer_cnt=%d,chain_size=%d\n", ctx->rx_timeout_offset, status.xfer_cnt,
                     read->ctrl.chain.len);

    if (read->vld && read->dest_addr) {
        evt.type            = WM_HAL_UART_RX_READY;
        evt.rx_timeout_flag = 1;
        evt.rx_data         = ((uint8_t *)(read->dest_addr)) + ctx->rx_timeout_offset;
        evt.rx_len          = status.xfer_cnt - ctx->rx_timeout_offset;

        if (evt.rx_len >= 0) {
#ifdef CONFIG_UART_STATISTICS_ENABLE
            ctx->recv_cnt += evt.rx_len;
#endif
            WM_HAL_UART_LOGI("HAL RFO: buf=%p,offset=%d,len=%d,fifo_cnt=%d,dma_run=%d\n", (void *)read->dest_addr,
                             ctx->rx_timeout_offset, evt.rx_len, wm_ll_uart_get_rxfifo_count(dev->reg), status.sts);

            /*update new timeout offset*/
            ctx->rx_timeout_offset = status.xfer_cnt;

            /*notify user rx ready*/
            if (ctx->rx_callback) {
                ctx->rx_callback(dev, &evt, ctx->rx_user_data);
            }
        }
    } else {
        WM_HAL_UART_TRACE_FAIL();
    }
}

static void wm_hal_uart_rx_it_receive(wm_hal_uart_dev_t *dev, uint32_t rx_fifo_timeout)
{
    wm_hal_uart_ctx_t *ctx  = dev->ctx;
    wm_hal_dma_desc_t *read = ctx->r;
    wm_hal_uart_evt_t evt   = { .type = WM_HAL_UART_RX_READY, .rx_timeout_flag = 1 };
    uint8_t *pdata;

    int fifo_cnt = wm_ll_uart_get_rxfifo_count(dev->reg);
    int tmp_cnt  = fifo_cnt;

    while (read->vld && read->dest_addr && fifo_cnt > 0) {
        pdata = (uint8_t *)read->dest_addr;

        while (ctx->rx_timeout_offset < read->ctrl.chain.len && fifo_cnt > 0) {
            pdata[ctx->rx_timeout_offset++] = wm_ll_uart_getchar(dev->reg);
            fifo_cnt--;
        }

        /*current node end.*/
        evt.rx_data = pdata + ctx->rx_read_offset;
        evt.rx_len  = ctx->rx_timeout_offset - ctx->rx_read_offset;

        if (ctx->rx_timeout_offset == read->ctrl.chain.len) {
            WM_HAL_UART_LOGI("--receive node done,offset = %u,len=%d\n", ctx->rx_read_offset, evt.rx_len);

            /*receive end.*/
            ctx->rx_timeout_offset = 0;
            ctx->rx_read_offset    = 0;

            read->vld       = 0;
            read->dest_addr = 0;

            /*go to next block*/
            read = read->next;

#ifdef CONFIG_UART_STATISTICS_ENABLE
            ctx->recv_cnt += evt.rx_len;
#endif
            /*notify user rx ready*/
            if (ctx->rx_callback) {
                ctx->rx_callback(dev, &evt, ctx->rx_user_data);
            }
        } else if (rx_fifo_timeout) {
            /*current node not end.*/

            WM_HAL_UART_LOGI("--receive timeout,offset = %u,len=%d\n", ctx->rx_read_offset, evt.rx_len);
            /*notify user rx ready*/
#ifdef CONFIG_UART_STATISTICS_ENABLE
            ctx->recv_cnt += evt.rx_len;
#endif
            ctx->rx_read_offset = ctx->rx_timeout_offset;
            if (ctx->rx_callback) {
                ctx->rx_callback(dev, &evt, ctx->rx_user_data);
            }
        } else {
            WM_HAL_UART_LOGI("--receive ... ,offset = %u,len=%d\n", ctx->rx_read_offset, evt.rx_len);
        }
    }

    /*timeout come late, notify pre receiving data.*/
    if (tmp_cnt == 0 && rx_fifo_timeout && ctx->rx_timeout_offset > ctx->rx_read_offset) {
        /*current node end.*/
        evt.rx_data = ((uint8_t *)read->dest_addr) + ctx->rx_read_offset;
        evt.rx_len  = ctx->rx_timeout_offset - ctx->rx_read_offset;

        WM_HAL_UART_LOGI("--receive 0, timeout ... ,offset = %u,len=%d\n", ctx->rx_read_offset, evt.rx_len);
#ifdef CONFIG_UART_STATISTICS_ENABLE
        ctx->recv_cnt += evt.rx_len;
#endif
        ctx->rx_read_offset = ctx->rx_timeout_offset;
        if (ctx->rx_callback) {
            ctx->rx_callback(dev, &evt, ctx->rx_user_data);
        }
    }

    WM_HAL_UART_LOGI("== it reaceive end: %d %d\n", fifo_cnt, tmp_cnt);
    if (fifo_cnt > 0 && fifo_cnt == tmp_cnt) {
        /*buffer is full*/
        WM_HAL_UART_LOGE("buffer is full,disable rx irq\n");
        wm_ll_uart_disable_irq(dev->reg, WM_HAL_RX_IT_MASKS);
#ifdef CONFIG_UART_STATISTICS_ENABLE
        ctx->error[WM_HAL_UART_STATISTICS_BUFFER_FULL]++;
        ctx->error[WM_HAL_UART_STATISTICS_RECV_RST]++;
#endif
    }

    ctx->r = read;
}

void wm_hal_uart_dma_rx_fifo_overrun_err(wm_hal_uart_dev_t *dev)
{
    wm_hal_uart_ctx_t *ctx = dev->ctx;
    wm_dma_sts_info_t status;
    int fifo_cnt = wm_ll_uart_get_rxfifo_count(dev->reg);
    /*get dma receive cnt int current buffer.*/
    wm_hal_dma_get_status(dev->rx_dma, dev->rx_dma_ch, &status);

    if (status.sts == WM_DMA_RUNNIG && fifo_cnt > WM_UART_HW_FIFO_NUM / 2) {
        WM_HAL_UART_LOGE("dma_status=%d, rx_fifo_cnt=%d, restart!\n", status.sts, fifo_cnt);
        ctx->rx_timeout_offset = 0;
        wm_hal_dma_stop(dev->rx_dma, dev->rx_dma_ch);
        wm_hal_uart_start_rx_dma(dev, ctx->r);
    }
}

int wm_hal_uart_control(wm_hal_uart_dev_t *dev, int cmd, uint32_t param, void *arg)
{
    WM_HAL_UART_CHECK_PARAM(dev);

    return WM_ERR_NO_SUPPORT;
}

int wm_hal_uart_putchar(wm_hal_uart_dev_t *dev, int ch)
{
    WM_HAL_UART_CHECK_PARAM(dev);

    while (wm_ll_uart_get_txfifo_count(dev->reg)) {
        ;
    }
    wm_ll_uart_putchar(dev->reg, (unsigned char)ch);

    return WM_ERR_SUCCESS;
}

int wm_hal_uart_getchar(wm_hal_uart_dev_t *dev)
{
    WM_HAL_UART_CHECK_PARAM(dev);

    while (!wm_ll_uart_get_rxfifo_count(dev->reg)) {
        ;
    }

    return (unsigned char)wm_ll_uart_getchar(dev->reg);
}

int wm_hal_uart_tx_polling(wm_hal_uart_dev_t *dev, const uint8_t *data, uint32_t size)
{
    uint32_t count = size;

    WM_HAL_UART_CHECK_PARAM(dev && data && size > 0);

    while (count--) {
        while (wm_ll_uart_get_txfifo_count(dev->reg)) {
            ;
        }
        wm_ll_uart_putchar(dev->reg, *data++);
    }

#ifdef CONFIG_UART_STATISTICS_ENABLE
    wm_hal_uart_ctx_t *ctx = dev->ctx;
    if (ctx) {
        ctx->send_cnt += size;
    }
#endif

    return size;
}

int wm_hal_uart_rx_polling(wm_hal_uart_dev_t *dev, uint8_t *buffer, uint32_t size, uint32_t timeout_ms)
{
    wm_uart_reg_t *reg;
    int count;
    wm_hal_uart_ctx_t *ctx;

    WM_HAL_UART_CHECK_PARAM(dev && dev->ctx && buffer && size > 0);

    ctx               = dev->ctx;
    reg               = dev->reg;
    ctx->timeout_flag = 0;
    count             = 0;

    //TODO process timeout_ms
    while (count < size) {
        /*Wait rx data*/
        while (!wm_ll_uart_get_rxfifo_count(reg)) {
            if (ctx->timeout_flag) {
                /*End of sending by the other party. Therefore, this reception is stopped.*/
                break;
            }
        }

        if (ctx->timeout_flag) {
            break;
        }
        buffer[count++] = wm_ll_uart_getchar(reg);
    }

    return count;
}

int wm_hal_uart_tx_it(wm_hal_uart_dev_t *dev, const uint8_t *data, uint32_t size)
{
    int ret;
    wm_hal_uart_ctx_t *ctx;

    WM_HAL_UART_CHECK_PARAM(dev && dev->ctx && data && size > 0);

    ctx = dev->ctx;
    if (ctx->tx_size > 0) {
        /*tx is using*/
        WM_HAL_UART_TRACE_FAIL();
        ret = WM_ERR_FAILED;
    } else {
        ctx->tx_data   = (uint8_t *)data;
        ctx->tx_size   = size;
        ctx->tx_offset = 0;

        WM_HAL_UART_LOGI("start tx_it,size=%d\n", (int)size);

        int tx_fifo_free = WM_UART_HW_FIFO_NUM - wm_ll_uart_get_txfifo_count(dev->reg);

        if (tx_fifo_free > size) {
            tx_fifo_free = size;
        }

#ifdef CONFIG_UART_STATISTICS_ENABLE
        ctx->send_cnt += tx_fifo_free;
#endif

        /*disable tx irq, for not breaking the following write block.*/
        wm_ll_uart_disable_irq(dev->reg, WM_HAL_TX_IT_MASKS);
        while (ctx->tx_offset < tx_fifo_free) {
            wm_ll_uart_putchar(dev->reg, data[ctx->tx_offset++]);
        }
        /*enable tx irq , write other data in interrupt.*/
        wm_ll_uart_enable_irq(dev->reg, WM_HAL_TX_IT_MASKS);

        ret = WM_ERR_SUCCESS;
    }

    return ret;
}

int wm_hal_uart_rx_it(wm_hal_uart_dev_t *dev, uint8_t *buffer, uint32_t size)
{
    wm_hal_uart_ctx_t *ctx;
    wm_hal_dma_desc_t *tail;

    WM_HAL_UART_CHECK_PARAM(dev && dev->ctx && buffer && size > 0);

    ctx = dev->ctx;

    if (!ctx->w) {
        /*first use, init chain*/
        wm_hal_uart_rx_init_chain(dev);
    }

    tail = ctx->w;

    if (tail->dest_addr) {
        /*Can't append any more.*/
        WM_HAL_UART_LOGI("Can't append. chain=%p\n", tail);
        return WM_ERR_FAILED;
    }

    /*set address and size*/
    tail->ctrl.chain.len = size;
    tail->dest_addr      = (uint32_t)buffer;
    tail->vld            = WM_UART_DMA_STATUS_BUF_EMPTY; /**< enable receiving */
    ctx->w               = tail->next;

    wm_ll_uart_enable_irq(dev->reg, WM_HAL_RX_ERR_MASKS | WM_HAL_RX_IT_MASKS);
    WM_HAL_UART_LOGI("append buf, chain=%p,len=%u,addr=%x,next=%p\n", tail, tail->ctrl.chain.len, tail->dest_addr, tail->next);

    return WM_ERR_SUCCESS;
}

int wm_hal_uart_tx_dma(wm_hal_uart_dev_t *dev, const uint8_t *data, uint32_t size)
{
    int ret;

    wm_hal_uart_ctx_t *ctx;
    wm_hal_dma_desc_t desc;
    uint32_t hw_addr;

    WM_HAL_UART_CHECK_PARAM(dev && dev->ctx && data && size > 0 && dev->tx_dma);

    ctx = dev->ctx;
    if (ctx->tx_size > 0) {
        /*tx is using*/
        WM_HAL_UART_TRACE_FAIL();
        ret = WM_ERR_FAILED;
    } else {
        ctx->tx_size = size;

        hw_addr = (uint32_t)wm_ll_uart_get_tx_window_addr(dev->reg);

        wm_hal_uart_init_dma_desc(&desc, dev->port, dev->tx_dma_ch, (uint32_t)data, hw_addr, size, 0);
        wm_hal_dma_register_callback(dev->tx_dma, dev->tx_dma_ch, wm_hal_uart_dma_tx_done_callback, dev);
        wm_hal_dma_config(dev->tx_dma, &desc);
        wm_ll_uart_enable_tx_dma(dev->reg, 1);
        wm_hal_dma_start(dev->tx_dma, dev->tx_dma_ch);

#ifdef CONFIG_UART_STATISTICS_ENABLE
        ctx->send_cnt += size;
#endif
        ret = WM_ERR_SUCCESS;
    }

    return ret;
}

int wm_hal_uart_rx_dma(wm_hal_uart_dev_t *dev, uint8_t *buffer, uint32_t size)
{
    wm_hal_uart_ctx_t *ctx;
    wm_hal_dma_desc_t *tail;
    wm_dma_sts_info_t status;

    WM_HAL_UART_CHECK_PARAM(dev && dev->ctx && buffer && size > 0 && dev->rx_dma);

    ctx = dev->ctx;

    if (!ctx->w) {
        /*first use, init chain*/
        wm_hal_uart_rx_init_chain(dev);
    }

    tail = ctx->w;

    if (tail->dest_addr) {
        /*Can't append any more.*/
        WM_HAL_UART_LOGI("Can't append. chain=%p\n", tail);
        return WM_ERR_FAILED;
    }

    /*set address and size*/
    tail->ctrl.chain.len = size;
    tail->dest_addr      = (uint32_t)buffer;
    tail->vld            = WM_UART_DMA_STATUS_BUF_EMPTY; /**< enable dma receiving */
    ctx->w               = tail->next;

    WM_HAL_UART_LOGI("append buf, chain=%p,len=%u,addr=%x,next=%p\n", tail, tail->ctrl.chain.len, tail->dest_addr, tail->next);

    /*check and start dma*/
    wm_hal_dma_get_status(dev->rx_dma, dev->rx_dma_ch, &status);
    if (status.sts != WM_DMA_RUNNIG) {
        wm_hal_uart_start_rx_dma(dev, tail);
#ifdef CONFIG_UART_STATISTICS_ENABLE
        ctx->error[WM_HAL_UART_STATISTICS_RECV_RST]++;
#endif
    }

    return WM_ERR_SUCCESS;
}

int wm_hal_uart_register_tx_callback(wm_hal_uart_dev_t *dev, wm_hal_uart_callback_t user_callback, void *user_data)
{
    wm_hal_uart_ctx_t *ctx;

    WM_HAL_UART_CHECK_PARAM(dev && dev->ctx);

    ctx               = dev->ctx;
    ctx->tx_callback  = user_callback;
    ctx->tx_user_data = user_data;

    return WM_ERR_SUCCESS;
}

int wm_hal_uart_register_rx_callback(wm_hal_uart_dev_t *dev, wm_hal_uart_callback_t user_callback, void *user_data)
{
    wm_hal_uart_ctx_t *ctx;

    WM_HAL_UART_CHECK_PARAM(dev && dev->ctx);

    ctx               = dev->ctx;
    ctx->rx_callback  = user_callback;
    ctx->rx_user_data = user_data;

    return WM_ERR_SUCCESS;
}

int wm_hal_uart_set_config(wm_hal_uart_dev_t *dev, wm_hal_uart_config_t *config)
{
    /*set baudrate,data bits, stop bits, parity, flow ctrl*/

    WM_HAL_UART_CHECK_PARAM(dev && dev->ctx && config);

    /*check data bits*/
    if (!(config->data_bits >= 0 && config->data_bits < WM_UART_DATA_BIT_MAX)) {
        WM_HAL_UART_TRACE_FAIL();
        return WM_ERR_INVALID_PARAM;
    }

    /*check stop bits*/
    if (!(config->stop_bits >= 0 && config->stop_bits < WM_UART_STOP_BIT_MAX)) {
        WM_HAL_UART_TRACE_FAIL();
        return WM_ERR_INVALID_PARAM;
    }

    /*check parity*/
    if (!(config->parity >= WM_UART_PARITY_NONE && config->parity < WM_UART_PARITY_MAX)) {
        WM_HAL_UART_TRACE_FAIL();
        return WM_ERR_INVALID_PARAM;
    }

    /*check flow ctrl*/
    if (!(config->flow_ctrl >= 0 && config->flow_ctrl < WM_UART_FLOW_CTRL_MAX)) {
        WM_HAL_UART_TRACE_FAIL();
        return WM_ERR_INVALID_PARAM;
    }

    /*check baudrate*/
    if (wm_hal_uart_check_baudrate(config->baudrate) != WM_ERR_SUCCESS) {
        WM_HAL_UART_TRACE_FAIL();
        return WM_ERR_INVALID_PARAM;
    }

    /*set config*/
    wm_ll_uart_set_baudrate(dev->reg, WM_HAL_UART_CAL_BAUDRATE(dev->ref_clock, config->baudrate));
    wm_ll_uart_set_data_bits(dev->reg, (wm_uart_data_bits_t)config->data_bits);
    wm_ll_uart_set_stop_bits(dev->reg, (wm_uart_data_bits_t)config->stop_bits);
    wm_hal_uart_set_parity(dev, config->parity);
    wm_hal_uart_set_flow_ctrl(dev, config->flow_ctrl);

    ((wm_hal_uart_ctx_t *)dev->ctx)->config = *config;

    return WM_ERR_SUCCESS;
}

int wm_hal_uart_get_config(wm_hal_uart_dev_t *dev, wm_hal_uart_config_t *config)
{
    WM_HAL_UART_CHECK_PARAM(dev && dev->ctx && config);

    *config = ((wm_hal_uart_ctx_t *)dev->ctx)->config;

    return WM_ERR_SUCCESS;
}

int wm_hal_uart_set_baudrate(wm_hal_uart_dev_t *dev, uint32_t baudrate)
{
    WM_HAL_UART_CHECK_PARAM(dev && dev->ctx);

    /*check baudrate*/
    if (wm_hal_uart_check_baudrate(baudrate) != WM_ERR_SUCCESS) {
        WM_HAL_UART_TRACE_FAIL();
        return WM_ERR_INVALID_PARAM;
    }

    wm_ll_uart_set_baudrate(dev->reg, WM_HAL_UART_CAL_BAUDRATE(dev->ref_clock, baudrate));

    ((wm_hal_uart_ctx_t *)dev->ctx)->config.baudrate = baudrate;

    return WM_ERR_SUCCESS;
}

int wm_hal_uart_get_baudrate(wm_hal_uart_dev_t *dev, uint32_t *baudrate)
{
    WM_HAL_UART_CHECK_PARAM(dev && dev->ctx && baudrate);

    *baudrate = ((wm_hal_uart_ctx_t *)dev->ctx)->config.baudrate;

    return WM_ERR_SUCCESS;
}

int wm_hal_uart_set_data_bits(wm_hal_uart_dev_t *dev, wm_uart_data_bits_t data_bits)
{
    WM_HAL_UART_CHECK_PARAM(dev && dev->ctx && data_bits == WM_UART_DATA_BIT_8);

    wm_ll_uart_set_data_bits(dev->reg, (wm_uart_data_bits_t)data_bits);

    ((wm_hal_uart_ctx_t *)dev->ctx)->config.data_bits = data_bits;

    return WM_ERR_SUCCESS;
}

int wm_hal_uart_get_data_bits(wm_hal_uart_dev_t *dev, wm_uart_data_bits_t *data_bits)
{
    WM_HAL_UART_CHECK_PARAM(dev && dev->ctx && data_bits);

    *data_bits = ((wm_hal_uart_ctx_t *)dev->ctx)->config.data_bits;

    return WM_ERR_SUCCESS;
}

int wm_hal_uart_set_stop_bits(wm_hal_uart_dev_t *dev, wm_uart_stop_bits_t stop_bits)
{
    WM_HAL_UART_CHECK_PARAM(dev && dev->ctx && stop_bits >= WM_UART_STOP_BIT_1 && stop_bits < WM_UART_STOP_BIT_MAX);

    wm_ll_uart_set_stop_bits(dev->reg, (wm_uart_stop_bits_t)stop_bits);

    ((wm_hal_uart_ctx_t *)dev->ctx)->config.stop_bits = stop_bits;

    return WM_ERR_SUCCESS;
}

int wm_hal_uart_get_stop_bits(wm_hal_uart_dev_t *dev, wm_uart_stop_bits_t *stop_bits)
{
    WM_HAL_UART_CHECK_PARAM(dev && dev->ctx && stop_bits);

    *stop_bits = ((wm_hal_uart_ctx_t *)dev->ctx)->config.stop_bits;

    return WM_ERR_SUCCESS;
}

int wm_hal_uart_set_parity(wm_hal_uart_dev_t *dev, wm_uart_parity_t parity)
{
    WM_HAL_UART_CHECK_PARAM(dev && dev->ctx && parity >= WM_UART_PARITY_NONE && parity < WM_UART_PARITY_MAX);

    if (parity == WM_UART_PARITY_NONE) {
        /*Disable parity*/
        wm_ll_uart_enable_parity(dev->reg, 0);
    } else {
        /*enable parity*/
        wm_ll_uart_enable_parity(dev->reg, 1);
        wm_ll_uart_set_parity(dev->reg, parity);
    }
    ((wm_hal_uart_ctx_t *)dev->ctx)->config.parity = parity;
    return WM_ERR_SUCCESS;
}

int wm_hal_uart_get_parity(wm_hal_uart_dev_t *dev, wm_uart_parity_t *parity)
{
    WM_HAL_UART_CHECK_PARAM(dev && dev->ctx && parity);

    *parity = ((wm_hal_uart_ctx_t *)dev->ctx)->config.parity;

    return WM_ERR_SUCCESS;
}

int wm_hal_uart_set_flow_ctrl(wm_hal_uart_dev_t *dev, wm_uart_flowctrl_t flow_ctrl)
{
    WM_HAL_UART_CHECK_PARAM(dev && dev->ctx && flow_ctrl >= WM_UART_FLOW_CTRL_DISABLE && flow_ctrl < WM_UART_FLOW_CTRL_MAX);

    if (flow_ctrl == WM_UART_FLOW_CTRL_DISABLE) {
        /*Disable RTS*/
        wm_ll_uart_set_flow_ctrl(dev->reg, 0);
    } else if (flow_ctrl & WM_UART_FLOW_CTRL_RTS) {
        /*enable RTS*/
        wm_ll_uart_set_rts_trig_level(dev->reg, 6);
        wm_ll_uart_enable_afc(dev->reg, 1);
    }

    WM_HAL_UART_LOGI("flow ctrl=%d\n", flow_ctrl);

    if (flow_ctrl & WM_UART_FLOW_CTRL_CTS) {
        /*Disable CTS*/
        wm_ll_uart_enable_irq(dev->reg, WM_LL_UART_CTS_CHANGE);
    } else {
        /*enable CTS*/
        wm_ll_uart_disable_irq(dev->reg, WM_LL_UART_CTS_CHANGE);
        wm_ll_uart_enable_tx(dev->reg, 1); /*maybe tx disabled by cts change in isr*/
    }

    ((wm_hal_uart_ctx_t *)dev->ctx)->config.flow_ctrl = flow_ctrl;

    return WM_ERR_SUCCESS;
}

int wm_hal_uart_get_flow_ctrl(wm_hal_uart_dev_t *dev, wm_uart_flowctrl_t *flow_ctrl)
{
    WM_HAL_UART_CHECK_PARAM(dev && dev->ctx && flow_ctrl);

    *flow_ctrl = ((wm_hal_uart_ctx_t *)dev->ctx)->config.flow_ctrl;

    return WM_ERR_SUCCESS;
}

int wm_hal_uart_print_statistics(int port)
{
#ifdef CONFIG_UART_STATISTICS_ENABLE
    wm_hal_uart_ctx_t *ctx;

    WM_HAL_UART_CHECK_PARAM(port >= 0 && port < WM_UART_PORT_NUM);
    WM_HAL_UART_CHECK_PARAM(g_uart_dev[port] && g_uart_dev[port]->ctx);

    (void)ctx;

    ctx = g_uart_dev[port]->ctx;

    wm_log_info("\nuart%d,dma=%p,%p  %d,%d", port, g_uart_dev[port]->tx_dma, g_uart_dev[port]->rx_dma,
                g_uart_dev[port]->tx_dma_ch, g_uart_dev[port]->rx_dma_ch);

    wm_log_info("send_cnt=%llu", ctx->send_cnt);
    wm_log_info("recv_cnt=%llu", ctx->recv_cnt);

    wm_log_info("break detect=%u", ctx->error[WM_HAL_UART_STATISTICS_BREAK]);
    wm_log_info("frame_err=%u", ctx->error[WM_HAL_UART_STATISTICS_FRAME_ERR]);
    wm_log_info("parity_err=%u", ctx->error[WM_HAL_UART_STATISTICS_PARITY_ERR]);
    wm_log_info("fifo_overun=%u", ctx->error[WM_HAL_UART_STATISTICS_OVERRUN]);
    wm_log_info("recv reset=%u", ctx->error[WM_HAL_UART_STATISTICS_RECV_RST]);
    wm_log_info("buffer_full=%u", ctx->error[WM_HAL_UART_STATISTICS_BUFFER_FULL]);
#endif
    return WM_ERR_SUCCESS;
}
