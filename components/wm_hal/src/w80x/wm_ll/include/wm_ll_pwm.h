/**
 * @file wm_ll_pwm.h
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

#ifndef __WM_LL_PWM_H__
#define __WM_LL_PWM_H__

#include "wm_pwm_reg_struct.h"
#include "wm_attr.h"
#include "wm_reg_op.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief          Set clock divider for ch0~ch4
 * @param[in]      hw Peripheral PWM hardware instance address
 *                 hw is the type of @arg wm_pwm_reg_t
 * @param[in]      channel is the channel of then PWM controller
 *                 type of @enum wm_ll_pwm_channels, range 0~4
 * @param[in]      div is the value of clock divider
 */
void wm_ll_pwm_set_clkdiv(wm_pwm_reg_t *hw, uint8_t channel, uint16_t div);

/**
 * @brief          Get clock divider for ch0~ch4
 * @param[in]      hw Peripheral PWM hardware instance address
 *                 hw is the type of @arg wm_pwm_reg_t
 * @param[in]      channel is the channel of then PWM controller
 *                 type of @enum wm_ll_pwm_channels, range 0~4
 * @return         the divider value
 */
uint16_t wm_ll_pwm_get_clkdiv(wm_pwm_reg_t *hw, uint8_t channel);

/**
 * @brief          Enable / disable pwm counter function for the specified channel
 * @param[in]      hw Peripheral PWM hardware instance address
 *                 hw is the type of @arg wm_pwm_reg_t
 * @param[in]      channel is the channel of then PWM controller
 *                 type of @enum wm_ll_pwm_channels, range 0~4
 * @param[in]      en True if enable the counter, otherwise False
 */
void wm_ll_pwm_set_chan_cnt_en(wm_pwm_reg_t *hw, uint8_t channel, bool en);

/**
 * @brief          Enable / disable pwm counter function for all channels
 * @param[in]      hw Peripheral PWM hardware instance address
 *                 hw is the type of @arg wm_pwm_reg_t
 * @param[in]      en True if enable the counter, otherwise False
 */
void wm_ll_pwm_set_chans_cnt_en(wm_pwm_reg_t *hw, bool en);

/**
 * @brief          Enable / disable pwm counter function for channels in bitmap
 * @param[in]      hw Peripheral PWM hardware instance address
 *                 hw is the type of @arg wm_pwm_reg_t
 * @param[in]      en True if enable the counter, otherwise False
 */
void wm_ll_pwm_set_chans_map_cnt_en(wm_pwm_reg_t *hw, uint8_t bits, bool en);

/**
 * @brief       Get counter state of PWM function for the specified channel
 * @param[in]   hw Peripheral PWM hardware instance address
 *              hw is of type @arg wm_pwm_reg_t
 * @param[in]   channel is the channel of the PWM controller
 *              type of @enum wm_ll_pwm_channels, range 0~4
 * @return      If the counter enable bit is 1, returns true; otherwise, returns false
 */
ATTRIBUTE_INLINE bool wm_ll_pwm_get_chan_cnt_en(wm_pwm_reg_t *hw, uint8_t channel)
{
    return (hw->ctrl.cnten & WM_BIT(channel));
}

/**
 * @brief          enable / disable the inverse function for channel 0
 * @param[in]      hw Peripheral PWM hardware instance address
 *                 hw is the type of @arg wm_pwm_reg_t
 * @param[in]      en means that whether enable the inverse function for channel 0
 */
ATTRIBUTE_INLINE void wm_ll_pwm_set_ch0_capture_inv_en(wm_pwm_reg_t *hw, bool en)
{
    hw->ctrl.capinv = en;
}

/**
 * @brief          Get if capture inverse function for channel 0 is enabled
 * @param[in]      hw Peripheral PWM hardware instance address
 *                 hw is the type of @arg wm_pwm_reg_t
 * @param[in]      en means that whether enable the inverse function for channel 0
 * @return         If the ch0 capture inverse enabled return True, otherwise False
 */
ATTRIBUTE_INLINE bool wm_ll_pwm_get_ch0_capture_inv_en(wm_pwm_reg_t *hw)
{
    return hw->ctrl.capinv;
}

/**
 * @brief          enable / disable the capture function for channel 0
 * @param[in]      hw Peripheral PWM hardware instance address
 *                 hw is the type of @arg wm_pwm_reg_t
 * @param[in]      en means that whether enable the capture function for channel 0
 */
ATTRIBUTE_INLINE void wm_ll_pwm_set_ch0_capture_en(wm_pwm_reg_t *hw, bool en)
{
    hw->ctrl.cpen = en;
}

/**
 * @brief          Get if  capture function for channel 0 is enabled
 * @param[in]      hw Peripheral PWM hardware instance address
 *                 hw is the type of @arg wm_pwm_reg_t
 * @param[in]      en means that whether enable the capture function for channel 0
 * @return         If the ch0 capture enabled return True, otherwise False
 */
ATTRIBUTE_INLINE bool wm_ll_pwm_get_ch0_capture_en(wm_pwm_reg_t *hw)
{
    return hw->ctrl.cpen;
}

/**
 * @brief          enable / disable the inverse function for channel 4
 * @param[in]      hw Peripheral PWM hardware instance address
 *                 hw is the type of @arg wm_pwm_reg_t
 * @param[in]      en means that whether enable the inverse function for channel 4
 */
ATTRIBUTE_INLINE void wm_ll_pwm_set_ch4_capture_inv_en(wm_pwm_reg_t *hw, bool en)
{
    hw->ch4_ctrl2.capinv2 = en;
}

/**
 * @brief          Get if the capture inverse function for channel 4 is enabled
 * @param[in]      hw Peripheral PWM hardware instance address
 *                 hw is the type of @arg wm_pwm_reg_t
 * @return         If the ch4 capture inverse enabled return True, otherwise False
 */
ATTRIBUTE_INLINE bool wm_ll_pwm_get_ch4_capture_inv_en(wm_pwm_reg_t *hw)
{
    return hw->ch4_ctrl2.capinv2;
}

/**
 * @brief          enable / disable the capture function for channel 4
 * @param[in]      hw Peripheral PWM hardware instance address
 *                 hw is the type of @arg wm_pwm_reg_t
 * @param[in]      en means that whether enable the capture function for channel 4
 */
ATTRIBUTE_INLINE void wm_ll_pwm_set_ch4_capture_en(wm_pwm_reg_t *hw, bool en)
{
    hw->ch4_ctrl2.cpen2 = en;
}

/**
 * @brief          Get if the capture function for channel 4 is enabled
 * @param[in]      hw Peripheral PWM hardware instance address
 *                 hw is the type of @arg wm_pwm_reg_t
 * @return         If the ch4 capture enabled return True, otherwise False
 */
ATTRIBUTE_INLINE bool wm_ll_pwm_get_ch4_capture_en(wm_pwm_reg_t *hw)
{
    return hw->ch4_ctrl2.cpen2;
}

/**
 * @brief          enable / disable the inverse function for specified channel
 * @param[in]      hw Peripheral PWM hardware instance address
 *                 hw is the type of @arg wm_pwm_reg_t
 * @param[in]      channel is the channel of then PWM controller
 *                 type of @enum wm_ll_pwm_channels, can be 0, 4
 * @param[in]      en means that whether enable the inverse function
 */
void wm_ll_pwm_set_capture_inv_en(wm_pwm_reg_t *hw, uint8_t channel, bool en);

/**
 * @brief          Get if  capture inverse function for ch0 or ch4 is enabled
 * @param[in]      hw Peripheral PWM hardware instance address
 *                 hw is the type of @arg wm_pwm_reg_t
 * @param[in]      channel is the channel of then PWM controller
 *                 type of @enum wm_ll_pwm_channels, can be 0, 4
 * @return         If the capture inverse enabled return True, otherwise False
 */
bool wm_ll_pwm_get_capture_inv_en(wm_pwm_reg_t *hw, uint8_t channel);

/**
 * @brief          Enable / disable the inverse function for specified channel
 * @param[in]      hw Peripheral PWM hardware instance address
 *                 hw is the type of @arg wm_pwm_reg_t
 * @param[in]      channel is the channel of then PWM controller
 *                 type of @enum wm_ll_pwm_channels, can be 0, 4
 * @param[in]      en means that whether enable the capture function
 */
void wm_ll_pwm_set_capture_en(wm_pwm_reg_t *hw, uint8_t channel, bool en);

/**
 * @brief          Get if  capture function for ch0 or ch4 is enabled
 * @param[in]      hw Peripheral PWM hardware instance address
 *                 hw is the type of @arg wm_pwm_reg_t
 * @param[in]      channel is the channel of then PWM controller
 *                 type of @enum wm_ll_pwm_channels, can be 0, 4
 * @return         If the capture enabled return True, otherwise False
 */
bool wm_ll_pwm_get_capture_en(wm_pwm_reg_t *hw, uint8_t channel);

/**
 * @brief          Set the counter mode(center / left edge / right edge) for channel each channel
 * @param[in]      hw Peripheral PWM hardware instance address
 *                 hw is the type of @arg wm_pwm_reg_t
 * @param[in]      channel is the channel of then PWM controller
 *                 type of @enum wm_ll_pwm_channels, range 0~4
 * @param[in]      cnt_type the PWM counter work mode @enum wm_ll_pwm_cnt_type
 */
void wm_ll_pwm_set_cnt_type(wm_pwm_reg_t *hw, uint8_t channel, uint8_t cnt_type);

/**
 * @brief          Get the counter mode(center / left edge / right edge) for channel each channel
 * @param[in]      hw Peripheral PWM hardware instance address
 *                 hw is the type of @arg wm_pwm_reg_t
 * @param[in]      channel is the channel of then PWM controller
 *                 type of @enum wm_ll_pwm_channels, range 0~4
 * @return         the PWM counter work mode @enum wm_ll_pwm_cnt_type
 */
uint8_t wm_ll_pwm_get_cnt_type(wm_pwm_reg_t *hw, uint8_t channel);

/**
 * @brief          enable two channel synchronization for specified channel pair
 * @param[in]      hw Peripheral PWM hardware instance address
 *                 hw is the type of @arg wm_pwm_reg_t
 * @param[in]      channel is the channel of then PWM controller
 *                 type of @enum wm_ll_pwm_channels, range 0~3
 * @param[in]      en enable synchronization or not
 */
void wm_ll_pwm_set_twosync_mode_en(wm_pwm_reg_t *hw, uint8_t channel, bool en);

/**
 * @brief          Get if the twosycn is enabled for the specified channel
 * @param[in]      hw Peripheral PWM hardware instance address
 *                 hw is the type of @arg wm_pwm_reg_t
 * @param[in]      channel is the channel of then PWM controller
 *                 type of @enum wm_ll_pwm_channels, range 0~3
 * @return         If the enabled return True, otherwise False
 */
bool wm_ll_pwm_get_twosync_mode_en(wm_pwm_reg_t *hw, uint8_t channel);

/**
 * @brief          Set channel which support capture to matched state mode,
 *                 which may be output or high impedance
 * @param[in]      hw Peripheral PWM hardware instance address
 *                 hw is the type of @arg wm_pwm_reg_t
 * @param[in]      channel is the channel of then PWM controller
 *                 type of @enum wm_ll_pwm_channels, ch0, ch4
 * @param[in]      sel the channel state to be selected
 */
void wm_ll_pwm_set_ch_output_state(wm_pwm_reg_t *hw, uint8_t channel, uint8_t sel);

/**
 * @brief          Set channel which support capture to matched state mode,
 *                 which may be output or high impedance
 * @param[in]      hw Peripheral PWM hardware instance address
 *                 hw is the type of @arg wm_pwm_reg_t
 * @param[in]      channel is the channel of then PWM controller
 *                 type of @enum wm_ll_pwm_channels, ch0, ch4
 * @return         the state for the specificed channel @enum wm_ll_pwm_out_state
 */
uint8_t wm_ll_pwm_get_ch_output_state(wm_pwm_reg_t *hw, uint8_t channel);

/**
 * @brief          enable loop or autoload output mode
 * @param[in]      hw Peripheral PWM hardware instance address
 *                 hw is the type of @arg wm_pwm_reg_t
 * @param[in]      channel is the channel of then PWM controller
 *                 type of @enum wm_ll_pwm_channels, range 0~4
 * @param[in]      en the PWM output autoload mode enable or not
 */
void wm_ll_pwm_set_autoload_mode_en(wm_pwm_reg_t *hw, uint8_t channel, bool en);

/**
 * @brief          Get if the autoload or loop mode is enabled
 * @param[in]      hw Peripheral PWM hardware instance address
 *                 hw is the type of @arg wm_pwm_reg_t
 * @param[in]      channel is the channel of then PWM controller
 *                 type of @enum wm_ll_pwm_channels, range 0~4
 * @return         If the enabled return True, otherwise False
 */
bool wm_ll_pwm_get_autoload_mode_en(wm_pwm_reg_t *hw, uint8_t channel);

/**
 * @brief          Enable all synchronization for channel 0 ~ 3
 * @param[in]      hw Peripheral PWM hardware instance address
 *                 hw is the type of @arg wm_pwm_reg_t
 * @param[in]      en enable synchronization or not
 */
ATTRIBUTE_INLINE void wm_ll_pwm_set_allsync_mode_en(wm_pwm_reg_t *hw, bool en)
{
    hw->ctrl.allsyncen = en;
}

/**
 * @brief          Get if the allsync mode is enabled
 * @param[in]      hw Peripheral PWM hardware instance address
 *                 hw is the type of @arg wm_pwm_reg_t
 * @return         If the enabled return True, otherwise False
 */
ATTRIBUTE_INLINE bool wm_ll_pwm_get_allsync_mode_en(wm_pwm_reg_t *hw)
{
    return hw->ctrl.allsyncen;
}

/**
 * @brief          Enable / disable PWM output polarity inversion configure
 * @param[in]      hw Peripheral PWM hardware instance address
 *                 hw is the type of @arg wm_pwm_reg_t
 * @param[in]      channel is the channel of then PWM controller
 *                 type of @enum wm_ll_pwm_channels, range 0~4
 * @param[in]      en enable polarity inverse or not
 */
void wm_ll_pwm_set_output_ploarity_inv_en(wm_pwm_reg_t *hw, uint8_t channel, int en);

/**
 * @brief          Get if the plority inverse is enabled for the specificed channel
 * @param[in]      hw Peripheral PWM hardware instance address
 *                 hw is the type of @arg wm_pwm_reg_t
 * @param[in]      channel is the channel of then PWM controller
 *                 type of @enum wm_ll_pwm_channels, range 0~4
 * @return         If the enabled return True, otherwise False
 */
bool wm_ll_pwm_get_output_ploarity_inv_en(wm_pwm_reg_t *hw, uint8_t channel);

/**
 * @brief          output mode config for complementary or not
 * @param[in]      hw Peripheral PWM hardware instance address
 *                 hw is the type of @arg wm_pwm_reg_t
 * @param[in]      channel is the channel of then PWM controller
 *                 type of @enum wm_ll_pwm_channels, range 0~3
 * @param[in]      en enable complement mode or not
 */
void wm_ll_pwm_set_complement_en(wm_pwm_reg_t *hw, uint8_t channel, bool en);

/**
 * @brief          Get if the complement is enabled for the specificed channel
 * @param[in]      hw Peripheral PWM hardware instance address
 *                 hw is the type of @arg wm_pwm_reg_t
 * @param[in]      channel is the channel of then PWM controller
 *                 type of @enum wm_ll_pwm_channels, range 0~4
 * @return         If the enabled return True, otherwise False
 */
bool wm_ll_pwm_get_complement_en(wm_pwm_reg_t *hw, uint8_t channel);

/**
 * @brief          Set the period for the specified channel
 * @param[in]      hw Peripheral PWM hardware instance address
 *                 hw is the type of @arg wm_pwm_reg_t
 * @param[in]      channel is the channel of then PWM controller
 *                 type of @enum wm_ll_pwm_channels, range 0~4
 * @param[in]      period the period of each channel
 */
void wm_ll_pwm_set_period(wm_pwm_reg_t *hw, uint8_t channel, uint8_t period);

/**
 * @brief          Get the period value for the specificed channel
 * @param[in]      hw Peripheral PWM hardware instance address
 *                 hw is the type of @arg wm_pwm_reg_t
 * @param[in]      channel is the channel of then PWM controller
 *                 type of @enum wm_ll_pwm_channels, range 0~4
 * @return         the period time value
 */
uint8_t wm_ll_pwm_get_period(wm_pwm_reg_t *hw, uint8_t channel);

/**
 * @brief          Set the number of period to be generated
 * @param[in]      hw Peripheral PWM hardware instance address
 *                 hw is the type of @arg wm_pwm_reg_t
 * @param[in]      channel is the channel of then PWM controller
 *                 type of @enum wm_ll_pwm_channels, range 0~4
 * @param[in]      num the period num to be generated
 */
void wm_ll_pwm_set_period_num(wm_pwm_reg_t *hw, uint8_t channel, uint8_t num);

/**
 * @brief          Get the period num value for the specificed channel
 * @param[in]      hw Peripheral PWM hardware instance address
 *                 hw is the type of @arg wm_pwm_reg_t
 * @param[in]      channel is the channel of then PWM controller
 *                 type of @enum wm_ll_pwm_channels, range 0~4
 * @return         the period num value
 */
uint8_t wm_ll_pwm_get_period_num(wm_pwm_reg_t *hw, uint8_t channel);

/**
 * @brief          Set the CMP register value
 * @param[in]      hw Peripheral PWM hardware instance address
 *                 hw is the type of @arg wm_pwm_reg_t
 * @param[in]      channel is the channel of then PWM controller
 *                 type of @enum wm_ll_pwm_channels, range 0~4
 * @param[in]      value the CMP value to be used
 */
void wm_ll_pwm_set_cmp_cnt(wm_pwm_reg_t *hw, uint8_t channel, uint8_t value);

/**
 * @brief          Get the CMP register value
 * @param[in]      hw Peripheral PWM hardware instance address
 *                 hw is the type of @arg wm_pwm_reg_t
 * @param[in]      channel is the channel of then PWM controller
 *                 type of @enum wm_ll_pwm_channels, range 0~4
 * @return         the CMP value
 */
uint8_t wm_ll_pwm_get_cmp_cnt(wm_pwm_reg_t *hw, uint8_t channel);

/**
 * @brief          enable / disable dead time zone for complement mode
 * @param[in]      hw Peripheral PWM hardware instance address
 *                 hw is the type of @arg wm_pwm_reg_t
 * @param[in]      channel is the channel of then PWM controller
 *                 type of @enum wm_ll_pwm_channels, range 0~4
 *                 (ch0+ch1, ch2+ch3)
 * @param[in]      en if enable dead time zone or not
 */
void wm_ll_pwm_set_dead_time_en(wm_pwm_reg_t *hw, uint8_t channel, bool en);

/**
 * @brief          check dead zone if valid for specified channel pair
 * @param[in]      hw Peripheral PWM hardware instance address
 *                 hw is the type of @arg wm_pwm_reg_t
 * @param[in]      channel is the channel of then PWM controller
 *                 type of @enum wm_ll_pwm_channels, range 0~4
 *                 (ch0+ch1, ch2+ch3)
 * @return         True if dead time valid, otherwise False
 */
bool wm_ll_pwm_get_dead_time_en(wm_pwm_reg_t *hw, uint8_t channel);

/**
 * @brief          Set the dead zone clock divider register
 * @param[in]      hw Peripheral PWM hardware instance address
 *                 hw is the type of @arg wm_pwm_reg_t
 * @param[in]      value the dead time clock divider value to be used
 */
ATTRIBUTE_INLINE void wm_ll_pwm_set_dead_time_clkdiv(wm_pwm_reg_t *hw, uint8_t div)
{
    hw->dead_time.dtdiv = div;
}

/**
 * @brief          Get the dead zone clock divider register
 * @param[in]      hw Peripheral PWM hardware instance address
 *                 hw is the type of @arg wm_pwm_reg_t
 */
ATTRIBUTE_INLINE uint8_t wm_ll_pwm_get_dead_time_clkdiv(wm_pwm_reg_t *hw)
{
    return hw->dead_time.dtdiv;
}

/**
 * @brief          Set the dead zone interval between each ch pair register
 * @param[in]      hw Peripheral PWM hardware instance address
 *                 hw is the type of @arg wm_pwm_reg_t
 * @param[in]      channel is the channel of then PWM controller
 *                 type of @enum wm_ll_pwm_channels, range 0~4
 * @param[in]      value the dead time interval value to be used
 */
void wm_ll_pwm_set_dead_time_cnt(wm_pwm_reg_t *hw, uint8_t channel, uint8_t cnt);

/**
 * @brief          Get the dead zone interval between each ch pair register
 * @param[in]      hw Peripheral PWM hardware instance address
 *                 hw is the type of @arg wm_pwm_reg_t
 * @param[in]      channel is the channel of then PWM controller
 *                 type of @enum wm_ll_pwm_channels, range 0~4
 * @return         the interval counter value of dead time section
 */
uint8_t wm_ll_pwm_get_dead_time_cnt(wm_pwm_reg_t *hw, uint8_t channel);

/**
 * @brief          Set DMA request interrupt enable for cha0~ch3
 * @param[in]      hw Peripheral PWM hardware instance address
 *                 hw is the type of @arg wm_pwm_reg_t
 * @param[in]      channel is the channel of then PWM controller
 *                 type of @enum wm_ll_pwm_channels, range 0~4
 * @param[in]      en the flag indicated interrupt enable
 */
void wm_ll_pwm_set_dma_request_int_en(wm_pwm_reg_t *hw, uint8_t channel, bool en);

/**
 * @brief          Get DMA request interrupt enable for cha0~ch3
 * @param[in]      hw Peripheral PWM hardware instance address
 *                 hw is the type of @arg wm_pwm_reg_t
 * @param[in]      channel is the channel of then PWM controller
 *                 type of @enum wm_ll_pwm_channels, range 0~4
 * @return         True if dma interrupt is enabled, otherwise False
 */
bool wm_ll_pwm_get_dma_request_int_en(wm_pwm_reg_t *hw, uint8_t channel);

/**
 * @brief          Enable / disable falling edge interrupt
 * @param[in]      hw Peripheral PWM hardware instance address
 *                 hw is the type of @arg wm_pwm_reg_t
 * @param[in]      channel is the channel of then PWM controller
 *                 type of @enum wm_ll_pwm_channels, range 0~4
 * @param[in]      en flag indicated interrupt enable
 */
void wm_ll_pwm_set_falling_int_en(wm_pwm_reg_t *hw, uint8_t channel, bool en);

/**
 * @brief          Get if falling edge interrupt is enabled
 * @param[in]      hw Peripheral PWM hardware instance address
 *                 hw is the type of @arg wm_pwm_reg_t
 * @param[in]      channel is the channel of then PWM controller
 *                 type of @enum wm_ll_pwm_channels, range 0~4
 * @return         True if falling edge interrupt is enabled, otherwise False
 */
bool wm_ll_pwm_get_falling_int_en(wm_pwm_reg_t *hw, uint8_t channel);

/**
 * @brief          Enable / disable rising edge interrupt
 * @param[in]      hw Peripheral PWM hardware instance address
 *                 hw is the type of @arg wm_pwm_reg_t
 * @param[in]      channel is the channel of then PWM controller
 *                 type of @enum wm_ll_pwm_channels, range 0~4
 * @param[in]      en flag indicated interrupt enable
 */
void wm_ll_pwm_set_rising_int_en(wm_pwm_reg_t *hw, uint8_t channel, bool en);

/**
 * @brief          Get if rising edge interrupt is enabled
 * @param[in]      hw Peripheral PWM hardware instance address
 *                 hw is the type of @arg wm_pwm_reg_t
 * @param[in]      channel is the channel of then PWM controller
 *                 type of @enum wm_ll_pwm_channels, range 0~4
 * @return         True if rising edge interrupt is enabled, otherwise False
 */
bool wm_ll_pwm_get_rising_int_en(wm_pwm_reg_t *hw, uint8_t channel);

/**
 * @brief          Enable / disable period interrupt
 * @param[in]      hw Peripheral PWM hardware instance address
 *                 hw is the type of @arg wm_pwm_reg_t
 * @param[in]      channel is the channel of then PWM controller
 *                 type of @enum wm_ll_pwm_channels, range 0~4
 */
void wm_ll_pwm_set_period_int_en(wm_pwm_reg_t *hw, uint8_t channel, bool en);

/**
 * @brief          Get if period interrupt is enabled
 * @param[in]      hw Peripheral PWM hardware instance address
 *                 hw is the type of @arg wm_pwm_reg_t
 * @param[in]      channel is the channel of then PWM controller
 *                 type of @enum wm_ll_pwm_channels, range 0~4
 * @return         True if period interrupt is enabled, otherwise False
 */
bool wm_ll_pwm_get_period_int_en(wm_pwm_reg_t *hw, uint8_t channel);

/**
 * @brief          clear interrupt status
 * @param[in]      hw Peripheral PWM hardware instance address
 *                 hw is the type of @arg wm_pwm_reg_t
 */
ATTRIBUTE_INLINE void wm_ll_pwm_clear_int_status(wm_pwm_reg_t *hw)
{
    hw->int_status.val = 0;
}

/**
 * @brief          Clear counter overflow status
 * @param[in]      hw Peripheral PWM hardware instance address
 *                 hw is the type of @arg wm_pwm_reg_t
 * @param[in]      channel is the channel of then PWM controller
 *                 type of @enum wm_ll_pwm_channels, range 0~4
 */
void wm_ll_pwm_clear_cnt_overflow_status(wm_pwm_reg_t *hw, uint8_t channel);

/**
 * @brief          Get counter overflow status
 * @param[in]      hw Peripheral PWM hardware instance address
 *                 hw is the type of @arg wm_pwm_reg_t
 * @param[in]      channel is the channel of then PWM controller
 *                 type of @enum wm_ll_pwm_channels, range 0~4
 * @return         True if counter overflow interrupt is set, otherwise False
 */
bool wm_ll_pwm_get_cnt_overflow_status(wm_pwm_reg_t *hw, uint8_t channel);

/**
 * @brief          Clear Falling edge delay interrupt flag
 * @param[in]      hw Peripheral PWM hardware instance address
 *                 hw is the type of @arg wm_pwm_reg_t
 * @param[in]      channel is the channel of then PWM controller
 *                 type of @enum wm_ll_pwm_channels, range 0~4
 */
void wm_ll_pwm_clear_falling_int_overrun_status(wm_pwm_reg_t *hw, uint8_t channel);

/**
 * @brief          Get Falling edge delay interrupt flag
 * @param[in]      hw Peripheral PWM hardware instance address
 *                 hw is the type of @arg wm_pwm_reg_t
 * @param[in]      channel is the channel of then PWM controller
 *                 type of @enum wm_ll_pwm_channels, range 0~4
 * @return         True if falling edge delay intrrupt flag is set
 */
bool wm_ll_pwm_get_falling_int_overrun_status(wm_pwm_reg_t *hw, uint8_t channel);

/**
 * @brief          Get Rising edge delay interrupt flag
 * @param[in]      hw Peripheral PWM hardware instance address
 *                 hw is the type of @arg wm_pwm_reg_t
 * @param[in]      channel is the channel of then PWM controller
 *                 type of @enum wm_ll_pwm_channels, range 0~4
 */
void wm_ll_pwm_clear_rising_int_overrun_status(wm_pwm_reg_t *hw, uint8_t channel);

/**
 * @brief          Get Rising edge delay interrupt flag
 * @param[in]      hw Peripheral PWM hardware instance address
 *                 hw is the type of @arg wm_pwm_reg_t
 * @param[in]      channel is the channel of then PWM controller
 *                 type of @enum wm_ll_pwm_channels, range 0~4
 * @return         True if rising edge delay intrrupt flag is set
 */
bool wm_ll_pwm_get_rising_int_overrun_status(wm_pwm_reg_t *hw, uint8_t channel);

/**
 * @brief          Clear Capture falling edge interrupt flag
 * @param[in]      hw Peripheral PWM hardware instance address
 *                 hw is the type of @arg wm_pwm_reg_t
 * @param[in]      channel is the channel of then PWM controller
 *                 type of @enum wm_ll_pwm_channels, range 0~4
 */
void wm_ll_pwm_clear_cap_falling_int_status(wm_pwm_reg_t *hw, uint8_t channel);

/**
 * @brief          Get Capture falling edge interrupt flag
 * @param[in]      hw Peripheral PWM hardware instance address
 *                 hw is the type of @arg wm_pwm_reg_t
 * @param[in]      channel is the channel of then PWM controller
 *                 type of @enum wm_ll_pwm_channels, range 0~4
 */
bool wm_ll_pwm_get_cap_falling_int_status(wm_pwm_reg_t *hw, uint8_t channel);

/**
 * @brief          Clear Capture rising edge interrupt flag
 * @param[in]      hw Peripheral PWM hardware instance address
 *                 hw is the type of @arg wm_pwm_reg_t
 * @param[in]      channel is the channel of then PWM controller
 *                 type of @enum wm_ll_pwm_channels, range 0~4
 */
void wm_ll_pwm_clear_cap_rising_int_status(wm_pwm_reg_t *hw, uint8_t channel);

/**
 * @brief          Get Capture rising edge interrupt flag
 * @param[in]      hw Peripheral PWM hardware instance address
 *                 hw is the type of @arg wm_pwm_reg_t
 * @param[in]      channel is the channel of then PWM controller
 *                 type of @enum wm_ll_pwm_channels, range 0~4
 * @return         True if capture rising edge intrrupt flag is set
 */
bool wm_ll_pwm_get_cap_rising_int_status(wm_pwm_reg_t *hw, uint8_t channel);

/**
 * @brief          Clear channel period interrupt status
 * @param[in]      hw Peripheral PWM hardware instance address
 *                 hw is the type of @arg wm_pwm_reg_t
 * @param[in]      channel is the channel of then PWM controller
 *                 type of @enum wm_ll_pwm_channels, range 0~4
 */
ATTRIBUTE_INLINE void wm_ll_pwm_clear_period_int_status(wm_pwm_reg_t *hw, uint8_t channel)
{
    hw->int_status.pif |= (WM_BIT(channel));
}

/**
 * @brief          Get channel period interrupt status
 * @param[in]      hw Peripheral PWM hardware instance address
 *                 hw is the type of @arg wm_pwm_reg_t
 * @param[in]      channel is the channel of then PWM controller
 *                 type of @enum wm_ll_pwm_channels, range 0~4
 * @return         True if period interrupt is set, otherwise False
 */
ATTRIBUTE_INLINE bool wm_ll_pwm_get_period_int_status(wm_pwm_reg_t *hw, uint8_t channel)
{
    return (hw->int_status.pif & WM_BIT(channel));
}

/**
 * @brief          clear all channel interrupt status
 * @param[in]      hw Peripheral PWM hardware instance address
 *                 hw is the type of @arg wm_pwm_reg_t
 * @param[in]      channel is the channel of then PWM controller
 *                 type of @enum wm_ll_pwm_channels, range 0~4
 */
void wm_ll_pwm_clear_all_int_status(wm_pwm_reg_t *hw);

/**
 * @brief          Get channel 0 capture value from falling edge register
 * @param[in]      hw Peripheral PWM hardware instance address
 *                 hw is the type of @arg wm_pwm_reg_t
 * @return         The ch0 apature value of falling edge register
 */
ATTRIBUTE_INLINE uint16_t wm_ll_pwm_get_ch0_falling_cap_value(wm_pwm_reg_t *hw)
{
    // return hw->ch0_capture.pwm_fcapdat;
    return WM_LL_REG_FILED_COPY_READ(hw->ch0_capture, pwm_fcapdat);
}

/**
 * @brief          Get channel 0 capture value from rising edge register
 * @param[in]      hw Peripheral PWM hardware instance address
 *                 hw is the type of @arg wm_pwm_reg_t
 * @return         The ch0 apature value of rising edge register
 */
ATTRIBUTE_INLINE uint16_t wm_ll_pwm_get_ch0_rising_cap_value(wm_pwm_reg_t *hw)
{
    // return hw->ch0_capture.pwm_rcapdat;
    return WM_LL_REG_FILED_COPY_READ(hw->ch0_capture, pwm_rcapdat);
}
/**
 * @brief          Get channel 4 capture value from falling edge register
 * @param[in]      hw Peripheral PWM hardware instance address
 *                 hw is the type of @arg wm_pwm_reg_t
 * @return         The ch4 apature value of falling edge register
 */
ATTRIBUTE_INLINE uint16_t wm_ll_pwm_get_ch4_falling_cap_value(wm_pwm_reg_t *hw)
{
    // return hw->ch4_capture.pwm_fcap2dat;
    return WM_LL_REG_FILED_COPY_READ(hw->ch4_capture, pwm_fcap2dat);
}

/**
 * @brief          Get channel 4 capture value from rising edge register
 * @param[in]      hw Peripheral PWM hardware instance address
 *                 hw is the type of @arg wm_pwm_reg_t
 * @return         The ch4 apature value of rising edge register
 */
ATTRIBUTE_INLINE uint16_t wm_ll_pwm_get_ch4_rising_cap_value(wm_pwm_reg_t *hw)
{
    // return hw->ch4_capture.pwm_rcap2dat;
    return WM_LL_REG_FILED_COPY_READ(hw->ch4_capture, pwm_rcap2dat);
}

/**
 * @brief          Get target channel capture value from specified edge register
 * @param[in]      hw Peripheral PWM hardware instance address
 *                 hw is the type of @arg wm_pwm_reg_t
 * @param[in]      channel is the channel of then PWM controller
 *                 type of @enum wm_ll_pwm_channels, can be 0, 4
 * @param[in]      rising if get rising edge capture value
 * @return         The ch4 apature value of rising edge register
 */
uint16_t wm_ll_pwm_get_cap_value(wm_pwm_reg_t *hw, uint8_t channel, bool rising);

/**
 * @brief          Enable / Disable the PWM brake function
 * @param[in]      hw Peripheral PWM hardware instance address
 *                 hw is the type of @arg wm_pwm_reg_t
 * @param[in]      channel is the channel of then PWM controller
 *                 type of @enum wm_ll_pwm_channels, range 0~4
 * @param[in]      en enable brake or not
 */
void wm_ll_pwm_set_brake_en(wm_pwm_reg_t *hw, uint8_t channel, int en);

/**
 * @brief          Get if the PWM brake function is enabled
 * @param[in]      hw Peripheral PWM hardware instance address
 *                 hw is the type of @arg wm_pwm_reg_t
 * @param[in]      channel is the channel of then PWM controller
 *                 type of @enum wm_ll_pwm_channels, range 0~4
 * @return         True if the brake mode is enabled, otherwise False
 */
ATTRIBUTE_INLINE bool wm_ll_pwm_get_brake_en(wm_pwm_reg_t *hw, uint8_t channel)
{
    return (hw->brake.brkctl & WM_BIT(channel));
}

/**
 * @brief          Set the active level for the specified channel
 * @param[in]      hw Peripheral PWM hardware instance address
 *                 hw is the type of @arg wm_pwm_reg_t
 * @param[in]      channel is the channel of then PWM controller
 *                 type of @enum wm_ll_pwm_channels, range 0~4
 * @param[in]      high True when output high level for brake mode
 */
void wm_ll_pwm_set_brake_level(wm_pwm_reg_t *hw, uint8_t channel, int high);

/**
 * @brief          Get the active level for the specified channel
 * @param[in]      hw Peripheral PWM hardware instance address
 *                 hw is the type of @arg wm_pwm_reg_t
 * @param[in]      channel is the channel of then PWM controller
 *                 type of @enum wm_ll_pwm_channels, range 0~4
 * @return         True if the brake level is high, otherwise False
 */
ATTRIBUTE_INLINE bool wm_ll_pwm_get_brake_level_high(wm_pwm_reg_t *hw, uint8_t channel)
{
    return (hw->brake.bkod & WM_BIT(channel));
}

/**
 * @brief          Get the capture value register address
 * @param[in]      hw Peripheral PWM hardware instance address
 *                 hw is the type of @arg wm_pwm_reg_t
 * @param[in]      channel is the channel of then PWM controller
 *                 type of @enum wm_ll_pwm_channels, range 0~4
 * @return         the register address of capture used by DMA source
 */
uint32_t wm_ll_pwm_get_cap_addr(wm_pwm_reg_t *hw, uint8_t channel);

#ifdef __cplusplus
}
#endif

#endif /* __WM_LL_PWM_H__ */
