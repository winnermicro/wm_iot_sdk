/**
 * @file wm_drv_rng.c
 *
 * @brief DRV_RNG Module
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
#include "wm_drv_rng.h"
#include "wm_hal_rng.h"

#undef LOG_TAG
#define LOG_TAG "drv_rng"
#include "wm_log.h"

#define WM_DRV_RNG_DEBUG

#ifdef WM_DRV_RNG_DEBUG

#define WM_DRV_RNG_TRACE_FAIL() wm_log_error("%s@%d", __func__, __LINE__)
#define WM_DRV_LOG_WARN()       wm_log_warn("%s@%d", __func__, __LINE__)

#else

#define WM_DRV_RNG_TRACE_FAIL()
#define WM_DRV_LOG_WARN()

#endif

/**
 * @brief Check param p and return r
 */
#define WM_DRV_RNG_ASSERT_PARAM_RET(p, r) \
    if (!(p)) {                           \
        WM_DRV_RNG_TRACE_FAIL();          \
        return (r);                       \
    }

/**
 * @brief Check param and return WM_ERR_INVALID_PARAM
 */
#define WM_DRV_RNG_ASSERT_PARAM(p)   \
    if (!(p)) {                      \
        WM_DRV_RNG_TRACE_FAIL();     \
        return WM_ERR_INVALID_PARAM; \
    }

#include "wm_drv_ops_rng.c"

wm_device_t *wm_drv_rng_init(const char *dev_name)
{
    int ret          = WM_ERR_SUCCESS;
    wm_device_t *dev = wm_dt_get_device_by_name(dev_name);
    WM_DRV_RNG_ASSERT_PARAM_RET(dev, NULL);
    ret = ((wm_drv_rng_ops_t *)dev->ops)->init(dev);
    return (ret == WM_ERR_ALREADY_INITED || ret == WM_ERR_SUCCESS ? dev : NULL);
}

int wm_drv_rng_deinit(wm_device_t *dev)
{
    WM_DRV_RNG_ASSERT_PARAM(dev);
    return ((wm_drv_rng_ops_t *)dev->ops)->deinit(dev);
}

uint32_t wm_drv_rng_read(wm_device_t *dev)
{
    uint32_t ret = 0;
    if (!(dev)) {
        WM_DRV_RNG_TRACE_FAIL();
        return ret;
    }

    ret = ((wm_drv_rng_ops_t *)dev->ops)->rng_read(dev);
    return ret;
}

int wm_drv_rng_read_bytes(wm_device_t *dev, void *buf, uint32_t len)
{
    WM_DRV_RNG_ASSERT_PARAM(dev && buf && len > 0);
    return ((wm_drv_rng_ops_t *)dev->ops)->rng_read_bytes(dev, buf, len);
}

int wm_drv_fill_random(void *buf, uint32_t len)
{
    wm_device_t *dev;

    /*initialize rng*/
    dev = wm_drv_rng_init("rng");
    WM_DRV_RNG_ASSERT_PARAM(dev);
    /* generate random */
    return wm_drv_rng_read_bytes(dev, buf, len);
}
