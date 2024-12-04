/**
 * @file wm_hal_irq.c
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

#include <assert.h>
#include "wm_types.h"
#include "wm_error.h"
#include "core_804.h"
#include "csi_gcc.h"
#include "wm_hal_irq.h"

#define LOG_TAG "hal_irq"
#include "wm_log.h"

#define WM_HAL_IRQ_DEBUG 1

#if WM_HAL_IRQ_DEBUG
/* Use wm_printf_direct because some log in critial area */
#define WM_HAL_IRQ_PRINTF(...) wm_printf_direct(__VA_ARGS__);
#else
#define WM_HAL_IRQ_PRINTF(...)
#endif

extern wm_hal_irq_info_t g_sw_irq_vector[WM_IRQ_MAX];
extern void irq_interrupt_entry(void);

/* Each caller maintains its own interrupt status in the critical nesting
variable. */
#define WM_IRQ_RECURSIVE_DEFAULT_NUM 0xaaaaaaaa
static volatile unsigned long g_CriticalNesting = WM_IRQ_RECURSIVE_DEFAULT_NUM;

static void wm_hal_irq_unexpected_handler(wm_irq_no_t irq, void *arg)
{
    WM_HAL_IRQ_PRINTF("irq :%d unregistered\r\n", irq);
}

void wm_hal_irq_init(void)
{
    uint8_t i;

    /* Point all interrupt vectors to the unexpected interrupt */
    for (i = 0; i < WM_IRQ_MAX; i++) {
        g_sw_irq_vector[i].handler = wm_hal_irq_unexpected_handler;
        g_sw_irq_vector[i].arg     = NULL;
    }
}

int wm_hal_irq_attach_hw_vector(wm_irq_no_t irq, wm_hal_hw_irq_handle handle)
{
    if (irq >= WM_IRQ_MAX) {
        return WM_ERR_INVALID_PARAM;
    }

    wm_hal_irq_set_critical();
    if (g_sw_irq_vector[irq].handler != wm_hal_irq_unexpected_handler) {
        WM_HAL_IRQ_PRINTF("software vector already be attached, hw attach will make it invalid ");
    }
    csi_vic_set_vector(irq, (uint32_t)handle);
    wm_hal_irq_release_critical();

    return WM_ERR_SUCCESS;
}

int wm_hal_irq_detach_hw_vector(wm_irq_no_t irq)
{
    if (irq >= WM_IRQ_MAX) {
        return WM_ERR_INVALID_PARAM;
    }

    wm_hal_irq_set_critical();
    csi_vic_set_vector(irq, (uint32_t)irq_interrupt_entry);
    wm_hal_irq_release_critical();

    return WM_ERR_SUCCESS;
}

int wm_hal_irq_attach_sw_vector(wm_irq_no_t irq, wm_hal_sw_irq_handle handle, void *priv)
{
    if (irq >= WM_IRQ_MAX) {
        return WM_ERR_INVALID_PARAM;
    }

    /* check hw vector */
    if (irq_vectors[WM_IRQ_SYS_INTR_NUM + irq] != (uint32_t)irq_interrupt_entry) {
        WM_HAL_IRQ_PRINTF("hw vector already been attached, sw attach will be ignored");
        return WM_ERR_NOT_ALLOWED;
    }

    /* attach sw vector */
    g_sw_irq_vector[irq].handler = handle;
    g_sw_irq_vector[irq].arg     = priv;
    return WM_ERR_SUCCESS;
}

int wm_hal_irq_detach_sw_vector(wm_irq_no_t irq)
{
    if (irq >= WM_IRQ_MAX) {
        return WM_ERR_INVALID_PARAM;
    }

    /* check hw vector */
    if (irq_vectors[WM_IRQ_SYS_INTR_NUM + irq] != (uint32_t)irq_interrupt_entry) {
        WM_HAL_IRQ_PRINTF("hw vector already been attached, sw detach will be ignored");
        return WM_ERR_NOT_ALLOWED;
    }

    g_sw_irq_vector[irq].handler = wm_hal_irq_unexpected_handler;
    g_sw_irq_vector[irq].arg     = NULL;
    return WM_ERR_SUCCESS;
}

uint32_t wm_hal_irq_save(void)
{
    uint32_t ret = WM_ERR_SUCCESS;

    wm_hal_irq_set_critical();
    ret = csi_irq_save();
    wm_hal_irq_release_critical();

    return ret;
}

void wm_hal_irq_restore(uint32_t irq_state)
{
    wm_hal_irq_set_critical();
    csi_irq_restore(irq_state);
    wm_hal_irq_release_critical();
}

int wm_hal_irq_enable(wm_irq_no_t irq)
{
    if (irq >= WM_IRQ_MAX) {
        return WM_ERR_INVALID_PARAM;
    }

    wm_hal_irq_set_critical();
    NVIC_ClearPendingIRQ(irq);
    NVIC_EnableIRQ(irq);
    NVIC_SetWakeupIRQ(irq);
    wm_hal_irq_release_critical();

    return WM_ERR_SUCCESS;
}

int wm_hal_irq_disable(wm_irq_no_t irq)
{
    if (irq >= WM_IRQ_MAX) {
        return WM_ERR_INVALID_PARAM;
    }

    wm_hal_irq_set_critical();
    NVIC_DisableIRQ(irq);
    NVIC_ClearWakeupIRQ(irq);
    wm_hal_irq_release_critical();

    return WM_ERR_SUCCESS;
}

int wm_hal_irq_set_pending(wm_irq_no_t irq)
{
    if (irq >= WM_IRQ_MAX) {
        return WM_ERR_INVALID_PARAM;
    }

    wm_hal_irq_set_critical();
    NVIC_SetPendingIRQ(irq);
    wm_hal_irq_release_critical();

    return WM_ERR_SUCCESS;
}

int wm_hal_irq_clear_pending(wm_irq_no_t irq)
{
    if (irq >= WM_IRQ_MAX) {
        return WM_ERR_INVALID_PARAM;
    }

    wm_hal_irq_set_critical();
    NVIC_ClearPendingIRQ(irq);
    wm_hal_irq_release_critical();

    return WM_ERR_SUCCESS;
}

int wm_hal_irq_set_wakeup(wm_irq_no_t irq)
{
    if (irq >= WM_IRQ_MAX) {
        return WM_ERR_INVALID_PARAM;
    }

    wm_hal_irq_set_critical();
    NVIC_SetWakeupIRQ(irq);
    wm_hal_irq_release_critical();

    return WM_ERR_SUCCESS;
}

void wm_hal_irq_set_critical()
{
    __disable_irq();
    g_CriticalNesting++;
}

void wm_hal_irq_release_critical()
{
    if (g_CriticalNesting == 0) {
        assert(0);
    }

    if (g_CriticalNesting)
        g_CriticalNesting--;

    if (g_CriticalNesting == WM_IRQ_RECURSIVE_DEFAULT_NUM) {
        __enable_irq();
    }
}
