/**
 * @file wm_drv_rng.h
 *
 * @brief DRV_RNG Module
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

#ifndef __WM_DRV_RNG_H__
#define __WM_DRV_RNG_H__

#include "wm_soc_cfgs.h"
#include "wm_types.h"
#include "wm_dt.h"

#include "wm_hal_rng.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup WM_DRV_RNG_Type_Definitions WM DRV_RNG Type Definitions
 * @brief WinnerMicro DRV_RNG Type Definitions
 */

/**
 * @addtogroup WM_DRV_RNG_Type_Definitions
 * @{
 */

typedef wm_hal_rng_cfg_t wm_drv_rng_cfg_t;

/**
 * @}
 */

/**
 * @defgroup WM_DRV_RNG_Functions WM DRV_RNG Functions
 * @brief WinnerMicro DRV_RNG Functions
 */

/**
 * @addtogroup WM_DRV_RNG_Functions
 * @{
 */

/**
 * @brief  Initialize RNG driver
 *
 * @param  dev_name  RNG device name
 *
 * @return
  *    - NULL: failed
  *    - others: RNG driver device
 *
 * @note This API must be called before all other RNG API can be called
 *
 */
wm_device_t *wm_drv_rng_init(const char *dev_name);

/**
 * @brief     Release access to GPSEC hardware and deinitialize RNG driver
 *
 * @param     dev  RNG driver device
 *
 * @return
 *    - WM_ERR_SUCCESS: succeed
 *    - WM_ERR_INVALID_PARAM: invalid argument
 *    - others: failed
 */
int wm_drv_rng_deinit(wm_device_t *dev);

/**
 * @brief  Get one random from hardware RNG
 * @param [in]   dev    Pointer to the RNG device structure.
 * @return Random value
 */
uint32_t wm_drv_rng_read(wm_device_t *dev);

/**
 * @brief Fill a buffer with random from hardware RNG
 *
 * @param [in]   dev Pointer to the RNG device structure.
 * @param [in]   buf Pointer to buffer to fill with random numbers.
 * @param [in]   len Length of buffer in bytes
 * @return
 *    - WM_ERR_SUCCESS: succeed
 *    - WM_ERR_INVALID_PARAM: invalid argument
 *    - others: failed
 */
int wm_drv_rng_read_bytes(wm_device_t *dev, void *buf, uint32_t len);

/**
 * @brief Fill a buffer with random from hardware RNG
 *
 * @param [in]   buf Pointer to buffer to fill with random numbers.
 * @param [in]   len Length of buffer in bytes
 * @return
 *    - WM_ERR_SUCCESS: succeed
 *    - others: failed
 */
int wm_drv_fill_random(void *buf, uint32_t len);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* __WM_DRV_RNG_H__ */
