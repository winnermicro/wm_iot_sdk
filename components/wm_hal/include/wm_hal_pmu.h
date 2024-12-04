/**
 * @file wm_hal_pmu.h
 *
 * @brief HAL_PMU Module
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

#ifndef __WM_HAL_PMU_H__
#define __WM_HAL_PMU_H__

#include "wm_soc_cfgs.h"
#include "wm_types.h"

#include "wm_hal_irq.h"
#include "wm_ll_pmu.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup WM_HAL_PMU_Enumerations WM HAL_PMU Enumerations
 * @brief WinnerMicro HAL_PMU Enumerations
 */

/**
 * @addtogroup WM_HAL_PMU_Enumerations
 * @{
 */

/**
 * @brief Define pmu interrupt type
 *
 */
typedef enum {
    WM_PMU_IRQ_RTC = 0, /**< for RTC */
    WM_PMU_IRQ_TIMER,   /**< for PMU timer */
    WM_PMU_IRQ_IO_WAKE, /**< for PMU IO wake */

    WM_PMU_IRQ_MAX,
} wm_pmu_irq_type_t;

/**
 * @brief Define pmu low power mode
 *
 */
typedef enum {
    WM_PMU_POWER_MODE_NONE = 0, /**< normal mode */
    WM_PMU_POWER_MODE_SLEEP,    /**< sleep mode */
    WM_PMU_POWER_MODE_STANDBY,  /**< standby mode */

    WM_PMU_POWER_MODE_MAX,
} wm_pmu_power_mode_t;

/**
 * @brief Define pre wake-up status
 *
 */
typedef enum {
    WM_PMU_WAKEUP_PRE_STATUS_NONE,    /**< system is not wake up from low power mode */
    WM_PMU_WAKEUP_PRE_STATUS_SLEEP,   /**< wake up from sleep */
    WM_PMU_WAKEUP_PRE_STATUS_STANDBY, /**< wake up from standby */
} wm_pmu_wakeup_pre_status_t;

/**
 * @}
 */

/**
 * @defgroup WM_HAL_PMU_Type_Definitions WM HAL_PMU Type Definitions
 * @brief WinnerMicro HAL_PMU Type Definitions
 */

/**
 * @addtogroup WM_HAL_PMU_Type_Definitions
 * @{
 */

/**
 * @brief Prototype of PMU HAL callback function
 */
typedef void (*hal_pmu_callback_t)(wm_pmu_irq_type_t type, void *ptr);

/**
 * @}
 */

/**
 * @defgroup WM_HAL_PMU_Structures WM HAL_PMU Structures
 * @brief WinnerMicro HAL_PMU Structures
 */

/**
 * @addtogroup WM_HAL_PMU_Structures
 * @{
 */

/**
 * @brief PMU wakeup io config
 */
typedef struct {
    uint8_t wake_thres_level; /**< wakeup io trigger time */
} wm_pmu_wakeup_io_config_t;

/**
 * @brief HAL PMU structure
 */
typedef struct {
    bool is_init;                 /**< PMU is already initialized?  */
    wm_irq_no_t irq_num;          /**< interrupt number for PMU module */
    wm_pmu_reg_t *reg_base;       /**< PMU controller reg base addr info from device table */
    hal_pmu_callback_t call_back; /**< interrupt callback function */
    void *user_data;              /**< used for interrupt callback function */
} wm_hal_pmu_dev_t;

/**
 * @}
 */

/**
 * @defgroup WM_HAL_PMU_Functions WM HAL_PMU Functions
 * @brief WinnerMicro HAL_PMU Functions
 */

/**
 * @addtogroup WM_HAL_PMU_Functions
 * @{
 */

/**
 * @brief  Initialize PMU hardware
 *
 * @param[in]     dev      PMU HAL device
 *
 * @return
 *    - WM_ERR_SUCCESS: succeed
 *    - WM_ERR_INVALID_PARAM: invalid argument
 *    - others: failed
 */
int wm_hal_pmu_init(wm_hal_pmu_dev_t *dev);

/**
 * @brief  Deinitialize PMU hardware
 *
 * @param[in]     dev      PMU HAL device
 *
 * @return
 *    - WM_ERR_SUCCESS: succeed
 *    - WM_ERR_INVALID_PARAM: invalid argument
 *    - others: failed
 *
 * @note This is dummy function, it will not uninstall PMU.
 */
int wm_hal_pmu_deinit(wm_hal_pmu_dev_t *dev);

/**
 * @brief  Set PMU clock source
 *
 * @param[in]     dev           PMU HAL device
 * @param[in]     clk_src       PMU clock source @ref wm_pmu_clock_source_t
 *
 * @return
 *    - WM_ERR_SUCCESS: succeed
 *    - WM_ERR_INVALID_PARAM: invalid argument
 *    - others: failed
 */
int wm_hal_pmu_set_clock_source(wm_hal_pmu_dev_t *dev, wm_pmu_clock_source_t clk_src);

/**
 * @brief  Calibration PMU internal RC circuit
 *
 * @param[in]     dev      PMU HAL device
 *
 * @return
 *    - WM_ERR_SUCCESS: succeed
 *    - WM_ERR_INVALID_PARAM: invalid argument
 *    - others: failed
 */
int wm_hal_pmu_calibration_internal_rc(wm_hal_pmu_dev_t *dev);

/**
 * @brief  Set PMU power mode
 *
 * @param[in]     dev      PMU HAL device
 * @param[in]     mode     PMU power mode @ wm_pmu_power_mode_t
 *
 * @return
 *    - WM_ERR_SUCCESS: succeed
 *    - WM_ERR_INVALID_PARAM: invalid argument
 *    - others: failed
 *
 * @warning Use this API, may cause the device  enter sleep or standby mode.
 *          Please set the wake-up source in advance.
 */
int wm_hal_pmu_set_power_mode(wm_hal_pmu_dev_t *dev, wm_pmu_power_mode_t mode);

/**
 * @brief  Set PMU wakeup io config
 *
 * @param[in]     dev      PMU HAL device
 * @param[in]     config   Wakeup io config @ wm_pmu_wakeup_io_config_t
 *
 * @return
 *    - WM_ERR_SUCCESS: succeed
 *    - WM_ERR_INVALID_PARAM: invalid argument
 *    - others: failed
 *
 * @note wakeup io duration time can be determined by WM_PMU_WAKEUP_IO_THRES_LEVEL
 */
int wm_hal_pmu_set_wakeup_io(wm_hal_pmu_dev_t *dev, wm_pmu_wakeup_io_config_t *config);

/**
 * @brief  Register HAL pmu callback function
 *
 * @param[in]     dev       PMU HAL device
 * @param[in]     call_back Callback function
 * @param[in]     user_data User data for callback function
 *
 * @return
 *    - WM_ERR_SUCCESS: succeed
 *    - WM_ERR_INVALID_PARAM: invalid argument
 *    - others: failed
 */
int wm_hal_pmu_register_callback(wm_hal_pmu_dev_t *dev, hal_pmu_callback_t call_back, void *user_data);

/**
 * @brief  Query pre wakeup status
 *
 * @param[in]  dev       PMU HAL device
 * @param[out] status    Status write back address @ wm_pmu_wakeup_pre_status_t
 *
 * @return
 *    - WM_ERR_SUCCESS: succeed
 *    - WM_ERR_INVALID_PARAM: invalid argument
 *    - others: failed
 */
int wm_hal_pmu_get_status_before_wakeup(wm_hal_pmu_dev_t *dev, wm_pmu_wakeup_pre_status_t *status);

/**
 * @brief  Set PMU timer0
 *
 * @param[in]     dev       PMU HAL device
 * @param[in]     time_s    PMU timer wakeup time (second)
 *
 * @return
 *    - WM_ERR_SUCCESS: succeed
 *    - WM_ERR_INVALID_PARAM: invalid argument
 *    - others: failed
 *
 * @note W800 time_s range is 1~65535
 */
int wm_hal_pmu_set_timer0(wm_hal_pmu_dev_t *dev, uint32_t time_s);

/**
 * @brief  Abort PMU timer0
 *
 * @param[in]     dev       PMU HAL device
 *
 * @return
 *    - WM_ERR_SUCCESS: succeed
 *    - WM_ERR_INVALID_PARAM: invalid argument
 *    - others: failed
 */
int wm_hal_pmu_abort_timer0(wm_hal_pmu_dev_t *dev);

/**
 * @brief  Set PMU timer1
 *
 * @param[in]     dev       PMU HAL device
 * @param[in]     time_ms   PMU timer wakeup time (millisecond)
 *
 * @return
 *    - WM_ERR_SUCCESS: succeed
 *    - WM_ERR_INVALID_PARAM: invalid argument
 *    - others: failed
 *
 * @note W800 time_ms range is 1~65535
 */
int wm_hal_pmu_set_timer1(wm_hal_pmu_dev_t *dev, uint32_t time_ms);

/**
 * @brief  Abort PMU timer1
 *
 * @param[in]     dev       PMU HAL device
 *
 * @return
 *    - WM_ERR_SUCCESS: succeed
 *    - WM_ERR_INVALID_PARAM: invalid argument
 *    - others: failed
 */
int wm_hal_pmu_abort_timer1(wm_hal_pmu_dev_t *dev);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* __WM_HAL_PMU_H__ */
