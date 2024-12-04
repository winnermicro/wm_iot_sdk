/**
 * @file wm_ll_psram.h
 *
 * @brief ll psram Module
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

#ifndef __WM_LL_PSRAM_H__
#define __WM_LL_PSRAM_H__

#include "wm_types.h"
#include "wm_attr.h"
#include "wm_psram_reg_struct.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief          Psram reset
 * @param[in]      reg Psram regsiter handle,type of  @ref wm_psram_reg_t *
 */
ATTRIBUTE_INLINE void wm_ll_psram_rst(wm_psram_reg_t *reg)
{
    reg->ctrl.rst = 1;
}

/**
 * @brief          Get psram reset status
 * @param[in]      reg Psram regsiter handle,type of  @ref wm_psram_reg_t *
 * @retval         reset status
 */
ATTRIBUTE_INLINE int wm_ll_psram_get_rst_status(wm_psram_reg_t *reg)
{
    return reg->ctrl.rst;
}

/**
 * @brief          Enable quad mode
 * @param[in]      reg Psram regsiter handle,type of  @ref wm_psram_reg_t *
 * @param[in]      en  Enable or not
 */
ATTRIBUTE_INLINE void wm_ll_psram_enable_quad(wm_psram_reg_t *reg, bool en)
{
    reg->ctrl.quad = en;
}

/**
 * @brief          Get quad mode enable or not
 * @param[in]      reg Psram regsiter handle,type of  @ref wm_psram_reg_t *
 * @retval         Enable status
 */
ATTRIBUTE_INLINE bool wm_ll_psram_is_quad_enabled(wm_psram_reg_t *reg)
{
    return (bool)reg->ctrl.quad;
}

/**
 * @brief          Enable BURST function on AHB bus or not
 * @param[in]      reg Psram regsiter handle,type of  @ref wm_psram_reg_t *
 * @param[in]      en  Enable or not
 */
ATTRIBUTE_INLINE void wm_ll_psram_enable_burst(wm_psram_reg_t *reg, bool en)
{
    reg->ctrl.inc_en_burst = en;
}

/**
 * @brief          Get BURST function enable or not
 * @param[in]      reg Psram regsiter handle,type of  @ref wm_psram_reg_t *
 * @retval         Enable status
 */
ATTRIBUTE_INLINE bool wm_ll_psram_is_burst_enabled(wm_psram_reg_t *reg)
{
    return (bool)reg->ctrl.inc_en_burst;
}

/**
 * @brief          SPI frequency division setting
 * @param[in]      reg Psram regsiter handle,type of  @ref wm_psram_reg_t *
 * @param[in]      division Multiple of frequency division,
 *                    Can only be configured with a value of 2 or higher
 */
ATTRIBUTE_INLINE void wm_ll_psram_set_division(wm_psram_reg_t *reg, uint8_t division)
{
    reg->ctrl.division = division;
}

/**
 * @brief          Get frequency division
 * @param[in]      reg Psram regsiter handle,type of  @ref wm_psram_reg_t *
 * @retval         Frequency division
 */
ATTRIBUTE_INLINE uint8_t wm_ll_psram_get_division(wm_psram_reg_t *reg)
{
    return (uint8_t)reg->ctrl.division;
}

/**
 * @brief          CS high-level minimum time setting, measured in AHB clock cycles,
 * @param[in]      reg Psram regsiter handle,type of  @ref wm_psram_reg_t *
 * @param[in]      tcph CS high-level minimum time,
 *                    Must be greater than 1. The specific time can be set according to the instructions in
                      different PSRAM manuals. If unclear, the default value can be left unchanged
 */
ATTRIBUTE_INLINE void wm_ll_psram_set_tcph(wm_psram_reg_t *reg, uint8_t tcph)
{
    reg->ctrl.tcph = tcph;
}

/**
 * @brief          Get CS high-level minimum time setting
 * @param[in]      reg Psram regsiter handle,type of  @ref wm_psram_reg_t *
 * @retval         CS high-level minimum time setting
 */
ATTRIBUTE_INLINE uint8_t wm_ll_psram_get_tcph(wm_psram_reg_t *reg)
{
    return (uint8_t)reg->ctrl.tcph;
}

/**
 * @brief          Enable PSRAM half sleep mode
 * @param[in]      reg Psram regsiter handle,type of  @ref wm_psram_reg_t *
 * @param[in]      en  Enable or not
 */
ATTRIBUTE_INLINE void wm_ll_psram_enable_hsm(wm_psram_reg_t *reg, bool en)
{
    reg->ctrl.hsm = en;
}

/**
 * @brief          Get PSRAM half sleep mode enable or not
 * @param[in]      reg Psram regsiter handle,type of  @ref wm_psram_reg_t *
 * @retval         PSRAM half sleep mode enable status
 */
ATTRIBUTE_INLINE bool wm_ll_psram_is_hsm_enabled(wm_psram_reg_t *reg)
{
    return (bool)reg->ctrl.inc_en_burst;
}

/**
 * @brief          Set PSRAM ctrl
 * @param[in]      reg Psram regsiter handle,type of  @ref wm_psram_reg_t *
 * @param[in]      val  ctrl register value
 */
ATTRIBUTE_INLINE void wm_ll_psram_set_ctrl(wm_psram_reg_t *reg, uint32_t val)
{
    reg->ctrl.val = val;
}

/**
 * @brief          Get PSRAM ctrl register value
 * @param[in]      reg Psram regsiter handle,type of  @ref wm_psram_reg_t *
 * @retval         ctrl retister value
 */
ATTRIBUTE_INLINE bool wm_ll_psram_get_ctrl(wm_psram_reg_t *reg)
{
    return reg->ctrl.val;
}

/**
 * @brief          Set CS timeout time, which is low level maximum time
 * @param[in]      reg Psram regsiter handle,type of  @ref wm_psram_reg_t *
 * @param[in]      cs_timeout CS to a low maximum time for BURST mode
 */
ATTRIBUTE_INLINE void wm_ll_psram_set_cs_timeout(wm_psram_reg_t *reg, uint16_t cs_timeout)
{
    reg->over_timer.cs_timeout = cs_timeout;
}

/**
 * @brief          Get CS timeout, which is low level maximum time
 * @param[in]      reg Psram regsiter handle,type of  @ref wm_psram_reg_t *
 * @retval         CS low level maximum time
 */
ATTRIBUTE_INLINE uint16_t wm_ll_psram_get_cs_timeout(wm_psram_reg_t *reg)
{
    return (uint16_t)reg->over_timer.cs_timeout;
}

#ifdef __cplusplus
}
#endif

#endif