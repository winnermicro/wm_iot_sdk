/**
 * @file wm_ll_uart.h
 *
 * @brief ll uart Module
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

#ifndef __WM_LL_UART_H__
#define __WM_LL_UART_H__

#include "wm_types.h"
#include "wm_attr.h"
#include "wm_uart_reg_struct.h"
#include "wm_reg_op.h"
#include "wm_soc_cfgs.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @enum    uart interrupt mask
 */
typedef enum {
    WM_LL_UART_TX_FIFO_EMPTY      = (1 << 0), /*< [0] tx fifo empty interupt mask              */
    WM_LL_UART_TX_FIFO_TRIG_LEVEL = (1 << 1), /*< [1] tx fifo to trigger level interupt mask   */
    WM_LL_UART_RX_FIFO_TRIG_LEVEL = (1 << 2), /*< [2] rx fifo to trigger level interupt mask   */
    WM_LL_UART_RX_DATA_TIMEOUT    = (1 << 3), /*< [3] rx data timeout interupt mask            */
    WM_LL_UART_CTS_CHANGE         = (1 << 4), /*< [4] CTS change interupt mask                 */
    WM_LL_UART_BREAK_DETECT       = (1 << 5), /*< [5] break checked interupt mask              */
    WM_LL_UART_FRAME_ERROR        = (1 << 6), /*< [6] data frame error interupt mask           */
    WM_LL_UART_PATIRY_ERROR       = (1 << 7), /*< [7] parity error interupt mask               */
    WM_LL_UART_RXFIFO_OVER_ERROR  = (1 << 8), /*< [8] rx fifo over run interupt mask           */
    WM_LL_UART_7816_ERROR_SIGNAL  = (1 << 9)  /*< [9] 7816 error signal interupt mask          */
} wm_ll_uart_irq_mask_t;

/**
 * @brief          Set uart data bits
 * @param[in]      reg Uart channel regsiter handle,type of  @arg wm_uart_reg_t *
 * @param[in]      bit Data bits,type of  @arg wm_uart_data_bits_t
 */
ATTRIBUTE_INLINE void wm_ll_uart_set_data_bits(wm_uart_reg_t *reg, wm_uart_data_bits_t bits)
{
    reg->line_ctrl.data_bits = bits;
}

/**
 * @brief          Get uart data bits
 * @param[in]      reg Uart channel regsiter handle,type of  @arg wm_uart_reg_t *
 * @retval         Data bits,type of  @ref wm_uart_data_bits_t
 */
ATTRIBUTE_INLINE wm_uart_data_bits_t wm_ll_uart_get_data_bits(wm_uart_reg_t *reg)
{
    return reg->line_ctrl.data_bits;
}

/**
 * @brief          Set uart stop bits
 * @param[in]      reg Uart channel regsiter handle,type of  @arg wm_uart_reg_t *
 * @param[in]      bit Stop bits,type of  @arg wm_uart_stop_bits_t
 */
ATTRIBUTE_INLINE void wm_ll_uart_set_stop_bits(wm_uart_reg_t *reg, wm_uart_stop_bits_t bits)
{
    reg->line_ctrl.stop_bits = bits;
}

/**
 * @brief           Get uart stop bits
 * @param[in]       reg Uart channel regsiter handle,type of  @arg wm_uart_reg_t *
 * @retval          Stop bits,type of  @ref wm_uart_stop_bits_t
 */
ATTRIBUTE_INLINE wm_uart_stop_bits_t wm_ll_uart_get_stop_bits(wm_uart_reg_t *reg)
{
    return reg->line_ctrl.stop_bits;
}

/**
 * @brief          Enable uart parity
 * @param[in]      reg Uart channel regsiter handle,type of  @arg wm_uart_reg_t *
 * @param[in]      en Parity Enable or not
 */
ATTRIBUTE_INLINE void wm_ll_uart_enable_parity(wm_uart_reg_t *reg, bool en)
{
    reg->line_ctrl.parity_en = en;
}

/**
 * @brief          Get parity enabled or not
 * @param[in]      reg Uart channel regsiter handle,type of  @arg wm_uart_reg_t *
 * @retval         Parity enable status
 */
ATTRIBUTE_INLINE bool wm_ll_uart_is_parity_enabled(wm_uart_reg_t *reg)
{
    return reg->line_ctrl.parity_en;
}

/**
 * @brief          Set uart parity type
 * @param[in]      reg Uart channel regsiter handle,type of  @ref wm_uart_reg_t *
 * @param[in]      parity Paitry check,type of  @ref wm_uart_parity_t
 */
ATTRIBUTE_INLINE void wm_ll_uart_set_parity(wm_uart_reg_t *reg, wm_uart_parity_t parity)
{
    /*The enum is 1, 2; the register need 0, 1*/
    reg->line_ctrl.parity = parity - 1;
}

/**
 * @brief          Get uart parity type
 * @param[in]      reg Uart channel regsiter handle,type of  @arg wm_uart_reg_t *
 * @retval         Get paitry type,type of  @ref wm_uart_parity_t
 */
ATTRIBUTE_INLINE wm_uart_parity_t wm_ll_uart_get_parity(wm_uart_reg_t *reg)
{
    /*The enum is 1, 2; the register need 0, 1*/
    return reg->line_ctrl.parity + 1;
}

/**
 * @brief          Enable uart break , Send break packets.Uart will send a break packet after it is set,
                   and it will automatically clear 0 after sending
 * @param[in]      reg Uart channel regsiter handle,type of  @arg wm_uart_reg_t *
 * @param[in]      en Enable or not
 */
ATTRIBUTE_INLINE void wm_ll_uart_enable_break(wm_uart_reg_t *reg, bool en)
{
    reg->line_ctrl.break_en = en;
}

/**
 * @brief          Get break enabled or not
 * @param[in]      reg Uart channel regsiter handle,type of  @arg wm_uart_reg_t *
 * @retval         Break enabled
 */
ATTRIBUTE_INLINE bool wm_ll_uart_is_break_enabled(wm_uart_reg_t *reg)
{
    return (bool)reg->line_ctrl.break_en;
}

/**
 * @brief          Enable uart tx enable
 * @param[in]      reg Uart channel regsiter handle,type of  @arg wm_uart_reg_t *
 * @param[in]      en  Enable or not,type of  @arg bool
 */
ATTRIBUTE_INLINE void wm_ll_uart_enable_tx(wm_uart_reg_t *reg, bool en)
{
    reg->line_ctrl.tx_en = en;
}

/**
 * @brief          Get tx enabled or not
 * @param[in]      reg Uart channel regsiter handle,type of  @arg wm_uart_reg_t *
 * @retval         Enable or not
 */
ATTRIBUTE_INLINE bool wm_ll_uart_is_tx_enabled(wm_uart_reg_t *reg)
{
    return (bool)reg->line_ctrl.tx_en;
}

/**
 * @brief          Enable uart rx enable
 * @param[in]      reg Uart channel regsiter handle,type of  @arg wm_uart_reg_t *
 * @param[in]      en  Enable or not,type of  @arg bool
 */
ATTRIBUTE_INLINE void wm_ll_uart_enable_rx(wm_uart_reg_t *reg, bool en)
{
    reg->line_ctrl.rx_en = en;
}

/**
 * @brief          Get rx enabled or not
 * @param[in]      reg Uart channel regsiter handle,type of  @arg wm_uart_reg_t *
 * @retval         Enable or not
 */
ATTRIBUTE_INLINE bool wm_ll_uart_is_rx_enabled(wm_uart_reg_t *reg)
{
    return (bool)reg->line_ctrl.rx_en;
}

/**
 * @brief          Set line control
 * @param[in]      reg Uart channel regsiter handle,type of  @arg wm_uart_reg_t *
 * @param[in]      val  line control value
 */
ATTRIBUTE_INLINE void wm_ll_uart_set_line_ctrl(wm_uart_reg_t *reg, uint32_t val)
{
    reg->line_ctrl.val = val;
}

/**
 * @brief          Set flow control
 * @param[in]      reg Uart channel regsiter handle,type of  @arg wm_uart_reg_t *
 * @param[in]      val  flow control value
 */
ATTRIBUTE_INLINE void wm_ll_uart_set_flow_ctrl(wm_uart_reg_t *reg, uint32_t val)
{
    reg->flow_ctrl.val = val;
}

/**
 * @brief          Enable hardware auto flow control (UART mode)
 *                 The receiving condition RTS is generated using RTS_trigger_level control, which is highly effective.
 * @param[in]      reg Uart channel regsiter handle,type of  @arg wm_uart_reg_t *
 * @param[in]      en  Enable or not,type of  @arg bool
 */
ATTRIBUTE_INLINE void wm_ll_uart_enable_afc(wm_uart_reg_t *reg, bool en)
{
    reg->flow_ctrl.afc_en = en;
}

/**
 * @brief          Get afc enabled or not
 * @param[in]      reg Uart channel regsiter handle,type of  @arg wm_uart_reg_t *
 * @retval         Enable or not
 */
ATTRIBUTE_INLINE bool wm_ll_uart_is_afc_enabled(wm_uart_reg_t *reg)
{
    return (bool)reg->flow_ctrl.afc_en;
}

/**
 * @brief          Enable RTS set
 *                 When AFC enable is invalid, the software can control the received flow by setting this bit.
                   When AFC enable is valid, this bit does not care.
 * @param[in]      reg Uart channel regsiter handle,type of  @arg wm_uart_reg_t *
 * @param[in]      en  Enable or not,type of  @arg bool
 */
ATTRIBUTE_INLINE void wm_ll_uart_enable_rts_set(wm_uart_reg_t *reg, bool en)
{
    reg->flow_ctrl.rts_set = en;
}

/**
 * @brief          Get rts set enabled or not
 * @param[in]      reg Uart channel regsiter handle,type of  @arg wm_uart_reg_t *
 * @retval         Enable or not
 */
ATTRIBUTE_INLINE bool wm_ll_uart_is_rts_set_enabled(wm_uart_reg_t *reg)
{
    return (bool)reg->flow_ctrl.rts_set;
}

/**
 * @brief           Set uart RTS trigger level
 *                  3’h0：rxfifo has more than 4 bytes
                    3’h1：rxfifo has more than 8 bytes
                    3’h2：rxfifo has more than 12 bytes
                    3’h3：rxfifo has more than 16 bytes
                    3’h4：rxfifo has more than 20 bytes
                    3’h5：rxfifo has more than 24 bytes
                    3’h6：rxfifo has more than 28 bytes
                    3’h7：rxfifo has more than 31 bytes .
 * @param[in]      reg Uart channel regsiter handle,type of  @arg wm_uart_reg_t *
 * @param[in]      trigger_level  RTS trigger level
 */
ATTRIBUTE_INLINE void wm_ll_uart_set_rts_trig_level(wm_uart_reg_t *reg, uint8_t trigger_level)
{
    reg->flow_ctrl.rts_trig_level = trigger_level;
}

/**
 * @brief          Get RTS trigger level
 * @param[in]      reg Uart channel regsiter handle,type of  @arg wm_uart_reg_t *
 * @retval         RTS trigger level
 */
ATTRIBUTE_INLINE uint8_t wm_ll_uart_get_rts_trig_level(wm_uart_reg_t *reg)
{
    return (uint8_t)reg->flow_ctrl.rts_trig_level;
}

/**
 * @brief          Set dma
 * @param[in]      reg Uart channel regsiter handle,type of  @arg wm_uart_reg_t *
 * @param[in]      val  dma value
 */
ATTRIBUTE_INLINE void wm_ll_uart_set_dma_ctrl(wm_uart_reg_t *reg, uint32_t val)
{
    reg->dma_ctrl.val = val;
}

/**
 * @brief          Enable tx dma
 * @param[in]      reg Uart channel regsiter handle,type of  @arg wm_uart_reg_t *
 * @param[in]      en  Enable or not,type of  @arg bool
 */
ATTRIBUTE_INLINE void wm_ll_uart_enable_tx_dma(wm_uart_reg_t *reg, bool en)
{
    reg->dma_ctrl.tx_dma_en = en;
}

/**
 * @brief          Get tx_dma enabled or not
 * @param[in]      reg Uart channel regsiter handle,type of  @arg wm_uart_reg_t *
 * @retval         Enable or not
 */
ATTRIBUTE_INLINE bool wm_ll_uart_is_tx_dma_enabled(wm_uart_reg_t *reg)
{
    return (bool)reg->dma_ctrl.tx_dma_en;
}

/**
 * @brief          Uart enable rx dma
 * @param[in]      reg Uart channel regsiter handle,type of  @arg wm_uart_reg_t *
 * @param[in]      en  Enable or not,type of  @arg bool
 */
ATTRIBUTE_INLINE void wm_ll_uart_enable_rx_dma(wm_uart_reg_t *reg, bool en)
{
    reg->dma_ctrl.rx_dma_en = en;
}

/**
 * @brief          Get rx_dma enabled or not
 * @param[in]      reg Uart channel regsiter handle,type of  @arg wm_uart_reg_t *
 * @retval         Enable or not
 */
ATTRIBUTE_INLINE bool wm_ll_uart_is_rx_dma_enabled(wm_uart_reg_t *reg)
{
    return (bool)reg->dma_ctrl.rx_dma_en;
}

/**
 * @brief          Uart enable fifo timeout
 * @param[in]      reg Uart channel regsiter handle,type of  @arg wm_uart_reg_t *
 * @param[in]      en  Enable or not,type of  @arg bool
 */
ATTRIBUTE_INLINE void wm_ll_uart_enable_rxfifo_timeout(wm_uart_reg_t *reg, bool en)
{
    reg->dma_ctrl.rxfifo_timeout_en = en;
}

/**
 * @brief          Get rxfifo_timeout enabled or not
 * @param[in]      reg Uart channel regsiter handle,type of  @arg wm_uart_reg_t *
 * @retval         Enable or not
 */
ATTRIBUTE_INLINE bool wm_ll_uart_is_rxfifo_timeout_enabled(wm_uart_reg_t *reg)
{
    return (bool)reg->dma_ctrl.rxfifo_timeout_en;
}

/**
 * @brief           Set uart rx fifo timeout
 *                  If there is data in rxfifo that is smaller than rxfifo_trigger_level,
 *                  and no new data is received within N packets, an rxfifo timeout interrupt is generated.
 *                  After the timing function is enabled, whether it is the first timing or the last timing
 *                  completion, the timing will only start after receiving at least one packet
 * @param[in]      reg Uart channel regsiter handle,type of  @arg wm_uart_reg_t *
 * @param[in]      timeout  Timeout time
 */
ATTRIBUTE_INLINE void wm_ll_uart_set_rxfifo_timeout(wm_uart_reg_t *reg, uint8_t timeout)
{
    reg->dma_ctrl.rxfifo_timeout_num = timeout;
}

/**
 * @brief          Get uart rx fifo timeout
 * @param[in]      reg Uart channel regsiter handle,type of  @arg wm_uart_reg_t *
 * @retval         Timeout time
 */
ATTRIBUTE_INLINE uint8_t wm_ll_uart_get_rxfifo_timeout(wm_uart_reg_t *reg)
{
    return (uint8_t)reg->dma_ctrl.rxfifo_timeout_num;
}

/**
 * @brief          Set fifo ctrl
 * @param[in]      reg Uart channel regsiter handle,type of  @arg wm_uart_reg_t *
 * @param[in]      val  fifo ctrl
 */
ATTRIBUTE_INLINE void wm_ll_uart_set_fifo_ctrl(wm_uart_reg_t *reg, uint32_t val)
{
    reg->fifo_ctrl.val = val;
}

/**
 * @brief          Reset tx fifo
 * @param[in]      reg Uart channel regsiter handle,type of  @arg wm_uart_reg_t *
 */
ATTRIBUTE_INLINE void wm_ll_uart_reset_tx_fifo(wm_uart_reg_t *reg)
{
    reg->fifo_ctrl.txfifo_reset = 1;
}

/**
 * @brief          Reset rx fifo
 * @param[in]      reg Uart channel regsiter handle,type of  @arg wm_uart_reg_t *
 */
ATTRIBUTE_INLINE void wm_ll_uart_reset_rx_fifo(wm_uart_reg_t *reg)
{
    reg->fifo_ctrl.rxfifo_reset = 1;
}

/**
 * @brief           Tx fifo trigger level
 *                  0: empty
 *                  1: 4 bytes
 *                  2: 8 bytes
 *                  3: 16 bytes.
 * @param[in]      reg Uart channel regsiter handle,type of  @arg wm_uart_reg_t *
 * @param[in]      trigger_level  RTS trigger level
 */
ATTRIBUTE_INLINE void wm_ll_uart_set_txfifo_trigger_level(wm_uart_reg_t *reg, uint8_t trigger_level)
{
    reg->fifo_ctrl.txfifo_trigger_level = trigger_level;
}

/**
 * @brief          Get tx fifo trigger level
 * @param[in]      reg Uart channel regsiter handle,type of  @arg wm_uart_reg_t *
 * @retval         Tx fifo trigger level
 */
ATTRIBUTE_INLINE uint8_t wm_ll_uart_get_txfifo_trigger_level(wm_uart_reg_t *reg)
{
    return (uint8_t)reg->fifo_ctrl.txfifo_trigger_level;
}

/**
 * @brief           Rx fifo trigger level
 *                  0: 1 byte
 *                  1: 4 bytes
 *                  2: 8 bytes
 *                  3: 16 bytes.
 * @param[in]      reg Uart channel regsiter handle,type of  @arg wm_uart_reg_t *
 * @param[in]      trigger_level  RTS trigger level
 */
ATTRIBUTE_INLINE void wm_ll_uart_set_rxfifo_trigger_level(wm_uart_reg_t *reg, uint8_t trigger_level)
{
    reg->fifo_ctrl.rxfifo_trigger_level = trigger_level;
}

/**
 * @brief          Get rx fifo trigger level
 * @param[in]      reg Uart channel regsiter handle,type of  @arg wm_uart_reg_t *
 * @retval         Rx fifo trigger level
 */
ATTRIBUTE_INLINE uint8_t wm_ll_uart_get_rxfifo_trigger_level(wm_uart_reg_t *reg)
{
    return (uint8_t)reg->fifo_ctrl.rxfifo_trigger_level;
}

/**
 * @brief          Set buadrate
 * @param[in]      reg Uart channel regsiter handle,type of  @arg wm_uart_reg_t *
 * @param[in]      baudrate  baudrate value
 */
ATTRIBUTE_INLINE void wm_ll_uart_set_baudrate(wm_uart_reg_t *reg, uint32_t baudrate_val)
{
    reg->baud_rate.val = baudrate_val;
}

/**
 * @brief          Get baudrate value
 * @param[in]      reg Uart channel regsiter handle,type of  @arg wm_uart_reg_t *
 * @retval         Baudrate value
 */
ATTRIBUTE_INLINE uint32_t wm_ll_uart_get_baudrate(wm_uart_reg_t *reg)
{
    return (uint32_t)reg->baud_rate.val;
}

/**
 * @brief          Set int mask
 * @param[in]      reg Uart channel regsiter handle,type of  @arg wm_uart_reg_t *
 * @param[in]      val  int mask
 */
ATTRIBUTE_INLINE void wm_ll_uart_set_int_mask(wm_uart_reg_t *reg, uint32_t val)
{
    reg->int_mask.val = val;
}

/**
 * @brief          Enable interrupt
 * @param[in]      reg Uart channel regsiter handle,type of  @arg wm_uart_reg_t *
 * @param[in]      mask  Mask bit
 */
ATTRIBUTE_INLINE void wm_ll_uart_enable_irq(wm_uart_reg_t *reg, wm_ll_uart_irq_mask_t mask)
{
    reg->int_mask.val &= ~mask;
}

/**
 * @brief          Disable interrupt
 * @param[in]      reg Uart channel regsiter handle,type of  @arg wm_uart_reg_t *
 * @param[in]      mask  Mask bit
 */
ATTRIBUTE_INLINE void wm_ll_uart_disable_irq(wm_uart_reg_t *reg, wm_ll_uart_irq_mask_t mask)
{
    reg->int_mask.val |= mask;
}

/**
 * @brief          Set int source
 * @param[in]      reg Uart channel regsiter handle,type of  @arg wm_uart_reg_t *
 * @param[in]      val  int source
 */
ATTRIBUTE_INLINE void wm_ll_uart_set_int_source(wm_uart_reg_t *reg, uint32_t val)
{
    reg->int_src.val = val;
}

/**
 * @brief          Detect interrupt source
 * @param[in]      reg Uart channel regsiter handle,type of  @arg wm_uart_reg_t *
 * @param[in]      mask  Mask bit
 * @retval         0     No interrupt happened
 * @retval         1     Interrupt happened
 */
ATTRIBUTE_INLINE bool wm_ll_uart_is_irq_from(wm_uart_reg_t *reg, wm_ll_uart_irq_mask_t mask)
{
    return (reg->int_src.val & mask) ? 1 : 0;
}

/**
 * @brief          Get interrupt source
 * @param[in]      reg Uart channel regsiter handle,type of  @arg wm_uart_reg_t *
 * @retval         Interrupt source
 */
ATTRIBUTE_INLINE uint32_t wm_ll_uart_get_irq_src(wm_uart_reg_t *reg)
{
    return reg->int_src.val;
}

/**
 * @brief          Set fifo status
 * @param[in]      reg Uart channel regsiter handle,type of  @arg wm_uart_reg_t *
 * @param[in]      val  fifo status
 */
ATTRIBUTE_INLINE void wm_ll_uart_set_fifo_status(wm_uart_reg_t *reg, uint32_t val)
{
    reg->fifo_status.val = val;
}

/**
 * @brief          Set tx fifo count
 * @param[in]      reg Uart channel regsiter handle,type of  @arg wm_uart_reg_t *
 * @param[in]      fifo_count  Fifo count
 */
ATTRIBUTE_INLINE void wm_ll_uart_set_txfifo_count(wm_uart_reg_t *reg, uint8_t fifo_count)
{
    reg->fifo_status.txfifo_count = fifo_count;
}

/**
 * @brief          Get tx fifo count
 * @param[in]      reg Uart channel regsiter handle,type of  @arg wm_uart_reg_t *
 * @retval         Tx fifo count
 */
ATTRIBUTE_INLINE uint8_t wm_ll_uart_get_txfifo_count(wm_uart_reg_t *reg)
{
    return (uint8_t)(reg->fifo_status.txfifo_count);
}

/**
 * @brief          Set rx fifo count
 * @param[in]      reg Uart channel regsiter handle,type of  @arg wm_uart_reg_t *
 * @param[in]      fifo_count  Fifo count
 */
ATTRIBUTE_INLINE void wm_ll_uart_set_rxfifo_count(wm_uart_reg_t *reg, uint8_t fifo_count)
{
    reg->fifo_status.rxfifo_count = fifo_count;
}

/**
 * @brief          Get rx fifo count
 * @param[in]      reg Uart channel regsiter handle,type of  @arg wm_uart_reg_t *
 * @retval         Rx fifo count
 */
ATTRIBUTE_INLINE uint8_t wm_ll_uart_get_rxfifo_count(wm_uart_reg_t *reg)
{
    return (uint8_t)(reg->fifo_status.rxfifo_count);
}

/**
 * @brief          putchar
 * @param[in]      reg Uart channel regsiter handle,type of  @arg wm_uart_reg_t *
 * @param[in]      ch charcter
 */
ATTRIBUTE_INLINE void wm_ll_uart_putchar(wm_uart_reg_t *reg, unsigned char ch)
{
    reg->tx_data_window = (unsigned char)ch;
}

/**
 * @brief          Get tx windowr address
 * @param[in]      reg Uart channel regsiter handle,type of  @arg wm_uart_reg_t *
 * @retval         Tx window addr
 */
ATTRIBUTE_INLINE void *wm_ll_uart_get_tx_window_addr(wm_uart_reg_t *reg)
{
    return (void *)&(reg->tx_data_window);
}

/**
 * @brief          getchar
 * @param[in]      reg Uart channel regsiter handle,type of  @arg wm_uart_reg_t *
 * @retval         character
 */
ATTRIBUTE_INLINE unsigned char wm_ll_uart_getchar(wm_uart_reg_t *reg)
{
    return (unsigned char)(reg->rx_data_window);
}

/**
 * @brief          Get rx window address
 * @param[in]      reg Uart channel regsiter handle,type of  @arg wm_uart_reg_t *
 * @retval         Rx window
 */
ATTRIBUTE_INLINE void *wm_ll_uart_get_rx_window_addr(wm_uart_reg_t *reg)
{
    return (void *)&(reg->rx_data_window);
}

/**
 * @brief          Enable CTS
 * @param[in]      reg Uart channel regsiter handle,type of  @arg wm_uart_reg_t *
 * @param[in]      en  Enable or not,type of  @arg bool
 */
ATTRIBUTE_INLINE void wm_ll_uart_enable_cts(wm_uart_reg_t *reg, bool en)
{
    reg->fifo_status.cts_status = en;
}

/**
 * @brief          Get CTS Enable or not
 * @param[in]      reg Uart channel regsiter handle,type of  @arg wm_uart_reg_t *
 * @retval         CTS enable or not
 */
ATTRIBUTE_INLINE bool wm_ll_uart_is_cts_enabled(wm_uart_reg_t *reg)
{
    return (bool)(reg->fifo_status.cts_status);
}

#ifdef __cplusplus
}
#endif

#endif
