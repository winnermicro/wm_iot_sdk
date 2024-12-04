/**
 * @file wm_hal_irq.h
 *
 * @brief irq Module
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
#ifndef __WM_HAL_IRQ_H__
#define __WM_HAL_IRQ_H__

#include "wm_types.h"
#include "wm_irq.h"

#ifdef __cplusplus
extern "C" {
#endif

#define WM_IRQ_HW_VECTOR_DECLARE(handler) ATTRIBUTE_ISR void handler(void)
#define WM_IRQ_SW_VECTOR_DECLARE(handler) void handler

typedef void (*wm_hal_sw_irq_handle)(wm_irq_no_t irq, void *arg);
typedef void (*wm_hal_hw_irq_handle)(void);

/**
 * @brief IRQ configuration struct
 *
 * @param handler      Address of the interrupt handler
 * @param arg          The argument provided to the interrupt handler
 */
typedef struct {
    wm_hal_sw_irq_handle handler;
    void *arg;
} wm_hal_irq_info_t;

/**
 * @brief Interrupt initialize.
 *
 */
void wm_hal_irq_init(void);

/**
 * @brief Attach hw interrupt vector with callback and replace sw vector, If you have registered before, registering again will invalidate the previous registration
 *        For the registered functions, please add the WM-IRQ-HW-ISR-ATTRIBUTE attribute and add csi_kernel_intrpt_enter at the beginning and csi_kernel_intrpt_exit at the end.
 *
 * @param [in] irq irq number
 * @param [in] handle interrupt callback
 *
 * @return
 *    - WM_ERR_SUCCESS: succeed
 *    - others: failed
 * @note   If hw vector attached, sw vector will be invalidated
 */
int wm_hal_irq_attach_hw_vector(wm_irq_no_t irq, wm_hal_hw_irq_handle handle);

/**
 * @brief Detach hw interrupt vector.
 *
 * @param [in] irq irq number
 *
 * @return
 *    - WM_ERR_SUCCESS: succeed
 *    - others: failed
 */
int wm_hal_irq_detach_hw_vector(wm_irq_no_t irq);

/**
 * @brief Attach sw interrupt with callback, If you have registered before, registering again will invalidate the previous registration.
 *
 * @param [in] irq irq number
 * @param [in] handle interrupt callback
 * @param [in] priv user data
 *
 * @return
 *    - WM_ERR_SUCCESS: succeed
 *    - others: failed
 */
int wm_hal_irq_attach_sw_vector(wm_irq_no_t irq, wm_hal_sw_irq_handle handle, void *priv);

/**
 * @brief Detach sw interrupt vector.
 *
 * @param [in] irq irq number
 *
 * @return
 *    - WM_ERR_SUCCESS: succeed
 *    - others: failed
 */
int wm_hal_irq_detach_sw_vector(wm_irq_no_t irq);

/**
 * @brief Disable global irq and save the previous status.
 *
 * @return last status
 */
uint32_t wm_hal_irq_save(void);

/**
 * @brief Enable global irq by the previous status.
 *
 * @param [in] irq_state previous status by wm_hal_irq_save
 */
void wm_hal_irq_restore(uint32_t irq_state);

/**
 * @brief Enable interrupt.
 *
 * @param [in] irq irq number
 *
 * @return
 *    - WM_ERR_SUCCESS: succeed
 *    - others: failed
 */
int wm_hal_irq_enable(wm_irq_no_t irq);

/**
 * @brief Disable interrupt.
 *
 * @param [in] irq irq number
 *
 * @return
 *    - WM_ERR_SUCCESS: succeed
 *    - others: failed
 */
int wm_hal_irq_disable(wm_irq_no_t irq);

/**
 * @brief Set interrupt with pending.
 *
 * @param [in] irq irq number
 *
 * @return
 *    - WM_ERR_SUCCESS: succeed
 *    - others: failed
 */
int wm_hal_irq_set_pending(wm_irq_no_t irq);

/**
 * @brief Clear interrupt pending status.
 *
 * @param [in] irq irq number
 *
 * @return
 *    - WM_ERR_SUCCESS: succeed
 *    - others: failed
 */
int wm_hal_irq_clear_pending(wm_irq_no_t irq);

/**
 * @brief Set wakeup interrupt.
 *
 * @param [in] irq irq number
 *
 * @return
 *    - WM_ERR_SUCCESS: succeed
 *    - others: failed
 */
int wm_hal_irq_set_wakeup(wm_irq_no_t irq);

/**
 * @brief Set critical, like wm_os_internal_set_critical.
 */
void wm_hal_irq_set_critical();

/**
 * @brief release critical, like wm_os_internal_release_critical.
 */
void wm_hal_irq_release_critical();

#ifdef __cplusplus
}
#endif

#endif /* __WM_HAL_IRQ_H__ */
