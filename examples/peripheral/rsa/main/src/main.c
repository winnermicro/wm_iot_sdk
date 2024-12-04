#define LOG_TAG "rsa_example"
#include "wm_log.h"

#include "wmsdk_config.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "mbedtls/platform.h"
#include "mbedtls/sha1.h"
#include "mbedtls/rsa.h"
#include "mbedtls/pk.h"
#include <string.h>

/*
 * Example RSA-1024 keypair, for test purposes
 */
#define KEY_LEN 128

#define RSA_N                          \
    "9292758453063D803DD603D5E777D788" \
    "8ED1D5BF35786190FA2F23EBC0848AEA" \
    "DDA92CA6C3D80B32C4D109BE0F36D6AE" \
    "7130B9CED7ACDF54CFC7555AC14EEBAB" \
    "93A89813FBF3C4F8066D2D800F7C38A8" \
    "1AE31942917403FF4946B0A83D3D3E05" \
    "EE57C6F5F5606FB5D4BC6CD34EE0801A" \
    "5E94BB77B07507233A0BC7BAC8F90F79"

#define RSA_E "10001"

#define RSA_D                          \
    "24BF6185468786FDD303083D25E64EFC" \
    "66CA472BC44D253102F8B4A9D3BFA750" \
    "91386C0077937FE33FA3252D28855837" \
    "AE1B484A8A9A45F7EE8C0C634F99E8CD" \
    "DF79C5CE07EE72C7F123142198164234" \
    "CABB724CF78B8173B9F880FC86322407" \
    "AF1FEDFDDE2BEB674CA15F3E81A1521E" \
    "071513A1E85B5DFA031F21ECAE91A34D"

#define RSA_P                          \
    "C36D0EB7FCD285223CFB5AABA5BDA3D8" \
    "2C01CAD19EA484A87EA4377637E75500" \
    "FCB2005C5C7DD6EC4AC023CDA285D796" \
    "C3D9E75E1EFC42488BB4F1D13AC30A57"

#define RSA_Q                          \
    "C000DF51A7C77AE8D7C7370C1FF55B69" \
    "E211C2B9E5DB1ED0BF61D0D9899620F4" \
    "910E4168387E3C30AA1E00C339A79508" \
    "8452DD96A9A5EA5D9DCA68DA636032AF"

#define PT_LEN 24
#define RSA_PT                                         \
    "\xAA\xBB\xCC\x03\x02\x01\x00\xFF\xFF\xFF\xFF\xFF" \
    "\x11\x22\x33\x0A\x0B\x0C\xCC\xDD\xDD\xDD\xDD\xDD"

#if defined(MBEDTLS_PKCS1_V15)
static int myrand(void *rng_state, unsigned char *output, size_t len)
{
#if !defined(__OpenBSD__)
    size_t i;

    if (rng_state != NULL)
        rng_state = NULL;

    for (i = 0; i < len; ++i)
        output[i] = rand();
#else
    if (rng_state != NULL)
        rng_state = NULL;

    arc4random_buf(output, len);
#endif /* !OpenBSD */

    return (0);
}
#endif /* MBEDTLS_PKCS1_V15 */

/*
 * Checkup routine
 */
static int mbedtls_rsa_self_test(int verbose)
{
    int ret = 0;
#if defined(MBEDTLS_PKCS1_V15)
    size_t len;
    mbedtls_rsa_context rsa;
    unsigned char rsa_plaintext[PT_LEN];
    unsigned char rsa_decrypted[PT_LEN];
    unsigned char rsa_ciphertext[KEY_LEN];
#if defined(MBEDTLS_SHA1_C)
    unsigned char sha1sum[20];
#endif

    mbedtls_mpi K;

    mbedtls_mpi_init(&K);
    mbedtls_rsa_init(&rsa);

    MBEDTLS_MPI_CHK(mbedtls_mpi_read_string(&K, 16, RSA_N));
    MBEDTLS_MPI_CHK(mbedtls_rsa_import(&rsa, &K, NULL, NULL, NULL, NULL));
    MBEDTLS_MPI_CHK(mbedtls_mpi_read_string(&K, 16, RSA_P));
    MBEDTLS_MPI_CHK(mbedtls_rsa_import(&rsa, NULL, &K, NULL, NULL, NULL));
    MBEDTLS_MPI_CHK(mbedtls_mpi_read_string(&K, 16, RSA_Q));
    MBEDTLS_MPI_CHK(mbedtls_rsa_import(&rsa, NULL, NULL, &K, NULL, NULL));
    MBEDTLS_MPI_CHK(mbedtls_mpi_read_string(&K, 16, RSA_D));
    MBEDTLS_MPI_CHK(mbedtls_rsa_import(&rsa, NULL, NULL, NULL, &K, NULL));
    MBEDTLS_MPI_CHK(mbedtls_mpi_read_string(&K, 16, RSA_E));
    MBEDTLS_MPI_CHK(mbedtls_rsa_import(&rsa, NULL, NULL, NULL, NULL, &K));

    MBEDTLS_MPI_CHK(mbedtls_rsa_complete(&rsa));

    if (verbose != 0)
        wm_log_raw(WM_LOG_LEVEL_INFO, "  RSA key validation: ");

    if (mbedtls_rsa_check_pubkey(&rsa) != 0 || mbedtls_rsa_check_privkey(&rsa) != 0) {
        if (verbose != 0)
            wm_log_raw(WM_LOG_LEVEL_INFO, "failed\n");

        ret = 1;
        goto cleanup;
    }

    if (verbose != 0)
        wm_log_raw(WM_LOG_LEVEL_INFO, "passed\n  PKCS#1 encryption : ");

    memcpy(rsa_plaintext, RSA_PT, PT_LEN);

    if (mbedtls_rsa_pkcs1_encrypt(&rsa, myrand, NULL, PT_LEN, rsa_plaintext, rsa_ciphertext) != 0) {
        if (verbose != 0)
            wm_log_raw(WM_LOG_LEVEL_INFO, "failed\n");

        ret = 1;
        goto cleanup;
    }

    if (verbose != 0)
        wm_log_raw(WM_LOG_LEVEL_INFO, "passed\n  PKCS#1 decryption : ");

    if (mbedtls_rsa_pkcs1_decrypt(&rsa, myrand, NULL, &len, rsa_ciphertext, rsa_decrypted, sizeof(rsa_decrypted)) != 0) {
        if (verbose != 0)
            wm_log_raw(WM_LOG_LEVEL_INFO, "failed\n");

        ret = 1;
        goto cleanup;
    }

    if (memcmp(rsa_decrypted, rsa_plaintext, len) != 0) {
        if (verbose != 0)
            wm_log_raw(WM_LOG_LEVEL_INFO, "failed\n");

        ret = 1;
        goto cleanup;
    }

    if (verbose != 0)
        wm_log_raw(WM_LOG_LEVEL_INFO, "passed\n");

#if defined(MBEDTLS_SHA1_C)
    if (verbose != 0)
        wm_log_raw(WM_LOG_LEVEL_INFO, "  PKCS#1 data sign  : ");

    if (mbedtls_sha1(rsa_plaintext, PT_LEN, sha1sum) != 0) {
        if (verbose != 0)
            wm_log_raw(WM_LOG_LEVEL_INFO, "failed\n");

        return (1);
    }

    if (mbedtls_rsa_pkcs1_sign(&rsa, myrand, NULL, MBEDTLS_MD_SHA1, 20, sha1sum, rsa_ciphertext) != 0) {
        if (verbose != 0)
            wm_log_raw(WM_LOG_LEVEL_INFO, "failed\n");

        ret = 1;
        goto cleanup;
    }

    if (verbose != 0)
        wm_log_raw(WM_LOG_LEVEL_INFO, "passed\n  PKCS#1 sig. verify: ");

    if (mbedtls_rsa_pkcs1_verify(&rsa, MBEDTLS_MD_SHA1, 20, sha1sum, rsa_ciphertext) != 0) {
        if (verbose != 0)
            wm_log_raw(WM_LOG_LEVEL_INFO, "failed\n");

        ret = 1;
        goto cleanup;
    }

    if (verbose != 0)
        wm_log_raw(WM_LOG_LEVEL_INFO, "passed\n");
#endif /* MBEDTLS_SHA1_C */

    if (verbose != 0)
        wm_log_raw(WM_LOG_LEVEL_INFO, "\n");

cleanup:
    mbedtls_mpi_free(&K);
    mbedtls_rsa_free(&rsa);
#else  /* MBEDTLS_PKCS1_V15 */
    ((void)verbose);
#endif /* MBEDTLS_PKCS1_V15 */
    return (ret);
}

static void rsa_hard_demo(void)
{
    mbedtls_rsa_self_test(1);
}

static int rsa_encrypt_decrypt_demo(void)
{
    int ret;
#if defined(MBEDTLS_PKCS1_V15)
    static const char *priv_key_pem = "-----BEGIN RSA PRIVATE KEY-----\n" //PKCS1 format
                                      "MIICXAIBAAKBgQDJufxmIFnkiuk+p1PlY8i/+9pZfNumHCviKR2NF2DQtTZCS5ZJ\n"
                                      "NRXLfS6HBbzO4Pi89KZUNVTrln47mCW2CR0+zdxA6RNPzjSfrLieKEMHq+f09xwg\n"
                                      "v8TUJbFN3uriyK1CxfkfG7J7ZOqrRkKtWAhXv00B1BFuEukLVDPxBBt4FwIDAQAB\n"
                                      "AoGAenXepHzVNWmiyPc5dhMKZonU9uhWS1PkzAnbt7BOhMTNaJeMZ2rDjWG0IiG4\n"
                                      "ifqG5ea7Jnmt0MruXO0zT3ve9RKIIuD6Vq++13McPmKxMZLNUcjtOZ2KmCsQAaK8\n"
                                      "DP5vf3lMQHmoH+E+V/yIWWhmTttoyu4bLmFOClkEWpMcmpECQQDjjFBUFzbD9Q84\n"
                                      "S8cz2YPX6D/oFpEgMKGz+PfwMm1Z8r96D4NVwrMYf+lL/ChrGPE/FT5anuvB70pN\n"
                                      "cIPFH9a5AkEA4vMjNWQiZTeSPnDiQr47IvpHdPBmjzBLqDnvxbIVc3yrsA/r5bGj\n"
                                      "4gAaXSj3L6NNpv0LuPtPyEoXkJZznP9dTwJABbjRjEvJJMRUzaWaLMObfSJp+PwY\n"
                                      "Sibu6geotHGQLmLcTcV2GAH9XvBYfYCpw4etCVkPaFf2tbm+tJPPvHVAgQJAaLck\n"
                                      "9LqqQCee+96cF42svZvborJVsBXpospwvrhHu4y3rzn5b3dClcHUaE3BmCJybu1t\n"
                                      "HkIe8tx0+Qrid3tAcwJBANuw6bugwvNbeTonLqoSn4arLhA2nkaWY//KTkhQQGY9\n"
                                      "iQgw1ss+ykXrbNwv7ATbDzo52t1ESf+Qq1RCHHZhih8=\n"
                                      "-----END RSA PRIVATE KEY-----\n";

    static const char *pub_key_pem = "-----BEGIN RSA PUBLIC KEY-----\n" //PKCS1 format
                                     "MIGJAoGBAMm5/GYgWeSK6T6nU+VjyL/72ll826YcK+IpHY0XYNC1NkJLlkk1Fct9\n"
                                     "LocFvM7g+Lz0plQ1VOuWfjuYJbYJHT7N3EDpE0/ONJ+suJ4oQwer5/T3HCC/xNQl\n"
                                     "sU3e6uLIrULF+R8bsntk6qtGQq1YCFe/TQHUEW4S6QtUM/EEG3gXAgMBAAE=\n"
                                     "-----END RSA PUBLIC KEY-----\n";
    mbedtls_pk_context priv_key, pub_key;
    size_t olen;
    unsigned char rsa_plaintext[PT_LEN] = "\xAA\xBB\xCC\x03\x02\x01\x00\xFF\xFF\xFF\xFF\xFF"
                                          "\x11\x22\x33\x0A\x0B\x0C\xCC\xDD\xDD\xDD\xDD\xDD";
    unsigned char rsa_ciphertext[PT_LEN];
    unsigned char rsa_decrypted[PT_LEN];

    mbedtls_pk_init(&priv_key);
    mbedtls_pk_init(&pub_key);

    // 1. parse private key
    ret = mbedtls_pk_parse_key(&priv_key, (const unsigned char *)priv_key_pem, strlen((char *)priv_key_pem) + 1, NULL, 0,
                               myrand, 0);
    if (ret != 0) {
        wm_log_raw(WM_LOG_LEVEL_INFO, "  Failed to parse private key: -0x%04x\n", -ret);
        goto cleanup;
    }

    // 2. parse public key
    ret = mbedtls_pk_parse_public_key(&pub_key, (const unsigned char *)pub_key_pem, strlen((char *)pub_key_pem) + 1);
    if (ret != 0) {
        wm_log_raw(WM_LOG_LEVEL_INFO, "  Failed to parse public key: -0x%04x\n", -ret);
        goto cleanup;
    }

    wm_log_raw(WM_LOG_LEVEL_INFO, "  parse RSA private key and public key passed\n  PKCS#1 encryption : ");

    //3. encrypt
    if (mbedtls_rsa_pkcs1_encrypt(mbedtls_pk_rsa(pub_key), myrand, NULL, PT_LEN, rsa_plaintext, rsa_ciphertext) != 0) {
        wm_log_raw(WM_LOG_LEVEL_INFO, "failed\n");

        ret = 1;
        goto cleanup;
    }

    wm_log_raw(WM_LOG_LEVEL_INFO, "passed\n  PKCS#1 decryption : ");

    //4. decrypt
    if (mbedtls_rsa_pkcs1_decrypt(mbedtls_pk_rsa(priv_key), myrand, NULL, &olen, rsa_ciphertext, rsa_decrypted,
                                  sizeof(rsa_decrypted)) != 0) {
        wm_log_raw(WM_LOG_LEVEL_INFO, "failed\n");

        ret = 1;
        goto cleanup;
    }

    if (memcmp(rsa_decrypted, rsa_plaintext, olen) != 0) {
        wm_log_raw(WM_LOG_LEVEL_INFO, "failed\n");

        ret = 1;
        goto cleanup;
    }

    wm_log_raw(WM_LOG_LEVEL_INFO, "passed\n");

cleanup:
    mbedtls_pk_free(&priv_key);
    mbedtls_pk_free(&pub_key);

#endif /* MBEDTLS_PKCS1_V15 */
    return (ret);
}

int main(void)
{
    wm_log_info("rsa demo.");

    rsa_hard_demo();

    rsa_encrypt_decrypt_demo();

    while (1) {
        wm_log_info("rsa demo.");
        vTaskDelay(pdMS_TO_TICKS(1000));
    }

    return 0;
}
