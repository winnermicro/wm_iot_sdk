/**
 * @file wm_drv_irq.c
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
#include "wm_types.h"
#include "core_804.h"
#include "csi_gcc.h"
#include "wm_osal.h"
#include "wm_drv_irq.h"

#define WM_DRV_IRQ_LOCK()   //wm_os_internal_set_critical()
#define WM_DRV_IRQ_UNLOCK() //wm_os_internal_release_critical()

int wm_drv_irq_attach_hw_vector(wm_irq_no_t irq, wm_drv_hw_irq_handle handle)
{
    int32_t err = WM_ERR_SUCCESS;

    WM_DRV_IRQ_LOCK();
    err = wm_hal_irq_attach_hw_vector(irq, handle);
    WM_DRV_IRQ_UNLOCK();
    return err;
}

int wm_drv_irq_detach_hw_vector(wm_irq_no_t irq)
{
    int32_t err = WM_ERR_SUCCESS;

    WM_DRV_IRQ_LOCK();
    err = wm_hal_irq_detach_hw_vector(irq);
    WM_DRV_IRQ_UNLOCK();
    return err;
}

int wm_drv_irq_attach_sw_vector(wm_irq_no_t irq, wm_drv_sw_irq_handle handle, void *arg)
{
    int32_t err = WM_ERR_SUCCESS;

    WM_DRV_IRQ_LOCK();
    err = wm_hal_irq_attach_sw_vector(irq, handle, arg);
    WM_DRV_IRQ_UNLOCK();
    return err;
}

int wm_drv_irq_detach_sw_vector(wm_irq_no_t irq)
{
    int32_t err = WM_ERR_SUCCESS;

    WM_DRV_IRQ_LOCK();
    err = wm_hal_irq_detach_sw_vector(irq);
    WM_DRV_IRQ_UNLOCK();
    return err;
}

uint32_t wm_drv_irq_save(void)
{
    return wm_hal_irq_save();
}

void wm_drv_irq_restore(uint32_t irq_state)
{
    wm_hal_irq_restore(irq_state);
}

int wm_drv_irq_enable(wm_irq_no_t irq)
{
    int32_t err = WM_ERR_SUCCESS;

    WM_DRV_IRQ_LOCK();
    err = wm_hal_irq_enable(irq);
    WM_DRV_IRQ_UNLOCK();

    return err;
}

int wm_drv_irq_disable(wm_irq_no_t irq)
{
    int32_t err = WM_ERR_SUCCESS;

    WM_DRV_IRQ_LOCK();
    err = wm_hal_irq_disable(irq);
    WM_DRV_IRQ_UNLOCK();

    return err;
}

int wm_drv_irq_set_pending(wm_irq_no_t irq)
{
    int32_t err = WM_ERR_SUCCESS;

    err = wm_hal_irq_set_pending(irq);
    return err;
}

int wm_drv_irq_clear_pending(wm_irq_no_t irq)
{
    int32_t err = WM_ERR_SUCCESS;

    err = wm_hal_irq_clear_pending(irq);
    return err;
}

int wm_drv_irq_set_wakeup(wm_irq_no_t irq)
{
    int32_t err = WM_ERR_SUCCESS;

    err = wm_hal_irq_set_wakeup(irq);
    return err;
}

int wm_drv_irq_clear_wakeup(wm_irq_no_t irq)
{
    int32_t err = WM_ERR_SUCCESS;

    err = wm_hal_irq_clear_wakeup(irq);
    return err;
}