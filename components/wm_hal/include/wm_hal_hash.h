/**
 * @file wm_hal_hash.h
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

#ifndef __WM_HAL_HASH_H__
#define __WM_HAL_HASH_H__

#include <string.h>
#include "wm_osal.h"
#include "wm_types.h"
#include "wm_soc_cfgs.h"
#include "wm_ll_gpsec.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * 32-bit integer manipulation macros (big endian)
 */
#define STORE32H(x, y)                       \
    {                                        \
        (y)[0] = (unsigned char)((x) >> 24); \
        (y)[1] = (unsigned char)((x) >> 16); \
        (y)[2] = (unsigned char)((x) >> 8);  \
        (y)[3] = (unsigned char)(x);         \
    }

/*
 * 32-bit integer manipulation macros (little endian)
 */
#define STORE32L(x, y)           \
    {                            \
        unsigned long __t = (x); \
        memcpy(y, &__t, 4);      \
    }

#define WM_HAL_HASH_BLOCK_BYTES 64
#ifndef min
#define min(a, b) (((a) < (b)) ? (a) : (b))
#endif /* min */

/**
  * @brief hash argorithm selection
  */
typedef enum {
    WM_HASH_ALG_MD5  = 0x00U, /*!< select md5 as the argorithm */
    WM_HASH_ALG_SHA1 = 0x01U, /*!< select sha1 as the argorithm */
} wm_hash_alg_t;

#define IS_HASH_ALG(alg)        ((alg == WM_HASH_ALG_MD5) || (alg == WM_HASH_ALG_SHA1))

#define WM_MD5_HASH_BYTES       16
#define WM_SHA1_HASH_BYTES      20
/**
  * @brief  Return digest length in words.
  * @param[in] alg hash argorithm selection.
  * @return Digest length
  */
#define HASH_DIGEST_LENGTH(alg) ((alg == WM_HASH_ALG_SHA1) ? 5U : 4U)

/**
  * @brief GPSEC State structures definition
  */
typedef enum {
    WM_GPSEC_STATE_RESET = 0x00U, /*!< Peripheral is not initialized            */
    WM_GPSEC_STATE_READY = 0x01U, /*!< Peripheral Initialized and ready for use */
    WM_GPSEC_STATE_BUSY  = 0x02U, /*!< Processing is ongoing          */
} wm_gpsec_state_t;

typedef struct {
#ifdef HAVE_NATIVE_INT64
    uint64_t length; /**< Total bit number of the message before padding */
#else
    uint32_t lengthHi;
    uint32_t lengthLo;
#endif
    uint32_t state[5];     /**< The initial digest and result digest */
    uint32_t curlen;       /**< the existing data length in bytes in the current block */
    unsigned char buf[64]; /**< data block being processed, the length of the block must be 64 bytes */
    int32_t alg;           /**< hash algorithm selection */
} wm_hal_hash_cfg_t;

/**
 * @brief HASH Device structure.
 *
 * The HASH structure contains the base address of the gpsec registers
 * and a configuration structure for the HASH. It is used to manage
 * and interact with the HASH hardware.
 * Note:In W800, RNG/SHA/MD5/CRC/AES/DES/3DES share the same hardware module, 
 *      therefore, only one function can be implemented at any given time.
 */
typedef struct {
    wm_gpsec_reg_t *gpsec_reg;  /**< Pointer to the base address of gpsec registers */
    wm_hal_hash_cfg_t hash_cfg; /**< Configuration for the HASH */
} wm_hal_hash_dev_t;

/**
 * @defgroup WM_HAL_HASH APIs
 * @brief WinnerMicro WM_HAL_HASH APIs
 */

/**
 * @addtogroup WM_HAL_HASH
 * @{
 */

/**
 * @brief Initialize the HASH.
 *
 * This function zeroize the HASH configuration, clear interrupt flag, unmask interrupt.
 *
 * @param[in] dev Pointer to the HASH device structure.
 *
 * @return
 *    - WM_ERR_SUCCESS: succeed
 *    - WM_ERR_INVALID_PARAM: invalid argument
 *    - Other error codes based on the initialization process
 */
int wm_hal_hash_init(wm_hal_hash_dev_t *dev);

/**
 * @brief Deinitialize the HASH.
 *
 * This function zeroize the HASH configuration, clear interrupt flag, mask interrupt.
 *
 * @param dev Pointer to the HASH device structure.
 *
 * @return
 *    - WM_ERR_SUCCESS: succeed
 *    - WM_ERR_INVALID_PARAM: invalid argument
 *    - Other error codes based on the deinitialization process
 */
int wm_hal_hash_deinit(wm_hal_hash_dev_t *dev);

/**
 * @brief Setup the HASH configuration.
 *
 * @param[in] dev Pointer to the HASH device structure.
 * @param[in] hash_cfg Configuration for the HASH
 * @param[in] alg HASH algorithm.
 *
 * @return
 *    - WM_ERR_SUCCESS: succeed
 */
int wm_hal_hash_start(wm_hal_hash_dev_t *dev, wm_hal_hash_cfg_t *hash_cfg, uint32_t alg);

/**
 * @brief Perform  a cryptographic hash function.
 *
 * @param[in] dev Pointer to the HASH device structure.
 * @param[in] cfg Pointer to the hash context of this op.
 * @return
 *    - WM_ERR_SUCCESS: succeed
 */
int wm_hal_hash_compress(wm_hal_hash_dev_t *dev, wm_hal_hash_cfg_t *cfg);

/**
 * @brief   Process a message block using HASH algorithm.
 *          This function performs a HASH block update operation. It continues an HASH message-digest operation,
 *          by processing len length message block pointed to by in, and by updating the HASH context pointed to by dev.
 *          This function may be called as many times as necessary, so the message may be processed in blocks.
 *
* @note     Digest is not retrieved by this API, user must resort to wm_hal_md5_final()
*           to read it.
 * @param[in] dev   Pointer to the HASH device structure.
 * @param[in] hash_cfg Configuration for the HASH
 * @param[in] buf   Pointer to the input buffer (buffer to be hashed).
 * @param[in] len   input data length in bytes
 *
 * @return
 *    - WM_ERR_SUCCESS: succeed
 *    - WM_ERR_INVALID_PARAM: invalid argument
 *    - Other error codes based on the update process
 */
int wm_hal_hash_update(wm_hal_hash_dev_t *dev, wm_hal_hash_cfg_t *hash_cfg, const unsigned char *buf, uint32_t len);

/**
 * @brief   End computation of a single HASH signature after several calls to wm_hal_md5_update() API.
 * @note    Digest is available in pOutBuffer.
 *
 * @param[in]   dev     Pointer to the HASH device structure.
 * @param[in]   hash_cfg Configuration for the HASH
 * @param[out]  out     pointer to the computed digest. Digest size is 20 bytes.
 *
 * @return HAL  status
 */
int wm_hal_hash_final(wm_hal_hash_dev_t *dev, wm_hal_hash_cfg_t *hash_cfg, unsigned char *out);

/**
 * @}
 */

/**
 * @defgroup WM_HAL_MD5 APIs
 * @brief WinnerMicro WM_HAL_MD5 APIs
 */

/**
 * @addtogroup WM_HAL_MD5
 * @{
 */

/**
 * @brief Initialize the MD5.
 *
 * This function zeroize the MD5 configuration, clear interrupt flag, unmask interrupt.
 *
 * @param[in] dev Pointer to the MD5 device structure.
 *
 * @return
 *    - WM_ERR_SUCCESS: succeed
 *    - WM_ERR_INVALID_PARAM: invalid argument
 *    - Other error codes based on the initialization process
 */
int wm_hal_md5_init(wm_hal_hash_dev_t *dev);

/**
 * @brief Deinitialize the MD5.
 *
 * This function zeroize the MD5 configuration, clear interrupt flag, mask interrupt.
 *
 * @param dev Pointer to the MD5 device structure.
 *
 * @return
 *    - WM_ERR_SUCCESS: succeed
 *    - WM_ERR_INVALID_PARAM: invalid argument
 *    - Other error codes based on the deinitialization process
 */
int wm_hal_md5_deinit(wm_hal_hash_dev_t *dev);

/**
 * @brief Setup the MD5 configuration.
 *
 * @param dev Pointer to the MD5 device structure.
 * @param hash_cfg Configuration for the HASH
 * 
 * @return
 *    - WM_ERR_SUCCESS: succeed
 */
int wm_hal_md5_start(wm_hal_hash_dev_t *dev, wm_hal_hash_cfg_t *hash_cfg);

/**
 * @brief   Process a message block using MD5 algorithm.
 *          This function performs a MD5 block update operation. It continues an MD5 message-digest operation,
 *          by processing len length message block pointed to by in, and by updating the MD5 context pointed to by dev.
 *          This function may be called as many times as necessary, so the message may be processed in blocks.
 *
* @note     Digest is not retrieved by this API, user must resort to wm_hal_md5_final()
*           to read it.
 * @param[in]   dev     Pointer to the MD5 device structure.
 * @param[in]   hash_cfg Configuration for the HASH
 * @param[in]   in      Pointer to the input buffer (buffer to be hashed).
 * @param[in]   len     input data length in bytes
 *
 * @return
 *    - WM_ERR_SUCCESS: succeed
 *    - WM_ERR_INVALID_PARAM: invalid argument
 *    - Other error codes based on the update process
 */
int wm_hal_md5_update(wm_hal_hash_dev_t *dev, wm_hal_hash_cfg_t *hash_cfg, const unsigned char *in, uint32_t len);

/**
 * @brief   End computation of a single HASH signature after several calls to wm_hal_md5_update() API.
 * @note    Digest is available in pOutBuffer.
 *
 * @param[in]   dev Pointer to the MD5 device structure.
 * @param[in]   hash_cfg Configuration for the HASH
 * @param[out]  out pointer to the computed digest. Digest size is 20 bytes.
 *
 * @return HAL  status
 */
int wm_hal_md5_final(wm_hal_hash_dev_t *dev, wm_hal_hash_cfg_t *hash_cfg, unsigned char *out);

/**
 * @}
 */

/**
 * @defgroup WM_HAL_SHA1 APIs
 * @brief WinnerMicro WM_HAL_SHA1 APIs
 */

/**
 * @addtogroup WM_HAL_SHA1
 * @{
 */

/**
 * @brief Initialize the SHA1.
 *
 * This function zeroize the SHA1 configuration, clear interrupt flag, unmask interrupt.
 *
 * @param[in] dev Pointer to the SHA1 device structure.
 *
 * @return
 *    - WM_ERR_SUCCESS: succeed
 *    - WM_ERR_INVALID_PARAM: invalid argument
 *    - Other error codes based on the initialization process
 */
int wm_hal_sha1_init(wm_hal_hash_dev_t *dev);

/**
 * @brief Deinitialize the SHA1.
 *
 * This function zeroize the SHA1 configuration, clear interrupt flag, mask interrupt.
 *
 * @param dev Pointer to the SHA1 device structure.
 *
 * @return
 *    - WM_ERR_SUCCESS: succeed
 *    - WM_ERR_INVALID_PARAM: invalid argument
 *    - Other error codes based on the deinitialization process
 */
int wm_hal_sha1_deinit(wm_hal_hash_dev_t *dev);

/**
 * @brief Setup the SHA1 configuration.
 *
 * @param dev Pointer to the SHA1 device structure.
 * @param hash_cfg Configuration for the HASH
 *
 * @return
 *    - WM_ERR_SUCCESS: succeed
 */
int wm_hal_sha1_start(wm_hal_hash_dev_t *dev, wm_hal_hash_cfg_t *hash_cfg);

/**
 * @brief   Process a message block using SHA1 algorithm.
 *          This function performs a SHA1 block update operation. It continues an SHA1 message-digest operation,
 *          by processing len length message block pointed to by in, and by updating the SHA1 context pointed to by dev.
 *          This function may be called as many times as necessary, so the message may be processed in blocks.
 *
* @note     Digest is not retrieved by this API, user must resort to wm_hal_sha1_final()
*           to read it.
 * @param[in]   dev     Pointer to the SHA1 device structure.
 * @param[in]   hash_cfg Configuration for the HASH
 * @param[in]   in      Pointer to the input buffer (buffer to be hashed).
 * @param[in]   len     input data length in bytes
 *
 * @return
 *    - WM_ERR_SUCCESS: succeed
 *    - WM_ERR_INVALID_PARAM: invalid argument
 *    - Other error codes based on the update process
 */
int wm_hal_sha1_update(wm_hal_hash_dev_t *dev, wm_hal_hash_cfg_t *hash_cfg, const unsigned char *in, uint32_t len);

/**
 * @brief   End computation of a single HASH signature after several calls to wm_hal_sha1_update() API.
 * @note    Digest is available in pOutBuffer.
 *
 * @param[in]   dev Pointer to the SHA1 device structure.
 * @param[in]   hash_cfg Configuration for the HASH
 * @param[out]  out pointer to the computed digest. Digest size is 20 bytes.
 *
 * @return HAL  status
 */
int wm_hal_sha1_final(wm_hal_hash_dev_t *dev, wm_hal_hash_cfg_t *hash_cfg, unsigned char *out);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* __WM_HAL_HASH_H__ */
