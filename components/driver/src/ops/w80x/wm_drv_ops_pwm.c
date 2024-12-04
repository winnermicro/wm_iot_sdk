/**
 * @file wm_drv_ops_pwm.c
 *
 * @brief DRV_OPS_PWM Module
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
#include "wm_hal_dma.h"
#include "wm_irq.h"
#include "wm_list.h"
#include "wm_hal_pwm.h"
#include "wm_osal.h"
#include "wm_soc.h"
#include "wm_soc_cfgs.h"
#include "wm_types.h"
#include "wm_error.h"
#include "wm_drv_dma.h"
#include "wm_drv_pwm.h"
#include "wm_drv_rcc.h"
#include "wm_drv_gpio.h"
#include "wm_drv_irq.h"
#include "wm_dt_hw.h"

#define WM_DRV_PWM_DEBUG    0
#define WM_DRV_PWM_GPIO_SET 1

#if WM_DRV_PWM_DEBUG
#include "wm_log.h"
#undef LOG_TAG
#define LOG_TAG               "PWM DRV"
#define WM_DRV_PWM_LOG_D(...) wm_log_info(__VA_ARGS__)
#define WM_DRV_PWM_LOG_I(...) wm_log_info(__VA_ARGS__)
#define WM_DRV_PWM_LOG_E(...) wm_log_error(__VA_ARGS__)

#define WM_DRV_PWM_LOG_MARKER

#define WM_DRV_PWM_ASSERT(v) assert(v)

static void dump_cfgs(wm_hal_pwm_channel_cfg_t *cfg)
{
    wm_log_info("\t\t--<cfg>--");
    wm_log_info("cfg.channel:%d", cfg->channel);
    wm_log_info("cfg.mode:%d", cfg->mode);
    wm_log_info("cfg.cnt_type:%d", cfg->cnt_type);
    wm_log_info("cfg.clkdiv:%d", cfg->clkdiv);
    wm_log_info("cfg.period:%d", cfg->period);
    wm_log_info("cfg.period_num:%d", cfg->period_num);
    wm_log_info("cfg.duty_cycle:%d", cfg->duty_cycle);
    wm_log_info("cfg.dead_time_en:%s", cfg->dead_time_en ? "True" : "False");
    wm_log_info("cfg.dead_time_cnt:%d", cfg->dead_time_cnt);
    wm_log_info("cfg.dead_time_clkdiv:%d", cfg->dead_time_clkdiv);
    wm_log_info("cfg.cap_int_type:%d", cfg->int_type);
    wm_log_info("cfg.period_int:%s", cfg->period_int ? "True" : "False");
    wm_log_info("cfg.inverse:%s", cfg->inverse ? "True" : "False");
    wm_log_info("cfg.autoload:%s", cfg->autoload ? "True" : "False");
    wm_log_info("cfg.output_high:%s", cfg->output_high ? "True" : "False");

    wm_log_info("cfg.dma_ch:%d", cfg->dma_channel);
    wm_log_info("cfg.dma_dev:%p", cfg->dma_dev);
    if (cfg->dma_dev && cfg->dma_dev->reg_base) {
        wm_log_info("cfg.dma_reg:%p", cfg->dma_dev->reg_base);
        wm_dma_reg_t *reg_base = cfg->dma_dev->reg_base;
        void *dump_addr        = (void *)&reg_base->dma_ch[cfg->dma_channel];
        wm_log_dump(WM_LOG_LEVEL_INFO, "dma reg:", 0xf, (void *)dump_addr, sizeof(wm_dma_ch_reg_t));
        wm_log_dump(WM_LOG_LEVEL_INFO, "dma_cfg:", 0xf, (void *)&cfg->dma_cfg, sizeof(wm_hal_dma_desc_t));
    }
    wm_os_internal_disp_task_stat_info();
}

void wm_drv_pwm_regs_dumps(wm_hal_pwm_dev_t *dev)
{
    wm_log_info("\t\t--<reg>--");
    wm_log_info("clkdiv01   : 0x%08x", dev->reg_dev->clkdiv01.val);
    wm_log_info("clkdiv23   : 0x%08x", dev->reg_dev->clkdiv23.val);
    wm_log_info("clkdiv4    : 0x%08x", dev->reg_dev->ch4_clkdiv.val);
    wm_log_info("ctrl       : 0x%08x", dev->reg_dev->ctrl.val);
    wm_log_info("ctrl41     : 0x%08x", dev->reg_dev->ch4_ctrl1.val);
    wm_log_info("ctrl42     : 0x%08x", dev->reg_dev->ch4_ctrl2.val);
    wm_log_info("period     : 0x%08x", dev->reg_dev->period.val);
    wm_log_info("period num : 0x%08x", dev->reg_dev->period_num.val);
    wm_log_info("compare    : 0x%08x", dev->reg_dev->compare.val);
    wm_log_info("dead time  : 0x%08x", dev->reg_dev->dead_time.val);
    wm_log_info("int ctrl   : 0x%08x", dev->reg_dev->int_ctrl.val);
    wm_log_info("int_stats  : 0x%08x", dev->reg_dev->int_status.val);
    wm_log_info("ch0_capture: 0x%08x", dev->reg_dev->ch0_capture.val);
    wm_log_info("ch4_capture: 0x%08x", dev->reg_dev->ch4_capture.val);
    wm_log_info("brake      : 0x%08x", dev->reg_dev->brake.val);
}

#else

#define WM_DRV_PWM_LOG_D(...)
#define WM_DRV_PWM_LOG_I(...)
#define WM_DRV_PWM_LOG_E(...)
#define WM_DRV_PWM_ASSERT(...)

#define WM_DRV_PWM_LOG_MARKER

#define dump_cfgs(arg...) \
    do {                  \
    } while (0)

#define wm_drv_pwm_regs_dumps(arg...) \
    do {                              \
    } while (0)
#endif

#define WM_PWM_PRE_VALID() \
    (!!device && device->state == WM_DEV_ST_INITED && device->drv && !!((wm_drv_pwm_data_t *)(device->drv))->ctx.lock)

#define WM_DRV_PWM_TRY_LOCK() \
    (WM_ERR_SUCCESS == wm_os_internal_mutex_acquire(((wm_drv_pwm_data_t *)(device->drv))->ctx.lock, WM_DRV_PWM_MUTEX_TIMEOUT))

#define WM_DRV_PWM_UNLOCK() wm_os_internal_mutex_release(((wm_drv_pwm_data_t *)(device->drv))->ctx.lock)

#define WM_DRV_PWM_OPS(d)   ((wm_drv_pwm_ops_t *)(d->ops))

enum {
    WM_IOCTL_PWM_CMD_SET_FREQ,
    WM_IOCTL_PWM_CMD_GET_FREQ,
    WM_IOCTL_PWM_CMD_SET_FREQ_CYCLE,
    WM_IOCTL_PWM_CMD_GET_FREQ_CYCLE,
    WM_IOCTL_PWM_CMD_SET_DUTY_CYCLE,
    WM_IOCTL_PWM_CMD_GET_DUTY_CYCLE,
    WM_IOCTL_PWM_CMD_SET_PERIOD_NUM,
    WM_IOCTL_PWM_CMD_GET_PERIOD_NUM,
    WM_IOCTL_PWM_CMD_SET_PERIOD_INT,
    WM_IOCTL_PWM_CMD_GET_PERIOD_INT,
    WM_IOCTL_PWM_CMD_SET_INVERSE,
    WM_IOCTL_PWM_CMD_GET_INVERSE,
    WM_IOCTL_PWM_CMD_SET_BRAKE_LEVEL,
    WM_IOCTL_PWM_CMD_GET_BRAKE_LEVEL,
    WM_IOCTL_PWM_CMD_SET_DEADTIME,
    WM_IOCTL_PWM_CMD_GET_DEADTIME,
    WM_IOCTL_PWM_CMD_SET_AUTORELOAD,
    WM_IOCTL_PWM_CMD_GET_AUTORELOAD,
    WM_IOCTL_PWM_CMD_SET_CAP_CB,
    WM_IOCTL_PWM_CMD_SET_OUT_CB,
    WM_IOCTL_PWM_CMD_SHW_CFGS,
};

/**
 * @brief used to descript data struct for each IOCTL command
 */
typedef struct {
    uint8_t channel;
    union {
        struct {
            uint8_t cnt;
            uint8_t clkdiv;
        } deadtime;
        struct {
            uint8_t period_cycle;
            uint16_t clkdiv;
        } freq_cycle;
        struct {
            wm_drv_pwm_report_cb_t cb;
            void *cb_arg;
        } report_cb;
        uint32_t freq_hz;
        uint8_t duty_cycle;
        uint8_t period_num;
        bool period_int;
        bool inverse;
        bool brake_level_high;
        bool autoreload;
    } u;
} wm_drv_pwm_ioctl_args_t;

/**
 * @brief user callback infomation structure
 */
typedef struct {
    void *cb_arg;              /**< the argument of cb which is registered by user */
    wm_drv_pwm_report_cb_t cb; /**< the callback registered by user */
} wm_drv_pwm_report_user_cb_t;

/**
 * @brief PWM driver context
 */
typedef struct {
    struct dl_list hal_ch_cfg_list;                         /**< configuration list for each channel */
    wm_dt_hw_pwm_t *hw;                                     /**< info from DT */
    wm_os_mutex_t *lock;                                    /**< Mutex for protecting the concurrent access of APIs */
    wm_device_t *dma_dev;                                   /**< DMA device pointer */
    wm_device_t *clock_dev;                                 /**< RCC clock device pointer */
    wm_drv_pwm_report_user_cb_t cap_cb[WM_PWM_CHANNEL_MAX]; /**< User cb for PWM capture done */
    wm_drv_pwm_report_user_cb_t out_cb[WM_PWM_CHANNEL_MAX]; /**< User cb for PWM output done */
} wm_drv_pwm_ctx_t;

/**
 * @brief PWM driver device structure
 */
typedef struct {
    wm_hal_pwm_dev_t hal_pwm_dev; /**< device pointer for HAL */
    wm_drv_pwm_ctx_t ctx;         /**< driver context */
} wm_drv_pwm_data_t;

/**
 * @brief A driver API abstraction layer for shielding the implementation of different hardware drivers.
 */
typedef struct {
    int (*init)(wm_device_t *dev);
    int (*deinit)(wm_drv_pwm_data_t *dev);
    int (*channel_init)(wm_drv_pwm_data_t *dev, wm_drv_pwm_channel_cfg_t *dcfg);
    int (*channel_deinit)(wm_drv_pwm_data_t *dev, uint8_t channel);
    int (*channel_start)(wm_drv_pwm_data_t *dev, uint8_t channel);
    int (*channel_stop)(wm_drv_pwm_data_t *dev, uint8_t channel);
    int (*channel_start_all)(wm_drv_pwm_data_t *dev);
    int (*channel_stop_all)(wm_drv_pwm_data_t *dev);
    int (*ioctl)(wm_drv_pwm_data_t *dev, uint8_t cmd, wm_drv_pwm_ioctl_args_t *args);
} wm_drv_pwm_ops_t;

#if WM_DRV_PWM_GPIO_SET
/* TODO: use common utils to configure GPIOs */
enum {
    PWM_IO_MAP_IDX0 = WM_PWM_CHANNEL_0,
    PWM_IO_MAP_IDX1 = WM_PWM_CHANNEL_1,
    PWM_IO_MAP_IDX2 = WM_PWM_CHANNEL_2,
    PWM_IO_MAP_IDX3 = WM_PWM_CHANNEL_3,
    PWM_IO_MAP_IDX4 = WM_PWM_CHANNEL_4,
    PWM_IO_MAP_IDX5 = WM_PWM_CHANNEL_MAX, /* index for brake input gpio */

    PWM_IO_MAP_IDX_MAX
};

static wm_gpio_num_t pwm_gpios[PWM_IO_MAP_IDX_MAX] = { WM_GPIO_NUM_MAX };

/* TODO: should call common reset function from GPIO module for future */
static void wm_drv_pwm_set_gpio_idle(uint8_t idx)
{
    if (idx >= PWM_IO_MAP_IDX_MAX || pwm_gpios[idx] == WM_GPIO_NUM_MAX) {
        return;
    }
    wm_drv_gpio_iomux_func_sel(pwm_gpios[idx], WM_GPIO_IOMUX_FUN5);

    wm_drv_gpio_set_pullmode(pwm_gpios[idx], WM_GPIO_PULL_UP);

    pwm_gpios[idx] = WM_GPIO_NUM_MAX;
}

static int wm_drv_pwm_gpio_config(uint8_t idx, wm_gpio_num_t pin, wm_gpio_pin_mux_t func)
{
    int ret = WM_ERR_INVALID_PARAM;

    wm_drv_pwm_set_gpio_idle(idx);

    pwm_gpios[idx] = pin;

    ret = wm_drv_gpio_iomux_func_sel(pin, func);

    if (ret == WM_ERR_SUCCESS) {
        if (idx == PWM_IO_MAP_IDX5) {
            ret = wm_drv_gpio_set_dir(pin, WM_GPIO_DIR_INPUT);
            if (ret == WM_ERR_SUCCESS) {
                ret = wm_drv_gpio_set_pullmode(pwm_gpios[idx], WM_GPIO_PULL_DOWN);
            }
        }

        if (ret == WM_ERR_SUCCESS) {
            WM_DRV_PWM_LOG_I("GPIO[%02d] configured, and clock enabled!", pin);
        }
    }

    return ret;
}

#endif

static void wm_driver_w800_output_callback(wm_hal_pwm_dev_t *dev, uint8_t channel)
{
    wm_device_t *drv_dev = NULL;

    wm_hal_pwm_channel_cfg_t *cfg = NULL;

    wm_drv_pwm_data_t *data = NULL;
    wm_drv_pwm_out_report_t report;

    if (dev == NULL) {
        WM_DRV_PWM_ASSERT(0);
        return;
    }

    drv_dev = dev->master_dev;
    if (drv_dev == NULL) {
        WM_DRV_PWM_ASSERT(0);
        return;
    }

    memset(&report, 0x0, sizeof(report));

    data              = drv_dev->drv;
    cfg               = dev->cfg[channel];
    report.chan       = channel;
    report.period_num = cfg->vo.period_num;

    if (data->ctx.out_cb[channel].cb) {
        /* output done event report when autoreload is enabled or period_num == 1 */
        if (cfg->autoload || (!cfg->autoload && cfg->period_num == 1)) {
            data->ctx.out_cb[channel].cb(&report, data->ctx.out_cb[channel].cb_arg);
        }
    }

    return;
}

static void wm_driver_w800_input_callback(wm_hal_pwm_dev_t *dev, uint8_t channel)
{
    wm_device_t *drv_dev = NULL;

    wm_hal_pwm_channel_cfg_t *cfg = NULL;

    wm_drv_pwm_data_t *data = NULL;
    wm_drv_pwm_cap_report_t report;

    if (dev == NULL) {
        WM_DRV_PWM_ASSERT(0);
        return;
    }

    drv_dev = dev->master_dev;
    if (drv_dev == NULL) {
        WM_DRV_PWM_ASSERT(0);
        return;
    }

    data = drv_dev->drv;
    cfg  = dev->cfg[channel];

    /* wm_hal_dma_stop(cfg->dma_dev, cfg->dma_channel); */

    memset(&report, 0x0, sizeof(report));

    report.report_num = cfg->report_cap_size;
    report.chan       = channel;
    report.value      = (wm_drv_pwm_cap_data_report_t *)&cfg->vi;

    if (data->ctx.cap_cb[channel].cb) {
        data->ctx.cap_cb[channel].cb(&report, data->ctx.cap_cb[channel].cb_arg);
    }

    /* wm_hal_dma_start(cfg->dma_dev, cfg->dma_channel); */

    return;
}

static void wm_drv_pwm_w800_free_ch_cfg(wm_drv_pwm_data_t *data, uint8_t channel)
{
    wm_hal_pwm_channel_cfg_t *item, *next;
    wm_drv_pwm_ctx_t *ctx = &data->ctx;

    dl_list_for_each_safe(item, next, &data->ctx.hal_ch_cfg_list, wm_hal_pwm_channel_cfg_t, item)
    {
        if (channel == WM_PWM_CHANNEL_MAX || channel == item->channel) {
            dl_list_del(&item->item);
            if (item->dma_dev != NULL && item->int_type == WM_PWM_CAP_INT_DMA) {
                /* wm_hal_dma_stop(item->dma_dev, item->dma_channel); */
                /* wm_hal_dma_release_ch(item->dma_dev, item->dma_channel); */
                wm_drv_dma_stop(ctx->dma_dev, item->dma_channel);
                wm_drv_dma_release_ch(ctx->dma_dev, item->dma_channel, WM_DRV_PWM_MUTEX_TIMEOUT);
            }
            wm_os_internal_free(item);
            if (channel != WM_PWM_CHANNEL_MAX) {
                break;
            }
        }
    }
}

static int wm_drv_pwm_w800_init(wm_device_t *device)
{
    int i;
    int ret = WM_ERR_SUCCESS;

    wm_dt_hw_pwm_t *hw            = device->hw;
    wm_drv_pwm_data_t *data       = device->drv;
    wm_drv_pwm_ctx_t *ctx         = &data->ctx;
    wm_hal_pwm_dev_t *hal_pwm_dev = &data->hal_pwm_dev;

    ctx->hw                   = device->hw;
    ctx->clock_dev            = wm_dt_get_device_by_name("rcc");
    hal_pwm_dev->irq_num      = hw->irq_cfg.irq_num;
    hal_pwm_dev->master_dev   = device;
    hal_pwm_dev->reg_dev      = (wm_pwm_reg_t *)hw->reg_base;
    hal_pwm_dev->master_clock = wm_drv_rcc_get_config_clock(ctx->clock_dev, WM_RCC_TYPE_APB) * UNIT_MHZ;

    wm_drv_clock_enable(ctx->clock_dev, WM_RCC_PWM_GATE_EN);

    if (WM_ERR_SUCCESS != wm_hal_pwm_init(hal_pwm_dev)) {
        WM_DRV_PWM_LOG_MARKER;
        return WM_ERR_FAILED;
    }

    /* init channel config list as we should keep the memory for HAL */
    dl_list_init(&ctx->hal_ch_cfg_list);

    for (i = 0; i < hw->pin_cfg_count; i++) {
        ret = wm_drv_pwm_gpio_config(i, hw->pin_cfg[i].pin_num, hw->pin_cfg[i].pin_mux);
        if (WM_ERR_SUCCESS != ret) {
            for (; i >= 0; i--) {
                wm_drv_pwm_set_gpio_idle(i);
            }
            wm_hal_pwm_deinit(hal_pwm_dev);
            wm_drv_clock_disable(ctx->clock_dev, WM_RCC_PWM_GATE_EN);
            break;
        }
    }

    return ret;
}

static int wm_drv_pwm_w800_deinit(wm_drv_pwm_data_t *data)
{
    int ret               = WM_ERR_SUCCESS;
    wm_drv_pwm_ctx_t *ctx = &data->ctx;

    /* de-initialize PWM HAL resources */
    ret = wm_hal_pwm_deinit(&data->hal_pwm_dev);
    if (ret != WM_ERR_SUCCESS) {
        WM_DRV_PWM_LOG_MARKER;
        return ret;
    }

    for (int i = 0; i < ctx->hw->pin_cfg_count; i++) {
        wm_drv_pwm_set_gpio_idle(i);
    }

    wm_drv_clock_reset(ctx->clock_dev, WM_RCC_PWM_RST);

    wm_drv_clock_disable(ctx->clock_dev, WM_RCC_PWM_GATE_EN);

    /* free all channel configure memory */
    wm_drv_pwm_w800_free_ch_cfg(data, WM_PWM_CHANNEL_MAX);

    return WM_ERR_SUCCESS;
}

static int wm_drv_pwm_w800_channel_init(wm_drv_pwm_data_t *data, wm_drv_pwm_channel_cfg_t *dcfg)
{
    wm_hal_pwm_channel_cfg_t *cfg = NULL;
    wm_drv_pwm_ctx_t *ctx         = &data->ctx;
    wm_dma_data_t *dma_data       = ctx->dma_dev->drv;

    /* XXX: for the issue that W800 DMA will overwrite a word when capture high frequency input signal */
    uint32_t extra_size = sizeof(wm_hal_pwm_cap_report_t) * (dcfg->cap_cache_cnt + 1);

    cfg = wm_os_internal_calloc(1, sizeof(*cfg) + extra_size);
    if (cfg == NULL) {
        return WM_ERR_NO_MEM;
    }

    cfg->channel = dcfg->channel;
    cfg->mode    = dcfg->mode;
    if (cfg->mode == WM_PWM_IN_CAP) {
        cfg->cnt_type = WM_PWM_CNT_TYPE_INC;
    } else {
        cfg->cnt_type = WM_PWM_CNT_TYPE_DEC;
    }

    cfg->clkdiv           = dcfg->clkdiv;
    cfg->period           = dcfg->period_cycle;
    cfg->period_num       = dcfg->period_num;
    cfg->duty_cycle       = dcfg->duty_cycle;
    cfg->dead_time_clkdiv = dcfg->dead_time_clkdiv;
    if (cfg->dead_time_clkdiv > WM_DRV_PWM_DEADTIME_CLKDIV_MAX) {
        goto lerr;
    }

    cfg->dead_time_cnt = dcfg->dead_time_cnt;
    if (cfg->dead_time_cnt) {
        cfg->dead_time_en = true;
    }

    cfg->period_int  = dcfg->period_int;
    cfg->inverse     = dcfg->inverse;
    cfg->autoload    = dcfg->autoload;
    cfg->output_high = dcfg->output_high;
    cfg->int_type    = dcfg->int_type;

    cfg->report_cap_size = dcfg->cap_cache_cnt;
    if (cfg->mode == WM_PWM_IN_CAP) {
        if (cfg->report_cap_size == 0) {
            goto lerr;
        }
    }

    cfg->o_cb = wm_driver_w800_output_callback;
    cfg->i_cb = wm_driver_w800_input_callback;

    if (cfg->int_type == WM_PWM_CAP_INT_DMA) {
        uint8_t dma_ch = 0;
        cfg->dma_dev   = &dma_data->hal_dev;
        if (WM_ERR_SUCCESS != wm_drv_dma_request_ch(ctx->dma_dev, &dma_ch, WM_DRV_PWM_MUTEX_TIMEOUT)) {
            goto lerr;
        }
        cfg->dma_channel = dma_ch;
    }

    if (WM_ERR_SUCCESS != wm_hal_pwm_channel_init(&data->hal_pwm_dev, cfg)) {
        WM_DRV_PWM_LOG_MARKER;
        goto free_channel;
    }

    wm_drv_pwm_w800_free_ch_cfg(data, cfg->channel);

    dl_list_add_tail(&data->ctx.hal_ch_cfg_list, &cfg->item);

    WM_DRV_PWM_LOG_D("pwm channel initialize successfully!");

    return WM_ERR_SUCCESS;

free_channel:
    if (cfg->int_type == WM_PWM_CAP_INT_DMA) {
        wm_drv_dma_release_ch(ctx->dma_dev, cfg->dma_channel, WM_DRV_PWM_MUTEX_TIMEOUT);
    }
lerr:
    wm_os_internal_free(cfg);

    return WM_ERR_FAILED;
}

static int wm_drv_pwm_w800_channel_deinit(wm_drv_pwm_data_t *data, uint8_t channel)
{
    if (WM_ERR_SUCCESS != wm_hal_pwm_channel_deinit(&data->hal_pwm_dev, channel)) {
        return WM_ERR_INVALID_PARAM;
    }

    wm_drv_pwm_w800_free_ch_cfg(data, channel);

    WM_DRV_PWM_LOG_D("pwm channel de-initialize successfully!");

    return WM_ERR_SUCCESS;
}

static int wm_drv_pwm_w800_channel_start(wm_drv_pwm_data_t *data, uint8_t channel)
{
    int ret = wm_hal_pwm_chan_start(&data->hal_pwm_dev, channel);
    if (ret != WM_ERR_SUCCESS) {
        WM_DRV_PWM_LOG_MARKER;
    }

    return ret;
}

static int wm_drv_pwm_w800_channel_stop(wm_drv_pwm_data_t *data, uint8_t channel)
{
    int ret = wm_hal_pwm_chan_stop(&data->hal_pwm_dev, channel);
    if (ret != WM_ERR_SUCCESS) {
        WM_DRV_PWM_LOG_MARKER;
    }

    return ret;
}

static int wm_drv_pwm_w800_channels_start(wm_drv_pwm_data_t *data)
{
    int ret = wm_hal_pwm_chans_start(&data->hal_pwm_dev);
    if (ret != WM_ERR_SUCCESS) {
        WM_DRV_PWM_LOG_MARKER;
    }

    return ret;
}

static int wm_drv_pwm_w800_channels_stop(wm_drv_pwm_data_t *data)
{
    int ret = wm_hal_pwm_chans_stop(&data->hal_pwm_dev);
    if (ret != WM_ERR_SUCCESS) {
        WM_DRV_PWM_LOG_MARKER;
    }

    return ret;
}

static int wm_drv_pwm_w800_ioctl(wm_drv_pwm_data_t *data, uint8_t cmd, wm_drv_pwm_ioctl_args_t *arg)
{
    int ret = WM_ERR_SUCCESS;

    switch (cmd) {
        case WM_IOCTL_PWM_CMD_SET_FREQ:
        {
            if (arg->u.freq_hz < WM_DRV_PWM_FREQ_MIN || arg->u.freq_hz > WM_DRV_PWM_FREQ_MAX) {
                ret = WM_ERR_FAILED;
                break;
            }

            /* XXX:
             * this may return fail while can not find divider and period,
             * so we can add more step for "approximate value" without
             * "exactly equal" check during binary search algo
             * */
            ret = wm_hal_pwm_set_freq(&data->hal_pwm_dev, arg->channel, arg->u.freq_hz);
            break;
        }
        case WM_IOCTL_PWM_CMD_GET_FREQ:
        {
            ret = wm_hal_pwm_get_freq(&data->hal_pwm_dev, arg->channel, &arg->u.freq_hz);
            break;
        }
        case WM_IOCTL_PWM_CMD_SET_FREQ_CYCLE:
        {
            ret = wm_hal_pwm_set_freq_inner(&data->hal_pwm_dev, arg->channel, arg->u.freq_cycle.period_cycle,
                                            arg->u.freq_cycle.clkdiv);
            break;
        }
        case WM_IOCTL_PWM_CMD_GET_FREQ_CYCLE:
        {
            ret = wm_hal_pwm_get_period(&data->hal_pwm_dev, arg->channel, &arg->u.freq_cycle.period_cycle);
            if (ret == WM_ERR_SUCCESS) {
                ret = wm_hal_pwm_get_clkdiv(&data->hal_pwm_dev, arg->channel, &arg->u.freq_cycle.clkdiv);
            }
            break;
        }
        case WM_IOCTL_PWM_CMD_SET_DUTY_CYCLE:
        {
            ret = wm_hal_pwm_set_duty_cycle(&data->hal_pwm_dev, arg->channel, arg->u.duty_cycle);
            break;
        }
        case WM_IOCTL_PWM_CMD_GET_DUTY_CYCLE:
        {
            ret = wm_hal_pwm_get_duty_cycle(&data->hal_pwm_dev, arg->channel, &arg->u.duty_cycle);
            break;
        }
        case WM_IOCTL_PWM_CMD_SET_PERIOD_NUM:
        {
            ret = wm_hal_pwm_set_period_num(&data->hal_pwm_dev, arg->channel, arg->u.period_num);
            break;
        }
        case WM_IOCTL_PWM_CMD_GET_PERIOD_NUM:
        {
            ret = wm_hal_pwm_get_period_num(&data->hal_pwm_dev, arg->channel, &arg->u.period_num);
            break;
        }
        case WM_IOCTL_PWM_CMD_SET_PERIOD_INT:
        {
            ret = wm_hal_pwm_set_period_int(&data->hal_pwm_dev, arg->channel, arg->u.period_int);
            break;
        }
        case WM_IOCTL_PWM_CMD_GET_PERIOD_INT:
        {
            ret = wm_hal_pwm_get_period_int(&data->hal_pwm_dev, arg->channel, &arg->u.period_int);
            break;
        }
        case WM_IOCTL_PWM_CMD_SET_INVERSE:
        {
            ret = wm_hal_pwm_set_inverse(&data->hal_pwm_dev, arg->channel, arg->u.inverse);
            break;
        }
        case WM_IOCTL_PWM_CMD_GET_INVERSE:
        {
            ret = wm_hal_pwm_get_inverse(&data->hal_pwm_dev, arg->channel, &arg->u.inverse);
            break;
        }
        case WM_IOCTL_PWM_CMD_SET_BRAKE_LEVEL:
        {
            ret = wm_hal_pwm_set_brake_level(&data->hal_pwm_dev, arg->channel, arg->u.brake_level_high);
            break;
        }
        case WM_IOCTL_PWM_CMD_GET_BRAKE_LEVEL:
        {
            ret = wm_hal_pwm_get_brake_level(&data->hal_pwm_dev, arg->channel, &arg->u.brake_level_high);
            break;
        }
        case WM_IOCTL_PWM_CMD_SET_DEADTIME:
        {
            if (arg->u.deadtime.clkdiv > WM_DRV_PWM_DEADTIME_CLKDIV_MAX) {
                ret = WM_ERR_INVALID_PARAM;
                break;
            }
            ret = wm_hal_pwm_set_deadtime(&data->hal_pwm_dev, arg->channel, arg->u.deadtime.cnt, arg->u.deadtime.clkdiv);
            break;
        }
        case WM_IOCTL_PWM_CMD_GET_DEADTIME:
        {
            ret =
                wm_hal_pwm_get_deadtime(&data->hal_pwm_dev, arg->channel, &arg->u.deadtime.cnt, &arg->u.deadtime.clkdiv, NULL);
            break;
        }
        case WM_IOCTL_PWM_CMD_SET_AUTORELOAD:
        {
            ret = wm_hal_pwm_set_autoreload_en(&data->hal_pwm_dev, arg->channel, arg->u.autoreload);
            break;
        }
        case WM_IOCTL_PWM_CMD_GET_AUTORELOAD:
        {
            ret = wm_hal_pwm_get_autoreload_en(&data->hal_pwm_dev, arg->channel, &arg->u.autoreload);
            break;
        }
        case WM_IOCTL_PWM_CMD_SET_CAP_CB:
        {
            wm_drv_pwm_ctx_t *ctx = &data->ctx;
            if (arg->channel >= WM_PWM_CHANNEL_MAX) {
                ret = WM_ERR_INVALID_PARAM;
                break;
            }
            ctx->cap_cb[arg->channel].cb     = arg->u.report_cb.cb;
            ctx->cap_cb[arg->channel].cb_arg = arg->u.report_cb.cb_arg;
            break;
        }
        case WM_IOCTL_PWM_CMD_SET_OUT_CB:
        {
            wm_drv_pwm_ctx_t *ctx = &data->ctx;
            if (arg->channel >= WM_PWM_CHANNEL_MAX) {
                ret = WM_ERR_INVALID_PARAM;
                break;
            }
            ctx->out_cb[arg->channel].cb     = arg->u.report_cb.cb;
            ctx->out_cb[arg->channel].cb_arg = arg->u.report_cb.cb_arg;
            break;
        }
        case WM_IOCTL_PWM_CMD_SHW_CFGS:
        {
            wm_hal_pwm_channel_cfg_t *cfg = NULL;

            for (int i = WM_PWM_CHANNEL_0; i < WM_PWM_CHANNEL_MAX; i++) {
                if (wm_hal_pwm_get_chan_cfg(&data->hal_pwm_dev, i, &cfg)) {
                    continue;
                }
                dump_cfgs(cfg);
            }
            wm_drv_pwm_regs_dumps(&data->hal_pwm_dev);
            break;
        }
    }

    return ret;
}

const wm_drv_pwm_ops_t wm_drv_pwm_ops = {
    .init              = wm_drv_pwm_w800_init,
    .deinit            = wm_drv_pwm_w800_deinit,
    .channel_init      = wm_drv_pwm_w800_channel_init,
    .channel_deinit    = wm_drv_pwm_w800_channel_deinit,
    .channel_start     = wm_drv_pwm_w800_channel_start,
    .channel_stop      = wm_drv_pwm_w800_channel_stop,
    .channel_start_all = wm_drv_pwm_w800_channels_start,
    .channel_stop_all  = wm_drv_pwm_w800_channels_stop,
    .ioctl             = wm_drv_pwm_w800_ioctl,
};
