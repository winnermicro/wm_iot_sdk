/**
 * @file wm_drv_ops_cryp.c
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
#include "wm_drv_rcc.h"

#include "wm_hal_crypto.h"

typedef struct {
    int (*init)(wm_device_t *dev);
    int (*deinit)(wm_device_t *dev);
    int (*rc4_init)(wm_device_t *dev, wm_drv_cryp_cfg_t *ctx, const unsigned char *key, uint32_t keylen);
    int (*aes_init)(wm_device_t *dev, wm_drv_cryp_cfg_t *ctx, const unsigned char *iv, const unsigned char *key,
                    uint32_t keylen, wm_gpsec_sub_mode_t sub_mode);
    int (*des_init)(wm_device_t *dev, wm_drv_cryp_cfg_t *ctx, const unsigned char *iv, const unsigned char *key,
                    uint32_t keylen, wm_gpsec_sub_mode_t sub_mode);
    int (*des3_init)(wm_device_t *dev, wm_drv_cryp_cfg_t *ctx, const unsigned char *iv, const unsigned char *key,
                     uint32_t keylen, wm_gpsec_sub_mode_t sub_mode);
    int (*cryp_rc4_op)(wm_device_t *dev, wm_drv_cryp_cfg_t *ctx, unsigned char *in, unsigned char *out, uint32_t len);
    int (*cryp_aes_op)(wm_device_t *dev, wm_drv_cryp_cfg_t *ctx, unsigned char *in, unsigned char *out, uint32_t len,
                       wm_gpsec_encrypt_decrypt_sel_t enc_dec);
    int (*cryp_des_op)(wm_device_t *dev, wm_drv_cryp_cfg_t *ctx, unsigned char *in, unsigned char *out, uint32_t len,
                       wm_gpsec_encrypt_decrypt_sel_t enc_dec);
    int (*cryp_3des_op)(wm_device_t *dev, wm_drv_cryp_cfg_t *ctx, unsigned char *in, unsigned char *out, uint32_t len,
                        wm_gpsec_encrypt_decrypt_sel_t enc_dec);
} wm_drv_cryp_ops_t;

/**
  * @brief  CRYP driver layer device struct
  */
typedef struct {
    wm_hal_cryp_dev_t hal_dev;
    wm_os_mutex_t *mutex;
    wm_device_t *clock_dev;
} wm_drv_cryp_data_t;

/**
  * @brief  CRYP device struct in dt
  */
typedef struct {
    char *name;               /**< device name                      */
    wm_dt_hw_crypto_t *hw;    /**< hardware info, @ref wm_dt_hw_crypto_t  */
    wm_drv_cryp_ops_t *ops;   /**< device operation interface       */
    wm_drv_cryp_data_t *drv;  /**< driver context data              */
    wm_device_status_t state; /**< device state                     */
    void *priv;               /**< user private data                */
} wm_dt_cryp_dev_t;

/**
  * @brief gpsec mutex ops
  * Note:In W800, RNG/SHA/MD5/CRC/AES/DES/3DES share the same hardware module,
  *      therefore, only one function can be implemented at any given time.
  */
extern wm_os_status_t wm_drv_creat_gpsec_mutex(void);
extern wm_os_status_t wm_drv_acquire_gpsec_mutex(uint32_t wait_time);
extern wm_os_status_t wm_drv_release_gpsec_mutex(void);

static int wm_w800_drv_cryp_init(wm_device_t *device)
{
    wm_dt_cryp_dev_t *dev       = (wm_dt_cryp_dev_t *)device;
    wm_drv_cryp_data_t *drv_dev = NULL;
    wm_hal_cryp_dev_t *hal_dev  = NULL;

    /* creat gpsec mutex if needed */
    if (wm_drv_creat_gpsec_mutex() != WM_OS_STATUS_SUCCESS) {
        WM_DRV_CRYP_TRACE_FAIL();
        return WM_ERR_FAILED;
    }
    /* ensure that the module has not been initialized and is not in the process of initialization. */
    if (dev->state == WM_DEV_ST_INITED) {
        return WM_ERR_ALREADY_INITED;
    }
    if (dev->state == WM_DEV_ST_INITING) {
        return WM_ERR_BUSY;
    }
    dev->state = WM_DEV_ST_INITING;

    /* calloc driver layer device drv */
    drv_dev = (wm_drv_cryp_data_t *)dev->drv;
    if (drv_dev == NULL) {
        drv_dev = calloc(1, sizeof(wm_drv_cryp_data_t));
        if (!drv_dev) {
            WM_DRV_CRYP_TRACE_FAIL();
            dev->state = WM_DEV_ST_UNINIT;
            return WM_ERR_NO_MEM;
        }
    }

    /* get cryp clock dev*/
    if ((drv_dev->clock_dev = wm_dt_get_device_by_name(dev->hw->rcc_device_name)) == NULL) {
        free(drv_dev);
        drv_dev = NULL;
        WM_DRV_CRYP_TRACE_FAIL();
        dev->state = WM_DEV_ST_UNINIT;
        return WM_ERR_FAILED;
    }

    /* init hal dev addr with hw */
    hal_dev            = &(drv_dev->hal_dev);
    hal_dev->gpsec_reg = (wm_gpsec_reg_t *)(dev->hw->reg_base);

    dev->drv = drv_dev;
    /* change state to WM_DEV_ST_INITED */
    dev->state = WM_DEV_ST_INITED;

    return WM_ERR_SUCCESS;
}

static int wm_w800_drv_cryp_deinit(wm_device_t *device)
{
    wm_dt_cryp_dev_t *dev = (wm_dt_cryp_dev_t *)device;

    /* make sure self module is already inited */
    if (dev->state == WM_DEV_ST_UNINIT || dev->drv == NULL) {
        WM_DRV_LOG_WARN();
        return WM_ERR_NO_INITED;
    }

    /*free driver layer device */
    free(dev->drv);
    dev->drv = NULL;

    /* change state to WM_DEV_ST_UNINIT */
    dev->state = WM_DEV_ST_UNINIT;

    return WM_ERR_SUCCESS;
}

int wm_w800_drv_rc4_init(wm_device_t *device, wm_drv_cryp_cfg_t *ctx, const unsigned char *key, uint32_t keylen)
{
    wm_dt_cryp_dev_t *dev = (wm_dt_cryp_dev_t *)(device);
    /* check some input params */
    WM_DRV_CRYP_ASSERT_PARAM(dev->drv);

    /* set configuration to ctx*/
    memset(ctx, 0, sizeof(wm_drv_cryp_cfg_t));
    memcpy(ctx->param.rc4_cfg.key, key, keylen);
    ctx->param.rc4_cfg.key_len = keylen;
    ctx->alg                   = CRYP_ALG_RC4;
    return WM_ERR_SUCCESS;
}

int wm_w800_drv_aes_init(wm_device_t *device, wm_drv_cryp_cfg_t *ctx, const unsigned char *iv, const unsigned char *key,
                         uint32_t keylen, wm_gpsec_sub_mode_t sub_mode)
{
    wm_dt_cryp_dev_t *dev = (wm_dt_cryp_dev_t *)(device);
    /* check some input params */
    WM_DRV_CRYP_ASSERT_PARAM(dev->drv);

    /* set configuration to ctx*/
    memset(ctx, 0, sizeof(wm_drv_cryp_cfg_t));
    memcpy(ctx->param.aes_cfg.key.skey, key, keylen);
    ctx->param.aes_cfg.key.key_len = keylen;
    memcpy(ctx->param.aes_cfg.iv, iv, AES_IV_LEN);
    ctx->param.aes_cfg.sub_mode = sub_mode;
    ctx->alg                    = CRYP_ALG_AES;
    return WM_ERR_SUCCESS;
}

int wm_w800_drv_des_init(wm_device_t *device, wm_drv_cryp_cfg_t *ctx, const unsigned char *iv, const unsigned char *key,
                         uint32_t keylen, wm_gpsec_sub_mode_t sub_mode)
{
    wm_dt_cryp_dev_t *dev = (wm_dt_cryp_dev_t *)(device);
    /* check some input params */
    WM_DRV_CRYP_ASSERT_PARAM(dev->drv);

    /* set configuration to ctx*/
    memset(ctx, 0, sizeof(wm_drv_cryp_cfg_t));
    memcpy(ctx->param.des_cfg.key.ek[0], key, keylen);
    memcpy(ctx->param.des_cfg.iv, iv, DES_IV_LEN);
    ctx->param.des_cfg.sub_mode = sub_mode;
    ctx->alg                    = CRYP_ALG_DES;
    return WM_ERR_SUCCESS;
}

int wm_w800_drv_3des_init(wm_device_t *device, wm_drv_cryp_cfg_t *ctx, const unsigned char *iv, const unsigned char *key,
                          uint32_t keylen, wm_gpsec_sub_mode_t sub_mode)
{
    wm_dt_cryp_dev_t *dev = (wm_dt_cryp_dev_t *)(device);
    /* check some input params */
    WM_DRV_CRYP_ASSERT_PARAM(dev->drv);

    /* set configuration to ctx*/
    memset(ctx, 0, sizeof(wm_drv_cryp_cfg_t));
    memcpy(ctx->param.des3_cfg.key.ek[0], key, keylen);
    memcpy(ctx->param.des3_cfg.iv, iv, DES3_IV_LEN);
    ctx->param.des3_cfg.sub_mode = sub_mode;
    ctx->alg                     = CRYP_ALG_3DES;
    return WM_ERR_SUCCESS;
}

int wm_w800_drv_rc4_encrypt_decrypt(wm_device_t *device, wm_drv_cryp_cfg_t *ctx, unsigned char *in, unsigned char *out,
                                    uint32_t len)
{
    wm_dt_cryp_dev_t *dev = (wm_dt_cryp_dev_t *)(device);
    wm_hal_cryp_dev_t *hal_dev;
    int ret;

    /* check some input params */
    WM_DRV_CRYP_ASSERT_PARAM(dev->drv);

    /* LOCK GPSEC resource */
    if (wm_drv_acquire_gpsec_mutex(WM_OS_WAIT_TIME_MAX) != WM_OS_STATUS_SUCCESS) {
        WM_DRV_CRYP_TRACE_FAIL();
        return WM_ERR_FAILED;
    }

    /* enable cryp clock */
    wm_drv_clock_enable(dev->drv->clock_dev, WM_RCC_GPSEC_GATE_EN);

    /* set configuration to hal dev */
    hal_dev = &dev->drv->hal_dev;
    memcpy(&hal_dev->cryp_cfg, ctx, sizeof(wm_drv_cryp_cfg_t));

    /* resort to hal function */
    ret = wm_hal_rc4_encrypt_decrypt(hal_dev, in, out, len);

    /* disable cryp clock */
    wm_drv_clock_disable(dev->drv->clock_dev, WM_RCC_GPSEC_GATE_EN);

    /* UNLOCK GPSEC resource */
    if (wm_drv_release_gpsec_mutex() != WM_OS_STATUS_SUCCESS) {
        WM_DRV_CRYP_TRACE_FAIL();
        return WM_ERR_FAILED;
    }

    return ret;
}

int wm_w800_drv_aes_encrypt_decrypt(wm_device_t *device, wm_drv_cryp_cfg_t *ctx, unsigned char *in, unsigned char *out,
                                    uint32_t len, wm_gpsec_encrypt_decrypt_sel_t enc_dec)
{
    wm_dt_cryp_dev_t *dev = (wm_dt_cryp_dev_t *)(device);
    wm_hal_cryp_dev_t *hal_dev;
    int ret;

    /* check some input params */
    WM_DRV_CRYP_ASSERT_PARAM(dev && dev->drv);

    /* LOCK GPSEC resource */
    if (wm_drv_acquire_gpsec_mutex(WM_OS_WAIT_TIME_MAX) != WM_OS_STATUS_SUCCESS) {
        WM_DRV_CRYP_TRACE_FAIL();
        return WM_ERR_FAILED;
    }

    /* enable cryp clock */
    wm_drv_clock_enable(dev->drv->clock_dev, WM_RCC_GPSEC_GATE_EN);

    /* set configuration to hal dev */
    hal_dev = &dev->drv->hal_dev;
    memcpy(&hal_dev->cryp_cfg, ctx, sizeof(wm_drv_cryp_cfg_t));

    /* resort to hal function */
    ret = wm_hal_aes_encrypt_decrypt(hal_dev, in, out, len, enc_dec);

    /* disable cryp clock */
    wm_drv_clock_disable(dev->drv->clock_dev, WM_RCC_GPSEC_GATE_EN);

    /* UNLOCK GPSEC resource */
    if (wm_drv_release_gpsec_mutex() != WM_OS_STATUS_SUCCESS) {
        WM_DRV_CRYP_TRACE_FAIL();
        return WM_ERR_FAILED;
    }

    return ret;
}

int wm_w800_drv_des_encrypt_decrypt(wm_device_t *device, wm_drv_cryp_cfg_t *ctx, unsigned char *in, unsigned char *out,
                                    uint32_t len, wm_gpsec_encrypt_decrypt_sel_t enc_dec)
{
    wm_dt_cryp_dev_t *dev = (wm_dt_cryp_dev_t *)(device);
    wm_hal_cryp_dev_t *hal_dev;
    int ret;

    /* check some input params */
    WM_DRV_CRYP_ASSERT_PARAM(dev->drv);

    /* LOCK GPSEC resource */
    if (wm_drv_acquire_gpsec_mutex(WM_OS_WAIT_TIME_MAX) != WM_OS_STATUS_SUCCESS) {
        WM_DRV_CRYP_TRACE_FAIL();
        return WM_ERR_FAILED;
    }

    /* enable cryp clock */
    wm_drv_clock_enable(dev->drv->clock_dev, WM_RCC_GPSEC_GATE_EN);

    /* set configuration to hal dev */
    hal_dev = &dev->drv->hal_dev;
    memcpy(&hal_dev->cryp_cfg, ctx, sizeof(wm_drv_cryp_cfg_t));

    /* resort to hal function */
    ret = wm_hal_des_encrypt_decrypt(hal_dev, in, out, len, enc_dec);

    /* disable cryp clock */
    wm_drv_clock_disable(dev->drv->clock_dev, WM_RCC_GPSEC_GATE_EN);

    /* UNLOCK GPSEC resource */
    if (wm_drv_release_gpsec_mutex() != WM_OS_STATUS_SUCCESS) {
        WM_DRV_CRYP_TRACE_FAIL();
        return WM_ERR_FAILED;
    }

    return ret;
}
int wm_w800_drv_3des_encrypt_decrypt(wm_device_t *device, wm_drv_cryp_cfg_t *ctx, unsigned char *in, unsigned char *out,
                                     uint32_t len, wm_gpsec_encrypt_decrypt_sel_t enc_dec)
{
    wm_dt_cryp_dev_t *dev = (wm_dt_cryp_dev_t *)(device);
    wm_hal_cryp_dev_t *hal_dev;
    int ret;

    /* check some input params */
    WM_DRV_CRYP_ASSERT_PARAM(dev->drv);

    /* LOCK GPSEC resource */
    if (wm_drv_acquire_gpsec_mutex(WM_OS_WAIT_TIME_MAX) != WM_OS_STATUS_SUCCESS) {
        WM_DRV_CRYP_TRACE_FAIL();
        return WM_ERR_FAILED;
    }

    /* enable cryp clock */
    wm_drv_clock_enable(dev->drv->clock_dev, WM_RCC_GPSEC_GATE_EN);

    /* set configuration to hal dev */
    hal_dev = &dev->drv->hal_dev;
    memcpy(&hal_dev->cryp_cfg, ctx, sizeof(wm_drv_cryp_cfg_t));

    /* resort to hal function */
    ret = wm_hal_3des_encrypt_decrypt(hal_dev, in, out, len, enc_dec);

    /* disable cryp clock */
    wm_drv_clock_disable(dev->drv->clock_dev, WM_RCC_GPSEC_GATE_EN);

    /* UNLOCK GPSEC resource */
    if (wm_drv_release_gpsec_mutex() != WM_OS_STATUS_SUCCESS) {
        WM_DRV_CRYP_TRACE_FAIL();
        return WM_ERR_FAILED;
    }

    return ret;
}

/* CRYP driver operations */
const wm_drv_cryp_ops_t wm_drv_crypto_ops = {
    .init         = wm_w800_drv_cryp_init,
    .deinit       = wm_w800_drv_cryp_deinit,
    .rc4_init     = wm_w800_drv_rc4_init,
    .aes_init     = wm_w800_drv_aes_init,
    .des_init     = wm_w800_drv_des_init,
    .des3_init    = wm_w800_drv_3des_init,
    .cryp_rc4_op  = wm_w800_drv_rc4_encrypt_decrypt,
    .cryp_aes_op  = wm_w800_drv_aes_encrypt_decrypt,
    .cryp_des_op  = wm_w800_drv_des_encrypt_decrypt,
    .cryp_3des_op = wm_w800_drv_3des_encrypt_decrypt,
};
