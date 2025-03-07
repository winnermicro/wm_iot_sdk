/**
 * @file wm_drv_ops_hash.c
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
#include "wm_drv_rcc.h"

#include "wm_hal_hash.h"

typedef struct {
    int (*init)(wm_device_t *dev);
    int (*deinit)(wm_device_t *dev);
    int (*md5_start)(wm_device_t *dev, wm_drv_hash_cfg_t *ctx);
    int (*md5_update)(wm_device_t *dev, wm_drv_hash_cfg_t *ctx, const unsigned char *in, uint32_t len);
    int (*md5_final)(wm_device_t *dev, wm_drv_hash_cfg_t *ctx, unsigned char *out);
    int (*sha1_start)(wm_device_t *dev, wm_drv_hash_cfg_t *ctx);
    int (*sha1_update)(wm_device_t *dev, wm_drv_hash_cfg_t *ctx, const unsigned char *in, uint32_t len);
    int (*sha1_final)(wm_device_t *dev, wm_drv_hash_cfg_t *ctx, unsigned char *out);
} wm_drv_hash_ops_t;

/**
  * @brief  HASH driver layer device struct
  */
typedef struct {
    wm_hal_hash_dev_t hal_dev;
    wm_os_mutex_t *mutex;
    wm_device_t *clock_dev;
} wm_drv_hash_data_t;

/**
  * @brief  HASH device struct in dt
  */
typedef struct {
    char *name;               /**< device name                      */
    wm_dt_hw_hash_t *hw;      /**< hardware info, @ref wm_dt_hw_hash_t  */
    wm_drv_hash_ops_t *ops;   /**< device operation interface       */
    wm_drv_hash_data_t *drv;  /**< driver context data              */
    wm_device_status_t state; /**< device state                     */
    void *priv;               /**< user private data                */
} wm_dt_hash_dev_t;

/**
  * @brief gpsec mutex ops
  * Note:In W800, RNG/SHA/MD5/CRC/AES/DES/3DES share the same hardware module,
  *      therefore, only one function can be implemented at any given time.
  */
wm_os_mutex_t *mutex_for_gpsec = NULL;
wm_os_status_t wm_drv_creat_gpsec_mutex(void)
{
    if (mutex_for_gpsec == NULL) {
        return wm_os_internal_mutex_create(&mutex_for_gpsec);
    }
    return WM_OS_STATUS_SUCCESS;
}
wm_os_status_t wm_drv_acquire_gpsec_mutex(uint32_t wait_time)
{
    if (mutex_for_gpsec == NULL) {
        return WM_OS_STATUS_SUCCESS;
    }
    return wm_os_internal_mutex_acquire(mutex_for_gpsec, wait_time);
}

wm_os_status_t wm_drv_release_gpsec_mutex(void)
{
    if (mutex_for_gpsec == NULL) {
        return WM_OS_STATUS_SUCCESS;
    }
    return wm_os_internal_mutex_release(mutex_for_gpsec);
}

static int wm_w800_drv_hash_init(wm_device_t *device)
{
    wm_dt_hash_dev_t *dev       = (wm_dt_hash_dev_t *)device;
    wm_drv_hash_data_t *drv_dev = NULL;
    wm_hal_hash_dev_t *hal_dev  = NULL;

    /* ensure that the module has not been initialized and is not in the process of initialization. */
    if (dev->state == WM_DEV_ST_INITED) {
        return WM_ERR_ALREADY_INITED;
    }
    if (dev->state == WM_DEV_ST_INITING) {
        WM_DRV_HASH_TRACE_FAIL();
        return WM_ERR_BUSY;
    }
    dev->state = WM_DEV_ST_INITING;

    /* creat gpsec mutex if needed */
    if (wm_drv_creat_gpsec_mutex() != WM_OS_STATUS_SUCCESS) {
        WM_DRV_HASH_TRACE_FAIL();
        return WM_ERR_FAILED;
    }

    /* calloc driver layer device drv */
    drv_dev = (wm_drv_hash_data_t *)dev->drv;
    if (drv_dev == NULL) {
        drv_dev = calloc(1, sizeof(wm_drv_hash_data_t));
        if (!drv_dev) {
            WM_DRV_HASH_TRACE_FAIL();
            dev->state = WM_DEV_ST_UNINIT;
            return WM_ERR_NO_MEM;
        }
    }

    /* get hash clock dev*/
    if ((drv_dev->clock_dev = wm_dt_get_device_by_name(dev->hw->rcc_device_name)) == NULL) {
        free(drv_dev);
        drv_dev = NULL;
        WM_DRV_HASH_TRACE_FAIL();
        dev->state = WM_DEV_ST_UNINIT;
        return WM_ERR_FAILED;
    }

    /* init hal dev attribute with hw */
    hal_dev            = &(drv_dev->hal_dev);
    hal_dev->gpsec_reg = (wm_gpsec_reg_t *)(dev->hw->reg_base);

    dev->drv = drv_dev;
    /* change state to WM_DEV_ST_INITED */
    dev->state = WM_DEV_ST_INITED;

    return WM_ERR_SUCCESS;
}

static int wm_w800_drv_hash_deinit(wm_device_t *device)
{
    wm_dt_hash_dev_t *dev = (wm_dt_hash_dev_t *)device;

    /* make sure self module is already inited */
    if (dev->state == WM_DEV_ST_UNINIT || dev->drv == NULL) {
        WM_DRV_LOG_WARN();
        return WM_ERR_NO_INITED;
    }

    /* disable hash clock */
    wm_drv_clock_disable(dev->drv->clock_dev, WM_RCC_GPSEC_GATE_EN);

    /*free driver layer device */
    free(dev->drv);
    dev->drv = NULL;

    /* change state to WM_DEV_ST_UNINIT */
    dev->state = WM_DEV_ST_UNINIT;

    return WM_ERR_SUCCESS;
}

static int wm_w800_drv_hash_update(wm_device_t *device, wm_drv_hash_cfg_t *hash_cfg, const unsigned char *buf, uint32_t len)
{
    wm_dt_hash_dev_t *dev      = (wm_dt_hash_dev_t *)(device);
    wm_hal_hash_dev_t *hal_dev = &dev->drv->hal_dev;
    uint32_t n;

    /* LOCK GPSEC resource */
    if (wm_drv_acquire_gpsec_mutex(WM_OS_WAIT_TIME_MAX) != WM_OS_STATUS_SUCCESS) {
        WM_DRV_HASH_TRACE_FAIL();
        return WM_ERR_FAILED;
    }

    while (len > 0) {
        n = min(len, (WM_HAL_HASH_BLOCK_BYTES - hash_cfg->curlen));
        memcpy(hash_cfg->buf + hash_cfg->curlen, buf, (size_t)n);
        hash_cfg->curlen += n;
        buf += n;
        len -= n;

        /* is 64 bytes full? */
        if (hash_cfg->curlen == WM_HAL_HASH_BLOCK_BYTES) {
            if (wm_hal_hash_compress(hal_dev, hash_cfg) != WM_ERR_SUCCESS) {
                WM_DRV_HASH_TRACE_FAIL();
                return WM_ERR_TIMEOUT;
            }
#ifdef HAVE_NATIVE_INT64
            hash_cfg->length += 512;
#else
            n = (hash_cfg->lengthLo + 512) & 0xFFFFFFFFL;
            if (n < hash_cfg->lengthLo) {
                hash_cfg->lengthHi++;
            }
            hash_cfg->lengthLo = n;
#endif /* HAVE_NATIVE_INT64 */
            hash_cfg->curlen = 0;
        }
    }

    /* UNLOCK GPSEC resource */
    if (wm_drv_release_gpsec_mutex() != WM_OS_STATUS_SUCCESS) {
        WM_DRV_HASH_TRACE_FAIL();
        return WM_ERR_FAILED;
    }

    return WM_ERR_SUCCESS;
}

static int wm_w800_drv_hash_final(wm_device_t *device, wm_drv_hash_cfg_t *hash_cfg, unsigned char *out)
{
    wm_dt_hash_dev_t *dev      = (wm_dt_hash_dev_t *)(device);
    wm_hal_hash_dev_t *hal_dev = &dev->drv->hal_dev;

    int32_t i;
#ifndef HAVE_NATIVE_INT64
    uint32_t n;
#endif
    if (hash_cfg->curlen >= sizeof(hash_cfg->buf) || out == NULL) {
        WM_DRV_HASH_TRACE_FAIL();
        return WM_ERR_INVALID_PARAM;
    }

    /*
     * increase the length of the message
     */
#ifdef HAVE_NATIVE_INT64
    hash_cfg->length += hash_cfg->curlen << 3;
#else
    n = (hash_cfg->lengthLo + (hash_cfg->curlen << 3)) & 0xFFFFFFFFL;
    if (n < hash_cfg->lengthLo) {
        hash_cfg->lengthHi++;
    }
    hash_cfg->lengthHi += (hash_cfg->curlen >> 29);
    hash_cfg->lengthLo = n;
#endif /* HAVE_NATIVE_INT64 */

    /*
     * append the '1' bit
     */
    hash_cfg->buf[hash_cfg->curlen++] = (unsigned char)0x80;

    /* LOCK GPSEC resource */
    if (wm_drv_acquire_gpsec_mutex(WM_OS_WAIT_TIME_MAX) != WM_OS_STATUS_SUCCESS) {
        WM_DRV_HASH_TRACE_FAIL();
        return WM_ERR_FAILED;
    }

    /*
     * if the length is currently above 56 bytes we append zeros then compress.
     * Then we can fall back to padding zeros and length encoding like normal.
     */
    if (hash_cfg->curlen > 56) {
        while (hash_cfg->curlen < 64) {
            hash_cfg->buf[hash_cfg->curlen++] = (unsigned char)0;
        }
        wm_hal_hash_compress(hal_dev, hash_cfg);
        hash_cfg->curlen = 0;
    }

    /*
        pad upto 56 bytes of zeroes
     */
    while (hash_cfg->curlen < 56) {
        hash_cfg->buf[hash_cfg->curlen++] = (unsigned char)0;
    }

    /*
     * store length
     */
#ifdef HAVE_NATIVE_INT64
    if (hash_cfg->alg == WM_HASH_ALG_SHA1) {
        STORE64H(hash_cfg->length, hash_cfg->buf + 56);
    } else {
        STORE64L(hash_cfg->length, hash_cfg->buf + 56);
    }
#else
    if (hash_cfg->alg == WM_HASH_ALG_SHA1) {
        STORE32H(hash_cfg->lengthHi, hash_cfg->buf + 56);
        STORE32H(hash_cfg->lengthLo, hash_cfg->buf + 60);
    } else {
        STORE32L(hash_cfg->lengthLo, hash_cfg->buf + 56);
        STORE32L(hash_cfg->lengthHi, hash_cfg->buf + 60);
    }
#endif /* HAVE_NATIVE_INT64 */
    wm_hal_hash_compress(hal_dev, hash_cfg);

    /* UNLOCK GPSEC resource */
    if (wm_drv_release_gpsec_mutex() != WM_OS_STATUS_SUCCESS) {
        WM_DRV_HASH_TRACE_FAIL();
        return WM_ERR_FAILED;
    }

    /*
     * copy output
     */
    for (i = 0; i < HASH_DIGEST_LENGTH(hash_cfg->alg); i++) {
        if (hash_cfg->alg == WM_HASH_ALG_SHA1) {
            STORE32H(hash_cfg->state[i], out + (4 * i));
        } else {
            STORE32L(hash_cfg->state[i], out + (4 * i));
        }
    }

    return WM_ERR_SUCCESS;
}

int wm_w800_drv_md5_start(wm_device_t *device, wm_drv_hash_cfg_t *ctx)
{
    wm_dt_hash_dev_t *dev      = (wm_dt_hash_dev_t *)device;
    wm_hal_hash_dev_t *hal_dev = NULL;

    /* check some input params */
    WM_DRV_HASH_ASSERT_PARAM(dev->drv);

    /* enable hash clock */
    wm_drv_clock_enable(dev->drv->clock_dev, WM_RCC_GPSEC_GATE_EN);

    /* resort to hal function */
    hal_dev = &(dev->drv->hal_dev);
    return wm_hal_hash_start(hal_dev, (wm_hal_hash_cfg_t *)ctx, WM_HASH_ALG_MD5);
}

int wm_w800_drv_md5_update(wm_device_t *device, wm_drv_hash_cfg_t *ctx, const unsigned char *in, uint32_t len)
{
    /* check some input params */
    WM_DRV_HASH_ASSERT_PARAM(device->drv && (ctx->alg == WM_HASH_ALG_MD5));

    /* resort to update function */
    return wm_w800_drv_hash_update(device, ctx, in, len);
}

int wm_w800_drv_md5_final(wm_device_t *device, wm_drv_hash_cfg_t *ctx, unsigned char *out)
{
    /* check some input params */
    WM_DRV_HASH_ASSERT_PARAM(device->drv && (ctx->alg == WM_HASH_ALG_MD5));

    /* resort to final function */
    return wm_w800_drv_hash_final(device, ctx, out);
}

int wm_w800_drv_sha1_start(wm_device_t *device, wm_drv_hash_cfg_t *ctx)
{
    wm_dt_hash_dev_t *dev      = (wm_dt_hash_dev_t *)device;
    wm_hal_hash_dev_t *hal_dev = NULL;

    /* check some input params */
    WM_DRV_HASH_ASSERT_PARAM(dev->drv);

    /* enable hash clock */
    wm_drv_clock_enable(dev->drv->clock_dev, WM_RCC_GPSEC_GATE_EN);

    /* resort to hal function */
    hal_dev = &(dev->drv->hal_dev);
    return wm_hal_hash_start(hal_dev, (wm_hal_hash_cfg_t *)ctx, WM_HASH_ALG_SHA1);
}

int wm_w800_drv_sha1_update(wm_device_t *device, wm_drv_hash_cfg_t *ctx, const unsigned char *in, uint32_t len)
{
    /* check some input params */
    WM_DRV_HASH_ASSERT_PARAM(device->drv && (ctx->alg == WM_HASH_ALG_SHA1));

    /* resort to update function */
    return wm_w800_drv_hash_update(device, ctx, in, len);
}

int wm_w800_drv_sha1_final(wm_device_t *device, wm_drv_hash_cfg_t *ctx, unsigned char *out)
{
    /* check some input params */
    WM_DRV_HASH_ASSERT_PARAM(device->drv && (ctx->alg == WM_HASH_ALG_SHA1));

    /* resort to final function */
    return wm_w800_drv_hash_final(device, ctx, out);
}

/* HASH driver operations */
const wm_drv_hash_ops_t wm_drv_hash_ops = {
    .init        = wm_w800_drv_hash_init,
    .deinit      = wm_w800_drv_hash_deinit,
    .md5_start   = wm_w800_drv_md5_start,
    .md5_update  = wm_w800_drv_md5_update,
    .md5_final   = wm_w800_drv_md5_final,
    .sha1_start  = wm_w800_drv_sha1_start,
    .sha1_update = wm_w800_drv_sha1_update,
    .sha1_final  = wm_w800_drv_sha1_final,
};
