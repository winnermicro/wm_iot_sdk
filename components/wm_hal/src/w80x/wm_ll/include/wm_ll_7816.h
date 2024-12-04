#ifndef __WM_LL_7816_H__
#define __WM_LL_7816_H__

#include "wm_types.h"
#include "wm_attr.h"
#include "wm_soc_cfgs.h"
#include "wm_reg_op.h"
#include "wm_ll_uart.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @enum    7816 clock out mode
 */
typedef enum {
    WM_LL_7816_CLOCK_OUT  = 0, /*< clock out   */
    WM_LL_7816_CLOCK_STOP = 1, /*< clock stop  */
} wm_ll_7816_clock_out_mode_t;

/**
 * @brief          Set 7816 protocol
 * @param[in]      reg Uart channel regsiter handle,type of  @arg wm_uart_reg_t *
 * @param[in]      protocol  7816 protocol,type of  @arg wm_7816_protocol_t
 */
ATTRIBUTE_INLINE void wm_ll_7816_set_protocol(wm_uart_reg_t *reg, wm_7816_protocol_t protocol)
{
    reg->line_ctrl.protocol = protocol;
}

/**
 * @brief          Get 7816 protocol
 * @param[in]      reg Uart channel regsiter handle,type of  @arg wm_uart_reg_t *
 * @retval         7816 protocol,type of  @arg wm_7816_protocol_t
 */
ATTRIBUTE_INLINE wm_7816_protocol_t wm_ll_7816_get_protocol(wm_uart_reg_t *reg)
{
    return (wm_7816_protocol_t)reg->line_ctrl.protocol;
}

/**
 * @brief          Set 7816 stop bit
 * @param[in]      reg Uart channel regsiter handle,type of  @arg wm_uart_reg_t *
 * @param[in]      bit Stop bit,type of  @arg wm_7816_stop_bits_t
 */
ATTRIBUTE_INLINE void wm_ll_7816_set_stop_bits(wm_uart_reg_t *reg, wm_7816_stop_bits_t bit)
{
    reg->line_ctrl.stop_bits = bit;
}

/**
 * @brief          Get 7816 stop bit
 * @param[in]      reg Uart channel regsiter handle,type of  @arg wm_uart_reg_t *
 * @retval         Stop bit,type of  @arg wm_7816_stop_bits_t
 */
ATTRIBUTE_INLINE wm_7816_stop_bits_t wm_ll_7816_get_stop_bits(wm_uart_reg_t *reg)
{
    return (wm_7816_stop_bits_t)reg->line_ctrl.stop_bits;
}

/**
 * @brief          Enable 7816 receive after parity error
 * @param[in]      reg Uart channel regsiter handle,type of  @arg wm_uart_reg_t *
 * @param[in]      en  Enable or not
 */
ATTRIBUTE_INLINE void wm_ll_7816_enable_parity_err_recv(wm_uart_reg_t *reg, bool en)
{
    reg->line_ctrl.parity_err_recv = en;
}

/**
 * @brief          Get parity error receive enable or not
 * @param[in]      reg Uart channel regsiter handle,type of  @arg wm_uart_reg_t *
 * @retval         Enable status
 */
ATTRIBUTE_INLINE bool wm_ll_7816_is_parity_err_recv_enabled(wm_uart_reg_t *reg)
{
    return (bool)reg->line_ctrl.parity_err_recv;
}

/**
 * @brief          Set 7816 transfer order
 * @param[in]      reg Uart channel regsiter handle,type of  @arg wm_uart_reg_t *
 * @param[in]      order 7816 transfer order,type of  @arg wm_7816_bits_order_t
 */
ATTRIBUTE_INLINE void wm_ll_7816_set_tranfer_order(wm_uart_reg_t *reg, wm_7816_bits_order_t order)
{
    reg->line_ctrl.parity = order;
}

/**
 * @brief          Get 7816 transfer order
 * @param[in]      reg Uart channel regsiter handle,type of  @arg wm_uart_reg_t *
 * @retval         Transfer order,type of  @arg wm_7816_bits_order_t
 */
ATTRIBUTE_INLINE wm_7816_bits_order_t wm_ll_7816_get_transfer_order(wm_uart_reg_t *reg)
{
    return (wm_7816_bits_order_t)reg->line_ctrl.parity;
}

/**
 * @brief          Set 7816 clock out mode
 * @param[in]      reg Uart channel regsiter handle,type of  @arg wm_uart_reg_t *
 * @param[in]      clock_mode  Clock out or not,type of  @arg wm_ll_7816_clock_out_mode_t
 */
ATTRIBUTE_INLINE void wm_ll_7816_set_clock_out_mode(wm_uart_reg_t *reg, wm_ll_7816_clock_out_mode_t clock_mode)
{
    /* [10] 0: Generate card clock output when configured in card mode
            1: Stop output clock*/
    reg->line_ctrl.clk_ctrl = clock_mode;
}

/**
 * @brief          Get 7816 clock out mode
 * @param[in]      reg Uart channel regsiter handle,type of  @arg wm_uart_reg_t *
 * @retval         Clock out mode,type of  @arg wm_ll_7816_clock_out_mode_t
 */
ATTRIBUTE_INLINE wm_ll_7816_clock_out_mode_t wm_ll_7816_get_clock_out_mode(wm_uart_reg_t *reg)
{
    return (wm_ll_7816_clock_out_mode_t)reg->line_ctrl.clk_ctrl;
}

/**
 * @brief          Set 7816 min block guard time
 * @param[in]      reg Uart channel regsiter handle,type of  @arg wm_uart_reg_t *
 * @param[in]      bgt  Block guard time
 */
ATTRIBUTE_INLINE void wm_ll_7816_set_min_bgt(wm_uart_reg_t *reg, uint8_t bgt)
{
    reg->line_ctrl.min_bgt = bgt;
}

/**
 * @brief          Get 7816 min block guard time
 * @param[in]      reg Uart channel regsiter handle,type of  @arg wm_uart_reg_t *
 * @retval         Block guard time
 */
ATTRIBUTE_INLINE uint8_t wm_ll_7816_get_min_bgt(wm_uart_reg_t *reg)
{
    return (uint8_t)reg->line_ctrl.min_bgt;
}

/**
 * @brief          Enable tx transfer retry
 * @param[in]      reg Uart channel regsiter handle,type of  @arg wm_uart_reg_t *
 * @param[in]      en  Enable or not
 */
ATTRIBUTE_INLINE void wm_ll_7816_enable_tx_retry_en(wm_uart_reg_t *reg, bool en)
{
    reg->line_ctrl.tx_retrans_en = en;
}

/**
 * @brief          Get tx retry enable or not
 * @param[in]      reg Uart channel regsiter handle,type of  @arg wm_uart_reg_t *
 * @retval         Enable status
 */
ATTRIBUTE_INLINE bool wm_ll_7816_is_tx_retry_enabled(wm_uart_reg_t *reg)
{
    return (bool)reg->line_ctrl.tx_retrans_en;
}

/**
 * @brief          Set rx transfer retry en
 * @param[in]      reg Uart channel regsiter handle,type of  @arg wm_uart_reg_t *
 * @param[in]      en  Enable or not
 */
ATTRIBUTE_INLINE void wm_ll_7816_enable_rx_retry(wm_uart_reg_t *reg, bool en)
{
    reg->line_ctrl.rx_retrans_en = en;
}

/**
 * @brief          Get rx retry enable or not
 * @param[in]      reg Uart channel regsiter handle,type of  @arg wm_uart_reg_t *
 * @retval         Enable status
 */
ATTRIBUTE_INLINE bool wm_ll_7816_is_rx_retry_enabled(wm_uart_reg_t *reg)
{
    return (bool)reg->line_ctrl.rx_retrans_en;
}

/**
 * @brief          Set tx transfer retry count
 * @param[in]      reg Uart channel regsiter handle,type of  @arg wm_uart_reg_t *
 * @param[in]      count  Retry count
 */
ATTRIBUTE_INLINE void wm_ll_7816_set_tx_retry_cnt(wm_uart_reg_t *reg, uint8_t count)
{
    reg->line_ctrl.tx_retrans_cnt = count;
}

/**
 * @brief          Get tx transfer retry count
 * @param[in]      reg Uart channel regsiter handle,type of  @arg wm_uart_reg_t *
 * @retval         Transfer retry count
 */
ATTRIBUTE_INLINE uint8_t wm_ll_7816_get_tx_retry_cnt(wm_uart_reg_t *reg)
{
    return (uint8_t)reg->line_ctrl.tx_retrans_cnt;
}

/**
 * @brief          Set rx transfer retry count
 * @param[in]      reg Uart channel regsiter handle,type of  @arg wm_uart_reg_t *
 * @param[in]      count  Retry count
 */
ATTRIBUTE_INLINE void wm_ll_7816_set_rx_retry_cnt(wm_uart_reg_t *reg, uint8_t count)
{
    reg->line_ctrl.rx_retrans_cnt = count;
}

/**
 * @brief          Get rx transfer retry count
 * @param[in]      reg Uart channel regsiter handle,type of  @arg wm_uart_reg_t *
 * @retval         Transfer retry count
 */
ATTRIBUTE_INLINE uint8_t wm_ll_7816_get_rx_retry_cnt(wm_uart_reg_t *reg)
{
    return (uint8_t)reg->line_ctrl.rx_retrans_cnt;
}

/**
 * @brief          Set guad time
 * @param[in]      reg Uart channel regsiter handle,type of  @arg wm_uart_reg_t *
 * @param[in]      guard_time  Guard time
 */
ATTRIBUTE_INLINE void wm_ll_7816_set_guard_time(wm_uart_reg_t *reg, uint8_t guard_time)
{
    WM_LL_REG_FILED_COPY_WRTIE(reg->guard_time, guard_time, guard_time);
}

/**
 * @brief          Get guard time
 * @param[in]      reg Uart channel regsiter handle,type of  @arg wm_uart_reg_t *
 * @retval         Guard time
 */
ATTRIBUTE_INLINE uint8_t wm_ll_7816_get_guard_time(wm_uart_reg_t *reg)
{
    return WM_LL_REG_FILED_COPY_READ(reg->guard_time, guard_time);
}

/**
 * @brief          Set rx receive wait time
 * @param[in]      reg Uart channel regsiter handle,type of  @arg wm_uart_reg_t *
 * @param[in]      wait_time  Rx receive wait time
 */
ATTRIBUTE_INLINE void wm_ll_7816_set_wait_time(wm_uart_reg_t *reg, uint32_t wait_time)
{
    reg->wait_time.wait_time = wait_time;
}

/**
 * @brief          Get rx receive wait time
 * @param[in]      reg Uart channel regsiter handle,type of  @arg wm_uart_reg_t *
 * @retval         Rx receive wait time
 */
ATTRIBUTE_INLINE uint32_t wm_ll_7816_get_wait_time(wm_uart_reg_t *reg)
{
    return (uint32_t)reg->wait_time.wait_time;
}

/**
 * @brief          Set etu
 * @param[in]      reg Uart channel regsiter handle,type of  @arg wm_uart_reg_t *
 * @param[in]      etu Etu
 */
ATTRIBUTE_INLINE void wm_ll_7816_set_etu(wm_uart_reg_t *reg, uint16_t etu)
{
    WM_LL_REG_FILED_COPY_WRTIE(reg->baud_rate, etu, etu);
}

/**
 * @brief          Get etu
 * @param[in]      reg Uart channel regsiter handle,type of  @arg wm_uart_reg_t *
 * @retval         Etu
 */
ATTRIBUTE_INLINE uint16_t wm_ll_7816_get_etu(wm_uart_reg_t *reg)
{
    return WM_LL_REG_FILED_COPY_READ(reg->baud_rate, etu);
}

/**
 * @brief          Set clock division
 * @param[in]      reg Uart channel regsiter handle,type of  @arg wm_uart_reg_t *
 * @param[in]      clock Division
 */
ATTRIBUTE_INLINE void wm_ll_7816_set_clk_division(wm_uart_reg_t *reg, uint8_t clk_div)
{
    reg->baud_rate.clk_div = clk_div;
}

/**
 * @brief          Get clock division
 * @param[in]      reg Uart channel regsiter handle,type of  @arg wm_uart_reg_t *
 * @retval         Clock division
 */
ATTRIBUTE_INLINE uint8_t wm_ll_7816_get_clk_division(wm_uart_reg_t *reg)
{
    return reg->baud_rate.clk_div;
}

/**
 * @brief          Enable 7816 mode
 * @param[in]      reg Uart channel regsiter handle,type of  @arg wm_uart_reg_t *
 * @param[in]      enable 7816 or uart mode, false: uart mode, true：7816 mode
 */
ATTRIBUTE_INLINE void wm_ll_7816_set_enable(wm_uart_reg_t *reg, bool en)
{
    reg->line_ctrl.mode = en;
}

/**
 * @brief          Get 7816 enable or not
 * @param[in]      reg Uart channel regsiter handle,type of  @arg wm_uart_reg_t *
 * @retval         Enable uart or 7816, true: 7816 mode, false: uart mode
 */
ATTRIBUTE_INLINE bool wm_ll_7816_get_enable(wm_uart_reg_t *reg)
{
    return (bool)reg->line_ctrl.mode;
}

#ifdef __cplusplus
}
#endif

#endif /* __WM_LL_7816_H__ */
