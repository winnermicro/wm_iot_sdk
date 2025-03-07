/**
 * @file wm_pm.h
 *
 * @brief Power management Module
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

#ifndef __WM_PM_H__
#define __WM_PM_H__

#include "wm_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup WM_PM_Enumerations WM PM Enumerations
 * @brief WinnerMicro PM Enumerations
 */

/**
 * @addtogroup WM_PM_Enumerations
 * @{
 */
typedef enum {
    WM_PM_MODE_NO_SLEEP    = 0, /**< nonsleeping */
    WM_PM_MODE_LIGHT_SLEEP = 1, /**< light sleep */
    WM_PM_MODE_DEEP_SLEEP  = 2, /**< deep sleep. Some peripherals do not remain operational */
    WM_PM_MODE_STANDBY     = 3, /**< standby mode. Upon awakening, the system will perform from scratch */
} wm_pm_mode_t;

typedef enum {
    WM_PM_SLEEP_DEV_TYPE_I2C   = (1 << 0), /**< peripheral type : I2C */
    WM_PM_SLEEP_DEV_TYPE_UART0 = (1 << 1), /**< peripheral type : UART0 */
    WM_PM_SLEEP_DEV_TYPE_UART1 = (1 << 2), /**< peripheral type : UART1 */
    WM_PM_SLEEP_DEV_TYPE_UART2 = (1 << 3), /**< peripheral type : UART2 */
    WM_PM_SLEEP_DEV_TYPE_UART3 = (1 << 4), /**< peripheral type : UART3 */
    WM_PM_SLEEP_DEV_TYPE_UART4 = (1 << 5), /**< peripheral type : UART4 */

    WM_PM_SLEEP_DEV_TYPE_UART5 = (1 << 6), /**< peripheral type : UART4 */

    WM_PM_SLEEP_DEV_TYPE_LSPI = (1 << 7), /**< peripheral type : LSPI */
    WM_PM_SLEEP_DEV_TYPE_DMA  = (1 << 8), /**< peripheral type : DMA */

    WM_PM_SLEEP_DEV_TYPE_TIMER = (1 << 10), /**< peripheral type : TIMER */
    WM_PM_SLEEP_DEV_TYPE_GPIO  = (1 << 11), /**< peripheral type : GPIO */
    WM_PM_SLEEP_DEV_TYPE_SDADC = (1 << 12), /**< peripheral type : SDADC */
    WM_PM_SLEEP_DEV_TYPE_PWM   = (1 << 13), /**< peripheral type : PWM */
    WM_PM_SLEEP_DEV_TYPE_LCD   = (1 << 14), /**< peripheral type : LCD */
    WM_PM_SLEEP_DEV_TYPE_I2S   = (1 << 15), /**< peripheral type : I2S */
    WM_PM_SLEEP_DEV_TYPE_RSA   = (1 << 16), /**< peripheral type : RSA */
    WM_PM_SLEEP_DEV_TYPE_GPSEC = (1 << 17), /**< peripheral type : GPSEC */

    WM_PM_SLEEP_DEV_TYPE_SDIO_MASTER  = (1 << 18), /**< peripheral type : SDIO */
    WM_PM_SLEEP_DEV_TYPE_PSRAM        = (1 << 19), /**< peripheral type : PSRAM */
    WM_PM_SLEEP_DEV_TYPE_BT           = (1 << 20), /**< peripheral type : BT */
    WM_PM_SLEEP_DEV_TYPE_TOUCH_SENSOR = (1 << 21)  /**< peripheral type : TOUCH */
} wm_pm_sleep_dev_t;

typedef enum {
    WM_PM_WAKEUP_DEV_TYPE_SDIO           = (1 << 0),
    WM_PM_WAKEUP_DEV_TYPE_MAC            = (1 << 1),
    WM_PM_WAKEUP_DEV_TYPE_RF_CFG         = (1 << 2),
    WM_PM_WAKEUP_DEV_TYPE_SEC            = (1 << 3),
    WM_PM_WAKEUP_DEV_TYPE_DMA_Channel0   = (1 << 4),
    WM_PM_WAKEUP_DEV_TYPE_DMA_Channel1   = (1 << 5),
    WM_PM_WAKEUP_DEV_TYPE_DMA_Channel2   = (1 << 6),
    WM_PM_WAKEUP_DEV_TYPE_DMA_Channel3   = (1 << 7),
    WM_PM_WAKEUP_DEV_TYPE_DMA_Channel4_7 = (1 << 8),
    WM_PM_WAKEUP_DEV_TYPE_DMA_BRUST      = (1 << 9),
    WM_PM_WAKEUP_DEV_TYPE_I2C            = (1 << 10),
    WM_PM_WAKEUP_DEV_TYPE_ADC            = (1 << 11),
    WM_PM_WAKEUP_DEV_TYPE_SPI_LS         = (1 << 12),
    WM_PM_WAKEUP_DEV_TYPE_SPI_HS         = (1 << 13),
    WM_PM_WAKEUP_DEV_TYPE_GPIOA          = (1 << 14),
    WM_PM_WAKEUP_DEV_TYPE_GPIOB          = (1 << 15),
    WM_PM_WAKEUP_DEV_TYPE_UART0          = (1 << 16),
    WM_PM_WAKEUP_DEV_TYPE_UART1          = (1 << 17),
    WM_PM_WAKEUP_DEV_TYPE_TOUCH          = (1 << 18),
    WM_PM_WAKEUP_DEV_TYPE_UART2_5        = (1 << 19),
    WM_PM_WAKEUP_DEV_TYPE_BLE            = (1 << 20),
    WM_PM_WAKEUP_DEV_TYPE_BT             = (1 << 21),
    WM_PM_WAKEUP_DEV_TYPE_PWM            = (1 << 22),
    WM_PM_WAKEUP_DEV_TYPE_I2S            = (1 << 23),
    WM_PM_WAKEUP_DEV_TYPE_SIDO_HOST      = (1 << 24),
    WM_PM_WAKEUP_DEV_TYPE_SYS_TICK       = (1 << 25),
    WM_PM_WAKEUP_DEV_TYPE_RSA            = (1 << 26),
    WM_PM_WAKEUP_DEV_TYPE_CRYPTION       = (1 << 27),
    WM_PM_WAKEUP_DEV_TYPE_FLASH          = (1 << 28),
    WM_PM_WAKEUP_DEV_TYPE_PMU            = (1 << 29),
    WM_PM_WAKEUP_DEV_TYPE_TIMER          = (1 << 30),
    WM_PM_WAKEUP_DEV_TYPE_WDG            = (1 << 31),
} wm_pm_wakeup_dev_t;

/**
 * @}
 */

/**
 * @defgroup WM_PM_Type_Definitions WM PM Type Definitions
 * @brief WinnerMicro PM Type Definitions
 */

/**
 * @addtogroup WM_PM_Type_Definitions
 * @{
 */

/**
  * @brief Get the amount of time (in milliseconds) that has been asleep.
  *        Only valid for deep sleep mode when using wake io.
  *
  * @return Get the time already asleep (in milliseconds)
  *
  */
typedef uint32_t (*wm_pm_get_slept_callback)(void);

/**
 * @}
 */

/**
 * @defgroup WM_PM_Structures WM PM Structures
 * @brief WinnerMicro PM Structures
 */

/**
 * @addtogroup WM_PM_Structures
 * @{
 */
typedef struct {
    wm_pm_mode_t mode; /**< sleep mode */

    wm_pm_sleep_dev_t sleep_dev_bitmap; /**< used to indicate which peripheral can sleep, valid only for light sleep mode. */
    wm_pm_wakeup_dev_t
        wakeup_dev_bitmap; /**< used to indicate which device interrupts can be woken up, valid only for light sleep mode. */

    wm_pm_get_slept_callback
        get_slept; /**< gets the amount of time (in milliseconds) that has been asleep, valid only for deep sleep mode when using wake io. */
} wm_pm_config_t;

/**
 * @}
 */

/**
 * @defgroup WM_PM_Functions WM PM Functions
 * @brief WinnerMicro PM Functions
 */

/**
 * @addtogroup WM_PM_Functions
 * @{
 */

/**
 * @brief     Get PM Configuration
 *
 * @param[out] config  store configuration
 *
 * @return
 *    - WM_ERR_SUCCESS: succeed
 *    - WM_ERR_INVALID_PARAM: invalid argument
 *    - others: failed
 */
int wm_pm_get_config(wm_pm_config_t *config);

/**
 * @brief     Set PM Configuration
 *
 * @param     config       configuration
 *
 * @return
 *    - WM_ERR_SUCCESS: succeed
 *    - WM_ERR_INVALID_PARAM: invalid argument
 *    - others: failed
 */
int wm_pm_set_config(wm_pm_config_t *config);

/**
  * @brief  Obtaining a Power Management Lock.
  *         After acquiring the lock, the system will not enter sleep mode until the lock is released.
  *
  * @warning After a power lock has been acquired, it cannot be acquired repeatedly, or the program will generate a deadlock.
  *
  * @return
  *    - WM_ERR_SUCCESS: succeed
  *    - others: failed
  */
int wm_pm_lock_acquire(void);

/**
  * @brief  Releasing the Power Management Lock.
  *
  * @return
  *    - WM_ERR_SUCCESS: succeed
  *    - others: failed
  */
int wm_pm_lock_release(void);

/**
 * @brief     go to sleep
 *
 * @param     ticks       sleep duration
 *
 * @return
 *    - WM_ERR_SUCCESS: succeed
 *    - WM_ERR_INVALID_PARAM: invalid argument
 *    - others: failed
 */
int wm_pm_enter_sleep(uint32_t ticks);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* __WM_PM_H__ */
