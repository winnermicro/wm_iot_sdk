/**
 * @file wm_hal_rsa.h
 *
 * @brief RSA HAL Module
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

#ifndef __WM_HAL_RSA_H__
#define __WM_HAL_RSA_H__

#include <string.h>
#include "wm_osal.h"
#include "wm_types.h"
#include "wm_soc_cfgs.h"
#include "wm_ll_rsa.h"
#include "mbedtls/bignum.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
  * @brief RSA State structures definition
  */
typedef enum {
    WM_RSA_STATE_RESET = 0x00U, /*!< Peripheral is not initialized            */
    WM_RSA_STATE_READY = 0x01U, /*!< Peripheral Initialized and ready for use */
    WM_RSA_STATE_BUSY  = 0x02U, /*!< Processing is ongoing          */
} wm_rsa_state_t;

/**
 * @brief RSA Device structure.
 *
 * The RSA structure contains the base address of the rsa registers.
 */
typedef struct {
    wm_rsa_reg_t *reg_base; /**< Pointer to the base address of gpsec registers */
} wm_hal_rsa_dev_t;

/**
 * @defgroup WM_HAL_RSA APIs
 * @brief WinnerMicro WM_HAL_RSA APIs
 */

/**
 * @addtogroup WM_HAL_RSA
 * @{
 */

#define WM_HAL_RSA_MAX_HARD_EXPTMOD_WORDLEN (64)
#define WM_HAL_RSA_MAX_HARD_EXPTMOD_BITLEN  (2048)

/**
 * @brief This function implements the large module power multiplication algorithm.
 *        X = A^E mod N.
 *
 * @param dev Pointer to the RSA device structure.
 * @param[out]  X Pointer to a bignumber. used to store the result of A^E mod N
 * @param[in]   A Pointer to a bignumber. base
 * @param[in]   E Pointer to a bignumber. exponent
 * @param[in]   N Pointer to a bignumber. modulus
 * @return 0
 */
int wm_hal_rsa_expt_mod(wm_hal_rsa_dev_t *dev, mbedtls_mpi *X, const mbedtls_mpi *A, const mbedtls_mpi *E,
                        const mbedtls_mpi *N);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* __WM_HAL_RSA_H__ */
