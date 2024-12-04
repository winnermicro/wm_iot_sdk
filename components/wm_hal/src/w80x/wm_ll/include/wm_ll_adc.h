/**
 * @file wm_ll_adc.h
 *
 * @brief adc Low Layer Module
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

#ifndef __WM_LL_ADC_H__
#define __WM_LL_ADC_H__

#include "wm_attr.h"
#include "wm_adc_reg_struct.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief          Get adc result value 
 * @param[in]      hw Peripheral ADC hardware instance address
 *                 hw is the type of @arg adc_reg_t
 * @return         adc result value.
 *                 Signed numbers with the most significant bit being the sign bit
 */
ATTRIBUTE_INLINE int wm_ll_adc_get_result_val(wm_adc_reg_t *hw)
{
    return WM_LL_REG_FILED_COPY_READ(hw->result, result_val);
}

/**
 * @brief          Enable/Disable the LDO power supply
 * @param[in]      hw Peripheral ADC hardware instance address
 *                 hw is the type of @arg adc_reg_t
 * @param[in]      en 0: disable ; 1: enable
 */
ATTRIBUTE_INLINE void wm_ll_adc_en_ldo_sdadc(wm_adc_reg_t *hw, int en)
{
    WM_LL_REG_FILED_COPY_WRTIE(hw->ana_ctrl, en_ldo_sdadc, en);
}

/**
 * @brief          Get the LDO power supply enable status
 * @param[in]      hw Peripheral ADC hardware instance address
 *                 hw is the type of @arg adc_reg_t
 * @return         LDO power supply enable status.
 *                 1: enable, 0: disable
 */
ATTRIBUTE_INLINE int wm_ll_adc_get_en_ldo_sdadc(wm_adc_reg_t *hw, int en)
{
    return WM_LL_REG_FILED_COPY_READ(hw->ana_ctrl, en_ldo_sdadc);
}

/**
 * @brief          Analog module digital logic part reset signal
 * @param[in]      hw Peripheral ADC hardware instance address
 *                 hw is the type of @arg adc_reg_t
 * @param[in]      reset 0: reset ; 1: normal work
 */
ATTRIBUTE_INLINE void wm_ll_adc_reset_sdadc(wm_adc_reg_t *hw, int reset)
{
    WM_LL_REG_FILED_COPY_WRTIE(hw->ana_ctrl, rstn_sdadc, reset);
}

/**
 * @brief          Get the analog module digital logic part reset status
 * @param[in]      hw Peripheral ADC hardware instance address
 *                 hw is the type of @arg adc_reg_t
 * @return         analog module digital logic part reset status
 */
ATTRIBUTE_INLINE int wm_ll_adc_get_reset_sdadc(wm_adc_reg_t *hw, int reset)
{
    return WM_LL_REG_FILED_COPY_READ(hw->ana_ctrl, rstn_sdadc);
}

/**
 * @brief          Enable/Disable the sdadc analog module is powered down
 * @param[in]      hw Peripheral ADC hardware instance address
 *                 hw is the type of @arg adc_reg_t
 * @param[in]      en 0: normal work ; 1: power_off
 */
ATTRIBUTE_INLINE void wm_ll_adc_en_pd_sdadc(wm_adc_reg_t *hw, int en)
{
    WM_LL_REG_FILED_COPY_WRTIE(hw->ana_ctrl, en_pd_sdadc, en);
}

/**
 * @brief          Get the sdadc analog module is powered down status
 * @param[in]      hw Peripheral ADC hardware instance address
 *                 hw is the type of @arg adc_reg_t
 * @return         sdadc analog module is powered down status.
 *                 0: normal work ; 1: power_off
 */
ATTRIBUTE_INLINE int wm_ll_adc_get_en_pd_sdadc(wm_adc_reg_t *hw, int en)
{
    return WM_LL_REG_FILED_COPY_READ(hw->ana_ctrl, en_pd_sdadc);
}

/**
 * @brief          Enable/Disable sdadc chopper signal PD signal
 * @param[in]      hw Peripheral ADC hardware instance address
 *                 hw is the type of @arg adc_reg_t
 * @param[in]      en 0: enable ; 1: disable
 */
ATTRIBUTE_INLINE void wm_ll_adc_chop_ens(wm_adc_reg_t *hw, int en)
{
    WM_LL_REG_FILED_COPY_WRTIE(hw->ana_ctrl, chop_ens, en);
}

/**
 * @brief          Get sdadc chopper signal PD signal status
 * @param[in]      hw Peripheral ADC hardware instance address
 *                 hw is the type of @arg adc_reg_t
 * @return         sdadc chopper signal PD signal status.
 *                 0: enable ; 1: disable
 */
ATTRIBUTE_INLINE int wm_ll_adc_get_chop_ens(wm_adc_reg_t *hw, int en)
{
    return WM_LL_REG_FILED_COPY_READ(hw->ana_ctrl, chop_ens);
}

/**
 * @brief          Set LDO chopper signal PD signal
 * @param[in]      hw Peripheral ADC hardware instance address
 *                 hw is the type of @arg adc_reg_t
 * @param[in]      signal LDO chopper signal PD signal
 */
ATTRIBUTE_INLINE void wm_ll_adc_set_chop_enr(wm_adc_reg_t *hw, int signal)
{
    WM_LL_REG_FILED_COPY_WRTIE(hw->ana_ctrl, chop_enr, signal);
}

/**
 * @brief          Get LDO chopper signal PD signal
 * @param[in]      hw Peripheral ADC hardware instance address
 *                 hw is the type of @arg adc_reg_t
 * @return         LDO chopper signal PD signal
 */
ATTRIBUTE_INLINE int wm_ll_adc_get_chop_enr(wm_adc_reg_t *hw)
{
    return WM_LL_REG_FILED_COPY_READ(hw->ana_ctrl, chop_enr);
}

/**
 * @brief          Set ADC working channel choice signal 
 * @param[in]      hw Peripheral ADC hardware instance address
 *                 hw is the type of @arg adc_reg_t
 * @param[in]      signal ADC working channel choice signal
 *                 b0000: AIN0 channel working. In DMA mode, the corresponding DMA channel 0 works
 *                 b0001: AIN1 channel working. In DMA mode, the corresponding DMA channel 1 works
 *                 b0010: AIN2 channel working. In DMA mode, the corresponding DMA channel 2 works
 *                 b0011: AIN3 channel working. In DMA mode, the corresponding DMA channel 3 works
 *                 b0100: reserved
 *                 b0101: reserved
 *                 b0110: reserved
 *                 b0111: reserved
 *                 b1000: AIN0/AIN1 differential signal input. In DMA mode, the corresponding DMA channel 0 works
 *                 b1001: AIN2/AIN3 differential signal input. In DMA mode, the corresponding DMA channel 2 works
 *                 b1010: reserved
 *                 b1011: reserved
 *                 b1100: Temperature sensor input, corresponding to DMA channel 2
 *                 b1101: Voltage detection module input, corresponding to DMA channel 3
 *                 b1110: offset checks the input
 *                 b1111: reserved        
 */
ATTRIBUTE_INLINE void wm_ll_adc_set_chl_choice(wm_adc_reg_t *hw, int channel)
{
    WM_LL_REG_FILED_COPY_WRTIE(hw->ana_ctrl, chl_choice, channel);
}

/**
 * @brief          Get ADC working channel choice signal 
 * @param[in]      hw Peripheral ADC hardware instance address
 *                 hw is the type of @arg adc_reg_t
 * @return         ADC working channel choice signal
 *                 b0000: AIN0 channel working. In DMA mode, the corresponding DMA channel 0 works
 *                 b0001: AIN1 channel working. In DMA mode, the corresponding DMA channel 1 works
 *                 b0010: AIN2 channel working. In DMA mode, the corresponding DMA channel 2 works
 *                 b0011: AIN3 channel working. In DMA mode, the corresponding DMA channel 3 works
 *                 b0100: reserved
 *                 b0101: reserved
 *                 b0110: reserved
 *                 b0111: reserved
 *                 b1000: AIN0/AIN1 differential signal input. In DMA mode, the corresponding DMA channel 0 works
 *                 b1001: AIN2/AIN3 differential signal input. In DMA mode, the corresponding DMA channel 2 works
 *                 b1010: reserved
 *                 b1011: reserved
 *                 b1100: Temperature sensor input, corresponding to DMA channel 2
 *                 b1101: Voltage detection module input, corresponding to DMA channel 3
 *                 b1110: offset checks the input
 *                 b1111: reserved    
 */
ATTRIBUTE_INLINE int wm_ll_adc_get_chl_choice(wm_adc_reg_t *hw)
{
    return WM_LL_REG_FILED_COPY_READ(hw->ana_ctrl, chl_choice);
}

/**
 * @brief          Clean PGA 
 * @param[in]      hw Peripheral ADC hardware instance address
 *                 hw is the type of @arg adc_reg_t
 */
ATTRIBUTE_INLINE void wm_ll_adc_clr_pga_ctrl(wm_adc_reg_t *hw)
{
    WM_LL_REG_FILED_COPY_WRTIE(hw->pga_ctrl, val, 0);
}

/**
 * @brief          Enable or disable PGA 
 * @param[in]      hw Peripheral ADC hardware instance address
 *                 hw is the type of @arg adc_reg_t
 * @param[in]      en 1: enable, 0: disable
 */
ATTRIBUTE_INLINE void wm_ll_adc_en_pga(wm_adc_reg_t *hw, int en)
{
    WM_LL_REG_FILED_COPY_WRTIE(hw->pga_ctrl, en_pga, en);
}

/**
 * @brief          Get PGA enable status
 * @param[in]      hw Peripheral ADC hardware instance address
 *                 hw is the type of @arg adc_reg_t
 * @return         PGA enable status.
 *                 1: enable, 0: disable
 */
ATTRIBUTE_INLINE int wm_ll_adc_get_en_pga(wm_adc_reg_t *hw)
{
    return WM_LL_REG_FILED_COPY_READ(hw->pga_ctrl, en_pga);
}

/**
 * @brief          Enable or disable PGA chopper
 * @param[in]      hw Peripheral ADC hardware instance address
 *                 hw is the type of @arg adc_reg_t
 * @param[in]      en 1: enable, 0: disable
 */
ATTRIBUTE_INLINE void wm_ll_adc_en_chop(wm_adc_reg_t *hw, int en)
{
    WM_LL_REG_FILED_COPY_WRTIE(hw->pga_ctrl, chop_enp, en);
}

/**
 * @brief          Get PGA chopper enable status
 * @param[in]      hw Peripheral ADC hardware instance address
 *                 hw is the type of @arg adc_reg_t
 * @return         PGA chopper enable status.
 *                 1: enable, 0: disable
 */
ATTRIBUTE_INLINE int wm_ll_adc_get_en_chop(wm_adc_reg_t *hw)
{
    return WM_LL_REG_FILED_COPY_READ(hw->pga_ctrl, chop_enp);
}

/**
 * @brief          Enable or disable Internal reference voltage bypass signal
 * @param[in]      hw Peripheral ADC hardware instance address
 *                 hw is the type of @arg adc_reg_t
 * @param[in]      en 1: Bypass internal reference voltage,
 *                    0: No bypassing
 */
ATTRIBUTE_INLINE void wm_ll_adc_en_bypass_ref(wm_adc_reg_t *hw, int en)
{
    WM_LL_REG_FILED_COPY_WRTIE(hw->pga_ctrl, bypass_ref, en);
}

/**
 * @brief          Get Internal reference voltage bypass signal status
 * @param[in]      hw Peripheral ADC hardware instance address
 *                 hw is the type of @arg adc_reg_t
 * @return         Internal reference voltage bypass signal status.
 *                 1: Bypass internal reference voltage,
 *                 0: No bypassing
 */
ATTRIBUTE_INLINE int wm_ll_adc_get_en_bypass_ref(wm_adc_reg_t *hw)
{
    return WM_LL_REG_FILED_COPY_READ(hw->pga_ctrl, bypass_ref);
}

/**
 * @brief          Enable or disable PGA bypass signal
 * @param[in]      hw Peripheral ADC hardware instance address
 *                 hw is the type of @arg adc_reg_t
 * @param[in]      en 1: Bypass PGA,
 *                    0: No bypassing
 */
ATTRIBUTE_INLINE void wm_ll_adc_en_bypass_pga(wm_adc_reg_t *hw, int en)
{
    WM_LL_REG_FILED_COPY_WRTIE(hw->pga_ctrl, bypass_pga, en);
}

/**
 * @brief          Get PGA bypass signal status
 * @param[in]      hw Peripheral ADC hardware instance address
 *                 hw is the type of @arg adc_reg_t
 * @return         PGA bypass signal status.
 *                 1: Bypass PGA,
 *                 0: No bypassing
 */
ATTRIBUTE_INLINE int wm_ll_adc_get_en_bypass_pga(wm_adc_reg_t *hw)
{
    return WM_LL_REG_FILED_COPY_READ(hw->pga_ctrl, bypass_pga);
}

/**
 * @brief           Set PGA gain
 * @param[in]       hw Peripheral ADC hardware instance address
 *                  hw is the type of @arg adc_reg_t
 * @param[in]       gain PGA gain 5 bit
 *                  BIT[2:0] configures GAIN1
 *                  BIT[4:3] configures GAIN2
 */
ATTRIBUTE_INLINE void wm_ll_adc_set_pga_gain(wm_adc_reg_t *hw, int gain)
{
    WM_LL_REG_FILED_COPY_WRTIE(hw->pga_ctrl, gain_ctrl_pga, gain);
}

/**
 * @brief           Get PGA gain
 * @param[in]       hw Peripheral ADC hardware instance address
 *                  hw is the type of @arg adc_reg_t
 * @return          PGA gain
 *                  BIT[2:0] configures GAIN1
 *                  BIT[4:3] configures GAIN2
 */
ATTRIBUTE_INLINE int wm_ll_adc_get_pga_gain(wm_adc_reg_t *hw)
{
    return WM_LL_REG_FILED_COPY_READ(hw->pga_ctrl, gain_ctrl_pga);
}

/**
 * @brief          Enable or disable Temperature sensor
 * @param[in]      hw Peripheral ADC hardware instance address
 *                 hw is the type of @arg adc_reg_t
 * @param[in]      en 1: enable, 0: disable
 */
ATTRIBUTE_INLINE void wm_ll_adc_en_temp(wm_adc_reg_t *hw, uint8_t en)
{
    WM_LL_REG_FILED_COPY_WRTIE(hw->temp_ctrl, en_temp, en);
}

/**
 * @brief          Get Temperature sensor enable status
 * @param[in]      hw Peripheral ADC hardware instance address
 *                 hw is the type of @arg adc_reg_t
 * @return         Temperature sensor enable status.
 *                 1: enable, 0: disable
 */
ATTRIBUTE_INLINE int wm_ll_adc_get_en_temp(wm_adc_reg_t *hw)
{
    return WM_LL_REG_FILED_COPY_READ(hw->temp_ctrl, en_temp);
}

/**
 * @brief          Set Temperature sensor calibration offset
 * @param[in]      hw Peripheral ADC hardware instance address
 *                 hw is the type of @arg adc_reg_t
 * @param[in]      cal Temperature sensor calibration offset.
 * @note           Set to 0 to collect data once and set to 1 to collect data once
 */
ATTRIBUTE_INLINE void wm_ll_adc_set_cal_offset_temp(wm_adc_reg_t *hw, uint8_t cal)
{
    WM_LL_REG_FILED_COPY_WRTIE(hw->temp_ctrl, cal_offset_temp, cal);
}

/**
 * @brief          Get Temperature sensor calibration offset
 * @param[in]      hw Peripheral ADC hardware instance address
 *                 hw is the type of @arg adc_reg_t
 * @return         Temperature sensor calibration offset.
 * @note           Set to 0 to collect data once and set to 1 to collect data once
 */
ATTRIBUTE_INLINE int wm_ll_adc_get_cal_offset_temp(wm_adc_reg_t *hw)
{
    return WM_LL_REG_FILED_COPY_READ(hw->temp_ctrl, cal_offset_temp);
}

/**
 * @brief          Set Temperature sensor gain
 * @param[in]      hw Peripheral ADC hardware instance address
 *                 hw is the type of @arg adc_reg_t
 * @param[in]      gain Temperature sensor gain
 *                 b00 : 2
 *                 b01 : 4
 *                 b10 : 6
 *                 b11 : 8
 */
ATTRIBUTE_INLINE void wm_ll_adc_set_temp_gain(wm_adc_reg_t *hw, uint8_t gain)
{
    WM_LL_REG_FILED_COPY_WRTIE(hw->temp_ctrl, gain_temp, gain);
}

/**
 * @brief          Get Temperature sensor gain
 * @param[in]      hw Peripheral ADC hardware instance address
 *                 hw is the type of @arg adc_reg_t
 * @return         Temperature sensor gain
 *                 b00 : 2
 *                 b01 : 4
 *                 b10 : 6
 *                 b11 : 8
 */
ATTRIBUTE_INLINE int wm_ll_adc_get_temp_gain(wm_adc_reg_t *hw)
{
    return WM_LL_REG_FILED_COPY_READ(hw->temp_ctrl, gain_temp);
}

/**
 * @brief          Enable or disable DMA
 * @param[in]      hw Peripheral ADC hardware instance address
 *                 hw is the type of @arg adc_reg_t
 * @param[in]      en DMA enable status.
 *                 1: enable, 0: disable
 */
ATTRIBUTE_INLINE void wm_ll_adc_en_dma(wm_adc_reg_t *hw, int en)
{
    WM_LL_REG_FILED_COPY_WRTIE(hw->adc_ctrl, en_dma, en);
}

/**
 * @brief          Get DMA enable status
 * @param[in]      hw Peripheral ADC hardware instance address
 *                 hw is the type of @arg adc_reg_t
 * @return         DMA enable status.
 *                 1: enable, 0: disable
 */
ATTRIBUTE_INLINE int wm_ll_adc_get_en_dma(wm_adc_reg_t *hw)
{
    return WM_LL_REG_FILED_COPY_READ(hw->adc_ctrl, en_dma);
}

/**
 * @brief          Enable or disable ADC interrupt
 * @param[in]      hw Peripheral ADC hardware instance address
 *                 hw is the type of @arg adc_reg_t
 * @param[in]      en ADC interrupt enable status.
 *                 1: enable, 0: disable
 */
ATTRIBUTE_INLINE void wm_ll_adc_en_int(wm_adc_reg_t *hw, int en)
{
    WM_LL_REG_FILED_COPY_WRTIE(hw->adc_ctrl, en_int, en);
}

/**
 * @brief          Get ADC interrupt enable status
 * @param[in]      hw Peripheral ADC hardware instance address
 *                 hw is the type of @arg adc_reg_t
 * @return         ADC interrupt enable status.
 *                 1: enable, 0: disable
 */
ATTRIBUTE_INLINE int wm_ll_adc_get_en_int(wm_adc_reg_t *hw)
{
    return WM_LL_REG_FILED_COPY_READ(hw->adc_ctrl, en_int);
}

/**
 * @brief          Enable or disable ADC comparison
 * @param[in]      hw Peripheral ADC hardware instance address
 *                 hw is the type of @arg adc_reg_t
 * @param[in]      en ADC comparison enable status.
 *                 1: enable, 0: disable
 */
ATTRIBUTE_INLINE void wm_ll_adc_en_cmp(wm_adc_reg_t *hw, int en)
{
    WM_LL_REG_FILED_COPY_WRTIE(hw->adc_ctrl, en_cmp, en);
}

/**
 * @brief          Get ADC comparison enable status
 * @param[in]      hw Peripheral ADC hardware instance address
 *                 hw is the type of @arg adc_reg_t
 * @return         ADC comparison enable status.
 *                 1: enable, 0: disable
 */
ATTRIBUTE_INLINE int wm_ll_adc_get_en_cmp(wm_adc_reg_t *hw)
{
    return hw->adc_ctrl.en_cmp;
}

/**
 * @brief          Enable or disable ADC comparison interrupt
 * @param[in]      hw Peripheral ADC hardware instance address
 *                 hw is the type of @arg adc_reg_t
 * @param[in]      en ADC comparison interrupt enable status.
 *                 1: enable, 0: disable
 */
ATTRIBUTE_INLINE void wm_ll_adc_en_cmp_int(wm_adc_reg_t *hw, int en)
{
    WM_LL_REG_FILED_COPY_WRTIE(hw->adc_ctrl, en_cmp_int, en);
}

/**
 * @brief          Get ADC comparison interrupt enable status
 * @param[in]      hw Peripheral ADC hardware instance address
 *                 hw is the type of @arg adc_reg_t
 * @return         ADC comparison interrupt enable status.
 *                 1: enable, 0: disable
 */
ATTRIBUTE_INLINE int wm_ll_adc_get_en_cmp_int(wm_adc_reg_t *hw)
{
    return WM_LL_REG_FILED_COPY_READ(hw->adc_ctrl, en_cmp_int);
}

/**
 * @brief          Set ADC comparison polarity
 * @param[in]      hw Peripheral ADC hardware instance address
 *                 hw is the type of @arg adc_reg_t
 * @param[in]      pol ADC comparison polarity.
 *                 1: an interrupt is generated when adc_result < cmp_value
 *                 0: an interrupt is generated when adc_result >= cmp_value
 */
ATTRIBUTE_INLINE void wm_ll_adc_set_cmp_pol(wm_adc_reg_t *hw, int pol)
{
    WM_LL_REG_FILED_COPY_WRTIE(hw->adc_ctrl, cmp_pol, pol);
}

/**
 * @brief          Get ADC comparison polarity
 * @param[in]      hw Peripheral ADC hardware instance address
 *                 hw is the type of @arg adc_reg_t
 * @return         ADC comparison polarity.
 *                 1: an interrupt is generated when adc_result < cmp_value
 *                 0: an interrupt is generated when adc_result >= cmp_value
 */
ATTRIBUTE_INLINE int wm_ll_adc_get_cmp_pol(wm_adc_reg_t *hw)
{
    return WM_LL_REG_FILED_COPY_READ(hw->adc_ctrl, cmp_pol);
}

/**
 * @brief          Set the time required for the analog circuit to remain 
 *                 stable after the software starts adc_start
 * @param[in]      hw Peripheral ADC hardware instance address
 *                 hw is the type of @arg adc_reg_t
 * @param[in]      time The time required for the analog circuit to remain 
 *                 stable after the software starts adc_start
 *                 the default is 80 pclk, (2us)
 */
ATTRIBUTE_INLINE void wm_ll_adc_set_ana_init_time(wm_adc_reg_t *hw, int time)
{
    WM_LL_REG_FILED_COPY_WRTIE(hw->adc_ctrl, ana_init_time, time);
}

/**
 * @brief          Get the time required for the analog circuit to remain 
 *                 stable after the software starts adc_start
 * @param[in]      hw Peripheral ADC hardware instance address
 *                 hw is the type of @arg adc_reg_t
 * @return         The time required for the analog circuit to remain 
 *                 stable after the software starts adc_start
 */
ATTRIBUTE_INLINE int wm_ll_adc_get_ana_init_time(wm_adc_reg_t *hw)
{
    return WM_LL_REG_FILED_COPY_READ(hw->adc_ctrl, ana_init_time);
}

/**
 * @brief          Set The time required for the analog circuit to remain 
 *                 stable after the software switches the data channel
 * @param[in]      hw Peripheral ADC hardware instance address
 *                 hw is the type of @arg adc_reg_t
 * @param[in]      time The time required for the analog circuit to remain 
 *                 stable after the software switches the data channel
 *                 the default is 80 pclk, (2us)
 */
ATTRIBUTE_INLINE void wm_ll_adc_set_ana_swth_time(wm_adc_reg_t *hw, int time)
{
    WM_LL_REG_FILED_COPY_WRTIE(hw->adc_ctrl, ana_swth_time, time);
}

/**
 * @brief          Get The time required for the analog circuit to remain 
 *                 stable after the software switches the data channel
 * @param[in]      hw Peripheral ADC hardware instance address
 *                 hw is the type of @arg adc_reg_t
 * @return         The time required for the analog circuit to remain 
 *                 stable after the software switches the data channel
 */
ATTRIBUTE_INLINE int wm_ll_adc_get_ana_swth_time(wm_adc_reg_t *hw)
{
    return WM_LL_REG_FILED_COPY_READ(hw->adc_ctrl, ana_swth_time);
}

/**
 * @brief          Get ADC all interrupt status
 * @param[in]      hw Peripheral ADC hardware instance address
 *                 hw is the type of @arg adc_reg_t
 */
ATTRIBUTE_INLINE int wm_ll_adc_get_adc_int_val(wm_adc_reg_t *hw)
{
    return WM_LL_REG_FILED_COPY_READ(hw->int_status, val);
}

/**
 * @brief          Set ADC interrupt status
 * @param[in]      hw Peripheral ADC hardware instance address
 *                 hw is the type of @arg adc_reg_t
 * @param[in]      status  The data conversion is interrupted, 
 *                 the hardware is set, and the software writes 1 to reset
 */
ATTRIBUTE_INLINE void wm_ll_adc_set_adc_int(wm_adc_reg_t *hw, int status)
{
    WM_LL_REG_FILED_COPY_WRTIE(hw->int_status, adc_int, status);
}

/**
 * @brief          Get ADC interrupt status
 * @param[in]      hw Peripheral ADC hardware instance address
 *                 hw is the type of @arg adc_reg_t
 * @return         The data conversion is interrupted, 
 *                 the hardware is set, and the software writes 1 to reset
 */
ATTRIBUTE_INLINE int wm_ll_adc_get_adc_int(wm_adc_reg_t *hw)
{
    return WM_LL_REG_FILED_COPY_READ(hw->int_status, adc_int);
}

/**
 * @brief          Set ADC Compare interrupt flags
 * @param[in]      hw Peripheral ADC hardware instance address
 *                 hw is the type of @arg adc_reg_t
 * @param[in]      status  Compare interrupt flags, 
 *                 the hardware is set, and the software writes 1 to reset
 */
ATTRIBUTE_INLINE void wm_ll_adc_set_cmp_int(wm_adc_reg_t *hw, int flags)
{
    WM_LL_REG_FILED_COPY_WRTIE(hw->int_status, cmp_int, flags);
}

/**
 * @brief          Get ADC Compare interrupt flags
 * @param[in]      hw Peripheral ADC hardware instance address
 *                 hw is the type of @arg adc_reg_t
 * @return         Compare interrupt flags, 
 *                 the hardware is set, and the software writes 1 to reset
 */
ATTRIBUTE_INLINE int wm_ll_adc_get_cmp_int(wm_adc_reg_t *hw)
{
    return WM_LL_REG_FILED_COPY_READ(hw->int_status, cmp_int);
}

/**
 * @brief          Set adc compare value 
 * @param[in]      hw Peripheral ADC hardware instance address
 *                 hw is the type of @arg adc_reg_t
 * @param[in]      val adc compare value.
 */
ATTRIBUTE_INLINE void wm_ll_adc_set_cmp_value(wm_adc_reg_t *hw, int val)
{
    WM_LL_REG_FILED_COPY_WRTIE(hw->cmp_value, cmp_value, val);
}

/**
 * @brief          Get adc compare value 
 * @param[in]      hw Peripheral ADC hardware instance address
 *                 hw is the type of @arg adc_reg_t
 * @return         adc compare value.
 */
ATTRIBUTE_INLINE int wm_ll_adc_get_cmp_value(wm_adc_reg_t *hw)
{
    return WM_LL_REG_FILED_COPY_READ(hw->cmp_value, cmp_value);
}

#ifdef __cplusplus
}
#endif

#endif /* __WM_LL_ADC_H__ */
