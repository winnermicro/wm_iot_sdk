/**
 * @file wm_ll_rsa.h
 *
 * @brief LL_RSA Module
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

#ifndef __WM_LL_RSA_H__
#define __WM_LL_RSA_H__

#include "wm_attr.h"
#include "wm_rsa_reg_struct.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup WM_LL_RSA_Enumerations WM LL_RSA Enumerations
 * @brief WinnerMicro LL_RSA Enumerations
 */

/**
 * @addtogroup WM_LL_RSA_Enumerations
 * @{
 */

/**
 * @enum    Mode selection for modular multiplication
 */
typedef enum {
    WM_LL_RSA_MODMUL_MODE_DD = 0, /*< modular multiplication mode DD */
    WM_LL_RSA_MODMUL_MODE_AB,     /*< modular multiplication mode AB */
    WM_LL_RSA_MODMUL_MODE_DB,     /*< modular multiplication mode DB */
    WM_LL_RSA_MODMUL_MODE_AA,     /*< modular multiplication mode AA */
} wm_ll_rsa_modmul_mode_t;

/**
 * @}
 */

/**
 * @defgroup WM_LL_RSA_Functions WM LL_RSA Functions
 * @brief WinnerMicro LL_RSA Functions
 */

/**
 * @addtogroup WM_LL_RSA_Functions
 * @{
 */

/**
 * @brief       Write XBUF
 * @param[in]   reg Pointer to rsa register structure
 * @param[in]   source address
 */
ATTRIBUTE_INLINE void wm_ll_rsa_monmul_write_a(wm_rsa_reg_t *reg, const uint32_t *const in)
{
    memcpy((uint32_t *)reg->xbuf, in, reg->rsa_n.val * sizeof(uint32_t));
}

/**
 * @brief       Read XBUF
 * @param[in]   reg Pointer to rsa register structure
 * @param[in]   destination address
 */
ATTRIBUTE_INLINE void wm_ll_rsa_monmul_read_a(wm_rsa_reg_t *reg, uint32_t *const out)
{
    memcpy(out, (uint32_t *)reg->xbuf, reg->rsa_n.val * sizeof(uint32_t));
}

/**
 * @brief       Write YBUF
 * @param[in]   reg Pointer to rsa register structure
 * @param[in]   source address
 */
ATTRIBUTE_INLINE void wm_ll_rsa_monmul_write_b(wm_rsa_reg_t *reg, const uint32_t *const in)
{
    memcpy((uint32_t *)reg->ybuf, in, reg->rsa_n.val * sizeof(uint32_t));
}

/**
 * @brief       Read YBUF
 * @param[in]   reg Pointer to rsa register structure
 * @param[in]   destination address
 */
ATTRIBUTE_INLINE void wm_ll_rsa_monmul_read_b(wm_rsa_reg_t *reg, uint32_t *const out)
{
    memcpy(out, (uint32_t *)reg->ybuf, reg->rsa_n.val * sizeof(uint32_t));
}

/**
 * @brief       Write MBUF
 * @param[in]   reg Pointer to rsa register structure
 * @param[in]   source address
 */
ATTRIBUTE_INLINE void wm_ll_rsa_monmul_write_m(wm_rsa_reg_t *reg, const uint32_t *const in)
{
    memcpy((uint32_t *)reg->mbuf, in, reg->rsa_n.val * sizeof(uint32_t));
}

/**
 * @brief       Read MBUF
 * @param[in]   reg Pointer to rsa register structure
 * @param[in]   destination address
 */
ATTRIBUTE_INLINE void wm_ll_rsa_monmul_read_m(wm_rsa_reg_t *reg, uint32_t *const out)
{
    memcpy(out, (uint32_t *)reg->mbuf, reg->rsa_n.val * sizeof(uint32_t));
}

/**
 * @brief       Write DBUF
 * @param[in]   reg Pointer to rsa register structure
 * @param[in]   source address
 */
ATTRIBUTE_INLINE void wm_ll_rsa_monmul_write_d(wm_rsa_reg_t *reg, const uint32_t *const in)
{
    memcpy((uint32_t *)reg->dbuf, in, reg->rsa_n.val * sizeof(uint32_t));
}

/**
 * @brief       Read DBUF
 * @param[in]   reg Pointer to rsa register structure
 * @param[in]   destination address
 */
ATTRIBUTE_INLINE void wm_ll_rsa_monmul_read_d(wm_rsa_reg_t *reg, uint32_t *const out)
{
    memcpy(out, (uint32_t *)reg->dbuf, reg->rsa_n.val * sizeof(uint32_t));
}

/**
 * @brief          Get modular multiplication completion flag
 * @param[in]      reg Pointer to rsa register structure.
 * @retval         Modular multiplication completion flag
 */
ATTRIBUTE_INLINE uint32_t wm_ll_rsa_get_cmplt_flag(wm_rsa_reg_t *reg)
{
    return reg->rsa_ctrl.modmul_cmplt;
}

/**
 * @brief          Clear modular multiplication completion flag
 * @param[in]      reg Pointer to rsa register structure.
 */
ATTRIBUTE_INLINE void wm_ll_rsa_clear_cmplt_flag(wm_rsa_reg_t *reg)
{
    reg->rsa_ctrl.modmul_cmplt = 0;
}

/**
 * @brief       Select modular multiplication mode
 * @param[in]   reg Pointer to rsa register structure
 * @param[in]   sel type of  @arg wm_ll_rsa_modmul_mode_t
 */
ATTRIBUTE_INLINE void wm_ll_rsa_set_modmul_mode(wm_rsa_reg_t *reg, wm_ll_rsa_modmul_mode_t sel)
{
    reg->rsa_ctrl.mode_sel = sel;
}

/**
 * @brief       Get modular multiplication mode
 * @param[in]   reg Pointer to rsa register structure
 * @return      type of  @arg wm_ll_rsa_modmul_mode_t
 */
ATTRIBUTE_INLINE wm_ll_rsa_modmul_mode_t wm_ll_rsa_get_modmul_mode(wm_rsa_reg_t *reg)
{
    return reg->rsa_ctrl.mode_sel;
}

/**
 * @brief          Soft reset RSA modular
 * @param[in]      reg Pointer to rsa register structure
 */
ATTRIBUTE_INLINE void wm_ll_rsa_set_soft_reset(wm_rsa_reg_t *reg)
{
    reg->rsa_ctrl.reset = 1;
}

/**
 * @brief          Get rsa soft reset bit
 * @param[in]      reg Pointer to rsa register structure
 * @return         RSA modular soft reset bit
 */
ATTRIBUTE_INLINE uint32_t wm_ll_rsa_get_soft_reset(wm_rsa_reg_t *reg)
{
    return reg->rsa_ctrl.reset;
}

/**
 * @brief       Set Modular multiplication start bit
 * @param[in]   reg Pointer to rsa register structure
 * @param[in]   start
 */
ATTRIBUTE_INLINE void wm_ll_rsa_set_modmul_start(wm_rsa_reg_t *reg)
{
    reg->rsa_ctrl.start = 1;
}

/**
 * @brief       Get Modular multiplication start bit
 * @param[in]   reg Pointer to rsa register structure
 * @return      rsa start bit
 */
ATTRIBUTE_INLINE uint32_t wm_ll_rsa_get_modmul_start(wm_rsa_reg_t *reg)
{
    return reg->rsa_ctrl.start;
}

/**
 * @brief       Set RSA MC value
 *              Note: Write only. Read value is 0.
 * @param[in]   reg Pointer to rsa register structure
 * @param[in]   value
 */
ATTRIBUTE_INLINE void wm_ll_rsa_set_mc_value(wm_rsa_reg_t *reg, uint32_t value)
{
    reg->rsa_mc.val = value;
}

/**
 * @brief       Set RSA N value, Modular multiplication length in words
 * @param[in]   reg Pointer to rsa register structure
 * @param[in]   value
 */
ATTRIBUTE_INLINE void wm_ll_rsa_set_n_value(wm_rsa_reg_t *reg, uint32_t value)
{
    reg->rsa_n.n = value;
}

/**
 * @brief       Get RSA N value, Modular multiplication length in words
 * @param[in]   reg Pointer to rsa register structure
 * @return      RSA N value
 */
ATTRIBUTE_INLINE uint32_t wm_ll_rsa_get_n_value(wm_rsa_reg_t *reg)
{
    return reg->rsa_n.n;
}

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* __WM_LL_RSA_H__ */
