/**
 * @file wm_rtc_reg_struct.h
 *
 * @brief register rtc Module
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

#ifndef __WM_RTC_REG_STRUCT_H__
#define __WM_RTC_REG_STRUCT_H__

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief rtc control1 register layout
 * This structure is used to control rtc
 *
 * @attention When setting alarm, RTC register needs to be written by word(HW limitation)
 *
 */
typedef union {
    struct {
        uint32_t second      : 6; /* [5:0]   Second initial value/second fixed value */
        uint32_t reserved_6  : 2; /* [7:6]   Reserved */
        uint32_t minute      : 6; /* [13:8]  Minute initial value/minute fixed value */
        uint32_t reserved_14 : 2; /* [15:14] Reserved */
        uint32_t hour        : 5; /* [20:16] Hourly initial value/hourly fixed value */
        uint32_t reserved_21 : 3; /* [23:21] Reserved */
        uint32_t day         : 5; /* [28:24] Daily initial value/daily fixed value */
        uint32_t reserved_29 : 2; /* [30:29] Reserved */
        uint32_t enable      : 1; /* [31]    RTC timing interrupt enable bit; 1 for enable interrupt, 0 for disable interrupt */
    };
    uint32_t val;
} wm_rtc_reg1_t;

/**
 * @brief rtc control2 register layout
 * This structure is used to control rtc
 */
typedef union {
    struct {
        uint32_t month       : 4;  /* [3:0]   Month initial value/month fixed value */
        uint32_t reserved_4  : 4;  /* [7:4]   Reserved */
        uint32_t year        : 7;  /* [14:8]  Year initial value/year fixed value */
        uint32_t reserved_15 : 1;  /* [15]    Reserved */
        uint32_t enable      : 1;  /* [16]    RTC timer counting enable bit ; 1 for enable, 0 for disable */
        uint32_t reserved_17 : 15; /* [31:17] Reserved */
    };
    uint32_t val;
} wm_rtc_reg2_t;

/**
 * @brief rtc register layout
 * must 32bit for each member
 * @note WM_RTC_BASE_ADDR = WM_PMU_BASE_ADDR + 0x0C
 */
typedef struct {
    volatile wm_rtc_reg1_t rtc_r1; /* WM_RTC_BASE_ADDR + 0x00 */
    volatile wm_rtc_reg2_t rtc_r2; /* WM_RTC_BASE_ADDR + 0x04 */
} wm_rtc_reg_t;

#ifdef __cplusplus
}
#endif

#endif /* __WM_RTC_REG_STRUCT_H__ */
