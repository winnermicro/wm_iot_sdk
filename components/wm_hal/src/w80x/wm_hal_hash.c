/**
 * @file wm_hal_hash.c
 *
 * @brief HASH HAL Module
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
#include "wm_hal_hash.h"

#undef LOG_TAG
#define LOG_TAG "HAL_HASH"
#include "wm_log.h"

#define WM_HAL_HASH_DEBUG 1
#if WM_HAL_HASH_DEBUG
#define WM_HAL_HASH_LOG_D(...)   wm_log_debug(__VA_ARGS__)
#define WM_HAL_HASH_LOG_I(...)   //wm_log_info(__VA_ARGS__)
#define WM_HAL_HASH_LOG_E(...)   wm_log_error(__VA_ARGS__)
#define WM_HAL_HASH_TRACE_FAIL() wm_log_error("%s@%d", __func__, __LINE__)
#else
#define WM_HAL_HASH_LOG_D(...)
#define WM_HAL_HASH_LOG_I(...)
#define WM_HAL_HASH_LOG_E(...)
#define WM_HAL_HASH_TRACE_FAIL()
#endif

/**
 * @brief Check param and return WM_ERR_INVALID_PARAM
 */
#define WM_HAL_HASH_ASSERT_PARAM(p)  \
    if (!(p)) {                      \
        WM_HAL_HASH_TRACE_FAIL();    \
        return WM_ERR_INVALID_PARAM; \
    }

int wm_hal_hash_init(wm_hal_hash_dev_t *dev)
{
    WM_HAL_HASH_ASSERT_PARAM(dev);
    wm_gpsec_reg_t *reg = dev->gpsec_reg;

    /* Check parameters */
    WM_HAL_HASH_ASSERT_PARAM(reg);

    /* clear interrupt flag */
    wm_ll_gpsec_clear_irq_flag(reg);
    /* mask completion interrupt */
    wm_ll_gpsec_mask_cmplt_irq(reg);

    return WM_ERR_SUCCESS;
}

int wm_hal_hash_deinit(wm_hal_hash_dev_t *dev)
{
    /* Check parameters */
    WM_HAL_HASH_ASSERT_PARAM(dev);

    wm_gpsec_reg_t *reg = dev->gpsec_reg;

    /* Check parameters */
    WM_HAL_HASH_ASSERT_PARAM(reg);

    /* clear interrupt flag */
    wm_ll_gpsec_clear_irq_flag(reg);
    /* mask completion interrupt */
    wm_ll_gpsec_mask_cmplt_irq(reg);

    return WM_ERR_SUCCESS;
}

int wm_hal_hash_start(wm_hal_hash_dev_t *dev, wm_hal_hash_cfg_t *cfg, uint32_t alg)
{
    WM_HAL_HASH_ASSERT_PARAM(cfg && IS_HASH_ALG(alg));
    (void)dev;
    cfg->state[0] = 0x67452301UL;
    cfg->state[1] = 0xefcdab89UL;
    cfg->state[2] = 0x98badcfeUL;
    cfg->state[3] = 0x10325476UL;
    if (alg == WM_HASH_ALG_SHA1) {
        cfg->state[4] = 0xc3d2e1f0UL;
    }
    cfg->curlen = 0;
#ifdef HAVE_NATIVE_INT64
    cfg->length = 0;
#else
    cfg->lengthHi = 0;
    cfg->lengthLo = 0;
#endif /* HAVE_NATIVE_INT64 */
    cfg->alg = alg;
    return WM_ERR_SUCCESS;
}

int wm_hal_hash_compress(wm_hal_hash_dev_t *dev, wm_hal_hash_cfg_t *cfg)
{
    /* Check parameters */
    WM_HAL_HASH_ASSERT_PARAM(dev && cfg);

    uint32_t time_out   = 0xFFFF;
    wm_gpsec_reg_t *reg = dev->gpsec_reg;

    /* Check parameters */
    WM_HAL_HASH_ASSERT_PARAM(reg);

    WM_HAL_HASH_LOG_I("hash compress start");

    wm_hal_hash_init(dev);
    /* set source address */
    wm_ll_gpsec_set_src_address(reg, cfg->buf);
    /* set config register */
    wm_ll_gpsec_set_total_bytes(reg, WM_HAL_HASH_BLOCK_BYTES);
    wm_ll_gpsec_set_cypher_mode(reg, cfg->alg == WM_HASH_ALG_SHA1 ? WM_GPSEC_CYPHER_MODE_SHA1 : WM_GPSEC_CYPHER_MODE_MD5);
    wm_ll_gpsec_set_digest(reg, cfg->state, HASH_DIGEST_LENGTH(cfg->alg));
    WM_HAL_HASH_LOG_I("line:%d flag:%d", __LINE__, wm_ll_gpsec_get_irq_flag(dev->gpsec_reg));
    /* start operation */
    wm_ll_gpsec_set_operation_start(reg, 1);
    WM_HAL_HASH_LOG_I("line:%d flag:%d", __LINE__, wm_ll_gpsec_get_irq_flag(dev->gpsec_reg));
    /* wait for complete flag to be raised */
    while (!wm_ll_gpsec_get_irq_flag(reg) && time_out) {
        time_out--;
    }
    WM_HAL_HASH_LOG_I("line:%d flag:%d", __LINE__, wm_ll_gpsec_get_irq_flag(dev->gpsec_reg));
    if (time_out == 0) {
        WM_HAL_HASH_LOG_E("line:%d timeout", __LINE__);
    }
    /* Clear complete Flag */
    wm_ll_gpsec_clear_irq_flag(reg);
    wm_ll_gpsec_get_digest(reg, cfg->state, HASH_DIGEST_LENGTH(cfg->alg));
    WM_HAL_HASH_LOG_I("hash compress end");
    return WM_ERR_SUCCESS;
}

int wm_hal_hash_update(wm_hal_hash_dev_t *dev, wm_hal_hash_cfg_t *hash_cfg, const unsigned char *buf, uint32_t len)
{
    /* Check parameters */
    WM_HAL_HASH_ASSERT_PARAM(dev && hash_cfg && buf && len > 0);
    uint32_t n;

    while (len > 0) {
        n = min(len, (WM_HAL_HASH_BLOCK_BYTES - hash_cfg->curlen));
        memcpy(hash_cfg->buf + hash_cfg->curlen, buf, (size_t)n);
        hash_cfg->curlen += n;
        buf += n;
        len -= n;

        /* is 64 bytes full? */
        if (hash_cfg->curlen == WM_HAL_HASH_BLOCK_BYTES) {
            if (wm_hal_hash_compress(dev, hash_cfg) != WM_ERR_SUCCESS) {
                WM_HAL_HASH_TRACE_FAIL();
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

    return WM_ERR_SUCCESS;
}

int wm_hal_hash_final(wm_hal_hash_dev_t *dev, wm_hal_hash_cfg_t *hash_cfg, unsigned char *out)
{
    /* Check parameters */
    WM_HAL_HASH_ASSERT_PARAM(dev && hash_cfg && out && IS_HASH_ALG(hash_cfg->alg));

    int32_t i;
#ifndef HAVE_NATIVE_INT64
    uint32_t n;
#endif
    if (hash_cfg->curlen >= sizeof(hash_cfg->buf) || out == NULL) {
        WM_HAL_HASH_TRACE_FAIL();
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

    /*
     * if the length is currently above 56 bytes we append zeros then compress.
     * Then we can fall back to padding zeros and length encoding like normal.
     */
    if (hash_cfg->curlen > 56) {
        while (hash_cfg->curlen < 64) {
            hash_cfg->buf[hash_cfg->curlen++] = (unsigned char)0;
        }
        wm_hal_hash_compress(dev, hash_cfg);
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
    wm_hal_hash_compress(dev, hash_cfg);

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

    WM_HAL_HASH_LOG_I("hash final end");

    return WM_ERR_SUCCESS;
}

int wm_hal_md5_init(wm_hal_hash_dev_t *dev)
{
    return wm_hal_hash_init(dev);
}

int wm_hal_md5_deinit(wm_hal_hash_dev_t *dev)
{
    return wm_hal_hash_deinit(dev);
}

int wm_hal_md5_start(wm_hal_hash_dev_t *dev, wm_hal_hash_cfg_t *hash_cfg)
{
    return wm_hal_hash_start(dev, hash_cfg, WM_HASH_ALG_MD5);
}

int wm_hal_md5_update(wm_hal_hash_dev_t *dev, wm_hal_hash_cfg_t *hash_cfg, const unsigned char *in, uint32_t len)
{
    return wm_hal_hash_update(dev, hash_cfg, in, len);
}

int wm_hal_md5_final(wm_hal_hash_dev_t *dev, wm_hal_hash_cfg_t *hash_cfg, unsigned char *out)
{
    return wm_hal_hash_final(dev, hash_cfg, out);
}

int wm_hal_sha1_init(wm_hal_hash_dev_t *dev)
{
    return wm_hal_hash_init(dev);
}

int wm_hal_sha1_deinit(wm_hal_hash_dev_t *dev)
{
    return wm_hal_hash_deinit(dev);
}

int wm_hal_sha1_start(wm_hal_hash_dev_t *dev, wm_hal_hash_cfg_t *hash_cfg)
{
    return wm_hal_hash_start(dev, hash_cfg, WM_HASH_ALG_SHA1);
}

int wm_hal_sha1_update(wm_hal_hash_dev_t *dev, wm_hal_hash_cfg_t *hash_cfg, const unsigned char *in, uint32_t len)
{
    return wm_hal_hash_update(dev, hash_cfg, in, len);
}

int wm_hal_sha1_final(wm_hal_hash_dev_t *dev, wm_hal_hash_cfg_t *hash_cfg, unsigned char *out)
{
    return wm_hal_hash_final(dev, hash_cfg, out);
}
