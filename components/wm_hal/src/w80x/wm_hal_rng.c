/**
 * @file wm_hal_rng.c
 *
 * @brief RNG HAL Module
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
#include <stdio.h>
#include <string.h>
#include "wm_soc.h"
#include "wm_reg_op.h"
#include "wm_regs_base.h"
#include "wm_ll_gpsec.h"
#include "wm_hal_rng.h"
#include "wm_osal.h"
#include "wm_hal_irq.h"
#include "wm_log.h"
#include "wm_drv_rcc.h"

#undef LOG_TAG
#define LOG_TAG "HAL_RNG"
#include "wm_log.h"

#define WM_HAL_RNG_DEBUG 1

#if WM_HAL_RNG_DEBUG
#define WM_HAL_RNG_LOG_D(...)   wm_log_debug(__VA_ARGS__)
#define WM_HAL_RNG_LOG_I(...)   wm_log_info(__VA_ARGS__)
#define WM_HAL_RNG_LOG_E(...)   wm_log_error(__VA_ARGS__)
#define WM_HAL_RNG_TRACE_FAIL() wm_log_error("%s@%d", __func__, __LINE__)
#else
#define WM_HAL_RNG_LOG_D(...)
#define WM_HAL_RNG_LOG_I(...)
#define WM_HAL_RNG_LOG_E(...)
#define WM_HAL_RNG_TRACE_FAIL()
#endif
/**
 * @brief Check param and return WM_ERR_INVALID_PARAM
 */
#define WM_HAL_RNG_ASSERT_PARAM(p)   \
    if (!(p)) {                      \
        WM_HAL_RNG_TRACE_FAIL();     \
        return WM_ERR_INVALID_PARAM; \
    }

int wm_hal_rng_init(wm_hal_rng_dev_t *dev)
{
    /* Check parameters */
    WM_HAL_RNG_ASSERT_PARAM(dev);

    wm_gpsec_reg_t *reg = dev->gpsec_reg;

    /* Check parameters */
    WM_HAL_RNG_ASSERT_PARAM(reg && IS_RNG_TYPE(dev->rng_cfg.type));

    if (dev->rng_cfg.type == WM_GPSEC_RESULT_DISP_TRNG) {
        /* clear irq flag */
        wm_ll_gpsec_clear_irq_flag(reg);
        /* mask completion interrupt */
        wm_ll_gpsec_mask_cmplt_irq(reg);
        /* set trng config register */
        wm_ll_gpsec_unmask_trng_irq(reg);
        wm_ll_gpsec_set_trng_charge_pump(reg, 4);
        wm_ll_gpsec_set_rng_result_select(reg, WM_GPSEC_RESULT_DISP_TRNG);
        wm_ll_gpsec_set_trng_start(reg, 1);
        /* This delay is for better randomness of early stage */
        mdelay(1);
    } else {
        WM_HAL_RNG_ASSERT_PARAM(IS_PRNG_BIT_WITH(dev->rng_cfg.bit_width));
        /* clear irq flag */
        wm_ll_gpsec_clear_irq_flag(reg);
        wm_ll_gpsec_set_prng_seed(reg, dev->rng_cfg.seed);
        /* set trng config register */
        wm_ll_gpsec_set_rng_result_select(reg, WM_GPSEC_RESULT_DISP_PRNG);
        /* set rng config register */
        wm_ll_gpsec_set_prng_bit_length(reg, dev->rng_cfg.bit_width);
        wm_ll_gpsec_set_prng_load_seed_method(reg, WM_GPSEC_RNG_LOAD);
        wm_ll_gpsec_set_prng_start(reg, 1);
    }

    return WM_ERR_SUCCESS;
}

int wm_hal_rng_deinit(wm_hal_rng_dev_t *dev)
{
    /* Check parameters */
    WM_HAL_RNG_ASSERT_PARAM(dev);

    wm_gpsec_reg_t *reg = dev->gpsec_reg;

    /* Check parameters */
    WM_HAL_RNG_ASSERT_PARAM(reg);

    /* set trng config register */
    wm_ll_gpsec_mask_trng_irq(reg);
    /* set rng config register */
    wm_ll_gpsec_set_prng_start(reg, 0);
    return WM_ERR_SUCCESS;
}

uint32_t wm_hal_rng_read(wm_hal_rng_dev_t *dev)
{
    uint32_t ret = 0;
    /* Check parameters */
    if (dev == NULL) {
        WM_HAL_RNG_TRACE_FAIL();
        return ret;
    }

    wm_gpsec_reg_t *reg = dev->gpsec_reg;
    uint32_t time_out   = 0xFFFF;

    /* Check parameters */
    if (reg == NULL || IS_RNG_TYPE(dev->rng_cfg.type) == 0) {
        WM_HAL_RNG_TRACE_FAIL();
        return ret;
    }

    if (dev->rng_cfg.type == WM_GPSEC_RESULT_DISP_TRNG) {
        while (!wm_ll_gpsec_get_irq_flag(reg) && time_out) {
            time_out--;
        }
        if (time_out == 0) {
            WM_HAL_RNG_LOG_E("line:%d timeout\n", __LINE__);
        }
        /* clear irq flag */
        wm_ll_gpsec_clear_irq_flag(reg);
    }

    return wm_ll_gpsec_get_rng(reg);
}

int wm_hal_rng_read_bytes(wm_hal_rng_dev_t *dev, void *buf, uint32_t len)
{
    /* Check parameters */
    WM_HAL_RNG_ASSERT_PARAM(dev && buf && len > 0 && IS_RNG_TYPE(dev->rng_cfg.type) &&
                            IS_PRNG_BIT_WITH(dev->rng_cfg.bit_width));

    uint8_t *buf_bytes = (uint8_t *)buf;
    uint8_t rng_bytes  = (dev->rng_cfg.bit_width == WM_GPSEC_RNG_32BIT) ? sizeof(uint32_t) : sizeof(uint16_t);
    if (dev->rng_cfg.type == WM_GPSEC_RESULT_DISP_TRNG) {
        rng_bytes = sizeof(uint32_t);
    }
    while (len > 0) {
        uint32_t word = wm_hal_rng_read(dev);
        if (len > rng_bytes) {
            memcpy(buf_bytes, &word, rng_bytes);
            buf_bytes += rng_bytes;
            len -= rng_bytes;
        } else {
            memcpy(buf_bytes, &word, len);
            len = 0;
        }
    }
    return WM_ERR_SUCCESS;
}
