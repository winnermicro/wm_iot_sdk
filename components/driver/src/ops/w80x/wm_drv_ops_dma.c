/**
  * @file wm_drv_ops_dma.c
  *
  * @brief dma drv api
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
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "wm_error.h"
#include "wm_drv_dma.h"
#include "wm_drv_rcc.h"
#include "wm_dt_hw.h"
#include "wm_dt.h"
#include "wm_soc_cfgs.h"
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
#define WM_DRV_DMA_FUNC_NAME      __func__
#define WM_DRV_DMA_DUMP_WIDTH     (0xf)

#define WM_DRV_DMA_BYTE_ALIGNMENT 4
#define WM_DRV_DMA_BURST_MAX      4
#define WM_DRV_DMA_ADDR_IS_PSRAM(addr) \
    (addr >= CONFIG_PSRAM_BASE_ADDR && addr < (CONFIG_PSRAM_BASE_ADDR + CONFIG_PSRAM_MAX_LENGTH))
#define WM_DRV_DMA_ADDR_IS_SRAM(addr) \
    (addr >= CONFIG_SRAM_BASE_ADDR && addr < (CONFIG_SRAM_BASE_ADDR + CONFIG_PSRAM_MAX_LENGTH))
#define WM_DRV_DMA_ADDR_IS_DRAM(addr) \
    (addr >= CONFIG_DRAM_BASE_ADDR && addr < (CONFIG_DRAM_BASE_ADDR + CONFIG_PSRAM_MAX_LENGTH))
#define WM_DRV_DMA_ADDR_IS_UART(addr) \
    (addr >= CONFIG_UART_BASE_ADDR && addr < (CONFIG_UART_BASE_ADDR + CONFIG_PSRAM_MAX_LENGTH))
#define WM_DRV_DMA_ADDR_IS_MEM(addr) \
    (WM_DRV_DMA_ADDR_IS_SRAM(addr) || WM_DRV_DMA_ADDR_IS_DRAM(addr) || WM_DRV_DMA_ADDR_IS_PSRAM(addr))

WM_DRV_DMA_STATIC wm_drv_dma_status_t wm_w800_driver_dma_init(wm_device_t *dev);
WM_DRV_DMA_STATIC wm_drv_dma_status_t wm_w800_driver_dma_deinit(wm_dma_data_t *dma_data);
WM_DRV_DMA_STATIC wm_drv_dma_status_t wm_w800_driver_dma_request_ch(wm_dma_data_t *dma_data, uint8_t *ch);
WM_DRV_DMA_STATIC wm_drv_dma_status_t wm_w800_driver_dma_release_ch(wm_dma_data_t *dma_data, uint8_t ch);
WM_DRV_DMA_STATIC wm_drv_dma_status_t wm_w800_driver_dma_config(wm_dma_data_t *dma_data, wm_drv_dma_desc_t *dma_desc);
WM_DRV_DMA_STATIC wm_drv_dma_status_t wm_w800_driver_dma_config_partial(wm_dma_data_t *dma_data, wm_drv_dma_desc_t *dma_desc);
WM_DRV_DMA_STATIC wm_drv_dma_status_t wm_w800_driver_dma_start(wm_dma_data_t *dma_data, wm_dma_ch_t ch);
WM_DRV_DMA_STATIC wm_drv_dma_status_t wm_w800_driver_dma_stop(wm_dma_data_t *dma_data, wm_dma_ch_t ch);
WM_DRV_DMA_STATIC wm_drv_dma_status_t wm_w800_driver_dma_get_status(wm_dma_data_t *dma_data, wm_dma_ch_t ch,
                                                                    wm_dma_sts_info_t *dma_sts);

WM_HAL_DMA_STATIC wm_hal_dma_desc_t dma_desc_convert[WM_DRV_DMA_CH_MAX]        = { 0 };
WM_HAL_DMA_STATIC wm_dma_req_sel_t drv_dma_req_src_drv2hal[WM_DRV_DMA_REQ_MAX] = {
    WM_DMA_PWM_CAP0_REQ, WM_DMA_PWM_CAP1_REQ, WM_DMA_LSPI_RX_REQ, WM_DMA_LSPI_TX_REQ, WM_DMA_ADC_CH0_REQ, WM_DMA_ADC_CH1_REQ,
    WM_DMA_ADC_CH2_REQ,  WM_DMA_ADC_CH3_REQ,  WM_DMA_I2S_RX_REQ,  WM_DMA_I2S_TX_REQ,  WM_DMA_SDIOH_REQ,   WM_DMA_UART_TX_REQ,
    WM_DMA_UART_RX_REQ,  WM_DMA_UART_TX_REQ,  WM_DMA_UART_RX_REQ, WM_DMA_UART_TX_REQ, WM_DMA_UART_RX_REQ, WM_DMA_UART_TX_REQ,
    WM_DMA_UART_RX_REQ,  WM_DMA_UART_TX_REQ,  WM_DMA_UART_RX_REQ, WM_DMA_UART_TX_REQ, WM_DMA_UART_RX_REQ
};

WM_HAL_DMA_STATIC wm_dma_ch_uart_sel_t drv_dma_uart_ch_drv2hal[WM_DRV_DMA_CH_UART5_RX_REQ - WM_DRV_DMA_CH_UART0_TX_REQ + 1] = {
    WM_DMA_CH_UART0_SEL,      WM_DMA_CH_UART0_SEL,      WM_DMA_CH_UART1_SEL, WM_DMA_CH_UART1_SEL,
    WM_DMA_CH_UART2_7816_SEL, WM_DMA_CH_UART2_7816_SEL, WM_DMA_CH_UART3_SEL, WM_DMA_CH_UART3_SEL,
    WM_DMA_CH_UART4_SEL,      WM_DMA_CH_UART4_SEL,      WM_DMA_CH_UART5_SEL, WM_DMA_CH_UART5_SEL
};

/**
 * @struct wm_drv_dma_ops_t
 * @brief dma driver ops
 */
typedef struct {
    wm_drv_dma_status_t (*init)(wm_device_t *dev);
    wm_drv_dma_status_t (*deinit)(wm_dma_data_t *dma_data);
    wm_drv_dma_status_t (*request_ch)(wm_dma_data_t *dma_data, uint8_t *ch);
    wm_drv_dma_status_t (*release_ch)(wm_dma_data_t *dma_data, uint8_t ch);
    wm_drv_dma_status_t (*config_dma)(wm_dma_data_t *dma_data, wm_drv_dma_desc_t *dma_desc);
    wm_drv_dma_status_t (*config_dma_partial)(wm_dma_data_t *dma_data, wm_drv_dma_desc_t *dma_desc);
    wm_drv_dma_status_t (*start_dma)(wm_dma_data_t *dma_data, wm_dma_ch_t ch);
    wm_drv_dma_status_t (*stop_dma)(wm_dma_data_t *dma_data, wm_dma_ch_t ch);
    wm_drv_dma_status_t (*get_status)(wm_dma_data_t *dma_data, wm_dma_ch_t ch, wm_dma_sts_info_t *dma_sts);
    //TODO
} wm_drv_dma_ops_t;

WM_HAL_DMA_STATIC wm_hal_dma_desc_t *wm_drv_dma_get_hal_desc(uint8_t ch)
{
    if (ch >= WM_DRV_DMA_CH_MAX) {
        WM_DRV_DMA_LOG_E("get hal desc err");
        return NULL;
    }

    return &dma_desc_convert[ch];
}

WM_DRV_DMA_STATIC void wm_drv_dma_isr_entry(wm_dma_ch_t ch, uint32_t sts, void *priv)
{
    wm_device_t *dev             = (wm_device_t *)priv;
    wm_hal_dma_desc_t *desc      = NULL;
    wm_hal_dma_desc_t *desc_free = NULL;
    wm_dma_data_t *dma_data      = (wm_dma_data_t *)dev->drv;

    if (dev != NULL) {
        WM_DRV_DMA_LOG_I("dma drv irq enter for dma dev:0x%x, ch:%d", (uint32_t)dev, ch);

        /* dma done, free desc in list mode */
        desc = (wm_hal_dma_desc_t *)dma_data->drv_ctx.desc[ch];
        if (desc->extend_ctrl.chain_mode == WM_DMA_CHAIN_MODE_LIST &&
            desc->extend_ctrl.chain_mode_en == WM_DMA_CHAIN_MODE_ENABLE) {
            while (desc != NULL) {
                desc_free = desc;
                desc      = desc->next;
                WM_DRV_DMA_FREE(desc_free);
            }
            dma_data->drv_ctx.desc[ch] = NULL;
        }

        /* dma ch cabllback */
        if (dma_data->drv_ctx.user_ch_cb[ch].cb != NULL) {
            dma_data->drv_ctx.user_ch_cb[ch].cb(ch, sts, dma_data->drv_ctx.user_ch_cb[ch].cb_priv);
        }
    }
}

WM_DRV_DMA_STATIC wm_drv_dma_status_t wm_w800_driver_dma_register_callback(wm_dma_data_t *dma_data,
                                                                           wm_drv_dma_ch_callback_t dma_callback,
                                                                           void *user_data)
{
    wm_drv_dma_status_t ret = WM_DRV_DMA_STATUS_SUCCESS;

    for (uint8_t ch = WM_DMA_CH_0; ch < WM_DMA_CH_MAX; ch++) {
        if (wm_hal_dma_register_callback(&dma_data->hal_dev, ch, (wm_hal_dma_callback_t)dma_callback, user_data) !=
            WM_HAL_DMA_STATUS_SUCCESS) {
            ret = WM_DRV_DMA_STATUS_FAILED;
        }
    }

    return ret;
}

WM_DRV_DMA_STATIC wm_drv_dma_status_t wm_w800_driver_dma_init(wm_device_t *dev)
{
    wm_dma_data_t *dma_data = NULL;
    wm_dt_hw_dma_t *hw      = (wm_dt_hw_dma_t *)dev->hw;
    wm_drv_dma_status_t ret = WM_DRV_DMA_STATUS_SUCCESS;

    dma_data = WM_DRV_DMA_MALLOC(WM_DRV_DMA_LEN_IN_BYTES(wm_dma_data_t));
    if (dma_data == NULL) {
        ret = WM_DRV_DMA_STATUS_NO_MEM;
        goto dma_init_ext;
    }
    WM_DRV_DMA_MEM_ZERO(dma_data, WM_DRV_DMA_LEN_IN_BYTES(wm_dma_data_t));
    dma_data->hal_dev.reg_base  = (wm_dma_reg_t *)hw->reg_base;
    dma_data->hal_dev.irq_num   = hw->irq_cfg.irq_num;
    dma_data->drv_ctx.clock_dev = wm_dt_get_device_by_name("rcc");
    wm_drv_clock_enable(dma_data->drv_ctx.clock_dev, WM_RCC_DMA_GATE_EN);

    if (wm_hal_dma_init(&dma_data->hal_dev) != WM_HAL_DMA_STATUS_SUCCESS) {
        ret = WM_DRV_DMA_STATUS_FAILED;
        goto dma_init_ext;
    }

    if (wm_w800_driver_dma_register_callback(dma_data, (wm_drv_dma_ch_callback_t)wm_drv_dma_isr_entry, (void *)dev) !=
        WM_DRV_DMA_STATUS_SUCCESS) {
        WM_DRV_DMA_FREE(dma_data);
        dma_data = NULL;
        ret      = WM_DRV_DMA_STATUS_FAILED;
        goto dma_init_ext;
    }

    WM_DRV_DMA_CREATE_LOCK(&dma_data->drv_ctx.lock);
    if (dma_data->drv_ctx.lock == NULL) {
        wm_log_error("lock create err");
        WM_DRV_DMA_FREE(dma_data);
        dma_data = NULL;
        ret      = WM_DRV_DMA_STATUS_FAILED;
        goto dma_init_ext;
    }

    dev->state = WM_DEV_ST_INITED;
    dev->drv   = (void *)dma_data;

dma_init_ext:
    WM_DRV_DMA_LOG_E("dma driver init done for dma dev:0x%x with ret:%d", dev, ret);
    return ret;
}

WM_DRV_DMA_STATIC wm_drv_dma_status_t wm_w800_driver_dma_deinit(wm_dma_data_t *dma_data)
{
    wm_drv_dma_status_t ret = WM_DRV_DMA_STATUS_SUCCESS;

    if (dma_data == NULL) {
        wm_log_error("dma null");
        return WM_DRV_DMA_STATUS_FAILED;
    }

    if (wm_hal_dma_deinit(&dma_data->hal_dev) != WM_HAL_DMA_STATUS_SUCCESS) {
        ret = WM_DRV_DMA_STATUS_FAILED;
    }

    if (dma_data->drv_ctx.clock_dev) {
        wm_drv_clock_disable(dma_data->drv_ctx.clock_dev, WM_RCC_DMA_GATE_EN);
    }

    return ret;
}

WM_DRV_DMA_STATIC wm_drv_dma_status_t wm_w800_driver_dma_request_ch(wm_dma_data_t *dma_data, uint8_t *ch)
{
    wm_drv_dma_status_t ret = WM_DRV_DMA_STATUS_SUCCESS;

    if (wm_hal_dma_request_ch(&dma_data->hal_dev, ch) != WM_HAL_DMA_STATUS_SUCCESS) {
        ret = WM_DRV_DMA_STATUS_FAILED;
    }

    return ret;
}

WM_DRV_DMA_STATIC wm_drv_dma_status_t wm_w800_driver_dma_release_ch(wm_dma_data_t *dma_data, uint8_t ch)
{
    wm_drv_dma_status_t ret = WM_DRV_DMA_STATUS_SUCCESS;

    if (ch >= WM_DMA_CH_MAX || ch < WM_DMA_CH_0) {
        wm_log_error("dma chl error");
        return WM_DRV_DMA_STATUS_FAILED;
    }

    if (wm_hal_dma_release_ch(&dma_data->hal_dev, ch) != WM_HAL_DMA_STATUS_SUCCESS) {
        ret = WM_DRV_DMA_STATUS_FAILED;
    }

    return ret;
}

WM_DRV_DMA_STATIC wm_drv_dma_status_t wm_w800_driver_dma_desc_convert(wm_drv_dma_desc_t *drv_desc, wm_hal_dma_desc_t *hal_desc)
{
    wm_drv_dma_status_t ret = WM_DRV_DMA_STATUS_SUCCESS;
    uint8_t unit            = 0;
    uint8_t unit_byte       = 0;
    uint8_t burst           = 0;

    WM_DRV_DMA_LOG_I("transfer drv_desc:0x%x to hal_desc:0x%x", (uint32_t *)drv_desc, (uint32_t *)hal_desc);

    if ((WM_DRV_DMA_ADDR_IS_MEM(drv_desc->src)) && (WM_DRV_DMA_ADDR_IS_MEM(drv_desc->dest))) {
        hal_desc->extend_ctrl.mode = WM_DMA_SW_MODE;
        if (WM_DRV_DMA_ADDR_IS_PSRAM(drv_desc->src) || WM_DRV_DMA_ADDR_IS_PSRAM(drv_desc->dest)) {
            if (drv_desc->ctrl.len % WM_DRV_DMA_BYTE_ALIGNMENT) {
                WM_DRV_DMA_LOG_E("psram need 4 byte align");
                return WM_DRV_DMA_STATUS_ERROR;
            }

            unit = WM_DMA_XFER_UNIT_WORD;
        } else {
            unit = WM_DMA_XFER_UNIT_BYTE;
        }
    } else {
        hal_desc->extend_ctrl.mode = WM_DMA_HW_MODE;
        if (WM_DRV_DMA_ADDR_IS_UART(drv_desc->src) || WM_DRV_DMA_ADDR_IS_UART(drv_desc->dest)) {
            unit = WM_DMA_XFER_UNIT_BYTE;
        } else {
            if (drv_desc->ctrl.len % WM_DRV_DMA_BYTE_ALIGNMENT) {
                WM_DRV_DMA_LOG_E("need 4 byte align");
                return WM_DRV_DMA_STATUS_ERROR;
            }
            unit = WM_DMA_XFER_UNIT_WORD;
        }
    }

    unit_byte = unit == WM_DMA_XFER_UNIT_BYTE      ? 1 :
                unit == WM_DMA_XFER_UNIT_HALF_WORD ? 2 :
                unit == WM_DMA_XFER_UNIT_WORD      ? 4 :
                                                     0;

    if (unit_byte) {
        if (WM_DRV_DMA_NORMAL_MODE == drv_desc->ctrl.dma_mode) {
            if (!(drv_desc->ctrl.len % (unit_byte * WM_DRV_DMA_BURST_MAX))) {
                burst = WM_DMA_BURST_4UNIT;
            } else {
                burst = WM_DMA_BURST_1UNIT;
            }
        } else {
            burst = WM_DMA_BURST_1UNIT;
        }
    } else {
        WM_DRV_DMA_LOG_E("dma unit err");
        return WM_DRV_DMA_STATUS_ERROR;
    }

    WM_DRV_DMA_LOG_I("busrt %d unit %d mode %d", burst, unit, hal_desc->extend_ctrl.mode);
    hal_desc->src_addr  = drv_desc->src;
    hal_desc->dest_addr = drv_desc->dest;
    if (drv_desc->ctrl.dma_mode == WM_DRV_DMA_LIST_MODE) {
        hal_desc->ctrl.chain.src_addr_inc  = drv_desc->ctrl.src_inc_mode;
        hal_desc->ctrl.chain.dest_addr_inc = drv_desc->ctrl.dest_inc_mode;
        hal_desc->ctrl.chain.len           = drv_desc->ctrl.len;
        hal_desc->ctrl.chain.data_unit     = unit;
        hal_desc->ctrl.chain.burst_size    = burst;
    } else {
        hal_desc->ctrl.nonchain.auto_reload   = drv_desc->ctrl.auto_reload;
        hal_desc->ctrl.nonchain.src_addr_inc  = drv_desc->ctrl.src_inc_mode;
        hal_desc->ctrl.nonchain.dest_addr_inc = drv_desc->ctrl.dest_inc_mode;
        hal_desc->ctrl.nonchain.len           = drv_desc->ctrl.len;
        hal_desc->ctrl.nonchain.data_unit     = unit;
        hal_desc->ctrl.nonchain.burst_size    = burst;
    }
    hal_desc->extend_ctrl.ch       = drv_desc->ctrl.ch;
    hal_desc->extend_ctrl.int_en   = drv_desc->ctrl.int_en;
    hal_desc->extend_ctrl.int_type = WM_DMA_INT_XFER_DONE;

    if (hal_desc->extend_ctrl.mode == WM_DMA_HW_MODE) {
        hal_desc->extend_ctrl.req_sel = drv_dma_req_src_drv2hal[drv_desc->ctrl.req_src];
        if (drv_desc->ctrl.req_src >= WM_DRV_DMA_CH_UART0_TX_REQ) {
            hal_desc->extend_ctrl.uart_dma_ch = drv_dma_uart_ch_drv2hal[drv_desc->ctrl.req_src - WM_DRV_DMA_CH_UART0_TX_REQ];
        }
    }

    if (drv_desc->ctrl.dma_mode == WM_DRV_DMA_WARP_MODE) {
        hal_desc->extend_ctrl.warp_mode_en = WM_DMA_WARP_CTRL_ENABLE;
        hal_desc->warp_info.src_warp_len   = drv_desc->ctrl.src_warp_len;
        hal_desc->warp_info.dest_warp_len  = drv_desc->ctrl.dest_warp_len;
    } else {
        hal_desc->extend_ctrl.warp_mode_en = WM_DMA_WARP_CTRL_DISABLE;
    }

    WM_DRV_DMA_LOG_DUMP(WM_LOG_LEVEL_INFO, "after transfer, hal:", WM_DRV_DMA_DUMP_WIDTH, (void *)hal_desc,
                        sizeof(wm_hal_dma_desc_t));
    return ret;
}

WM_DRV_DMA_STATIC wm_drv_dma_status_t wm_w800_driver_dma_config(wm_dma_data_t *dma_data, wm_drv_dma_desc_t *dma_desc)
{
    wm_hal_dma_desc_t *hal_desc  = wm_drv_dma_get_hal_desc(dma_desc->ctrl.ch);
    wm_hal_dma_desc_t *last_desc = hal_desc;
    wm_hal_dma_desc_t *tmp_desc  = NULL;
    uint8_t ch                   = dma_desc->ctrl.ch;
    bool is_first_desc           = WM_DRV_DMA_FIRST_DESC;
    wm_drv_dma_status_t ret      = WM_DRV_DMA_STATUS_SUCCESS;

    if (hal_desc == NULL || !dma_desc->ctrl.len) {
        wm_log_error("desc null or len err");
        return WM_DRV_DMA_STATUS_FAILED;
    }

    WM_DRV_DMA_LOG_DUMP(WM_LOG_LEVEL_INFO, "drv cfg desc:", WM_DRV_DMA_DUMP_WIDTH, (void *)dma_desc, sizeof(wm_drv_dma_desc_t));
    WM_DRV_DMA_MEM_ZERO(hal_desc, WM_DRV_DMA_LEN_IN_BYTES(wm_hal_dma_desc_t));
    /* drv desc transfer to hal desc, make sure all param check pass in caller */
    if (dma_desc->ctrl.dma_mode == WM_DRV_DMA_LIST_MODE) {
        do {
            WM_DRV_DMA_LOG_I("list dma_desc:0x%x", (uint32_t *)dma_desc);
            hal_desc = (wm_hal_dma_desc_t *)WM_DRV_DMA_MALLOC(WM_DRV_DMA_LEN_IN_BYTES(wm_hal_dma_desc_t));
            if (!hal_desc) {
                WM_DRV_DMA_LOG_E("dma malloc fail");
                ret = WM_DRV_DMA_STATUS_NO_MEM;
                goto exit;
            }
            WM_DRV_DMA_MEM_ZERO(hal_desc, WM_DRV_DMA_LEN_IN_BYTES(wm_hal_dma_desc_t));
            last_desc->next                     = hal_desc;
            hal_desc->vld                       = WM_BIT(WM_DMA_LIST_VLD);
            hal_desc->extend_ctrl.chain_mode    = WM_DMA_CHAIN_MODE_LIST;
            hal_desc->extend_ctrl.chain_mode_en = WM_DMA_CHAIN_MODE_ENABLE;
            if (is_first_desc) {
                dma_data->drv_ctx.desc[ch] = (void *)hal_desc;
                is_first_desc              = !WM_DRV_DMA_FIRST_DESC;
                WM_DRV_DMA_LOG_I("first desc:0x%x", dma_data->drv_ctx.desc[ch]);
            }
            hal_desc->next = NULL;
            wm_w800_driver_dma_desc_convert(dma_desc, hal_desc);
            last_desc = hal_desc;

            dma_desc = dma_desc->next;
        } while (dma_desc != NULL);
    } else {
        WM_DRV_DMA_LOG_I("dma desc:0x%x, 0x%x", (uint32_t *)dma_desc, hal_desc);
        dma_data->drv_ctx.desc[ch] = (void *)hal_desc;
        wm_w800_driver_dma_desc_convert(dma_desc, hal_desc);
    }

    if (wm_hal_dma_config(&dma_data->hal_dev, (wm_hal_dma_desc_t *)dma_data->drv_ctx.desc[ch]) != WM_HAL_DMA_STATUS_SUCCESS) {
        WM_DRV_DMA_LOG_E("dma cfg fail");
        ret = WM_DRV_DMA_STATUS_FAILED;
        goto exit;
    }

    return ret;
exit:
    last_desc = (wm_hal_dma_desc_t *)dma_data->drv_ctx.desc[ch];
    while (last_desc != NULL) {
        tmp_desc  = last_desc;
        last_desc = last_desc->next;
        WM_DRV_DMA_FREE(tmp_desc);
    }
    dma_data->drv_ctx.desc[ch] = NULL;
    return ret;
}

WM_DRV_DMA_STATIC wm_drv_dma_status_t wm_w800_driver_dma_config_partial(wm_dma_data_t *dma_data, wm_drv_dma_desc_t *dma_desc)
{
    wm_hal_dma_cfg_partial_desc_t hal_partial_desc = { 0 };
    wm_drv_dma_status_t ret                        = WM_DRV_DMA_STATUS_SUCCESS;

    WM_DRV_DMA_LOG_DUMP(WM_LOG_LEVEL_INFO, "drv parrtail cfg desc:", WM_DRV_DMA_DUMP_WIDTH, (void *)dma_desc,
                        sizeof(wm_drv_dma_desc_t));
    WM_DRV_DMA_MEM_ZERO(&hal_partial_desc, WM_DRV_DMA_LEN_IN_BYTES(wm_hal_dma_cfg_partial_desc_t));
    hal_partial_desc.src_addr  = dma_desc->src;
    hal_partial_desc.dest_addr = dma_desc->dest;
    hal_partial_desc.len       = dma_desc->ctrl.len;
    hal_partial_desc.ch        = dma_desc->ctrl.ch;

    if (wm_hal_dma_config_partial(&dma_data->hal_dev, &hal_partial_desc) != WM_HAL_DMA_STATUS_SUCCESS) {
        ret = WM_DRV_DMA_STATUS_FAILED;
    }

    return ret;
}

WM_DRV_DMA_STATIC wm_drv_dma_status_t wm_w800_driver_dma_start(wm_dma_data_t *dma_data, wm_dma_ch_t ch)
{
    wm_drv_dma_status_t ret = WM_DRV_DMA_STATUS_SUCCESS;

    if (ch >= WM_DMA_CH_MAX || ch < WM_DMA_CH_0) {
        wm_log_error("dma chl error");
        return WM_DRV_DMA_STATUS_FAILED;
    }

    if (wm_hal_dma_start(&dma_data->hal_dev, ch) != WM_HAL_DMA_STATUS_SUCCESS) {
        ret = WM_DRV_DMA_STATUS_FAILED;
    }

    return ret;
}

WM_DRV_DMA_STATIC wm_drv_dma_status_t wm_w800_driver_dma_stop(wm_dma_data_t *dma_data, wm_dma_ch_t ch)
{
    wm_drv_dma_status_t ret = WM_DRV_DMA_STATUS_SUCCESS;

    if (ch >= WM_DMA_CH_MAX || ch < WM_DMA_CH_0) {
        wm_log_error("dma chl error");
        return WM_DRV_DMA_STATUS_FAILED;
    }

    ret = wm_hal_dma_stop(&dma_data->hal_dev, ch);

    return ret;
}

WM_DRV_DMA_STATIC wm_drv_dma_status_t wm_w800_driver_dma_get_status(wm_dma_data_t *dma_data, wm_dma_ch_t ch,
                                                                    wm_dma_sts_info_t *dma_sts)
{
    wm_drv_dma_status_t ret = WM_DRV_DMA_STATUS_SUCCESS;

    if (ch >= WM_DMA_CH_MAX || ch < WM_DMA_CH_0) {
        wm_log_error("dma chl error");
        return WM_DRV_DMA_STATUS_FAILED;
    }

    ret = wm_hal_dma_get_status(&dma_data->hal_dev, ch, dma_sts);

    return ret;
}

const wm_drv_dma_ops_t wm_drv_dma_ops = {
    .init               = wm_w800_driver_dma_init,
    .deinit             = wm_w800_driver_dma_deinit,
    .request_ch         = wm_w800_driver_dma_request_ch,
    .release_ch         = wm_w800_driver_dma_release_ch,
    .config_dma         = wm_w800_driver_dma_config,
    .config_dma_partial = wm_w800_driver_dma_config_partial,
    .start_dma          = wm_w800_driver_dma_start,
    .stop_dma           = wm_w800_driver_dma_stop,
    .get_status         = wm_w800_driver_dma_get_status,
};