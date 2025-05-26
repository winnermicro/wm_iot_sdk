/**
 * @file wm_drv_gpio.h
 *
 * @brief gpio drv api
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

#ifndef __WM_DRV_GPIO_H__
#define __WM_DRV_GPIO_H__

#include "wm_types.h"
#include "wm_osal.h"
#include "wm_soc_cfgs.h"
#include "wm_dt.h"
#include "wm_drv_irq.h"
#include "wm_hal_gpio.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup WM_GPIO_TYPEs WM DRV GPIO TYPEs
 * @brief WinnerMicro DRV GPIO TYPEs
 */

/**
 * @addtogroup WM_GPIO_TYPEs
 * @{
 */

/**
 * @typedef wm_drv_gpio_isr_t
 * 
 * @brief GPIO interrupt handler
 *
 * @param arg User registered data
 */
typedef void (*wm_drv_gpio_isr_t)(void *arg);

/**
 * @}
 */

/**
 * @defgroup WM_GPIO_MACROs WM GPIO Macros
 * @brief WinnerMicro GPIO Macros
 */

/**
 * @addtogroup WM_GPIO_MACROs
 * @{
 */
/**
 * @brief Fast write gpio with value 0 or 1, Please note that it is not thread safe
 */
#define wm_drv_gpio_fast_write(pin, value) WM_HAL_GPIO_FAST_WRITE(pin, value)

/**
 * @brief Fast read gpio value, return 0 or 1
 */
#define wm_drv_gpio_fast_read(pin)         WM_HAL_GPIO_FAST_READ(pin)

/**
 * @brief Fast set gpio direction, WM_GPIO_DIR_INPUT or WM_GPIO_DIR_OUTPUT
 */
#define wm_drv_gpio_fast_set_dir(pin, dir) WM_HAL_GPIO_FAST_SET_DIR(pin, dir)

/**
 * @}
 */

/**
 * @defgroup WM_DRV_GPIO_APIs WM DRV GPIO APIs
 * @brief WinnerMicro DRV GPIO APIs
 */

/**
 * @addtogroup WM_DRV_GPIO_APIs
 * @{
 */

/**
 * @brief Initialize gpio dev, and configure gpio based on the device tree.
 *
 * @param [in] dev_name like "gpio", if device tree set
 * @return
 *    - device handle: device handle, used @ref wm_device_t
 *    - NULL: failed
 */
wm_device_t *wm_drv_gpio_init(char *dev_name);

/**
 * @brief Deinitialize gpio dev.
 *
 * @param [in] dev use @arg wm_device_t
 * @return
 *    - WM_ERR_SUCCESS: succeed
 *    - others: failed
 */
int wm_drv_gpio_deinit(wm_device_t *dev);

/**
 * @brief Set gpio dir.
 *
 * @param [in] pin use @arg wm_gpio_pin_num_t
 * @param [in] dir use @arg wm_gpio_dir_t
 * @return
 *    - WM_ERR_SUCCESS: succeed
 *    - others: failed
 */
int wm_drv_gpio_set_dir(wm_gpio_num_t pin, wm_gpio_dir_t dir);

/**
 * @brief Set gpio pullmode.
 *
 * @param [in]  pin use @arg wm_gpio_pin_num_t
 * @param [in]  pullmode use @arg wm_gpio_pupd_t
 * @return
 *    - WM_ERR_SUCCESS: succeed
 *    - others: failed
 */
int wm_drv_gpio_set_pullmode(wm_gpio_num_t pin, wm_gpio_pupd_t pullmode);

/**
 * @brief Write gpio with value 1.
 *
 * @param [in] pin use @arg wm_gpio_num_t
 * @return
 *    - WM_ERR_SUCCESS: succeed
 *    - others: failed 
 * @warning You should use this API after calling wm_drv_gpio_init.
 */
int wm_drv_gpio_data_set(wm_gpio_num_t pin);

/**
 * @brief Write gpio with value 0.
 *
 * @param [in] pin use @arg wm_gpio_num_t
 * @return
 *    - WM_ERR_SUCCESS: succeed
 *    - others: failed 
 * @warning You should use this API after calling wm_drv_gpio_init.
 */
int wm_drv_gpio_data_reset(wm_gpio_num_t pin);

/**
 * @brief Read gpio.
 *
 * @param [in] pin use @arg wm_gpio_num_t
 * @return
 *    - gpio value: 1 or 0
 *    - others: failed
 * @warning You should use this API after calling wm_drv_gpio_init. 
 */
int wm_drv_gpio_data_get(wm_gpio_num_t pin);

/**
 * @brief Select iomux. please read wm_io_mux.h for pinmux cfg
 *
 * @param [in] pin use @arg wm_gpio_num_t
 * @param [in] func use @arg wm_gpio_pin_mux_t
 * @return
 *    - WM_ERR_SUCCESS: succeed
 *    - others: failed
 */
int wm_drv_gpio_iomux_func_sel(wm_gpio_num_t pin, wm_gpio_pin_mux_t func);

/**
 * @brief Enable gpio irq.
 *
 * @param [in] pin use @arg wm_gpio_num_t
 * @return
 *    - WM_ERR_SUCCESS: succeed
 *    - others: failed
 */
int wm_drv_gpio_enable_isr(wm_gpio_num_t pin);

/**
 * @brief Disable gpio irq.
 *
 * @param [in] pin use @arg wm_gpio_num_t
 * @return
 *    - WM_ERR_SUCCESS: succeed
 *    - others: failed
 */
int wm_drv_gpio_disable_isr(wm_gpio_num_t pin);

/**
 * @brief Set gpio intr mode.
 *
 * @param [in] pin use @arg wm_gpio_num_t 
 * @param [in] mode use @arg wm_gpio_intr_mode_t 
 * @return
 *    - WM_ERR_SUCCESS: succeed
 *    - others: failed
 */
int wm_drv_gpio_set_intr_mode(wm_gpio_num_t pin, wm_gpio_intr_mode_t mode);

/**
  * @brief Add ISR handler for the corresponding GPIO pin.
  *
  * @param gpio_num GPIO number, If this pin is not set in the device tree, you need to initialize it separately
  * @param isr_callback ISR handler function for the corresponding GPIO number.
  * @param args parameter for ISR handler.
  *
  * @return
  *    - WM_ERR_SUCCESS: succeed
  *    - others: failed  
  * @warning You should use this API after calling wm_drv_gpio_init.
  */
int wm_drv_gpio_add_isr_callback(wm_gpio_num_t gpio_num, wm_drv_gpio_isr_t isr_callback, void *args);

/**
  * @brief Remove ISR handler for the corresponding GPIO pin.
  *
  * @param gpio_num GPIO number
  *
  * @return
  *    - WM_ERR_SUCCESS: succeed
  *    - others: failed  
  * @warning You should use this API after calling wm_drv_gpio_init.
  */
int wm_drv_gpio_remove_isr_callback(wm_gpio_num_t gpio_num);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* __WM_DRV_GPIO_H__ */
