/**
 * @file wm_drv_pmu.h
 *
 * @brief DRV_PMU Module
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

#ifndef __WM_DRV_PMU_H__
#define __WM_DRV_PMU_H__

#include "wm_soc_cfgs.h"
#include "wm_types.h"
#include "wm_dt.h"

#include "wm_hal_pmu.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup WM_DRV_PMU_Type_Definitions WM DRV_PMU Type Definitions
 * @brief WinnerMicro DRV_PMU Type Definitions
 */

/**
 * @addtogroup WM_DRV_PMU_Type_Definitions
 * @{
 */

/**
 * @brief pmu interrupt callback function
 *
 * @param[in] user_data  user data when registering callback
 *
 * @return
 */

typedef void (*wm_pmu_callback_t)(void *user_data);

/**
 * @}
 */

/**
 * @defgroup WM_DRV_PMU_Functions WM DRV_PMU Functions
 * @brief WinnerMicro DRV_PMU Functions
 */

/**
 * @addtogroup WM_DRV_PMU_Functions
 * @{
 */

/**
 * @brief  Initialize PMU device
 *
 * @param[in]     dev_name   PMU device name
 *
 * @return
 *    - NULL: failed
 *    - others: succeed
 *
 * @note This function will initialize WAKEUP IO minimum holding time @ WM_800_PMU_WAKEUP_IO_THRES_LEVEL
 */
wm_device_t *wm_drv_pmu_init(const char *dev_name);

/**
 * @brief  Deinitialize PMU device
 *
 * @param[in]     dev      PMU driver device
 *
 * @return
 *    - WM_ERR_SUCCESS: succeed
 *
 * @note This is dummy function, it will not uninstall PMU.
 */
int wm_drv_pmu_deinit(wm_device_t *dev);

/**
 * @brief  Set PMU clock source
 *
 * @param[in]     dev      PMU driver device
 * @param[in]     clk_src  PMU clock source @ wm_pmu_clock_source_t
 *
 * @return
 *    - WM_ERR_SUCCESS: succeed
 *    - WM_ERR_INVALID_PARAM: invalid argument
 *    - others: failed
 */
int wm_drv_pmu_set_clock_source(wm_device_t *dev, wm_pmu_clock_source_t clk_src);

/**
 * @brief  Set PMU power mode
 *
 * @param[in]     dev      PMU driver device
 * @param[in]     mode     PMU power mode @ wm_pmu_power_mode_t
 *
 * @return
 *    - WM_ERR_SUCCESS: succeed
 *    - WM_ERR_INVALID_PARAM: invalid argument
 *    - others: failed
 *
 * @warning Use this API, may cause the device enter sleep or standby mode.
 *          Please set the wake-up source in advance.
 */
int wm_drv_pmu_set_power_mode(wm_device_t *dev, wm_pmu_power_mode_t mode);

/**
 * @brief  Register PMU IRQ callback
 *
 * @param[in]     dev       PMU driver device
 * @param[in]     type      PMU irq type @ wm_pmu_irq_type_t
 * @param[in]     callback  User's callback function
 * @param[in]     user_data Data for user callback
 *
 * @return
 *    - WM_ERR_SUCCESS: succeed
 *    - WM_ERR_INVALID_PARAM: invalid argument
 *    - others: failed
 */
int wm_drv_pmu_register_irq_callback(wm_device_t *dev, wm_pmu_irq_type_t type, wm_pmu_callback_t callback, void *user_data);

/**
 * @brief  Unregister PMU IRQ callback
 *
 * @param[in]     dev       PMU driver device
 * @param[in]     type      PMU irq type @ wm_pmu_irq_type_t
 *
 * @return
 *    - WM_ERR_SUCCESS: succeed
 *    - WM_ERR_INVALID_PARAM: invalid argument
 *    - others: failed
 */
int wm_drv_pmu_unregister_irq_callback(wm_device_t *dev, wm_pmu_irq_type_t type);

/**
 * @brief  Query the PMU pre wakeup status
 *
 * @param[in]   dev       PMU driver device
 * @param[out]  status    Pre wakeup status write back address @ wm_pmu_wakeup_pre_status_t
 *
 * @return
 *    - WM_ERR_SUCCESS: succeed
 *    - WM_ERR_INVALID_PARAM: invalid argument
 *    - others: failed
 */
int wm_drv_pmu_get_status_before_wakeup(wm_device_t *dev, wm_pmu_wakeup_pre_status_t *status);

/**
 * @brief  Enable PMU timer
 *
 * @param[in]     dev       PMU driver device
 * @param[in]     time_ms   Timer times (millisecond)
 *
 * @return
 *    - WM_ERR_SUCCESS: succeed
 *    - WM_ERR_INVALID_PARAM: invalid argument
 *    - others: failed
 *
 * @note W800 time range is 1 ms ~ 65535 s.
 * @note W800 limitation: If using 40 M frequency division, any time below 3 ms will be treated as 3 ms.
 *                        If using 32 K clock, any time below 5 ms will be treated as 5 ms.
 * @note Insufficient precision of PMU timer, not recommended for use as a normal timer
 */
int wm_drv_pmu_set_timer(wm_device_t *dev, uint32_t time_ms);

/**
 * @brief  Abort PMU timer
 *
 * @param[in]     dev       PMU driver device
 *
 * @return
 *    - WM_ERR_SUCCESS: succeed
 *    - WM_ERR_INVALID_PARAM: invalid argument
 *    - others: failed
 */
int wm_drv_pmu_abort_timer(wm_device_t *dev);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* __WM_DRV_PMU_H__ */
