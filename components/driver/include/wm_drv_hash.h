/**
 * @file wm_drv_hash.h
 *
 * @brief DRV_HASH Module
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

#ifndef __WM_DRV_HASH_H__
#define __WM_DRV_HASH_H__

#include "wm_soc_cfgs.h"
#include "wm_types.h"
#include "wm_dt.h"

#include "wm_hal_hash.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup WM_DRV_HASH_Type_Definitions WM DRV_HASH Type Definitions
 * @brief WinnerMicro DRV_HASH Type Definitions
 */

/**
 * @addtogroup WM_DRV_HASH_Type_Definitions
 * @{
 */

typedef wm_hal_hash_cfg_t wm_drv_hash_cfg_t;

/**
 * @}
 */

/**
 * @defgroup WM_DRV_HASH_Functions WM DRV_HASH Functions
 * @brief WinnerMicro DRV_HASH Functions
 */

/**
 * @addtogroup WM_DRV_HASH_Functions
 * @{
 */

/**
 * @brief  Initialize HASH driver
 *
 * @param  dev_name  HASH device name
 *
 * @return
  *    - NULL: failed
  *    - others: HASH driver device
 *
 * @note This API must be called before all other HASH API can be called
 *
 */
wm_device_t *wm_drv_hash_init(const char *dev_name);

/**
 * @brief     Deinitialize HASH driver
 *
 * @param     dev  HASH driver device
 *
 * @return
 *    - WM_ERR_SUCCESS: succeed
 *    - WM_ERR_INVALID_PARAM: invalid argument
 *    - others: failed
 */
int wm_drv_hash_deinit(wm_device_t *dev);

/**
 * @brief Setup the MD5 configuration.
 *
 * @param[in]   dev Pointer to the MD5 device structure.
 * @param[in]   ctx    Pointer to a MD5 Context.
 *
 * @return
 *    - WM_ERR_SUCCESS: succeed
 *    - others: failed
 * @note    The parameter ctx must be a structure which is allocated externally.
 */
int wm_drv_md5_start(wm_device_t *dev, wm_drv_hash_cfg_t *ctx);

/**
 * @brief   Process a message block using MD5 algorithm.
 *          This function performs a MD5 block update operation. It continues an MD5 message-digest operation,
 *          by processing len length message block pointed to by in, and by updating the MD5 context pointed to by dev.
 *          This function may be called as many times as necessary, so the message may be processed in blocks.
 *
* @note     Digest is not retrieved by this API, user must resort to wm_drv_md5_final()
*           to read it.
 * @param[in]   dev     Pointer to the MD5 device structure.
 * @param[in]   ctx    Pointer to a MD5 Context.
 * @param[in]   in      Pointer to the input buffer (buffer to be hashed).
 * @param[in]   len     input data length in bytes
 *
 * @return
 *    - WM_ERR_SUCCESS: succeed
 *    - WM_ERR_INVALID_PARAM: invalid argument
 *    - Other error codes based on the update process
 */
int wm_drv_md5_update(wm_device_t *dev, wm_drv_hash_cfg_t *ctx, const unsigned char *in, uint32_t len);

/**
 * @brief   End computation of a single HASH signature after several calls to wm_drv_md5_update() API.
 * @note    Digest is available in pOutBuffer.
 *
 * @param[in]   dev Pointer to the MD5 device structure.
 * @param[in]   ctx Pointer to a MD5 Context.
 * @param[out]  out pointer to the computed digest. Digest size is 20 bytes.
 *
 *    - WM_ERR_SUCCESS: succeed
 *    - WM_ERR_INVALID_PARAM: invalid argument
 *    - Other error codes based on the update process
 */
int wm_drv_md5_final(wm_device_t *dev, wm_drv_hash_cfg_t *ctx, unsigned char *out);

/**
 * @brief   Compute a single HASH signature.
 *
 * @param[in]   in_buf Pointer to the input buffer (buffer to be hashed).
 * @param[in]   in_len input data length in bytes.
 * @param[out]  md5_hash pointer to the computed digest. Digest size is 20 bytes..
 *
 *    - WM_ERR_SUCCESS: succeed
 *    - WM_ERR_INVALID_PARAM: invalid argument
 *    - Other error codes based on the update process
 */
int wm_drv_md5(const void *in_buf, unsigned int in_len, void *md5_hash);

/**
 * @brief Setup the SHA1 configuration.
 *
 * @param[in]   dev Pointer to the SHA1 device structure.
 * @param[in]   ctx Pointer to a SHA1 Context.
 *
 * @return
 *    - WM_ERR_SUCCESS: succeed
 *    - others: failed
 */
int wm_drv_sha1_start(wm_device_t *dev, wm_drv_hash_cfg_t *ctx);

/**
 * @brief   Process a message block using SHA1 algorithm.
 *          This function performs a SHA1 block update operation. It continues an SHA1 message-digest operation,
 *          by processing len length message block pointed to by in, and by updating the SHA1 context pointed to by dev.
 *          This function may be called as many times as necessary, so the message may be processed in blocks.
 *
* @note     Digest is not retrieved by this API, user must resort to wm_drv_sha1_final()
*           to read it.
 * @param[in]   dev     Pointer to the SHA1 device structure.
 * @param[in]   ctx     Pointer to a SHA1 Context.
 * @param[in]   in      Pointer to the input buffer (buffer to be hashed).
 * @param[in]   len     input data length in bytes
 *
 * @return
 *    - WM_ERR_SUCCESS: succeed
 *    - WM_ERR_INVALID_PARAM: invalid argument
 *    - Other error codes based on the update process
 */
int wm_drv_sha1_update(wm_device_t *dev, wm_drv_hash_cfg_t *ctx, const unsigned char *in, uint32_t len);

/**
 * @brief   End computation of a single HASH signature after several calls to wm_drv_sha1_update() API.
 * @note    Digest is available in pOutBuffer.
 *
 * @param[in]   dev Pointer to the SHA1 device structure.
 * @param[in]   ctx Pointer to a SHA1 Context.
 * @param[out]  out pointer to the computed digest. Digest size is 20 bytes.
 *
 *    - WM_ERR_SUCCESS: succeed
 *    - WM_ERR_INVALID_PARAM: invalid argument
 *    - Other error codes based on the update process
 */
int wm_drv_sha1_final(wm_device_t *dev, wm_drv_hash_cfg_t *ctx, unsigned char *out);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* __WM_DRV_HASH_H__ */
