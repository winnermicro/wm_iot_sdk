/**
 * @file wm_ll_pwm.c
 *
 * @brief PWM LL Module
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

#include "wm_ll_pwm.h"
#include "wm_pwm_reg_struct.h"
#include <stdint.h>

void wm_ll_pwm_set_clkdiv(wm_pwm_reg_t *hw, uint8_t channel, uint16_t div)
{
    switch (channel) {
        case WM_PWM_CHANNEL_0:
        {
            WM_LL_REG_FILED_COPY_WRTIE(hw->clkdiv01, clkdiv0, div);
            break;
        }
        case WM_PWM_CHANNEL_1:
        {
            WM_LL_REG_FILED_COPY_WRTIE(hw->clkdiv01, clkdiv1, div);
            break;
        }
        case WM_PWM_CHANNEL_2:
        {
            WM_LL_REG_FILED_COPY_WRTIE(hw->clkdiv23, clkdiv2, div);
            break;
        }
        case WM_PWM_CHANNEL_3:
        {
            WM_LL_REG_FILED_COPY_WRTIE(hw->clkdiv23, clkdiv3, div);
            break;
        }
        case WM_PWM_CHANNEL_4:
        {
            WM_LL_REG_FILED_COPY_WRTIE(hw->ch4_clkdiv, clkdiv4, div);
            break;
        }
        default:
        {
            break;
        }
    }
}

uint16_t wm_ll_pwm_get_clkdiv(wm_pwm_reg_t *hw, uint8_t channel)
{
    switch (channel) {
        case WM_PWM_CHANNEL_0:
        {
            /* return hw->clkdiv01.clkdiv0; */
            return WM_LL_REG_FILED_COPY_READ(hw->clkdiv01, clkdiv0);
        }
        case WM_PWM_CHANNEL_1:
        {
            /* return hw->clkdiv01.clkdiv1; */
            return WM_LL_REG_FILED_COPY_READ(hw->clkdiv01, clkdiv1);
        }
        case WM_PWM_CHANNEL_2:
        {
            /* return hw->clkdiv23.clkdiv2; */
            return WM_LL_REG_FILED_COPY_READ(hw->clkdiv23, clkdiv2);
        }
        case WM_PWM_CHANNEL_3:
        {
            /* return hw->clkdiv23.clkdiv3; */
            return WM_LL_REG_FILED_COPY_READ(hw->clkdiv23, clkdiv3);
        }
        case WM_PWM_CHANNEL_4:
        {
            /* return hw->ch4_clkdiv.clkdiv4; */
            return WM_LL_REG_FILED_COPY_READ(hw->ch4_clkdiv, clkdiv4);
        }
        default:
        {
            return 0;
        }
    }
}

void wm_ll_pwm_set_chan_cnt_en(wm_pwm_reg_t *hw, uint8_t channel, bool en)
{
    if (en) {
        hw->ctrl.cnten |= WM_BIT(channel);
    } else {
        hw->ctrl.cnten &= ~WM_BIT(channel);
    }
}

void wm_ll_pwm_set_chans_cnt_en(wm_pwm_reg_t *hw, bool en)
{
    if (en) {
        hw->ctrl.cnten |= (WM_BIT(WM_PWM_CHANNEL_MAX) - 1);
    } else {
        hw->ctrl.cnten &= 0x0;
    }
}

void wm_ll_pwm_set_chans_map_cnt_en(wm_pwm_reg_t *hw, uint8_t bits, bool en)
{
    if (en) {
        hw->ctrl.cnten |= bits;
    } else {
        hw->ctrl.cnten &= ~bits;
    }
}

void wm_ll_pwm_set_capture_inv_en(wm_pwm_reg_t *hw, uint8_t channel, bool en)
{
    if (channel == WM_PWM_CHANNEL_4) {
        wm_ll_pwm_set_ch4_capture_inv_en(hw, en);
    } else {
        wm_ll_pwm_set_ch0_capture_inv_en(hw, en);
    }
}

bool wm_ll_pwm_get_capture_inv_en(wm_pwm_reg_t *hw, uint8_t channel)
{
    if (channel == WM_PWM_CHANNEL_4) {
        return wm_ll_pwm_get_ch4_capture_inv_en(hw);
    } else {
        return wm_ll_pwm_get_ch0_capture_inv_en(hw);
    }
}

void wm_ll_pwm_set_capture_en(wm_pwm_reg_t *hw, uint8_t channel, bool en)
{
    if (channel == WM_PWM_CHANNEL_4) {
        wm_ll_pwm_set_ch4_capture_en(hw, en);
    } else if (channel == WM_PWM_CHANNEL_0) {
        wm_ll_pwm_set_ch0_capture_en(hw, en);
    }
}

bool wm_ll_pwm_get_capture_en(wm_pwm_reg_t *hw, uint8_t channel)
{
    if (channel == WM_PWM_CHANNEL_4) {
        return wm_ll_pwm_get_ch4_capture_en(hw);
    } else if (channel == WM_PWM_CHANNEL_0) {
        return wm_ll_pwm_get_ch0_capture_en(hw);
    } else {
        return false;
    }
}

void wm_ll_pwm_set_cnt_type(wm_pwm_reg_t *hw, uint8_t channel, uint8_t cnt_type)
{
    switch (channel) {
        case WM_PWM_CHANNEL_4:
        {
            hw->ch4_ctrl1.cnttype4 = cnt_type & 0x3;
            break;
        }
        case WM_PWM_CHANNEL_0:
        {
            hw->ctrl.cnttype0 = cnt_type & 0x3;
            break;
        }
        case WM_PWM_CHANNEL_1:
        {
            hw->ctrl.cnttype1 = cnt_type & 0x3;
            break;
        }
        case WM_PWM_CHANNEL_2:
        {
            hw->ctrl.cnttype2 = cnt_type & 0x3;
            break;
        }
        case WM_PWM_CHANNEL_3:
        {
            hw->ctrl.cnttype3 = cnt_type & 0x3;
            break;
        }
        default:
        {
            break;
        }
    }
}

uint8_t wm_ll_pwm_get_cnt_type(wm_pwm_reg_t *hw, uint8_t channel)
{
    switch (channel) {
        case WM_PWM_CHANNEL_4:
        {
            return hw->ch4_ctrl1.cnttype4;
        }
        case WM_PWM_CHANNEL_0:
        {
            return hw->ctrl.cnttype0;
        }
        case WM_PWM_CHANNEL_1:
        {
            return hw->ctrl.cnttype1;
        }
        case WM_PWM_CHANNEL_2:
        {
            return hw->ctrl.cnttype2;
        }
        case WM_PWM_CHANNEL_3:
        {
            return hw->ctrl.cnttype3;
        }
        default:
        {
            return 0;
        }
    }
}

void wm_ll_pwm_set_twosync_mode_en(wm_pwm_reg_t *hw, uint8_t channel, bool en)
{
    uint8_t bit = 0;
    switch (channel) {
        case WM_PWM_CHANNEL_0:
        case WM_PWM_CHANNEL_1:
        {
            break;
        }
        case WM_PWM_CHANNEL_2:
        case WM_PWM_CHANNEL_3:
        {
            bit = 1;
            break;
        }
        default:
        {
            return;
        }
    }

    if (en) {
        hw->ctrl.twosyncen |= WM_BIT(bit);
    } else {
        hw->ctrl.twosyncen &= ~WM_BIT(bit);
    }
}

bool wm_ll_pwm_get_twosync_mode_en(wm_pwm_reg_t *hw, uint8_t channel)
{
    uint8_t bit = 0;
    switch (channel) {
        case WM_PWM_CHANNEL_0:
        case WM_PWM_CHANNEL_1:
        {
            break;
        }
        case WM_PWM_CHANNEL_2:
        case WM_PWM_CHANNEL_3:
        {
            bit = 1;
            break;
        }
        default:
        {
            return false;
        }
    }
    return (hw->ctrl.twosyncen & WM_BIT(bit));
}

void wm_ll_pwm_set_ch_output_state(wm_pwm_reg_t *hw, uint8_t channel, uint8_t sel)
{
    switch (channel) {
        case WM_PWM_CHANNEL_0:
        {
            hw->ctrl.poen = sel;
            break;
        }
        case WM_PWM_CHANNEL_4:
        {
            hw->ch4_ctrl2.poen2 = sel;
            break;
        }
        default:
        {
            break;
        }
    }
}

uint8_t wm_ll_pwm_get_ch_output_state(wm_pwm_reg_t *hw, uint8_t channel)
{
    switch (channel) {
        case WM_PWM_CHANNEL_0:
        {
            return hw->ctrl.poen;
        }
        case WM_PWM_CHANNEL_4:
        {
            return hw->ch4_ctrl2.poen2;
        }
        default:
        {
            return 0;
        }
    }
}

void wm_ll_pwm_set_autoload_mode_en(wm_pwm_reg_t *hw, uint8_t channel, bool en)
{
    switch (channel) {
        case WM_PWM_CHANNEL_4:
        {
            hw->ch4_ctrl1.cntmode4 = en;
            break;
        }
        case WM_PWM_CHANNEL_0:
        case WM_PWM_CHANNEL_1:
        case WM_PWM_CHANNEL_2:
        case WM_PWM_CHANNEL_3:
        {
            if (en) {
                hw->ctrl.cntmode |= WM_BIT(channel);
            } else {
                hw->ctrl.cntmode &= ~WM_BIT(channel);
            }
            break;
        }
        default:
        {
            break;
        }
    }
}

bool wm_ll_pwm_get_autoload_mode_en(wm_pwm_reg_t *hw, uint8_t channel)
{
    switch (channel) {
        case WM_PWM_CHANNEL_4:
        {
            return hw->ch4_ctrl1.cntmode4;
        }
        case WM_PWM_CHANNEL_0:
        case WM_PWM_CHANNEL_1:
        case WM_PWM_CHANNEL_2:
        case WM_PWM_CHANNEL_3:
        {
            return (hw->ctrl.cntmode & WM_BIT(channel));
        }
        default:
        {
            return false;
        }
    }
}

void wm_ll_pwm_set_output_ploarity_inv_en(wm_pwm_reg_t *hw, uint8_t channel, int en)
{
    switch (channel) {
        case WM_PWM_CHANNEL_4:
        {
            hw->ch4_ctrl1.pinv4 = en;
            break;
        }
        case WM_PWM_CHANNEL_0:
        case WM_PWM_CHANNEL_1:
        case WM_PWM_CHANNEL_2:
        case WM_PWM_CHANNEL_3:
        {
            if (en) {
                hw->ctrl.pinv |= WM_BIT(channel);
            } else {
                hw->ctrl.pinv &= ~WM_BIT(channel);
            }
            break;
        }
        default:
        {
            break;
        }
    }
}

bool wm_ll_pwm_get_output_ploarity_inv_en(wm_pwm_reg_t *hw, uint8_t channel)
{
    switch (channel) {
        case WM_PWM_CHANNEL_4:
        {
            return hw->ch4_ctrl1.pinv4;
        }
        case WM_PWM_CHANNEL_0:
        case WM_PWM_CHANNEL_1:
        case WM_PWM_CHANNEL_2:
        case WM_PWM_CHANNEL_3:
        {
            return (hw->ctrl.pinv & WM_BIT(channel));
        }
        default:
        {
            return false;
        }
    }
}

void wm_ll_pwm_set_complement_en(wm_pwm_reg_t *hw, uint8_t channel, bool en)
{
    uint8_t bit = 0;
    switch (channel) {
        case WM_PWM_CHANNEL_0:
        case WM_PWM_CHANNEL_1:
        {
            break;
        }
        case WM_PWM_CHANNEL_2:
        case WM_PWM_CHANNEL_3:
        {
            bit = 1;
            break;
        }
        default:
        {
            return;
        }
    }

    if (en) {
        hw->ctrl.outmode |= WM_BIT(bit);
    } else {
        hw->ctrl.outmode &= ~WM_BIT(bit);
    }
}

bool wm_ll_pwm_get_complement_en(wm_pwm_reg_t *hw, uint8_t channel)
{
    uint8_t bit = 0;
    switch (channel) {
        case WM_PWM_CHANNEL_0:
        case WM_PWM_CHANNEL_1:
        {
            break;
        }
        case WM_PWM_CHANNEL_2:
        case WM_PWM_CHANNEL_3:
        {
            bit = 1;
            break;
        }
        default:
        {
            return false;
        }
    }

    return (hw->ctrl.outmode & WM_BIT(bit));
}

void wm_ll_pwm_set_period(wm_pwm_reg_t *hw, uint8_t channel, uint8_t period)
{
    switch (channel) {
        case WM_PWM_CHANNEL_0:
        {
            /* hw->period.period0 = period; */
            WM_LL_REG_FILED_COPY_WRTIE(hw->period, period0, period);
            break;
        }
        case WM_PWM_CHANNEL_1:
        {
            /* hw->period.period1 = period; */
            WM_LL_REG_FILED_COPY_WRTIE(hw->period, period1, period);
            break;
        }
        case WM_PWM_CHANNEL_2:
        {
            /* hw->period.period2 = period; */
            WM_LL_REG_FILED_COPY_WRTIE(hw->period, period2, period);
            break;
        }
        case WM_PWM_CHANNEL_3:
        {
            /* hw->period.period3 = period; */
            WM_LL_REG_FILED_COPY_WRTIE(hw->period, period3, period);
            break;
        }
        case WM_PWM_CHANNEL_4:
        {
            hw->ch4_clkdiv.period4 = period;
            WM_LL_REG_FILED_COPY_WRTIE(hw->ch4_clkdiv, period4, period);
            break;
        }
        default:
        {
            break;
        }
    }
}

uint8_t wm_ll_pwm_get_period(wm_pwm_reg_t *hw, uint8_t channel)
{
    switch (channel) {
        case WM_PWM_CHANNEL_0:
        {
            /* return hw->period.period0; */
            return WM_LL_REG_FILED_COPY_READ(hw->period, period0);
        }
        case WM_PWM_CHANNEL_1:
        {
            /* return hw->period.period1; */
            return WM_LL_REG_FILED_COPY_READ(hw->period, period1);
        }
        case WM_PWM_CHANNEL_2:
        {
            /* return hw->period.period2; */
            return WM_LL_REG_FILED_COPY_READ(hw->period, period2);
        }
        case WM_PWM_CHANNEL_3:
        {
            /* return hw->period.period3; */
            return WM_LL_REG_FILED_COPY_READ(hw->period, period3);
        }
        case WM_PWM_CHANNEL_4:
        {
            /* return hw->ch4_clkdiv.period4; */
            return WM_LL_REG_FILED_COPY_READ(hw->ch4_clkdiv, period4);
        }
        default:
        {
            return 0;
        }
    }
}

void wm_ll_pwm_set_period_num(wm_pwm_reg_t *hw, uint8_t channel, uint8_t num)
{
    switch (channel) {
        case WM_PWM_CHANNEL_0:
        {
            /* hw->period_num.pnum0 = num; */
            WM_LL_REG_FILED_COPY_WRTIE(hw->period_num, pnum0, num);
            break;
        }
        case WM_PWM_CHANNEL_1:
        {
            /* hw->period_num.pnum1 = num; */
            WM_LL_REG_FILED_COPY_WRTIE(hw->period_num, pnum1, num);
            break;
        }
        case WM_PWM_CHANNEL_2:
        {
            /* hw->period_num.pnum2 = num; */
            WM_LL_REG_FILED_COPY_WRTIE(hw->period_num, pnum2, num);
            break;
        }
        case WM_PWM_CHANNEL_3:
        {
            /* hw->period_num.pnum3 = num; */
            WM_LL_REG_FILED_COPY_WRTIE(hw->period_num, pnum3, num);
            break;
        }
        case WM_PWM_CHANNEL_4:
        {
            /* hw->ch4_clkdiv.ch4_pnum = num; */
            WM_LL_REG_FILED_COPY_WRTIE(hw->ch4_clkdiv, ch4_pnum, num);
            break;
        }
        default:
        {
            break;
        }
    }
}

uint8_t wm_ll_pwm_get_period_num(wm_pwm_reg_t *hw, uint8_t channel)
{
    switch (channel) {
        case WM_PWM_CHANNEL_0:
        {
            /* return hw->period_num.pnum0; */
            return WM_LL_REG_FILED_COPY_READ(hw->period_num, pnum0);
        }
        case WM_PWM_CHANNEL_1:
        {
            /* return hw->period_num.pnum1; */
            return WM_LL_REG_FILED_COPY_READ(hw->period_num, pnum1);
        }
        case WM_PWM_CHANNEL_2:
        {
            /* return hw->period_num.pnum2; */
            return WM_LL_REG_FILED_COPY_READ(hw->period_num, pnum2);
        }
        case WM_PWM_CHANNEL_3:
        {
            /* return hw->period_num.pnum3; */
            return WM_LL_REG_FILED_COPY_READ(hw->period_num, pnum3);
        }
        case WM_PWM_CHANNEL_4:
        {
            /* return hw->ch4_clkdiv.ch4_pnum; */
            return WM_LL_REG_FILED_COPY_READ(hw->ch4_clkdiv, ch4_pnum);
        }
        default:
        {
            return false;
        }
    }
}

void wm_ll_pwm_set_cmp_cnt(wm_pwm_reg_t *hw, uint8_t channel, uint8_t value)
{
    switch (channel) {
        case WM_PWM_CHANNEL_0:
        {
            /* hw->compare.cmp0 = value; */
            WM_LL_REG_FILED_COPY_WRTIE(hw->compare, cmp0, value);
            break;
        }
        case WM_PWM_CHANNEL_1:
        {
            /* hw->compare.cmp1 = value; */
            WM_LL_REG_FILED_COPY_WRTIE(hw->compare, cmp1, value);
            break;
        }
        case WM_PWM_CHANNEL_2:
        {
            /* hw->compare.cmp2 = value; */
            WM_LL_REG_FILED_COPY_WRTIE(hw->compare, cmp2, value);
            break;
        }
        case WM_PWM_CHANNEL_3:
        {
            /* hw->compare.cmp3 = value; */
            WM_LL_REG_FILED_COPY_WRTIE(hw->compare, cmp3, value);
            break;
        }
        case WM_PWM_CHANNEL_4:
        {
            /* hw->ch4_ctrl1.cmp4 = value; */
            WM_LL_REG_FILED_COPY_WRTIE(hw->ch4_ctrl1, cmp4, value);
            break;
        }
        default:
        {
            break;
        }
    }
}

uint8_t wm_ll_pwm_get_cmp_cnt(wm_pwm_reg_t *hw, uint8_t channel)
{
    switch (channel) {
        case WM_PWM_CHANNEL_0:
        {
            /* return hw->compare.cmp0; */
            return WM_LL_REG_FILED_COPY_READ(hw->compare, cmp0);
        }
        case WM_PWM_CHANNEL_1:
        {
            /* return hw->compare.cmp1; */
            return WM_LL_REG_FILED_COPY_READ(hw->compare, cmp1);
        }
        case WM_PWM_CHANNEL_2:
        {
            /* return hw->compare.cmp2; */
            return WM_LL_REG_FILED_COPY_READ(hw->compare, cmp2);
        }
        case WM_PWM_CHANNEL_3:
        {
            /* return hw->compare.cmp3; */
            return WM_LL_REG_FILED_COPY_READ(hw->compare, cmp3);
        }
        case WM_PWM_CHANNEL_4:
        {
            /* return hw->ch4_ctrl1.cmp4; */
            return WM_LL_REG_FILED_COPY_READ(hw->ch4_ctrl1, cmp4);
        }
        default:
        {
            return 0;
        }
    }
}

bool wm_ll_pwm_get_dead_time_en(wm_pwm_reg_t *hw, uint8_t channel)
{
    switch (channel) {
        case WM_PWM_CHANNEL_0:
        case WM_PWM_CHANNEL_1:
        {
            return hw->dead_time.dten01;
        }
        case WM_PWM_CHANNEL_2:
        case WM_PWM_CHANNEL_3:
        {
            return hw->dead_time.dten23;
        }
        default:
        {
            return false;
        }
    }
}

void wm_ll_pwm_set_dead_time_en(wm_pwm_reg_t *hw, uint8_t channel, bool en)
{
    switch (channel) {
        case WM_PWM_CHANNEL_0:
        case WM_PWM_CHANNEL_1:
        {
            hw->dead_time.dten01 = en;
            break;
        }
        case WM_PWM_CHANNEL_2:
        case WM_PWM_CHANNEL_3:
        {
            hw->dead_time.dten23 = en;
            break;
        }
        default:
        {
            break;
        }
    }
}

void wm_ll_pwm_set_dead_time_cnt(wm_pwm_reg_t *hw, uint8_t channel, uint8_t cnt)
{
    switch (channel) {
        case WM_PWM_CHANNEL_0:
        case WM_PWM_CHANNEL_1:
        {
            /* hw->dead_time.dtcnt01 = cnt; */
            WM_LL_REG_FILED_COPY_WRTIE(hw->dead_time, dtcnt01, cnt);
            break;
        }
        case WM_PWM_CHANNEL_2:
        case WM_PWM_CHANNEL_3:
        {
            /* hw->dead_time.dtcnt23 = cnt; */
            WM_LL_REG_FILED_COPY_WRTIE(hw->dead_time, dtcnt23, cnt);
            break;
        }
        default:
        {
            break;
        }
    }
}

uint8_t wm_ll_pwm_get_dead_time_cnt(wm_pwm_reg_t *hw, uint8_t channel)
{
    switch (channel) {
        case WM_PWM_CHANNEL_0:
        case WM_PWM_CHANNEL_1:
        {
            /* return hw->dead_time.dtcnt01; */
            return WM_LL_REG_FILED_COPY_READ(hw->dead_time, dtcnt01);
        }
        case WM_PWM_CHANNEL_2:
        case WM_PWM_CHANNEL_3:
        {
            /* return hw->dead_time.dtcnt23; */
            return WM_LL_REG_FILED_COPY_READ(hw->dead_time, dtcnt23);
        }
        default:
        {
            return 0;
        }
    }
}

void wm_ll_pwm_set_dma_request_int_en(wm_pwm_reg_t *hw, uint8_t channel, bool en)
{
    switch (channel) {
        case WM_PWM_CHANNEL_4:
        {
            hw->ch4_ctrl2.dma_request2_mask = en;
            break;
        }
        case WM_PWM_CHANNEL_0:
        {
            hw->int_ctrl.dma_request_en = en;
            break;
        }
        case WM_PWM_CHANNEL_1:
        case WM_PWM_CHANNEL_2:
        case WM_PWM_CHANNEL_3:
        default:
        {
            break;
        }
    }
}

bool wm_ll_pwm_get_dma_request_int_en(wm_pwm_reg_t *hw, uint8_t channel)
{
    switch (channel) {
        case WM_PWM_CHANNEL_4:
        {
            return hw->ch4_ctrl2.dma_request2_mask;
        }
        case WM_PWM_CHANNEL_0:
        {
            return hw->int_ctrl.dma_request_en;
        }
        case WM_PWM_CHANNEL_1:
        case WM_PWM_CHANNEL_2:
        case WM_PWM_CHANNEL_3:
        default:
        {
            return false;
        }
    }
}

void wm_ll_pwm_set_falling_int_en(wm_pwm_reg_t *hw, uint8_t channel, bool en)
{
    switch (channel) {
        case WM_PWM_CHANNEL_4:
        {
            hw->ch4_ctrl2.flien2 = en;
            break;
        }
        case WM_PWM_CHANNEL_0:
        {
            hw->int_ctrl.flien = en;
            break;
        }
        case WM_PWM_CHANNEL_1:
        case WM_PWM_CHANNEL_2:
        case WM_PWM_CHANNEL_3:
        default:
        {
            break;
        }
    }
}

bool wm_ll_pwm_get_falling_int_en(wm_pwm_reg_t *hw, uint8_t channel)
{
    switch (channel) {
        case WM_PWM_CHANNEL_4:
        {
            return hw->ch4_ctrl2.flien2;
        }
        case WM_PWM_CHANNEL_0:
        {
            return hw->int_ctrl.flien;
        }
        case WM_PWM_CHANNEL_1:
        case WM_PWM_CHANNEL_2:
        case WM_PWM_CHANNEL_3:
        default:
        {
            return false;
        }
    }
}

void wm_ll_pwm_set_rising_int_en(wm_pwm_reg_t *hw, uint8_t channel, bool en)
{
    switch (channel) {
        case WM_PWM_CHANNEL_4:
        {
            hw->ch4_ctrl2.rlien2 = en;
            break;
        }
        case WM_PWM_CHANNEL_0:
        {
            hw->int_ctrl.rlien = en;
            break;
        }
        case WM_PWM_CHANNEL_1:
        case WM_PWM_CHANNEL_2:
        case WM_PWM_CHANNEL_3:
        default:
        {
            break;
        }
    }
}

bool wm_ll_pwm_get_rising_int_en(wm_pwm_reg_t *hw, uint8_t channel)
{
    switch (channel) {
        case WM_PWM_CHANNEL_4:
        {
            return hw->ch4_ctrl2.rlien2;
        }
        case WM_PWM_CHANNEL_0:
        {
            return hw->int_ctrl.rlien;
        }
        case WM_PWM_CHANNEL_1:
        case WM_PWM_CHANNEL_2:
        case WM_PWM_CHANNEL_3:
        default:
        {
            return false;
        }
    }
}

void wm_ll_pwm_set_period_int_en(wm_pwm_reg_t *hw, uint8_t channel, bool en)
{
    if (en) {
        hw->int_ctrl.pien |= WM_BIT(channel);
    } else {
        hw->int_ctrl.pien &= ~WM_BIT(channel);
    }
}

bool wm_ll_pwm_get_period_int_en(wm_pwm_reg_t *hw, uint8_t channel)
{
    return (hw->int_ctrl.pien & WM_BIT(channel));
}

void wm_ll_pwm_clear_cnt_overflow_status(wm_pwm_reg_t *hw, uint8_t channel)
{
    switch (channel) {
        case WM_PWM_CHANNEL_4:
        {
            hw->ch4_ctrl2.overfl2 = 1;
            break;
        }
        case WM_PWM_CHANNEL_0:
        {
            hw->int_status.overfl = 1;
            break;
        }
        case WM_PWM_CHANNEL_1:
        case WM_PWM_CHANNEL_2:
        case WM_PWM_CHANNEL_3:
        default:
        {
            break;
        }
    }
}

bool wm_ll_pwm_get_cnt_overflow_status(wm_pwm_reg_t *hw, uint8_t channel)
{
    if (channel == WM_PWM_CHANNEL_4) {
        return hw->ch4_ctrl2.overfl2;
    } else if (channel == WM_PWM_CHANNEL_0) {
        return hw->int_status.overfl;
    } else {
        return false;
    }
}

void wm_ll_pwm_clear_falling_int_overrun_status(wm_pwm_reg_t *hw, uint8_t channel)
{
    switch (channel) {
        case WM_PWM_CHANNEL_4:
        {
            hw->ch4_ctrl2.flifov2 = 1;
            break;
        }
        case WM_PWM_CHANNEL_0:
        {
            hw->int_status.flifov = 1;
            break;
        }
        case WM_PWM_CHANNEL_1:
        case WM_PWM_CHANNEL_2:
        case WM_PWM_CHANNEL_3:
        default:
        {
            break;
        }
    }
}

bool wm_ll_pwm_get_falling_int_overrun_status(wm_pwm_reg_t *hw, uint8_t channel)
{
    switch (channel) {
        case WM_PWM_CHANNEL_4:
        {
            return hw->ch4_ctrl2.flifov2;
        }
        case WM_PWM_CHANNEL_0:
        {
            return hw->int_status.flifov;
        }
        case WM_PWM_CHANNEL_1:
        case WM_PWM_CHANNEL_2:
        case WM_PWM_CHANNEL_3:
        default:
        {
            return false;
        }
    }
}

void wm_ll_pwm_clear_rising_int_overrun_status(wm_pwm_reg_t *hw, uint8_t channel)
{
    switch (channel) {
        case WM_PWM_CHANNEL_4:
        {
            hw->ch4_ctrl2.rlifov2 = 1;
            break;
        }
        case WM_PWM_CHANNEL_0:
        {
            hw->int_status.rlifov = 1;
            break;
        }
        case WM_PWM_CHANNEL_1:
        case WM_PWM_CHANNEL_2:
        case WM_PWM_CHANNEL_3:
        default:
        {
            break;
        }
    }
}

bool wm_ll_pwm_get_rising_int_overrun_status(wm_pwm_reg_t *hw, uint8_t channel)
{
    switch (channel) {
        case WM_PWM_CHANNEL_4:
        {
            return hw->ch4_ctrl2.rlifov2;
        }
        case WM_PWM_CHANNEL_0:
        {
            return hw->int_status.rlifov;
        }
        case WM_PWM_CHANNEL_1:
        case WM_PWM_CHANNEL_2:
        case WM_PWM_CHANNEL_3:
        default:
        {
            return false;
        }
    }
}

void wm_ll_pwm_clear_cap_falling_int_status(wm_pwm_reg_t *hw, uint8_t channel)
{
    switch (channel) {
        case WM_PWM_CHANNEL_4:
        {
            hw->ch4_ctrl2.cflif2 = 1;
            break;
        }
        case WM_PWM_CHANNEL_0:
        {
            hw->int_status.cflir = 1;
            break;
        }
        case WM_PWM_CHANNEL_1:
        case WM_PWM_CHANNEL_2:
        case WM_PWM_CHANNEL_3:
        default:
        {
            break;
        }
    }
}

bool wm_ll_pwm_get_cap_falling_int_status(wm_pwm_reg_t *hw, uint8_t channel)
{
    switch (channel) {
        case WM_PWM_CHANNEL_4:
        {
            return hw->ch4_ctrl2.cflif2;
        }
        case WM_PWM_CHANNEL_0:
        {
            return hw->int_status.cflir;
        }
        case WM_PWM_CHANNEL_1:
        case WM_PWM_CHANNEL_2:
        case WM_PWM_CHANNEL_3:
        default:
        {
            return false;
        }
    }
}

void wm_ll_pwm_clear_cap_rising_int_status(wm_pwm_reg_t *hw, uint8_t channel)
{
    switch (channel) {
        case WM_PWM_CHANNEL_4:
        {
            hw->ch4_ctrl2.crlif2 = 1;
            break;
        }
        case WM_PWM_CHANNEL_0:
        {
            hw->int_status.crlif = 1;
            break;
        }
        case WM_PWM_CHANNEL_1:
        case WM_PWM_CHANNEL_2:
        case WM_PWM_CHANNEL_3:
        default:
        {
            break;
        }
    }
}

bool wm_ll_pwm_get_cap_rising_int_status(wm_pwm_reg_t *hw, uint8_t channel)
{
    switch (channel) {
        case WM_PWM_CHANNEL_4:
        {
            return hw->ch4_ctrl2.crlif2;
        }
        case WM_PWM_CHANNEL_0:
        {
            return hw->int_status.crlif;
        }
        case WM_PWM_CHANNEL_1:
        case WM_PWM_CHANNEL_2:
        case WM_PWM_CHANNEL_3:
        default:
        {
            return false;
        }
    }
}

void wm_ll_pwm_clear_all_int_status(wm_pwm_reg_t *hw)
{
    hw->int_status.val   = (uint32_t)(-1);
    hw->int_status.pif   = (WM_BIT(WM_PWM_CHANNEL_MAX) - 1);
    hw->ch4_ctrl2.crlif2 = true;
    hw->ch4_ctrl2.cflif2 = true;
}

uint16_t wm_ll_pwm_get_cap_value(wm_pwm_reg_t *hw, uint8_t channel, bool rising)
{
    if (channel == WM_PWM_CHANNEL_0) {
        if (rising) {
            return wm_ll_pwm_get_ch0_rising_cap_value(hw);
        } else {
            return wm_ll_pwm_get_ch0_falling_cap_value(hw);
        }
    } else if (channel == WM_PWM_CHANNEL_4) {
        if (rising) {
            return wm_ll_pwm_get_ch4_rising_cap_value(hw);
        } else {
            return wm_ll_pwm_get_ch4_falling_cap_value(hw);
        }
    }

    return 0;
}

void wm_ll_pwm_set_brake_en(wm_pwm_reg_t *hw, uint8_t channel, int en)
{
    if (en) {
        hw->brake.brkctl |= WM_BIT(channel);
    } else {
        hw->brake.brkctl &= ~WM_BIT(channel);
    }
}

void wm_ll_pwm_set_brake_level(wm_pwm_reg_t *hw, uint8_t channel, int high)
{
    if (high) {
        hw->brake.bkod |= WM_BIT(channel);
    } else {
        hw->brake.bkod &= ~WM_BIT(channel);
    }
}

uint32_t wm_ll_pwm_get_cap_addr(wm_pwm_reg_t *hw, uint8_t channel)
{
    if (channel == WM_PWM_CHANNEL_0) {
        return (uint32_t)(&hw->ch0_capture.val);
    } else if (channel == WM_PWM_CHANNEL_4) {
        return (uint32_t)(&hw->ch4_capture.val);
    }

    return 0;
}
