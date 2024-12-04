/**
 * @file wm_hal_crc.c
 *
 * @brief CRC HAL Module
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
#include "wm_ll_gpsec.h"
#include "wm_hal_crc.h"
#undef LOG_TAG
#define LOG_TAG "HAL_CRC"
#include "wm_log.h"

#define WM_HAL_CRC_DEBUG 1

#if WM_HAL_CRC_DEBUG
#define WM_HAL_CRC_LOG_D(...)   wm_log_debug(__VA_ARGS__)
#define WM_HAL_CRC_LOG_I(...)   wm_log_info(__VA_ARGS__)
#define WM_HAL_CRC_LOG_E(...)   wm_log_error(__VA_ARGS__)
#define WM_HAL_CRC_TRACE_FAIL() wm_log_error("%s@%d", __func__, __LINE__)
#else
#define WM_HAL_CRC_LOG_D(...)
#define WM_HAL_CRC_LOG_I(...)
#define WM_HAL_CRC_LOG_E(...)
#define WM_HAL_CRC_TRACE_FAIL()
#endif

/**
 * @brief Check param and return WM_ERR_INVALID_PARAM
 */
#define WM_HAL_CRC_ASSERT_PARAM(p)   \
    if (!(p)) {                      \
        WM_HAL_CRC_TRACE_FAIL();     \
        return WM_ERR_INVALID_PARAM; \
    }

static uint32_t reverse_num(uint32_t num, uint8_t bit)
{
    uint32_t i;
    uint32_t ret = 0;
    for (i = 1; i < (bit + 1); i++) {
        if (num & 1) {
            ret |= 1 << (bit - i);
        }
        num >>= 1;
    }
    return ret;
}

static int set_crc_value(wm_hal_crc_dev_t *dev)
{
    /* Check parameters */
    WM_HAL_CRC_ASSERT_PARAM(IS_CRC_TYPE(dev->crc_cfg.type) && IS_CRC_REVERSE(dev->crc_cfg.reverse));

    if (dev->crc_cfg.reverse & WM_GPSEC_CRC_OUT_REVERSE) {
        uint8_t crc_bit = 16;
        uint32_t value  = 0;
        switch (dev->crc_cfg.type) {
            case WM_GPSEC_CRC8:
                crc_bit = 8;
                break;
            case WM_GPSEC_CRC16_MODBUS:
            case WM_GPSEC_CRC16_CCITT:
                crc_bit = 16;
                break;
            case WM_GPSEC_CRC32:
                crc_bit = 32;
                break;
            default:
                break;
        }
        value = reverse_num(dev->crc_cfg.state, crc_bit);
        wm_ll_gpsec_set_crc_value(dev->gpsec_reg, value);
    } else {
        wm_ll_gpsec_set_crc_value(dev->gpsec_reg, dev->crc_cfg.state);
    }
    return WM_ERR_SUCCESS;
}

int wm_hal_crc_init(wm_hal_crc_dev_t *dev)
{
    /* Check parameters */
    WM_HAL_CRC_ASSERT_PARAM(dev);

    wm_gpsec_reg_t *reg          = dev->gpsec_reg;
    wm_gpsec_crc_type_t crc_type = dev->crc_cfg.type;
    uint8_t reverse              = dev->crc_cfg.reverse;

    /* Check parameters */
    WM_HAL_CRC_ASSERT_PARAM(reg && IS_CRC_TYPE(crc_type) && IS_CRC_REVERSE(reverse));

    /* Clear complete Flag */
    wm_ll_gpsec_clear_irq_flag(reg);
    /* set config register */
    wm_ll_gpsec_mask_cmplt_irq(reg);
    wm_ll_gpsec_set_cypher_mode(reg, WM_GPSEC_CYPHER_MODE_CRC);
    wm_ll_gpsec_set_crc_type(reg, crc_type);
    wm_ll_gpsec_set_crc_reverse(reg, reverse);
    return WM_ERR_SUCCESS;
}

int wm_hal_crc_update(wm_hal_crc_dev_t *dev, unsigned char *in, uint32_t len)
{
    /* Check parameters */
    WM_HAL_CRC_ASSERT_PARAM(dev && in && len > 0 && len <= 65532);

    wm_gpsec_reg_t *reg = dev->gpsec_reg;
    /* Check parameters */
    WM_HAL_CRC_ASSERT_PARAM(reg);

    uint32_t time_out = 0xFFFF;

    wm_hal_crc_init(dev);
    /* set config register */
    wm_ll_gpsec_set_total_bytes(reg, len);
    /* set crc value register */
    set_crc_value(dev);
    /* set source address */
    wm_ll_gpsec_set_src_address(reg, in);
    /* Clear complete Flag */
    wm_ll_gpsec_clear_irq_flag(reg);

    /* start operation */
    wm_ll_gpsec_set_operation_start(reg, 1);
    /* wait for complete flag to be raised */
    while (!wm_ll_gpsec_get_irq_flag(reg) && time_out) {
        time_out--;
    }
    if (time_out == 0) {
        WM_HAL_CRC_LOG_E("%s@%d timeout", __func__, __LINE__);
    }
    /* Clear complete Flag */
    wm_ll_gpsec_clear_irq_flag(reg);
    /* Update crc value */
    dev->crc_cfg.state = wm_ll_gpsec_get_crc_value(reg);
    /* Clear crc fifo */
    wm_ll_gpsec_clear_crc_fifo(reg);
    /* mask completion interrupt */
    wm_ll_gpsec_mask_cmplt_irq(reg);
    return WM_ERR_SUCCESS;
}

int wm_hal_crc_final(wm_hal_crc_dev_t *dev, uint32_t *crc_val)
{
    /* Check parameters */
    WM_HAL_CRC_ASSERT_PARAM(dev && crc_val);

    *crc_val = dev->crc_cfg.state;
    return WM_ERR_SUCCESS;
}
