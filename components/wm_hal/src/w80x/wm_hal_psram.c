/**
 * @file wm_hal_psram.c
 *
 * @brief hal psram Module
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

#include "wmsdk_config.h"
#include "core_804.h"
#include "wm_types.h"
#include "wm_error.h"
#include "wm_soc.h"
#include "wm_ll_psram.h"
#include "wm_hal_psram.h"
#include "wm_hal_irq.h"
#include "wm_regs_base.h"

#define LOG_TAG "hal_psram"
#include "wm_log.h"

/**
 * @brief log macros
 */
#define WM_HAL_PSRAM_LOG              wm_log_debug
#define WM_HAL_PSRAM_TRACE_FAIL()     WM_HAL_PSRAM_LOG("err line %d", __LINE__)

/**
 * @brief status bits
 */
#define WM_HAL_PSRAM_STATUS_INIT      (1 << 0)
#define WM_HAL_PSRAM_STATUS_DMA_USING (1 << 1)

/**
 * @brief Min colck number of CS pulse on high level
 */

#define WM_HAL_PSRAM_TCPH             6

/**
 * @brief Burst mode enable
 */
#define WM_HAL_PSRAM_BURST_ENABLE     1

/**
 * @brief PSRAM size
 */
#define PSRAM_8M                      0x800000
#define PSRAM_4M                      0x400000
#define PSRAM_2M                      0x200000

static void wm_hal_psram_check_size(wm_hal_psram_dev_t *dev)
{
    uint64_t check_value  = 0x1a2b3c4d5e6fa55a;
    uint64_t check_value2 = 0xf5a5f6e5d4c3b2a1;

    const uint32_t check_arr[] = { PSRAM_2M, PSRAM_4M, PSRAM_8M };
    char *p                    = (char *)CONFIG_PSRAM_BASE_ADDR;
    int num                    = sizeof(check_arr) / sizeof(check_arr[0]);

    *((uint64_t *)(p + check_arr[num - 1] - 8)) = check_value;
    *((uint64_t *)(p))                          = 0; /*clear cache*/

    if (*((uint64_t *)(p + check_arr[num - 1] - 8)) == check_value && *((uint64_t *)(p + check_arr[1] - 8)) != check_value &&
        *((uint64_t *)(p + check_arr[0] - 8)) != check_value) {
        *((uint64_t *)(p + check_arr[num - 1] - 8)) = check_value2;
        *((uint64_t *)(p))                          = 0; /*clear cache*/

        if (*((uint64_t *)(p + check_arr[num - 1] - 8)) == check_value2 &&
            *((uint64_t *)(p + check_arr[1] - 8)) != check_value2 && *((uint64_t *)(p + check_arr[0] - 8)) != check_value2) {
            dev->size = PSRAM_8M;
            WM_HAL_PSRAM_LOG("size=8M");
            return;
        }
    }

    *((uint64_t *)(p + check_arr[1] - 8)) = check_value;
    *((uint64_t *)(p))                    = 0; /*clear cache*/

    if (*((uint64_t *)(p + check_arr[1] - 8)) == check_value && *((uint64_t *)(p + check_arr[0] - 8)) != check_value) {
        *((uint64_t *)(p + check_arr[1] - 8)) = check_value2;
        *((uint64_t *)(p))                    = 0; /*clear cache*/

        if (*((uint64_t *)(p + check_arr[1] - 8)) == check_value2 && *((uint64_t *)(p + check_arr[0] - 8)) != check_value2) {
            WM_HAL_PSRAM_LOG("size=4M");
            dev->size = PSRAM_4M;
            return;
        }
    }

    *((uint64_t *)(p + check_arr[0] - 8)) = check_value;
    *((uint64_t *)(p))                    = 0; /*clear cache*/

    if (*((uint64_t *)(p + check_arr[0] - 8)) == check_value) {
        *((uint64_t *)(p + check_arr[0] - 8)) = check_value2;
        *((uint64_t *)(p))                    = 0; /*clear cache*/
        if (*((uint64_t *)(p + check_arr[0] - 8)) == check_value2) {
            WM_HAL_PSRAM_LOG("size=2M");
            dev->size = PSRAM_2M;
        }
    }

    return;
}

static void wm_hal_psram_update_param(wm_hal_psram_dev_t *dev)
{
    wm_psram_ctrl_t ctrl;
    uint32_t div;

    /*calculate clock div*/
    div = dev->ref_clock / dev->clock_hz;

    /*TEST:
        CPU 240, div=1 , psram 240M , Fail
        CPU 240, div=2 , psram 120M , OK (There may be some risks,dependent on hardware)
        CPU 240, div=3 , psram 80M  , OK (Recommended)
        CPU 80,  div=1 , psram 80M  , Fail
        CPU 80,  div=2 , psram 40M  , OK
    */
    if (div < 2) {
        div = 2;
    }

    if (div > 15) {
        div = 15; /*only 4 bits in control reigister, max is 15*/
    }

    ctrl.val          = 0;
    ctrl.division     = div;
    ctrl.tcph         = WM_HAL_PSRAM_TCPH;
    ctrl.inc_en_burst = WM_HAL_PSRAM_BURST_ENABLE;
    ctrl.quad         = dev->quad;

    wm_ll_psram_set_ctrl(dev->reg, ctrl.val);
}

/**
 * @brief reset psram when updating config
 * 1 Write PSRAM_CTRL [0] to 1
 * 2 Wait until PSRAM_CTRL [0] is cleared automatically
 * 3 Reset the PSRAM controller through soft reset
 * 4 Reset other required parameters of PSRAM_CTRL
 */
static void wm_hal_psram_reset(wm_hal_psram_dev_t *dev)
{
    wm_psram_ctrl_t ctrl;

    /*get ctrl register*/
    ctrl.val = wm_ll_psram_get_ctrl(dev->reg);
    ctrl.rst = 1;

    /*STEP 1: reset psram*/
    wm_ll_psram_set_ctrl(dev->reg, ctrl.val);

    /*STEP 2: Wait reset end*/
    while (wm_ll_psram_get_rst_status(dev->reg)) {
        ;
    }

    /*TODO use general API*/
    extern void mdelay(uint32_t ms);
    /*STEP 3: soft reset psram controler*/
    WM_REG32_CLR_BIT(WM_CLK_RST_ADDR, WM_RCC_QSPI_RAM_RST);
    mdelay(1);
    WM_REG32_SET_BIT(WM_CLK_RST_ADDR, WM_RCC_QSPI_RAM_RST);
    mdelay(1);

    /*W800 need use QSPI mode at fisrt time*/
    dev->quad = WM_HAL_PSRAM_MODE_QSPI;

    /*STEP 4: set parameters*/
    wm_hal_psram_update_param(dev);

    /*wait set parameter end.*/
    while (wm_ll_psram_is_quad_enabled(dev->reg) != dev->quad) {
        ;
    }
}

int wm_hal_psram_init(wm_hal_psram_dev_t *dev)
{
    int quad;

    if (!(dev && dev->ref_clock > 0 && (dev->quad == 0 || dev->quad == 1) && dev->clock_hz > 0)) {
        return WM_ERR_INVALID_PARAM;
    }

    if (dev->status & WM_HAL_PSRAM_STATUS_INIT) {
        /*initialized before*/
        return WM_ERR_ALREADY_INITED;
    }

    quad = dev->quad;

    /*reset*/
    wm_hal_psram_reset(dev);

    /*check size*/
    wm_hal_psram_check_size(dev);

    dev->status |= WM_HAL_PSRAM_STATUS_INIT;

    /*Init as qspi mode, change to spi mode if specified*/
    if (quad == WM_HAL_PSRAM_MODE_SPI) {
        wm_hal_psram_ioctrl(dev, WM_HAL_PSRAM_CMD_SET_MODE, WM_HAL_PSRAM_MODE_SPI, NULL);
    }

    return WM_ERR_SUCCESS;
}

int wm_hal_psram_deinit(wm_hal_psram_dev_t *dev)
{
    if (!dev) {
        return WM_ERR_INVALID_PARAM;
    }

    if (dev->status & WM_HAL_PSRAM_STATUS_INIT) {
        dev->status &= ~WM_HAL_PSRAM_STATUS_INIT;
        return WM_ERR_SUCCESS;
    } else {
        return WM_ERR_FAILED;
    }
}

int wm_hal_psram_register_callback(wm_hal_psram_dev_t *dev, wm_hal_psram_callback_t callback, void *user_data)
{
    if (!dev) {
        return WM_ERR_INVALID_PARAM;
    }

    if (!(dev->status & WM_HAL_PSRAM_STATUS_INIT)) {
        return WM_ERR_NO_INITED;
    }

    dev->callback  = (void *)callback;
    dev->user_data = user_data;

    return WM_ERR_SUCCESS;
}

int wm_hal_psram_ioctrl(wm_hal_psram_dev_t *dev, int cmd, uint32_t param, uint32_t *arg)
{
    int ret = WM_ERR_FAILED;

    if (!dev) {
        return WM_ERR_INVALID_PARAM;
    }

    if (!(dev->status & WM_HAL_PSRAM_STATUS_INIT)) {
        return WM_ERR_NO_INITED;
    }

    switch (cmd) {
        case WM_HAL_PSRAM_CMD_SET_MODE:
        {
            if (param == WM_HAL_PSRAM_MODE_SPI || param == WM_HAL_PSRAM_MODE_QSPI) {
                /*set spi mode if change*/
                if (dev->quad != param) {
                    dev->quad = param;

                    /*STEP 4: set parameters*/
                    wm_hal_psram_update_param(dev);

                    /*wait set parameter end.*/
                    while (wm_ll_psram_is_quad_enabled(dev->reg) != dev->quad) {
                        ;
                    }
                    WM_HAL_PSRAM_LOG("change quad to %d", dev->quad);
                }
                ret = WM_ERR_SUCCESS;
            } else {
                ret = WM_ERR_INVALID_PARAM;
            }
            break;
        }
        case WM_HAL_PSRAM_CMD_ENABLE_HSM:
        {
            wm_ll_psram_enable_hsm(dev->reg, 1);
            ret = WM_ERR_SUCCESS;
            break;
        }
        case WM_HAL_PSRAM_CMD_DISABLE_HSM:
        {
            wm_ll_psram_enable_hsm(dev->reg, 0);
            ret = WM_ERR_SUCCESS;
            break;
        }
        default:
        {
            ret = WM_ERR_NO_SUPPORT;
            break;
        }
    }

    return ret;
}

static void wm_hal_psram_dma_done_callback(wm_dma_ch_t ch, uint32_t sts, void *user_data)
{
    wm_hal_psram_dev_t *dev = (wm_hal_psram_dev_t *)user_data;
    if (dev->callback) {
        ((wm_hal_psram_callback_t)(dev->callback))(dev, dev->user_data);
    }
    dev->status &= ~(WM_HAL_PSRAM_STATUS_DMA_USING);
}

int wm_hal_psram_get_size(wm_hal_psram_dev_t *dev, uint32_t *size)
{
    if (!(dev && size)) {
        return WM_ERR_INVALID_PARAM;
    }

    if (!(dev->status & WM_HAL_PSRAM_STATUS_INIT)) {
        return WM_ERR_NO_INITED;
    }

    *size = dev->size;

    return *size > 0 ? WM_ERR_SUCCESS : WM_ERR_FAILED;
}

int wm_hal_psram_memcpy(wm_hal_psram_dev_t *dev, void *dst, const void *src, size_t num, wm_hal_psram_dma_t *config)
{
    wm_hal_dma_desc_t desc;
    uint32_t left;
    uint32_t dma_size;

    if (!(dev && dst && src && num > 0)) {
        return WM_ERR_INVALID_PARAM;
    }

    /*check dma param valid, address aligned 4 byte*/
    if (!(config && config->dma_dev && !(((uint32_t)src) & 3) && !(((uint32_t)dst) & 3))) {
        return WM_ERR_INVALID_PARAM;
    }

    if (!(dev->status & WM_HAL_PSRAM_STATUS_INIT)) {
        return WM_ERR_NO_INITED;
    }

    if (dev->status & WM_HAL_PSRAM_STATUS_DMA_USING) {
        return WM_ERR_BUSY;
    }

    dev->status |= WM_HAL_PSRAM_STATUS_DMA_USING;

    left     = (num & 3);
    dma_size = num - left;
    /*copy the unaligned part by CPU*/
    memcpy(((char *)dst) + dma_size, ((char *)src) + dma_size, left);

    /*set dma desc*/
    memset(&desc, 0, sizeof(desc));
    desc.src_addr  = (uint32_t)src;
    desc.dest_addr = (uint32_t)dst;

    desc.ctrl.nonchain.len           = dma_size;
    desc.ctrl.nonchain.auto_reload   = WM_DMA_RELOAD_DISABLE;
    desc.ctrl.nonchain.src_addr_inc  = WM_DMA_ADDR_INC;
    desc.ctrl.nonchain.dest_addr_inc = WM_DMA_ADDR_INC;
    desc.ctrl.nonchain.data_unit     = WM_DMA_XFER_UNIT_WORD;
    desc.ctrl.nonchain.burst_size    = WM_DMA_BURST_1UNIT;
    desc.extend_ctrl.int_en          = WM_DMA_CH_EN_CTRL_ENABLE;
    desc.extend_ctrl.int_type        = WM_DMA_INT_XFER_DONE;
    desc.extend_ctrl.mode            = WM_DMA_SW_MODE;
    desc.extend_ctrl.chain_mode      = WM_DMA_CHAIN_MODE_NORMAL;
    desc.extend_ctrl.warp_mode_en    = WM_DMA_WARP_CTRL_DISABLE;
    desc.extend_ctrl.ch              = config->dma_ch;

    /*start dma copy*/
    wm_hal_dma_register_callback(config->dma_dev, config->dma_ch, wm_hal_psram_dma_done_callback, dev);
    wm_hal_dma_config(config->dma_dev, &desc);
    wm_hal_dma_start(config->dma_dev, config->dma_ch);

    WM_HAL_PSRAM_LOG("start dma ok , ch=%d", config->dma_ch);

    return WM_ERR_SUCCESS;
}
