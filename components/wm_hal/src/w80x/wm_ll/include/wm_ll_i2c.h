/**
 * @file wm_ll_i2c.h
 *
 * @brief ll i2c Module
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

#ifndef __WM_LL_I2C_H__
#define __WM_LL_I2C_H__

#include "wm_regs_base.h"
#include "wm_types.h"
#include "wm_attr.h"
#include "wm_i2c_reg_struct.h"
#include "wm_reg_op.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Tthe CR_SR register is used as control and status register, the following macro define control the bits mask.
 * @note As control register, some bits must set at the same time, as state register, can get state by bit.
 */
#define WM_LL_I2C_CR_STA  (0x1UL << 7) /**< ctrl start         */
#define WM_LL_I2C_CR_STO  (0x1UL << 6) /**< ctrl stop          */
#define WM_LL_I2C_CR_RD   (0x1UL << 5) /**< ctrl read          */
#define WM_LL_I2C_CR_WR   (0x1UL << 4) /**< ctrl write         */
#define WM_LL_I2C_CR_NAK  (0x1UL << 3) /**< ctrl reply ack/nak */
#define WM_LL_I2C_CR_IACK (0x1UL)      /**< ctrl iack          */

/**
 * @brief Set the i2c clock divider of low 8 bits
 * @param[in] i2c_dev the I2C register structure
 *
 * @param[in] div_low the clock divider setting for low 8 bits
 *
 */
ATTRIBUTE_INLINE void wm_ll_i2c_set_clock_div_low(wm_i2c_reg_t *i2c_dev, uint8_t div_low)
{
    WM_LL_REG_FILED_COPY_WRTIE(i2c_dev->low8bit, clk_div, div_low);
}

/**
 * @brief Get the i2c clock divider of low 8 bits
 * @param[in] i2c_dev the I2C register structure
 *
 */
ATTRIBUTE_INLINE uint8_t wm_ll_i2c_get_clock_div_low(wm_i2c_reg_t *i2c_dev)
{
    return WM_LL_REG_FILED_COPY_READ(i2c_dev->low8bit, clk_div);
}

/**
 * @brief Set the i2c clock divider of high 8 bits
 * @param[in] i2c_dev the I2C register structure
 *
 * @param[in] div_low the clock divider setting for high 8 bits
 *
 */
ATTRIBUTE_INLINE void wm_ll_i2c_set_clock_div_high(wm_i2c_reg_t *i2c_dev, uint8_t div_high)
{
    WM_LL_REG_FILED_COPY_WRTIE(i2c_dev->high8bit, clk_div, div_high);
}

/**
 * @brief Get the i2c clock divider of high 8 bits
 * @param[in] i2c_dev the I2C register structure
 *
 */
ATTRIBUTE_INLINE uint8_t wm_ll_i2c_get_clock_div_high(wm_i2c_reg_t *i2c_dev)
{
    return WM_LL_REG_FILED_COPY_READ(i2c_dev->high8bit, clk_div);
}

/**
 * @brief    Configure the I2C enable/disable
 * @param[in]  i2c_dev the I2C register structure
 *
 * @param[in] enable
 *              [0] disable I2C
 *              [1] enable I2C
 *
 */
ATTRIBUTE_INLINE void wm_ll_i2c_set_enable(wm_i2c_reg_t *i2c_dev, uint8_t enable)
{
    i2c_dev->ctrl.ctrl_enable = enable;
}

/**
 * @brief    Configure the I2C IRQ mask
 * @param[in]  i2c_dev the I2C register structure
 *
 */
ATTRIBUTE_INLINE void wm_ll_i2c_enable_int(wm_i2c_reg_t *i2c_dev)
{
    i2c_dev->ctrl.irq_mask = 0;
}

/**
 * @brief    Configure the I2C IRQ mask
 * @param[in]  i2c_dev the I2C register structure
 *
 */
ATTRIBUTE_INLINE void wm_ll_i2c_disable_int(wm_i2c_reg_t *i2c_dev)
{
    i2c_dev->ctrl.irq_mask = 1;
}

/**
 * @brief    Send one byte data
 * @param[in]   i2c_dev the I2C register structure
 *
 * @param[in]   data The data to be send
 *
 */
ATTRIBUTE_INLINE void wm_ll_i2c_send_one_byte(wm_i2c_reg_t *i2c_dev, uint8_t data)
{
    i2c_dev->data.val = data;
}

/**
 * @brief    Receive one byte data
 * @param[in]   i2c_dev the I2C register structure
 *
 * @param[out]  data The data to be receieved
 *
 */
ATTRIBUTE_INLINE uint8_t wm_ll_i2c_recieve_one_byte(wm_i2c_reg_t *i2c_dev)
{
    return (uint8_t)(i2c_dev->data.val);
}

/**
 * @brief    set control
 * @param[in]   i2c_dev the I2C register structure
 * @param[in]   val control value
 */
ATTRIBUTE_INLINE void wm_ll_i2c_set_ctrl(wm_i2c_reg_t *i2c_dev, uint32_t val)
{
    i2c_dev->ctrl_state.val = val;
}

/**
 * @brief Get the i2c clock divider of high 8 bits
 * @param[in] i2c_dev the I2C register structure
 *
 */
ATTRIBUTE_INLINE uint32_t wm_ll_i2c_get_state(wm_i2c_reg_t *i2c_dev)
{
    return i2c_dev->ctrl_state.val;
}

/**
 * @brief   Get interrupt state
 * @param[in]   i2c_dev the I2C register structure
 *
 * @return   0: no interrupt
 *           1: interrupt occurred
 */
ATTRIBUTE_INLINE uint8_t wm_ll_i2c_get_it_flag(wm_i2c_reg_t *i2c_dev)
{
    return i2c_dev->ctrl_state.ifb;
}

/**
 * @brief   Get transfer in progress state
 * @param[in]   i2c_dev the I2C register structure
 *
 * @return   0: not in transfer progress
 *           1: in transfer progress
 */
ATTRIBUTE_INLINE uint8_t wm_ll_i2c_is_in_xfer(wm_i2c_reg_t *i2c_dev)
{
    return i2c_dev->ctrl_state.tip;
}

/**
 * @brief   Get Arbitration Lost status, reserved
 * @param[in]   i2c_dev the I2C register structure
 *
 * @return   reserved
 *
 */
ATTRIBUTE_INLINE uint8_t wm_ll_i2c_get_state_arbitration_lost(wm_i2c_reg_t *i2c_dev)
{
    return i2c_dev->ctrl_state.al;
}

/**
 * @brief   Get busy status
 * @param[in]   i2c_dev the I2C register structure
 *
 * @return   0: idle
 *           1: busy
 */
ATTRIBUTE_INLINE uint8_t wm_ll_i2c_get_state_busy(wm_i2c_reg_t *i2c_dev)
{
    return i2c_dev->ctrl_state.busy;
}

/**
 * @brief   Get rx ack/nack
 * @param[in]   i2c_dev the I2C register structure
 *
 * @return   0: get ack from slave
 *           1: get nack from slave
 *
 */
ATTRIBUTE_INLINE uint8_t wm_ll_i2c_get_state_rx_ack(wm_i2c_reg_t *i2c_dev)
{
    return i2c_dev->ctrl_state.rx_ack;
}

/**
 * @brief   Record the setting value in TXR_RXR register
 * @param[in]   i2c_dev the I2C register structure
 *
 * @return   the data write in i2c_dev->data
 *
 */
ATTRIBUTE_INLINE uint8_t wm_ll_i2c_get_data_ro(wm_i2c_reg_t *i2c_dev)
{
    return WM_LL_REG_FILED_COPY_READ(i2c_dev->tx_data_ro, data_ro);
}

/**
 * @brief   Record the setting value in CR_SR register
 * @param[in]   i2c_dev the I2C register structure
 *
 * @return    the data write in i2c_dev->ctrl_state.ctrl.data
 *
 */
ATTRIBUTE_INLINE uint8_t wm_ll_i2c_get_ctrl_state_ro(wm_i2c_reg_t *i2c_dev)
{
    return WM_LL_REG_FILED_COPY_READ(i2c_dev->ctrl_state_ro, state_ro);
}

#ifdef __cplusplus
}
#endif

#endif /* __WM_LL_I2C_H__ */
