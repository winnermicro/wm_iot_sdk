/**
 * @file wm_hal_rng.h
 *
 * @brief RNG HAL Module
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

#ifndef __WM_HAL_RNG_H__
#define __WM_HAL_RNG_H__

#include <string.h>
#include "wm_osal.h"
#include "wm_types.h"
#include "wm_soc_cfgs.h"
#include "wm_ll_gpsec.h"

#ifdef __cplusplus
extern "C" {
#endif

#define IS_RNG_TYPE(type)          ((type) == WM_GPSEC_RESULT_DISP_TRNG || (type) == WM_GPSEC_RESULT_DISP_PRNG)
#define IS_PRNG_BIT_WITH(bit_with) ((bit_with) == WM_GPSEC_RNG_16BIT || (bit_with) == WM_GPSEC_RNG_32BIT)
/**
 * @brief RNG Configuration structure.
 *
 * The RNG Configuration structure contains various configuration
 * parameters required for setting up and controlling the RNG.
 */
typedef struct {
    wm_gpsec_result_select_t type;  /*!<Choose to generate true random numbers or pseudo-random, 0 for prng and 1 for trng */
    uint32_t seed;                  /*!<the seed for generating pseudo random numbers. */
    wm_gpsec_prng_bits_t bit_width; /*!<Choose to generate 16-bit pseudo random numbers or 32-bit pseudo random numbers.
                                            note: The W800 can only generate 32-bit true random numbers*/
} wm_hal_rng_cfg_t;

/**
 * @brief RNG Device structure.
 *
 * The RNG structure contains the base address of the gpsec registers
 * and a configuration structure for the RNG. It is used to manage
 * and interact with the RNG hardware.
 * Note:In W800, RNG/SHA/MD5/CRC/AES/DES/3DES share the same hardware module, 
 *      therefore, only one function can be implemented at any given time.
 */
typedef struct {
    wm_gpsec_reg_t *gpsec_reg; /**< Pointer to the base address of gpsec registers */
    wm_hal_rng_cfg_t rng_cfg;  /**< Configuration for the RNG */
} wm_hal_rng_dev_t;

/**
 * @defgroup WM_HAL_RNG APIs
 * @brief WinnerMicro WM_HAL_RNG APIs
 */

/**
 * @addtogroup WM_HAL_RNG
 * @{
 */

/**
 * @brief Initialize the RNG.
 *
 * This function sets up the RNG device with the provided configuration.
 *
 * @param dev Pointer to the RNG device structure.
 *
 * @return
 *    - WM_ERR_SUCCESS: succeed
 *    - WM_ERR_INVALID_PARAM: invalid argument
 *    - Other error codes based on the initialization process
 */
int wm_hal_rng_init(wm_hal_rng_dev_t *dev);

/**
 * @brief Deinitialize the RNG.
 *
 * @param dev Pointer to the RNG device structure.
 *
 * @return
 *    - WM_ERR_SUCCESS: succeed
 *    - Other error codes based on the deinitialization process
 */
int wm_hal_rng_deinit(wm_hal_rng_dev_t *dev);

/**
 * @brief  Get one random from hardware RNG
 *          note: If the generated random numbers are true random numbers, 
 *          they are 32-bit. If they are pseudo-random numbers, they can be either 16-bit 
 *          or 32-bit depending on the configuration at initialization.
 * @return Random value
 */
uint32_t wm_hal_rng_read(wm_hal_rng_dev_t *dev);

/**
 * @brief Fill a buffer with random from hardware RNG
 *
 * @note This function is implemented via calls to wm_hal_rng_read()
 *
 * @param dev Pointer to the RNG device structure
 * @param buf Pointer to buffer to fill with random numbers.
 * @param len Length of buffer in bytes
 * @return
 *    - WM_ERR_SUCCESS: succeed
 *    - WM_ERR_INVALID_PARAM: invalid argument
 *    - Other error codes based on the update process
 */
int wm_hal_rng_read_bytes(wm_hal_rng_dev_t *dev, void *buf, uint32_t len);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* __WM_HAL_RNG_H__ */
