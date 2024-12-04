/**
 * @file wm_drv_ops_rcc.c
 *
 * @brief rcc ops Module
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
#include "wm_dt_hw.h"

#define LOG_TAG "drv_rcc"
#include "wm_log.h"

#define WM_RCC_PERIPHERAL_CLK 160
#define WM_RCC_CPU_CLK_MIN    2
#define WM_RCC_CPU_CLK_MAX    240
#define WM_RCC_WLAN_CLK       160
#define WM_RCC_SD_ADK_CLK     1
#define WM_RCC_QFLASH_CLK1    40
#define WM_RCC_QFLASH_CLK2    80
#define WM_RCC_GPSEC_CLK1     80
#define WM_RCC_GPSEC_CLK2     160
#define WM_RCC_RSA_CLK1       80
#define WM_RCC_RSA_CLK2       160
#define WM_RCC_I2S_MCLK_MIN   2
#define WM_RCC_I2S_MCLK_MAX   64
#define WM_RCC_I2S_BCLK_MIN   1
#define WM_RCC_I2S_BCLK_MAX   1024

typedef enum {
    WM_IOCTL_RCC_CMD_SET_I2S,

    WM_IOCTL_RCC_CMD_MAX,
} wm_ioctl_rcc_cmd_t;

typedef struct {
    wm_ioctl_rcc_cmd_t cmd;
    union {
        struct {
            bool extal_en;
            bool mclk_en;
            uint32_t mclk_div;
            uint32_t bclk_div;
        } i2s;
    } u;
} wm_drv_rcc_ioctl_args_t;

typedef struct {
    int (*init)(wm_device_t *dev);
    int (*deinit)(wm_device_t *dev);
    int (*clock_enable)(wm_device_t *dev, uint32_t module_type);
    int (*clock_disable)(wm_device_t *dev, uint32_t module_type);
    int (*clock_reset)(wm_device_t *dev, uint32_t module_type);
    int (*config_clock)(wm_device_t *dev, wm_rcc_type_t module_type, uint16_t MHz);
    int (*get_config_clock)(wm_device_t *dev, wm_rcc_type_t module_type);
    int (*ioctl)(wm_device_t *device, wm_drv_rcc_ioctl_args_t *args);
} wm_drv_clock_ops_t;

/**
 * @brief Set the rcc clock enable
 * @param[in] module_type config the indicate module clock enable, please refer to #wm_rcc_clock_enable_t
 * @Usage wm_hal_clock_enable(WM_RCC_UART0_GATE_EN | WM_RCC_UART1_GATE_EN | WM_RCC_RF_CFG_GATE_EN);
 *
 */
static int wm_w800_drv_clock_enable(wm_device_t *dev, uint32_t module_type)
{
    int ret                     = 0;
    wm_hal_clock_dev_t *hal_dev = NULL;
    wm_drv_clock_ctx_t *drv_ctx = NULL;

    drv_ctx = (wm_drv_clock_ctx_t *)dev->drv;
    if (drv_ctx == NULL) {
        return WM_ERR_NO_INITED;
    }

    ret = wm_os_internal_mutex_acquire(drv_ctx->lock, WM_OS_WAIT_TIME_MAX);
    if (ret == WM_ERR_SUCCESS) {
        hal_dev = &(drv_ctx->hal_dev);
        wm_hal_clock_enable(hal_dev, module_type);
        wm_os_internal_mutex_release(drv_ctx->lock);
    }

    return ret;
}

/**
 * @brief Set the rcc clock disable
 * @param[in] module_type config the indicate module clock disable, please refer to #wm_rcc_clock_enable_t
 * @Usage wm_hal_clock_disable(WM_RCC_UART0_GATE_EN | WM_RCC_UART1_GATE_EN | WM_RCC_RF_CFG_GATE_EN);
 *
 */
static int wm_w800_drv_clock_disable(wm_device_t *dev, uint32_t module_type)
{
    int ret                     = 0;
    wm_hal_clock_dev_t *hal_dev = NULL;
    wm_drv_clock_ctx_t *drv_ctx = NULL;

    drv_ctx = (wm_drv_clock_ctx_t *)dev->drv;
    if (drv_ctx == NULL) {
        return WM_ERR_NO_INITED;
    }

    ret = wm_os_internal_mutex_acquire(drv_ctx->lock, WM_OS_WAIT_TIME_MAX);
    if (ret == WM_ERR_SUCCESS) {
        hal_dev = &(drv_ctx->hal_dev);
        wm_hal_clock_disable(hal_dev, module_type);
        wm_os_internal_mutex_release(drv_ctx->lock);
    }

    return ret;
}

/**
 * @brief Set the rcc clock reset
 * @param[in] module_type config the indicate module clock reset, please refer to #wm_rcc_clk_rst_t
 * @Usage wm_hal_clock_reset(WM_RCC_BBP_RST | WM_RCC_MAC_RST);
 *
 */
static int wm_w800_drv_clock_reset(wm_device_t *dev, uint32_t module_type)
{
    int ret                     = 0;
    wm_hal_clock_dev_t *hal_dev = NULL;
    wm_drv_clock_ctx_t *drv_ctx = NULL;

    drv_ctx = (wm_drv_clock_ctx_t *)dev->drv;
    if (drv_ctx == NULL) {
        return WM_ERR_NO_INITED;
    }

    ret = wm_os_internal_mutex_acquire(drv_ctx->lock, WM_OS_WAIT_TIME_MAX);
    if (ret == WM_ERR_SUCCESS) {
        hal_dev = &(drv_ctx->hal_dev);
        wm_hal_clock_reset(hal_dev, module_type);
        wm_os_internal_mutex_release(drv_ctx->lock);
    }

    return ret;
}

/**
 * @brief Clock initial
 *
 */
static int wm_w800_drv_clock_init(wm_device_t *dev)
{
    int err                     = WM_ERR_SUCCESS;
    wm_drv_clock_ctx_t *drv_ctx = NULL;
    wm_dt_hw_rcc_t *rcc_hw      = NULL;
    wm_dt_hw_rcc_t *hw_rcc      = NULL;
    wm_dt_hw_rcc_cfg_t *rcc_cfg = NULL;

    if (dev->drv == NULL && (dev->state == WM_DEV_ST_UNINIT)) {
        dev->state = WM_DEV_ST_INITING;

        drv_ctx = (wm_drv_clock_ctx_t *)wm_os_internal_malloc(sizeof(wm_drv_clock_ctx_t));
        if (!drv_ctx) {
            return WM_ERR_NO_MEM;
        }
        memset(drv_ctx, 0x00, sizeof(wm_drv_clock_ctx_t));

        rcc_hw = (wm_dt_hw_rcc_t *)dev->hw;
        //drv_ctx->hw               = rcc_hw;
        drv_ctx->hal_dev.reg_base = rcc_hw->reg_base;

        err = wm_os_internal_mutex_create(&drv_ctx->lock);
        if (err == WM_OS_STATUS_SUCCESS) {
            wm_hal_clock_init(&drv_ctx->hal_dev);
            hw_rcc  = (wm_dt_hw_rcc_t *)dev->hw;
            rcc_cfg = hw_rcc->rcc_cfg;
            for (int i = 0; i < hw_rcc->rcc_cfg_count; i++) {
                wm_hal_rcc_config_clock(&drv_ctx->hal_dev, rcc_cfg->type, rcc_cfg->clock);
                rcc_cfg++;
            }
            dev->drv   = drv_ctx;
            dev->state = WM_DEV_ST_INITED;
        } else {
            wm_os_internal_free(drv_ctx);
            dev->drv   = NULL;
            dev->state = WM_DEV_ST_UNINIT;
        }
    }

    return err;
}

/**
 * @brief Clock deinitial
 *
 */
static int wm_w800_drv_clock_deinit(wm_device_t *dev)
{
    wm_drv_clock_ctx_t *drv_ctx = NULL;
    wm_hal_clock_dev_t *hal_dev = NULL;
    int ret                     = WM_ERR_SUCCESS;

    drv_ctx = (wm_drv_clock_ctx_t *)dev->drv;
    if (drv_ctx && dev->state == WM_DEV_ST_INITED) {
        dev->drv = NULL;
        wm_os_internal_mutex_acquire(drv_ctx->lock, WM_OS_WAIT_TIME_MAX);
        if (dev->state == WM_DEV_ST_INITED) {
            wm_os_internal_mutex_delete(drv_ctx->lock);
            hal_dev = &(drv_ctx->hal_dev);
            wm_hal_clock_deinit(hal_dev);
            wm_os_internal_free(drv_ctx);
            dev->state = WM_DEV_ST_UNINIT;
        } else {
            dev->state = WM_DEV_ST_UNKNOWN;
        }
    }

    return ret;
}

/**
 * @brief Set the rcc clock
 * @param[in] module_type config the indicate module clock, please refer to #wm_rcc_type_t
 * @param[in] MHz the clock setting, unit: MHz
 * @note The Periphal/WLAN/CPU clock should be a divider of PLL clock(480MHz), others value is invalid;
 *       The SD ADC clock should be a divider of 40MHz;
 *       The wlan clock should be 160MHz to work normal;
 *       The CPU clock should above 40MHz to make wifi work, and maxmum clock is 240MHz;
 *       The QFLASH clock should be 40MHz or 80MHz;
 *       The GPSEC clock should be 80MHz or 160MHz;
 *       The RSA clock should be 80MHz or 160MHz;
 *       No need to config APB clock, used for query;
 */
static int wm_w800_drv_rcc_config_clock(wm_device_t *dev, wm_rcc_type_t module_type, uint16_t MHz)
{
    int ret                     = WM_ERR_SUCCESS;
    wm_hal_clock_dev_t *hal_dev = NULL;
    wm_drv_clock_ctx_t *drv_ctx = NULL;

    drv_ctx = (wm_drv_clock_ctx_t *)dev->drv;
    if (drv_ctx == NULL) {
        return WM_ERR_NO_INITED;
    }

    hal_dev = &(drv_ctx->hal_dev);

    if (WM_PLL_CLOCK % MHz != 0) {
        wm_log_error("Invalid clock input,must be a divide of 480MHz.\r\n");
        return WM_ERR_INVALID_PARAM;
    }
    if (module_type >= WM_RCC_TYPE_APB) {
        wm_log_error("error moudle: %d\r\n", module_type);
        return WM_ERR_INVALID_PARAM;
    }

    switch (module_type) {
        case WM_RCC_TYPE_PERIPHERAL:
        {
            if (MHz != WM_RCC_PERIPHERAL_CLK) {
                ret = WM_ERR_INVALID_PARAM;
            }
            break;
        }

        case WM_RCC_TYPE_WLAN:
        {
            if (MHz != WM_RCC_WLAN_CLK) {
                ret = WM_ERR_INVALID_PARAM;
            }
            break;
        }

        case WM_RCC_TYPE_CPU:
        {
            if (MHz < WM_RCC_CPU_CLK_MIN || MHz > WM_RCC_CPU_CLK_MAX) {
                ret = WM_ERR_INVALID_PARAM;
            }
            break;
        }

        case WM_RCC_TYPE_SD_ADC:
        {
            if (MHz != WM_RCC_SD_ADK_CLK) {
                ret = WM_ERR_INVALID_PARAM;
            }
            break;
        }

        case WM_RCC_TYPE_QFLASH:
        {
            if (MHz != WM_RCC_QFLASH_CLK1 && MHz != WM_RCC_QFLASH_CLK2) {
                ret = WM_ERR_INVALID_PARAM;
            }
            break;
        }

        case WM_RCC_TYPE_GPSEC:
        {
            if (MHz != WM_RCC_GPSEC_CLK1 && MHz != WM_RCC_GPSEC_CLK2) {
                ret = WM_ERR_INVALID_PARAM;
            }
            break;
        }

        case WM_RCC_TYPE_RSA:
        {
            if (MHz != WM_RCC_RSA_CLK1 && MHz != WM_RCC_RSA_CLK2) {
                ret = WM_ERR_INVALID_PARAM;
            }
            break;
        }

        default:
        {
            break;
        }
    }

    if (ret != WM_ERR_SUCCESS) {
        return ret;
    }

    if (wm_os_internal_task_schedule_state() != WM_OS_SCHED_NOT_START) {
        ret = wm_os_internal_mutex_acquire(drv_ctx->lock, WM_OS_WAIT_TIME_MAX);
    }
    if (ret == WM_ERR_SUCCESS) {
        ret = wm_hal_rcc_config_clock(hal_dev, module_type, MHz);
        if (wm_os_internal_task_schedule_state() != WM_OS_SCHED_NOT_START) {
            wm_os_internal_mutex_release(drv_ctx->lock);
        }
    }

    return ret;
}

/**
 * @brief Get the rcc clock
 * @param[in] module_type config the clock module, please refer to #wm_rcc_type_t
 * @return the current clock setting, unit: MHz
 */
static int wm_w800_drv_rcc_get_config_clock(wm_device_t *dev, wm_rcc_type_t module_type)
{
    int ret = 0;
    uint8_t clock;
    wm_hal_clock_dev_t *hal_dev = NULL;
    wm_drv_clock_ctx_t *drv_ctx = NULL;

    drv_ctx = (wm_drv_clock_ctx_t *)dev->drv;
    if (drv_ctx == NULL) {
        return WM_ERR_NO_INITED;
    }

    hal_dev = &(drv_ctx->hal_dev);

    if (module_type >= WM_RCC_TYPE_MAX) {
        wm_log_error("Invalid clock type.\r\n");
        return WM_ERR_INVALID_PARAM;
    }

    ret = wm_os_internal_mutex_acquire(drv_ctx->lock, WM_OS_WAIT_TIME_MAX);

    if (ret == WM_ERR_SUCCESS) {
        clock = wm_hal_rcc_get_clock_config(hal_dev, module_type);
        wm_os_internal_mutex_release(drv_ctx->lock);
        ret = clock;
    }

    return ret;
}

static int wm_w800_drv_rcc_ioctl(wm_device_t *dev, wm_drv_rcc_ioctl_args_t *args)
{
    int ret                     = WM_ERR_SUCCESS;
    wm_hal_clock_dev_t *hal_dev = NULL;
    wm_drv_clock_ctx_t *drv_ctx = NULL;

    drv_ctx = (wm_drv_clock_ctx_t *)dev->drv;
    if (drv_ctx == NULL) {
        return WM_ERR_NO_INITED;
    }

    hal_dev = &(drv_ctx->hal_dev);

    if (wm_os_internal_task_schedule_state() != WM_OS_SCHED_NOT_START) {
        wm_os_internal_mutex_acquire(drv_ctx->lock, WM_OS_WAIT_TIME_MAX);
    }

    switch (args->cmd) {
        case WM_IOCTL_RCC_CMD_SET_I2S:
        {
            if (args->u.i2s.mclk_en &&
                (args->u.i2s.mclk_div > WM_RCC_I2S_MCLK_MAX || args->u.i2s.mclk_div < WM_RCC_I2S_MCLK_MIN)) {
                ret = WM_ERR_INVALID_PARAM;
                break;
            }

            if (args->u.i2s.bclk_div > WM_RCC_I2S_BCLK_MAX || args->u.i2s.bclk_div < WM_RCC_I2S_BCLK_MIN) {
                ret = WM_ERR_INVALID_PARAM;
                break;
            }
            wm_hal_rcc_set_i2s_clock_config(hal_dev, args->u.i2s.extal_en, args->u.i2s.mclk_en, args->u.i2s.mclk_div,
                                            args->u.i2s.bclk_div);
            break;
        }
        default:
        {
            break;
        }
    }

    if (wm_os_internal_task_schedule_state() != WM_OS_SCHED_NOT_START) {
        wm_os_internal_mutex_release(drv_ctx->lock);
    }

    return ret;
}

/*internal interface ops*/
const wm_drv_clock_ops_t wm_drv_rcc_ops = {
    .init             = wm_w800_drv_clock_init,
    .deinit           = wm_w800_drv_clock_deinit,
    .clock_enable     = wm_w800_drv_clock_enable,
    .clock_disable    = wm_w800_drv_clock_disable,
    .clock_reset      = wm_w800_drv_clock_reset,
    .config_clock     = wm_w800_drv_rcc_config_clock,
    .get_config_clock = wm_w800_drv_rcc_get_config_clock,
    .ioctl            = wm_w800_drv_rcc_ioctl,
};
