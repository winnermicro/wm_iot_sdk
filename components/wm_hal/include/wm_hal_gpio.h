/**
 * @file wm_hal_gpio.h
 *
 * @brief gpio hal api
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

#ifndef __WM_HAL_GPIO_H__
#define __WM_HAL_GPIO_H__

#include "wm_ll_gpio.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief GPIO interrupt handler
 *
 * @param arg User registered data
 */
typedef void (*wm_hal_gpio_isr_t)(void *arg);

typedef struct {
    wm_gpio_dir_t dir;
    wm_gpio_pupd_t pull_mode;
    wm_gpio_intr_mode_t intr_mode;
} wm_hal_gpio_config_t;

typedef struct {
    wm_hal_gpio_isr_t callback; /*!< isr function */
    void *args;                 /*!< isr function args */
} wm_hal_gpio_isr_func_t;

typedef struct {
    wm_hal_gpio_isr_func_t *gpio_isr_func;
} wm_hal_gpio_dev_t;

#define WM_HAL_GPIO_COVERT_GROUPA_NUM 15
#define WM_HAL_GPIO_NUM_COVERT(pin)   (pin = pin > WM_HAL_GPIO_COVERT_GROUPA_NUM ? pin - WM_HAL_GPIO_COVERT_GROUPA_NUM - 1 : pin)

#define WM_HAL_GPIO_FAST_WRITE(pin, value)              \
    do {                                                \
        wm_gpio_reg_t *gpio_reg = NULL;                 \
        int __tmp               = 0;                    \
        __tmp                   = pin;                  \
        gpio_reg                = g_gpio_map[__tmp].hw; \
        WM_HAL_GPIO_NUM_COVERT(__tmp);                  \
        wm_ll_gpio_write_pin(gpio_reg, __tmp, value);   \
    } while (0)
/**
 * @brief Initialize gpio pin.
 *
 * @param [in] pin use @ref wm_gpio_num_t
 * @param [in] cfg use @ref wm_hal_gpio_config_t *
 * @return
 *    - WM_ERR_SUCCESS: succeed
 *    - others: failed
 */
int wm_hal_gpio_init(wm_gpio_num_t pin, const wm_hal_gpio_config_t *cfg);

/**
 * @brief Deinitialize gpio pin.
 *
 * @param [in] pin use @ref wm_gpio_num_t
 * @return
 *    - WM_ERR_SUCCESS: succeed
 *    - others: failed
 */
int wm_hal_gpio_deinit(wm_gpio_num_t pin);

/**
 * @brief Install the GPIO driver's ISR handler service, which allows per-pin GPIO interrupt handlers
 * 
 * @param [in] gpio_hal use @ref wm_hal_gpio_dev_t *
 * @return
 *    - WM_ERR_SUCCESS: succeed
 *    - others: failed
 */
int wm_hal_gpio_install_isr_service(wm_hal_gpio_dev_t *gpio_hal);

/**
 * @brief Uninstall the driver's GPIO ISR service, freeing related resources.
 * 
 * @param [in] gpio_hal use @ref wm_hal_gpio_dev_t *
 * @return
 *    - WM_ERR_SUCCESS: succeed
 *    - others: failed
 */
int wm_hal_gpio_uninstall_isr_service(wm_hal_gpio_dev_t *gpio_hal);

/**
 * @brief Add ISR handler for the corresponding GPIO pin.
 *
 * Call this function after using wm_hal_gpio_install_isr_service() to
 * install the driver's GPIO ISR handler service.
 * 
 * @param [in] gpio_hal use @ref wm_hal_gpio_dev_t*
 * @param [in] gpio_num use @ref wm_gpio_num_t
 * @param [in] isr_handler use wm_hal_gpio_isr_t
 * @param [in] args User data to pass to the ISR handler
 *
 * @return
 *    - WM_ERR_SUCCESS: succeed
 *    - others: failed
 */
int wm_hal_gpio_add_isr_handler(wm_hal_gpio_dev_t *gpio_hal, wm_gpio_num_t gpio_num, wm_hal_gpio_isr_t isr_handler, void *args);

/**
 * @brief Remove ISR handler for the corresponding GPIO pin.
 * 
 * @param [in] gpio_hal use @ref wm_hal_gpio_dev_t*
 * @param [in] gpio_num use @ref wm_gpio_num_t
 *
 * @return
 *    - WM_ERR_SUCCESS: succeed
 *    - others: failed
 */
int wm_hal_gpio_remove_isr_handler(wm_hal_gpio_dev_t *gpio_hal, wm_gpio_num_t gpio_num);

/**
 * @brief Set gpio dir.
 *
 * @param [in] pin use @ref wm_gpio_num_t
 * @param [in] dir use @ref wm_gpio_dir_t
 * @return
 *    - WM_ERR_SUCCESS: succeed
 *    - others: failed
 */
int wm_hal_gpio_set_dir(wm_gpio_num_t pin, wm_gpio_dir_t dir);

/**
 * @brief Set gpio pullmode.
 *
 * @param [in] pin use @ref wm_gpio_num_t
 * @param [in] pullmode use @ref wm_gpio_pupd_t
 * @return
 *    - WM_ERR_SUCCESS: succeed
 *    - others: failed
 */
int wm_hal_gpio_set_pullmode(wm_gpio_num_t pin, wm_gpio_pupd_t pullmode);

/**
 * @brief Write gpio with value 1.
 *
 * @param [in] pin use @ref wm_gpio_num_t
 * @return
 *    - WM_ERR_SUCCESS: succeed
 *    - others: failed
 */
int wm_hal_gpio_data_set(wm_gpio_num_t pin);

/**
 * @brief Write gpio with value 0.
 *
 * @param [in] pin use @ref wm_gpio_num_t
 * @return
 *    - WM_ERR_SUCCESS: succeed
 *    - others: failed
 */
int wm_hal_gpio_data_reset(wm_gpio_num_t pin);

/**
 * @brief Read gpio.
 *
 * @param [in] pin use @ref wm_gpio_num_t
 * @return
 *    - gpio value: 1 or 0
 *    - others: failed
 */
int wm_hal_gpio_data_get(wm_gpio_num_t pin);

/**
 * @brief Select iomux. please read wm_io_mux.h for pinmux cfg
 *
 * @param [in] pin use @ref wm_gpio_num_t
 * @param [in] func
 * @return
 *    - WM_ERR_SUCCESS: succeed
 *    - others: failed
 */
int wm_hal_gpio_iomux_func_sel(wm_gpio_num_t pin, int func);

/**
 * @brief Get irq status.
 *
 * @param [in] pin use @ref wm_gpio_num_t
 * @return
 *    - 1: irq occurs
 *    - 0: irq did not occurs
 *    - others: failed
 */
int wm_hal_gpio_get_irq_status(wm_gpio_num_t pin);

/**
 * @brief Clear irq status.
 *
 * @param [in] pin use @ref wm_gpio_num_t
 * @return
 *    - WM_ERR_SUCCESS: succeed
 *    - others: failed
 */
int wm_hal_gpio_clr_irq_status(uint32_t pin);

/**
 * @brief Enable gpio irq.
 *
 * @param [in] pin use @ref wm_gpio_num_t
 * @return
 *    - WM_ERR_SUCCESS: succeed
 *    - others: failed
 */
int wm_hal_gpio_enable_irq(wm_gpio_num_t pin);

/**
 * @brief Disable gpio irq.
 *
 * @param [in] pin use @ref wm_gpio_num_t
 * @return
 *    - WM_ERR_SUCCESS: succeed
 *    - others: failed
 */
int wm_hal_gpio_disable_irq(uint32_t pin);

/**
 * @brief Set gpio intr mode.
 *
 * @param [in] pin use @ref wm_gpio_num_t 
 * @param [in] mode use @ref wm_gpio_intr_mode_t
 * @return
 *    - WM_ERR_SUCCESS: succeed
 *    - others: failed
 */
int wm_hal_gpio_set_intr_mode(uint32_t pin, wm_gpio_intr_mode_t mode);

#ifdef __cplusplus
}
#endif

#endif /* __WM_HAL_GPIO_H__ */
