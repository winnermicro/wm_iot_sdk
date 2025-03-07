/**
 * @file wm_hal_cryp.c
 *
 * @brief CRYP HAL Module
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
#include "wm_hal_crypto.h"
#include "wm_osal.h"
#include "wm_hal_irq.h"

#undef LOG_TAG
#define LOG_TAG "HAL_CRYP"
#include "wm_log.h"

#define WM_HAL_CRYP_DEBUG 1

#if WM_HAL_CRYP_DEBUG
#define WM_HAL_CRYP_LOG_D(...)   //wm_log_debug(__VA_ARGS__)
#define WM_HAL_CRYP_LOG_I(...)   wm_log_info(__VA_ARGS__)
#define WM_HAL_CRYP_LOG_E(...)   wm_log_error(__VA_ARGS__)
#define WM_HAL_CRYP_TRACE_FAIL() wm_log_error("%s@%d", __func__, __LINE__)
#else
#define WM_HAL_CRYP_LOG_D(...)
#define WM_HAL_CRYP_LOG_I(...)
#define WM_HAL_CRYP_LOG_E(...)
#define WM_HAL_CRYP_TRACE_FAIL()
#endif

/**
 * @brief Check param and return WM_ERR_INVALID_PARAM
 */
#define WM_HAL_CRYP_ASSERT_PARAM(p)  \
    if (!(p)) {                      \
        WM_HAL_CRYP_TRACE_FAIL();    \
        return WM_ERR_INVALID_PARAM; \
    }

/**
 * @brief Wait for complete flag to be raised.
 *
 * @param dev pointer to a wm_hal_cryp_dev_t structure.
 *
 */
void wm_hal_cryp_wait_complete(wm_hal_cryp_dev_t *dev)
{
    uint32_t time_out = 0xFFFFFF;
    while (!wm_ll_gpsec_get_irq_flag(dev->gpsec_reg) && time_out) {
        time_out--;
    }
    if (time_out == 0) {
        WM_HAL_CRYP_LOG_E("%s@%d timeout", __func__, __LINE__);
    }
}

int wm_hal_rc4_init(wm_hal_cryp_dev_t *dev)
{
    /* Check parameters */
    WM_HAL_CRYP_ASSERT_PARAM(dev);
    int ret             = WM_ERR_SUCCESS;
    wm_gpsec_reg_t *reg = dev->gpsec_reg;
    uint16_t key_len    = dev->cryp_cfg.param.rc4_cfg.key_len;
    unsigned char *key  = (unsigned char *)dev->cryp_cfg.param.rc4_cfg.key;

    /* Check parameters */
    WM_HAL_CRYP_ASSERT_PARAM(reg && IS_RC4_KEYSIZE(key_len) && key);

    /* set key */
    wm_ll_gpsec_set_key(reg, key, key_len >> 2);
    /* set config register */
    wm_ll_gpsec_reset_config_reg(reg);
    wm_ll_gpsec_mask_cmplt_irq(reg);
    wm_ll_gpsec_set_cypher_mode(reg, WM_GPSEC_CYPHER_MODE_RC4);
    wm_ll_gpsec_set_rc4_block_length(reg, key_len == RC4_128BIT_KEYLEN ? WM_GPSEC_RC4_128BIT : WM_GPSEC_RC4_256BIT);
    wm_ll_gpsec_set_rc4_soft_reset(reg);
    /* Clear complete Flag */
    wm_ll_gpsec_clear_irq_flag(reg);

    return ret;
}

int wm_hal_rc4_deinit(wm_hal_cryp_dev_t *dev)
{
    /* Check parameters */
    WM_HAL_CRYP_ASSERT_PARAM(dev);

    int ret             = WM_ERR_SUCCESS;
    wm_gpsec_reg_t *reg = dev->gpsec_reg;

    /* Check parameters */
    WM_HAL_CRYP_ASSERT_PARAM(reg);

    /* clear interrupt flag */
    wm_ll_gpsec_clear_irq_flag(reg);
    /* mask completion interrupt */
    wm_ll_gpsec_mask_cmplt_irq(reg);

    return ret;
}

int wm_hal_rc4_encrypt_decrypt(wm_hal_cryp_dev_t *dev, unsigned char *in, unsigned char *out, uint32_t len)
{
    /* Check parameters */
    WM_HAL_CRYP_ASSERT_PARAM(dev && in && out && len > 0 && len <= 65532);

    wm_gpsec_reg_t *reg = dev->gpsec_reg;

    /* Check parameters */
    WM_HAL_CRYP_ASSERT_PARAM(reg);

    wm_hal_rc4_init(dev);
    wm_ll_gpsec_set_src_address(reg, in);
    wm_ll_gpsec_set_dest_address(reg, out);
    /* set config register */
    wm_ll_gpsec_set_total_bytes(reg, len);
    /* start operation */
    wm_ll_gpsec_set_operation_start(reg, 1);

    /* Wait for complete flag to be raised */
    wm_hal_cryp_wait_complete(dev);

    /* Clear complete Flag */
    wm_ll_gpsec_clear_irq_flag(reg);

    return WM_ERR_SUCCESS;
}

int wm_hal_aes_init(wm_hal_cryp_dev_t *dev)
{
    /* Check parameters */
    WM_HAL_CRYP_ASSERT_PARAM(dev);
    int ret                      = WM_ERR_SUCCESS;
    wm_gpsec_reg_t *reg          = dev->gpsec_reg;
    uint16_t key_len             = dev->cryp_cfg.param.aes_cfg.key.key_len;
    unsigned char *key           = (unsigned char *)dev->cryp_cfg.param.aes_cfg.key.skey;
    unsigned char *iv            = (unsigned char *)dev->cryp_cfg.param.aes_cfg.iv;
    wm_gpsec_sub_mode_t sub_mode = dev->cryp_cfg.param.aes_cfg.sub_mode;

    /* Check parameters */
    WM_HAL_CRYP_ASSERT_PARAM(reg && IS_AES_KEYSIZE(key_len) && key && iv && IS_AES_SUBMODE(sub_mode));

    /* set key */
    wm_ll_gpsec_set_key(reg, key, key_len >> 2);
    /* set iv */
    wm_ll_gpsec_set_iv(reg, iv, AES_IV_LEN >> 2);
    /* set config register */
    wm_ll_gpsec_reset_config_reg(reg);
    wm_ll_gpsec_mask_cmplt_irq(reg);
    wm_ll_gpsec_set_cypher_mode(reg, WM_GPSEC_CYPHER_MODE_AES);
    wm_ll_gpsec_set_sub_mode(reg, sub_mode);
    wm_ll_gpsec_set_aes_soft_reset(reg);
    /* Clear complete Flag */
    wm_ll_gpsec_clear_irq_flag(reg);

    return ret;
}

int wm_hal_aes_deinit(wm_hal_cryp_dev_t *dev)
{
    /* Check parameters */
    WM_HAL_CRYP_ASSERT_PARAM(dev);
    int ret             = WM_ERR_SUCCESS;
    wm_gpsec_reg_t *reg = dev->gpsec_reg;

    /* Check parameters */
    WM_HAL_CRYP_ASSERT_PARAM(reg);

    /* clear interrupt flag */
    wm_ll_gpsec_clear_irq_flag(reg);
    /* mask completion interrupt */
    wm_ll_gpsec_mask_cmplt_irq(reg);

    return ret;
}

int wm_hal_aes_encrypt_decrypt(wm_hal_cryp_dev_t *dev, unsigned char *in, unsigned char *out, uint32_t len,
                               wm_gpsec_encrypt_decrypt_sel_t enc_dec)
{
    /* Check parameters */
    WM_HAL_CRYP_ASSERT_PARAM(dev && in && out && IS_AES_BUFFERSIZE(len) && len <= 0xFFFF && IS_CRYP_OP(enc_dec));

    wm_gpsec_reg_t *reg                                  = dev->gpsec_reg;
    wm_gpsec_sub_mode_t sub_mode __attribute__((unused)) = dev->cryp_cfg.param.aes_cfg.sub_mode;

    /* Check parameters */
    WM_HAL_CRYP_ASSERT_PARAM(reg);

    wm_hal_aes_init(dev);

    wm_ll_gpsec_set_src_address(reg, in);
    wm_ll_gpsec_set_dest_address(reg, out);
    /* set config register */
    wm_ll_gpsec_set_total_bytes(reg, len);
    wm_ll_gpsec_set_encrypt_or_decrypt(reg, enc_dec);
    WM_HAL_CRYP_LOG_D(
        "[%d]:aes[%d] %s %s start", wm_os_internal_get_time_ms(), len, enc_dec == WM_GPSEC_ENCRYPT ? "ENCRYPT" : "DECRYPT",
        sub_mode == WM_GPSEC_ECB ? "ECB" : (sub_mode == WM_GPSEC_CBC ? "CBC" : (sub_mode == WM_GPSEC_CTR ? "CTR" : "MAC")));

    /* start operation */
    wm_ll_gpsec_set_operation_start(reg, 1);

    /* Wait for complete flag to be raised */
    wm_hal_cryp_wait_complete(dev);
    /* Clear complete Flag */
    wm_ll_gpsec_clear_irq_flag(reg);
    WM_HAL_CRYP_LOG_D("[%d]:aes end %d", wm_os_internal_get_time_ms(), wm_ll_gpsec_get_cmplt_bytes(reg));
    return WM_ERR_SUCCESS;
}

int wm_hal_des_init(wm_hal_cryp_dev_t *dev)
{
    /* Check parameters */
    WM_HAL_CRYP_ASSERT_PARAM(dev);
    int ret                      = WM_ERR_SUCCESS;
    wm_gpsec_reg_t *reg          = dev->gpsec_reg;
    unsigned char *key           = (unsigned char *)dev->cryp_cfg.param.des_cfg.key.ek[0];
    unsigned char *iv            = (unsigned char *)dev->cryp_cfg.param.des_cfg.iv;
    wm_gpsec_sub_mode_t sub_mode = dev->cryp_cfg.param.des_cfg.sub_mode;

    /* Check parameters */
    WM_HAL_CRYP_ASSERT_PARAM(reg && key && iv && IS_DES_SUBMODE(sub_mode));

    /* set key */
    wm_ll_gpsec_set_key(reg, key, DES_KEY_LEN >> 2);
    /* set iv */
    wm_ll_gpsec_set_iv(reg, iv, DES_IV_LEN >> 2);
    /* set config register */
    wm_ll_gpsec_reset_config_reg(reg);
    wm_ll_gpsec_mask_cmplt_irq(reg);
    wm_ll_gpsec_set_cypher_mode(reg, WM_GPSEC_CYPHER_MODE_DES);
    wm_ll_gpsec_set_sub_mode(reg, sub_mode);
    wm_ll_gpsec_set_des_soft_reset(reg);
    /* Clear complete Flag */
    wm_ll_gpsec_clear_irq_flag(reg);

    return ret;
}

int wm_hal_des_deinit(wm_hal_cryp_dev_t *dev)
{
    /* Check parameters */
    WM_HAL_CRYP_ASSERT_PARAM(dev);
    int ret             = WM_ERR_SUCCESS;
    wm_gpsec_reg_t *reg = dev->gpsec_reg;

    /* Check parameters */
    WM_HAL_CRYP_ASSERT_PARAM(reg);

    /* clear interrupt flag */
    wm_ll_gpsec_clear_irq_flag(reg);
    /* mask completion interrupt */
    wm_ll_gpsec_mask_cmplt_irq(reg);

    return ret;
}

int wm_hal_des_encrypt_decrypt(wm_hal_cryp_dev_t *dev, unsigned char *in, unsigned char *out, uint32_t len,
                               wm_gpsec_encrypt_decrypt_sel_t enc_dec)
{
    /* Check parameters */
    WM_HAL_CRYP_ASSERT_PARAM(dev && in && out && IS_DES_BUFFERSIZE(len) && len <= 0xFFFF && IS_CRYP_OP(enc_dec));

    wm_gpsec_reg_t *reg = dev->gpsec_reg;

    /* Check parameters */
    WM_HAL_CRYP_ASSERT_PARAM(reg);

    wm_hal_des_init(dev);
    wm_ll_gpsec_set_src_address(reg, in);
    wm_ll_gpsec_set_dest_address(reg, out);
    /* set config register */
    wm_ll_gpsec_set_total_bytes(reg, len);
    wm_ll_gpsec_set_encrypt_or_decrypt(reg, enc_dec);
    WM_HAL_CRYP_LOG_D("[%d]:des[%d] %s %s start", wm_os_internal_get_time_ms(), len,
                      enc_dec == WM_GPSEC_ENCRYPT ? "ENCRYPT" : "DECRYPT",
                      dev->cryp_cfg.param.des_cfg.sub_mode == WM_GPSEC_ECB ?
                          "ECB" :
                          (dev->cryp_cfg.param.des_cfg.sub_mode == WM_GPSEC_CBC ?
                               "CBC" :
                               (dev->cryp_cfg.param.des_cfg.sub_mode == WM_GPSEC_CTR ? "CTR" : "MAC")));

    /* start operation */
    wm_ll_gpsec_set_operation_start(reg, 1);

    /* Wait for complete flag to be raised */
    wm_hal_cryp_wait_complete(dev);
    /* Clear complete Flag */
    wm_ll_gpsec_clear_irq_flag(reg);
    WM_HAL_CRYP_LOG_D("[%d]:des end %d", wm_os_internal_get_time_ms(), wm_ll_gpsec_get_cmplt_bytes(reg));
    return WM_ERR_SUCCESS;
}

int wm_hal_3des_init(wm_hal_cryp_dev_t *dev)
{
    /* Check parameters */
    WM_HAL_CRYP_ASSERT_PARAM(dev);
    int ret                      = WM_ERR_SUCCESS;
    wm_gpsec_reg_t *reg          = dev->gpsec_reg;
    unsigned char *key           = (unsigned char *)dev->cryp_cfg.param.des3_cfg.key.ek[0];
    unsigned char *iv            = (unsigned char *)dev->cryp_cfg.param.des3_cfg.iv;
    wm_gpsec_sub_mode_t sub_mode = dev->cryp_cfg.param.des3_cfg.sub_mode;

    /* Check parameters */
    WM_HAL_CRYP_ASSERT_PARAM(reg && key && iv && IS_DES_SUBMODE(sub_mode));

    /* set key */
    wm_ll_gpsec_set_key(reg, key, DES3_KEY_LEN >> 2);
    /* set iv */
    wm_ll_gpsec_set_iv(reg, iv, DES3_IV_LEN >> 2);
    /* set config register */
    wm_ll_gpsec_reset_config_reg(reg);
    wm_ll_gpsec_mask_cmplt_irq(reg);
    wm_ll_gpsec_set_cypher_mode(reg, WM_GPSEC_CYPHER_MODE_3DES);
    wm_ll_gpsec_set_sub_mode(reg, sub_mode);
    wm_ll_gpsec_set_des_soft_reset(reg);
    /* Clear complete Flag */
    wm_ll_gpsec_clear_irq_flag(reg);

    return ret;
}

int wm_hal_3des_deinit(wm_hal_cryp_dev_t *dev)
{
    /* Check parameters */
    WM_HAL_CRYP_ASSERT_PARAM(dev);
    int ret             = WM_ERR_SUCCESS;
    wm_gpsec_reg_t *reg = dev->gpsec_reg;

    /* Check parameters */
    WM_HAL_CRYP_ASSERT_PARAM(reg);

    /* clear interrupt flag */
    wm_ll_gpsec_clear_irq_flag(reg);
    /* mask completion interrupt */
    wm_ll_gpsec_mask_cmplt_irq(reg);

    return ret;
}

int wm_hal_3des_encrypt_decrypt(wm_hal_cryp_dev_t *dev, unsigned char *in, unsigned char *out, uint32_t len,
                                wm_gpsec_encrypt_decrypt_sel_t enc_dec)
{
    /* Check parameters */
    WM_HAL_CRYP_ASSERT_PARAM(dev && in && out && IS_DES_BUFFERSIZE(len) && len <= 0xFFFF && IS_CRYP_OP(enc_dec));
    wm_gpsec_reg_t *reg = dev->gpsec_reg;

    /* Check parameters */
    WM_HAL_CRYP_ASSERT_PARAM(reg);

    wm_hal_3des_init(dev);
    wm_ll_gpsec_set_src_address(reg, in);
    wm_ll_gpsec_set_dest_address(reg, out);
    /* set config register */
    wm_ll_gpsec_set_total_bytes(reg, len);
    wm_ll_gpsec_set_encrypt_or_decrypt(reg, enc_dec);
    WM_HAL_CRYP_LOG_D("[%d]:3des[%d] %s %s start", wm_os_internal_get_time_ms(), len,
                      enc_dec == WM_GPSEC_ENCRYPT ? "ENCRYPT" : "DECRYPT",
                      dev->cryp_cfg.param.des3_cfg.sub_mode == WM_GPSEC_ECB ?
                          "ECB" :
                          (dev->cryp_cfg.param.des3_cfg.sub_mode == WM_GPSEC_CBC ?
                               "CBC" :
                               (dev->cryp_cfg.param.des3_cfg.sub_mode == WM_GPSEC_CTR ? "CTR" : "MAC")));

    /* start operation */
    wm_ll_gpsec_set_operation_start(reg, 1);

    /* Wait for complete flag to be raised */
    wm_hal_cryp_wait_complete(dev);
    /* Clear complete Flag */
    wm_ll_gpsec_clear_irq_flag(reg);
    WM_HAL_CRYP_LOG_D("[%d]:3des end %d", wm_os_internal_get_time_ms(), wm_ll_gpsec_get_cmplt_bytes(reg));
    return WM_ERR_SUCCESS;
}
