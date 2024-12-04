/**
 * @file wm_adc_reg_struct.h
 *
 * @brief adc registers struct Module
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

#ifndef __WM_ADC_REG_STRUCT_H__
#define __WM_ADC_REG_STRUCT_H__

#include "wm_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Store the ADC acquisition value and data comparison value.
 * This structure is used to control adc result value
 */
typedef union {
    struct {
        uint32_t result_val  : 18; /* [17:0]  The ADC converts the resulting value with an effective bit width of 18bits. 
                                                       Signed numbers with the most significant bit being the sign bit */
        uint32_t reserved_18 : 14; /* [31:18] Reserved read only */
    };
    uint32_t val;
} wm_adc_result_t;

/**
 * @brief Configure ADC related functions
 * This structure is used to config adc related functions
 */
typedef union {
    struct {
        uint32_t en_ldo_sdadc : 1; /* [0]     The ADC LDO enable ; bit 1 : enable , bit 0 : disable */
        uint32_t rstn_sdadc : 1; /* [1]     Analog module digital logic part reset signal; bit 1 : normal work , bit 0 : reset */
        uint32_t
            en_pd_sdadc : 1; /* [2]     The sdadc analog module is powered down and enabled; bit 1 : power_off , bit 0 : normal work */
        uint32_t reserved_3  : 1;  /* [3]     Reserved read only */
        uint32_t chop_ens    : 1;  /* [4]     sdadc chopper signal PD signal; bit 1 : disable , bit 0 : enable */
        uint32_t chop_enr    : 2;  /* [6:5]   LDO chopper signal PD signal */
        uint32_t reserved_7  : 1;  /* [7]     Reserved read only */
        uint32_t chl_choice  : 4;  /* [11:8]  ADC working channel choice signal 
                                                        b0000: AIN0 channel working. In DMA mode, the corresponding DMA channel 0 works
                                                        b0001: AIN1 channel working. In DMA mode, the corresponding DMA channel 1 works
                                                        b0010: AIN2 channel working. In DMA mode, the corresponding DMA channel 2 works
                                                        b0011: AIN3 channel working. In DMA mode, the corresponding DMA channel 3 works
                                                        b0100: reserved
                                                        b0101: reserved
                                                        b0110: reserved
                                                        b0111: reserved
                                                        b1000: AIN0/AIN1 differential signal input. In DMA mode, the corresponding DMA channel 0 works
                                                        b1001: AIN2/AIN3 differential signal input. In DMA mode, the corresponding DMA channel 2 works
                                                        b1010: reserved
                                                        b1011: reserved
                                                        b1100: Temperature sensor input, corresponding to DMA channel 2
                                                        b1101: Voltage detection module input, corresponding to DMA channel 3
                                                        b1110: offset checks the input
                                                        b1111: reserved */
        uint32_t reserved_12 : 20; /* [31:12] Reserved read only */
    };
    uint32_t val;
} wm_adc_ana_ctrl_t;

/**
 * @brief Configure PGA related functions
 * This structure is used to config PGA related functions
 */
typedef union {
    struct {
        uint32_t en_pga        : 1;  /* [0]    The PGA enable signal; bit 1 : enable , bit 0 : disable */
        uint32_t chop_enp      : 1;  /* [1]    The PGA chopper enable signal; bit 1 : enable , bit 0 : disable */
        uint32_t bypass_ref    : 1;  /* [2]    Internal reference voltage bypass signal; 
                                                          bit 1 : Bypass internal reference voltage , 
                                                          bit 0 : No bypassing */
        uint32_t bypass_pga    : 1;  /* [3]    PGA bypass signal ; bit 1 : Bypass PGA , bit 0 : No bypassing */
        uint32_t gain_ctrl_pga : 5;  /* [8:4]  PGA gain configuration ; BIT[8:7] configures GAIN2, BIT[6:4] configures GAIN1 */
        uint32_t reserved_9    : 23; /* [31:9] Reserved read only */
    };
    uint32_t val;
} wm_adc_pga_ctrl_t;

/**
 * @brief Configure temperature sensor related functions
 * This structure is used to config temperature sensor related functions
 */
typedef union {
    struct {
        uint32_t en_temp         : 1;  /* [0]    The TEMP enable signal; bit 1 : enable , bit 0 : disable */
        uint32_t cal_offset_temp : 1;  /* [1]    Temperature sensor offset calibration function
                                                          Set to 0 to collect data once and set to 1 to collect data once */
        uint32_t reserved_2      : 2;  /* [3:2]  Reserved read only */
        uint32_t gain_temp       : 2;  /* [5:4]  temp gain control ; 
                                                          b00 : 2
                                                          b01 : 4
                                                          b10 : 6
                                                          b11 : 8 */
        uint32_t reserved_6      : 26; /* [31:6] Reserved read only */
    };
    uint32_t val;
} wm_adc_temp_ctrl_t;

/**
 * @brief Configure the ADC module function
 * This structure is used to config ADC module functions
 */
typedef union {
    struct {
        uint32_t en_dma     : 1; /* [0]     The DMA enable signal; bit 1 : enable , bit 0 : disable */
        uint32_t en_int     : 1; /* [1]     ADC data conversion interrupt enable signal ; bit 1 : enable , bit 0 : disable */
        uint32_t reserved_2 : 2; /* [3:2]   Reserved read only */
        uint32_t en_cmp     : 1; /* [4]     ADC comparison function enable signal ; bit 1 : enable , bit 0 : disable */
        uint32_t en_cmp_int : 1; /* [5]     ADC comparison interrupt enable signal ; bit 1 : enable , bit 0 : disable */
        uint32_t cmp_pol    : 1; /* [6]     b0: an interrupt is generated when adc_result >= cmp_value
                                                           b1: an interrupt is generated when adc_result < cmp_value */
        uint32_t reserved_7 : 1; /* [7]     Reserved read only */
        uint32_t
            ana_init_time : 10; /* [17:8]  After the software starts adc_start, the time required for the analog circuit to remain stable
                                                           the default is 80 pclk, (2us) */
        uint32_t reserved_18 : 2; /* [19:18] Reserved read only */
        uint32_t
            ana_swth_time : 10; /* [29:20] The time required for the analog circuit to remain stable after the software switches the data channel
                                                           the default is 80 pclk, (2us) */
        uint32_t reserved_30 : 2; /* [31:30] Reserved read only */
    };
    uint32_t val;
} wm_adc_ctrl_t;

/**
 * @brief The interrupt status register of the ADC module
 * This structure is used to control adc interrupt status
 */
typedef union {
    struct {
        uint32_t
            adc_int : 1; /* [0]    The data conversion is interrupted, the hardware is set, and the software writes 1 to reset */
        uint32_t cmp_int    : 1;  /* [1]    Compare interrupt flags, the hardware is set, and the software writes 1 to reset */
        uint32_t reserved_2 : 30; /* [31:2] Reserved read only */
    };
    uint32_t val;
} wm_adc_int_status_t;

/**
 * @brief Compare threshold Settings
 * This structure is used to settings compare threshold 
 */
typedef union {
    struct {
        uint32_t cmp_value   : 18; /* [17:0]  The value to compare */
        uint32_t reserved_18 : 14; /* [31:18] Reserved read only */
    };
    uint32_t val;
} wm_adc_cmp_value_t;

/**
 * @brief adc register layout
 * must 32bit for each member
 * @note WM_SD_ADC_BASE_ADDR = WM_APB_BASE_ADDR + 0x200
 */
typedef struct {
    volatile wm_adc_result_t result;         /* WM_SD_ADC_BASE_ADDR + 0x0000 */
    volatile wm_adc_ana_ctrl_t ana_ctrl;     /* WM_SD_ADC_BASE_ADDR + 0x0004 */
    volatile wm_adc_pga_ctrl_t pga_ctrl;     /* WM_SD_ADC_BASE_ADDR + 0x0008 */
    volatile wm_adc_temp_ctrl_t temp_ctrl;   /* WM_SD_ADC_BASE_ADDR + 0x000c */
    volatile wm_adc_ctrl_t adc_ctrl;         /* WM_SD_ADC_BASE_ADDR + 0x0010 */
    volatile wm_adc_int_status_t int_status; /* WM_SD_ADC_BASE_ADDR + 0x0014 */
    volatile wm_adc_cmp_value_t cmp_value;   /* WM_SD_ADC_BASE_ADDR + 0x0018 */
} wm_adc_reg_t;

#ifdef __cplusplus
}
#endif

#endif /* __WM_ADC_REG_STRUCT_H__ */
