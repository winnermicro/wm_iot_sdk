/**
 * @file wm_hal_crc.h
 *
 * @brief CRC HAL Module
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

#ifndef __WM_HAL_CRC_H__
#define __WM_HAL_CRC_H__

#include <string.h>
#include "wm_osal.h"
#include "wm_types.h"
#include "wm_soc_cfgs.h"
#include "wm_ll_gpsec.h"

#ifdef __cplusplus
extern "C" {
#endif

#define IS_CRC_TYPE(TYPE) \
    ((TYPE) == WM_GPSEC_CRC8 || (TYPE) == WM_GPSEC_CRC16_MODBUS || (TYPE) == WM_GPSEC_CRC16_CCITT || (TYPE) == WM_GPSEC_CRC32)

#define IS_CRC_REVERSE(REVERSE)                                                         \
    ((REVERSE) == WM_GPSEC_CRC_REVERSE_NONE || (REVERSE) == WM_GPSEC_CRC_OUT_REVERSE || \
     (REVERSE) == WM_GPSEC_CRC_IN_REVERSE || (REVERSE) == WM_GPSEC_CRC_OUT_IN_REVERSE)

/**
 * @brief CRC Configuration structure.
 *
 * The CRC Configuration structure contains various configuration
 * parameters required for setting up and controlling the CRC.
 */
typedef struct {
    uint32_t state;  /**< The initial value input and result value output. */
    uint8_t type;    /**< The type of CRC, reference the wm_gpsec_crc_type_t enum. */
    uint8_t reverse; /**< The reverse configuration of CRC, bit0 means output reverse and bit1 means input reverse. */
} wm_hal_crc_cfg_t;

/**
 * @brief CRC Device structure.
 *
 * The CRC structure contains the base address of the gpsec registers
 * and a configuration structure for the CRC. It is used to manage
 * and interact with the CRC hardware.
 * Note:In W800, RNG/SHA/MD5/CRC/AES/DES/3DES share the same hardware module, 
 *      therefore, only one function can be implemented at any given time.
 */
typedef struct {
    wm_gpsec_reg_t *gpsec_reg; /**< Pointer to the base address of gpsec registers */
    wm_hal_crc_cfg_t crc_cfg;  /**< Configuration for the CRC */
} wm_hal_crc_dev_t;

/**
 * @defgroup WM_HAL_CRC APIs
 * @brief WinnerMicro WM_HAL_CRC APIs
 */

/**
 * @addtogroup WM_HAL_CRC
 * @{
 */

/**
 * @brief Initialize the CRC.
 *
 * This function sets up the CRC device with the provided configuration.
 *
 * @param[in] dev Pointer to the CRC device structure.
 *
 * @return
 *    - WM_ERR_SUCCESS: succeed
 *    - WM_ERR_INVALID_PARAM: invalid argument
 *    - Other error codes based on the initialization process
 */
int wm_hal_crc_init(wm_hal_crc_dev_t *dev);

/**
 * @brief   This function updates the CRC value with a variable length bytes.
 *          This function may be called as many times as necessary, so the message may be processed in blocks.
 *
 * @param[in]   dev     Pointer to the CRC device structure.
 * @param[in]   in      Pointer to the input data buffer
 * @param[in]   len     input data length in bytes
 *
 * @return
 *    - WM_ERR_SUCCESS: succeed
 *    - WM_ERR_INVALID_PARAM: invalid argument
 *    - Other error codes based on the update process
 */
int wm_hal_crc_update(wm_hal_crc_dev_t *dev, unsigned char *in, uint32_t len);

/**
 * @brief   output CRC result.
 *
 * @param[in]   dev     Pointer to the CRC device structure.
 * @param[out]  crc_val final result
 *
 * @return
 *    - WM_ERR_SUCCESS: succeed
 *    - WM_ERR_INVALID_PARAM: invalid argument
 *    - Other error codes based on the update process
 */
int wm_hal_crc_final(wm_hal_crc_dev_t *dev, uint32_t *crc_val);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* __WM_HAL_CRC_H__ */
