/**
 * @file wm_hal_rcc.h
 *
 * @brief hall rcc Module
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

#ifndef WM_HAL_RCC_H
#define WM_HAL_RCC_H

#ifdef __cplusplus
extern "C" {
#endif

#include "wm_soc_cfgs.h"

typedef struct {
    uint32_t reg_base; //init paramter, rcc reaister base address
} wm_hal_clock_dev_t;

void wm_hal_rcc_set_i2s_clock_config(wm_hal_clock_dev_t *hal_dev, bool extal_en, bool mclk_en, uint32_t mclk_div,
                                     uint32_t bclk_div);

/**
 * @brief Set the rcc clock enable
 *
 * @param[in] hal_dev  RCC device hal layer pointer
 * @param[in] module_type config the indicate module clock enable, please refer to #wm_rcc_clock_enable_t
 *
 *
 */
void wm_hal_clock_enable(wm_hal_clock_dev_t *hal_dev, uint32_t module_type);

/**
 * @brief Set the rcc clock disable
 *
 * @param[in] hal_dev  RCC device hal layer pointer
 * @param[in] module_type config the indicate module clock disable, please refer to #wm_rcc_clock_enable_t
 *
 *
 */
void wm_hal_clock_disable(wm_hal_clock_dev_t *hal_dev, uint32_t module_type);

/**
 * @brief Set the rcc clock reset
 *
 * @param[in] hal_dev  RCC device hal layer pointer
 * @param[in] module_type config the indicate module clock reset, please refer to #wm_rcc_clk_rst_t
 *
 *
 */
void wm_hal_clock_reset(wm_hal_clock_dev_t *hal_dev, uint32_t module_type);

/**
 * @brief Clock initial
 * @param[in] hal_dev  RCC device hal layer pointer
 *
 */
void wm_hal_clock_init(wm_hal_clock_dev_t *hal_dev);

/**
 * @brief Clock deinitial
 * @param[in] hal_dev  RCC device hal layer pointer
 *
 */
void wm_hal_clock_deinit(wm_hal_clock_dev_t *hal_dev);

/**
 * @brief Set the rcc clock
 * @param[in] hal_dev  RCC device hal layer pointer
 * @param[in] module_type config the indicate module clock, please refer to #wm_rcc_type_t
 * @param[in] clock the clock setting, unit: MHz
 *
 * @note The Periphal/WLAN/CPU clock should be a divider of PLL clock(480MHz), others value is invalid;
 *       The SD ADC clock should be a divider of 40MHz;
 *       The wlan clock should be 160MHz to work normal;
 *       The CPU clock should above 40MHz to make wifi work, and maxmum clock is 240MHz;
 *       The QFLASH clock should be 40MHz or 80MHz;
 *       The GPSEC clock should be 80MHz or 160MHz;
 *       The RSA clock should be 80MHz or 160MHz;
 *       No need to config APB clock, used for query;
 */
int32_t wm_hal_rcc_config_clock(wm_hal_clock_dev_t *hal_dev, wm_rcc_type_t module_type, uint8_t clock);

/**
 * @brief Get the rcc clock
 *
 * @param[in] hal_dev  RCC device hal layer pointer
 * @param[in] module_type config the clock module, please refer to #wm_rcc_type_t
 *
 * @return the current clock setting, unit: MHz
 */
uint8_t wm_hal_rcc_get_clock_config(wm_hal_clock_dev_t *hal_dev, wm_rcc_type_t module_type);

#ifdef __cplusplus
}
#endif

#endif /* WM_HAL_RCC_H */
