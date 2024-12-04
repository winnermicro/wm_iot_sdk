/**
 * @file wm_uart_reg_struct.h
 *
 * @brief register uart Module
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

#ifndef __WM_UART_REG_STRUCT_H__
#define __WM_UART_REG_STRUCT_H__

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Data line ctonral register
 * This union is used to control uart data
 */
typedef union {
    struct {
        uint32_t data_bits       : 2; /**< [1:0] Control uart data bit length,
                                        3: 8bit
                                        2: 7bit
                                        1: 6bit
                                        0: 5bit */
        uint32_t stop_bits       : 1; /**< [2] Control uart stop bit,
                                        uart mode:
                                            1: 2 stop bit.
                                            0: 1 stop bit
                                        7816 mode:
                                            1: 1.5 stop bit.
                                            0: 0.5 stop bit */
        uint32_t parity_en       : 1; /**< [3] Enables parity,
                                        1: Enables parity.
                                        0: disables parity */
        uint32_t parity          : 1; /**< [4] set parity,forward and reverse
                                        uart mode:
                                            1: odd check.
                                            0: even check
                                        7816 mode:
                                            1: MSB, bit 7 transfer fisrt
                                            0: LSB, bit 0 transfer fisrt */
        uint32_t break_en        : 1; /**< [5] Send break packets. Uart will send a break packet after it is set,
                                        and it will automatically clear 0 after sending.*/

        uint32_t tx_en           : 1; /**< [6] uart send enable,
                                        1: Enables send.
                                        0: NA */
        uint32_t rx_en           : 1; /**< [7] uart receive enable,
                                        1: Enables receive.
                                        0: NA */

        /**<The following items are for 7816 only*/

        uint32_t protocol        : 1; /**< [8] 7816 mode, 0: T0, 1: T1*/
        uint32_t parity_err_recv : 1; /**< [9] 7816 parity error receive or not, 0: not recv, 1: recv*/
        uint32_t clk_ctrl        : 1; /**< [10] 0: Generate card clock output when configured in card mode
                                                1: stop output clock*/
        uint32_t min_bgt         : 5; /**< [15:11] Min Block Guard Time for 7816 Card
                                            Min Block Guard Time calculation: 10+stop bits (default 2 bits)+configuration value MIN_BGT
                                            Note:
                                            T=0: The minimum time interval between the starting and falling edges of two consecutive characters in
                                            opposite directions of transmission and reception cannot be less than 16 ETUs. It is necessary to be able
                                            to correctly interpret characters with an interval of 15 ETUs between the received starting bit falling
                                            edge and the last sent byte starting bit falling edge.
                                            T=1: The minimum time interval (block protection time, B GT) between the starting and falling edges of two
                                            consecutive characters in opposite directions must be 22 ETUs. It is necessary to be able to correctly
                                            explain that the interval between the falling edge of the starting bit received and the falling edge
                                            of the starting bit of the last byte sent is within 21 ETUs of the received characters*/
        uint32_t tx_retrans_cnt  : 3; /**< [18:16] tx auto retrans count*/
        uint32_t tx_retrans_en   : 1; /**< [19] tx auto retrans enable  */
        uint32_t rx_retrans_cnt  : 3; /**< [22:20] rx auto retrans count*/
        uint32_t rx_retrans_en   : 1; /**< [23] rx auto retrans enable  */
        uint32_t mode            : 1; /**< [24] mode , 0: uart, 1:7816  */

        uint32_t reserved_25     : 7; /**< [31:25] Reserved             */
    };
    uint32_t val;
} wm_uart_line_ctrl_t;

/**
 * @brief automatic flow control register
 * This union is used to control uart flow
 */
typedef union {
    struct {
        uint32_t afc_en         : 1; /**< [0] AFC enable (UART mode)
                                               The receiving condition RTS is generated using RTS_trigger_level control, which is highly effective.*/
        uint32_t rts_set        : 1; /**< [1] RTS set (UART mode)
                                               When AFC enable is invalid, the software can control the received flow by setting this bit.
                                               When AFC enable is valid, this bit does not care.*/
        uint32_t rts_trig_level : 3; /**< [4:2] RTS trigger level（UART mode）,valid if afc_enable,Decide when to disable RTS.
                                                3’h0：rxfifo has more than 4 bytes
                                                3’h1：rxfifo has more than 8 bytes
                                                3’h2：rxfifo has more than 12 bytes
                                                3’h3：rxfifo has more than 16 bytes
                                                3’h4：rxfifo has more than 20 bytes
                                                3’h5：rxfifo has more than 24 bytes
                                                3’h6：rxfifo has more than 28 bytes
                                                3’h7：rxfifo has more than 31 bytes */
        uint32_t reserved_5     : 27; /**< [31:5] Reserved   */
    };
    uint32_t val;
} wm_uart_flow_ctrl_t;

/**
 * @brief DMA control register
 * This union is used to control uart dma
 */
typedef union {
    struct {
        uint32_t tx_dma_en          : 1;  /**< [0] tx dma enable            */
        uint32_t rx_dma_en          : 1;  /**< [1] rx dma enable            */
        uint32_t rxfifo_timeout_en  : 1;  /**< [2] rx fifo timeout enable   */
        uint32_t rxfifo_timeout_num : 5;  /**< [7:3] rx fifo timeout num    */
        uint32_t reserved_8         : 24; /**< [31:8] Reserved              */
    };
    uint32_t val;
} wm_uart_dma_ctrl_t;

/**
 * @brief fifo control register
 * This union is used to control uart fifo
 */
typedef union {
    struct {
        uint32_t txfifo_reset         : 1;  /**< [0] reset tx fifo, clear fifo     */
        uint32_t rxfifo_reset         : 1;  /**< [1] reset rx fifo, clear fifo     */
        uint32_t txfifo_trigger_level : 2;  /**< [3:2] txfifo trigger level
                                                0: empty
                                                1: 4 bytes
                                                2: 8 bytes
                                                3: 16 bytes                    */
        uint32_t rxfifo_trigger_level : 2;  /**< [5:4] rxfifo trigger level
                                                0: 1 byte
                                                1: 4 bytes
                                                2: 8 bytes
                                                3: 16 bytes                     */
        uint32_t reserved_6           : 26; /**< [31:6] Reserved   */
    };
    uint32_t val;
} wm_uart_fifo_ctrl_t;

/**
 * @brief baud rate  control register
 * This union is used to control uart baud rate
 */
typedef union {
    /*for uart*/
    struct {
        uint32_t ubdiv       : 16; /**< [15:0]
                                        UART mode:
                                            The integer part of the system clock divided by 16 times the
                                            baud rate clock quotient is subtracted by 1.
                                            The default system clock is 40MHz and the baud rate is 19200.
                                            (Reference baud rate calculation method)*/
        uint32_t ubdiv_frac  : 4;  /**<[19:16]
                                        UART mode:
                                            The decimal part of the system clock divided by 16 times the baud rate
                                            clock quotient indicates. The specific value is frac x 16.
                                            (Refer to the chapter on baud rate calculation methods)*/
        uint32_t reserved_20 : 12; /**< [31:20] Reserved   */
    };

    /*for 7816*/
    struct {
        uint32_t etu         : 16; /**< [15:0]
                                        7816 mode:
                                            Ubdiv=Fi/Di (Fi, Di are parameters fed back by the smart card,
                                            edu frequency: f_etuclk=fsc_clk/(ubdiv+1))*/
        uint32_t clk_div     : 6;  /**<[21:16]
                                        7816 mode:
                                            Ubdiv_frac=(fclkaPB+fsc_clk)/(2 * fsc_clk) -1; (Refer to
                                            7816 clock calculation method)*/
        uint32_t reserved_22 : 10; /**< [31:22] Reserved   */
    };

    uint32_t val;

} wm_uart_baudrate_ctrl_t;

/**
 * @brief interrupt masking register
 * This union is used to control uart interrupt
 */
typedef union {
    struct {
        uint32_t txfifo_empty         : 1;  /**< [0] tx fifo empty interupt mask              */
        uint32_t txfifo_trigger_level : 1;  /**< [1] tx fifo to trigger level interupt mask   */
        uint32_t rxfifo_trigger_level : 1;  /**< [2] rx fifo to trigger level interupt mask   */
        uint32_t rx_data_timeout      : 1;  /**< [3] rx data timeout interupt mask            */
        uint32_t cts_change           : 1;  /**< [4] CTS change interupt mask                 */
        uint32_t break_detect         : 1;  /**< [5] break checked interupt mask              */
        uint32_t frame_err            : 1;  /**< [6] data frame error interupt mask           */
        uint32_t parity_err           : 1;  /**< [7] parity error interupt mask               */
        uint32_t rxfifo_over_err      : 1;  /**< [8] rx fifo over run interupt mask           */
        uint32_t error_signal         : 1;  /**< [9] 7816 error signal interupt mask          */
        uint32_t reserved_10          : 22; /**< [31:10] Reserved   */
    };
    uint32_t val;
} wm_uart_int_mask_t;

/**
 * @brief interrupt state register
 * This union is used to get uart interrupt state
 */
typedef union {
    struct {
        uint32_t txfifo_empty         : 1;  /**< [0] tx fifo empty               */
        uint32_t txfifo_trigger_level : 1;  /**< [1] tx fifo to trigger level    */
        uint32_t rxfifo_trigger_level : 1;  /**< [2] rx fifo to trigger level    */
        uint32_t rx_data_timeout      : 1;  /**< [3] rx data timeout             */
        uint32_t cts_change           : 1;  /**< [4] CTS change                  */
        uint32_t break_det            : 1;  /**< [5] break checked               */
        uint32_t frame_err            : 1;  /**< [6] data frame error            */
        uint32_t parity_err           : 1;  /**< [7] parity error                */
        uint32_t rxfifo_over_err      : 1;  /**< [8] rx fifo over run            */
        uint32_t error_signal         : 1;  /**< [9] 7816 error signal           */
        uint32_t reserved_10          : 22; /**< [31:10] Reserved                */
    };
    uint32_t val;
} wm_uart_int_src_t;

/**
 * @brief fifo state register
 * This union is used to get uart fifo state
 */
typedef union {
    struct {
        uint32_t txfifo_count : 6;  /**< [5:0] tx fifo count              */
        uint32_t rxfifo_count : 6;  /**< [11:6] rx fifo count             */
        uint32_t cts_status   : 1;  /**< [12] CTS status                  */
        uint32_t reserved_13  : 19; /**< [31:13] Reserved                 */
    };
    uint32_t val;
} wm_uart_fifo_status_t;

/**
 * @brief guard time register
 * This union is used to 7816
 */
typedef union {
    struct {
        uint32_t guard_time : 8;  /**< [7:0] guard time
                                        In 7816 mode, guard time calculation:
                                        10+stop bits+configuration value ex_gt_2um  */
        uint32_t reserved_8 : 24; /**< [31:8] Reserved                            */
    };
    uint32_t val;
} wm_7816_guard_time_t;

/**
 * @brief guard time register
 * This union is used to 7816
 */
typedef union {
    struct {
        uint32_t wait_time   : 24; /**< [23:0] wat_time
                                        In 7816 mode, CWT and BWT time, configured as the maximum default value.
                                        (In T1 mode: BWT=(11 etu+2BWI * 960 * Fd/fsc))*/
        uint32_t reserved_24 : 8;  /**< [31:24] Reserved   */
    };
    uint32_t val;
} wm_7816_wait_time_t;

/**
 * @brief uart register layout
 */
typedef struct {
    volatile wm_uart_line_ctrl_t line_ctrl;     /**< WM_UART_BASE_ADDR          */
    volatile wm_uart_flow_ctrl_t flow_ctrl;     /**< WM_UART_BASE_ADDR + 0x004  */
    volatile wm_uart_dma_ctrl_t dma_ctrl;       /**< WM_UART_BASE_ADDR + 0x008  */
    volatile wm_uart_fifo_ctrl_t fifo_ctrl;     /**< WM_UART_BASE_ADDR + 0x00C  */
    volatile wm_uart_baudrate_ctrl_t baud_rate; /**< WM_UART_BASE_ADDR + 0x010  */
    volatile wm_uart_int_mask_t int_mask;       /**< WM_UART_BASE_ADDR + 0x014  */
    volatile wm_uart_int_src_t int_src;         /**< WM_UART_BASE_ADDR + 0x018  */
    volatile wm_uart_fifo_status_t fifo_status; /**< WM_UART_BASE_ADDR + 0x01C  */
    volatile uint32_t tx_data_window;           /**< WM_UART_BASE_ADDR + 0x020  */
    volatile uint32_t reserved_1[3];            /**< WM_UART_BASE_ADDR + 0x024  */
    volatile uint32_t rx_data_window;           /**< WM_UART_BASE_ADDR + 0x030  */
    volatile uint32_t reserved_2[3];            /**< WM_UART_BASE_ADDR + 0x034  */
    volatile wm_7816_guard_time_t guard_time;   /**< WM_UART_BASE_ADDR + 0x040  */
    volatile wm_7816_wait_time_t wait_time;     /**< WM_UART_BASE_ADDR + 0x044  */
} wm_uart_reg_t;

#ifdef __cplusplus
}
#endif

#endif /* __WM_UART_REG_STRUCT_H__ */
