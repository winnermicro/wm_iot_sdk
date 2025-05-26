/**
 * @file wm_drv_hash.c
 *
 * @brief DRV_HASH Module
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
#include "wm_drv_hash.h"
#include "wm_hal_hash.h"
#undef LOG_TAG
#define LOG_TAG "drv_hash"
#include "wm_log.h"

#define WM_DRV_HASH_DEBUG

#ifdef WM_DRV_HASH_DEBUG

#define WM_DRV_HASH_TRACE_FAIL() wm_log_error("%s@%d", __func__, __LINE__)
#define WM_DRV_LOG_WARN()        wm_log_warn("%s@%d", __func__, __LINE__)

#else

#define WM_DRV_HASH_TRACE_FAIL()
#define WM_DRV_LOG_WARN()

#endif

/**
 * @brief Check param p and return r
 */
#define WM_DRV_HASH_ASSERT_PARAM_RET(p, r) \
    if (!(p)) {                            \
        WM_DRV_HASH_TRACE_FAIL();          \
        return (r);                        \
    }

/**
 * @brief Check param and return WM_ERR_INVALID_PARAM
 */
#define WM_DRV_HASH_ASSERT_PARAM(p)  \
    if (!(p)) {                      \
        WM_DRV_HASH_TRACE_FAIL();    \
        return WM_ERR_INVALID_PARAM; \
    }

#include "wm_drv_ops_hash.c"

wm_device_t *wm_drv_hash_init(const char *dev_name)
{
    int ret          = WM_ERR_SUCCESS;
    wm_device_t *dev = wm_dt_get_device_by_name(dev_name);
    WM_DRV_HASH_ASSERT_PARAM_RET(dev, NULL);
    ret = ((wm_drv_hash_ops_t *)dev->ops)->init(dev);
    return (ret == WM_ERR_ALREADY_INITED || ret == WM_ERR_SUCCESS ? dev : NULL);
}

int wm_drv_hash_deinit(wm_device_t *dev)
{
    WM_DRV_HASH_ASSERT_PARAM(dev);
    return ((wm_drv_hash_ops_t *)dev->ops)->deinit(dev);
}

int wm_drv_md5_start(wm_device_t *dev, wm_drv_hash_cfg_t *ctx)
{
    WM_DRV_HASH_ASSERT_PARAM(dev && ctx);
    return ((wm_drv_hash_ops_t *)dev->ops)->md5_start(dev, ctx);
}

int wm_drv_md5_update(wm_device_t *dev, wm_drv_hash_cfg_t *ctx, const unsigned char *in, uint32_t len)
{
    WM_DRV_HASH_ASSERT_PARAM(dev && ctx && in && len > 0);
    return ((wm_drv_hash_ops_t *)dev->ops)->md5_update(dev, ctx, in, len);
}

int wm_drv_md5_final(wm_device_t *dev, wm_drv_hash_cfg_t *ctx, unsigned char *out)
{
    WM_DRV_HASH_ASSERT_PARAM(dev && ctx && out);
    return ((wm_drv_hash_ops_t *)dev->ops)->md5_final(dev, ctx, out);
}

int wm_drv_md5(const void *in_buf, unsigned int in_len, void *md5_hash)
{
    wm_device_t *dev;
    wm_drv_hash_cfg_t ctx;
    int ret = WM_ERR_SUCCESS;

    /*initialize hash*/
    dev = wm_drv_hash_init("hash");
    if (dev == NULL) {
        WM_DRV_HASH_TRACE_FAIL();
        return WM_ERR_FAILED;
    }

    ret = wm_drv_md5_start(dev, &ctx);
    if (ret != WM_ERR_SUCCESS) {
        return ret;
    }
    ret = wm_drv_md5_update(dev, &ctx, in_buf, in_len);
    if (ret != WM_ERR_SUCCESS) {
        return ret;
    }
    ret = wm_drv_md5_final(dev, &ctx, md5_hash);
    if (ret != WM_ERR_SUCCESS) {
        return ret;
    }
    /* disable cryp clock */
    wm_drv_clock_disable(wm_dt_get_device_by_name("rcc"), WM_RCC_GPSEC_GATE_EN);
    return WM_ERR_SUCCESS;
}

int wm_drv_sha1_start(wm_device_t *dev, wm_drv_hash_cfg_t *ctx)
{
    WM_DRV_HASH_ASSERT_PARAM(dev && ctx);
    return ((wm_drv_hash_ops_t *)dev->ops)->sha1_start(dev, ctx);
}

int wm_drv_sha1_update(wm_device_t *dev, wm_drv_hash_cfg_t *ctx, const unsigned char *in, uint32_t len)
{
    WM_DRV_HASH_ASSERT_PARAM(dev && ctx && in && len > 0);
    return ((wm_drv_hash_ops_t *)dev->ops)->sha1_update(dev, ctx, in, len);
}

int wm_drv_sha1_final(wm_device_t *dev, wm_drv_hash_cfg_t *ctx, unsigned char *out)
{
    WM_DRV_HASH_ASSERT_PARAM(dev && ctx && out);
    return ((wm_drv_hash_ops_t *)dev->ops)->sha1_final(dev, ctx, out);
}
