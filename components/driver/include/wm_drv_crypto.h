/**
 * @file wm_drv_crypto.h
 *
 * @brief DRV_CRYPTO Module
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

#ifndef __WM_DRV_CRYPTO_H__
#define __WM_DRV_CRYPTO_H__

#include "wm_soc_cfgs.h"
#include "wm_types.h"
#include "wm_dt.h"

#include "wm_hal_crypto.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup WM_DRV_CRYP_Type_Definitions WM DRV_CRYP Type Definitions
 * @brief WinnerMicro DRV_CRYP Type Definitions
 */

/**
 * @addtogroup WM_DRV_CRYP_Type_Definitions
 * @{
 */

typedef wm_hal_cryp_cfg_t wm_drv_cryp_cfg_t;

/**
 * @}
 */

/**
 * @defgroup WM_DRV_CRYP_Functions WM DRV_CRYP Functions
 * @brief WinnerMicro DRV_CRYP Functions
 */

/**
 * @addtogroup WM_DRV_CRYP_Functions
 * @{
 */

/**
 * @brief   Initialize CRYP driver,
 *          initialize resources shared by encryption and decryption algorithms such as RC4, AES, DES, and 3DES.
 *
 * @param  dev_name  CRYP device name
 *
 * @return
  *    - NULL: failed
  *    - others: CRYP driver device
 *
 * @note This API must be called before all other CRYP API can be called
 *
 */
wm_device_t *wm_drv_cryp_init(const char *dev_name);

/**
 * @brief     Deinitialize CRYP driver
 *
 * @param     dev  CRYP driver device
 *
 * @return
 *    - WM_ERR_SUCCESS: succeed
 *    - WM_ERR_INVALID_PARAM: invalid argument
 *    - others: failed
 */
int wm_drv_cryp_deinit(wm_device_t *dev);

/**
 * @brief Fill the wm_drv_cryp_cfg_t structure pointed to by ctx with necessary data.
 *
 * @param[in]   dev     Pointer to the CRYP device structure.
 * @param[in]   ctx     Pointer to the Cipher Context.
 * @param[in]   key     Pointer to the key.
 * @param[in]   keylen  the length of key.
 * @return
 *    - WM_ERR_SUCCESS: succeed
 *    - others: failed
 * @note    The parameter ctx must be a structure which is allocated externally.
 *          And all of Context parameters in the initializing methods should be allocated externally too.
 */
int wm_drv_rc4_init(wm_device_t *dev, wm_drv_cryp_cfg_t *ctx, const unsigned char *key, uint32_t keylen);

/**
 * @brief Fill the psCipherContext_t structure pointed to by ctx with necessary data.
 *
 * @param[in]   dev     Pointer to the CRYP device structure.
 * @param[in]   ctx     Pointer to the Cipher Context.
 * @param[in]   iv      Pointer to the iv.
 * @param[in]   key     Pointer to the key.
 * @param[in]   keylen  the length of key.
 * @param[in]   sub_mode    DES/ 3DES Algorithm support ECB/CBC mode, AES Algorithm support ECB/CBC/CTR/MAC mode.
 * @return
 *    - WM_ERR_SUCCESS: succeed
 *    - others: failed
 */
int wm_drv_aes_init(wm_device_t *dev, wm_drv_cryp_cfg_t *ctx, const unsigned char *iv, const unsigned char *key,
                    uint32_t keylen, wm_gpsec_sub_mode_t sub_mode);

/**
 * @brief Fill the psCipherContext_t structure pointed to by ctx with necessary data.
 *
 * @param[in]   dev     Pointer to the CRYP device structure.
 * @param[in]   ctx     Pointer to the Cipher Context.
 * @param[in]   iv      Pointer to the iv.
 * @param[in]   key     Pointer to the key.
 * @param[in]   keylen  the length of key.
 * @param[in]   sub_mode    DES/ 3DES Algorithm support ECB/CBC mode, AES Algorithm support ECB/CBC/CTR/MAC mode.
 * @return
 *    - WM_ERR_SUCCESS: succeed
 *    - others: failed
 */
int wm_drv_des_init(wm_device_t *dev, wm_drv_cryp_cfg_t *ctx, const unsigned char *iv, const unsigned char *key,
                    uint32_t keylen, wm_gpsec_sub_mode_t sub_mode);

/**
 * @brief Fill the psCipherContext_t structure pointed to by ctx with necessary data.
 *
 * @param[in]   dev     Pointer to the CRYP device structure.
 * @param[in]   ctx     Pointer to the Cipher Context.
 * @param[in]   iv      Pointer to the iv.
 * @param[in]   key     Pointer to the key.
 * @param[in]   keylen  the length of key.
 * @param[in]   sub_mode    DES/ 3DES Algorithm support ECB/CBC mode, AES Algorithm support ECB/CBC/CTR/MAC mode.
 * @return
 *    - WM_ERR_SUCCESS: succeed
 *    - others: failed
 */
int wm_drv_3des_init(wm_device_t *dev, wm_drv_cryp_cfg_t *ctx, const unsigned char *iv, const unsigned char *key,
                     uint32_t keylen, wm_gpsec_sub_mode_t sub_mode);

/**
 * @brief   This function encrypts or decrypts a variable length data stream according to RC4.
 *          The RC4 algorithm it generates a "keystream" which is simply XORed with the plaintext to produce the ciphertext stream.
 *          Decryption is exactly the same as encryption. This function also decrypts a variable length data stream according to RC4.
 *
 * @param[in]   dev pointer to a wm_device_t structure.
 * @param[in]   ctx Pointer to the Cipher Context.
 * @param[in]   in  Pointer to the input plaintext data stream(or the encrypted text data stream) of variable length.
 * @param[in]   out Pointer to the resulting ciphertext data stream.
 * @param[in]   len Length of the input data stream in byte.
 *
 * @return
 *    - WM_ERR_SUCCESS: succeed
 *    - WM_ERR_INVALID_PARAM: invalid argument
 *    - Other error codes based on the encrypt/decrypt process
 */
int wm_drv_rc4_encrypt_decrypt(wm_device_t *dev, wm_drv_cryp_cfg_t *ctx, unsigned char *in, unsigned char *out, uint32_t len);

/**
 * @brief   This function encrypts or decrypts a variable length data stream according to AES.
 *
 * @param[in]   dev pointer to a wm_device_t structure.
 * @param[in]   ctx Pointer to the Cipher Context.
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
int wm_drv_aes_encrypt_decrypt(wm_device_t *dev, wm_drv_cryp_cfg_t *ctx, unsigned char *in, unsigned char *out, uint32_t len,
                               wm_gpsec_encrypt_decrypt_sel_t enc_dec);

/**
 * @brief   This function encrypts or decrypts a variable length data stream according to DES.
 *
 * @param[in]   dev pointer to a wm_device_t structure.
 * @param[in]   ctx Pointer to the Cipher Context.
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
int wm_drv_des_encrypt_decrypt(wm_device_t *dev, wm_drv_cryp_cfg_t *ctx, unsigned char *in, unsigned char *out, uint32_t len,
                               wm_gpsec_encrypt_decrypt_sel_t enc_dec);

/**
 * @brief   This function encrypts or decrypts a variable length data stream according to 3DES.
 *
 * @param[in]   dev pointer to a wm_device_t structure.
 * @param[in]   ctx Pointer to the Cipher Context.
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
int wm_drv_3des_encrypt_decrypt(wm_device_t *dev, wm_drv_cryp_cfg_t *ctx, unsigned char *in, unsigned char *out, uint32_t len,
                                wm_gpsec_encrypt_decrypt_sel_t enc_dec);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* __WM_DRV_CRYPTO_H__ */
