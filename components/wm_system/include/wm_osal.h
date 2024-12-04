/**
 * @file wm_osal.c
 *
 * @brief WM OS abstraction layer(for internal use only)
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
#ifndef __WM_OSAL_H__
#define __WM_OSAL_H__

#include <stdio.h>
#include <stdlib.h>
#include <core_804.h>
#include "wmsdk_config.h"
#include "wm_types.h"
#include "wm_error.h"
#include "wm_heap.h"
#include "wm_utils.h"

#ifdef __cplusplus
extern "C" {
#endif

/** OS TICK Frequency, this value needs to keep the same as OS self-defined tick value*/
extern const uint32_t HZ;

/** ENUMERATION of OS */
enum WM_OS_TYPE {
    WM_OS_UCOSII            = 0,
    wm_os_internal_freeRTOS = 1,
    WM_OS_MAX_NUM
};

/** TYPE definition of WM_OS_STK */
#ifdef WM_OS_STK
#undef WM_OS_STK
#endif
typedef uint32_t WM_OS_STK;

/** TYPE definition of wm_os_task_t */
typedef void *wm_os_task_t;

/** TYPE definition of wm_os_timer_t */
typedef void wm_os_timer_t;

/** TYPE definition of wm_os_sem_t */
typedef void wm_os_sem_t;

/** TYPE definition of wm_os_queue_t */
typedef void wm_os_queue_t;

/** TYPE definition of wm_os_mailbox_t */
typedef void wm_os_mailbox_t;

/** TYPE definition of wm_os_mutex_t */
typedef void wm_os_mutex_t;

/** TYPE definition of wm_os_event_t */
typedef void wm_os_event_t;

/** TYPE definition of wm_os_timer_callback */
typedef void (*wm_os_timer_callback)(wm_os_timer_t *ptmr, void *parg);

/** ENUMERATION definition of OS STATUS */
typedef enum wm_os_status {
    WM_OS_STATUS_SUCCESS = WM_ERR_SUCCESS,
    WM_OS_STATUS_ERROR   = WM_ERR_FAILED,
    WM_OS_STATUS_TIMEOUT = WM_ERR_TIMEOUT,
} wm_os_status_t;

/** ENUMERATION of OS scheduler state */
typedef enum WM_OS_SCHED_STATE {
    WM_OS_SCHED_SUSPEND   = 0,
    WM_OS_SCHED_NOT_START = 1,
    WM_OS_SCHED_RUNNING   = 2,
} wm_os_sched_state_t;

/** Max waiting time */
#define WM_OS_WAIT_TIME_MAX 0xffffffffUL

void wm_os_internal_time_tick(void *p);
void wm_os_internal_init(void *arg);
void wm_os_internal_start_scheduler(void);
int wm_os_internal_get_type(void);
wm_os_status_t wm_os_internal_task_create(wm_os_task_t *task, const char *name, void (*entry)(void *param), void *param,
                                          uint32_t stk_size, uint32_t prio, uint32_t flag);
wm_os_status_t wm_os_internal_task_del(wm_os_task_t *task);
wm_os_status_t wm_os_internal_task_del_ex(void *handle, void (*freefun)(void *arg), void *arg);
wm_os_task_t wm_os_internal_task_id(void);
const char *wm_os_internal_get_task_name(void);
void wm_os_internal_set_task_name(wm_os_task_t *task, const char *name);
wm_os_sched_state_t wm_os_internal_task_schedule_state(void);
wm_os_status_t wm_os_internal_task_resume(wm_os_task_t *task);
void wm_os_internal_task_yield(void);
wm_os_status_t wm_os_internal_task_suspend(wm_os_task_t *task);
void wm_os_internal_task_schedule_lock(void);
void wm_os_internal_task_schedule_unlock(void);
uint32_t wm_os_internal_task_get_max_priority(void);
wm_os_status_t wm_os_internal_recursive_mutex_create(wm_os_mutex_t **mutex);
wm_os_status_t wm_os_internal_recursive_mutex_delete(wm_os_mutex_t *mutex);
wm_os_status_t wm_os_internal_recursive_mutex_acquire(wm_os_mutex_t *mutex, uint32_t wait_time);
wm_os_status_t wm_os_internal_recursive_mutex_release(wm_os_mutex_t *mutex);
wm_os_status_t wm_os_internal_mutex_create(wm_os_mutex_t **mutex);
wm_os_status_t wm_os_internal_mutex_delete(wm_os_mutex_t *mutex);
wm_os_status_t wm_os_internal_mutex_acquire(wm_os_mutex_t *mutex, uint32_t wait_time);
wm_os_status_t wm_os_internal_mutex_release(wm_os_mutex_t *mutex);
wm_os_status_t wm_os_internal_sem_create(wm_os_sem_t **sem, uint32_t cnt);
wm_os_status_t wm_os_internal_sem_reset(wm_os_sem_t *sem);
wm_os_status_t wm_os_internal_sem_delete(wm_os_sem_t *sem);
wm_os_status_t wm_os_internal_sem_acquire(wm_os_sem_t *sem, uint32_t wait_time);
wm_os_status_t wm_os_internal_sem_acquire_ms(wm_os_sem_t *sem, uint32_t wait_time_ms);
wm_os_status_t wm_os_internal_sem_release(wm_os_sem_t *sem);
uint16_t wm_os_internal_sem_get_count(wm_os_sem_t *sem);
wm_os_status_t wm_os_internal_queue_create(wm_os_queue_t **queue, uint32_t queue_size);
wm_os_status_t wm_os_internal_queue_create_ex(wm_os_queue_t **queue, uint32_t item_size, uint32_t queue_size);
wm_os_status_t wm_os_internal_queue_delete(wm_os_queue_t *queue);
wm_os_status_t wm_os_internal_queue_send(wm_os_queue_t *queue, void *msg);
wm_os_status_t wm_os_internal_queue_forever_send(wm_os_queue_t *queue, void *msg);
wm_os_status_t wm_os_internal_queue_send_ex(wm_os_queue_t *queue, void *msg);
wm_os_status_t wm_os_internal_queue_send_to_front(wm_os_queue_t *queue, void *msg);
wm_os_status_t wm_os_internal_queue_send_to_back(wm_os_queue_t *queue, void *msg);
wm_os_status_t wm_os_internal_queue_flush(wm_os_queue_t *queue);
wm_os_status_t wm_os_internal_queue_receive(wm_os_queue_t *queue, void **msg, uint32_t wait_time);
wm_os_status_t wm_os_internal_queue_receive_ms(wm_os_queue_t *queue, void **msg, uint32_t wait_time_ms);
uint8_t wm_os_internal_queue_is_empty(wm_os_queue_t *queue);
uint32_t wm_os_internal_queue_space_available(wm_os_queue_t *queue);
wm_os_status_t wm_os_internal_queue_remove(wm_os_queue_t *queue, void *msg);
wm_os_status_t wm_os_internal_queue_peek(wm_os_queue_t *queue, void **msg, uint32_t wait_time);
wm_os_status_t wm_os_internal_mailbox_create(wm_os_mailbox_t **mailbox, uint32_t mailbox_size);
wm_os_status_t wm_os_internal_mailbox_delete(wm_os_mailbox_t *mailbox);
wm_os_status_t wm_os_internal_mailbox_send(wm_os_mailbox_t *mailbox, void *msg);
wm_os_status_t wm_os_internal_mailbox_receive(wm_os_mailbox_t *mailbox, void **msg, uint32_t wait_time);
wm_os_status_t wm_os_internal_event_create(wm_os_event_t **event);
wm_os_status_t wm_os_internal_event_delete(wm_os_event_t *event);
wm_os_status_t wm_os_internal_event_get(wm_os_event_t *event, uint32_t wait_event, uint32_t *return_event, bool equal,
                                        uint32_t wait_time);
wm_os_status_t wm_os_internal_event_put(wm_os_event_t *event, uint32_t event_bits);
wm_os_status_t wm_os_internal_event_clear_bits(wm_os_event_t *event, uint32_t event_bits);
wm_os_status_t wm_os_internal_event_get_bits(wm_os_event_t *event, uint32_t *event_bits);
uint32_t wm_os_internal_get_time(void);
uint32_t wm_os_internal_get_time_ms(void);
void wm_os_internal_set_critical(void);
void wm_os_internal_release_critical(void);
wm_os_status_t wm_os_internal_timer_create(wm_os_timer_t **timer, wm_os_timer_callback callback, void *callback_arg,
                                           uint32_t period, bool repeat, char *name);
wm_os_status_t wm_os_internal_timer_create_ms(wm_os_timer_t **timer, wm_os_timer_callback callback, void *callback_arg,
                                              uint32_t period_ms, bool repeat, char *name);
void wm_os_internal_timer_start(wm_os_timer_t *timer);
void wm_os_internal_timer_change(wm_os_timer_t *timer, uint32_t ticks);
void wm_os_internal_timer_change_ms(wm_os_timer_t *timer, uint32_t ms);
void wm_os_internal_timer_stop(wm_os_timer_t *timer);
void wm_os_internal_timer_reset(wm_os_timer_t *timer);
wm_os_status_t wm_os_internal_timer_delete(wm_os_timer_t *timer);
void wm_os_internal_time_delay(uint32_t ticks);
void wm_os_internal_time_delay_ms(uint32_t ms);
uint8_t wm_os_internal_timer_active(wm_os_timer_t *timer);
uint32_t wm_os_internal_timer_expirytime(wm_os_timer_t *timer);
void wm_os_internal_disp_task_stat_info(void);
void wm_os_internal_disp_sys_runtime_stats_info(void);
uint8_t wm_os_internal_get_isr_count(void);

#ifdef CONFIG_HEAP_USE_TRACING
#define wm_os_internal_malloc_tracing(size, file, line) wm_heap_caps_alloc_tracing(size, WM_HEAP_CAP_DEFAULT, file, line)
#define wm_os_internal_realloc_tracing(ptr, size, file, line) \
    wm_heap_caps_realloc_tracing(ptr, size, WM_HEAP_CAP_DEFAULT, file, line)
void *wm_os_internal_calloc_tracing(size_t nelem, size_t elsize, const char *file, int line);
#define wm_os_internal_malloc(size)          wm_os_internal_malloc_tracing(size, __FILE__, __LINE__)
#define wm_os_internal_realloc(ptr, size)    wm_os_internal_realloc_tracing(ptr, size, __FILE__, __LINE__)
#define wm_os_internal_calloc(nelem, elsize) wm_os_internal_calloc_tracing(nelem, elsize, __FILE__, __LINE__)
#else //CONFIG_HEAP_USE_TRACING
#define wm_os_internal_malloc(size)       wm_heap_caps_alloc(size, WM_HEAP_CAP_DEFAULT)
#define wm_os_internal_realloc(ptr, size) wm_heap_caps_realloc(ptr, size, WM_HEAP_CAP_DEFAULT)
void *wm_os_internal_calloc(size_t nelem, size_t elsize);
#endif //CONFIG_HEAP_USE_TRACING

#define wm_os_internal_free(ptr) wm_heap_caps_free(ptr)

#ifdef __cplusplus
}
#endif

#endif /* end of __WM_OSAL_H__ */
