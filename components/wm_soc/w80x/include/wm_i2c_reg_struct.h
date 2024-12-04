/**
 * @file wm_i2c_reg_struct.h
 *
 * @brief register i2c Module
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

#ifndef __WM_I2C_REG_STRUCT_H__
#define __WM_I2C_REG_STRUCT_H__

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief i2c clock division register layout
 * This structure is used to clock division configuration for prescal low 8 bit
 */
typedef union {
    struct {
        uint32_t clk_div    : 8;  /**< [7:0] clock division configuration,
                                        eg: apb_clk=40MHz, SCL=100K
                                        prescale = (40*1000)/(5*100) - 1 = d79
                                        apb_clk=40MHz, SCL=400K
                                        prescale = (40*1000)/(5*100) - 1 = d19 */
        uint32_t reserved_8 : 24; /**< [31:8] Reserved */
    };
    uint32_t val;
} wm_i2c_div_low8_bit_t;

/**
 * @brief i2c clock division register layout
 * This structure is used to clock division configuration for prescal high 8 bit
 */
typedef union {
    struct {
        uint32_t clk_div    : 8;  /**< [7:0] clock division configuration,
                                        eg: apb_clk=40MHz, SCL=100K
                                        prescale = (40*1000)/(5*100) - 1 = d79
                                        apb_clk=40MHz, SCL=400K
                                        prescale = (40*1000)/(5*100) - 1 = d19 */
        uint32_t reserved_8 : 24; /**< [31:8] Reserved */
    };
    uint32_t val;
} wm_i2c_div_high8_bit_t;

/**
 * @brief i2c control register layout
 * This structure is used to control i2c
 */
typedef union {
    struct {
        uint32_t reserved_0  : 6;  /**< [5:0] Reserved */
        uint32_t irq_mask    : 1;  /**< [6] 1 for enable interrupt, 0 for disable interrupt */
        uint32_t ctrl_enable : 1;  /**< [7] 1 for control enable, 0 for control disable */
        uint32_t reserved_8  : 24; /**< [31:8] Reserved */
    };
    uint32_t val;
} wm_i2c_ctrl_t;

/**
 * @brief i2c data register layout
 * This structure is used to control i2c tx/rx one byte
 */
typedef union {
    struct {
        uint32_t data       : 8;  /**< [7:0] data */
        uint32_t reserved_8 : 24; /**< [31:8] Reserved */
    };
    uint32_t val;
} wm_i2c_data_t;

/**
 * @brief i2c tx/rx control and status register layout
 * This structure is used to control i2c tx/rx
 * @note:  the register is used as control and status register,as control register,
 * some bits must set at the same time, so not define control bits.
 */
typedef union {
    struct {
        uint32_t ifb        : 1; /**< [0:0] interrupt flag bit
                                    *   0: No intrrupt generate,
                                    *   1: set to 1 when transmission is completed or AL */
        uint32_t tip        : 1; /**< [1:1] transfer in progress
                                    *   0: No transfer in progress
                                    *   1: There is a transmission in progress */
        uint32_t reserved_2 : 3; /*[4:2] reserved */

        uint32_t al         : 1; /**< [5:5] Arbitration Lost, this bit reserved */
        uint32_t busy       : 1; /**< [6:6] busy
                                    *   0: set 0 after STOP
                                    *   1: set 1 after STRAT */
        uint32_t rx_ack     : 1; /**< [7:7] ack/nack status from slave
                                    *   0: ack from slave
                                    *   1: nack from slave */

        uint32_t reserved_8 : 24; /**< [31:8] Reserved */
    };
    uint32_t val;
} wm_i2c_ctrl_state_t;

/**
 * @brief i2c data register layout
 * This structure is used to record the data write in wm_i2c_data_t
 */
typedef union {
    struct {
        uint32_t data_ro    : 8;  /**< [7:0] data_ro */
        uint32_t reserved_8 : 24; /**< [31:8] Reserved */
    };
    uint32_t val;
} wm_i2c_data_ro_t;

/**
 * @brief i2c tx/rx control and status register layout
 * This structure is used to record the data write in wm_i2c_ctrl_state_t
 */
typedef union {
    union {
        struct {
            uint32_t state_ro   : 8;  /**< [7:0] state_ro */
            uint32_t reserved_8 : 24; /**< [31:8] Reserved */
        };
    };
    uint32_t val;
} wm_i2c_ctrl_state_ro_t;

/**
 * @brief i2c register layout
 * Notice: must 32bit for each member
 */
typedef struct {
    volatile wm_i2c_div_low8_bit_t low8bit;        /**< WM_I2C_BASE_ADDR        */
    volatile wm_i2c_div_high8_bit_t high8bit;      /**< WM_I2C_BASE_ADDR+0x004  */
    volatile wm_i2c_ctrl_t ctrl;                   /**< WM_I2C_BASE_ADDR+0x008  */
    volatile wm_i2c_data_t data;                   /**< *WM_I2C_BASE_ADDR+0x00c */
    volatile wm_i2c_ctrl_state_t ctrl_state;       /**< WM_I2C_BASE_ADDR+0x0010 */
    volatile wm_i2c_data_ro_t tx_data_ro;          /**< WM_I2C_BASE_ADDR+0x0014 */
    volatile wm_i2c_ctrl_state_ro_t ctrl_state_ro; /**< WM_I2C_BASE_ADDR+0x0018 */
} wm_i2c_reg_t;

#ifdef __cplusplus
}
#endif

#endif /* __WM_I2C_REG_STRUCT_H__ */
