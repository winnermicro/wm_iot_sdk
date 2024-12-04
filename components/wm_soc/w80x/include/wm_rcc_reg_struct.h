/**
 * @file wm_rcc_reg_struct.h
 *
 * @brief register rcc Module
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

#ifndef __WM_RCC_REG_STRUCT_H__
#define __WM_RCC_REG_STRUCT_H__

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief rcc clock enable register layout
 * This structure is used to clock enable configuration
 */
typedef union {
    struct {
        uint32_t i2c_gate_enable    : 1;  /* [0] Configure the i2c clock enable; 0: disable, 1: enable */
        uint32_t uart0_gate_enable  : 1;  /* [1] Configure the uart0 clock enable; 0: disable, 1: enable */
        uint32_t uart1_gate_enable  : 1;  /* [2] Configure the uart1 clock enable; 0: disable, 1: enable */
        uint32_t uart2_gate_enable  : 1;  /* [3] Configure the uart2 clock enable; 0: disable, 1: enable */
        uint32_t uart3_gate_enable  : 1;  /* [4] Configure the uart3 clock enable; 0: disable, 1: enable */
        uint32_t uart4_gate_enable  : 1;  /* [5] Configure the uart4 clock enable; 0: disable, 1: enable */
        uint32_t uart5_gate_enable  : 1;  /* [6] Configure the uart5 clock enable; 0: disable, 1: enable */
        uint32_t lspi_gate_enable   : 1;  /* [7] Configure the low speed spi clock enable; 0: disable, 1: enable */
        uint32_t dma_gate_enable    : 1;  /* [8] Configure the dma clock enable; 0: disable, 1: enable */
        uint32_t rf_cfg_gate_enable : 1;  /* [9] Configure the rf cfg clock enable,for internal use; 0: disable, 1: enable */
        uint32_t timer_gate_enable  : 1;  /* [10] Configure the timer clock enable; 0: disable, 1: enable */
        uint32_t gpio_gate_enable   : 1;  /* [11] Configure the gpio clock enable; 0: disable, 1: enable */
        uint32_t sd_adc_gate_enable : 1;  /* [12] Configure the sd adc clock enable; 0: disable, 1: enable */
        uint32_t pwm_gate_enable    : 1;  /* [13] Configure the pwm clock enable; 0: disable, 1: enable */
        uint32_t lcd_gate_enable    : 1;  /* [14] Configure the lcd clock enable; 0: disable, 1: enable */
        uint32_t i2s_gate_enable    : 1;  /* [15] Configure the i2s clock enable; 0: disable, 1: enable */
        uint32_t rsa_gate_enable    : 1;  /* [16] Configure the rsa clock enable; 0: disable, 1: enable */
        uint32_t gpsec_gate_enable  : 1;  /* [17] Configure the gpsec clock enable; 0: disable, 1: enable */
        uint32_t sdiom_gate_enable  : 1;  /* [18] Configure the sdio master clock enable; 0: disable, 1: enable */
        uint32_t qsram_gate_enable  : 1;  /* [19] Configure the qspi ram clock enable; 0: disable, 1: enable */
        uint32_t bt_gate_enable     : 1;  /* [20] Configure the bt/ble clock enable; 0: disable, 1: enable */
        uint32_t touch_gate_enable  : 1;  /* [21] Configure the touch sensor clock enable; 0: disable, 1: enable */
        uint32_t reserved           : 10; /* [31:22] Reserved */
    };
    uint32_t val;
} wm_rcc_clock_enable_t;

/**
 * @brief rcc clock mask register layout
 * This structure is used to clock mask configuration
 */
typedef union {
    struct {
        uint32_t pmu_clk_gate_msk  : 1;  /* [0] Configure the pmu clock gate mask; 
                                            0: Allow to close the clock gate, 1: Do not allow to close the clock gate */
        uint32_t sdio_ahb_gate_msk : 1;  /* [1] Configure the sdio ahb clock gate mask; 
                                            0: Allow to close the clock gate, 1: Do not allow to close the clock gate */
        uint32_t reserved_2        : 4;  /* [31:8] Reserved */
        uint32_t cpu_clk_gate_msk  : 1;  /* [6] Configure the cpu clock gate mask; 
                                            0: Allow to close the clock gate, 1: Do not allow to close the clock gate */
        uint32_t reserved_7        : 24; /* [31:8] Reserved */
    };
    uint32_t val;
} wm_rcc_clock_gate_mask_t;

/**
 * @brief rcc clock reset control register layout
 * This structure is used to control clock reset
 */
typedef union {
    struct {
        uint32_t bbp_rst      : 1; /* [0] Soft reset the bbp module, for internal use; 0: reset, 1: reset release */
        uint32_t mac_rst      : 1; /* [1] Soft reset the mac module, for internal use; 0: reset, 1: reset release */
        uint32_t sec_rst      : 1; /* [2] Soft reset the security module, for internal use; 0: reset, 1: reset release */
        uint32_t sdio_ahb_rst : 1; /* [3] Soft reset the sdio ahb module; 0: reset, 1: reset release */
        uint32_t dma_rst      : 1; /* [4] Soft reset the dma module; 0: reset, 1: reset release */
        uint32_t mem_mng_rst  : 1; /* [5] Soft reset the mem_mng module; 0: reset, 1: reset release */
        uint32_t apb_rst      : 1; /* [6] Soft reset the apb bridge module; 0: reset, 1: reset release */
        uint32_t bus1_rst     : 1; /* [7] Soft reset the bus1 module; 0: reset, 1: reset release */
        uint32_t bus2_rst     : 1; /* [8] Soft reset the bus2 module; 0: reset, 1: reset release */
        uint32_t i2c_rst      : 1; /* [9] Soft reset the i2c module; 0: reset, 1: reset release */
        uint32_t uart0_rst    : 1; /* [10] Soft reset the uart0 module; 0: reset, 1: reset release */
        uint32_t uart1_rst    : 1; /* [11] Soft reset the uart1 module; 0: reset, 1: reset release */
        uint32_t uart2_rst    : 1; /* [12] Soft reset the uart2 module; 0: reset, 1: reset release */
        uint32_t uart3_rst    : 1; /* [13] Soft reset the uart3 module; 0: reset, 1: reset release */
        uint32_t uart4_rst    : 1; /* [14] Soft reset the uart4 module; 0: reset, 1: reset release */
        uint32_t uart5_rst    : 1; /* [15] Soft reset the uart5 module; 0: reset, 1: reset release */
        uint32_t spim_rst     : 1; /* [16] Soft reset the low speed spi module; 0: reset, 1: reset release */
        uint32_t spis_rst     : 1; /* [17] Soft reset the high speed spi module; 0: reset, 1: reset release */
        uint32_t rf_cfg_rst   : 1; /* [18] Soft reset the rf module, for internal use; 0: reset, 1: reset release */
        uint32_t gpio_rst     : 1; /* [19] Soft reset the gpio module; 0: reset, 1: reset release */
        uint32_t timer_rst    : 1; /* [20] Soft reset the timer module; 0: reset, 1: reset release */
        uint32_t sar_adc_rst  : 1; /* [21] Soft reset the sar_adc module; 0: reset, 1: reset release */
        uint32_t pwm_rst      : 1; /* [22] Soft reset the pwm module; 0: reset, 1: reset release */
        uint32_t lcd_rst      : 1; /* [23] Soft reset the lcd module; 0: reset, 1: reset release */
        uint32_t i2s_rst      : 1; /* [24] Soft reset the i2s module; 0: reset, 1: reset release */
        uint32_t rsa_rst      : 1; /* [25] Soft reset the rsa module; 0: reset, 1: reset release */
        uint32_t gpsec_rst    : 1; /* [26] Soft reset the gpsec module; 0: reset, 1: reset release */
        uint32_t sdiom_rst    : 1; /* [27] Soft reset the sdio master module; 0: reset, 1: reset release */
        uint32_t qspi_ram_rst : 1; /* [28] Soft reset the qspi_ram module; 0: reset, 1: reset release */
        uint32_t bt_rst       : 1; /* [29] Soft reset the bt/ble module; 0: reset, 1: reset release */
        uint32_t flash_rst    : 1; /* [30] Soft reset the flash module; 0: reset, 1: reset release */
        uint32_t touch_rst    : 1; /* [31] Soft reset the touch module; 0: reset, 1: reset release */
    };
    uint32_t val;
} wm_rcc_clk_rst_t;

/**
 * @brief rcc clock divider register layout
 * This structure is used to control rcc clock divider
 */
typedef union {
    struct {
        uint32_t cpu_clk_div        : 8; /* [7:0] cpu clock divider */
        uint32_t wlan_clk_div       : 8; /* [15:8] WLAN clock divider */
        uint32_t bus_syncdn_factor  : 8; /* [23:16] The clock scaling factor between bus1 and bus2 */
        uint32_t peripheral_clk_div : 4; /* [27:24] peripheral clock divider */
        uint32_t reserved           : 3; /* [30:28] Reserved */
        uint32_t freq_div_en        : 1; /* [31] clock divider enable */
    };
    uint32_t val;
} wm_rcc_clk_div_t;

/**
 * @brief rcc debug control register layout
 */
typedef union {
    struct {
        uint32_t reserved_1     : 4;  /* [3:0] Reserved */
        uint32_t rsa_clk_sel    : 1;  /* [4] rsa clock slection; 0: 80MHz, 1: 160MHz */
        uint32_t gpspec_clk_sel : 1;  /* [5] gpspec clock slection; 0: 80MHz, 1: 160MHz */
        uint32_t qflash_clk_sel : 1;  /* [6] qflash clock slection; 0: 40MHz, 1: 80MHz */
        uint32_t reserved_2     : 1;  /* [7] Reserved */
        uint32_t sd_adc_div     : 8;  /* [15:8] sigma-delta ADC clock divider */
        uint32_t jtag_en        : 1;  /* [16] jtag function enable; 0: disable jtag, 1: enable jtag */
        uint32_t reserved_3     : 15; /* [31:17] Reserved */
    };
    uint32_t val;
} wm_rcc_debug_ctrl_t;

/**
 * @brief rcc i2s clock control register layout
 */
typedef union {
    struct {
        uint32_t extal_en : 1;  /* [0] external XTAL selection; 0: internal XTAL, 1: external XTAL */
        uint32_t mclk_en  : 1;  /* [1] mclk enable; 0: disable mclk, 1: enable mclk */
        uint32_t mclk_div : 6;  /* [7:2] mclk divider*/
        uint32_t bclk_div : 10; /* [17:8] bclk divider */
        uint32_t reserved : 14; /* [31:18] Reserved */
    };
    uint32_t val;
} wm_rcc_i2s_ctrl_t;

/**
 * @brief rcc clock reset state register layout
 */
typedef union {
    struct {
        uint32_t wdog_rst_state     : 1;  /* [0] watch dog reset state; 0: do not trigger reset, 1: triggered reset */
        uint32_t cpu_rst_state      : 1;  /* [1] cpu reset state; 0: do not trigger reset, 1: triggered reset */
        uint32_t wdog_rst_state_clr : 6;  /* [7:2] watch dog reset state clear, write 1 to clear reset state */
        uint32_t cpu_rst_state_clr  : 10; /* [17:8] cpu reset state clear, write 1 to clear reset state */
        uint32_t reserved           : 14; /* [31:18] Reserved */
    };
    uint32_t val;
} wm_rcc_clk_rst_state_t;

/**
 * @brief rcc register layout
 * Notice: must 32bit for each member
 */
typedef struct {
    volatile wm_rcc_clock_enable_t clk_en;         /*WM_RCC_BASE_ADDR*/
    volatile wm_rcc_clock_gate_mask_t clk_msk;     /*WM_RCC_BASE_ADDR+0x0004*/
    volatile uint32_t reserved;                    /*WM_RCC_BASE_ADDR+0x0008*/
    volatile wm_rcc_clk_rst_t clk_rst;             /*WM_RCC_BASE_ADDR+0x000c*/
    volatile wm_rcc_clk_div_t clk_div;             /*WM_RCC_BASE_ADDR+0x0010*/
    volatile wm_rcc_debug_ctrl_t clk_dbg_ctrl;     /*WM_RCC_BASE_ADDR+0x0014*/
    volatile wm_rcc_i2s_ctrl_t i2s_ctrl;           /*WM_RCC_BASE_ADDR+0x0018*/
    volatile wm_rcc_clk_rst_state_t clk_rst_state; /*WM_RCC_BASE_ADDR+0x001c*/
} wm_rcc_reg_t;

#ifdef __cplusplus
}
#endif

#endif /* __WM_RCC_REG_STRUCT_H__ */
