/**
 * @file wm_drv_crc.h
 *
 * @brief DRV_CRC Module
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

#ifndef __WM_DRV_CRC_H__
#define __WM_DRV_CRC_H__

#include "wm_soc_cfgs.h"
#include "wm_types.h"
#include "wm_dt.h"

#include "wm_hal_crc.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup WM_DRV_CRC_Type_Definitions WM DRV_CRC Type Definitions
 * @brief WinnerMicro DRV_CRC Type Definitions
 */

/**
 * @addtogroup WM_DRV_CRC_Type_Definitions
 * @{
 */

typedef wm_hal_crc_cfg_t wm_drv_crc_cfg_t;

/**
 * @}
 */

/**
 * @defgroup WM_DRV_CRC_Functions WM DRV_CRC Functions
 * @brief WinnerMicro DRV_CRC Functions
 */

/**
 * @addtogroup WM_DRV_CRC_Functions
 * @{
 */

/**
 * @brief  Initialize CRC driver
 *
 * @param  dev_name  CRC device name 
 *
 * @return
  *    - NULL: failed
  *    - others: CRC driver device
 *
 * @note This API must be called before all other CRC API can be called
 *
 */
wm_device_t *wm_drv_crc_init(const char *dev_name);

/**
 * @brief     Deinitialize CRC driver
 *
 * @param     dev  CRC driver device
 *
 * @return
 *    - WM_ERR_SUCCESS: succeed
 *    - WM_ERR_INVALID_PARAM: invalid argument
 *    - others: failed
 */
int wm_drv_crc_deinit(wm_device_t *dev);

/**
 * @brief Fill the wm_drv_crc_cfg_t structure pointed to by ctx with necessary data.
 *
 * @param [in]   dev        Pointer to the CRC device structure.
 * @param [in]   ctx        Pointer to the CRC Context.
 * @param [in]   state      The initial value input and result value output.
 * @param [in]   type       The type of CRC, reference the wm_gpsec_crc_type_t enum.
 * @param [in]   reverse    The reverse configuration of CRC, bit0 means output reverse and bit1 means input reverse.
 * @return
 *    - WM_ERR_SUCCESS: succeed
 *    - others: failed
 * @note    The parameter ctx must be a structure which is allocated externally.
 *          And all of Context parameters in the initializing methods should be allocated externally too.
 */
int wm_drv_crc_cfg(wm_device_t *dev, wm_drv_crc_cfg_t *ctx, uint32_t state, uint8_t type, uint8_t reverse);

/**
 * @brief   This function updates the CRC value with a variable length bytes.
 *          This function may be called as many times as necessary, so the message may be processed in blocks.
 *
 * @param[in]   dev     Pointer to the CRC device structure.
 * @param[in]   ctx     Pointer to the CRC Context.
 * @param[in]   in      Pointer to the input data buffer
 * @param[in]   len     input data length in bytes
 *
 * @return
 *    - WM_ERR_SUCCESS: succeed
 *    - WM_ERR_INVALID_PARAM: invalid argument
 *    - Other error codes based on the update process
 */
int wm_drv_crc_update(wm_device_t *dev, wm_drv_crc_cfg_t *ctx, unsigned char *in, uint32_t len);

/**
 * @brief   output CRC result.
 *
 * @param[in]   dev     Pointer to the CRC device structure.
 * @param[in]   ctx     Pointer to the CRC Context.
 * @param[out]  crc_val final result
 *
 * @return
 *    - WM_ERR_SUCCESS: succeed
 *    - WM_ERR_INVALID_PARAM: invalid argument
 */
int wm_drv_crc_final(wm_device_t *dev, wm_drv_crc_cfg_t *ctx, uint32_t *crc_val);

/**
 * @brief Generic function for computing a CRC-32 with input and output
 *        reflection.
 *
 * @note initial seed: 0xFFFFFFFF
 * @param buf Input bytes for the computation
 * @param len Length of the input in bytes
 *
 * @return The computed CRC32 value
 */
uint32_t wm_drv_crc32_reverse(const void *buf, unsigned int len);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* __WM_DRV_CRC_H__ */
