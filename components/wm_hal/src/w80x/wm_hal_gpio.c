/**
 * @file wm_hal_gpio.h
 *
 * @brief Gpio Hal Module
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
#include <string.h>
#include "wm_types.h"
#include "wm_error.h"
#include "wm_soc_cfgs.h"
#include "wm_irq.h"
#include "wm_hal_irq.h"
#include "wm_osal.h"
#include "wm_hal_gpio.h"

static uint8_t g_gpio_data_en = 0;

static void wm_gpio_isr_handler(wm_irq_no_t irq, void *arg)
{
    int gpio_start              = 0;
    int gpio_end                = 0;
    int i                       = 0;
    wm_hal_gpio_dev_t *gpio_hal = NULL;

    if (WM_IRQ_GPIOA == irq) {
#if CONFIG_BUILD_TYPE_W801S
        gpio_start = WM_GPIO_NUM_0;
        gpio_end   = WM_GPIO_NUM_15;
#elif CONFIG_BUILD_TYPE_W800
        gpio_start = WM_GPIO_NUM_0;
        gpio_end   = WM_GPIO_NUM_7;
#elif CONFIG_BUILD_TYPE_W803 || CONFIG_BUILD_TYPE_W802
        gpio_start = WM_GPIO_NUM_0;
        gpio_end   = WM_GPIO_NUM_14;
#elif CONFIG_BUILD_TYPE_W805
        gpio_start = WM_GPIO_NUM_1;
        gpio_end   = WM_GPIO_NUM_7;
#elif CONFIG_BUILD_TYPE_W806
        gpio_start = WM_GPIO_NUM_0;
        gpio_end   = WM_GPIO_NUM_15;
#else
        gpio_start = WM_GPIO_NUM_0;
        gpio_end   = WM_GPIO_NUM_15;
#endif
    } else if (WM_IRQ_GPIOB == irq) {
#if CONFIG_BUILD_TYPE_W801S
        gpio_start = WM_GPIO_NUM_16;
        gpio_end   = WM_GPIO_NUM_43;
#elif CONFIG_BUILD_TYPE_W800
        gpio_start = WM_GPIO_NUM_16;
        gpio_end   = WM_GPIO_NUM_36;
#elif CONFIG_BUILD_TYPE_W802
        gpio_start = WM_GPIO_NUM_16;
        gpio_end   = WM_GPIO_NUM_45;
#elif CONFIG_BUILD_TYPE_W803 || CONFIG_BUILD_TYPE_W805
        gpio_start = WM_GPIO_NUM_16;
        gpio_end   = WM_GPIO_NUM_36;
#elif CONFIG_BUILD_TYPE_W806
        gpio_start = WM_GPIO_NUM_16;
        gpio_end   = WM_GPIO_NUM_43;
#else
        gpio_start = WM_GPIO_NUM_16;
        gpio_end   = WM_GPIO_NUM_47;
#endif
    } else {
        return;
    }

    for (i = gpio_start; i <= gpio_end; i++) {
        if (wm_hal_gpio_get_irq_status(i)) {
            wm_hal_gpio_clr_irq_status(i);

            if (arg) {
                gpio_hal = (wm_hal_gpio_dev_t *)arg;

                if (gpio_hal->gpio_isr_func) {
                    if (gpio_hal->gpio_isr_func[i].callback) {
                        gpio_hal->gpio_isr_func[i].callback(gpio_hal->gpio_isr_func[i].args);
                    }
                }
            }
        }
    }
}

int wm_hal_gpio_init(wm_gpio_num_t pin, const wm_hal_gpio_config_t *cfg)
{
    wm_gpio_reg_t *gpio_reg = NULL;

    if (unlikely(!WM_GPIO_PIN_VALID(pin) || !cfg)) {
        return WM_ERR_INVALID_PARAM;
    }

    gpio_reg = g_gpio_map[pin].hw;
    WM_HAL_GPIO_NUM_COVERT(pin);

    //disable rw
    wm_ll_gpio_rw_disable(gpio_reg, pin);
    wm_ll_gpio_disable_af(gpio_reg, pin);

    //Set dir
    switch (cfg->dir) {
        case WM_GPIO_DIR_INPUT:
        {
            wm_ll_gpio_set_dir_input(gpio_reg, pin);
            break;
        }
        case WM_GPIO_DIR_OUTPUT:
        {
            wm_ll_gpio_set_dir_output(gpio_reg, pin);
            break;
        }
        default:
        {
            return WM_ERR_INVALID_PARAM;
        }
    }

    //Set pull up/pull down/pin float
    switch (cfg->pull_mode) {
        case WM_GPIO_FLOAT:
        {
            wm_ll_gpio_pulldown_disable(gpio_reg, pin);
            wm_ll_gpio_pullup_disable(gpio_reg, pin);
            break;
        }
        case WM_GPIO_PULL_UP:
        {
            wm_ll_gpio_pulldown_disable(gpio_reg, pin);
            wm_ll_gpio_pullup_enable(gpio_reg, pin);
            break;
        }
        case WM_GPIO_PULL_DOWN:
        {
            wm_ll_gpio_pullup_disable(gpio_reg, pin);
            wm_ll_gpio_pulldown_enable(gpio_reg, pin);
            break;
        }
        default:
        {
            return WM_ERR_INVALID_PARAM;
        }
    }

    // set intr mode
    if (cfg->intr_mode) {
        switch (cfg->intr_mode) {
            case WM_GPIO_IRQ_TRIG_FALLING_EDGE:
            {
                wm_ll_gpio_set_edge_trigger(gpio_reg, pin);
                wm_ll_gpio_set_single_edge(gpio_reg, pin);
                wm_ll_gpio_set_low_or_falling(gpio_reg, pin);
                wm_ll_gpio_enable_irq(gpio_reg, pin);
                break;
            }
            case WM_GPIO_IRQ_TRIG_RISING_EDGE:
            {
                wm_ll_gpio_set_edge_trigger(gpio_reg, pin);
                wm_ll_gpio_set_single_edge(gpio_reg, pin);
                wm_ll_gpio_set_high_or_rising(gpio_reg, pin);
                wm_ll_gpio_enable_irq(gpio_reg, pin);
                break;
            }
            case WM_GPIO_IRQ_TRIG_DOUBLE_EDGE:
            {
                wm_ll_gpio_set_edge_trigger(gpio_reg, pin);
                wm_ll_gpio_set_double_edge(gpio_reg, pin);
                wm_ll_gpio_enable_irq(gpio_reg, pin);
                break;
            }
            case WM_GPIO_IRQ_TRIG_LOW_LEVEL:
            {
                wm_ll_gpio_set_level_trigger(gpio_reg, pin);
                wm_ll_gpio_set_low_or_falling(gpio_reg, pin);
                wm_ll_gpio_enable_irq(gpio_reg, pin);
                break;
            }
            case WM_GPIO_IRQ_TRIG_HIGH_LEVEL:
            {
                wm_ll_gpio_set_level_trigger(gpio_reg, pin);
                wm_ll_gpio_set_high_or_rising(gpio_reg, pin);
                wm_ll_gpio_enable_irq(gpio_reg, pin);
                break;
            }
            default:
            {
                return WM_ERR_INVALID_PARAM;
            }
        }
    }

    wm_ll_gpio_rw_enable(gpio_reg, pin);

    return WM_ERR_SUCCESS;
}

int wm_hal_gpio_deinit(wm_gpio_num_t pin)
{
    wm_gpio_reg_t *gpio_reg = NULL;

    if (unlikely(!WM_GPIO_PIN_VALID(pin))) {
        return WM_ERR_INVALID_PARAM;
    }

    gpio_reg = g_gpio_map[pin].hw;
    WM_HAL_GPIO_NUM_COVERT(pin);

    //disable irq
    wm_ll_gpio_disable_irq(gpio_reg, pin);

    // set pin float
    wm_ll_gpio_pulldown_disable(gpio_reg, pin);
    wm_ll_gpio_pullup_disable(gpio_reg, pin);

    // set data_en disable
    wm_ll_gpio_rw_disable(gpio_reg, pin);

    // set disable pinmux
    wm_ll_gpio_disable_af(gpio_reg, pin);

    return WM_ERR_SUCCESS;
}

int wm_hal_gpio_set_dir(wm_gpio_num_t pin, wm_gpio_dir_t dir)
{
    wm_gpio_reg_t *gpio_reg = NULL;

    if (unlikely(!WM_GPIO_PIN_VALID(pin))) {
        return WM_ERR_INVALID_PARAM;
    }

    gpio_reg = g_gpio_map[pin].hw;
    WM_HAL_GPIO_NUM_COVERT(pin);

    switch (dir) {
        case WM_GPIO_DIR_INPUT:
        {
            wm_ll_gpio_set_dir_input(gpio_reg, pin);
            break;
        }
        case WM_GPIO_DIR_OUTPUT:
        {
            wm_ll_gpio_set_dir_output(gpio_reg, pin);
            break;
        }
        default:
        {
            return WM_ERR_INVALID_PARAM;
        }
    }

    wm_ll_gpio_rw_enable(gpio_reg, pin);

    return WM_ERR_SUCCESS;
}

int wm_hal_gpio_set_pullmode(wm_gpio_num_t pin, wm_gpio_pupd_t pullmode)
{
    wm_gpio_reg_t *gpio_reg = NULL;

    if (unlikely(!WM_GPIO_PIN_VALID(pin))) {
        return WM_ERR_INVALID_PARAM;
    }

    gpio_reg = g_gpio_map[pin].hw;
    WM_HAL_GPIO_NUM_COVERT(pin);

    switch (pullmode) {
        case WM_GPIO_FLOAT:
        {
            wm_ll_gpio_pulldown_disable(gpio_reg, pin);
            wm_ll_gpio_pullup_disable(gpio_reg, pin);
            break;
        }
        case WM_GPIO_PULL_UP:
        {
            wm_ll_gpio_pulldown_disable(gpio_reg, pin);
            wm_ll_gpio_pullup_enable(gpio_reg, pin);
            break;
        }
        case WM_GPIO_PULL_DOWN:
        {
            wm_ll_gpio_pullup_disable(gpio_reg, pin);
            wm_ll_gpio_pulldown_enable(gpio_reg, pin);
            break;
        }
        default:
        {
            return WM_ERR_INVALID_PARAM;
        }
    }

    wm_ll_gpio_clr_irq_status(gpio_reg, pin);
    return WM_ERR_SUCCESS;
}

int wm_hal_gpio_data_set(wm_gpio_num_t pin)
{
    if (unlikely(!WM_GPIO_PIN_VALID(pin))) {
        return WM_ERR_INVALID_PARAM;
    }

    if (g_gpio_data_en == 0) {
        g_gpio_data_en = 1;
        wm_ll_gpio_write_pin(g_gpio_map[pin].hw,
                             pin > WM_HAL_GPIO_COVERT_GROUPA_NUM ? pin - WM_HAL_GPIO_COVERT_GROUPA_NUM - 1 : pin, true);
        g_gpio_data_en = 0;
        return WM_ERR_SUCCESS;
    }

    return WM_ERR_FAILED;
}

int wm_hal_gpio_data_reset(wm_gpio_num_t pin)
{
    if (unlikely(!WM_GPIO_PIN_VALID(pin))) {
        return WM_ERR_INVALID_PARAM;
    }

    if (g_gpio_data_en == 0) {
        g_gpio_data_en = 1;
        wm_ll_gpio_write_pin(g_gpio_map[pin].hw,
                             pin > WM_HAL_GPIO_COVERT_GROUPA_NUM ? pin - WM_HAL_GPIO_COVERT_GROUPA_NUM - 1 : pin, false);
        g_gpio_data_en = 0;
        return WM_ERR_SUCCESS;
    }

    return WM_ERR_FAILED;
}

int wm_hal_gpio_data_get(wm_gpio_num_t pin)
{
    wm_gpio_reg_t *gpio_reg = NULL;
    int data                = 0;

    if (unlikely(!WM_GPIO_PIN_VALID(pin))) {
        return WM_ERR_INVALID_PARAM;
    }

    if (g_gpio_data_en == 0) {
        gpio_reg = g_gpio_map[pin].hw;
        WM_HAL_GPIO_NUM_COVERT(pin);

        data = wm_ll_gpio_read_pin(gpio_reg, pin);
        return data;
    }

    return WM_ERR_FAILED;
}

int wm_hal_gpio_iomux_func_sel(wm_gpio_num_t pin, int func)
{
    wm_gpio_reg_t *gpio_reg = NULL;

    if (unlikely(!WM_GPIO_PIN_VALID(pin))) {
        return WM_ERR_INVALID_PARAM;
    }

    gpio_reg = g_gpio_map[pin].hw;
    WM_HAL_GPIO_NUM_COVERT(pin);

    switch (func) {
        case WM_GPIO_IOMUX_FUN1:
        {
            wm_ll_gpio_enable_af(gpio_reg, pin);
            wm_ll_gpio_set_af1(gpio_reg, pin);
            wm_ll_gpio_pulldown_disable(gpio_reg, pin);
            wm_ll_gpio_pullup_disable(gpio_reg, pin);
            break;
        }
        case WM_GPIO_IOMUX_FUN2:
        {
            wm_ll_gpio_enable_af(gpio_reg, pin);
            wm_ll_gpio_set_af2(gpio_reg, pin);
            wm_ll_gpio_pulldown_disable(gpio_reg, pin);
            wm_ll_gpio_pullup_disable(gpio_reg, pin);
            break;
        }
        case WM_GPIO_IOMUX_FUN3:
        {
            wm_ll_gpio_enable_af(gpio_reg, pin);
            wm_ll_gpio_set_af3(gpio_reg, pin);
            wm_ll_gpio_pulldown_disable(gpio_reg, pin);
            wm_ll_gpio_pullup_disable(gpio_reg, pin);
            break;
        }
        case WM_GPIO_IOMUX_FUN4:
        {
            wm_ll_gpio_enable_af(gpio_reg, pin);
            wm_ll_gpio_set_af4(gpio_reg, pin);
            wm_ll_gpio_pulldown_disable(gpio_reg, pin);
            wm_ll_gpio_pullup_disable(gpio_reg, pin);
            break;
        }
        case WM_GPIO_IOMUX_FUN6:
        case WM_GPIO_IOMUX_FUN7:
        {
            wm_ll_gpio_set_dir_input(gpio_reg, pin);
            wm_ll_gpio_pulldown_disable(gpio_reg, pin);
            wm_ll_gpio_pullup_disable(gpio_reg, pin);
        }
        case WM_GPIO_IOMUX_FUN5:
        {
            wm_ll_gpio_disable_af(gpio_reg, pin);
            break;
        }
        default:
        {
            return WM_ERR_INVALID_PARAM;
        }
    }

    return WM_ERR_SUCCESS;
}

int wm_hal_gpio_get_irq_status(wm_gpio_num_t pin)
{
    wm_gpio_reg_t *gpio_reg = NULL;

    if (unlikely(!WM_GPIO_PIN_VALID(pin))) {
        return WM_ERR_INVALID_PARAM;
    }

    gpio_reg = g_gpio_map[pin].hw;
    WM_HAL_GPIO_NUM_COVERT(pin);

    return wm_ll_gpio_get_irq_status(gpio_reg, pin);
}

int wm_hal_gpio_clr_irq_status(uint32_t pin)
{
    wm_gpio_reg_t *gpio_reg = NULL;

    if (unlikely(!WM_GPIO_PIN_VALID(pin))) {
        return WM_ERR_INVALID_PARAM;
    }

    gpio_reg = g_gpio_map[pin].hw;
    WM_HAL_GPIO_NUM_COVERT(pin);

    wm_ll_gpio_clr_irq_status(gpio_reg, pin);

    return WM_ERR_SUCCESS;
}

int wm_hal_gpio_enable_irq(wm_gpio_num_t pin)
{
    wm_gpio_reg_t *gpio_reg = NULL;

    if (unlikely(!WM_GPIO_PIN_VALID(pin))) {
        return WM_ERR_INVALID_PARAM;
    }

    gpio_reg = g_gpio_map[pin].hw;
    WM_HAL_GPIO_NUM_COVERT(pin);

    wm_ll_gpio_clr_irq_status(gpio_reg, pin);
    wm_ll_gpio_enable_irq(gpio_reg, pin);

    return WM_ERR_SUCCESS;
}

int wm_hal_gpio_disable_irq(uint32_t pin)
{
    wm_gpio_reg_t *gpio_reg = NULL;

    if (unlikely(!WM_GPIO_PIN_VALID(pin))) {
        return WM_ERR_INVALID_PARAM;
    }

    gpio_reg = g_gpio_map[pin].hw;
    WM_HAL_GPIO_NUM_COVERT(pin);

    wm_ll_gpio_disable_irq(gpio_reg, pin);

    return WM_ERR_SUCCESS;
}

int wm_hal_gpio_pulldown_enable(uint32_t pin)
{
    wm_gpio_reg_t *gpio_reg = NULL;

    if (unlikely(!WM_GPIO_PIN_VALID(pin))) {
        return WM_ERR_INVALID_PARAM;
    }

    gpio_reg = g_gpio_map[pin].hw;
    WM_HAL_GPIO_NUM_COVERT(pin);

    wm_ll_gpio_pulldown_enable(gpio_reg, pin);
    return WM_ERR_SUCCESS;
}

int wm_hal_gpio_pulldown_disable(uint32_t pin)
{
    wm_gpio_reg_t *gpio_reg = NULL;

    if (unlikely(!WM_GPIO_PIN_VALID(pin))) {
        return WM_ERR_INVALID_PARAM;
    }

    gpio_reg = g_gpio_map[pin].hw;
    WM_HAL_GPIO_NUM_COVERT(pin);

    wm_ll_gpio_pulldown_disable(gpio_reg, pin);
    return WM_ERR_SUCCESS;
}

int wm_hal_gpio_pullup_enable(uint32_t pin)
{
    wm_gpio_reg_t *gpio_reg = NULL;

    if (unlikely(!WM_GPIO_PIN_VALID(pin))) {
        return WM_ERR_INVALID_PARAM;
    }

    gpio_reg = g_gpio_map[pin].hw;
    WM_HAL_GPIO_NUM_COVERT(pin);

    wm_ll_gpio_pullup_enable(gpio_reg, pin);
    return WM_ERR_SUCCESS;
}

int wm_hal_gpio_pullup_disable(uint32_t pin)
{
    wm_gpio_reg_t *gpio_reg = NULL;

    if (unlikely(!WM_GPIO_PIN_VALID(pin))) {
        return WM_ERR_INVALID_PARAM;
    }

    gpio_reg = g_gpio_map[pin].hw;
    WM_HAL_GPIO_NUM_COVERT(pin);

    wm_ll_gpio_pullup_disable(gpio_reg, pin);
    return WM_ERR_SUCCESS;
}

int wm_hal_gpio_set_intr_mode(uint32_t pin, wm_gpio_intr_mode_t mode)
{
    wm_gpio_reg_t *gpio_reg = NULL;

    if (unlikely(!WM_GPIO_PIN_VALID(pin))) {
        return WM_ERR_INVALID_PARAM;
    }

    gpio_reg = g_gpio_map[pin].hw;
    WM_HAL_GPIO_NUM_COVERT(pin);

    // set intr mode
    switch (mode) {
        case WM_GPIO_IRQ_TRIG_FALLING_EDGE:
        {
            wm_ll_gpio_set_edge_trigger(gpio_reg, pin);
            wm_ll_gpio_set_single_edge(gpio_reg, pin);
            wm_ll_gpio_set_low_or_falling(gpio_reg, pin);
            break;
        }
        case WM_GPIO_IRQ_TRIG_RISING_EDGE:
        {
            wm_ll_gpio_set_edge_trigger(gpio_reg, pin);
            wm_ll_gpio_set_single_edge(gpio_reg, pin);
            wm_ll_gpio_set_high_or_rising(gpio_reg, pin);
            break;
        }
        case WM_GPIO_IRQ_TRIG_DOUBLE_EDGE:
        {
            wm_ll_gpio_set_edge_trigger(gpio_reg, pin);
            wm_ll_gpio_set_double_edge(gpio_reg, pin);
            break;
        }
        case WM_GPIO_IRQ_TRIG_LOW_LEVEL:
        {
            wm_ll_gpio_set_level_trigger(gpio_reg, pin);
            wm_ll_gpio_set_low_or_falling(gpio_reg, pin);
            break;
        }
        case WM_GPIO_IRQ_TRIG_HIGH_LEVEL:
        {
            wm_ll_gpio_set_level_trigger(gpio_reg, pin);
            wm_ll_gpio_set_high_or_rising(gpio_reg, pin);
            break;
        }
        default:
        {
            return WM_ERR_INVALID_PARAM;
        }
    }

    return WM_ERR_SUCCESS;
}

int wm_hal_gpio_install_isr_service(wm_hal_gpio_dev_t *gpio_hal)
{
    int ret                               = WM_ERR_NOT_ALLOWED;
    wm_hal_gpio_isr_func_t *gpio_isr_func = NULL;

    if (likely(!gpio_hal->gpio_isr_func)) {
        gpio_isr_func = (wm_hal_gpio_isr_func_t *)wm_os_internal_malloc(WM_GPIO_NUM_MAX * sizeof(wm_hal_gpio_isr_func_t));
        if (!gpio_isr_func) {
            return WM_ERR_NO_MEM;
        }
        memset(gpio_isr_func, 0x00, WM_GPIO_NUM_MAX * sizeof(wm_hal_gpio_isr_func_t));

        gpio_hal->gpio_isr_func = gpio_isr_func;
        wm_hal_irq_attach_sw_vector(WM_IRQ_GPIOA, wm_gpio_isr_handler, gpio_hal);
        wm_hal_irq_attach_sw_vector(WM_IRQ_GPIOB, wm_gpio_isr_handler, gpio_hal);
        wm_hal_irq_enable(WM_IRQ_GPIOA);
        wm_hal_irq_enable(WM_IRQ_GPIOB);
        return WM_ERR_SUCCESS;
    }

    return ret;
}

int wm_hal_gpio_uninstall_isr_service(wm_hal_gpio_dev_t *gpio_hal)
{
    if (unlikely(!gpio_hal || !gpio_hal->gpio_isr_func)) {
        return WM_ERR_NOT_ALLOWED;
    }

    wm_hal_irq_disable(WM_IRQ_GPIOA);
    wm_hal_irq_disable(WM_IRQ_GPIOB);
    wm_hal_irq_detach_sw_vector(WM_IRQ_GPIOA);
    wm_hal_irq_detach_sw_vector(WM_IRQ_GPIOB);
    wm_os_internal_free(gpio_hal->gpio_isr_func);
    gpio_hal->gpio_isr_func = NULL;
    return WM_ERR_SUCCESS;
}

int wm_hal_gpio_add_isr_handler(wm_hal_gpio_dev_t *gpio_hal, wm_gpio_num_t gpio_num, wm_hal_gpio_isr_t isr_handler, void *args)
{
    if (unlikely(!WM_GPIO_PIN_VALID(gpio_num))) {
        return WM_ERR_INVALID_PARAM;
    }

    if (gpio_hal && gpio_hal->gpio_isr_func) {
        gpio_hal->gpio_isr_func[gpio_num].callback = isr_handler;
        gpio_hal->gpio_isr_func[gpio_num].args     = args;
    } else {
        return WM_ERR_NO_INITED;
    }

    return WM_ERR_SUCCESS;
}

int wm_hal_gpio_remove_isr_handler(wm_hal_gpio_dev_t *gpio_hal, wm_gpio_num_t gpio_num)
{
    if (unlikely(!WM_GPIO_PIN_VALID(gpio_num))) {
        return WM_ERR_INVALID_PARAM;
    }

    if (gpio_hal && gpio_hal->gpio_isr_func) {
        gpio_hal->gpio_isr_func[gpio_num].callback = NULL;
        gpio_hal->gpio_isr_func[gpio_num].args     = NULL;
    } else {
        return WM_ERR_NO_INITED;
    }

    return WM_ERR_SUCCESS;
}
