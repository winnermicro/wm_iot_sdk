/**
 * @file wm_drv_cryp.c
 *
 * @brief DRV_CRYP Module
 *
 */

/**
 *  Copyright 2022-2024 Beijing WinnerMicroelectronics Co.,Ltd.
 *
 *  Licensed under the Apache License, Version 2.0 (the "License"){}
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
#include "wmsdk_config.h"
#include "wm_types.h"
#include "wm_error.h"
#include "wm_osal.h"

#include "wm_dt_hw.h"
#include "wm_dt.h"
#include "wm_drv_crypto.h"
#include "wm_hal_crypto.h"

#undef LOG_TAG
#define LOG_TAG "drv_crypto"
#include "wm_log.h"

#define WM_DRV_CRYP_DEBUG

#ifdef WM_DRV_CRYP_DEBUG

#define WM_DRV_CRYP_TRACE_FAIL() wm_log_error("%s@%d", __func__, __LINE__)
#define WM_DRV_LOG_WARN()        wm_log_warn("%s@%d", __func__, __LINE__)

#else

#define WM_DRV_CRYP_TRACE_FAIL()
#define WM_DRV_LOG_WARN()

#endif

/**
 * @brief Check param p and return r
 */
#define WM_DRV_CRYP_ASSERT_PARAM_RET(p, r) \
    if (!(p)) {                            \
        WM_DRV_CRYP_TRACE_FAIL();          \
        return (r);                        \
    }

/**
 * @brief Check param and return WM_ERR_INVALID_PARAM
 */
#define WM_DRV_CRYP_ASSERT_PARAM(p)  \
    if (!(p)) {                      \
        WM_DRV_CRYP_TRACE_FAIL();    \
        return WM_ERR_INVALID_PARAM; \
    }

#include "wm_drv_ops_crypto.c"

wm_device_t *wm_drv_cryp_init(const char *dev_name)
{
    int ret          = WM_ERR_SUCCESS;
    wm_device_t *dev = wm_dt_get_device_by_name(dev_name);
    WM_DRV_CRYP_ASSERT_PARAM_RET(dev, NULL);
    ret = ((wm_drv_cryp_ops_t *)dev->ops)->init(dev);
    return (ret == WM_ERR_ALREADY_INITED || ret == WM_ERR_SUCCESS ? dev : NULL);
}

int wm_drv_cryp_deinit(wm_device_t *dev)
{
    WM_DRV_CRYP_ASSERT_PARAM(dev);
    return ((wm_drv_cryp_ops_t *)dev->ops)->deinit(dev);
}

int wm_drv_rc4_init(wm_device_t *dev, wm_drv_cryp_cfg_t *ctx, const unsigned char *key, uint32_t keylen)
{
    WM_DRV_CRYP_ASSERT_PARAM(dev && ctx && key && IS_RC4_KEYSIZE(keylen));
    return ((wm_drv_cryp_ops_t *)dev->ops)->rc4_init(dev, ctx, key, keylen);
}

int wm_drv_aes_init(wm_device_t *dev, wm_drv_cryp_cfg_t *ctx, const unsigned char *iv, const unsigned char *key,
                    uint32_t keylen, wm_gpsec_sub_mode_t sub_mode)
{
    WM_DRV_CRYP_ASSERT_PARAM(dev && ctx && (iv || sub_mode == WM_GPSEC_ECB) && key && IS_AES_KEYSIZE(keylen) &&
                             IS_AES_SUBMODE(sub_mode));
    return ((wm_drv_cryp_ops_t *)dev->ops)->aes_init(dev, ctx, iv, key, keylen, sub_mode);
}

int wm_drv_des_init(wm_device_t *dev, wm_drv_cryp_cfg_t *ctx, const unsigned char *iv, const unsigned char *key,
                    uint32_t keylen, wm_gpsec_sub_mode_t sub_mode)
{
    WM_DRV_CRYP_ASSERT_PARAM(dev && ctx && iv && key && IS_DES_KEYSIZE(keylen) && IS_DES_SUBMODE(sub_mode));
    return ((wm_drv_cryp_ops_t *)dev->ops)->des_init(dev, ctx, iv, key, keylen, sub_mode);
}

int wm_drv_3des_init(wm_device_t *dev, wm_drv_cryp_cfg_t *ctx, const unsigned char *iv, const unsigned char *key,
                     uint32_t keylen, wm_gpsec_sub_mode_t sub_mode)
{
    WM_DRV_CRYP_ASSERT_PARAM(dev && ctx && iv && key && IS_3DES_KEYSIZE(keylen) && IS_DES_SUBMODE(sub_mode));
    return ((wm_drv_cryp_ops_t *)dev->ops)->des3_init(dev, ctx, iv, key, keylen, sub_mode);
}

int wm_drv_rc4_encrypt_decrypt(wm_device_t *dev, wm_drv_cryp_cfg_t *ctx, unsigned char *in, unsigned char *out, uint32_t len)
{
    WM_DRV_CRYP_ASSERT_PARAM(dev && ctx && in && out && len > 0);
    return ((wm_drv_cryp_ops_t *)dev->ops)->cryp_rc4_op(dev, ctx, in, out, len);
}

int wm_drv_aes_encrypt_decrypt(wm_device_t *dev, wm_drv_cryp_cfg_t *ctx, unsigned char *in, unsigned char *out, uint32_t len,
                               wm_gpsec_encrypt_decrypt_sel_t enc_dec)

{
    WM_DRV_CRYP_ASSERT_PARAM(dev && ctx && in && out && IS_AES_BUFFERSIZE(len) && IS_CRYP_OP(enc_dec));
    return ((wm_drv_cryp_ops_t *)dev->ops)->cryp_aes_op(dev, ctx, in, out, len, enc_dec);
}

int wm_drv_des_encrypt_decrypt(wm_device_t *dev, wm_drv_cryp_cfg_t *ctx, unsigned char *in, unsigned char *out, uint32_t len,
                               wm_gpsec_encrypt_decrypt_sel_t enc_dec)
{
    WM_DRV_CRYP_ASSERT_PARAM(dev && ctx && in && out && IS_DES_BUFFERSIZE(len) && IS_CRYP_OP(enc_dec));
    return ((wm_drv_cryp_ops_t *)dev->ops)->cryp_des_op(dev, ctx, in, out, len, enc_dec);
}

int wm_drv_3des_encrypt_decrypt(wm_device_t *dev, wm_drv_cryp_cfg_t *ctx, unsigned char *in, unsigned char *out, uint32_t len,
                                wm_gpsec_encrypt_decrypt_sel_t enc_dec)
{
    WM_DRV_CRYP_ASSERT_PARAM(dev && ctx && in && out && IS_DES_BUFFERSIZE(len) && IS_CRYP_OP(enc_dec));
    return ((wm_drv_cryp_ops_t *)dev->ops)->cryp_3des_op(dev, ctx, in, out, len, enc_dec);
}
