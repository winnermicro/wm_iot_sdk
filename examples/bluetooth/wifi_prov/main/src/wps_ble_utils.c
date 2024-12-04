/*
 * Copyright (c) 2018 WinnerMicro Semiconductor ASA
 *
 */

/** @file
 *  @brief WinnerMicro WiFi provision by BLE sample
 */
#include <stddef.h>
#include <string.h>
#include <errno.h>
#include <stdint.h>

#include "wm_drv_rng.h"
#include "wm_drv_crypto.h"
#include "mbedtls/pk.h"

#define LOG_TAG "wps_ble"
#include "wm_log.h"

#define CRYPTO_DEVICE "crypto"

static int PKCS7Padding(uint8_t *str, int len)
{
    int remain, i;
    remain = 16 - len % 16;

    for (i = 0; i < remain; i++) {
        str[len + i] = remain;
    }

    str[len + i] = '\0';
    return len + remain;
}

static int DePKCS7Padding(uint8_t *str, int len)
{
    int remain = str[len - 1];
    return len - remain;
}

static int rng_func(void *ctx, unsigned char *out, size_t len)
{
    wm_drv_fill_random(out, len);
    return 0;
}

int bt_rsa_encrypt(uint8_t *pub_key, int pubkey_size, uint8_t *src_ptr, int length, uint8_t *dest_ptr, int dest_len)
{
    int ret = -1;
    mbedtls_pk_context ctx_pk;
    unsigned char *p = pub_key;
    size_t olen      = 0;

    mbedtls_pk_init(&ctx_pk);
    ret = mbedtls_pk_parse_subpubkey(&p, p + pubkey_size, &ctx_pk);
    if (ret) {
        wm_log_warn("Can't import public key %d\n ", ret);
        goto out;
    }

    ret = mbedtls_pk_encrypt(&ctx_pk, src_ptr, length, dest_ptr, &olen, dest_len, rng_func, NULL);
    if (ret) {
        wm_log_warn("rsa encrypt fail %d\n ", ret);
        goto out;
    }

out:
    mbedtls_pk_free(&ctx_pk);
    return ret;
}

void *bt_aes_ctx_init(const unsigned char *key, size_t len)
{
    wm_drv_cryp_cfg_t *ctx = NULL;
    wm_device_t *dev;

    /*initialize cryp */
    dev = wm_drv_cryp_init(CRYPTO_DEVICE);
    if (dev == NULL) {
        return NULL;
    }

    ctx = malloc(sizeof(*ctx));
    if (ctx == NULL) {
        return NULL;
    }
    memset(ctx, 0, sizeof(*ctx));
    if (wm_drv_aes_init(dev, ctx, NULL, key, len, WM_GPSEC_ECB) != WM_ERR_SUCCESS) {
        free(ctx);
        ctx = NULL;
    }

    return ctx;
}

int bt_aes_decrypt(void *ctx, uint8_t *src_ptr, int length, uint8_t *dest_ptr)
{
    int ret      = -1;
    uint8_t *buf = NULL;

    if (src_ptr == NULL || length % 16 != 0) {
        goto out;
    }

    buf = malloc(length);
    if (NULL == buf) {
        goto out;
    }

    memcpy(buf, src_ptr, length);

    wm_device_t *dev;
    dev = wm_drv_cryp_init(CRYPTO_DEVICE);
    ret = wm_drv_aes_encrypt_decrypt(dev, ctx, src_ptr, buf, length, WM_GPSEC_DECRYPT);
    if (ret != WM_ERR_SUCCESS) {
        goto out;
    }

    memcpy(dest_ptr, buf, length);
    ret = DePKCS7Padding(dest_ptr, length);
out:
    if (buf) {
        free(buf);
    }
    return ret;
}

int bt_aes_encrypt(void *ctx, const uint8_t *src_ptr, int length, uint8_t *dest_ptr)
{
    int ret                        = -1;
    int len                        = 0;
    unsigned char *aes_encode_temp = NULL;

    aes_encode_temp = malloc(length + 16);

    if (aes_encode_temp == NULL) {
        goto out;
    }

    memset(aes_encode_temp, 0, length + 16);
    memcpy(aes_encode_temp, src_ptr, length);
    len = PKCS7Padding(aes_encode_temp, length);

    wm_device_t *dev;
    dev = wm_drv_cryp_init(CRYPTO_DEVICE);
    ret = wm_drv_aes_encrypt_decrypt(dev, ctx, aes_encode_temp, dest_ptr, len, WM_GPSEC_ENCRYPT);
    if (ret != WM_ERR_SUCCESS) {
        goto out;
    }

    ret = len;
out:

    if (aes_encode_temp) {
        free(aes_encode_temp);
    }

    return ret;
}
