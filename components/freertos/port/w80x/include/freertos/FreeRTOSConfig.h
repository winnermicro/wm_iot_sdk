/*
    FreeRTOS V7.0.2 - Copyright (C) 2011 Real Time Engineers Ltd.


    ***************************************************************************
     *                                                                       *
     *    FreeRTOS tutorial books are available in pdf and paperback.        *
     *    Complete, revised, and edited pdf reference manuals are also       *
     *    available.                                                         *
     *                                                                       *
     *    Purchasing FreeRTOS documentation will not only help you, by       *
     *    ensuring you get running as quickly as possible and with an        *
     *    in-depth knowledge of how to use FreeRTOS, it will also help       *
     *    the FreeRTOS project to continue with its mission of providing     *
     *    professional grade, cross platform, de facto standard solutions    *
     *    for microcontrollers - completely free of charge!                  *
     *                                                                       *
     *    >>> See http://www.FreeRTOS.org/Documentation for details. <<<     *
     *                                                                       *
     *    Thank you for using FreeRTOS, and thank you for your support!      *
     *                                                                       *
    ***************************************************************************


    This file is part of the FreeRTOS distribution.

    FreeRTOS is free software; you can redistribute it and/or modify it under
    the terms of the GNU General Public License (version 2) as published by the
    Free Software Foundation AND MODIFIED BY the FreeRTOS exception.
    >>>NOTE<<< The modification to the GPL is included to allow you to
    distribute a combined work that includes FreeRTOS without being obliged to
    provide the source code for proprietary components outside of the FreeRTOS
    kernel.  FreeRTOS is distributed in the hope that it will be useful, but
    WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
    or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
    more details. You should have received a copy of the GNU General Public
    License and the FreeRTOS license exception along with FreeRTOS; if not it
    can be viewed here: http://www.freertos.org/a00114.html and also obtained
    by writing to Richard Barry, contact details for whom are available on the
    FreeRTOS WEB site.

    1 tab == 4 spaces!

    http://www.FreeRTOS.org - Documentation, latest information, license and
    contact details.

    http://www.SafeRTOS.com - A version that is certified for use in safety
    critical systems.

    http://www.OpenRTOS.com - Commercial support, development, porting,
    licensing and training services.
*/

#ifndef FREERTOS_CONFIG_H
#define FREERTOS_CONFIG_H

#include "wmsdk_config.h"
#include <assert.h>

/*-----------------------------------------------------------
 * Application specific definitions.
 *
 * These definitions should be adjusted for your particular hardware and
 * application requirements.
 *
 * THESE PARAMETERS ARE DESCRIBED WITHIN THE 'CONFIGURATION' SECTION OF THE
 * FreeRTOS API DOCUMENTATION AVAILABLE ON THE FreeRTOS.org WEB SITE.
 *
 * See http://www.freertos.org/a00110.html.
 *----------------------------------------------------------*/

#define configUSE_PREEMPTION 1

#if CONFIG_FREERTOS_ENABLE_IDLE_HOOK
#define configUSE_IDLE_HOOK 1
#else
#define configUSE_IDLE_HOOK 0
#endif

#if CONFIG_FREERTOS_USE_TICKLESS_IDLE
#define configUSE_TICKLESS_IDLE               1
#define configEXPECTED_IDLE_TIME_BEFORE_SLEEP CONFIG_FREERTOS_IDLE_TIME_BEFORE_SLEEP
void vApplicationSleep(unsigned int xExpectedIdleTime);
#define portSUPPRESS_TICKS_AND_SLEEP(xExpectedIdleTime) vApplicationSleep(xExpectedIdleTime)
#else
#define configUSE_TICKLESS_IDLE               0
#define configEXPECTED_IDLE_TIME_BEFORE_SLEEP 2
#define portSUPPRESS_TICKS_AND_SLEEP(xExpectedIdleTime)
#endif

#if CONFIG_FREERTOS_ENABLE_TICK_HOOK
#define configUSE_TICK_HOOK 1
#else
#define configUSE_TICK_HOOK 0
#endif

#define configCPU_CLOCK_HZ       ((unsigned long)40000000) /* =12.0MHz xtal multiplied by 5 using the PLL. */ //???????????????

#define configTICK_RATE_HZ       ((portTickType)CONFIG_FREERTOS_HZ)

#define configMAX_PRIORITIES     (16)

#define configMINIMAL_STACK_SIZE ((unsigned short)CONFIG_FREERTOS_IDLE_TASK_STACKSIZE)

#define configMAX_TASK_NAME_LEN  (CONFIG_FREERTOS_MAX_TASK_NAME_LEN)

#if CONFIG_FREERTOS_ENABLE_RUN_TIME_STATS
#define configGENERATE_RUN_TIME_STATS (1)
#if configGENERATE_RUN_TIME_STATS == 1
extern unsigned long vGetRunTimeCounterValue(void);
#define portCONFIGURE_TIMER_FOR_RUN_TIME_STATS() \
    {                                            \
    }
#define portGET_RUN_TIME_COUNTER_VALUE() vGetRunTimeCounterValue()
#endif
#endif

#if CONFIG_FREERTOS_USE_TRACE_FACILITY
#define configUSE_TRACE_FACILITY 1
#else
#define configUSE_TRACE_FACILITY 0
#endif

#define configUSE_16_BIT_TICKS          0
#define configIDLE_SHOULD_YIELD         1

#define configQUEUE_REGISTRY_SIZE       (CONFIG_FREERTOS_QUEUE_REGISTRY_SIZE)

#define configSEMAPHORE_MAX_VALUE       25

/* Co-routine definitions. */
#define configUSE_CO_ROUTINES           0
#define configMAX_CO_ROUTINE_PRIORITIES (2)

/* Set the following definitions to 1 to include the API function, or zero
to exclude the API function. */

#define INCLUDE_vTaskPrioritySet        1
#define INCLUDE_uxTaskPriorityGet       1
#define INCLUDE_vTaskDelete             1
#define INCLUDE_vTaskCleanUpResources   0
#define INCLUDE_vTaskSuspend            1
#define INCLUDE_vTaskDelayUntil         1
#define INCLUDE_vTaskDelay              1

#define configUSE_COUNTING_SEMAPHORES   1
#define configSUPPORT_STATIC_ALLOCATION 1

#define configTIMER_TASK_PRIORITY       (CONFIG_FREERTOS_TIMER_TASK_PRIORITY)
#define configTIMER_QUEUE_LENGTH        (CONFIG_FREERTOS_TIMER_QUEUE_LENGTH)
#define configTIMER_TASK_STACK_DEPTH    (CONFIG_FREERTOS_TIMER_TASK_STACK_DEPTH)

#define configUSE_TIMERS                1
#define configUSE_MUTEXES               (1)

#define INCLUDE_xTimerPendFunctionCall  1

#if CONFIG_FREERTOS_USE_STATS_FORMATTING_FUNCTIONS
#define configUSE_STATS_FORMATTING_FUNCTIONS (1)
#else
#define configUSE_STATS_FORMATTING_FUNCTIONS (0)
#endif

#define INCLUDE_xTaskGetCurrentTaskHandle       (1)

#define configUSE_APPLICATION_TASK_TAG          1
#define INCLUDE_xQueueGetMutexHolder            1

#define configUSE_PORT_OPTIMISED_TASK_SELECTION 0

#define configUSE_RECURSIVE_MUTEXES             1

#if CONFIG_FREERTOS_ENABLE_STATIC_TASK_CLEAN_UP
extern void vPortCleanUpTCB(void *pxTCB);
#define portCLEAN_UP_TCB(pxTCB) vPortCleanUpTCB(pxTCB)
#endif

#if !CONFIG_FREERTOS_ASSERT_DISABLE
extern int wm_printf_direct(const char *fmt, ...) __attribute__((format(printf, 1, 2)));
#if CONFIG_FREERTOS_ASSERT_FAIL_ABORT
#define configASSERT(a) assert(a)
#elif CONFIG_FREERTOS_ASSERT_FAIL_PRINT_CONTINUE
#define configASSERT(a)                                                 \
    do {                                                                \
        if ((a) == 0) {                                                 \
            wm_printf_direct("Assert : %s %d\r\n", __FILE__, __LINE__); \
        }                                                               \
    } while (0)
#endif
#endif

#if CONFIG_FREERTOS_CHECK_STACKOVERFLOW
#define configCHECK_FOR_STACK_OVERFLOW 1
#else
#define configCHECK_FOR_STACK_OVERFLOW 0
#endif

#if CONFIG_COMPONENT_POSIX_ENABLED
#define configUSE_POSIX_ERRNO 1
#else
#define configUSE_POSIX_ERRNO 0
#endif

#endif /* FREERTOS_CONFIG_H */
