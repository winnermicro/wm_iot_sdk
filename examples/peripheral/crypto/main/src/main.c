#define LOG_TAG "crypto_example"
#include "wm_log.h"

#include "wmsdk_config.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "wm_drv_crypto.h"

#define OP_DATA_BYTES 1024

#define CRYPTO_DEVICE "crypto"

static int rc4_hard_demo(void)
{
    uint8_t *in   = NULL;
    uint8_t *out1 = NULL;
    uint8_t *out2 = NULL;
    uint8_t *key  = NULL;
    int i;
    wm_drv_cryp_cfg_t ctx;
    wm_device_t *dev;

    /*initialize rng*/
    dev = wm_drv_cryp_init(CRYPTO_DEVICE);
    if (dev == NULL) {
        wm_log_error("init fail.");
    }

    in   = malloc(OP_DATA_BYTES);
    out1 = malloc(OP_DATA_BYTES);
    out2 = malloc(OP_DATA_BYTES);
    key  = malloc(16);

    if ((in == NULL) || (out1 == NULL) || (out2 == NULL) || (key == NULL)) {
        wm_log_info("malloc err");
        goto OUT;
    }

    for (i = 0; i < OP_DATA_BYTES; i++) {
        in[i] = rand();
    }

    for (i = 0; i < 16; i++) {
        key[i] = rand();
    }

    memset(out1, 0, OP_DATA_BYTES);
    memset(out2, 0, OP_DATA_BYTES);

    if (wm_drv_rc4_init(dev, &ctx, key, RC4_128BIT_KEYLEN) != WM_ERR_SUCCESS) {
        wm_log_info("rc4 init err");
        goto OUT;
    }
    wm_drv_rc4_encrypt_decrypt(dev, &ctx, in, out1, OP_DATA_BYTES);
    wm_drv_rc4_encrypt_decrypt(dev, &ctx, out1, out2, OP_DATA_BYTES);

    if (memcmp(in, out2, OP_DATA_BYTES)) {
        wm_log_info("rc4 test fail");
    } else {
        wm_log_info("rc4 test success");
    }

OUT:
    if (in != NULL)
        free(in);
    if (out1 != NULL)
        free(out1);
    if (out2 != NULL)
        free(out2);
    if (key != NULL)
        free(key);
    wm_drv_cryp_deinit(dev);

    return 0;
}

static int aes_hard_demo(void)
{
    uint8_t *in   = NULL;
    uint8_t *out1 = NULL;
    uint8_t *out2 = NULL;
    uint8_t *key  = NULL;
    uint8_t *iv   = NULL;
    int i;
    wm_drv_cryp_cfg_t ctx;
    wm_device_t *dev;

    /*initialize cryp */
    dev = wm_drv_cryp_init(CRYPTO_DEVICE);
    if (dev == NULL) {
        wm_log_error("init fail.");
    }

    in   = malloc(OP_DATA_BYTES); //AES input bytes must be an integer multiple of 16, or an error will occur.
    out1 = malloc(OP_DATA_BYTES);
    out2 = malloc(OP_DATA_BYTES);
    key  = malloc(16);
    iv   = malloc(16);

    if ((in == NULL) || (out1 == NULL) || (out2 == NULL) || (key == NULL) || (iv == NULL)) {
        wm_log_info("malloc err");
        goto OUT;
    }

    for (i = 0; i < OP_DATA_BYTES; i++) {
        in[i] = i; //rand();
    }

    for (i = 0; i < 16; i++) {
        key[i] = i; //rand();
        iv[i]  = i; //rand();
    }

    /*  AES ECB mode */
    memset(out1, 0, OP_DATA_BYTES);
    memset(out2, 0, OP_DATA_BYTES);
    if (wm_drv_aes_init(dev, &ctx, iv, key, AES_128BIT_KEYLEN, WM_GPSEC_ECB) != WM_ERR_SUCCESS) {
        wm_log_info("aes init err");
        goto OUT;
    }
    wm_drv_aes_encrypt_decrypt(dev, &ctx, in, out1, OP_DATA_BYTES, WM_GPSEC_ENCRYPT);
    wm_drv_aes_encrypt_decrypt(dev, &ctx, out1, out2, OP_DATA_BYTES, WM_GPSEC_DECRYPT);
    if (memcmp(in, out2, OP_DATA_BYTES)) {
        wm_log_info("aes ecb test fail");
    } else {
        wm_log_info("aes ecb test success");
    }

    /*  AES CBC mode */
    memset(out1, 0, OP_DATA_BYTES);
    memset(out2, 0, OP_DATA_BYTES);
    if (wm_drv_aes_init(dev, &ctx, iv, key, AES_128BIT_KEYLEN, WM_GPSEC_CBC) != WM_ERR_SUCCESS) {
        wm_log_info("aes init err");
        goto OUT;
    }
    wm_drv_aes_encrypt_decrypt(dev, &ctx, in, out1, OP_DATA_BYTES, WM_GPSEC_ENCRYPT);
    wm_drv_aes_encrypt_decrypt(dev, &ctx, out1, out2, OP_DATA_BYTES, WM_GPSEC_DECRYPT);
    if (memcmp(in, out2, OP_DATA_BYTES)) {
        wm_log_info("aes cbc test fail");
    } else {
        wm_log_info("aes cbc test success");
    }

    /*  AES CTR mode */
    memset(out1, 0, OP_DATA_BYTES);
    memset(out2, 0, OP_DATA_BYTES);
    if (wm_drv_aes_init(dev, &ctx, iv, key, AES_128BIT_KEYLEN, WM_GPSEC_CTR) != WM_ERR_SUCCESS) {
        wm_log_info("aes init err");
        goto OUT;
    }
    wm_drv_aes_encrypt_decrypt(dev, &ctx, in, out1, OP_DATA_BYTES, WM_GPSEC_ENCRYPT);
    wm_drv_aes_encrypt_decrypt(dev, &ctx, out1, out2, OP_DATA_BYTES, WM_GPSEC_DECRYPT);
    if (memcmp(in, out2, OP_DATA_BYTES)) {
        wm_log_info("aes ctr test fail");
    } else {
        wm_log_info("aes ctr test success");
    }

OUT:
    if (in != NULL)
        free(in);
    if (out1 != NULL)
        free(out1);
    if (out2 != NULL)
        free(out2);
    if (key != NULL)
        free(key);
    if (iv != NULL)
        free(iv);
    wm_drv_cryp_deinit(dev);

    return 0;
}

static int des_hard_demo(void)
{
    uint8_t *in   = NULL;
    uint8_t *out1 = NULL;
    uint8_t *out2 = NULL;
    uint8_t *key  = NULL;
    uint8_t *iv   = NULL;
    int i;
    wm_drv_cryp_cfg_t ctx;
    wm_device_t *dev;

    /*initialize cryp */
    dev = wm_drv_cryp_init(CRYPTO_DEVICE);
    if (dev == NULL) {
        wm_log_error("init fail.");
    }

    in   = malloc(OP_DATA_BYTES);
    out1 = malloc(OP_DATA_BYTES);
    out2 = malloc(OP_DATA_BYTES);
    key  = malloc(8);
    iv   = malloc(8);

    if ((in == NULL) || (out1 == NULL) || (out2 == NULL) || (key == NULL) || (iv == NULL)) {
        wm_log_info("malloc err");
        goto OUT;
    }

    for (i = 0; i < OP_DATA_BYTES; i++) {
        in[i] = i; //rand();
    }

    for (i = 0; i < 8; i++) {
        key[i] = i; //rand();
        iv[i]  = i; //rand();
    }

    /* DES ECB mode */
    memset(out1, 0, OP_DATA_BYTES);
    memset(out2, 0, OP_DATA_BYTES);
    if (wm_drv_des_init(dev, &ctx, iv, key, DES_KEY_LEN, WM_GPSEC_ECB) != WM_ERR_SUCCESS) {
        wm_log_info("des init err");
        goto OUT;
    }
    wm_drv_des_encrypt_decrypt(dev, &ctx, in, out1, OP_DATA_BYTES, WM_GPSEC_ENCRYPT);
    wm_drv_des_encrypt_decrypt(dev, &ctx, out1, out2, OP_DATA_BYTES, WM_GPSEC_DECRYPT);
    if (memcmp(in, out2, OP_DATA_BYTES)) {
        wm_log_info("des ecb test fail");
    } else {
        wm_log_info("des ecb test success");
    }

    /* DES CBC mode */
    memset(out1, 0, OP_DATA_BYTES);
    memset(out2, 0, OP_DATA_BYTES);
    if (wm_drv_des_init(dev, &ctx, iv, key, DES_KEY_LEN, WM_GPSEC_CBC) != WM_ERR_SUCCESS) {
        wm_log_info("des init err");
        goto OUT;
    }
    wm_drv_des_encrypt_decrypt(dev, &ctx, in, out1, OP_DATA_BYTES, WM_GPSEC_ENCRYPT);
    wm_drv_des_encrypt_decrypt(dev, &ctx, out1, out2, OP_DATA_BYTES, WM_GPSEC_DECRYPT);
    if (memcmp(in, out2, OP_DATA_BYTES)) {
        wm_log_info("des cbc test fail");
    } else {
        wm_log_info("des cbc test success");
    }

OUT:
    if (in != NULL)
        free(in);
    if (out1 != NULL)
        free(out1);
    if (out2 != NULL)
        free(out2);
    if (key != NULL)
        free(key);
    if (iv != NULL)
        free(iv);
    wm_drv_cryp_deinit(dev);

    return 0;
}

static int des3_hard_demo(void)
{
    uint8_t *in   = NULL;
    uint8_t *out1 = NULL;
    uint8_t *out2 = NULL;
    uint8_t *key  = NULL;
    uint8_t *iv   = NULL;
    int i;
    wm_drv_cryp_cfg_t ctx;
    wm_device_t *dev;

    /*initialize cryp */
    dev = wm_drv_cryp_init(CRYPTO_DEVICE);
    if (dev == NULL) {
        wm_log_error("init fail.");
    }

    in   = malloc(OP_DATA_BYTES);
    out1 = malloc(OP_DATA_BYTES);
    out2 = malloc(OP_DATA_BYTES);
    key  = malloc(24);
    iv   = malloc(8);

    if ((in == NULL) || (out1 == NULL) || (out2 == NULL) || (key == NULL) || (iv == NULL)) {
        wm_log_info("malloc err");
        goto OUT;
    }

    for (i = 0; i < OP_DATA_BYTES; i++) {
        in[i] = i; //rand();
    }

    for (i = 0; i < 24; i++) {
        key[i] = i; //rand();
    }

    for (i = 0; i < 8; i++) {
        iv[i] = i; //rand();
    }

    /* 3DES ECB mode */
    memset(out1, 0, OP_DATA_BYTES);
    memset(out2, 0, OP_DATA_BYTES);
    if (wm_drv_3des_init(dev, &ctx, iv, key, DES3_KEY_LEN, WM_GPSEC_ECB) != WM_ERR_SUCCESS) {
        wm_log_info("3des init err");
        goto OUT;
    }
    wm_drv_3des_encrypt_decrypt(dev, &ctx, in, out1, OP_DATA_BYTES, WM_GPSEC_ENCRYPT);
    wm_drv_3des_encrypt_decrypt(dev, &ctx, out1, out2, OP_DATA_BYTES, WM_GPSEC_DECRYPT);
    if (memcmp(in, out2, OP_DATA_BYTES)) {
        wm_log_info("3des ecb test fail");
    } else {
        wm_log_info("3des ecb test success");
    }

    /* 3DES cbc mode */
    memset(out1, 0, OP_DATA_BYTES);
    memset(out2, 0, OP_DATA_BYTES);
    if (wm_drv_3des_init(dev, &ctx, iv, key, DES3_KEY_LEN, WM_GPSEC_CBC) != WM_ERR_SUCCESS) {
        wm_log_info("3des init err");
        goto OUT;
    }
    wm_drv_3des_encrypt_decrypt(dev, &ctx, in, out1, OP_DATA_BYTES, WM_GPSEC_ENCRYPT);
    wm_drv_3des_encrypt_decrypt(dev, &ctx, out1, out2, OP_DATA_BYTES, WM_GPSEC_DECRYPT);
    if (memcmp(in, out2, OP_DATA_BYTES)) {
        wm_log_info("3des cbc test fail");
    } else {
        wm_log_info("3des cbc test success");
    }

OUT:
    if (in != NULL)
        free(in);
    if (out1 != NULL)
        free(out1);
    if (out2 != NULL)
        free(out2);
    if (key != NULL)
        free(key);
    if (iv != NULL)
        free(iv);
    wm_drv_cryp_deinit(dev);

    return 0;
}

int main(void)
{
    wm_log_info("crypto demo.");

    rc4_hard_demo();

    aes_hard_demo();

    des_hard_demo();

    des3_hard_demo();

    while (1) {
        wm_log_info("crypto demo.");
        vTaskDelay(pdMS_TO_TICKS(1000));
    }

    return 0;
}
