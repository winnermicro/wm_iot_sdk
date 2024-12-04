/**
 * @file wm_ll_rcc.h
 *
 * @brief ll rcc Module
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

#ifndef __WM_LL_RCC_H__
#define __WM_LL_RCC_H__

#include "wm_regs_base.h"
#include "wm_types.h"
#include "wm_attr.h"
#include "wm_rcc_reg_struct.h"
#include "wm_reg_op.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Set the rcc clock enable
 * @param[in] rcc_dev the rcc register structure
 *
 * @param[in] clk_en the clock enable/disable setting
 *
 */
ATTRIBUTE_INLINE void wm_ll_rcc_set_clock_enable(wm_rcc_reg_t *rcc_dev, uint32_t clk_en)
{
    rcc_dev->clk_en.val = clk_en;
}

/**
 * @brief Get the rcc clock setting
 * @param[in] rcc_dev the rcc register structure
 *
 * @return the current clock setting
 */
ATTRIBUTE_INLINE uint32_t wm_ll_rcc_get_clock_enable(wm_rcc_reg_t *rcc_dev)
{
    return rcc_dev->clk_en.val;
}

/**
 * @brief Set the rcc clock gate mask
 * @param[in] rcc_dev the rcc register structure
 *
 * @param[in] clk_msk the clock gate mask setting
 *
 */
ATTRIBUTE_INLINE void wm_ll_rcc_set_clock_gate_mask(wm_rcc_reg_t *rcc_dev, uint32_t clk_msk)
{
    rcc_dev->clk_msk.val = clk_msk;
}

/**
 * @brief Get the rcc clock gate mask setting
 * @param[in] rcc_dev the rcc register structure
 *
 * @return the current clock gate mask setting
 */
ATTRIBUTE_INLINE uint32_t wm_ll_rcc_get_clock_gate_mask(wm_rcc_reg_t *rcc_dev)
{
    return rcc_dev->clk_msk.val;
}

/**
 * @brief Set the rcc pmu clock gate mask
 * @param[in] rcc_dev the rcc register structure
 *
 * @param[in] pmu_msk the pmu clock gate mask setting
 *
 */
ATTRIBUTE_INLINE void wm_ll_rcc_set_pmu_clock_gate_mask(wm_rcc_reg_t *rcc_dev, bool pmu_msk)
{
    rcc_dev->clk_msk.pmu_clk_gate_msk = pmu_msk;
}

/**
 * @brief Set the rcc sdio ahb clock gate mask
 * @param[in] rcc_dev the rcc register structure
 *
 * @param[in] pmu_msk the adio ahb clock gate mask setting
 *
 */
ATTRIBUTE_INLINE void wm_ll_rcc_set_sdio_ahb_clock_gate_mask(wm_rcc_reg_t *rcc_dev, bool sdio_msk)
{
    rcc_dev->clk_msk.sdio_ahb_gate_msk = sdio_msk;
}

/**
 * @brief Set the rcc cpu clock gate mask
 * @param[in] rcc_dev the rcc register structure
 *
 * @param[in] pmu_msk the cpu clock gate mask setting
 *
 */
ATTRIBUTE_INLINE void wm_ll_rcc_set_cpu_clock_gate_mask(wm_rcc_reg_t *rcc_dev, bool cpu_msk)
{
    rcc_dev->clk_msk.cpu_clk_gate_msk = cpu_msk;
}

/**
 * @brief Set the rcc clock reset
 * @param[in] rcc_dev the rcc register structure
 *
 * @param[in] clk_rst the clock reset setting
 *
 */
ATTRIBUTE_INLINE void wm_ll_rcc_set_clock_reset(wm_rcc_reg_t *rcc_dev, uint32_t clk_rst)
{
    rcc_dev->clk_rst.val = clk_rst;
}

/**
 * @brief Get the rcc clock reset setting
 * @param[in] rcc_dev the rcc register structure
 *
 * @return the current clock reset setting
 */
ATTRIBUTE_INLINE uint32_t wm_ll_rcc_get_clock_reset(wm_rcc_reg_t *rcc_dev)
{
    return rcc_dev->clk_rst.val;
}

/**
 * @brief Set the rcc clock divider freq enable
 * @param[in] rcc_dev the rcc register structure
 *
 * @param[in] freq_en the clock divider freq enable setting
 *
 */
ATTRIBUTE_INLINE void wm_ll_rcc_set_clock_div_freq_en(wm_rcc_reg_t *rcc_dev, bool freq_en)
{
    rcc_dev->clk_div.freq_div_en = freq_en;
}

/**
 * @brief Get the rcc clock reset setting
 * @param[in] rcc_dev the rcc register structure
 *
 * @return the current clock reset setting
 */
ATTRIBUTE_INLINE uint8_t wm_ll_rcc_get_clock_div_freq_en(wm_rcc_reg_t *rcc_dev)
{
    return rcc_dev->clk_div.freq_div_en;
}

/**
 * @brief Set the rcc cpu clock divider
 * @param[in] rcc_dev the rcc register structure
 *
 * @param[in] cpu_div the cpu clock divider
 *
 */
ATTRIBUTE_INLINE void wm_ll_rcc_set_cpu_clock_div(wm_rcc_reg_t *rcc_dev, uint8_t cpu_div)
{
    WM_LL_REG_FILED_COPY_WRTIE(rcc_dev->clk_div, cpu_clk_div, cpu_div);
}

/**
 * @brief Get the rcc cpu clock divider
 * @param[in] rcc_dev the rcc register structure
 *
 * @return the cpu clock divider
 */
ATTRIBUTE_INLINE uint8_t wm_ll_rcc_get_cpu_clock_div(wm_rcc_reg_t *rcc_dev)
{
    return WM_LL_REG_FILED_COPY_READ(rcc_dev->clk_div, cpu_clk_div);
}

/**
 * @brief Set the rcc wlan clock divider
 * @param[in] rcc_dev the rcc register structure
 *
 * @param[in] wlan_div the wlan clock divider
 *
 */
ATTRIBUTE_INLINE void wm_ll_rcc_set_wlan_clock_div(wm_rcc_reg_t *rcc_dev, uint8_t wlan_div)
{
    WM_LL_REG_FILED_COPY_WRTIE(rcc_dev->clk_div, wlan_clk_div, wlan_div);
}

/**
 * @brief Get the rcc wlan clock divider
 * @param[in] rcc_dev the rcc register structure
 *
 * @return the wlan clock divider
 */
ATTRIBUTE_INLINE uint8_t wm_ll_rcc_get_wlan_clock_div(wm_rcc_reg_t *rcc_dev)
{
    return WM_LL_REG_FILED_COPY_READ(rcc_dev->clk_div, wlan_clk_div);
}

/**
 * @brief Set the clock scaling factor between bus1 and bus2
 * @param[in] rcc_dev the rcc register structure
 *
 * @param[in] bus_syncdn_factor the bus syncdn factor
 *
 */
ATTRIBUTE_INLINE void wm_ll_rcc_set_bus_syncdn_factor(wm_rcc_reg_t *rcc_dev, uint8_t bus_syncdn_factor)
{
    WM_LL_REG_FILED_COPY_WRTIE(rcc_dev->clk_div, bus_syncdn_factor, bus_syncdn_factor);
}

/**
 * @brief Get the clock scaling factor between bus1 and bus2
 * @param[in] rcc_dev the rcc register structure
 *
 * @return the bus syncdn factor
 */
ATTRIBUTE_INLINE uint8_t wm_ll_rcc_get_bus_syncdn_factor(wm_rcc_reg_t *rcc_dev)
{
    return WM_LL_REG_FILED_COPY_READ(rcc_dev->clk_div, bus_syncdn_factor);
}

/**
 * @brief Set the rcc peripheral clock divider
 * @param[in] rcc_dev the rcc register structure
 *
 * @param[in] peripheral_div the peripheral clock divider
 *
 */
ATTRIBUTE_INLINE void wm_ll_rcc_set_peripheral_clock_div(wm_rcc_reg_t *rcc_dev, uint8_t peripheral_div)
{
    rcc_dev->clk_div.peripheral_clk_div = peripheral_div;
}

/**
 * @brief Get the rcc peripheral clock divider
 * @param[in] rcc_dev the rcc register structure
 *
 * @return the peripheral clock divider
 */
ATTRIBUTE_INLINE uint8_t wm_ll_rcc_get_peripheral_clock_div(wm_rcc_reg_t *rcc_dev)
{
    return rcc_dev->clk_div.peripheral_clk_div;
}

/**
 * @brief Set the rcc jtag clock enable
 * @param[in] rcc_dev the rcc register structure
 *
 * @param[in] jtag_en the jtag enable setting
 *
 */
ATTRIBUTE_INLINE void wm_ll_rcc_set_jtag_enable(wm_rcc_reg_t *rcc_dev, bool jtag_en)
{
    rcc_dev->clk_dbg_ctrl.jtag_en = jtag_en;
}

/**
 * @brief Get the rcc jtag clock enable setting
 * @param[in] rcc_dev the rcc register structure
 *
 * @return the jtag enable setting
 */
ATTRIBUTE_INLINE uint8_t wm_ll_rcc_get_jtag_enable(wm_rcc_reg_t *rcc_dev)
{
    return rcc_dev->clk_dbg_ctrl.jtag_en;
}

/**
 * @brief Set the rcc sd adc clock divider
 * @param[in] rcc_dev the rcc register structure
 *
 * @param[in] sd_adc_div the sd adc clock divider
 *
 */
ATTRIBUTE_INLINE void wm_ll_rcc_set_sd_adc_clock_div(wm_rcc_reg_t *rcc_dev, uint8_t sd_adc_div)
{
    WM_LL_REG_FILED_COPY_WRTIE(rcc_dev->clk_dbg_ctrl, sd_adc_div, sd_adc_div);
}

/**
 * @brief Get the rcc sd adc clock divider
 * @param[in] rcc_dev the rcc register structure
 *
 * @return the sd adc clock divider setting
 */
ATTRIBUTE_INLINE uint8_t wm_ll_rcc_get_sd_adc_clock_div(wm_rcc_reg_t *rcc_dev)
{
    return WM_LL_REG_FILED_COPY_READ(rcc_dev->clk_dbg_ctrl, sd_adc_div);
}

/**
 * @brief Set the rcc qflash clock selection
 * @param[in] rcc_dev the rcc register structure
 *
 * @param[in] clk_sel qflash clock selection
 *                [0] 40MHz
 *                [1] 80MHz
 *
 */
ATTRIBUTE_INLINE void wm_ll_rcc_set_qflash_clock_selection(wm_rcc_reg_t *rcc_dev, uint8_t clk_sel)
{
    rcc_dev->clk_dbg_ctrl.qflash_clk_sel = clk_sel;
}

/**
 * @brief Get the rcc qflash clock selection
 * @param[in] rcc_dev the rcc register structure
 *
 * @return the qflash clock selection setting
 *      [0] 40MHz
 *      [1] 80MHz
 */
ATTRIBUTE_INLINE uint8_t wm_ll_rcc_get_qflash_clock_selection(wm_rcc_reg_t *rcc_dev)
{
    return rcc_dev->clk_dbg_ctrl.qflash_clk_sel;
}

/**
 * @brief Set the rcc gpsec clock selection
 * @param[in] rcc_dev the rcc register structure
 *
 * @param[in] clk_sel gpsec clock selection
 *                [0] 80MHz
 *                [1] 160MHz
 *
 */
ATTRIBUTE_INLINE void wm_ll_rcc_set_gpsec_clock_selection(wm_rcc_reg_t *rcc_dev, uint8_t clk_sel)
{
    rcc_dev->clk_dbg_ctrl.gpspec_clk_sel = clk_sel;
}

/**
 * @brief Get the rcc gpsec clock selection
 * @param[in] rcc_dev the rcc register structure
 *
 * @return the gpsec clock selection setting
 *      [0] 80MHz
 *      [1] 160MHz
 */
ATTRIBUTE_INLINE uint8_t wm_ll_rcc_get_gpsec_clock_selection(wm_rcc_reg_t *rcc_dev)
{
    return rcc_dev->clk_dbg_ctrl.gpspec_clk_sel;
}

/**
 * @brief Set the rcc rsa clock selection
 * @param[in] rcc_dev the rcc register structure
 *
 * @param[in] clk_sel rsa clock selection
 *                [0] 80MHz
 *                [1] 160MHz
 *
 */
ATTRIBUTE_INLINE void wm_ll_rcc_set_rsa_clock_selection(wm_rcc_reg_t *rcc_dev, uint8_t clk_sel)
{
    rcc_dev->clk_dbg_ctrl.rsa_clk_sel = clk_sel;
}

/**
 * @brief Get the rcc rsa clock selection
 * @param[in] rcc_dev the rcc register structure
 *
 * @return the rsa clock selection setting
 *      [0] 80MHz
 *      [1] 160MHz
 */
ATTRIBUTE_INLINE uint8_t wm_ll_rcc_get_rsa_clock_selection(wm_rcc_reg_t *rcc_dev)
{
    return rcc_dev->clk_dbg_ctrl.rsa_clk_sel;
}

/**
 * @brief Set the rcc i2s clock configre
 * @param[in] rcc_dev the rcc register structure
 *
 * @param[in] extal_en external XTAL configure; 0: use internal XTAL, 1: use external XTAL
 * @param[in] mclk_en mclk configure; 0: disable mclk, 1: enable mclk
 * @param[in] mclk_div mclk divider
 * @param[in] bclk_div bclk divider
 *
 */
ATTRIBUTE_INLINE void wm_ll_rcc_set_i2s_clock_config(wm_rcc_reg_t *rcc_dev, bool extal_en, bool mclk_en, uint32_t mclk_div,
                                                     uint32_t bclk_div)
{
    rcc_dev->i2s_ctrl.extal_en = extal_en;
    rcc_dev->i2s_ctrl.mclk_en  = mclk_en;
    rcc_dev->i2s_ctrl.mclk_div = mclk_div;
    rcc_dev->i2s_ctrl.bclk_div = bclk_div;
}

/**
 * @brief Set the rcc i2s clock configre
 * @param[in] rcc_dev the rcc register structure
 *
 * @param[out] extal_en external XTAL configure; 0: use internal XTAL, 1: use external XTAL
 * @param[out] mclk_en mclk configure; 0: disable mclk, 1: enable mclk
 * @param[out] mclk_div mclk divider
 * @param[out] bclk_div bclk divider
 *
 */
ATTRIBUTE_INLINE void wm_ll_rcc_get_i2s_clock_config(wm_rcc_reg_t *rcc_dev, bool extal_en, bool mclk_en, uint8_t mclk_div,
                                                     uint8_t bclk_div)
{
    extal_en = rcc_dev->i2s_ctrl.extal_en;
    mclk_en  = rcc_dev->i2s_ctrl.mclk_en;
    mclk_div = rcc_dev->i2s_ctrl.mclk_div;
    bclk_div = rcc_dev->i2s_ctrl.bclk_div;
}

/**
 * @brief Query the cpu clock reset state
 * @param[in] rcc_dev the rcc register structure
 *
 * @param[out] rst_state the cpu clock reset state
 *                [0] do not trigger reset
 *                [1] triggered reset
 *
 */
ATTRIBUTE_INLINE void wm_ll_rcc_cpu_clock_reset_state_query(wm_rcc_reg_t *rcc_dev, uint8_t rst_state)
{
    rst_state = rcc_dev->clk_rst_state.cpu_rst_state;
}

/**
 * @brief Query the watch dog clock reset state
 * @param[in] rcc_dev the rcc register structure
 *
 * @param[out] rst_state the watch dog clock reset state
 *                [0] do not trigger reset
 *                [1] triggered reset
 *
 */
ATTRIBUTE_INLINE void wm_ll_rcc_wdog_clock_reset_state_query(wm_rcc_reg_t *rcc_dev, uint8_t rst_state)
{
    rst_state = rcc_dev->clk_rst_state.wdog_rst_state;
}

/**
 * @brief Clear the cpu clock reset state
 * @param[in] rcc_dev the rcc register structure
 *
 */
ATTRIBUTE_INLINE void wm_ll_rcc_cpu_clock_reset_state_clr(wm_rcc_reg_t *rcc_dev)
{
    rcc_dev->clk_rst_state.cpu_rst_state_clr = 1;
}

/**
 * @brief Clear the watch dog clock reset state
 * @param[in] rcc_dev the rcc register structure
 *
 */
ATTRIBUTE_INLINE void wm_ll_rcc_wdog_clock_reset_state_clr(wm_rcc_reg_t *rcc_dev)
{
    rcc_dev->clk_rst_state.wdog_rst_state_clr = 1;
}

#ifdef __cplusplus
}
#endif

#endif /* __WM_LL_RCC_H__ */
