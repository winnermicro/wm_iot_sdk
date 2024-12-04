/**
 * @file wm_hal_rcc.c
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

#include "wm_rcc_reg_struct.h"
#include "wm_regs_base.h"
#include "wm_ll_rcc.h"
#include "wm_hal_rcc.h"
#include "wm_soc_cfgs.h"
#include "wm_error.h"

void wm_hal_rcc_set_i2s_clock_config(wm_hal_clock_dev_t *hal_dev, bool extal_en, bool mclk_en, uint32_t mclk_div,
                                     uint32_t bclk_div)
{
    wm_rcc_reg_t *reg = (wm_rcc_reg_t *)hal_dev->reg_base;
    wm_ll_rcc_set_i2s_clock_config(reg, extal_en, mclk_en, mclk_div, bclk_div);
}

/**
 * @brief Set the rcc clock enable
 * @param[in] module_type config the indicate module clock enable, please refer to #wm_rcc_clock_enable_t
 * @Usage wm_hal_clock_enable(WM_RCC_UART0_GATE_EN | WM_RCC_UART1_GATE_EN | WM_RCC_RF_CFG_GATE_EN);
 *
 */
void wm_hal_clock_enable(wm_hal_clock_dev_t *hal_dev, uint32_t module_type)
{
    uint32_t clock_en;
    wm_rcc_reg_t *rcc_reg = (wm_rcc_reg_t *)hal_dev->reg_base;

    clock_en = wm_ll_rcc_get_clock_enable(rcc_reg);
    wm_ll_rcc_set_clock_enable(rcc_reg, module_type | clock_en);
}

/**
 * @brief Set the rcc clock disable
 * @param[in] module_type config the indicate module clock disable, please refer to #wm_rcc_clock_enable_t
 * @Usage wm_hal_clock_disable(WM_RCC_UART0_GATE_EN | WM_RCC_UART1_GATE_EN | WM_RCC_RF_CFG_GATE_EN);
 *
 */
void wm_hal_clock_disable(wm_hal_clock_dev_t *hal_dev, uint32_t module_type)
{
    uint32_t clock_en;
    wm_rcc_reg_t *rcc_reg = (wm_rcc_reg_t *)hal_dev->reg_base;

    clock_en = wm_ll_rcc_get_clock_enable(rcc_reg);
    wm_ll_rcc_set_clock_enable(rcc_reg, clock_en & (~module_type));
}

/**
 * @brief Set the rcc clock reset
 * @param[in] module_type config the indicate module clock reset, please refer to #wm_rcc_clk_rst_t
 * @Usage wm_hal_clock_reset(WM_RCC_BBP_RST | WM_RCC_MAC_RST);
 *
 */
void wm_hal_clock_reset(wm_hal_clock_dev_t *hal_dev, uint32_t module_type)
{
    wm_rcc_reg_t *rcc_reg = (wm_rcc_reg_t *)hal_dev->reg_base;

    wm_ll_rcc_set_clock_reset(rcc_reg, ~module_type);
    //delay?
    wm_ll_rcc_set_clock_reset(rcc_reg, 0xffffffff);
}

/**
 * @brief Clock initial
 *
 */
void wm_hal_clock_init(wm_hal_clock_dev_t *hal_dev)
{
    //dummy
}

/**
 * @brief Clock deinitial
 *
 */
void wm_hal_clock_deinit(wm_hal_clock_dev_t *hal_dev)
{
    //dummy
}

/**
 * @brief Set the rcc clock
 * @param[in] module_type config the indicate module clock, please refer to #wm_rcc_type_t
 * @param[in] clock the clock setting, unit: MHz
 * @note The Periphal/WLAN/CPU clock should be a divider of PLL clock(480MHz), others value is invalid;
 *       The SD ADC clock should be a divider of 40MHz;
 *       The wlan clock should be 160MHz to work normal;
 *       The CPU clock should above 40MHz to make wifi work, and maxmum clock is 240MHz;
 *       The QFLASH clock should be 40MHz or 80MHz;
 *       The GPSEC clock should be 80MHz or 160MHz;
 *       The RSA clock should be 80MHz or 160MHz;
 *       No need to config APB clock, used for query;
 */
int32_t wm_hal_rcc_config_clock(wm_hal_clock_dev_t *hal_dev, wm_rcc_type_t module_type, uint8_t clock)
{
    uint8_t clock_div;
    wm_rcc_reg_t *rcc_reg = (wm_rcc_reg_t *)hal_dev->reg_base;

    if (WM_PLL_CLOCK % clock != 0) {
        return WM_ERR_INVALID_PARAM;
    }
    switch (module_type) {
        case WM_RCC_TYPE_PERIPHERAL:
        {
            clock_div = WM_PLL_CLOCK / clock;
            wm_ll_rcc_set_clock_div_freq_en(rcc_reg, false);
            wm_ll_rcc_set_peripheral_clock_div(rcc_reg, clock_div);
            wm_ll_rcc_set_clock_div_freq_en(rcc_reg, true);
            break;
        }
        case WM_RCC_TYPE_WLAN:
        {
            clock_div = WM_PLL_CLOCK / clock;
            wm_ll_rcc_set_clock_div_freq_en(rcc_reg, false);
            wm_ll_rcc_set_wlan_clock_div(rcc_reg, clock_div);
            if (wm_ll_rcc_get_cpu_clock_div(rcc_reg) > 12) {
                wm_ll_rcc_set_bus_syncdn_factor(rcc_reg, 1);
            } else {
                wm_ll_rcc_set_bus_syncdn_factor(rcc_reg, clock_div * 4 / wm_ll_rcc_get_cpu_clock_div(rcc_reg));
            }
            wm_ll_rcc_set_clock_div_freq_en(rcc_reg, true);
            break;
        }
        case WM_RCC_TYPE_CPU:
        {
            clock_div = WM_PLL_CLOCK / clock;
            wm_ll_rcc_set_clock_div_freq_en(rcc_reg, false);
            wm_ll_rcc_set_cpu_clock_div(rcc_reg, clock_div);
            if (clock_div > 12) {
                wm_ll_rcc_set_bus_syncdn_factor(rcc_reg, 1);
            } else {
                wm_ll_rcc_set_bus_syncdn_factor(rcc_reg, wm_ll_rcc_get_wlan_clock_div(rcc_reg) * 4 / clock_div);
            }
            wm_ll_rcc_set_clock_div_freq_en(rcc_reg, true);
            break;
        }
        case WM_RCC_TYPE_SD_ADC:
        {
            if ((WM_PLL_CLOCK / 12) % clock != 0) {
                return WM_ERR_INVALID_PARAM;
            }
            clock_div = (WM_PLL_CLOCK / 12) / clock;
            wm_ll_rcc_set_clock_div_freq_en(rcc_reg, false);
            wm_ll_rcc_set_sd_adc_clock_div(rcc_reg, clock_div);
            wm_ll_rcc_set_clock_div_freq_en(rcc_reg, true);
            break;
        }
        case WM_RCC_TYPE_QFLASH:
        {
            if (clock == 40) {
                wm_ll_rcc_set_qflash_clock_selection(rcc_reg, 0);
            } else if (clock == 80) {
                wm_ll_rcc_set_qflash_clock_selection(rcc_reg, 1);
            } else {
                return WM_ERR_INVALID_PARAM;
            }
            break;
        }
        case WM_RCC_TYPE_GPSEC:
        {
            if (clock == 80) {
                wm_ll_rcc_set_gpsec_clock_selection(rcc_reg, 0);
            } else if (clock == 160) {
                wm_ll_rcc_set_gpsec_clock_selection(rcc_reg, 1);
            } else {
                return WM_ERR_INVALID_PARAM;
            }
            break;
        }
        case WM_RCC_TYPE_RSA:
        {
            if (clock == 80) {
                wm_ll_rcc_set_rsa_clock_selection(rcc_reg, 0);
            } else if (clock == 160) {
                wm_ll_rcc_set_rsa_clock_selection(rcc_reg, 1);
            } else {
                return WM_ERR_INVALID_PARAM;
            }
            break;
        }
        case WM_RCC_TYPE_APB:
        case WM_RCC_TYPE_MAX:
        {
            return WM_ERR_INVALID_PARAM;
        }
    }

    return 0;
}

/**
 * @brief Get the rcc clock
 * @param[in] module_type config the clock module, please refer to #wm_rcc_type_t
 * @return the current clock setting, unit: MHz
 */
uint8_t wm_hal_rcc_get_clock_config(wm_hal_clock_dev_t *hal_dev, wm_rcc_type_t module_type)
{
    uint8_t clock_div, clock = 0;
    wm_rcc_reg_t *rcc_reg = (wm_rcc_reg_t *)hal_dev->reg_base;

    switch (module_type) {
        case WM_RCC_TYPE_PERIPHERAL:
        {
            clock_div = wm_ll_rcc_get_peripheral_clock_div(rcc_reg);
            clock     = WM_PLL_CLOCK / clock_div;
            break;
        }
        case WM_RCC_TYPE_WLAN:
        {
            clock_div = wm_ll_rcc_get_wlan_clock_div(rcc_reg);
            clock     = WM_PLL_CLOCK / clock_div;
            break;
        }
        case WM_RCC_TYPE_CPU:
        {
            clock_div = wm_ll_rcc_get_cpu_clock_div(rcc_reg);
            clock     = WM_PLL_CLOCK / clock_div;
            break;
        }
        case WM_RCC_TYPE_SD_ADC:
        {
            clock_div = wm_ll_rcc_get_sd_adc_clock_div(rcc_reg);
            clock     = (WM_PLL_CLOCK / 12) / clock_div;
            break;
        }
        case WM_RCC_TYPE_QFLASH:
        {
            if (wm_ll_rcc_get_qflash_clock_selection(rcc_reg) == 0) {
                clock = 40;
            } else if (wm_ll_rcc_get_qflash_clock_selection(rcc_reg) == 1) {
                clock = 80;
            }
            break;
        }
        case WM_RCC_TYPE_GPSEC:
        {
            if (wm_ll_rcc_get_gpsec_clock_selection(rcc_reg) == 0) {
                clock = 80;
            } else if (wm_ll_rcc_get_gpsec_clock_selection(rcc_reg) == 1) {
                clock = 160;
            }
            break;
        }
        case WM_RCC_TYPE_RSA:
        {
            if (wm_ll_rcc_get_rsa_clock_selection(rcc_reg) == 0) {
                clock = 80;
            } else if (wm_ll_rcc_get_rsa_clock_selection(rcc_reg) == 1) {
                clock = 160;
            }
            break;
        }
        case WM_RCC_TYPE_APB:
        {
            clock = (WM_PLL_CLOCK / wm_ll_rcc_get_cpu_clock_div(rcc_reg)) / wm_ll_rcc_get_bus_syncdn_factor(rcc_reg);
            break;
        }
        case WM_RCC_TYPE_MAX:
        {
            return WM_ERR_INVALID_PARAM;
        }
    }
    return clock;
}
