/**
 * @file wm_drv_crc.c
 *
 * @brief DRV_CRC Module
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
#include "wm_drv_crc.h"
#include "wm_hal_crc.h"

#undef LOG_TAG
#define LOG_TAG "drv_crc"
#include "wm_log.h"

#define WM_DRV_CRC_DEBUG

#ifdef WM_DRV_CRC_DEBUG

#define WM_DRV_CRC_TRACE_FAIL() wm_log_error("%s@%d", __func__, __LINE__)
#define WM_DRV_LOG_WARN()       wm_log_warn("%s@%d", __func__, __LINE__)

#else

#define WM_DRV_CRC_TRACE_FAIL()
#define WM_DRV_LOG_WARN()

#endif

/**
 * @brief Check param p and return r
 */
#define WM_DRV_CRC_ASSERT_PARAM_RET(p, r) \
    if (!(p)) {                           \
        WM_DRV_CRC_TRACE_FAIL();          \
        return (r);                       \
    }

/**
 * @brief Check param and return WM_ERR_INVALID_PARAM
 */
#define WM_DRV_CRC_ASSERT_PARAM(p)   \
    if (!(p)) {                      \
        WM_DRV_CRC_TRACE_FAIL();     \
        return WM_ERR_INVALID_PARAM; \
    }

#include "wm_drv_ops_crc.c"

wm_device_t *wm_drv_crc_init(const char *dev_name)
{
    int ret          = WM_ERR_SUCCESS;
    wm_device_t *dev = wm_dt_get_device_by_name(dev_name);
    WM_DRV_CRC_ASSERT_PARAM_RET(dev, NULL);
    ret = ((wm_drv_crc_ops_t *)dev->ops)->init(dev);
    return (ret == WM_ERR_ALREADY_INITED || ret == WM_ERR_SUCCESS ? dev : NULL);
}

int wm_drv_crc_deinit(wm_device_t *dev)
{
    WM_DRV_CRC_ASSERT_PARAM(dev);
    return ((wm_drv_crc_ops_t *)dev->ops)->deinit(dev);
}

int wm_drv_crc_cfg(wm_device_t *dev, wm_drv_crc_cfg_t *ctx, uint32_t state, uint8_t type, uint8_t reverse)
{
    WM_DRV_CRC_ASSERT_PARAM(dev && ctx && IS_CRC_TYPE(type) && IS_CRC_REVERSE(reverse));
    return ((wm_drv_crc_ops_t *)dev->ops)->crc_cfg(dev, ctx, state, type, reverse);
}

int wm_drv_crc_update(wm_device_t *dev, wm_drv_crc_cfg_t *ctx, unsigned char *in, uint32_t len)
{
    WM_DRV_CRC_ASSERT_PARAM(dev && ctx && in && len > 0);
    return ((wm_drv_crc_ops_t *)dev->ops)->crc_update(dev, ctx, in, len);
}

int wm_drv_crc_final(wm_device_t *dev, wm_drv_crc_cfg_t *ctx, uint32_t *crc_val)
{
    WM_DRV_CRC_ASSERT_PARAM(dev && ctx && crc_val);
    return ((wm_drv_crc_ops_t *)dev->ops)->crc_final(dev, ctx, crc_val);
}

uint32_t wm_drv_crc32_reverse(const void *buf, unsigned int len)
{
    wm_drv_crc_cfg_t ctx;
    wm_device_t *dev  = NULL;
    uint32_t crcvalue = 0;
    if (buf == NULL || len == 0) {
        WM_DRV_CRC_TRACE_FAIL();
        return crcvalue;
    }

    dev = wm_drv_crc_init("crc");
    wm_drv_crc_cfg(dev, &ctx, 0xFFFFFFFF, WM_GPSEC_CRC32, WM_GPSEC_CRC_OUT_IN_REVERSE);
    wm_drv_crc_update(dev, &ctx, (unsigned char *)buf, len);
    wm_drv_crc_final(dev, &ctx, &crcvalue);
    return crcvalue;
}
