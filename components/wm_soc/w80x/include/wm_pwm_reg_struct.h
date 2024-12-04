/**
 * @file wm_pwm_reg_struct.h
 *
 * @brief PWM Module
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

#ifndef __WM_PWM_REG_STRUCT_H__
#define __WM_PWM_REG_STRUCT_H__

#include "wm_types.h"
#include "wm_reg_op.h"
#include "wm_soc_cfgs.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief clock divider register for ch0 and ch1 of PWM
 * This union is used to describe PWM registers.
 */
typedef union {
    struct {
        uint32_t clkdiv0 : 16; /**< [15:0] RW - Clock Divider 0 - 16'h0
                                *   CH0 divider counter
                                *   The division ratio is determined by the counter value
                                *   Note: The division range is (0-65535), input 0 or 1 if no division is required. */
        uint32_t clkdiv1 : 16; /**< [31:16] RW - Clock Divider 1 - 16'h0
                                *   CH1 divider counter
                                *   Same as CH0 */
    };
    uint32_t val; /**< Value representing the structure */
} wm_pwm_clkdiv01_t;

/**
 * @brief clock divider register for ch2 and ch3 of PWM
 * This union is used to describe PWM registers.
 */
typedef union {
    struct {
        uint32_t clkdiv2 : 16; /**< [15:0] RW - Clock Divider 2 - 16'h0
                                *   CH2 divider counter
                                *   Same as CH0 */
        uint32_t clkdiv3 : 16; /**< [31:16] RW - Clock Divider 3 - 16'h0
                                *   CH3 divider counter
                                *   Same as CH0 */
    };
    uint32_t val; /**< Value representing the structure */
} wm_pwm_clkdiv23_t;

/**
 * @brief main ctrl register for PWM controller
 * This union is used to describe PWM registers.
 */
typedef union {
    struct {
        uint32_t outmode     : 2; /**< [1:0] RW - Output Mode - 2'b0
                                     *   Output mode
                                     *   1'b0: Non-complementary mode for every two channels
                                     *   1'b1: Complementary mode for every two channels
                                     *   BIT1 controls CH2 and CH3
                                     *   BIT0 controls CH0 and CH1 */
        uint32_t pinv        : 4; /**< [5:2] RW - PWM Output Polarity Enable - 4'h0
                                     *   PWM output signal polarity enable
                                     *   1'b0: PWM output polarity inversion disable
                                     *   1'b1: PWM output polarity inversion enable */
        uint32_t allsyncen   : 1; /**< [6] RW - All Channel Sync Mode Enable - 1'b0
                                     *   All-channel synchronous mode enable signal
                                     *   1'b0: Do not allow all-channel synchronization
                                     *   1'b1: Allow all-channel synchronization */
        uint32_t reserved_7  : 1; /**< [7] Reserved - 1'b0 */
        uint32_t cntmode     : 4; /**< [11:8] RW - PWM Generation Mode - 4'h0
                                     *   PWM generation loop mode
                                     *   1'b0: Single mode
                                     *   1'b1: Auto-load mode
                                     *   Each bit controls each channel, from high to low to control PW3, PW2, PW1, and PW0 */
        uint32_t poen        : 1; /**< [12] RW - PWM Output Enable - 1'b0
                                     *   PWM pin output enable bit
                                     *   1'b0: PWM pin set to output state
                                     *   1'b1: PWM pin set to high-impedance state
                                     *   Note: Only for CH0 */
        uint32_t reserved_13 : 1; /**< [13] Reserved - 1'b0 */
        uint32_t twosyncen   : 2; /**< [15:14] RW - Two-Channel Sync Mode Enable - 2'b0
                                     *   Two-channel synchronous mode enable signal
                                     *   1'b0: Do not allow two-channel synchronization
                                     *   1'b1: Allow two-channel synchronization */
        uint32_t cnttype0    : 2; /**< [17:16] RW - Counter Type 0 - 2'b0
                                     *   CH0 counter counting mode
                                     *   2'b00: Edge-aligned mode (counter counting mode is increment, only for capture mode)
                                     *   2'b01: Edge-aligned mode (counter counting mode is decrement, only for PWM mode)
                                     *   2'b10: Center-aligned mode (only for PWM mode) */
        uint32_t cnttype1    : 2; /**< [19:18] RW - Counter Type 1 - 2'b0
                                     *   CH1 counter counting mode
                                     *   Same as CH0 */
        uint32_t cnttype2    : 2; /**< [21:20] RW - Counter Type 2 - 2'b0
                                     *   CH2 counter counting mode
                                     *   Same as CH0 */
        uint32_t cnttype3    : 2; /**< [23:22] RW - Counter Type 3 - 2'b0
                                     *   CH3 counter counting mode
                                     *   Same as CH0 */
        uint32_t cpen        : 1; /**< [24] RW - Capture Enable - 1'b0
                                     *   Capture function enable flag
                                     *   1'b0: CH0 capture function invalid, RCAPDAT and FCAPDAT values will not be updated
                                     *   1'b1: CH0 capture function valid, capture and latch PWM counter, stored respectively
                                     *     in RCAPDAT (rising edge latch) and FCAPDAT (falling edge latch) */
        uint32_t capinv      : 1; /**< [25] RW - Capture Inverse Enable - 1'b0
                                     *   Capture reverse enable flag
                                     *   1'b0: Capture mode input signal reverse invalid
                                     *   1'b1: Capture mode input signal reverse valid, the input signal is inverted */
        uint32_t reserved_26 : 1; /**< [26] Reserved - 1'b0 */
        uint32_t cnten       : 5; /**< [31:27] RW - Counter Enable - 5'b0
                                     *   Counter counting enable
                                     *   1'b0: Stop counting
                                     *   1'b1: Start counting
                                     *   Each bit controls each channel, from high to low to control CH4, CH3, CH2, CH1, and CH0 */
    };
    uint32_t val; /**< Value representing the structure */
} wm_pwm_ctrl_t;

/**
 * @brief duty register for ch0 ~ ch3 of PWM
 * This union is used to describe PWM registers.
 */
typedef union {
    struct {
        uint32_t period0 : 8; /**< [7:0] RW - Period 0 - 8'h0
                               *   CH0 period register value
                               *   (Note: period cannot exceed 255)
                               *
                               *   "Edge-aligned mode (counter counting mode is decrement)":
                               *   PERIOD register value, period value is (PERIOD+1)
                               *   Duty cycle = (CMP+1)/(PERIOD+1)
                               *   CMP >= PERIOD: PWM output is fixed high
                               *   CMP < PERIOD: PWM low level width is (PERIOD-CMP), high level width is (CMP+1)
                               *   CMP = 0: PWM low level width is PERIOD, high level width is 1
                               *
                               *   "Center-aligned mode":
                               *   PERIOD register value: period is 2*(PERIOD+1)
                               *   Duty cycle = (2CMP+1)/(2(PERIOD+1))
                               *   CMP > PERIOD: PWM remains high
                               *   CMP <= PERIOD: PWM low level = 2*(PERIOD-CMP)+1, high level = (2CMP)+1
                               *   CMP = 0: PWM low level width is 2 PERIOD+1, high level width is 1
                               *
                               *   Note: In "Center-aligned mode", the number of periods should not be 255.
                               *   Regardless of the alignment mode selected, the channel period is determined
                               *    jointly by the division number (N) and the number of periods (P),
                               *    that is: the input clock is 40MHz, and the clock frequency after division
                               *    is f_div: f_div=40MHz/N, where N is the division number (16bit).
                               *    The output frequency f_output is: f_output=f_div/P, where P is the number of periods.
                               *   Note: In PWM mode, when the counter is set to edge-aligned mode,
                               *    the counting mode needs to be set to decrement mode.
                               * */
        uint32_t period1 : 8; /**< [15:8] RW - Period 1 - 8'h0
                               *   CH1 period register value
                               *   Same as PERIOD0 */
        uint32_t period2 : 8; /**< [23:16] RW - Period 2 - 8'h0
                               *   CH2 period register value
                               *   Same as PERIOD0 */
        uint32_t period3 : 8; /**< [31:24] RW - Period 3 - 8'h0
                               *   CH3 period register value
                               *   Same as PERIOD0 */
    };
    uint32_t val; /**< Value representing the structure */
} wm_pwm_period_t;

/**
 * @brief period number register for ch0 ~ ch3 of PWM
 * This union is used to describe PWM registers.
 */
typedef union {
    struct {
        uint32_t pnum0 : 8; /**< [7:0] RW - PWM 0 Period Number - 8'h0
                             *   PWM0 generation period number
                             *   Set the PWM3 period number PNUM3, when PNUM3 PWM signals are generated,
                             *       stop generating signals, and trigger interrupts and set interrupt status words */
        uint32_t pnum1 : 8; /**< [15:8] RW - PWM 1 Period Number - 8'h0
                             *   PWM1 generation period number
                             *   Same as PNUM0 */
        uint32_t pnum2 : 8; /**< [23:16] RW - PWM 2 Period Number - 8'h0
                             *   PWM2 generation period number
                             *   Same as PNUM0 */
        uint32_t pnum3 : 8; /**< [31:24] RW - PWM 3 Period Number - 8'h0
                             *   PWM3 generation period number
                             *   Same as PNUM0 */
    };
    uint32_t val; /**< Value representing the structure */
} wm_pwm_period_num_t;

/**
 * @brief compare register for ch0 ~ ch3 of PWM
 * This union is used to describe PWM registers.
 */
typedef union {
    struct {
        uint32_t cmp0 : 8; /**< [7:0] RW - Compare 0 - 8'h0
                           *   P0 comparison register value
                           *
                           *   "Edge-aligned mode (counter counting mode is decrement)":
                           *   PERIOD register value, period value is (PERIOD+1)
                           *   Duty cycle = (CP+1)/(PERIOD+1)
                           *   CP >= PERIOD: PWM output is fixed high
                           *   CP < PERIOD: PWM low level width is (PERIOD--CMP), high level width is (CMP+1)
                           *   CP = 0: PWM low level width is PERIOD, high level width is 1
                           *
                           *   Edge-aligned mode (counter counting mode is increment)":
                           *   Not Application, just for capture mode
                           *
                           *   "Center-aligned mode":
                           *   PERIOD register value: period is 2*(PERIOD+1)
                           *   Duty cycle = (2CP+1)/2(PERIOD+1)
                           *   CP > PERIOD: PWM remains high
                           *   CP <= PERI0D: PWM low level = 2*(PERI0D-CP)+1, high level = (2*CMP)+1
                           *   CP = 0: PWM low level width is 2 PERI0D+1, high level width is 1.
                           *   Regardless of the alignment mode selected, the channel period is determined
                           *    jointly by the division number (N) and the number of periods (P),
                           *    that is: the input clock is 40MHz, and the clock frequency after division is
                           *    f_div: f_div=40Hz/N, where N is the division number (16bit).
                           *    The output frequency t_output is: t_output=f_div/P, where P is the period number.
                           *
                           *   Note: In PW mode, when the counter is set to edge-aligned mode,
                           *    the counting mode needs to be set to decrement mode. */
        uint32_t cmp1 : 8; /**< [15:8] RW - Compare 1 - 8'h0
                           *   P1 comparison register value
                           *   Same as CMP0 */
        uint32_t cmp2 : 8; /**< [23:16] RW - Compare 2 - 8'h0
                           *   P2 comparison register value
                           *   Same as CMP0 */
        uint32_t cmp3 : 8; /**< [31:24] RW - Compare 3 - 8'h0
                           *   P3 comparison register value
                           *   Same as CMP0 */
    };
    uint32_t val; /**< Value representing the structure */
} wm_pwm_compare_t;

/**
 * @brief dead time register for ch0 ~ ch3 of PWM
 * This union is used to describe PWM registers.
 */
typedef union {
    struct {
        uint32_t dtcnt01     : 8;  /**< [7:0] RW - Dead Time Count 01 - 8'h0
                                          *   Dead time interval between channels 0 and 1
                                          *   8 bits determine the dead time interval value, and the dead time clock is determined by DTDIV */
        uint32_t dtcnt23     : 8;  /**< [15:8] RW - Dead Time Count 23 - 8'h0
                                          *   Dead time interval between channels 3 and 2
                                          *   8 bits determine the dead time interval value, and the dead time clock is determined by DTDIV */
        uint32_t dtdiv       : 2;  /**< [17:16] RW - Dead Time Divider - 2'b0
                                          *   Dead time clock divider control
                                          *   2'b00: Dead time clock equals base clock (40MHz)
                                          *   2'b01: Dead time clock equals base clock (40MHz) divided by two
                                          *   2'b10: Dead time clock equals base clock (40MHz) divided by four
                                          *   2'b11: Dead time clock equals base clock (40MHz) divided by eight */
        uint32_t reserved_18 : 2;  /**< [19:18] Reserved - 2'b0 */
        uint32_t dten01      : 1;  /**< [20] RW - Dead Time Enable 01 - 1'b0
                                          *   Whether channels 0 and 1 can insert dead time valid identification
                                          *   The dead time valid signal is only valid after the
                                          *      channel's complementary mode is turned on. And if the insertion valid signal is 0,
                                          *      there is no dead time insertion for the complementary signals output by the two channels
                                          *   1'b0: Dead time insertion invalid
                                          *   1'b1: Dead time insertion valid */
        uint32_t dten23      : 1;  /**< [21] RW - Dead Time Enable 23 - 1'b0
                                          *   Whether channels 2 and 3 can insert dead time valid identification
                                          *   Same as DTEN01 */
        uint32_t reserved_22 : 10; /**< [31:22] Reserved - 10'h0 */
    }; /**< Structure packing attribute */
    uint32_t val; /**< Value representing the structure */
} wm_pwm_dead_time_t;

/**
 * @brief pwm interrupt control structure
 * This union is used to describe PWM registers.
 */
typedef union {
    struct {
        uint32_t pien           : 5;  /**< [4:0] RW - Period Interrupt Enable - 5'b0
                                      *   PN period interrupt enable bit
                                      *   1'b0: Period interrupt invalid
                                      *   1'b1: Period interrupt valid
                                      *   Note: When the counter counts to 0 and the number of PWM periods meets PWM_PNUM, the interrupt is triggered. */
        uint32_t rlien          : 1;  /**< [5] RW - Rising Edge Buffer Interrupt Enable - 1'b0
                                      *   Rising edge buffer interrupt enable bit
                                      *   1'b0: Rising buffer interrupt invalid
                                      *   1'b1: Rising edge buffer interrupt valid
                                      *   Note: For CH0 */
        uint32_t flien          : 1;  /**< [6] RW - Falling Edge Buffer Interrupt Enable - 1'b0
                                      *   Falling edge buffer interrupt enable bit
                                      *   1'b0: Falling edge buffer interrupt invalid
                                      *   1'b1: Falling edge buffer interrupt valid
                                      *   Note: For CH0 */
        uint32_t dma_request_en : 1;  /**< [7] RW - DMA Request Enable - 1'b0
                                      *   DMA request enable
                                      *   1'b0: DMA request invalid
                                      *   1'b1: DMA request valid */
        uint32_t reserved_8     : 24; /**< [31:8] Reserved - 24'h0 */
    };
    uint32_t val; /**< Value representing the structure */
} wm_pwm_int_ctrl_t;

/**
 * @brief pwm interrupt status structure
 * This union is used to describe PWM registers.
 */
typedef union {
    struct {
        uint32_t pif         : 5;  /**< [4:0] RW - Period Interrupt Flag - 5'b0
                                    *   PWM period interrupt flag
                                    *   When PM generates a specified period PWM signal, this flag is set to 1
                                    *   Writing 1 clears this flag
                                    *   Each bit controls each channel, from high to low, respectively controlling PT4, PT3, PT2, PT1, and PT0 */
        uint32_t crlif       : 1;  /**< [5] RW - Capture Rising Interrupt - 1'b0
                                    *   Capture rising edge interrupt flag
                                    *   1'b0: No rising edge captured
                                    *   1'b1: Rising edge captured
                                    *   Note: Writing 1 clears this flag;
                                    *   Note: Only for CH0 */
        uint32_t cflir       : 1;  /**< [6] RW - Capture Falling Interrupt - 1'b0
                                    *   Capture falling edge interrupt flag
                                    *   1'b0: No falling edge captured
                                    *   1'b1: Falling edge captured
                                    *   Note: Writing 1 clears this flag;
                                    *   Note: Only for CH0 */
        uint32_t rlifov      : 1;  /**< [7] RW - Rising Lifo Overflow - 1'b0
                                    *   Rising edge delay interrupt flag
                                    *   1'b0: No rising edge delay interrupt occurred
                                    *   1'b1: Rising edge delay interrupt occurred
                                    *   Note: When the user clears CRIF, this bit is also cleared */
        uint32_t flifov      : 1;  /**< [8] RW - Falling Lifo Overflow - 1'b0
                                    *   Falling edge delay interrupt flag
                                    *   1'b0: No falling edge delay interrupt occurred
                                    *   1'b1: Falling edge delay interrupt occurred
                                    *   Note: When the user clears CFIF, this bit is also cleared */
        uint32_t overfl      : 1;  /**< [9] RW - Overflow - 1'b0
                                    *   Counter overflow flag
                                    *   1'b0: Counter not overflow
                                    *   1'b1: Counter overflow
                                    *   Note: When the user clears CIF or CLIF, this bit is also cleared */
        uint32_t reserved_10 : 22; /**< [31:10] Reserved bits - 12'h0 */
    };
    uint32_t val; /**< Value representing the structure */
} wm_pwm_int_status_t;

/**
 * @brief capture data from ch0 of PWM
 * This union is used to describe PWM registers.
 */
typedef union {
    struct {
        uint32_t pwm_rcapdat : 16; /**< [15:0] RO - PWM Rising Edge Capture Data - 16'h0
                                    *   Capture Rising Edge Register
                                    *   Stores the current counter value when there is a rising edge in the input signal */
        uint32_t pwm_fcapdat : 16; /**< [31:16] RO - PWM Falling Edge Capture Data - 16'h0
                                    *   Capture Falling Edge Register
                                    *   Stores the current counter value when there is a falling edge in the input signal */
    };
    uint32_t val; /**< Value representing the structure */
} wm_pwm_ch0_capture_t;

/**
 * @brief register to control brake mode of PWM
 * This union is used to describe PWM registers.
 */
typedef union {
    struct {
        uint32_t reserved_0  : 3;  /**< [2:0] Reserved - 3'b0 */
        uint32_t bkod        : 5;  /**< [7:3] RW - Brake Output Control Register - 5'b0
                                     *   Brake output control register
                                     *   1'b0: PWM output low level when brake mode is valid
                                     *   1'b1: PWM output high level when brake mode is valid
                                     *   [7:3] map to CH0, CH1, CH2, CH3, CH4 */
        uint32_t reserved_8  : 3;  /**< [10:8] Reserved - 3'b0 */
        uint32_t brkctl      : 5;  /**< [15:11] RW - Brake Control - 5'b0
                                     *   Brake mode enable
                                     *   1'b0: Brake mode disabled
                                     *   1'b1: Brake mode enabled */
        uint32_t reserved_16 : 16; /**< [31:16] Reserved - 16'h0 */
    };
    uint32_t val; /**< Value representing the structure */
} wm_pwm_brake_t;

/**
 * @brief register to control clkdiv and period for ch4 of PWM
 * This union is used to describe PWM registers.
 */
typedef union {
    struct {
        uint32_t ch4_pnum : 8;  /**< [7:0] RW - CH4 PWM Generation Period Number - 8'h0
                                  *   Set PWM4 period number to PNUM4
                                  *   When PWM generates PNUM4 PWM signals, stop generating signals,
                                  *   trigger interrupt, and set interrupt status word */
        uint32_t period4  : 8;  /**< [15:8] RW - CH4 Period Register Value - 8'h0
                                  *   CH4 period register value
                                  *   Note: Period cannot exceed 255
                                  *   Note: In "center-aligned mode", the period number should not be 255.
                                  *
                                  * In edge-aligned mode (counter counting mode is decrement):
                                  * The value of the PERIOD register represents the period (PERIOD+1).
                                  * Duty cycle = (CMP+1)/(PERIOD+1).
                                  * CMP>=PERIOD: PWM output remains high.
                                  * CMP<PERIOD: PWM low pulse width is (PERIOD-CMP), high pulse width is (CMP+1).
                                  * CMP=0: PWM low pulse width is PERIOD, high pulse width is 1.
                                  *
                                  * Center-aligned mode:
                                  * The value of the PERIOD register: period is 2*(PERIOD+1).
                                  * Duty cycle = (2CMP+1)/(2(PERIOD+1)).
                                  * CMP>PERIOD: PWM remains high.
                                  * CMP<=PERIOD: PWM low pulse width = 2*(PERIOD-CMP)+1, high pulse width = (2*CMP)+1.
                                  * CMP=0: PWM low pulse width is 2*PERIOD+1, high pulse width is 1.
                                  * Note: In the center-aligned mode, the period should not be 255. Regardless of the mode,
                                  *    the channel period is determined jointly by the prescaler (N) and the period count (P),
                                  *    that is: the input clock is 40MHz, and after prescaling, the clock frequency f_div is:
                                  *    f_div = 40MHz/N, where N is the prescaler (16-bit). The output frequency f_output is:
                                  *    f_output=f_div/P, where P is the period count. In PWM mode, when the counter is set to edge-aligned mode,
                                  *    the counting mode should be set to decrement mode. */
        uint32_t clkdiv4  : 16; /**< [31:16] RW - CH4 Clock Divider - 16'h0
                                  *   CH4 clock divider counter
                                  *   The divider number is determined by the counter value
                                  *   Note: Divider range is (0-65535), if no division is needed, input 0 or 1. */
    };
    uint32_t val; /**< Value representing the structure */
} wm_pwm_ch4_freq_clkdiv_t;

/**
 * @brief main register1 to control ch4 of PWM
 * This union is used to describe PWM registers.
 */
typedef union {
    struct {
        uint32_t pinv4       : 1;  /**< [0] RW - CH4 Output Signal Polarity Enable - 1'b0
                                    *   1'b0: PWM output polarity inversion disabled
                                    *   1'b1: PWM output polarity inversion enabled */
        uint32_t cntmode4    : 1;  /**< [1] RW - CH4 Generation Loop Mode - 1'b0
                                    *   1'b0: Single mode
                                    *   1'b1: Auto-reload mode
                                    *   Note: During the CNTMODE change process, PWM_CMPDAT is reset to zero */
        uint32_t reserved_2  : 1;  /**< [2] Reserved bit - 1'b0 */
        uint32_t cnttype4    : 2;  /**< [4:3] RW - CH4 Counter Counting Mode - 2'b0
                                    *   2'b00: Edge-aligned mode (counter counting mode is increment, only for capture mode)
                                    *   2'b01: Edge-aligned mode (counter counting mode is decrement, only for PWM mode)
                                    *   2'b10: Center-aligned mode (only for PWM mode)
                                    *   Note: In PWM mode, when the counter is set to edge-aligned mode, the counting method needs to be set to decrement mode. */
        uint32_t reserved_5  : 3;  /**< [7:5] Reserved bits - 3'b0 */
        uint32_t cmp4        : 8;  /**< [15:8] RW - CH4 Period Register Value - 8'h0
                                    * In edge-aligned mode (counter counting mode is decrement):
                                    * The value of the PERIOD register represents the period (PERIOD+1).
                                    * Duty cycle = (CMP+1)/(PERIOD+1).
                                    * CMP>=PERIOD: PWM output remains high.
                                    * CMP<PERIOD: PWM low pulse width is (PERIOD-CMP), high pulse width is (CMP+1).
                                    * CMP=0: PWM low pulse width is PERIOD, high pulse width is 1.
                                    *
                                    * Center-aligned mode:
                                    * The value of the PERIOD register: period is 2*(PERIOD+1).
                                    * Duty cycle = (2CMP+1)/2 * (PERIOD+1).
                                    * CMP>PERIOD: PWM remains high.
                                    * CMP<=PERIOD: PWM low pulse width = 2*(PERIOD-CMP)+1, high pulse width = (2*CMP)+1.
                                    * CMP=0: PWM low pulse width is 2*PERIOD+1, high pulse width is 1.
                                    * Note: In the center-aligned mode, the period should not be 255. Regardless of the mode,
                                    *    the channel period is determined jointly by the prescaler (N) and the period count (P),
                                    *    that is: the input clock is 40MHz, and after prescaling, the clock frequency f_div is:
                                    *    f_div = 40MHz/N, where N is the prescaler (16-bit). The output frequency f_output is:
                                    *    f_output=f_div/P, where P is the period count. In PWM mode, when the counter is set to edge-aligned mode,
                                    *    the counting mode should be set to decrement mode.
                                    * */
        uint32_t reserved_16 : 16; /**< [31:16] Reserved bits - 16'h0 */
    };
    uint32_t val; /**< Value representing the structure */
} wm_pwm_ch4_ctrl1_t;

/**
 * @brief capture data from ch4 of PWM
 * This union is used to describe PWM registers.
 */
typedef union {
    struct {
        uint32_t pwm_rcap2dat : 16; /**< [15:0] RO - PWM Rising Capture Data Register
                                      *   Capture rising edge register
                                      *   Stores the current counter value when there is a rising edge in the input signal */
        uint32_t pwm_fcap2dat : 16; /**< [31:16] RO - PWM Falling Capture Data Register
                                      *   Capture falling edge register
                                      *   Stores the current counter value when there is a falling edge in the input signal */
    };
    uint32_t val; /**< Value representing the structure */
} wm_pwm_ch4_capture_t;

/**
 * @brief main register2 to control ch4 of PWM
 * This union is used to describe PWM registers.
 */
typedef union {
    struct {
        uint32_t capinv2           : 1;  /**< [0] RW - Capture Invert 2 - 1'b0
                                          *   Capture invert enable flag
                                          *   1'b0: Capture invert invalid
                                          *   1'b1: Capture invert valid
                                          *   Note: Only for CH4 */
        uint32_t cpen2             : 1;  /**< [1] RW - Capture Enable 2 - 1'b0
                                          *   Capture function enable flag
                                          *   1'b0: CH4 capture invalid
                                          *   1'b1: CH4 capture valid
                                          *   Note: Only for CH4 */
        uint32_t poen2             : 1;  /**< [2] RW - PWM Output Enable 2 - 1'b0
                                          *   PWM pin output enable bit
                                          *   1'b0: PWM pin output
                                          *   1'b1: PWM pin tri-state
                                          *   Note: Only for CH4 */
        uint32_t crlif2            : 1;  /**< [3] RW - Capture Rising 2 - 1'b0
                                          *   Capture rising edge interrupt flag
                                          *   1'b0: No rising edge captured
                                          *   1'b1: Rising edge captured
                                          *   Note: Only for CH4 */
        uint32_t cflif2            : 1;  /**< [4] RW - Capture Falling 2 - 1'b0
                                          *   Capture falling edge interrupt flag
                                          *   1'b0: No falling edge captured
                                          *   1'b1: Falling edge captured
                                          *   Note: Only for CH4 */
        uint32_t rlifov2           : 1;  /**< [5] RW - Rising Lifo Overflow 2 - 1'b0
                                          *   Rising edge delay interrupt flag
                                          *   1'b0: No rising edge delay interrupt
                                          *   1'b1: Rising edge delay interrupt
                                          *   Note: Only for CH4 */
        uint32_t flifov2           : 1;  /**< [6] RW - Falling Lifo Overflow 2 - 1'b0
                                          *   Falling edge delay interrupt flag
                                          *   1'b0: No falling edge delay interrupt
                                          *   1'b1: Falling edge delay interrupt
                                          *   Note: Only for CH4 */
        uint32_t overfl2           : 1;  /**< [7] RW - Overflow 2 - 1'b0
                                          *   Counter overflow flag
                                          *   1'b0: Counter not overflow
                                          *   1'b1: Counter overflow
                                          *   Note: Only for CH4 */
        uint32_t rlien2            : 1;  /**< [8] RW - flien 2 - 1'b0
                                          *   Rising edge capture interrupt enable bit
                                          *   1'b0: Rising edge interrupt invalid
                                          *   1'b1: Rising edge interrupt valid
                                          *   Note: Only for CH4 */
        uint32_t flien2            : 1;  /**< [9] RW - flien 2 - 1'b0
                                          *   Falling edge capture interrupt enable bit
                                          *   1'b0: Falling edge interrupt invalid
                                          *   1'b1: Falling edge interrupt valid
                                          *   Note: Only for CH4 */
        uint32_t dma_request2_mask : 1;  /**< [10] RW - DMA Request 2 Mask - 1'b0
                                          *   DMA request 2 enable
                                          *   1'b0: DMA_request2 invalid
                                          *   1'b1: DMA_request2 valid
                                          *   Note: Only for CH4 */
        uint32_t reserved_11       : 21; /**< [31:11] Reserved bits - 21'h0 */
    }; /**< Structure packing attribute */
    uint32_t val; /**< Value representing the structure */
} wm_pwm_ch4_ctrl2_t;

/**
 * @brief PWM registers layout
 * Notice: must 32bit for each member
 */
typedef struct {
    volatile wm_pwm_clkdiv01_t clkdiv01;
    volatile wm_pwm_clkdiv23_t clkdiv23;
    volatile wm_pwm_ctrl_t ctrl;
    volatile wm_pwm_period_t period;
    volatile wm_pwm_period_num_t period_num;
    volatile wm_pwm_compare_t compare;
    volatile wm_pwm_dead_time_t dead_time;
    volatile wm_pwm_int_ctrl_t int_ctrl;
    volatile wm_pwm_int_status_t int_status;
    volatile wm_pwm_ch0_capture_t ch0_capture;
    volatile wm_pwm_brake_t brake;
    volatile wm_pwm_ch4_freq_clkdiv_t ch4_clkdiv;
    volatile wm_pwm_ch4_ctrl1_t ch4_ctrl1;
    volatile wm_pwm_ch4_capture_t ch4_capture;
    volatile wm_pwm_ch4_ctrl2_t ch4_ctrl2;
} wm_pwm_reg_t;

#ifdef __cplusplus
}
#endif

#endif /* __WM_PWM_REG_STRUCT_H__ */
