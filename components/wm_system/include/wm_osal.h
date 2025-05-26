/**
 * @file    wm_osal.h
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

/**
 * @defgroup WM_OSAL_APIs WM OS AL APIs
 * @brief WinnerMicro OS AL APIs
 */

/**
 * @addtogroup WM_OSAL_APIs
 * @{
 */

/**
 * @brief          This function is used to tick handler
 *
 * @param[in]      *p    argument
 *
 * @return         None
 *
 * @note           None
 */
void wm_os_internal_time_tick(void *p);

/**
 * @brief          This function is used to initialize OS common resource
 *
 * @param[in]      *arg
 *
 * @return         None
 *
 * @note           None
 */
void wm_os_internal_init(void *arg);

/**
 * @brief          This function is used to start task scheduler
 *
 * @return         None
 *
 * @note           None
 */
void wm_os_internal_start_scheduler(void);

/**
 * @brief          This function is used to get OS type
 *
 * @retval         0     WM_OS_UCOSII
 * @retval         1     WM_OS_FREERTOS
 *
 * @note           May not be used by now.
 */
int wm_os_internal_get_type(void);

/**
 * @brief          This function is used to create a task. Tasks can either be created prior to
                   the start of multitasking or by a running task.
                   A task cannot be created in an ISR.
 *
 * @param[in]      *task      pointer to the task
 * @param[in]      name       the task's name
 * @param[in]      entry      the task's entry function
 * @param[in]      param      pointer to an optional data area which can be
                              used to pass parameters to the task when the
                              task first executes.  Where the task is
                              concerned it thinks it was invoked and passed
                              the argument 'param' as follows:
                              void Task (void *param)
                              {
                                  for (;;) {
                                      Task code;
                                  }
                              }
 * @param[in]      stk_size   the size of the stack in number of elements.
                              If WM_OS_STK is set to INT8U,
                              'stk_size' corresponds to the number of bytes
                              available.
                              If WM_OS_STK is set to INT16U, 'stk_size' contains
                              the number of 16-bit entries available.
                              Finally, if WM_OS_STK is set to INT32U,
                              'stk_size' contains the number of 32-bit entries
                              available on the stack.
 * @param[in]      prio       the task's priority.  A unique priority MUST be
                              assigned to each task and the higher the number,
                              the higher the priority.
 * @param[in]      flag       contains additional information about the behavior of the task
 *
 * @retval         WM_OS_STATUS_SUCCESS     the call was successful.
 * @retval         WM_OS_STATUS_ERROR       failed
 */
wm_os_status_t wm_os_internal_task_create(wm_os_task_t *task, const char *name, void (*entry)(void *param), void *param,
                                          uint32_t stk_size, uint32_t prio, uint32_t flag);

/**
 * @brief          This function allows you to delete a task.  The calling
                   task can delete itself by its own task handle.
                   The deleted task is returned to the dormant state
                   and can be re-activated by creating the deleted task
                   again.
 *
 * @param[in]      *task              pointer to the task to be deleted
 *
 * @retval         WM_OS_STATUS_SUCCESS     the call is successful
 * @retval         WM_OS_STATUS_ERROR       failed
 *
 * @note           Generally, you do not need to call this function in your application.
 */
wm_os_status_t wm_os_internal_task_del(wm_os_task_t *task);

/**
 * @brief          This function allows you to delete a task.  The calling
                   task can delete itself by taks's handler.
                   The deleted task is returned to the dormant state
                   and can be re-activated by creating the deleted task
                   again.
 *
 * @param[in]      handle                task handler to delete
 * @param[in]      freefun               function to free resource
 * @param[in]      arg                   function custom variables
 *
 * @retval         WM_OS_STATUS_SUCCESS     the call is successful
 * @retval         WM_OS_STATUS_ERROR       failed
 *
 */
wm_os_status_t wm_os_internal_task_del_ex(void *handle, void (*freefun)(void *arg), void *arg);

/**
 * @brief          This function can get the current task handle
 *
 * @retval         pxCurrentTCB          get current task handle
 *
 */
wm_os_task_t wm_os_internal_task_id(void);

/**
 * @brief          This function can get the task name of a task
 *
 * @retval         pcTaskName         get the task name
 * @retval         "Task"             not get the task name
 *
 */
const char *wm_os_internal_get_task_name(void);

/**
 * @brief          This function is called to change task name
 *
 * @param[in]      *task               task handler to this task
 * @param[in]      *name               task name
 *
 * @retval         0           always success
 *
 */
void wm_os_internal_set_task_name(wm_os_task_t *task, const char *name);

/**
 * @brief          This function gets the current status of the scheduler
 *
 * @retval         0           taskSCHEDULER_SUSPENDED
 * @retval         1           taskSCHEDULER_NOT_STARTED
 * @retval         2           taskSCHEDULER_RUNNING
 *
 */
wm_os_sched_state_t wm_os_internal_task_schedule_state(void);

/**
 * @brief          This function resume a suspened task
 *
 * @param[in]      *task               task handler to resume
 *
 * @retval         0           always success
 *
 */
wm_os_status_t wm_os_internal_task_resume(wm_os_task_t *task);

/**
 * @brief          This function is called to force to context switch
 *
 * @retval         void
 *
 */
void wm_os_internal_task_yield(void);

/**
 * @brief          This function is called to suspend task
 *
 * @param[in]      *task               task handler to suspend
 *
 * @retval         0           always success
 *
 */
wm_os_status_t wm_os_internal_task_suspend(wm_os_task_t *task);

/**
 * @brief          This function is called to suspend task scheduling
 *
 * @retval         void
 *
 */
void wm_os_internal_task_schedule_lock(void);

/**
 * @brief          This function is called to resume task scheduling
 *
 * @retval         void
 *
 */
void wm_os_internal_task_schedule_unlock(void);

/**
 * @brief          This function is called to get the maximum priority of tasks.
 *
 * @retval         uint32_t  The maximum priority value of tasks.
 */
uint32_t wm_os_internal_task_get_max_priority(void);

/**
 * @brief          This function creates a mutual recursive semaphore
 *
 * @param[in]      **mutex   pointer to the event control clock (OS_EVENT)
                             associated with the created mutex.
 *
 * @retval         WM_OS_STATUS_SUCCESS     the call was successful
 * @retval         WM_OS_STATUS_ERROR       failed
 *
 * @note           1) The LEAST significant 8 bits of '.OSEventCnt' are used
                      to hold the priority number of the task owning the mutex
                      or 0xFF if no task owns the mutex.
                   2) The MOST  significant 8 bits of '.OSEventCnt' are used to
                      hold the priority number to use to reduce priority
                      inversion.
 */
wm_os_status_t wm_os_internal_recursive_mutex_create(wm_os_mutex_t **mutex);

/**
 * @brief          This function deletes a mutual recursive semaphore and
                   readies all tasks pending on the it
 *
 * @param[in]      *mutex    pointer to the event control block associated
                             with the desired mutex
 *
 * @retval         WM_OS_STATUS_SUCCESS     The call was successful and the mutex
                                      was deleted
 * @retval         WM_OS_STATUS_ERROR       failed
 *
 * @note           1) This function must be used with care.  Tasks that would
                      normally expect the presence of the mutex MUST check the
                      return code of OSMutexPend().
                   2) This call can potentially disable interrupts for a long
                      time.  The interrupt disable time is directly
                      proportional to the number of tasks waiting on the mutex.
                   3) Because ALL tasks pending on the mutex will be readied,
                      you MUST be careful because the resource(s) will no
                      longer be guarded by the mutex.
                   4) IMPORTANT: In the 'OS_DEL_ALWAYS' case, we assume that
                      the owner of the Mutex (if there is one) is ready-to-run
                      and is thus NOT pending on another kernel object or has
                      delayed itself.In other words, if a task owns the mutex
                      being deleted,that task will be made ready-to-run at
                      its original priority.
 */
wm_os_status_t wm_os_internal_recursive_mutex_delete(wm_os_mutex_t *mutex);

/**
 * @brief          This function waits for a mutual recursive semaphore
 *
 * @param[in]      *mutex     pointer to the event control block
                              associated with the desired mutex
 * @param[in]      wait_time  an optional timeout period (in clock ticks).
                              If non-zero, your task will wait for the resource
                              up to the amount of time specified by
                              this argument.
                              If you specify 0, however, your task will wait
                              forever at the specified mutex or,
                              until the resource becomes available.
 *
 * @retval         WM_OS_STATUS_SUCCESS     The call was successful and your task
                                      owns the mutex
 * @retval         WM_OS_STATUS_ERROR       failed
 *
 * @note           1) The task that owns the Mutex MUST NOT pend on
                      any other event while it owns the mutex.
                   2) You MUST NOT change the priority of the task
                      that owns the mutex
 */
wm_os_status_t wm_os_internal_recursive_mutex_acquire(wm_os_mutex_t *mutex, uint32_t wait_time);

/**
 * @brief          This function releases a mutual recursive semaphore
 *
 * @param[in]      *mutex    pointer to the event control block
                             associated with the desired mutex
 *
 * @retval         WM_OS_STATUS_SUCCESS     The call was successful and the mutex was signaled.
 * @retval         WM_OS_STATUS_ERROR       failed
 *
 * @note           None
 */
wm_os_status_t wm_os_internal_recursive_mutex_release(wm_os_mutex_t *mutex);

/**
 * @brief          This function creates a mutual exclusion semaphore
 *
 * @param[in]      **mutex   pointer to the event control clock (OS_EVENT)
                             associated with the created mutex.
 *
 * @retval         WM_OS_STATUS_SUCCESS     the call was successful
 * @retval         WM_OS_STATUS_ERROR       failed
 *
 * @note           1) The LEAST significant 8 bits of '.OSEventCnt' are used
                      to hold the priority number of the task owning the mutex
                      or 0xFF if no task owns the mutex.
                   2) The MOST  significant 8 bits of '.OSEventCnt' are used to
                      hold the priority number to use to reduce priority
                      inversion.
 */
wm_os_status_t wm_os_internal_mutex_create(wm_os_mutex_t **mutex);

/**
 * @brief          This function deletes a mutual exclusion semaphore and
                   readies all tasks pending on the it
 *
 * @param[in]      *mutex    pointer to the event control block associated
                             with the desired mutex
 *
 * @retval         WM_OS_STATUS_SUCCESS     The call was successful and the mutex
                                      was deleted
 * @retval         WM_OS_STATUS_ERROR       failed
 *
 * @note           1) This function must be used with care.  Tasks that would
                      normally expect the presence of the mutex MUST check the
                      return code of OSMutexPend().
                   2) This call can potentially disable interrupts for a long
                      time.  The interrupt disable time is directly
                      proportional to the number of tasks waiting on the mutex.
                   3) Because ALL tasks pending on the mutex will be readied,
                      you MUST be careful because the resource(s) will no
                      longer be guarded by the mutex.
                   4) IMPORTANT: In the 'OS_DEL_ALWAYS' case, we assume that
                      the owner of the Mutex (if there is one) is ready-to-run
                      and is thus NOT pending on another kernel object or has
                      delayed itself.In other words, if a task owns the mutex
                      being deleted,that task will be made ready-to-run at
                      its original priority.
 */
wm_os_status_t wm_os_internal_mutex_delete(wm_os_mutex_t *mutex);

/**
 * @brief          This function waits for a mutual exclusion semaphore
 *
 * @param[in]      *mutex     pointer to the event control block
                              associated with the desired mutex
 * @param[in]      wait_time  an optional timeout period (in clock ticks).
                              If non-zero, your task will wait for the resource
                              up to the amount of time specified by
                              this argument.
                              If you specify 0, however, your task will wait
                              forever at the specified mutex or,
                              until the resource becomes available.
 *
 * @retval         WM_OS_STATUS_SUCCESS     The call was successful and your task
                                      owns the mutex
 * @retval         WM_OS_STATUS_ERROR       failed
 *
 * @note           1) The task that owns the Mutex MUST NOT pend on
                      any other event while it owns the mutex.
                   2) You MUST NOT change the priority of the task
                      that owns the mutex
 */
wm_os_status_t wm_os_internal_mutex_acquire(wm_os_mutex_t *mutex, uint32_t wait_time);

/**
 * @brief          This function releases a mutual exclusion semaphore
 *
 * @param[in]      *mutex    pointer to the event control block
                             associated with the desired mutex
 *
 * @retval         WM_OS_STATUS_SUCCESS     The call was successful and the mutex was signaled.
 * @retval         WM_OS_STATUS_ERROR       failed
 *
 * @note           None
 */
wm_os_status_t wm_os_internal_mutex_release(wm_os_mutex_t *mutex);

/**
 * @brief          This function creates a semaphore
 *
 * @param[in]      **sem    pointer to the event control block (OS_EVENT)
                            associated with the created semaphore
 * @param[in]      cnt      the initial value for the semaphore.
                            If the value is 0, no resource is available
                            (or no event has occurred).
                            You initialize the semaphore to a non-zero value
                            to specify how many resources are available
                            (e.g. if you have 10 resources, you would
                            initialize the semaphore to 10).
 *
 * @retval         WM_OS_STATUS_SUCCESS     success,The call was successful
 * @retval         WM_OS_STATUS_ERROR       failed
 *
 * @note           None
 */
wm_os_status_t wm_os_internal_sem_create(wm_os_sem_t **sem, uint32_t cnt);

/**
 * @brief          This function reset a semaphore.
 *
 * @param[in]      *sem    pointer to the event control block associated
                           with the desired semaphore
 *
 * @retval         WM_HAL_SUCCESS     success
 * @retval         WM_HAL_ERROR       failed
 *
 * @note           None
 */
wm_os_status_t wm_os_internal_sem_reset(wm_os_sem_t *sem);

/**
 * @brief          This function deletes a semaphore and readies all tasks
                   pending on this semaphore.
 *
 * @param[in]      *sem    pointer to the event control block associated
                           with the desired semaphore
 *
 * @retval         WM_OS_STATUS_SUCCESS     success
 * @retval         WM_OS_STATUS_ERROR       failed
 *
 * @note           None
 */
wm_os_status_t wm_os_internal_sem_delete(wm_os_sem_t *sem);

/**
 * @brief          This function waits for a semaphore
 *
 * @param[in]      *sem         pointer to the event control block
                                associated with the desired semaphore
 * @param[in]      wait_time    an optional timeout period (in clock ticks).
                                If non-zero, your task will wait for the
                                resource up to the amount of time specified
                                by this argument.If you specify 0, however,
                                your task will wait forever at the specified
                                semaphore or, until the resource becomes
                                available (or the event occurs).
 *
 * @retval         WM_OS_STATUS_SUCCESS     success
 * @retval         WM_OS_STATUS_ERROR       failed
 *
 * @note           None
 */
wm_os_status_t wm_os_internal_sem_acquire(wm_os_sem_t *sem, uint32_t wait_time);

/**
 * @brief          This function waits for a semaphore
 *
 * @param[in]      *sem         pointer to the event control block
                                associated with the desired semaphore
 * @param[in]      wait_time_ms an optional timeout period (in ms).
                                If non-zero, your task will wait for the
                                resource up to the amount of time specified
                                by this argument.If you specify 0, however,
                                your task will wait forever at the specified
                                semaphore or, until the resource becomes
                                available (or the event occurs).
 *
 * @retval         WM_OS_STATUS_SUCCESS     success
 * @retval         WM_OS_STATUS_ERROR       failed
 *
 * @note           None
 */
wm_os_status_t wm_os_internal_sem_acquire_ms(wm_os_sem_t *sem, uint32_t wait_time_ms);

/**
 * @brief          This function signals a semaphore
 *
 * @param[in]      *sem    pointer to the event control block associated
                           with the desired semaphore
 *
 * @retval         WM_OS_STATUS_SUCCESS     success
 * @retval         WM_OS_STATUS_ERROR       failed
 *
 * @note           None
 */
wm_os_status_t wm_os_internal_sem_release(wm_os_sem_t *sem);

/**
 * @brief          This function gets the current number of semaphores
 *
 * @param[in]      *sem    pointer to the event control block associated
                           with the desired semaphore
 *
 * @retval         count   counting semaphore current count value
 * @retval         0       binary semaphore is not available
 * @retval         1       binary semaphore is available
 *
 * @note           None
 */
uint16_t wm_os_internal_sem_get_count(wm_os_sem_t *sem);

/**
 * @brief          This function creates a message queue if free event cont
                   rol blocks are available
 *
 * @param[in]      **queue       pointer to the event control clock (OS_EVENT)
                                 associated with the created queue
 * @param[in]      queue_size    the number of elements in the storage area
 *
 * @retval         WM_OS_STATUS_SUCCESS     success
 * @retval         WM_OS_STATUS_ERROR       failed
 *
 * @note           None
 */
wm_os_status_t wm_os_internal_queue_create(wm_os_queue_t **queue, uint32_t queue_size);

/**
 * @brief          This function creates a message queue with item size and queue size
 *
 * @param[in]      **queue       pointer to the event control clock (OS_EVENT)
                                 associated with the created queue
*  @param[in]      item_size     each element size in the storage area
 * @param[in]      queue_size    the number of elements in the storage area
 *
 * @retval         WM_OS_STATUS_SUCCESS     success
 * @retval         WM_OS_STATUS_ERROR       failed
 *
 * @note           None
 */
wm_os_status_t wm_os_internal_queue_create_ex(wm_os_queue_t **queue, uint32_t item_size, uint32_t queue_size);

/**
 * @brief          This function deletes a message queue and readies all
                   tasks pending on the queue
 *
 * @param[in]      *queue    pointer to the event control block associated
                             with the desired queue
 *
 * @retval         WM_OS_STATUS_SUCCESS     success
 * @retval         WM_OS_STATUS_ERROR       failed
 *
 * @note           None 
 */
wm_os_status_t wm_os_internal_queue_delete(wm_os_queue_t *queue);

/**
 * @brief          This function sends a message to a queue
 *
 * @param[in]      *queue     pointer to the event control block
                              associated with the desired queue
 * @param[in]      *msg       pointer to the message to send.
 *
 * @retval         0     success
 * @retval         other failed
 *
 * @note           None
 */
wm_os_status_t wm_os_internal_queue_send(wm_os_queue_t *queue, void *msg);

/**
 * @brief          This function sends a message to a queue with a blocking behavior.
 *

 * @param[in]      *queue     pointer to the event control block
                              associated with the desired queue
 * @param[in]      *msg       pointer to the message to send.
 *
 * @retval         0     success
 * @retval         other failed
 *
 * @note           This function will block indefinitely until the message is successfully sent.
 */
wm_os_status_t wm_os_internal_queue_forever_send(wm_os_queue_t *queue, void *msg);

/**
 * @brief          This function sends a message to a queue
 *
 * @param[in]      *queue     pointer to the event control block
                              associated with the desired queue
 * @param[in]      *msg       pointer to the message to send.It
                              will copy the size specified in
                              wm_os_queue_create_ex
 *
 * @retval         0     success
 * @retval         other failed
 *
 * @note           wm_os_queue_send only send a pointer address, but
                   wm_os_queue_send_ex send a struct msg content
 */
wm_os_status_t wm_os_internal_queue_send_ex(wm_os_queue_t *queue, void *msg);

/**
 * @brief          This function sends a message to a head of the queue
 *
 * @param[in]      *queue     pointer to the event control block
                              associated with the desired queue
 * @param[in]      *msg       pointer to the message to send.
 *
 * @retval         0     success
 * @retval         other failed
 *
 * @note           None
 */
wm_os_status_t wm_os_internal_queue_send_to_front(wm_os_queue_t *queue, void *msg);

/**
 * @brief          This function sends a message to a tail of the queue
 *
 * @param[in]      *queue     pointer to the event control block
                              associated with the desired queue
 * @param[in]      *msg       pointer to the message to send.
 *
 * @retval         0     success
 * @retval         other failed
 *
 * @note           None
 */
wm_os_status_t wm_os_internal_queue_send_to_back(wm_os_queue_t *queue, void *msg);

/**
 * @brief          This function is used to flush the contents of the message
                   queue.
 *
 * @param[in]      *queue     pointer to the event control block
                              associated with the desired queue
 *
 * @retval         WM_OS_STATUS_SUCCESS     success
 * @retval         WM_OS_STATUS_ERROR       failed
 *
 * @note           None
 */
wm_os_status_t wm_os_internal_queue_flush(wm_os_queue_t *queue);

/**
 * @brief          This function waits for a message to be sent to a queue
 *
 * @param[in]      *queue       pointer to the event control block associated
                                with the desired queue
 * @param[in]      **msg        pointer to the message received
 * @param[in]      wait_time    an optional timeout period (in clock ticks).
                                If non-zero, your task will wait for a message
                                to arrive at the queue up to the amount of time
                                specified by this argument.  If you specify WM_OS_WAIT_TIME_MAX,
                                however, your task will wait forever at the
                                specified queue or, until a message arrives.
 *
 * @retval         WM_OS_STATUS_SUCCESS     success
 * @retval         WM_OS_STATUS_ERROR       failed
 *
 * @note           None
 */
wm_os_status_t wm_os_internal_queue_receive(wm_os_queue_t *queue, void **msg, uint32_t wait_time);

/**
 * @brief          This function waits for a message to be sent to a queue
 *
 * @param[in]      *queue       pointer to the event control block associated
                                with the desired queue
 * @param[in]      **msg        pointer to the message received
 * @param[in]      wait_time_ms an optional timeout period (in ms).
                                If non-zero, your task will wait for a message
                                to arrive at the queue up to the amount of time
                                specified by this argument.  If you specify WM_OS_WAIT_TIME_MAX,
                                however, your task will wait forever at the
                                specified queue or, until a message arrives.
 *
 * @retval         WM_OS_STATUS_SUCCESS     success
 * @retval         WM_OS_STATUS_ERROR       failed
 *
 * @note           None
 */
wm_os_status_t wm_os_internal_queue_receive_ms(wm_os_queue_t *queue, void **msg, uint32_t wait_time_ms);

/**
 * @brief          This function determines whether the queue is empty
 *
 * @param[in]      *queue       pointer to the event control block associated
                                with the desired queue
 *
 * @retval         0       queue not empty
 * @retval         1       queue empty
 *
 * @note           None
 */
uint8_t wm_os_internal_queue_is_empty(wm_os_queue_t *queue);

/**
 * @brief         This function return the number of free spaces available in a queue.
 *                This is equal to the number of items that can be sent to the queue
 *                before the queue becomes full if no items are removed.
 *
 * @param          *queue       pointer to the event control block associated
                                with the desired queue
 * @return         The number of spaces available in the queue.
 * @note           None
 */
uint32_t wm_os_internal_queue_space_available(wm_os_queue_t *queue);

/**
 * @brief          This function is used to remove a queue
 *
 * @param[in]      *queue       pointer to the event control block associated
                                with the desired queue
 * @param[in]      msg          pointer to the message remove
 *
 * @retval         0       the queue was removed success
 * @retval         other   the queue was removed failed
 *
 * @note           None
 */
wm_os_status_t wm_os_internal_queue_remove(wm_os_queue_t *queue, void *msg);

/**
 * @brief          This function is used to peek an element from a queue, the element
 *                 still exists in the queue
 *
 * @param[in]      *queue       pointer to the event control block associated
                                with the desired queue
 * @param[in]      msg          pointer to the message remove
 * @param[in]      wait_time    an optional timeout period (in clock ticks).
                                If non-zero, your task will wait for a message
                                to arrive at the queue up to the amount of time
                                specified by this argument.  If you specify WM_OS_WAIT_TIME_MAX,
                                however, your task will wait forever at the
                                specified queue or, until a message arrives.
 * @retval         0       the element is retrieved from the queue sucessfully
 * @retval         other   the operation failed, eg. timeout occurs;
 *
 * @note           None
 */
wm_os_status_t wm_os_internal_queue_peek(wm_os_queue_t *queue, void **msg, uint32_t wait_time);

/**
 * @brief          This function creates a message mailbox if free event
                   control blocks are available.
 *
 * @param[in]      **mailbox     pointer to the event control clock (OS_EVENT)
                                 associated with the created mailbox
 * @param[in]      mailbox_size  size
 *
 * @retval         WM_OS_STATUS_SUCCESS     success
 * @retval         WM_OS_STATUS_ERROR       failed
 *
 * @note           None
 */
wm_os_status_t wm_os_internal_mailbox_create(wm_os_mailbox_t **mailbox, uint32_t mailbox_size);

/**
 * @brief          This function deletes a mailbox and readies all of the tasks
                   pending on the this mailbox.
 *
 * @param[in]      *mailbox    pointer to the event control block
                               associated with the desired mailbox.
 *
 * @retval         WM_OS_STATUS_SUCCESS    success
 * @retval         WM_OS_STATUS_ERROR      failed
 *
 * @note           None
 */
wm_os_status_t wm_os_internal_mailbox_delete(wm_os_mailbox_t *mailbox);

/**
 * @brief          This function sends a message to a mailbox
 *
 * @param[in]      *mailbox    pointer to the event control block associated
                               with the desired mailbox
 * @param[in]      *msg        pointer to the message to send.
                               You MUST NOT send a NULL pointer
 *
 * @retval         WM_OS_STATUS_SUCCESS     success
 * @retval         WM_OS_STATUS_ERROR       failed
 *
 * @note           None
 */
wm_os_status_t wm_os_internal_mailbox_send(wm_os_mailbox_t *mailbox, void *msg);

/**
 * @brief          This function waits for a message to be sent to a mailbox
 *
 * @param[in]      *mailbox    pointer to the event control block associated
                               with the desired mailbox
 * @param[in]      **msg       pointer to the message received
 * @param[in]      wait_time   an optional timeout period (in clock ticks).
                               If non-zero, your task will wait for a message
                               to arrive at the mailbox up to the amount of
                               time specified by this argument.
                               If you specify 0, however, your task will wait
                               forever at the specified mailbox or,
                               until a message arrives.
 *
 * @retval         WM_OS_STATUS_SUCCESS     success
 * @retval         WM_OS_STATUS_ERROR       failed
 *
 * @note           None
 */
wm_os_status_t wm_os_internal_mailbox_receive(wm_os_mailbox_t *mailbox, void **msg, uint32_t wait_time);

/**
 * @brief          This function mainly create an event group
 *
 * @param[in]      **event       pointer to a successful event group handle
 *
 * @retval         WM_OS_STATUS_SUCCESS     success
 * @retval         WM_OS_STATUS_ERROR       failed
 *
 * @note           None
 */
wm_os_status_t wm_os_internal_event_create(wm_os_event_t **event);

/**
 * @brief          This function mainly delete an event group
 *
 * @param[in]      *event       a event group handle
 *
 * @retval         WM_OS_STATUS_SUCCESS     success
 * @retval         WM_OS_STATUS_ERROR       failed
 *
 * @note           None
 */
wm_os_status_t wm_os_internal_event_delete(wm_os_event_t *event);

/**
 * @brief          This function get the event flag bit value in the event group
 *
 * @param[in]      *event          a event group handle
 * @param[in]      wait_event      waiting event flag bit
 * @param[in]      *return_event   receives the event flag bit value in the event group
 * @param[in]      equal           the value of the event flag bit
 * @param[in]      wait_time       wait for blocking time
 *
 * @retval         WM_OS_STATUS_SUCCESS     success
 * @retval         WM_OS_STATUS_ERROR       failed
 *
 * @note           None
 */
wm_os_status_t wm_os_internal_event_get(wm_os_event_t *event, uint32_t wait_event, uint32_t *return_event, bool equal,
                                        uint32_t wait_time);

/**
 * @brief          This function set the event flag bit in an event group
 *
 * @param[in]      *event       a event group handle
 * @param[in]      event_bits   the event flag bit in the event group needs to be set
 *
 * @retval         WM_OS_STATUS_SUCCESS     success
 * @retval         WM_OS_STATUS_ERROR       failed
 *
 * @note           None
 */
wm_os_status_t wm_os_internal_event_put(wm_os_event_t *event, uint32_t event_bits);

/**
*
* @brief          Clear bits within an event group.
*
* @param[in]      *event       a event group handle
* @param[in]      event_bits   A bitwise value that indicates the bit or bits to cleared
*
* @retval         WM_OS_STATUS_SUCCESS     success
* @retval         WM_OS_STATUS_ERROR       failed
*
* @note           None  For example, to clear bit 3 only, set event_bits to
*                 0x08.  To clear bit 3 and bit 0 set event_bits to 0x09.
*/
wm_os_status_t wm_os_internal_event_clear_bits(wm_os_event_t *event, uint32_t event_bits);

/**
*
* @brief          Retrieve bits within an event group.
*
* @param[in]      *event       a event group handle
* @param[out]     *event_bits   A bitwise value that stored in the event group
*
*
* @retval         WM_OS_STATUS_SUCCESS     success
* @retval         WM_OS_STATUS_ERROR       failed
*
*/
wm_os_status_t wm_os_internal_event_get_bits(wm_os_event_t *event, uint32_t *event_bits);

/**
 * @brief          This function is used by your application to obtain the
                   current value of the 32-bit counter which keeps track
                   of the number of clock ticks since the system starts.
 *
 * @retval         current value of OSTime
 *
 * @note           None
 */
uint32_t wm_os_internal_get_time(void);

/**
 * @brief          This function is used by your application to obtain the
                   current value of the 32-bit counter which keeps track
                   of the number of ms since the system starts.
 *
 * @retval         current value of OSTime
 *
 * @note           None
 */
uint32_t wm_os_internal_get_time_ms(void);

/**
 * @brief          This function is used to disable interrupts by preserving
                   the state of interrupts
 *
 * @return         None
 *
 * @note           None
 */
void wm_os_internal_set_critical(void);

/**
 * @brief          This function is used to enable interrupts by preserving
                   the state of interrupts
 *
 * @return         None
 *
 * @note           None
 */
void wm_os_internal_release_critical(void);

/**
 * @brief          This function is called by your application code to create
                   a timer
 *
 * @param[in]      **timer        pointer to an OS_TMR data structure.
                                  This is the 'handle' that your application will
                                  use to reference the timer created
 * @param[in]      callback       pointer to a callback function that will
                                  be called when the timer expires.  The callback
                                  function must be declared as follows
                                  void MyCallback (OS_TMR *ptmr, void *p_arg);
 * @param[in]      *callback_arg  argument (a pointer) that is passed to
                                  the callback function when it is called
 * @param[in]      period         The 'period' being repeated for the timer.
                                  If you specified 'OS_TMR_OPT_PERIODIC' as
                                  an option, when the timer expires, it will
                                  automatically restart with the same period.
 * @param[in]      repeat         if repeat
 * @param[in]      *name          pointer to an ASCII string that is used to
                                  name the timer.  Names are useful for
                                  debugging.
 *
 * @retval         WM_OS_STATUS_SUCCESS     success
 * @retval         WM_OS_STATUS_ERROR       failed
 *
 * @note           None
 */
wm_os_status_t wm_os_internal_timer_create(wm_os_timer_t **timer, wm_os_timer_callback callback, void *callback_arg,
                                           uint32_t period, bool repeat, char *name);

/**
 * @brief          This function is called by your application code to create
                   a timer
 *
 * @param[in]      **timer        pointer to an OS_TMR data structure.
                                  This is the 'handle' that your application will
                                  use to reference the timer created
 * @param[in]      callback       pointer to a callback function that will
                                  be called when the timer expires.  The callback
                                  function must be declared as follows
                                  void MyCallback (OS_TMR *ptmr, void *p_arg);
 * @param[in]      *callback_arg  argument (a pointer) that is passed to
                                  the callback function when it is called
 * @param[in]      period_ms      The 'period' being repeated for the timer.
                                  If you specified 'OS_TMR_OPT_PERIODIC' as
                                  an option, when the timer expires, it will
                                  automatically restart with the same period.
 * @param[in]      repeat         if repeat
 * @param[in]      *name          pointer to an ASCII string that is used to
                                  name the timer.  Names are useful for
                                  debugging.
 *
 * @retval         WM_OS_STATUS_SUCCESS     success
 * @retval         WM_OS_STATUS_ERROR       failed
 *
 * @note           None
 */
wm_os_status_t wm_os_internal_timer_create_ms(wm_os_timer_t **timer, wm_os_timer_callback callback, void *callback_arg,
                                              uint32_t period_ms, bool repeat, char *name);

/**
 * @brief          This function is called by your application code to start
                   a timer.
 *
 * @param[in]      *timer    pointer to an OS_TMR
 *
 * @return         None
 *
 * @note           None
 */
void wm_os_internal_timer_start(wm_os_timer_t *timer);

/**
 * @brief          This function is called by your application code to change
 *                 a timer wait time.
 *
 * @param[in]      *timer   Is a pointer to an OS_TMR
 * @param[in]      ticks    is the wait time
 *
 * @return         None
 *
 * @note           None
 */
void wm_os_internal_timer_change(wm_os_timer_t *timer, uint32_t ticks);

/**
 * @brief          This function is called by your application code to change
 *                 a timer wait time.
 *
 * @param[in]      *timer   Is a pointer to an OS_TMR
 * @param[in]      ms       Is the wait time, unit: milliseconds
 *
 * @return         None
 *
 * @note           None
 */
void wm_os_internal_timer_change_ms(wm_os_timer_t *timer, uint32_t ms);

/**
 * @brief          This function is called by your application code to stop
                   a timer
 *
 * @param[in]      *timer   pointer to the timer to stop.
 *
 * @return         None
 *
 * @note           None
 */
void wm_os_internal_timer_stop(wm_os_timer_t *timer);

/**
 * @brief          This function is called by your application code to reset
                   a timer
 *
 * @param[in]      *timer   pointer to the timer to reset.
 *
 * @return         None
 *
 * @note           None
 */
void wm_os_internal_timer_reset(wm_os_timer_t *timer);

/**
 * @brief          This function is called by your application code to delete
                   a timer
 *
 * @param[in]      *timer    pointer to the timer to delete
 *
 * @retval         WM_OS_STATUS_SUCCESS     success
 * @retval         WM_OS_STATUS_ERROR       failed
 *
 * @note           None
 */
wm_os_status_t wm_os_internal_timer_delete(wm_os_timer_t *timer);

/**
 * @brief          This function is called to delay execution of the currently
                   running task until the specified number of system
                   ticks expires. This, of course, directly equates to
                   delaying the current task for some time to expire.
                   There will be no delay if the specified delay is 0.
                   If the specified delay is greater than 0 then,
                   a context switch will executed.
 *
 * @param[in]      ticks   the time delay that the task will be suspended
                           in number of clock 'ticks'.Note that by specifying 0,
                           the task will not be delayed.
 *
 * @return         None
 *
 * @note           None
 */
void wm_os_internal_time_delay(uint32_t ticks);

/**
 * @brief          This function is called to delay execution of the currently
                   running task until the specified number of system
                   ticks expires. This, of course, directly equates to
                   delaying the current task for some time to expire.
                   There will be no delay if the specified delay is 0.
                   If the specified delay is greater than 0 then,
                   a context switch will executed.
 *
 * @param[in]      ms      the time delay that the task will be suspended
                           in number of ms.Note that by specifying 0,
                           the task will not be delayed.
 *
 * @return         None
 *
 * @note           None
 */
void wm_os_internal_time_delay_ms(uint32_t ms);

/**
 * @brief          This function queries whether the software timer is active
 *                 or dormant
 *
 * @param[in]      *timer    pointer to the timer to active
 *
 * @retval         0       the timer is dormant
 * @retval         1       the timer is active
 *
 * @note           None
 */
uint8_t wm_os_internal_timer_active(wm_os_timer_t *timer);

/**
 * @brief          This function returns when the timer expires
 *
 * @param[in]      *timer    pointer to the timer to expirytime
 *
 * @retval         returns the timer expires time
 *
 * @note           None
 */
uint32_t wm_os_internal_timer_expirytime(wm_os_timer_t *timer);

/**
 * @brief          This function is used to display all the tasks' detail status.
 *
 * @return         None
 *
 * @note           None
 */
void wm_os_internal_disp_task_stat_info(void);

/**
 * @brief          This function to display sys runtime stats info.
 *
 * @return         None
 *
 * @note           None
*/
void wm_os_internal_disp_sys_runtime_stats_info(void);

/**
 * @brief          This function returns the ISR count.
 *
 * @return         0
 * @return         1
 *
 * @note           None
 */
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

/**
 * @brief          This function is used to dynamically open a block of memory.
 *
 * @param[in]      size    The amount of memory that needs to be dynamically opened.
 *
 * @return         NULL    malloc failed.
 *                 pointer pointer to the address of the allocated memory.
 *
 */
#define wm_os_internal_malloc(size)       wm_heap_caps_alloc(size, WM_HEAP_CAP_DEFAULT)

/**
 * @brief          This function is called by memory reallocation.
 *
 * @param[in]      ptr     pointer to the address of the allocated memory.
 * @param[in]      size    The amount of memory that needs to be reallocated.
 *
 * @return         NULL    realloc failed.
 *                 pointer pointer to the address of the allocated memory.
 *
 */
#define wm_os_internal_realloc(ptr, size) wm_heap_caps_realloc(ptr, size, WM_HEAP_CAP_DEFAULT)

/**
 * @brief          This function dynamically opens up a block of memory and initializes the memory to 0.
 *
 * @param[in]      nelem     The number of elements to be allocated.
 * @param[in]      elsize    Size of element.
 *
 * @return         NULL    calloc failed.
 *                 pointer pointer to the address of the allocated memory.
 *
 */
void *wm_os_internal_calloc(size_t nelem, size_t elsize);
#endif //CONFIG_HEAP_USE_TRACING

/**
 * @brief          This function is used to free dynamically created memory.
 *
 * @param[in]      ptr     The pointer points to the first address of memory.
 *
 * @return         None
 *
 */
#define wm_os_internal_free(ptr) wm_heap_caps_free(ptr)

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* end of __WM_OSAL_H__ */
