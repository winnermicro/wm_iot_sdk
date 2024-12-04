/**
 * @file wm_hal_crypto.h
 *
 * @brief crypto HAL Module
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

#ifndef __WM_HAL_CRYPTO_H__
#define __WM_HAL_CRYPTO_H__

#include <string.h>
#include "wm_osal.h"
#include "wm_types.h"
#include "wm_soc_cfgs.h"
#include "wm_ll_gpsec.h"

#ifdef __cplusplus
extern "C" {
#endif

#define RC4_128BIT_KEYLEN        16
#define RC4_256BIT_KEYLEN        32

#define AES_BLOCKLEN             16
#define AES_128BIT_KEYLEN        16
#define AES_IV_LEN               AES_BLOCKLEN

#define DES_BLOCKLEN             8
#define DES_KEY_LEN              8
#define DES_IV_LEN               8

#define DES3_BLOCKLEN            8
#define DES3_KEY_LEN             24
#define DES3_IV_LEN              8

#define IS_CRYP_OP(OP)           ((OP) == WM_GPSEC_ENCRYPT || (OP) == WM_GPSEC_DECRYPT)
#define IS_RC4_KEYSIZE(KEYSIZE)  ((KEYSIZE) == RC4_128BIT_KEYLEN || (KEYSIZE) == RC4_256BIT_KEYLEN)
#define IS_AES_KEYSIZE(KEYSIZE)  ((KEYSIZE) == AES_128BIT_KEYLEN)
/**< DES/3DES support algorithm ECB/CBC, AES support algorithm ECB/CBC/CTR/MAC */
#define IS_AES_SUBMODE(SUBMODE)  ((SUBMODE) == WM_GPSEC_ECB || (SUBMODE) == WM_GPSEC_CBC || (SUBMODE) == WM_GPSEC_CTR)

#define IS_AES_BUFFERSIZE(SIZE)  (SIZE > 0 && ((SIZE) % AES_BLOCKLEN) == 0U)

#define IS_DES_KEYSIZE(KEYSIZE)  ((KEYSIZE) == DES_KEY_LEN)
#define IS_3DES_KEYSIZE(KEYSIZE) ((KEYSIZE) == DES3_KEY_LEN)
/**< DES/3DES support algorithm ECB/CBC, AES support algorithm ECB/CBC/CTR/MAC */
#define IS_DES_SUBMODE(SUBMODE)  ((SUBMODE) == WM_GPSEC_ECB || (SUBMODE) == WM_GPSEC_CBC)
#define IS_DES_BUFFERSIZE(SIZE)  (SIZE > 0 && ((SIZE) % DES_BLOCKLEN) == 0U)

typedef struct {
    unsigned char key[256]; /**< The key used for encryption/decryption */
    uint32_t key_len;       /**< key length in bytes */
    unsigned char x;        /**< not use fixme */
    unsigned char y;        /**< not use fixme */
} wm_hal_rc4_cfg_t;

typedef struct {
    uint32_t skey[64]; /**< The key used for encryption/decryption */
    uint16_t key_len;  /**< key length in bytes, fixme if it must be 16, we should remove it */
} psAesKey_t;

typedef struct {
    psAesKey_t key;
    unsigned char iv[AES_IV_LEN]; /**< The initialization vector used */
    wm_gpsec_sub_mode_t sub_mode; /**< DES/ 3DES support algorithm ECB/CBC
                                         AES support algorithm ECB/CBC/CTR/MAC
                                         This parameter can be a value of @ref wm_gpsec_sub_mode_t*/
} wm_hal_aes_cfg_t;

typedef struct {
    uint32_t ek[3][32]; /**< key. In DES mode, only 8 bytes are required. fixme */
    uint32_t dk[3][32]; /**< not use fixme */
} psDesKey_t;

typedef struct {
    psDesKey_t key;
    unsigned char iv[DES_IV_LEN];
    wm_gpsec_sub_mode_t sub_mode; /**< DES/3DES Algorithm ECB/CBC
                                         AES Algorithm ECB/CBC/CTR/MAC
                                         This parameter can be a value of @ref wm_gpsec_sub_mode_t*/
} wm_hal_des_cfg_t;

typedef struct {
    uint32_t ek[3][32]; /**< key. In 3DES mode, only 24 bytes are required. fixme */
    uint32_t dk[3][32]; /**< not use fixme */
} psDes3Key_t;

typedef struct {
    psDes3Key_t key;
    unsigned char iv[DES3_IV_LEN];
    wm_gpsec_sub_mode_t sub_mode; /**< DES/ 3DES Algorithm ECB/CBC
                                         AES Algorithm ECB/CBC/CTR/MAC
                                         This parameter can be a value of @ref wm_gpsec_sub_mode_t*/
} wm_hal_3des_cfg_t;

typedef enum {
    CRYP_ALG_RC4 = 0,
    CRYP_ALG_AES,
    CRYP_ALG_DES,
    CRYP_ALG_3DES,
} wm_hal_cryp_alg_t;

typedef union {
    wm_hal_rc4_cfg_t rc4_cfg;
    wm_hal_aes_cfg_t aes_cfg;
    wm_hal_des_cfg_t des_cfg;
    wm_hal_3des_cfg_t des3_cfg;
} wm_hal_cryp_param_t;

typedef struct {
    wm_hal_cryp_alg_t alg;
    wm_hal_cryp_param_t param;
} wm_hal_cryp_cfg_t;

/**
 * @brief CRYP Device structure.
 *
 * The CRYP structure contains the base address of the gpsec registers
 * and a configuration structure for the CRYP(AES/DES/3DES/RC4 hardware module). It is used to manage
 * and interact with the CRYP hardware.
 * Note:In W800, RNG/SHA/MD5/CRC/AES/DES/3DES share the same hardware module, 
 *      therefore, only one function can be implemented at any given time.
 */
typedef struct {
    wm_gpsec_reg_t *gpsec_reg;  /**< Pointer to the base address of gpsec registers */
    wm_hal_cryp_cfg_t cryp_cfg; /**< Configuration for the CRYP */
    uint16_t irq_no;            /**< irq number of the CRYP module */
} wm_hal_cryp_dev_t;

/**
 * @defgroup WM_HAL_RC4 APIs
 * @brief WinnerMicro WM_HAL_RC4 APIs
 */

/**
 * @addtogroup WM_HAL_RC4
 * @{
 */

/**
 * @brief   Initializes the RC4 according to the specified
 *          parameters in the wm_hal_rc4_cfg_t.
 * @param  dev pointer to a wm_hal_cryp_dev_t structure that contains
 *         the configuration information for RC4 module
 *
 * @return
 *    - WM_ERR_SUCCESS: succeed
 *    - WM_ERR_INVALID_PARAM: invalid argument
 *    - Other error codes based on the initialization process
 */
int wm_hal_rc4_init(wm_hal_cryp_dev_t *dev);

/**
 * @brief Deinitialize the RC4.
 *        disable interrupt, clear interrupt flag, reset RC4 module.
 *
 * @param dev pointer to a wm_hal_cryp_dev_t structure.
 *
 * @return
 *    - WM_ERR_SUCCESS: succeed
 *    - WM_ERR_INVALID_PARAM: invalid argument
 *    - Other error codes based on the deinitialization process
 */
int wm_hal_rc4_deinit(wm_hal_cryp_dev_t *dev);

/**
 * @brief   This function encrypts or decrypts a variable length data stream according to RC4.
 *          The RC4 algorithm it generates a "keystream" which is simply XORed with the plaintext to produce the ciphertext stream.
 *          Decryption is exactly the same as encryption. This function also decrypts a variable length data stream according to RC4.
 *
 * @param[in]   dev pointer to a wm_hal_cryp_dev_t structure.
 * @param[in]   in  Pointer to the input plaintext data stream(or the encrypted text data stream) of variable length.
 * @param[in]   out Pointer to the resulting ciphertext data stream.
 * @param[in]   len Length of the input data stream in byte.
 *
 * @return
 *    - WM_ERR_SUCCESS: succeed
 *    - WM_ERR_INVALID_PARAM: invalid argument
 *    - Other error codes based on the encrypt/decrypt process
 */
int wm_hal_rc4_encrypt_decrypt(wm_hal_cryp_dev_t *dev, unsigned char *in, unsigned char *out, uint32_t len);

/**
 * @}
 */

/**
 * @defgroup WM_HAL_AES APIs
 * @brief WinnerMicro WM_HAL_AES APIs
 */

/**
 * @addtogroup WM_HAL_AES
 * @{
 */

/**
 * @brief  Initializes the AES according to the specified
 *         parameters in the wm_hal_aes_cfg_t.
           note: W800 AES module supports ECB/CBC/CTR/MAC modes.
 * @param  dev pointer to a wm_hal_cryp_dev_t structure that contains
 *         the configuration information for AES module
 *
 * @return
 *    - WM_ERR_SUCCESS: succeed
 *    - WM_ERR_INVALID_PARAM: invalid argument
 *    - Other error codes based on the initialization process
 */
int wm_hal_aes_init(wm_hal_cryp_dev_t *dev);

/**
 * @brief Deinitialize the AES.
 *        disable interrupt, clear interrupt flag, reset AES module.
 *
 * @param dev pointer to a wm_hal_cryp_dev_t structure.
 *
 * @return
 *    - WM_ERR_SUCCESS: succeed
 *    - WM_ERR_INVALID_PARAM: invalid argument
 *    - Other error codes based on the deinitialization process
 */
int wm_hal_aes_deinit(wm_hal_cryp_dev_t *dev);

/**
 * @brief   This function encrypts or decrypts a variable length data stream according to AES.
 *
 * @param[in]   dev pointer to a wm_hal_cryp_dev_t structure.
 * @param[in]   in  Pointer to the input plaintext data stream(or the encrypted text data stream) of variable length.
 * @param[in]   out Pointer to the resulting ciphertext data stream.
 * @param[in]   len Length of the input data stream in byte.
 * @param[in]   enc_dec The cryption way which indicates encryption or decryption.
 *
 * @return
 *    - WM_ERR_SUCCESS: succeed
 *    - WM_ERR_INVALID_PARAM: invalid argument
 *    - Other error codes based on the encrypt/decrypt process
 */
int wm_hal_aes_encrypt_decrypt(wm_hal_cryp_dev_t *dev, unsigned char *in, unsigned char *out, uint32_t len,
                               wm_gpsec_encrypt_decrypt_sel_t enc_dec);

/**
 * @}
 */

/**
 * @defgroup WM_HAL_DES APIs
 * @brief WinnerMicro WM_HAL_DES APIs
 */

/**
 * @addtogroup WM_HAL_DES
 * @{
 */

/**
 * @brief  Initializes the DES according to the specified
 *         parameters in the wm_hal_des_cfg_t.
           note: W800 DES module supports ECB/CBC modes.
 * @param  dev pointer to a wm_hal_cryp_dev_t structure that contains
 *         the configuration information for DES module
 *
 * @return
 *    - WM_ERR_SUCCESS: succeed
 *    - WM_ERR_INVALID_PARAM: invalid argument
 *    - Other error codes based on the initialization process
 */
int wm_hal_des_init(wm_hal_cryp_dev_t *dev);

/**
 * @brief Deinitialize the DES.
 *        disable interrupt, clear interrupt flag, reset DES module.
 *
 * @param dev pointer to a wm_hal_cryp_dev_t structure.
 *
 * @return
 *    - WM_ERR_SUCCESS: succeed
 *    - WM_ERR_INVALID_PARAM: invalid argument
 *    - Other error codes based on the deinitialization process
 */
int wm_hal_des_deinit(wm_hal_cryp_dev_t *dev);

/**
 * @brief   This function encrypts or decrypts a variable length data stream according to DES.
 *
 * @param[in]   dev pointer to a wm_hal_cryp_dev_t structure.
 * @param[in]   in  Pointer to the input plaintext data stream(or the encrypted text data stream) of variable length.
 * @param[in]   out Pointer to the resulting ciphertext data stream.
 * @param[in]   len Length of the input data stream in byte.
 * @param[in]   enc_dec The cryption way which indicates encryption or decryption.
 *
 * @return
 *    - WM_ERR_SUCCESS: succeed
 *    - WM_ERR_INVALID_PARAM: invalid argument
 *    - Other error codes based on the encrypt/decrypt process
 */
int wm_hal_des_encrypt_decrypt(wm_hal_cryp_dev_t *dev, unsigned char *in, unsigned char *out, uint32_t len,
                               wm_gpsec_encrypt_decrypt_sel_t enc_dec);

/**
 * @}
 */

/**
 * @defgroup WM_HAL_3DES APIs
 * @brief WinnerMicro WM_HAL_3DES APIs
 */

/**
 * @addtogroup WM_HAL_3DES
 * @{
 */

/**
 * @brief  Initializes the 3DES according to the specified
 *         parameters in the wm_hal_3def_cfg_t.
           note: W800 3des module supports ECB/CBC modes.
 * @param  dev pointer to a wm_hal_cryp_dev_t structure that contains
 *         the configuration information for DES module
 *
 * @return
 *    - WM_ERR_SUCCESS: succeed
 *    - WM_ERR_INVALID_PARAM: invalid argument
 *    - Other error codes based on the initialization process
 */
int wm_hal_3des_init(wm_hal_cryp_dev_t *dev);

/**
 * @brief Deinitialize the 3DES.
 *        disable interrupt, clear interrupt flag, reset 3DES module.
 *
 * @param dev pointer to a wm_hal_cryp_3dev_t structure.
 *
 * @return
 *    - WM_ERR_SUCCESS: succeed
 *    - WM_ERR_INVALID_PARAM: invalid argument
 *    - Other error codes based on the deinitialization process
 */
int wm_hal_3des_deinit(wm_hal_cryp_dev_t *dev);

/**
 * @brief   This function encrypts or decrypts a variable length data stream according to 3DES.
 *
 * @param[in]   dev pointer to a wm_hal_cryp_3dev_t structure.
 * @param[in]   in  Pointer to the input plaintext data stream(or the encrypted text data stream) of variable length.
 * @param[in]   out Pointer to the resulting ciphertext data stream.
 * @param[in]   len Length of the input data stream in byte.
 * @param[in]   enc_dec The cryption way which indicates encryption or decryption.
 *
 * @return
 *    - WM_ERR_SUCCESS: succeed
 *    - WM_ERR_INVALID_PARAM: invalid argument
 *    - Other error codes based on the encrypt/decrypt process
 */
int wm_hal_3des_encrypt_decrypt(wm_hal_cryp_dev_t *dev, unsigned char *in, unsigned char *out, uint32_t len,
                                wm_gpsec_encrypt_decrypt_sel_t enc_dec);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* __WM_HAL_CRYPTO_H__ */
