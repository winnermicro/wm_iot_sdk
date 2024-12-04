/**
 * @file wm_pmu_reg_struct.h
 *
 * @brief register pmu Module
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

#ifndef __WM_PMU_REG_STRUCT_H__
#define __WM_PMU_REG_STRUCT_H__

#include "wm_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief pmu control register layout
 */
typedef union {
    struct {
        uint32_t stdby_en         : 1;  /* [0] standby enable configure; 0: wakeup state, 1: enter standby state */
        uint32_t sleep_en         : 1;  /* [1] sleep enable configure; 0: wakeup state, 1: enter sleep state */
        uint32_t io_sleep_en      : 1;  /* [2]  enable the key can trigger enter sleep function;
                                     0: disable, 1: io_wakeup key can trigger sleep in active state */
        uint32_t cal32k_en        : 1;  /* [3] RC 32K calibration start switch;
                                   0: calibration circuit reset, 1: start calibration circuit  */
        uint32_t bypass_32k       : 1;  /* [4] 32k xtal bypass control */
        uint32_t mux_bg           : 1;  /* [5] DLDO_CORE reference voltage source select; 0: ABG, 1: DBG */
        uint32_t wake_thres_level : 4;  /* [9:6] minimum time to wakeup by key, unit: 128ms */
        uint32_t wake_neg_sel     : 1;  /* [10] wakeup key isr polar select */
        uint32_t reserved_11      : 21; /* [31:11] Reserved */
    };
    uint32_t val;
} wm_pmu_ctrl_t;

/**
 * @brief pmu timer0 register layout
 */
typedef union {
    struct {
        uint32_t timer0_val  : 16; /* [15:0] timer0 value,uint: s */
        uint32_t timer0_en   : 1;  /* [16] timer0 enable */
        uint32_t reserved_17 : 15; /* [31:17] Reserved */
    };
    uint32_t val;
} wm_pmu_timer0_t;

/**
 * @brief pmu timer1 register layout
 */
typedef union {
    struct {
        uint32_t listen_interval : 16; /* [15:0] listen interval setting */
        uint32_t timer1_en       : 1;  /* [16] timer1 enable */
        uint32_t timer1_mcs      : 1;  /* [17] bcn_rcvd valid */
        uint32_t reserved_18     : 2;  /* [19:18] reserved */
        uint32_t pre_tbtt_timer  : 4;  /* [23:20] pre tbtt timer setting */
        uint32_t tbtt_timer      : 7;  /* [30:24] tbtt timer setting */
        uint32_t reserved_31     : 1;  /* [31] Reserved */
    };
    uint32_t val;
} wm_pmu_timer1_t;

/**
 * @brief pmu isr register layout
 */
typedef union {
    struct {
        uint32_t timer0_isr_flag  : 1;  /* [0] Timer0 ISR flag; 0: no timer0 ISR, 1: have timer0 ISR, write 1 to clear */
        uint32_t timer1_isr_flag  : 1;  /* [1] Timer1 ISR flag; 0: no timer1 ISR, 1: have timer1 ISR, write 1 to clear */
        uint32_t io_wake_isr_flag : 1;  /* [2] IO wakeup ISR flag; 0: no io wake ISR, 1: have io wake ISR, write 1 to clear */
        uint32_t io_sleep_flag    : 1;  /* [3] IO sleep ISR flag;  0: no io sleep ISR, 1: have io sleep ISR, write 1 to clear */
        uint32_t rtc_isr_flag     : 1;  /* [4] RTC ISR flag; 0: no RTC ISR, 1: have RTC ISR, write 1 to clear */
        uint32_t dpll_timeout     : 1;  /* [5] DPLL lock status; 0: DPLL locked within 2ms, 1: DPLL locked without 2ms */
        uint32_t osc_timeout      : 1;  /* [6] 40M OSC lock status; 0: OSC locked within 40ms, 1: OSC locked without 40ms */
        uint32_t standby_status   : 1;  /* [7] power on status; 0: initial power on or reset, 1: wake up from standby mode */
        uint32_t last_sleep_state : 1;  /* [8] show the state of last power on;
                                          0: power on or reset, 1: wakeup from sleep, write 1 to clear */
        uint32_t reserved_9       : 23; /* [31:9] Reserved */
    };
    uint32_t val;
} wm_pmu_isr_t;

/**
 * @brief pmu wlan stts register layout
 */
typedef union {
    struct {
        uint32_t abg_en          : 1;  /* [0] ABG enable signal; 0: close ABG, 1: ABG is controlled by PMU */
        uint32_t tune_dldo_core  : 3;  /* [3:1] Voltage adjustment signal of DLDO-CORE; Default=1.1V@(100),Step=50mV */
        uint32_t da_capdet_ibias : 3;  /* [6:4] CAPDET bias current programmable: 5u*<2>+2.5u*<1>+1.25u*<0> */
        uint32_t dbg_en          : 1;  /* [7] DBG enable signal; 0: close DBG, 1: DBG is controlled by PMU */
        uint32_t xtal_40m_en     : 1;  /* [8] 40M enable signal; 0: close 40M clk, 1: open 40M clk when sleep/standby */
        uint32_t wlan_tx_idle    : 1;  /* [9] TX idle flag; 1: TX is idle. */
        uint32_t wlan_rx_idle    : 1;  /* [10] RX idle flag; 1: RX is idle. */
        uint32_t reserved_11     : 21; /* [31:11] Reserved */
    };
    uint32_t val;
} wm_pmu_wlan_stts_t;

/**
 * @brief pmu bk register layout
 */
typedef union {
    struct {
        uint32_t reserved_0       : 17; /* [16:0] Reserved */
        uint32_t pd_lna_pmu       : 1;  /* [17] Turn off LNA in standby mode to prevent damage to LNA caused by large signals */
        uint32_t afc_clk_sel      : 1;  /* [18] AFC_32K calibration clock selection: 0:40M; 1: 24M */
        uint32_t xtal_buf_bg_mux  : 1;  /* [19] XTAL BUF BG MUX; 0: choose DBG, 1: choose ABG */
        uint32_t xtal_buf_dbg_mux : 1;  /* [20] XTAL BUF DBG EN; 0: close DBG, 1: open DBG */
        uint32_t reserved_21      : 1;  /* [21] Reserved */
        uint32_t dpll_mmd_cfg     : 1;  /* [22] DPLL division ratio settings; 1: default using 40M crystal; 0: SPI settings */
        uint32_t dpll_config      : 1;  /* [23] DPLL control enable; 1: DPLL use 40M crystal; 0: SPI settings */
        uint32_t reserved_24      : 8;  /* [31:24] Reserved */
    };
    uint32_t val;
} wm_pmu_bk_t;

/**
 * @brief pmu register layout
 * must 32bit for each member
 */
typedef struct {
    volatile wm_pmu_ctrl_t pmu_ctrl;       /* WM_PMU_BASE_ADDR + 0x00 */
    volatile wm_pmu_timer0_t pmu_timer0;   /* WM_PMU_BASE_ADDR + 0x04 */
    volatile wm_pmu_timer1_t pmu_timer1;   /* WM_PMU_BASE_ADDR + 0x08 */
    volatile uint32_t reserved1;           /* WM_PMU_BASE_ADDR + 0x0c */
    volatile uint32_t reserved2;           /* WM_PMU_BASE_ADDR + 0x10 */
    volatile wm_pmu_isr_t pmu_isr;         /* WM_PMU_BASE_ADDR + 0x14 */
    volatile uint32_t reserved3;           /* WM_PMU_BASE_ADDR + 0x18 */
    volatile wm_pmu_wlan_stts_t wlan_stts; /* WM_PMU_BASE_ADDR + 0x1c */
    volatile wm_pmu_bk_t pmu_bk;           /* WM_PMU_BASE_ADDR + 0x20 */
} wm_pmu_reg_t;

#ifdef __cplusplus
}
#endif

#endif /* __WM_PMU_REG_STRUCT_H__ */
