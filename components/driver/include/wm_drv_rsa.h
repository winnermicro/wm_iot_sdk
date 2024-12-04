/**
 * @file wm_drv_rsa.h
 *
 * @brief DRV_RSA Module
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

#ifndef __WM_DRV_RSA_H__
#define __WM_DRV_RSA_H__

#include "wm_soc_cfgs.h"
#include "wm_types.h"
#include "wm_dt.h"

#include "wm_hal_rsa.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup WM_DRV_RSA_Functions WM DRV_RSA Functions
 * @brief WinnerMicro DRV_RSA Functions
 */

/**
 * @addtogroup WM_DRV_RSA_Functions
 * @{
 */

/**
 * @brief   Initialize RSA driver.
 *
 * @param   dev_name  RSA device name
 *
 * @return
  *    - NULL: failed
  *    - others: RSA driver device
 *
 * @note This API must be called before all other RSA API can be called
 *
 */
wm_device_t *wm_drv_rsa_init(const char *dev_name);

/**
 * @brief     Deinitialize RSA driver
 *
 * @param     dev  RSA driver device
 *
 * @return
 *    - WM_ERR_SUCCESS: succeed
 *    - WM_ERR_INVALID_PARAM: invalid argument
 *    - others: failed
 */
int wm_drv_rsa_deinit(wm_device_t *dev);

/**
 * @brief This function implements the large module power multiplication algorithm.
 *        X = A^E mod N.
 *
 * @param[in]   dev   Pointer to the RSA device structure.
 * @param[out]  X Pointer to a bignumber. used to store the result of A^E mod N
 * @param[in]   A Pointer to a bignumber. base
 * @param[in]   E Pointer to a bignumber. exponent
 * @param[in]   N Pointer to a bignumber. modulus
 * @return 0
 */
int wm_drv_rsa_expt_mod(wm_device_t *dev, mbedtls_mpi *X, const mbedtls_mpi *A, const mbedtls_mpi *E, const mbedtls_mpi *N);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* __WM_DRV_RSA_H__ */
