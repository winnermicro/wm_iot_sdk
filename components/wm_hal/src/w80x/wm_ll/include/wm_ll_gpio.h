/**
 * @file wm_ll_gpio.h
 *
 * @brief gpio ll api
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

#ifndef __WM_LL_GPIO_H__
#define __WM_LL_GPIO_H__

#include "wm_attr.h"
#include "wm_reg_op.h"
#include "wm_soc_cfgs.h"
#include "wm_gpio_reg_struct.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief          Get gpio interrupt status
 * 
 * @param[in]      hw Peripheral GPIO hardware instance address
 *                 hw is the type of @arg wm_gpio_reg_t
 *                 This parameter can be @arg @ref GPIOA or GPIOB
 * @param[in]      Pin is the type of @arg enum wm_gpio_num_t
 *                 Pin This parameter can be one of the following values
 *                 GPIOA: @arg @ref  GPIO_PIN_0 - GPIO_PIN_15
 *                 GPIOB: @arg @ref  GPIO_PIN_16 - GPIO_PIN_47
 * @retval         0     no interrupt happened
 * @retval         1     interrupt happened
 */
ATTRIBUTE_INLINE int wm_ll_gpio_get_irq_status(wm_gpio_reg_t *hw, wm_gpio_num_t pin)
{
    return ((WM_REG32_GET_BIT(&hw->mis, WM_BIT(pin))) ? 1 : 0);
}

/**
 * @brief          Clear gpio interrupt flag
 * @param[in]      hw Peripheral GPIO hardware instance address
 *                 hw is the type of @arg wm_gpio_reg_t
 *                 This parameter can be @arg @ref GPIOA or GPIOB
 * @param[in]      Pin is the type of @arg enum wm_gpio_num_t
 *                 Pin This parameter can be one of the following values
 *                 GPIOA: @arg @ref  GPIO_PIN_0 - GPIO_PIN_15
 *                 GPIOB: @arg @ref  GPIO_PIN_16 - GPIO_PIN_47
 */
ATTRIBUTE_INLINE void wm_ll_gpio_clr_irq_status(wm_gpio_reg_t *hw, wm_gpio_num_t pin)
{
    WM_REG32_SET_BIT(&hw->ic, WM_BIT(pin));
}

/**
 * @brief          Set interrupt edge trigger type
 * @param[in]      hw Peripheral GPIO hardware instance address
 *                 hw is the type of @arg wm_gpio_reg_t
 *                 This parameter can be @arg @ref GPIOA or GPIOB
 * @param[in]      Pin is the type of @arg enum wm_gpio_num_t
 *                 Pin This parameter can be one of the following values
 *                 GPIOA: @arg @ref  GPIO_PIN_0 - GPIO_PIN_15
 *                 GPIOB: @arg @ref  GPIO_PIN_16 - GPIO_PIN_47
 */
ATTRIBUTE_INLINE void wm_ll_gpio_set_edge_trigger(wm_gpio_reg_t *hw, wm_gpio_num_t pin)
{
    WM_REG32_CLR_BIT(&hw->is, WM_BIT(pin));
}

/**
 * @brief          Set interrupt level trigger type
 * @param[in]      hw Peripheral GPIO hardware instance address
 *                 hw is the type of @arg wm_gpio_reg_t
 *                 This parameter can be @arg @ref GPIOA or GPIOB
 * @param[in]      Pin is the type of @arg enum wm_gpio_num_t
 *                 Pin This parameter can be one of the following values
 *                 GPIOA: @arg @ref  GPIO_PIN_0 - GPIO_PIN_15
 *                 GPIOB: @arg @ref  GPIO_PIN_16 - GPIO_PIN_47
 */
ATTRIBUTE_INLINE void wm_ll_gpio_set_level_trigger(wm_gpio_reg_t *hw, wm_gpio_num_t pin)
{
    WM_REG32_SET_BIT(&hw->is, WM_BIT(pin));
}

/**
 * @brief          Set interrupt single edge trigger type
 * @param[in]      hw Peripheral GPIO hardware instance address
 *                 hw is the type of @arg wm_gpio_reg_t
 *                 This parameter can be @arg @ref GPIOA or GPIOB
 * @param[in]      Pin is the type of @arg enum wm_gpio_num_t
 *                 Pin This parameter can be one of the following values
 *                 GPIOA: @arg @ref  GPIO_PIN_0 - GPIO_PIN_15
 *                 GPIOB: @arg @ref  GPIO_PIN_16 - GPIO_PIN_47
 */
ATTRIBUTE_INLINE void wm_ll_gpio_set_single_edge(wm_gpio_reg_t *hw, wm_gpio_num_t pin)
{
    WM_REG32_CLR_BIT(&hw->ibe, WM_BIT(pin));
}

/**
 * @brief          GPIO set interrupt double edgetrigger type
 * @param[in]      hw Peripheral GPIO hardware instance address
 *                 hw is the type of @arg wm_gpio_reg_t
 *                 This parameter can be @arg @ref GPIOA or GPIOB
 * @param[in]      Pin is the type of @arg enum wm_gpio_num_t
 *                 Pin This parameter can be one of the following values
 *                 GPIOA: @arg @ref  GPIO_PIN_0 - GPIO_PIN_15
 *                 GPIOB: @arg @ref  GPIO_PIN_16 - GPIO_PIN_47
 */
ATTRIBUTE_INLINE void wm_ll_gpio_set_double_edge(wm_gpio_reg_t *hw, wm_gpio_num_t pin)
{
    WM_REG32_SET_BIT(&hw->ibe, WM_BIT(pin));
}

/**
 * @brief          Set interrupt high level or rising edge trigger type
 * @param[in]      hw Peripheral GPIO hardware instance address
 *                 hw is the type of @arg wm_gpio_reg_t
 *                 This parameter can be @arg @ref GPIOA or GPIOB
 * @param[in]      Pin is the type of @arg enum wm_gpio_num_t
 *                 Pin This parameter can be one of the following values
 *                 GPIOA: @arg @ref  GPIO_PIN_0 - GPIO_PIN_15
 *                 GPIOB: @arg @ref  GPIO_PIN_16 - GPIO_PIN_47
 */
ATTRIBUTE_INLINE void wm_ll_gpio_set_high_or_rising(wm_gpio_reg_t *hw, wm_gpio_num_t pin)
{
    WM_REG32_SET_BIT(&hw->iev, WM_BIT(pin));
}

/**
 * @brief          GPIO set interrupt low level or fallig edge trigger type
 * @param[in]      hw Peripheral GPIO hardware instance address
 *                 hw is the type of @arg wm_gpio_reg_t
 *                 This parameter can be @arg @ref GPIOA or GPIOB
 * @param[in]      Pin is the type of @arg enum wm_gpio_num_t
 *                 Pin This parameter can be one of the following values
 *                 GPIOA: @arg @ref  GPIO_PIN_0 - GPIO_PIN_15
 *                 GPIOB: @arg @ref  GPIO_PIN_16 - GPIO_PIN_47
 */
ATTRIBUTE_INLINE void wm_ll_gpio_set_low_or_falling(wm_gpio_reg_t *hw, wm_gpio_num_t pin)
{
    WM_REG32_CLR_BIT(&hw->iev, WM_BIT(pin));
}

/**
 * @brief          Enable gpio irq
 * @param[in]      hw Peripheral GPIO hardware instance address
 *                 hw is the type of @arg wm_gpio_reg_t
 *                 This parameter can be @arg @ref GPIOA or GPIOB
 * @param[in]      Pin is the type of @arg enum wm_gpio_num_t
 *                 Pin This parameter can be one of the following values
 *                 GPIOA: @arg @ref  GPIO_PIN_0 - GPIO_PIN_15
 *                 GPIOB: @arg @ref  GPIO_PIN_16 - GPIO_PIN_47
 */
ATTRIBUTE_INLINE void wm_ll_gpio_enable_irq(wm_gpio_reg_t *hw, wm_gpio_num_t pin)
{
    WM_REG32_SET_BIT(&hw->ie, WM_BIT(pin));
}

/**
 * @brief          Disable gpio irq
 * @param[in]      hw Peripheral GPIO hardware instance address
 *                 hw is the type of @arg wm_gpio_reg_t
 *                 This parameter can be @arg @ref GPIOA or GPIOB
 * @param[in]      Pin is the type of @arg enum wm_gpio_num_t
 *                 Pin This parameter can be one of the following values
 *                 GPIOA: @arg @ref  GPIO_PIN_0 - GPIO_PIN_15
 *                 GPIOB: @arg @ref  GPIO_PIN_16 - GPIO_PIN_47
 */
ATTRIBUTE_INLINE void wm_ll_gpio_disable_irq(wm_gpio_reg_t *hw, wm_gpio_num_t pin)
{
    WM_REG32_CLR_BIT(&hw->ie, WM_BIT(pin));
}

/**
 * @brief          Enable gpio af
 * @param[in]      hw Peripheral GPIO hardware instance address
 *                 hw is the type of @arg wm_gpio_reg_t
 *                 This parameter can be @arg @ref GPIOA or GPIOB
 * @param[in]      Pin is the type of @arg enum wm_gpio_num_t
 *                 Pin This parameter can be one of the following values
 *                 GPIOA: @arg @ref  GPIO_PIN_0 - GPIO_PIN_15
 *                 GPIOB: @arg @ref  GPIO_PIN_16 - GPIO_PIN_47
 */
ATTRIBUTE_INLINE void wm_ll_gpio_enable_af(wm_gpio_reg_t *hw, wm_gpio_num_t pin)
{
    WM_REG32_SET_BIT(&hw->af_sel, WM_BIT(pin));
}

/**
 * @brief          Disable gpio af
 * @param[in]      hw Peripheral GPIO hardware instance address
 *                 hw is the type of @arg wm_gpio_reg_t
 *                 This parameter can be @arg @ref GPIOA or GPIOB
 * @param[in]      Pin is the type of @arg enum wm_gpio_num_t
 *                 Pin This parameter can be one of the following values
 *                 GPIOA: @arg @ref  GPIO_PIN_0 - GPIO_PIN_15
 *                 GPIOB: @arg @ref  GPIO_PIN_16 - GPIO_PIN_47
 */
ATTRIBUTE_INLINE void wm_ll_gpio_disable_af(wm_gpio_reg_t *hw, wm_gpio_num_t pin)
{
    WM_REG32_CLR_BIT(&hw->af_sel, WM_BIT(pin));
}

/**
 * @brief          Select GPIO af1
 * @param[in]      hw Peripheral GPIO hardware instance address
 *                 hw is the type of @arg wm_gpio_reg_t
 *                 This parameter can be @arg @ref GPIOA or GPIOB
 * @param[in]      Pin is the type of @arg enum wm_gpio_num_t
 *                 Pin This parameter can be one of the following values
 *                 GPIOA: @arg @ref  GPIO_PIN_0 - GPIO_PIN_15
 *                 GPIOB: @arg @ref  GPIO_PIN_16 - GPIO_PIN_47
 */
ATTRIBUTE_INLINE void wm_ll_gpio_set_af1(wm_gpio_reg_t *hw, wm_gpio_num_t pin)
{
    WM_REG32_CLR_BIT(&hw->sf_s1, WM_BIT(pin));
    WM_REG32_CLR_BIT(&hw->af_s0, WM_BIT(pin));
}

/**
 * @brief          Select GPIO af2
 * @param[in]      hw Peripheral GPIO hardware instance address
 *                 hw is the type of @arg wm_gpio_reg_t
 *                 This parameter can be @arg @ref GPIOA or GPIOB
 * @param[in]      Pin is the type of @arg enum wm_gpio_num_t
 *                 Pin This parameter can be one of the following values
 *                 GPIOA: @arg @ref  GPIO_PIN_0 - GPIO_PIN_15
 *                 GPIOB: @arg @ref  GPIO_PIN_16 - GPIO_PIN_47
 * @return         None
 * @note           None
 */
ATTRIBUTE_INLINE void wm_ll_gpio_set_af2(wm_gpio_reg_t *hw, wm_gpio_num_t pin)
{
    WM_REG32_CLR_BIT(&hw->sf_s1, WM_BIT(pin));
    WM_REG32_SET_BIT(&hw->af_s0, WM_BIT(pin));
}

/**
 * @brief          Select GPIO af3
 * @param[in]      hw Peripheral GPIO hardware instance address
 *                 hw is the type of @arg wm_gpio_reg_t
 *                 This parameter can be @arg @ref GPIOA or GPIOB
 * @param[in]      Pin is the type of @arg enum wm_gpio_num_t
 *                 Pin This parameter can be one of the following values
 *                 GPIOA: @arg @ref  GPIO_PIN_0 - GPIO_PIN_15
 *                 GPIOB: @arg @ref  GPIO_PIN_16 - GPIO_PIN_47
 * @return         None
 * @note           None
 */
ATTRIBUTE_INLINE void wm_ll_gpio_set_af3(wm_gpio_reg_t *hw, wm_gpio_num_t pin)
{
    WM_REG32_SET_BIT(&hw->sf_s1, WM_BIT(pin));
    WM_REG32_CLR_BIT(&hw->af_s0, WM_BIT(pin));
}

/**
 * @brief          Select GPIO af4
 * @param[in]      hw Peripheral GPIO hardware instance address
 *                 hw is the type of @arg wm_gpio_reg_t
 *                 This parameter can be @arg @ref GPIOA or GPIOB
 * @param[in]      Pin is the type of @arg enum wm_gpio_num_t
 *                 Pin This parameter can be one of the following values
 *                 GPIOA: @arg @ref  GPIO_PIN_0 - GPIO_PIN_15
 *                 GPIOB: @arg @ref  GPIO_PIN_16 - GPIO_PIN_47
 * @return         None
 * @note           None
 */
ATTRIBUTE_INLINE void wm_ll_gpio_set_af4(wm_gpio_reg_t *hw, wm_gpio_num_t pin)
{
    WM_REG32_SET_BIT(&hw->sf_s1, WM_BIT(pin));
    WM_REG32_SET_BIT(&hw->af_s0, WM_BIT(pin));
}

/**
 * @brief          Enable GPIO Pull_up
 * @param[in]      hw Peripheral GPIO hardware instance address
 *                 hw is the type of @arg wm_gpio_reg_t
 *                 This parameter can be @arg @ref GPIOA or GPIOB
 * @param[in]      Pin is the type of @arg enum wm_gpio_num_t
 *                 Pin This parameter can be one of the following values
 *                 GPIOA: @arg @ref  GPIO_PIN_0 - GPIO_PIN_15
 *                 GPIOB: @arg @ref  GPIO_PIN_16 - GPIO_PIN_47
 */
ATTRIBUTE_INLINE void wm_ll_gpio_pullup_enable(wm_gpio_reg_t *hw, wm_gpio_num_t pin)
{
    WM_REG32_CLR_BIT(&hw->pull_en, WM_BIT(pin));
}

/**
 * @brief          Disable GPIO Pull_up
 * @param[in]      hw Peripheral GPIO hardware instance address
 *                 hw is the type of @arg wm_gpio_reg_t
 *                 This parameter can be @arg @ref GPIOA or GPIOB
 * @param[in]      Pin is the type of @arg enum GPIO_PIN
 *                 Pin This parameter can be one of the following values
 *                 GPIOA: @arg @ref  GPIO_PIN_0 - GPIO_PIN_15
 *                 GPIOB: @arg @ref  GPIO_PIN_16 - GPIO_PIN_47
 */
ATTRIBUTE_INLINE void wm_ll_gpio_pullup_disable(wm_gpio_reg_t *hw, wm_gpio_num_t pin)
{
    WM_REG32_SET_BIT(&hw->pull_en, WM_BIT(pin));
}

/**
 * @brief          Enable GPIO Pull_down
 * @param[in]      hw Peripheral GPIO hardware instance address
 *                 hw is the type of @arg wm_gpio_reg_t
 *                 This parameter can be @arg @ref GPIOA or GPIOB
 * @param[in]      Pin is the type of @arg enum wm_gpio_num_t
 *                 Pin This parameter can be one of the following values
 *                 GPIOA: @arg @ref  GPIO_PIN_0 - GPIO_PIN_15
 *                 GPIOB: @arg @ref  GPIO_PIN_16 - GPIO_PIN_47
 */
ATTRIBUTE_INLINE void wm_ll_gpio_pulldown_enable(wm_gpio_reg_t *hw, wm_gpio_num_t pin)
{
    WM_REG32_SET_BIT(&hw->dn_ena, WM_BIT(pin));
}

/**
 * @brief          Disable GPIO Pull_down
 * @param[in]      hw Peripheral GPIO hardware instance address
 *                 hw is the type of @arg wm_gpio_reg_t
 *                 This parameter can be @arg @ref GPIOA or GPIOB
 * @param[in]      Pin is the type of @arg enum wm_gpio_num_t
 *                 Pin This parameter can be one of the following values
 *                 GPIOA: @arg @ref  GPIO_PIN_0 - GPIO_PIN_15
 *                 GPIOB: @arg @ref  GPIO_PIN_16 - GPIO_PIN_47
 */
ATTRIBUTE_INLINE void wm_ll_gpio_pulldown_disable(wm_gpio_reg_t *hw, wm_gpio_num_t pin)
{
    WM_REG32_CLR_BIT(&hw->dn_ena, WM_BIT(pin));
}

/**
 * @brief          Set the InputDir of GPIO
 * @param[in]      hw Peripheral GPIO hardware instance address
 *                 hw is the type of @arg wm_gpio_reg_t
 *                 This parameter can be @arg @ref GPIOA or GPIOB
 * @param[in]      Pin is the type of @arg enum wm_gpio_num_t
 *                 Pin This parameter can be one of the following values
 *                 GPIOA: @arg @ref  GPIO_PIN_0 - GPIO_PIN_15
 *                 GPIOB: @arg @ref  GPIO_PIN_16 - GPIO_PIN_47
 */
ATTRIBUTE_INLINE void wm_ll_gpio_set_dir_input(wm_gpio_reg_t *hw, wm_gpio_num_t pin)
{
    WM_REG32_CLR_BIT(&hw->dir, WM_BIT(pin));
}

/**
 * @brief          Set the OutputDir of GPIO
 * @param[in]      hw Peripheral GPIO hardware instance address
 *                 hw is the type of @arg wm_gpio_reg_t
 *                 This parameter can be @arg @ref GPIOA or GPIOB
 * @param[in]      Pin is the type of @arg enum wm_gpio_num_t
 *                 Pin This parameter can be one of the following values
 *                 GPIOA: @arg @ref  GPIO_PIN_0 - GPIO_PIN_15
 *                 GPIOB: @arg @ref  GPIO_PIN_16 - GPIO_PIN_47
 */
ATTRIBUTE_INLINE void wm_ll_gpio_set_dir_output(wm_gpio_reg_t *hw, wm_gpio_num_t pin)
{
    WM_REG32_SET_BIT(&hw->dir, WM_BIT(pin));
}

/**
 * @brief          Enable the W/R of GPIO Pin
 * @param[in]      hw Peripheral GPIO hardware instance address
 *                 hw is the type of @arg wm_gpio_reg_t
 *                 This parameter can be @arg @ref GPIOA or GPIOB
 * @param[in]      Pin is the type of @arg enum wm_gpio_num_t
 *                 Pin This parameter can be one of the following values
 *                 GPIOA: @arg @ref  GPIO_PIN_0 - GPIO_PIN_15
 *                 GPIOB: @arg @ref  GPIO_PIN_16 - GPIO_PIN_47
 */
ATTRIBUTE_INLINE void wm_ll_gpio_rw_enable(wm_gpio_reg_t *hw, wm_gpio_num_t pin)
{
    WM_REG32_SET_BIT(&hw->data_en, WM_BIT(pin));
}

/**
 * @brief          Disable the W/R of GPIO Pin
 * @param[in]      hw Peripheral GPIO hardware instance address
 *                 hw is the type of @arg wm_gpio_reg_t
 *                 This parameter can be @arg @ref GPIOA or GPIOB
 * @param[in]      Pin is the type of @arg enum GPIO_PIN
 *                 Pin This parameter can be one of the following values
 *                 GPIOA: @arg @ref  GPIO_PIN_0 - GPIO_PIN_15
 *                 GPIOB: @arg @ref  GPIO_PIN_16 - GPIO_PIN_47
 */
ATTRIBUTE_INLINE void wm_ll_gpio_rw_disable(wm_gpio_reg_t *hw, wm_gpio_num_t pin)
{
    WM_REG32_CLR_BIT(&hw->data_en, WM_BIT(pin));
}

/**
 * @brief          Get the W/R of GPIO Pin is enable
 * @param[in]      hw Peripheral GPIO hardware instance address
 *                 hw is the type of @arg wm_gpio_reg_t
 *                 This parameter can be @arg @ref GPIOA or GPIOB
 * @param[in]      Pin is the type of @arg enum GPIO_PIN
 *                 Pin This parameter can be one of the following values
 *                 GPIOA: @arg @ref  GPIO_PIN_0 - GPIO_PIN_15
 *                 GPIOB: @arg @ref  GPIO_PIN_16 - GPIO_PIN_47
 */
ATTRIBUTE_INLINE int wm_ll_gpio_rw_isenable(wm_gpio_reg_t *hw, wm_gpio_num_t pin)
{
    return (WM_REG32_GET_BIT(&hw->data_en, WM_BIT(pin)) > 0) ? 1 : 0;
}

/**
 * @brief          Read the Value of GPIO Pin
 * @param[in]      hw Peripheral GPIO hardware instance address
 *                 hw is the type of @arg wm_gpio_reg_t
 *                 This parameter can be @arg @ref GPIOA or GPIOB
 * @param[in]      Pin is the type of @arg enum wm_gpio_num_t
 *                 Pin This parameter can be one of the following values
 *                 GPIOA: @arg @ref  GPIO_PIN_0 - GPIO_PIN_15
 *                 GPIOB: @arg @ref  GPIO_PIN_16 - GPIO_PIN_47
 * @return         the Value of GPIO Pin
 * @note           None
 */
ATTRIBUTE_INLINE int wm_ll_gpio_read_pin(wm_gpio_reg_t *hw, wm_gpio_num_t pin)
{
    return (WM_REG32_GET_BIT(&hw->data, WM_BIT(pin)) ? 1 : 0);
}

/**
 * @brief          Write Value to GPIO Pin
 * @param[in]      hw Peripheral GPIO hardware instance address
 *                 hw is the type of @arg wm_gpio_reg_t
 *                 This parameter can be @arg @ref GPIOA or GPIOB
 * @param[in]      Pin is the type of @arg enum wm_gpio_num_t
 *                 Pin This parameter can be one of the following values
 *                 GPIOA: @arg @ref  GPIO_PIN_0 - GPIO_PIN_15
 *                 GPIOB: @arg @ref  GPIO_PIN_16 - GPIO_PIN_47
 * @param[in]      Value is the type of @arg bool
 *                 @arg @ref GPIO_PIN_RESET
 *                 @arg @ref GPIO_PIN_SET
 */
ATTRIBUTE_INLINE void wm_ll_gpio_write_pin(wm_gpio_reg_t *hw, wm_gpio_num_t pin, bool value)
{
    WM_REG32_WRITE(&hw->data, (WM_REG32_READ(&hw->data) & ~(1 << pin)) | (value << pin));
}

#ifdef __cplusplus
}
#endif

#endif /* __WM_LL_GPIO_H__ */
