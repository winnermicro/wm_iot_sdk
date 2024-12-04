/**
 * @file wm_hal_pwm.c
 *
 * @brief PWM HAL Module
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
#include <string.h>
#include "wm_list.h"
#include "wm_ll_pwm.h"
#include "wm_hal_pwm.h"
#include "wm_pwm_reg_struct.h"
#include "wm_soc.h"
#include "wm_soc_cfgs.h"
#include "wm_types.h"
#include "wm_error.h"
#include "wm_hal_dma.h"
#include "wm_hal_irq.h"

#define WM_HAL_PWM_DEBUG 0

#if WM_HAL_PWM_DEBUG
#include "wm_log.h"
#undef LOG_TAG
#define LOG_TAG               "PWM HAL"
#define WM_HAL_PWM_LOG_D(...) wm_log_info(__VA_ARGS__)
#define WM_HAL_PWM_LOG_I(...) wm_log_info(__VA_ARGS__)
#define WM_HAL_PWM_LOG_E(...) wm_log_error(__VA_ARGS__)
#define WM_HAL_PWM_LOG_MARKER WM_HAL_PWM_LOG_E("%s %d", __func__, __LINE__)
#else
#define WM_HAL_PWM_LOG_D(...) \
    do {                      \
    } while (0)
#define WM_HAL_PWM_LOG_I(...) \
    do {                      \
    } while (0)
#define WM_HAL_PWM_LOG_E(...) \
    do {                      \
    } while (0)
#define WM_HAL_PWM_LOG_MARKER

#endif

/* check if both rising and falling edge data is ready */
#define WM_HAL_PWM_EDGE_MASK (0x3)

typedef int (*pwm_moder)(wm_hal_pwm_dev_t *dev, uint8_t channel);

static wm_hal_pwm_channel_cfg_t *wm_hal_pwm_check_channel_cfg(wm_hal_pwm_dev_t *dev, uint8_t channel);

static int wm_hal_pwm_init_out_independent(wm_hal_pwm_dev_t *dev, uint8_t channel);
static int wm_hal_pwm_init_out_2ch_sync(wm_hal_pwm_dev_t *dev, uint8_t channel);
static int wm_hal_pwm_init_out_allch_sync(wm_hal_pwm_dev_t *dev, uint8_t channel);
static int wm_hal_pwm_init_out_complement(wm_hal_pwm_dev_t *dev, uint8_t channel);
static int wm_hal_pwm_init_out_brake(wm_hal_pwm_dev_t *dev, uint8_t channel);
static int wm_hal_pwm_init_in_cap(wm_hal_pwm_dev_t *dev, uint8_t channel);

static pwm_moder pwm_moders[WM_PWM_MODE_MAX] = {
    wm_hal_pwm_init_out_independent, wm_hal_pwm_init_out_2ch_sync, wm_hal_pwm_init_out_allch_sync,
    wm_hal_pwm_init_out_complement,  wm_hal_pwm_init_out_brake,    wm_hal_pwm_init_in_cap,
};

static void wm_hal_pwm_output_irq_cb(void *arg)
{
    wm_hal_pwm_channel_cfg_t *cfg = NULL;
    wm_hal_pwm_dev_t *dev         = (wm_hal_pwm_dev_t *)arg;

    for (int i = WM_PWM_CHANNEL_0; i < WM_PWM_CHANNEL_MAX; i++) {
        if (!wm_ll_pwm_get_period_int_status(dev->reg_dev, i)) {
            continue;
        }

        cfg = dev->cfg[i];

        if (cfg->o_cb && cfg->autoload) {
            memset(&cfg->vo, 0x0, sizeof(cfg->vo));
            cfg->vo.period_num = wm_ll_pwm_get_period_num(dev->reg_dev, i);
            cfg->o_cb(dev, i);
        }

        /* clear the output period num interrupt */
        wm_ll_pwm_clear_period_int_status(dev->reg_dev, i);
    }
}

static inline void wm_hal_pwm_get_falling_int_data(wm_hal_pwm_dev_t *dev, uint8_t channel, bool single)
{
    wm_hal_pwm_channel_cfg_t *cfg = dev->cfg[channel];

    uint8_t f = cfg->int_done.u.e.falling_done_cnt;
    uint8_t r = cfg->int_done.u.e.rising_done_cnt;

    /* check if falling edge data ready, and get the ready data */
    if (!wm_ll_pwm_get_cap_falling_int_status(dev->reg_dev, channel)) {
        return;
    }
    /* no falling edge interrupt delay / overrun happend before */
    if (cfg && !wm_ll_pwm_get_falling_int_overrun_status(dev->reg_dev, channel)) {
        cfg->vi[f].fval = wm_ll_pwm_get_cap_value(dev->reg_dev, channel, false);
        if (single) {
            cfg->vi[r].rval = wm_ll_pwm_get_cap_value(dev->reg_dev, channel, true);
            r++;
        }

        f++;

        f                                  = f <= cfg->report_cap_size ? f : cfg->report_cap_size;
        r                                  = r <= cfg->report_cap_size ? r : cfg->report_cap_size;
        cfg->int_done.u.e.falling_done_cnt = f;
        cfg->int_done.u.e.rising_done_cnt  = r;
    }

    /* Clear the corresponding interrupt status. */
    wm_ll_pwm_clear_cap_falling_int_status(dev->reg_dev, channel);
}

static inline void wm_hal_pwm_get_rising_int_data(wm_hal_pwm_dev_t *dev, uint8_t channel, bool single)
{
    wm_hal_pwm_channel_cfg_t *cfg = dev->cfg[channel];

    uint8_t f = cfg->int_done.u.e.falling_done_cnt;
    uint8_t r = cfg->int_done.u.e.rising_done_cnt;

    /* check if rising edge data ready, and get the ready data */
    if (!wm_ll_pwm_get_cap_rising_int_status(dev->reg_dev, channel)) {
        return;
    }

    /* no rising edge interrupt delay / overrun happend before */
    if (cfg && !wm_ll_pwm_get_rising_int_overrun_status(dev->reg_dev, channel)) {
        cfg->vi[r].rval = wm_ll_pwm_get_cap_value(dev->reg_dev, channel, true);
        if (single) {
            cfg->vi[f].fval = wm_ll_pwm_get_cap_value(dev->reg_dev, channel, false);
            f++;
        }

        r++;

        f                                  = f <= cfg->report_cap_size ? f : cfg->report_cap_size;
        r                                  = r <= cfg->report_cap_size ? r : cfg->report_cap_size;
        cfg->int_done.u.e.falling_done_cnt = f;
        cfg->int_done.u.e.rising_done_cnt  = r;
    }

    /* Clear the corresponding interrupt status. */
    wm_ll_pwm_clear_cap_rising_int_status(dev->reg_dev, channel);
}

/*
 * The PWM capture first generates a falling edge, followed by a rising edge.
 * Therefore,for one complete sampling,
 * at least one non-overrun falling edge and one non-overrun rising edge
 * need to be captured to be valid.
 *
 * */
static void wm_hal_pwm_get_edge_int_data(wm_hal_pwm_dev_t *dev, uint8_t channel)
{
    wm_hal_pwm_get_falling_int_data(dev, channel, false);
    wm_hal_pwm_get_rising_int_data(dev, channel, false);
}

static void wm_hal_pwm_ch_input_irq_cb(wm_hal_pwm_dev_t *dev, uint8_t channel)
{
    /* XXX:TODO:
     * When collecting low-frequency inputs with a high sampling frequency,
     * counter flipping may occur, so this scenario needs to be considered.
     *
     * counter overflow, only need to clear this status
     * */
    if (wm_ll_pwm_get_cnt_overflow_status(dev->reg_dev, channel)) {
        wm_ll_pwm_clear_cap_rising_int_status(dev->reg_dev, channel);
        wm_ll_pwm_clear_cap_falling_int_status(dev->reg_dev, channel);
        return;
    }

    /* capture function is not enabled for this channel */
    if (!wm_ll_pwm_get_capture_en(dev->reg_dev, channel)) {
        return;
    }

    wm_hal_pwm_channel_cfg_t *cfg = NULL;
    cfg                           = wm_hal_pwm_check_channel_cfg(dev, channel);
    if (cfg == NULL) {
        return;
    }

    if (cfg->mode != WM_PWM_IN_CAP) {
        wm_ll_pwm_clear_cap_rising_int_status(dev->reg_dev, channel);
        wm_ll_pwm_clear_cap_falling_int_status(dev->reg_dev, channel);
        return;
    }

    switch (cfg->int_type) {
        case WM_PWM_CAP_INT_FALLING:
        {
            wm_hal_pwm_get_falling_int_data(dev, channel, true);
            break;
        }
        case WM_PWM_CAP_INT_RISING:
        {
            wm_hal_pwm_get_rising_int_data(dev, channel, true);
            break;
        }
        case WM_PWM_CAP_INT_RIGSING_FALLING:
        {
            wm_hal_pwm_get_edge_int_data(dev, channel);
            break;
        }
        case WM_PWM_CAP_INT_DMA:
        default:
        {
            wm_ll_pwm_clear_cap_rising_int_status(dev->reg_dev, channel);
            wm_ll_pwm_clear_cap_falling_int_status(dev->reg_dev, channel);
            return;
        }
    }

    if (cfg->int_done.u.edge_done_cnt != (cfg->report_cap_size << 8 | cfg->report_cap_size)) {
        return;
    }

    /* report data to user context */
    if (cfg->i_cb) {
        cfg->i_cb(dev, channel);
    }
    cfg->int_done.u.edge_done_cnt = 0;
}

static void wm_hal_pwm_input_irq_cb(void *arg)
{
    wm_hal_pwm_dev_t *dev = (wm_hal_pwm_dev_t *)arg;

    /* only ch0 and ch4 support capture mode */
    wm_hal_pwm_ch_input_irq_cb(dev, WM_PWM_CHANNEL_0);
    wm_hal_pwm_ch_input_irq_cb(dev, WM_PWM_CHANNEL_4);
}

static void wm_hal_pwm_dma_done_cb(wm_dma_ch_t ch, uint32_t sts, void *user_data)
{
    wm_hal_pwm_channel_cfg_t *cfg = NULL;
    wm_hal_pwm_dev_t *dev         = (wm_hal_pwm_dev_t *)user_data;

    for (int i = WM_PWM_CHANNEL_0; i < WM_PWM_CHANNEL_MAX; i++) {
        cfg = wm_hal_pwm_check_channel_cfg(dev, i);
        if (cfg && cfg->dma_channel == ch && cfg->i_cb) {
            cfg->i_cb(dev, i);
            /* XXX: DMA issue, we SHOULD NOT use reload mode of DMA which will cause memory overwrite by DMA */
            wm_hal_dma_start(cfg->dma_dev, cfg->dma_channel);
            break;
        }
    }
}

/* XXX: set interrupt enable / disable should match with toggle int irq */
static void wm_hal_pwm_set_cap_int(wm_hal_pwm_dev_t *dev, uint8_t channel, bool rising, bool falling, bool dma)
{
    wm_ll_pwm_set_dma_request_int_en(dev->reg_dev, channel, dma);
    wm_ll_pwm_set_falling_int_en(dev->reg_dev, channel, falling);
    wm_ll_pwm_set_rising_int_en(dev->reg_dev, channel, rising);
}

/* XXX: set interrupt enable / disable should match with toggle int irq */
static void wm_hal_pwm_set_out_int(wm_hal_pwm_dev_t *dev, uint8_t channel, bool en)
{
    wm_ll_pwm_set_period_int_en(dev->reg_dev, channel, en);
}

static void wm_hal_pwm_clear_channel_int(wm_hal_pwm_dev_t *dev, uint8_t channel)
{
    /* XXX: disable interrupt first in case counter overflow */
    wm_hal_pwm_set_cap_int(dev, channel, false, false, false);
    wm_hal_pwm_set_out_int(dev, channel, false);
    wm_ll_pwm_clear_period_int_status(dev->reg_dev, channel);
    wm_ll_pwm_clear_cap_falling_int_status(dev->reg_dev, channel);
    wm_ll_pwm_clear_cap_rising_int_status(dev->reg_dev, channel);
}

static bool wm_hal_pwm_check_dev(wm_hal_pwm_dev_t *dev)
{
    if (dev == NULL) {
        return false;
    } else {
        return true;
    }
}

static wm_hal_pwm_channel_cfg_t *wm_hal_pwm_check_channel_cfg(wm_hal_pwm_dev_t *dev, uint8_t channel)
{
    if (channel >= WM_PWM_CHANNEL_MAX) {
        return NULL;
    }

    if (wm_hal_pwm_check_dev(dev)) {
        return dev->cfg[channel];
    }

    return NULL;
}

static uint8_t wm_hal_pwm_chans2bits(uint8_t mode, uint8_t channel)
{
    uint8_t bits = 0;

    if (mode == WM_PWM_OUT_ALLSYNC || channel == WM_PWM_CHANNEL_MAX) {
        bits |= (WM_BIT(WM_PWM_CHANNEL_MAX) - 1);
    } else {
        bits = WM_BIT(channel);
        if (mode == WM_PWM_OUT_2SYNC || mode == WM_PWM_OUT_MC) {
            bits |= WM_BIT((channel + 1));
        }
    }

    return bits;
}

static uint8_t wm_hal_pwm_get_chans_bits(wm_hal_pwm_dev_t *dev, uint8_t channel)
{
    uint8_t bits = 0;

    wm_hal_pwm_channel_cfg_t *cfg = NULL;

    do {
        if (!wm_hal_pwm_check_dev(dev)) {
            break;
        }

        if (channel < WM_PWM_CHANNEL_MAX) {
            cfg = dev->cfg[channel];
            if (cfg == NULL) {
                break;
            }
            bits = wm_hal_pwm_chans2bits(cfg->mode, channel);
            break;
        }

        for (int i = WM_PWM_CHANNEL_0; i < WM_PWM_CHANNEL_MAX; i++) {
            cfg = dev->cfg[i];
            if (cfg == NULL) {
                continue;
            }

            bits |= wm_hal_pwm_chans2bits(cfg->mode, i);
        }
    } while (0);

    return bits;
}

static uint8_t wm_hal_pwm_clr_chans_issue_bits(wm_hal_pwm_dev_t *dev)
{
    uint8_t bits = 0;

    wm_hal_pwm_channel_cfg_t *cfg = NULL;

    for (int i = WM_PWM_CHANNEL_0; i < WM_PWM_CHANNEL_MAX; i++) {
        cfg = dev->cfg[i];
        if (cfg == NULL) {
            continue;
        }
        if (cfg->mode == WM_PWM_OUT_BRAKE && cfg->output_high) {
            bits |= wm_hal_pwm_chans2bits(cfg->mode, i);
        }
    }

    return ~bits;
}

static int wm_hal_pwm_find_clkdiv(wm_hal_pwm_dev_t *dev, uint32_t freq, uint8_t period, uint16_t *clkdiv)
{
    int ret = 0;
    float freq_temp;
    uint16_t mid_clockdiv;
    uint32_t master_temp;
    uint32_t left_clkdiv  = WM_PWM_MIN_CLKDIV;
    uint32_t right_clkdiv = WM_PWM_MAX_CLKDIV;

    while (left_clkdiv <= right_clkdiv) {
        mid_clockdiv = left_clkdiv + ((right_clkdiv - left_clkdiv) >> 1);

        if (!mid_clockdiv) {
            return -1;
        }

        freq_temp = dev->master_clock * 1.0 / (period + 1) / mid_clockdiv;
        /* XXX: uncomment this line for approximate value */
        /* master_temp = (uint32_t)(freq_temp * (period + 1) * mid_clockdiv); */
        master_temp = (uint32_t)freq_temp * ((period + 1) * mid_clockdiv);

        if (freq == (uint32_t)freq_temp && dev->master_clock == master_temp) {
            *clkdiv = mid_clockdiv;
            return 0;
        } else if (freq > freq_temp) {
            right_clkdiv = mid_clockdiv - 1;
            ret          = -1;
        } else {
            left_clkdiv = mid_clockdiv + 1;
            ret         = 1;
        }
    }

    return ret;
}

static int wm_hal_pwm_find_period_clkdiv(wm_hal_pwm_dev_t *dev, uint32_t freq, uint8_t *period, uint16_t *clkdiv)
{
    int ret              = WM_ERR_FAILED;
    uint16_t high_period = WM_PWM_MAX_PERIOD;

    for (; high_period >= 0; high_period--) {
        ret = wm_hal_pwm_find_clkdiv(dev, freq, (uint8_t)high_period, clkdiv);
        if (ret == 0) {
            *period = high_period;
            return 0;
        }

        if (high_period == 0) {
            break;
        }
    }

    return WM_ERR_NOT_ALLOWED;
}

static int wm_hal_pwm_init_out_common(wm_hal_pwm_dev_t *dev, uint8_t channel)
{
    wm_hal_pwm_channel_cfg_t *cfg = dev->cfg[channel];
    enum wm_pwm_cnt_type ctype    = cfg->cnt_type;

    /* check if the PWM counter type matched with current mode */
    if (ctype != WM_PWM_CNT_TYPE_DEC && ctype != WM_PWM_CNT_TYPE_CENTER) {
        WM_HAL_PWM_LOG_MARKER;
        return WM_ERR_INVALID_PARAM;
    }

    /* disable capture mode */
    wm_ll_pwm_set_capture_en(dev->reg_dev, channel, false);
    wm_ll_pwm_set_capture_inv_en(dev->reg_dev, channel, false);

    /* set the clock divider for the specified channel */
    wm_ll_pwm_set_clkdiv(dev->reg_dev, channel, cfg->clkdiv);

    /* set the period for the specified channel */
    wm_ll_pwm_set_period(dev->reg_dev, channel, cfg->period);

    /* set CMP counter (for duty) register config item */
    wm_ll_pwm_set_cmp_cnt(dev->reg_dev, channel, cfg->duty_cycle);

    /* stop when period count reached */
    wm_ll_pwm_set_period_num(dev->reg_dev, channel, cfg->period_num);

    /* enable / disable period count reached interrupt */
    wm_hal_pwm_set_out_int(dev, channel, cfg->period_int);

    /* set counter type */
    wm_ll_pwm_set_cnt_type(dev->reg_dev, channel, ctype);

    /* set auto reload config */
    wm_ll_pwm_set_autoload_mode_en(dev->reg_dev, channel, cfg->autoload);

    /* outen for output mode when channel is ch0 and ch4 */
    wm_ll_pwm_set_ch_output_state(dev->reg_dev, channel, WM_PWM_OUT_EN_STATE_OUT);

    /* enable / disable ploarity inverse */
    wm_ll_pwm_set_output_ploarity_inv_en(dev->reg_dev, channel, cfg->inverse);

    return WM_ERR_SUCCESS;
}

static int wm_hal_pwm_init_out_independent(wm_hal_pwm_dev_t *dev, uint8_t channel)
{
    /* wm_hal_pwm_channel_cfg_t *cfg = dev->cfg[channel]; */

    /* XXX:
     * shoudl disable all channel sync mode for chan4?,
     * all sync mode just for chan0, chan1, chan2, chan3 in spec?
     * but old sdk include chan4
     *
     * if (channel != WM_PWM_CHANNEL_4) {}
     * */
    wm_ll_pwm_set_allsync_mode_en(dev->reg_dev, false);

    /* disable two channel sync channel mode */
    wm_ll_pwm_set_twosync_mode_en(dev->reg_dev, channel, false);

    /* disalbe complement mode for the channel pairs */
    wm_ll_pwm_set_complement_en(dev->reg_dev, channel, false);

    /* disable brake output mode for this channel */
    wm_ll_pwm_set_brake_en(dev->reg_dev, channel, false);

    /* disable dead time zone for complement */
    wm_ll_pwm_set_dead_time_en(dev->reg_dev, channel, false);

    return wm_hal_pwm_init_out_common(dev, channel);
}

static int wm_hal_pwm_init_out_2ch_sync(wm_hal_pwm_dev_t *dev, uint8_t channel)
{
    /* configure two channel sync via set the primary channel */
    if (channel != WM_PWM_CHANNEL_0 && channel != WM_PWM_CHANNEL_2) {
        WM_HAL_PWM_LOG_MARKER;
        return WM_ERR_INVALID_PARAM;
    }

    /* disable all channel sync channel mode */
    wm_ll_pwm_set_allsync_mode_en(dev->reg_dev, false);

    /* disalbe complement mode for the channel pairs */
    wm_ll_pwm_set_complement_en(dev->reg_dev, channel, false);

    /* disable brake output mode for this channel */
    wm_ll_pwm_set_brake_en(dev->reg_dev, channel, false);

    /* enable two channel sync channel mode */
    wm_ll_pwm_set_twosync_mode_en(dev->reg_dev, channel, true);

    return wm_hal_pwm_init_out_common(dev, channel);
}

static int wm_hal_pwm_init_out_allch_sync(wm_hal_pwm_dev_t *dev, uint8_t channel)
{
    wm_hal_pwm_channel_cfg_t *cfg = dev->cfg[channel];
    if (channel != WM_PWM_CHANNEL_0) {
        /* all channel sync will copy the cfg of ch0???? */
        dev->cfg[WM_PWM_CHANNEL_0] = cfg;
    }

    /* disable two channel sync channel mode */
    wm_ll_pwm_set_twosync_mode_en(dev->reg_dev, WM_PWM_CHANNEL_0, false);
    wm_ll_pwm_set_twosync_mode_en(dev->reg_dev, WM_PWM_CHANNEL_2, false);

    /* disalbe complement mode for the channel pairs */
    wm_ll_pwm_set_complement_en(dev->reg_dev, WM_PWM_CHANNEL_0, false);
    wm_ll_pwm_set_complement_en(dev->reg_dev, WM_PWM_CHANNEL_2, false);

    /* disable brake output mode for all channels */
    for (int i = WM_PWM_CHANNEL_0; i < WM_PWM_CHANNEL_MAX; i++) {
        wm_ll_pwm_set_brake_en(dev->reg_dev, i, false);
    }

    /* XXX: should set ch4 to OUT??? */
    wm_ll_pwm_set_ch_output_state(dev->reg_dev, WM_PWM_CHANNEL_4, WM_PWM_OUT_EN_STATE_OUT);

    /* disable dead time zone for complement */
    wm_ll_pwm_set_dead_time_en(dev->reg_dev, channel, false);

    /* enable all channel sync channel mode */
    wm_ll_pwm_set_allsync_mode_en(dev->reg_dev, true);

    /* XXX: default force to channel 0 in all channel sync mode? */
    return wm_hal_pwm_init_out_common(dev, WM_PWM_CHANNEL_0);
}

static int wm_hal_pwm_init_out_complement(wm_hal_pwm_dev_t *dev, uint8_t channel)
{
    wm_hal_pwm_channel_cfg_t *cfg = dev->cfg[channel];

    if (channel == WM_PWM_CHANNEL_4) {
        return WM_ERR_INVALID_PARAM;
    }

    /* disable two channel sync channel mode */
    wm_ll_pwm_set_twosync_mode_en(dev->reg_dev, channel, false);

    /* disable brake output mode for this channel */
    wm_ll_pwm_set_brake_en(dev->reg_dev, channel, false);

    /* disable all channel sync channel mode */
    wm_ll_pwm_set_allsync_mode_en(dev->reg_dev, false);

    /* set dead time zone clock divider, clkdiv should within [0 ~ 3] */
    wm_ll_pwm_set_dead_time_clkdiv(dev->reg_dev, cfg->dead_time_clkdiv);

    /* set dead time clock cycle count which is decided by dead_time_clkdiv */
    wm_ll_pwm_set_dead_time_cnt(dev->reg_dev, channel, cfg->dead_time_cnt);

    /* enable / disable dead time zone for complement */
    wm_ll_pwm_set_dead_time_en(dev->reg_dev, channel, cfg->dead_time_en);

    WM_HAL_PWM_LOG_D("dead_time_cnt:%d, dead_time_clkdiv:%d, en:%d", cfg->dead_time_cnt, cfg->dead_time_clkdiv,
                     cfg->dead_time_en);

    /* enable complement mode for the channel pairs */
    wm_ll_pwm_set_complement_en(dev->reg_dev, channel, true);

    return wm_hal_pwm_init_out_common(dev, channel);
}

static int wm_hal_pwm_init_out_brake(wm_hal_pwm_dev_t *dev, uint8_t channel)
{
    wm_hal_pwm_channel_cfg_t *cfg = dev->cfg[channel];

    /* disable two channel sync channel mode */
    wm_ll_pwm_set_twosync_mode_en(dev->reg_dev, channel, false);

    /* disable all channel sync channel mode */
    wm_ll_pwm_set_allsync_mode_en(dev->reg_dev, false);

    /* enable complement mode for the channel pairs */
    wm_ll_pwm_set_complement_en(dev->reg_dev, channel, false);

    /* set output high / low level for brake mode */
    wm_ll_pwm_set_brake_level(dev->reg_dev, channel, cfg->output_high);

    /* disable dead time zone for complement */
    wm_ll_pwm_set_dead_time_en(dev->reg_dev, channel, false);

    /* enable brake output mode for this channel */
    wm_ll_pwm_set_brake_en(dev->reg_dev, channel, true);

    /* clear unused variable */
    cfg->period           = 0;
    cfg->clkdiv           = 0;
    cfg->period_num       = 0;
    cfg->duty_cycle       = 0;
    cfg->dead_time_clkdiv = 0;
    cfg->dead_time_cnt    = 0;
    cfg->dead_time_en     = false;
    cfg->period_int       = false;
    cfg->inverse          = false;

    return wm_hal_pwm_init_out_common(dev, channel);
}

static int wm_hal_pwm_init_in_cap_dma(wm_hal_pwm_dev_t *dev, uint8_t channel)
{
    wm_hal_pwm_channel_cfg_t *cfg = NULL;

    cfg = wm_hal_pwm_check_channel_cfg(dev, channel);
    if (cfg == NULL) {
        return WM_ERR_FAILED;
    }

    if (cfg->dma_channel > WM_DMA_CH_MAX) {
        return WM_ERR_FAILED;
    }

    if (cfg->dma_dev == NULL) {
        return WM_ERR_FAILED;
    }

    memset(&cfg->dma_cfg, 0x0, sizeof(cfg->dma_cfg));

    switch (channel) {
        case WM_PWM_CHANNEL_0:
        {
            cfg->dma_cfg.extend_ctrl.req_sel = WM_DMA_PWM_CAP0_REQ;
            break;
        }
        case WM_PWM_CHANNEL_4:
        {
            cfg->dma_cfg.extend_ctrl.req_sel = WM_DMA_PWM_CAP1_REQ;
            break;
        }
        case WM_PWM_CHANNEL_1:
        case WM_PWM_CHANNEL_2:
        case WM_PWM_CHANNEL_3:
        default:
        {
            return WM_ERR_FAILED;
        }
    }

    cfg->dma_cfg.src_addr = wm_ll_pwm_get_cap_addr(dev->reg_dev, channel);
    if (cfg->dma_cfg.src_addr == 0) {
        WM_HAL_PWM_LOG_MARKER;
        return WM_ERR_FAILED;
    }

    cfg->dma_cfg.ctrl.nonchain.auto_reload   = WM_DMA_RELOAD_DISABLE;
    cfg->dma_cfg.dest_addr                   = (uint32_t)cfg->vi;
    cfg->dma_cfg.ctrl.nonchain.src_addr_inc  = WM_DMA_ADDR_FIXED;
    cfg->dma_cfg.ctrl.nonchain.dest_addr_inc = WM_DMA_ADDR_INC;
    cfg->dma_cfg.ctrl.nonchain.len           = sizeof(wm_hal_pwm_cap_report_t) * cfg->report_cap_size;

    cfg->dma_cfg.extend_ctrl.ch           = cfg->dma_channel;
    cfg->dma_cfg.extend_ctrl.int_en       = WM_DMA_CH_EN_CTRL_ENABLE;
    cfg->dma_cfg.extend_ctrl.int_type     = WM_DMA_INT_XFER_DONE;
    cfg->dma_cfg.extend_ctrl.mode         = WM_DMA_HW_MODE;
    cfg->dma_cfg.extend_ctrl.chain_mode   = WM_DMA_CHAIN_MODE_NORMAL;
    cfg->dma_cfg.extend_ctrl.warp_mode_en = WM_DMA_WARP_CTRL_DISABLE;
    cfg->dma_cfg.ctrl.nonchain.data_unit  = WM_DMA_XFER_UNIT_WORD;
    cfg->dma_cfg.ctrl.nonchain.burst_size = WM_DMA_BURST_1UNIT;

    wm_hal_dma_register_callback(cfg->dma_dev, cfg->dma_channel, wm_hal_pwm_dma_done_cb, dev);

    wm_hal_dma_config(cfg->dma_dev, &cfg->dma_cfg);

    return WM_ERR_SUCCESS;
}

static int wm_hal_set_pwm_dma_en(wm_hal_pwm_dev_t *dev, uint8_t channel, bool en)
{
    int ret = WM_ERR_SUCCESS;

    wm_hal_pwm_channel_cfg_t *cfg = NULL;

    do {
        cfg = wm_hal_pwm_check_channel_cfg(dev, channel);
        if (cfg == NULL) {
            break;
        }
        if (cfg->mode != WM_PWM_IN_CAP) {
            break;
        }

        if (cfg->int_type != WM_PWM_CAP_INT_DMA) {
            break;
        }

        if (cfg->dma_dev == NULL) {
            break;
        }

        if (cfg->dma_channel >= WM_DMA_CH_MAX) {
            break;
        }

        if (en) {
            ret = wm_hal_dma_start(cfg->dma_dev, cfg->dma_channel);
        } else {
            if ((ret = wm_hal_dma_stop(cfg->dma_dev, cfg->dma_channel)) != WM_ERR_SUCCESS) {
                break;
            }
            ret = wm_hal_dma_clr_req(cfg->dma_dev, cfg->dma_cfg.extend_ctrl.req_sel);
        }
    } while (0);

    return ret;
}

static int wm_hal_pwm_set_cap_int_type(wm_hal_pwm_dev_t *dev, uint8_t channel)
{
    wm_hal_pwm_channel_cfg_t *cfg = dev->cfg[channel];

    switch (cfg->int_type) {
        case WM_PWM_CAP_INT_RISING:
        {
            wm_hal_pwm_set_cap_int(dev, channel, true, false, false);
            break;
        }
        case WM_PWM_CAP_INT_FALLING:
        {
            wm_hal_pwm_set_cap_int(dev, channel, false, true, false);
            break;
        }
        case WM_PWM_CAP_INT_RIGSING_FALLING:
        {
            wm_hal_pwm_set_cap_int(dev, channel, true, true, false);
            break;
        }
        case WM_PWM_CAP_INT_DMA:
        {
            wm_hal_pwm_set_cap_int(dev, channel, false, false, true);
            /* XXX: configure DMA for PWM capture channel, should be started in pwm_chan_start */
            return wm_hal_pwm_init_in_cap_dma(dev, channel);
        }
        default:
        {
            return WM_ERR_INVALID_PARAM;
        }
    }

    return WM_ERR_SUCCESS;
}

static int wm_hal_pwm_init_in_cap(wm_hal_pwm_dev_t *dev, uint8_t channel)
{
    int ret                       = WM_ERR_SUCCESS;
    wm_hal_pwm_channel_cfg_t *cfg = dev->cfg[channel];

    if (channel != WM_PWM_CHANNEL_0 && channel != WM_PWM_CHANNEL_4) {
        return WM_ERR_INVALID_PARAM;
    }

    /* only increament counter type supported for capture mode */
    if (cfg->cnt_type != WM_PWM_CNT_TYPE_INC) {
        return WM_ERR_INVALID_PARAM;
    }

    /* set the clock divider for the specified channel */
    wm_ll_pwm_set_clkdiv(dev->reg_dev, channel, cfg->clkdiv);

    /* update period cycle setting */
    wm_ll_pwm_set_period(dev->reg_dev, channel, cfg->period);

    /* update ducy cycle setting */
    wm_ll_pwm_set_period(dev->reg_dev, channel, 0);

    /* disable indepenent output mode */
    /* capture mode require IO work in tristate mode */
    wm_ll_pwm_set_ch_output_state(dev->reg_dev, channel, WM_PWM_OUT_EN_STATE_TRI);

    /* disable two channel sync channel mode */
    wm_ll_pwm_set_twosync_mode_en(dev->reg_dev, channel, false);

    /* disable all channel sync channel mode */
    wm_ll_pwm_set_allsync_mode_en(dev->reg_dev, false);

    /* disable complement mode for the channel pairs */
    wm_ll_pwm_set_complement_en(dev->reg_dev, channel, false);

    /* enable brake output mode for this channel */
    wm_ll_pwm_set_brake_en(dev->reg_dev, channel, false);

    /* force enable auto reload function for capture mode */
    wm_ll_pwm_set_autoload_mode_en(dev->reg_dev, channel, true);

    /* enable / disable capture inverse */
    wm_ll_pwm_set_capture_inv_en(dev->reg_dev, channel, cfg->inverse);

    /* set counter type to increment mode for capture mode */
    wm_ll_pwm_set_cnt_type(dev->reg_dev, channel, cfg->cnt_type);

    /* XXX: disable Period interrupt for capture mode */
    wm_hal_pwm_set_out_int(dev, channel, false);

    /* the interrupt align type for capture */
    ret = wm_hal_pwm_set_cap_int_type(dev, channel);
    if (WM_ERR_SUCCESS != ret) {
        return ret;
    }

    /* enable capture mode */
    wm_ll_pwm_set_capture_en(dev->reg_dev, channel, true);

    return WM_ERR_SUCCESS;
}

void wm_hal_pwm_isr_handler(wm_irq_no_t irq, void *arg)
{
    /* check if output intrrupt is triggerd */
    wm_hal_pwm_output_irq_cb(arg);

    /* check if input intrrupt is triggerd */
    wm_hal_pwm_input_irq_cb(arg);
}

int wm_hal_pwm_chans_start(wm_hal_pwm_dev_t *dev)
{
    uint8_t bits = 0;

    if (!wm_hal_pwm_check_dev(dev)) {
        return WM_ERR_INVALID_PARAM;
    }

    bits = wm_hal_pwm_get_chans_bits(dev, WM_PWM_CHANNEL_MAX);
    if (bits == 0) {
        return WM_ERR_FAILED;
    }

    bits &= wm_hal_pwm_clr_chans_issue_bits(dev);

    wm_ll_pwm_set_chans_map_cnt_en(dev->reg_dev, bits, true);

    for (int i = WM_PWM_CHANNEL_0; i < WM_PWM_CHANNEL_MAX; i++) {
        if (!!(bits & WM_BIT(i))) {
            if (wm_hal_set_pwm_dma_en(dev, i, true) != WM_ERR_SUCCESS) {
                return WM_ERR_FAILED;
            }
        }
    }

    return WM_ERR_SUCCESS;
}

int wm_hal_pwm_chans_stop(wm_hal_pwm_dev_t *dev)
{
    uint8_t bits = 0;

    if (!wm_hal_pwm_check_dev(dev)) {
        return WM_ERR_INVALID_PARAM;
    }

    bits = wm_hal_pwm_get_chans_bits(dev, WM_PWM_CHANNEL_MAX);

    wm_ll_pwm_set_chans_map_cnt_en(dev->reg_dev, bits, false);

    for (int i = WM_PWM_CHANNEL_0; i < WM_PWM_CHANNEL_MAX; i++) {
        if (!!(bits & WM_BIT(i))) {
            if (wm_hal_set_pwm_dma_en(dev, i, false) != WM_ERR_SUCCESS) {
                return WM_ERR_FAILED;
            }
        }
    }

    return bits ? WM_ERR_SUCCESS : WM_ERR_NOT_FOUND;
}

int wm_hal_pwm_chan_start(wm_hal_pwm_dev_t *dev, uint8_t channel)
{
    wm_hal_pwm_channel_cfg_t *cfg = NULL;

    cfg = wm_hal_pwm_check_channel_cfg(dev, channel);
    if (cfg == NULL) {
        return WM_ERR_NOT_FOUND;
    }

    /* XXX: HW issue
     * If set to trigger a high-level output on the brake, the PWM counter should not
     * be activated at this time; otherwise, the brake will become ineffective. */
    if (cfg->mode == WM_PWM_OUT_BRAKE && cfg->output_high) {
        return WM_ERR_SUCCESS;
    }

    wm_ll_pwm_set_chan_cnt_en(dev->reg_dev, channel, true);

    return wm_hal_set_pwm_dma_en(dev, channel, true);
}

int wm_hal_pwm_chan_stop(wm_hal_pwm_dev_t *dev, uint8_t channel)
{
    wm_hal_pwm_channel_cfg_t *cfg = NULL;

    cfg = wm_hal_pwm_check_channel_cfg(dev, channel);
    if (cfg == NULL) {
        return WM_ERR_NOT_FOUND;
    }

    wm_ll_pwm_set_chan_cnt_en(dev->reg_dev, channel, false);

    return wm_hal_set_pwm_dma_en(dev, channel, false);
}

int wm_hal_pwm_channel_init(wm_hal_pwm_dev_t *dev, wm_hal_pwm_channel_cfg_t *cfg)
{
    uint8_t bits = 0;
    uint8_t chan = WM_PWM_CHANNEL_0;

    if (!wm_hal_pwm_check_dev(dev)) {
        return WM_ERR_NOT_FOUND;
    }

    if (cfg == NULL) {
        WM_HAL_PWM_LOG_E("pwm module should be initialized!");
        return WM_ERR_INVALID_PARAM;
    }

    chan = cfg->channel;
    if (chan >= WM_PWM_CHANNEL_MAX) {
        WM_HAL_PWM_LOG_E("chan[%d] overflow!", chan);
        return WM_ERR_INVALID_PARAM;
    }

    dev->cfg[chan] = cfg;

    wm_hal_pwm_clear_channel_int(dev, chan);

    /* stop the channel before config to the mode */
    bits = wm_hal_pwm_get_chans_bits(dev, chan);

    wm_ll_pwm_set_chans_map_cnt_en(dev->reg_dev, bits, false);

    /* return wm_hal_pwm_channel_do_init(dev, dev->cfg[chan]); */
    return pwm_moders[dev->cfg[chan]->mode](dev, chan);
}

int wm_hal_pwm_channel_deinit(wm_hal_pwm_dev_t *dev, uint8_t channel)
{
    uint8_t bits                  = 0;
    wm_hal_pwm_channel_cfg_t *cfg = NULL;

    if (dev == NULL || dev->reg_dev == NULL) {
        return WM_ERR_INVALID_PARAM;
    }

    cfg = wm_hal_pwm_check_channel_cfg(dev, channel);
    if (cfg == NULL) {
        return WM_ERR_NOT_FOUND;
    }

    WM_HAL_PWM_LOG_D("pwm channel[%d] start to de-initialize!", channel);

    bits = wm_hal_pwm_get_chans_bits(dev, channel);

    /* we should stop dma if enabled */
    wm_hal_set_pwm_dma_en(dev, channel, false);

    wm_ll_pwm_set_chans_map_cnt_en(dev->reg_dev, bits, false);

    wm_ll_pwm_set_allsync_mode_en(dev->reg_dev, false);

    /* disable two channel sync channel mode */
    wm_ll_pwm_set_twosync_mode_en(dev->reg_dev, channel, false);

    /* disalbe complement mode for the channel pairs */
    wm_ll_pwm_set_complement_en(dev->reg_dev, channel, false);

    /* disable brake output mode for this channel */
    wm_ll_pwm_set_brake_en(dev->reg_dev, channel, false);

    /* disable dead time zone for complement */
    wm_ll_pwm_set_dead_time_en(dev->reg_dev, channel, false);

    wm_ll_pwm_set_capture_en(dev->reg_dev, channel, false);
    wm_ll_pwm_set_capture_inv_en(dev->reg_dev, channel, false);

    //wm_hal_pwm_set_out_int(dev, channel, false);
    //wm_hal_pwm_set_cap_int(dev, channel, false, false, false);
    wm_hal_pwm_clear_channel_int(dev, channel);

    dev->cfg[channel] = NULL;

    return WM_ERR_SUCCESS;
}

/* should call this pwm init for each PWM controoler initialization */
int wm_hal_pwm_init(wm_hal_pwm_dev_t *dev)
{
    if (dev == NULL) {
        WM_HAL_PWM_LOG_MARKER;
        return WM_ERR_INVALID_PARAM;
    }

    if (dev->reg_dev == NULL) {
        WM_HAL_PWM_LOG_MARKER;
        return WM_ERR_INVALID_PARAM;
    }

    if (wm_hal_irq_attach_sw_vector(dev->irq_num, wm_hal_pwm_isr_handler, dev)) {
        WM_HAL_PWM_LOG_MARKER;
        return WM_ERR_FAILED;
    }

    wm_hal_irq_enable(WM_IRQ_PWM);

    return WM_ERR_SUCCESS;
}

int wm_hal_pwm_deinit(wm_hal_pwm_dev_t *dev)
{
    if (!wm_hal_pwm_check_dev(dev)) {
        WM_HAL_PWM_LOG_E("dev should be initialized first!");
        return WM_ERR_NOT_ALLOWED;
    }

    if (dev->reg_dev == NULL) {
        return WM_ERR_INVALID_PARAM;
    }

    /* stop all channels' PWM counter */
    wm_ll_pwm_set_chans_cnt_en(dev->reg_dev, false);

    if (WM_ERR_SUCCESS != wm_hal_irq_disable(dev->irq_num)) {
        return WM_ERR_FAILED;
    }
    if (WM_ERR_SUCCESS != wm_hal_irq_detach_sw_vector(dev->irq_num)) {
        return WM_ERR_FAILED;
    }

    for (int i = WM_PWM_CHANNEL_0; i < WM_PWM_CHANNEL_MAX; i++) {
        wm_hal_pwm_channel_deinit(dev, i);
        dev->cfg[i] = NULL;
    }

    wm_ll_pwm_clear_all_int_status(dev->reg_dev);

    return WM_ERR_SUCCESS;
}

int wm_hal_pwm_set_freq(wm_hal_pwm_dev_t *dev, uint8_t channel, uint32_t freq)
{
    uint8_t bits = 0;
    uint32_t max_freq, min_freq;
    wm_hal_pwm_channel_cfg_t *cfg = NULL;

    if (freq == 0) {
        WM_HAL_PWM_LOG_MARKER;
        return WM_ERR_NOT_ALLOWED;
    }

    cfg = wm_hal_pwm_check_channel_cfg(dev, channel);
    if (cfg == NULL) {
        WM_HAL_PWM_LOG_MARKER;
        return WM_ERR_INVALID_PARAM;
    }

    min_freq = (dev->master_clock + WM_PWM_MAX_PERIOD) / (WM_PWM_MAX_PERIOD + 1);
    min_freq = (min_freq + WM_PWM_MAX_CLKDIV - 1) / (WM_PWM_MAX_CLKDIV);

    max_freq = dev->master_clock / (WM_PWM_MIN_PERIOD + 1) / (WM_PWM_MIN_CLKDIV);

    if (freq > max_freq || freq < min_freq) {
        WM_HAL_PWM_LOG_MARKER;
        return WM_ERR_NOT_ALLOWED;
    }

    /* freq = master_clock / (period + 1) / (clkdiv) */

    uint8_t period_temp = wm_ll_pwm_get_period(dev->reg_dev, channel);

    uint16_t clkdiv_temp = dev->master_clock / (period_temp + 1) / freq;
    if (clkdiv_temp > 0 && dev->master_clock == (clkdiv_temp * freq * (period_temp + 1))) {
        goto find;
    } else if (!wm_hal_pwm_find_period_clkdiv(dev, freq, &period_temp, &clkdiv_temp)) {
        goto find;
    }

    WM_HAL_PWM_LOG_I("Find period & clkdiv for freq[%lu] failed", freq);

    return WM_ERR_NOT_ALLOWED;

find:
    /* XXX: if channel(s) have been enabled, we should disable first */
    if (wm_ll_pwm_get_chan_cnt_en(dev->reg_dev, channel)) {
        bits = wm_hal_pwm_get_chans_bits(dev, channel);
    }

    wm_ll_pwm_set_chans_map_cnt_en(dev->reg_dev, bits, false);

    wm_ll_pwm_set_period(dev->reg_dev, channel, period_temp);

    wm_ll_pwm_set_clkdiv(dev->reg_dev, channel, clkdiv_temp);

    wm_ll_pwm_set_chans_map_cnt_en(dev->reg_dev, bits, true);

    cfg->period = period_temp;
    cfg->clkdiv = clkdiv_temp;

    WM_HAL_PWM_LOG_I("Find period[%d], clkdiv[%u] for freq[%lu]!", period_temp, clkdiv_temp, freq);

    return WM_ERR_SUCCESS;
}

int wm_hal_pwm_set_freq_inner(wm_hal_pwm_dev_t *dev, uint8_t channel, uint8_t period, uint16_t clkdiv)
{
    uint8_t bits                  = 0;
    wm_hal_pwm_channel_cfg_t *cfg = NULL;

    cfg = wm_hal_pwm_check_channel_cfg(dev, channel);
    if (cfg == NULL) {
        return WM_ERR_INVALID_PARAM;
    }

    /* XXX: if channel(s) have been enabled, we should disable first */
    if (wm_ll_pwm_get_chan_cnt_en(dev->reg_dev, channel)) {
        bits = wm_hal_pwm_get_chans_bits(dev, channel);
    }

    wm_ll_pwm_set_chans_map_cnt_en(dev->reg_dev, bits, false);

    /* freq = master_clock / (period + 1) / (clkdiv) */

    wm_ll_pwm_set_period(dev->reg_dev, channel, period);

    wm_ll_pwm_set_clkdiv(dev->reg_dev, channel, clkdiv);

    wm_ll_pwm_set_chans_map_cnt_en(dev->reg_dev, bits, true);

    cfg->period = period;
    cfg->clkdiv = clkdiv;

    return WM_ERR_SUCCESS;
}

int wm_hal_pwm_get_freq(wm_hal_pwm_dev_t *dev, uint8_t channel, uint32_t *freq_hz)
{
    wm_hal_pwm_channel_cfg_t *cfg = NULL;

    if (freq_hz == NULL) {
        WM_HAL_PWM_LOG_MARKER;
        return WM_ERR_INVALID_PARAM;
    }

    cfg = wm_hal_pwm_check_channel_cfg(dev, channel);
    if (cfg == NULL) {
        WM_HAL_PWM_LOG_MARKER;
        return WM_ERR_INVALID_PARAM;
    }

    cfg->period = wm_ll_pwm_get_period(dev->reg_dev, channel);

    cfg->clkdiv = wm_ll_pwm_get_clkdiv(dev->reg_dev, channel);

    if (cfg->clkdiv == 0) {
        *freq_hz = dev->master_clock / (cfg->period + 1);
    } else {
        *freq_hz = dev->master_clock / (cfg->period + 1) / cfg->clkdiv;
    }

    return WM_ERR_SUCCESS;
}

int wm_hal_pwm_get_clkdiv(wm_hal_pwm_dev_t *dev, uint8_t channel, uint16_t *clkdiv)
{
    wm_hal_pwm_channel_cfg_t *cfg = NULL;

    if (clkdiv == NULL) {
        WM_HAL_PWM_LOG_MARKER;
        return WM_ERR_INVALID_PARAM;
    }

    cfg = wm_hal_pwm_check_channel_cfg(dev, channel);
    if (cfg == NULL) {
        WM_HAL_PWM_LOG_MARKER;
        return WM_ERR_INVALID_PARAM;
    }

    cfg->clkdiv = wm_ll_pwm_get_clkdiv(dev->reg_dev, channel);

    *clkdiv = cfg->clkdiv;

    return WM_ERR_SUCCESS;
}

int wm_hal_pwm_get_period(wm_hal_pwm_dev_t *dev, uint8_t channel, uint8_t *period)
{
    wm_hal_pwm_channel_cfg_t *cfg = NULL;

    if (period == NULL) {
        WM_HAL_PWM_LOG_MARKER;
        return WM_ERR_INVALID_PARAM;
    }

    cfg = wm_hal_pwm_check_channel_cfg(dev, channel);
    if (cfg == NULL) {
        WM_HAL_PWM_LOG_MARKER;
        return WM_ERR_INVALID_PARAM;
    }

    cfg->period = wm_ll_pwm_get_period(dev->reg_dev, channel);

    *period = cfg->period;

    return WM_ERR_SUCCESS;
}

int wm_hal_pwm_set_period_num(wm_hal_pwm_dev_t *dev, uint8_t channel, uint8_t num)
{
    uint8_t bits                  = 0;
    wm_hal_pwm_channel_cfg_t *cfg = NULL;

    cfg = wm_hal_pwm_check_channel_cfg(dev, channel);
    if (cfg == NULL) {
        WM_HAL_PWM_LOG_MARKER;
        return WM_ERR_INVALID_PARAM;
    }
    /* XXX: if channel(s) have been enabled, we should disable first */
    if (wm_ll_pwm_get_chan_cnt_en(dev->reg_dev, channel)) {
        bits = wm_hal_pwm_get_chans_bits(dev, channel);
    }

    wm_ll_pwm_set_chans_map_cnt_en(dev->reg_dev, bits, false);

    wm_ll_pwm_set_period_num(dev->reg_dev, channel, num);

    wm_ll_pwm_set_chans_map_cnt_en(dev->reg_dev, bits, true);

    cfg->period_num = num;

    return WM_ERR_SUCCESS;
}

int wm_hal_pwm_get_period_num(wm_hal_pwm_dev_t *dev, uint8_t channel, uint8_t *period_num)
{
    wm_hal_pwm_channel_cfg_t *cfg = NULL;

    if (period_num == NULL) {
        WM_HAL_PWM_LOG_MARKER;
        return WM_ERR_INVALID_PARAM;
    }

    cfg = wm_hal_pwm_check_channel_cfg(dev, channel);
    if (cfg == NULL) {
        WM_HAL_PWM_LOG_MARKER;
        return WM_ERR_INVALID_PARAM;
    }

    cfg->period_num = wm_ll_pwm_get_period_num(dev->reg_dev, channel);

    *period_num = cfg->period_num;

    return WM_ERR_SUCCESS;
}

int wm_hal_pwm_set_duty_cycle(wm_hal_pwm_dev_t *dev, uint8_t channel, uint8_t duty_cycle)
{
    uint8_t bits                  = 0;
    wm_hal_pwm_channel_cfg_t *cfg = NULL;

    cfg = wm_hal_pwm_check_channel_cfg(dev, channel);
    if (cfg == NULL) {
        WM_HAL_PWM_LOG_MARKER;
        return WM_ERR_INVALID_PARAM;
    }

    /* XXX: if channel(s) have been enabled, we should disable first */
    if (wm_ll_pwm_get_chan_cnt_en(dev->reg_dev, channel)) {
        bits = wm_hal_pwm_get_chans_bits(dev, channel);
    }

    wm_ll_pwm_set_chans_map_cnt_en(dev->reg_dev, bits, false);

    wm_ll_pwm_set_cmp_cnt(dev->reg_dev, channel, duty_cycle);

    wm_ll_pwm_set_chans_map_cnt_en(dev->reg_dev, bits, true);

    cfg->duty_cycle = duty_cycle;

    return WM_ERR_SUCCESS;
}

int wm_hal_pwm_get_duty_cycle(wm_hal_pwm_dev_t *dev, uint8_t channel, uint8_t *duty_cycle)
{
    wm_hal_pwm_channel_cfg_t *cfg = NULL;

    if (duty_cycle == NULL) {
        WM_HAL_PWM_LOG_MARKER;
        return WM_ERR_INVALID_PARAM;
    }

    cfg = wm_hal_pwm_check_channel_cfg(dev, channel);
    if (cfg == NULL) {
        WM_HAL_PWM_LOG_MARKER;
        return WM_ERR_INVALID_PARAM;
    }

    cfg->duty_cycle = wm_ll_pwm_get_cmp_cnt(dev->reg_dev, channel);

    *duty_cycle = cfg->duty_cycle;

    return WM_ERR_SUCCESS;
}

/* XXX: DRV layer should try hunting-pecking the valid period and clock div */
uint8_t wm_hal_pwm_get_cycle_from_duty(wm_hal_pwm_dev_t *dev, uint8_t channel, float duty)
{
    float duty_cycle_float = 0.0;

    enum wm_pwm_cnt_type cnt_type;

    wm_hal_pwm_channel_cfg_t *cfg = NULL;

    cfg = wm_hal_pwm_check_channel_cfg(dev, channel);
    if (cfg == NULL) {
        return 0;
    }

    if (duty > 1 || duty < 0) {
        return 0;
    }

    cnt_type = wm_ll_pwm_get_cnt_type(dev->reg_dev, channel);

    if (cnt_type == WM_PWM_CNT_TYPE_DEC) {
        /* duty = (CMP + 1) / (PERIOD + 1) */
        duty_cycle_float = (duty * (cfg->period + 1)) - 1;
    } else if (cnt_type == WM_PWM_CNT_TYPE_CENTER) {
        /* duty = (2 * CMP + 1) / (2 * (PERIOD + 1)) */
        duty_cycle_float = ((duty * ((cfg->period + 1) << 1)) - 1) / 2;
    }

    if (duty_cycle_float == (uint8_t)duty_cycle_float) {
        return duty_cycle_float;
    } else {
        WM_HAL_PWM_LOG_MARKER;
        return 0;
    }
}

int wm_hal_pwm_set_inverse(wm_hal_pwm_dev_t *dev, uint8_t channel, bool inverse)
{
    uint8_t bits                  = 0;
    wm_hal_pwm_channel_cfg_t *cfg = NULL;

    cfg = wm_hal_pwm_check_channel_cfg(dev, channel);
    if (cfg == NULL) {
        WM_HAL_PWM_LOG_MARKER;
        return WM_ERR_NOT_FOUND;
    }

    /* XXX: if channel(s) have been enabled, we should disable first */
    if (wm_ll_pwm_get_chan_cnt_en(dev->reg_dev, channel)) {
        bits = wm_hal_pwm_get_chans_bits(dev, channel);
    }

    wm_ll_pwm_set_chans_map_cnt_en(dev->reg_dev, bits, false);

    if (cfg->mode == WM_PWM_IN_CAP) {
        wm_ll_pwm_set_capture_inv_en(dev->reg_dev, channel, inverse);
    } else {
        wm_ll_pwm_set_output_ploarity_inv_en(dev->reg_dev, channel, inverse);
    }

    wm_ll_pwm_set_chans_map_cnt_en(dev->reg_dev, bits, true);

    cfg->inverse = inverse;

    return WM_ERR_SUCCESS;
}

int wm_hal_pwm_get_inverse(wm_hal_pwm_dev_t *dev, uint8_t channel, bool *en)
{
    wm_hal_pwm_channel_cfg_t *cfg = NULL;

    if (en == NULL) {
        WM_HAL_PWM_LOG_MARKER;
        return WM_ERR_INVALID_PARAM;
    }

    cfg = wm_hal_pwm_check_channel_cfg(dev, channel);
    if (cfg == NULL) {
        WM_HAL_PWM_LOG_MARKER;
        return WM_ERR_INVALID_PARAM;
    }

    if (cfg->mode == WM_PWM_IN_CAP) {
        cfg->inverse = wm_ll_pwm_get_capture_inv_en(dev->reg_dev, channel);
    } else {
        cfg->inverse = wm_ll_pwm_get_output_ploarity_inv_en(dev->reg_dev, channel);
    }

    *en = cfg->inverse;

    return WM_ERR_SUCCESS;
}

int wm_hal_pwm_set_brake_level(wm_hal_pwm_dev_t *dev, uint8_t channel, bool high)
{
    uint8_t bits                  = 0;
    wm_hal_pwm_channel_cfg_t *cfg = NULL;

    cfg = wm_hal_pwm_check_channel_cfg(dev, channel);
    if (cfg == NULL) {
        return WM_ERR_INVALID_PARAM;
    }

    if (cfg->mode != WM_PWM_OUT_BRAKE) {
        return WM_ERR_SUCCESS;
    }

    /* should disable first */
    bits = wm_hal_pwm_get_chans_bits(dev, channel);

    wm_ll_pwm_set_chans_map_cnt_en(dev->reg_dev, bits, false);

    wm_ll_pwm_set_brake_level(dev->reg_dev, channel, high);

    cfg->output_high = high;

    /* XXX: HW issue
     * If set to trigger a high-level output on the brake, the PWM counter should not
     * be activated at this time; otherwise, the brake will become ineffective. */
    if (cfg->output_high) {
        return WM_ERR_SUCCESS;
    }

    wm_ll_pwm_set_chan_cnt_en(dev->reg_dev, channel, true);

    return WM_ERR_SUCCESS;
}

int wm_hal_pwm_get_brake_level(wm_hal_pwm_dev_t *dev, uint8_t channel, bool *en)
{
    wm_hal_pwm_channel_cfg_t *cfg = NULL;

    if (en == NULL) {
        WM_HAL_PWM_LOG_MARKER;
        return WM_ERR_INVALID_PARAM;
    }

    cfg = wm_hal_pwm_check_channel_cfg(dev, channel);
    if (cfg == NULL) {
        WM_HAL_PWM_LOG_MARKER;
        return WM_ERR_INVALID_PARAM;
    }

    cfg->output_high = wm_ll_pwm_get_brake_level_high(dev->reg_dev, channel);

    *en = cfg->output_high;

    return WM_ERR_SUCCESS;
}

int wm_hal_pwm_set_deadtime(wm_hal_pwm_dev_t *dev, uint8_t channel, uint8_t cnt, uint8_t clkdiv)
{
    uint8_t bits                  = 0;
    wm_hal_pwm_channel_cfg_t *cfg = NULL;

    cfg = wm_hal_pwm_check_channel_cfg(dev, channel);
    if (cfg == NULL) {
        WM_HAL_PWM_LOG_MARKER;
        return WM_ERR_INVALID_PARAM;
    }

    if (clkdiv > WM_PWM_MAX_DT_CLKDIV) {
        WM_HAL_PWM_LOG_MARKER;
        return WM_ERR_INVALID_PARAM;
    }

    /* XXX: if channel(s) have been enabled, we should disable first */
    if (wm_ll_pwm_get_chan_cnt_en(dev->reg_dev, channel)) {
        bits = wm_hal_pwm_get_chans_bits(dev, channel);
    }

    wm_ll_pwm_set_chans_map_cnt_en(dev->reg_dev, bits, false);

    wm_ll_pwm_set_dead_time_cnt(dev->reg_dev, channel, cnt);

    wm_ll_pwm_set_dead_time_clkdiv(dev->reg_dev, clkdiv);

    if (cnt > 0) {
        wm_ll_pwm_set_dead_time_en(dev->reg_dev, channel, true);
        cfg->dead_time_en = true;
    } else {
        wm_ll_pwm_set_dead_time_en(dev->reg_dev, channel, false);
        cfg->dead_time_en = false;
    }

    wm_ll_pwm_set_chans_map_cnt_en(dev->reg_dev, bits, true);

    cfg->dead_time_cnt    = cnt;
    cfg->dead_time_clkdiv = clkdiv;

    return WM_ERR_SUCCESS;
}

int wm_hal_pwm_get_deadtime(wm_hal_pwm_dev_t *dev, uint8_t channel, uint8_t *cnt, uint8_t *clkdiv, bool *en)
{
    wm_hal_pwm_channel_cfg_t *cfg = NULL;

    cfg = wm_hal_pwm_check_channel_cfg(dev, channel);
    if (cfg == NULL) {
        WM_HAL_PWM_LOG_MARKER;
        return WM_ERR_INVALID_PARAM;
    }

    cfg->dead_time_cnt    = wm_ll_pwm_get_dead_time_cnt(dev->reg_dev, channel);
    cfg->dead_time_clkdiv = wm_ll_pwm_get_dead_time_clkdiv(dev->reg_dev);
    cfg->dead_time_en     = wm_ll_pwm_get_dead_time_en(dev->reg_dev, channel);

    if (cnt) {
        *cnt = cfg->dead_time_cnt;
    }

    if (clkdiv) {
        *clkdiv = cfg->dead_time_clkdiv;
    }

    if (en) {
        *en = cfg->dead_time_en;
    }

    return WM_ERR_SUCCESS;
}

int wm_hal_pwm_set_deadtime_en(wm_hal_pwm_dev_t *dev, uint8_t channel, bool en)
{
    wm_hal_pwm_channel_cfg_t *cfg = NULL;

    cfg = wm_hal_pwm_check_channel_cfg(dev, channel);
    if (cfg == NULL) {
        WM_HAL_PWM_LOG_MARKER;
        return WM_ERR_INVALID_PARAM;
    }

    wm_ll_pwm_set_dead_time_en(dev->reg_dev, channel, en);

    cfg->dead_time_en = en;

    return WM_ERR_SUCCESS;
}

int wm_hal_pwm_set_autoreload_en(wm_hal_pwm_dev_t *dev, uint8_t channel, bool en)
{
    uint8_t bits                  = 0;
    wm_hal_pwm_channel_cfg_t *cfg = NULL;

    cfg = wm_hal_pwm_check_channel_cfg(dev, channel);
    if (cfg == NULL) {
        WM_HAL_PWM_LOG_MARKER;
        return WM_ERR_NO_SUPPORT;
    }

    /* XXX: if channel(s) have been enabled, we should disable first */
    if (wm_ll_pwm_get_chan_cnt_en(dev->reg_dev, channel)) {
        bits = wm_hal_pwm_get_chans_bits(dev, channel);
    }

    wm_ll_pwm_set_chans_map_cnt_en(dev->reg_dev, bits, false);

    wm_ll_pwm_set_autoload_mode_en(dev->reg_dev, channel, en);

    wm_ll_pwm_set_chans_map_cnt_en(dev->reg_dev, bits, true);

    cfg->autoload = en;

    return WM_ERR_SUCCESS;
}

int wm_hal_pwm_get_autoreload_en(wm_hal_pwm_dev_t *dev, uint8_t channel, bool *en)
{
    wm_hal_pwm_channel_cfg_t *cfg = NULL;

    if (en == NULL) {
        WM_HAL_PWM_LOG_MARKER;
        return WM_ERR_INVALID_PARAM;
    }

    cfg = wm_hal_pwm_check_channel_cfg(dev, channel);
    if (cfg == NULL) {
        return WM_ERR_INVALID_PARAM;
    }

    cfg->autoload = wm_ll_pwm_get_autoload_mode_en(dev->reg_dev, channel);

    *en = cfg->autoload;

    return WM_ERR_SUCCESS;
}

int wm_hal_pwm_set_period_int(wm_hal_pwm_dev_t *dev, uint8_t channel, bool en)
{
    wm_hal_pwm_channel_cfg_t *cfg = NULL;

    cfg = wm_hal_pwm_check_channel_cfg(dev, channel);
    if (cfg == NULL) {
        WM_HAL_PWM_LOG_MARKER;
        return WM_ERR_INVALID_PARAM;
    }

    wm_ll_pwm_set_period_int_en(dev->reg_dev, channel, en);

    cfg->period_int = en;

    return WM_ERR_SUCCESS;
}

int wm_hal_pwm_get_period_int(wm_hal_pwm_dev_t *dev, uint8_t channel, bool *en)
{
    wm_hal_pwm_channel_cfg_t *cfg = NULL;

    if (en == NULL) {
        WM_HAL_PWM_LOG_MARKER;
        return WM_ERR_INVALID_PARAM;
    }

    cfg = wm_hal_pwm_check_channel_cfg(dev, channel);
    if (cfg == NULL) {
        WM_HAL_PWM_LOG_MARKER;
        return WM_ERR_INVALID_PARAM;
    }

    cfg->period_int = wm_ll_pwm_get_period_int_en(dev->reg_dev, channel);

    *en = cfg->period_int;

    return WM_ERR_SUCCESS;
}

int wm_hal_pwm_get_chan_cfg(wm_hal_pwm_dev_t *dev, uint8_t channel, wm_hal_pwm_channel_cfg_t **cfg)
{
    wm_hal_pwm_channel_cfg_t *cfg_set = wm_hal_pwm_check_channel_cfg(dev, channel);
    if (cfg_set == NULL) {
        return WM_ERR_INVALID_PARAM;
    }

    *cfg = cfg_set;

    return WM_ERR_SUCCESS;
}
