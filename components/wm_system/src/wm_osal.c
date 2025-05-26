#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "wm_types.h"
#include "wm_osal.h"
#include "wm_irq.h"
#include "wm_drv_irq.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"
#include "freertos/timers.h"
#include "freertos/event_groups.h"

extern void coret_irq_handler(void);

/*
*********************************************************************************************************
*                                     CREATE A TASK (Extended Version)
*
* Description: This function is used to have uC/OS-II manage the execution of a task.  Tasks can either
*              be created prior to the start of multitasking or by a running task.  A task cannot be
*              created by an ISR.
*
* Arguments  : task      is a pointer to the task'
*
*   name  is the task's name
*
*   entry is the task's entry function
*
*              param     is a pointer to an optional data area which can be used to pass parameters to
*                        the task when the task first executes.  Where the task is concerned it thinks
*                        it was invoked and passed the argument 'param' as follows:
*
*                            void Task (void *param)
*                            {
*                                for (;;) {
*                                    Task code;
*                                }
*                            }
*
*              stk_size  is the size of the stack in number of elements.  If WM_OS_STK is set to u8,
*                        'stk_size' corresponds to the number of bytes available.  If WM_OS_STK is set to
*                        INT16U, 'stk_size' contains the number of 16-bit entries available.  Finally, if
*                        WM_OS_STK is set to INT32U, 'stk_size' contains the number of 32-bit entries
*                        available on the stack.
*
*              prio      is the task's priority.  A unique priority MUST be assigned to each task and the
*                        lower the number, the higher the priority.
*
*              flag       contains additional information about the behavior of the task.
*
* Returns    : WM_OS_STATUS_SUCCESS             if the function was successful.
*              WM_OS_STATUS_ERROR
*********************************************************************************************************
*/
wm_os_status_t wm_os_internal_task_create(wm_os_task_t *task, const char *name, void (*entry)(void *param), void *param,
                                          uint32_t stk_size, uint32_t prio, uint32_t flag)
{
    wm_os_status_t os_status;
    BaseType_t xreturn;

    stk_size /= sizeof(StackType_t);
    xreturn = xTaskCreate(entry, name, stk_size, param, prio, (TaskHandle_t *const)task);

    if (xreturn == pdTRUE) {
        os_status = WM_OS_STATUS_SUCCESS;
    } else {
        if (task) {
            *task = NULL;
        }
        os_status = WM_OS_STATUS_ERROR;
    }

    return os_status;
}

/*
*********************************************************************************************************
*                                            DELETE A TASK
*
* Description: This function allows you to delete a task.  The calling task can delete itself by
*              its own priority number.  The deleted task is returned to the dormant state and can be
*              re-activated by creating the deleted task again.
*
* Arguments  : prio: the task priority
*                    freefun: function to free resource
*
* Returns    : WM_OS_STATUS_SUCCESS             if the call is successful
*               WM_OS_STATUS_ERROR
*********************************************************************************************************
*/
#if (INCLUDE_vTaskDelete == 1)
wm_os_status_t wm_os_internal_task_del(wm_os_task_t *task)
{
    vTaskDelete((TaskHandle_t)task);

    return WM_OS_STATUS_SUCCESS;
}

wm_os_status_t wm_os_internal_task_del_ex(void *handle, void (*freefun)(void *arg), void *arg)
{
    vTaskDeleteEX(handle, freefun, arg);

    return WM_OS_STATUS_SUCCESS;
}
#endif

#if (INCLUDE_vTaskSuspend == 1)
/*
*********************************************************************************************************
*                                            SUSPEND A TASK
*
* Description: This function is called to suspend a task.
*
* Arguments  : task     is a pointer to the task
*
* Returns    : WM_OS_STATUS_SUCCESS               if the requested task is suspended
*
* Note       : You should use this function with great care.  If you suspend a task that is waiting for
*              an event (i.e. a message, a semaphore, a queue ...) you will prevent this task from
*              running when the event arrives.
*********************************************************************************************************
*/
wm_os_status_t wm_os_internal_task_suspend(wm_os_task_t *task)
{
    vTaskSuspend(*task);

    return WM_OS_STATUS_SUCCESS;
}
/*
**********************************************************************************************************
*
* Returns:  get current task handle;
*
**********************************************************************************************************
*/
wm_os_task_t wm_os_internal_task_id(void)
{
    return (wm_os_task_t)xTaskGetCurrentTaskHandle();
}
/*
**********************************************************************************************************
*
* Returns:  get current task state;
*
**********************************************************************************************************
*/
wm_os_sched_state_t wm_os_internal_task_schedule_state(void)
{
    return (wm_os_sched_state_t)xTaskGetSchedulerState();
}

/*
*********************************************************************************************************
*                                        RESUME A SUSPENDED TASK
*
* Description: This function is called to resume a previously suspended task.
*
* Arguments  : task     is a pointer to the task
*
* Returns    : WM_OS_STATUS_SUCCESS                if the requested task is resumed
*
*********************************************************************************************************
*/
wm_os_status_t wm_os_internal_task_resume(wm_os_task_t *task)
{
    vTaskResume(*task);

    return WM_OS_STATUS_SUCCESS;
}
#endif

/*
*********************************************************************************************************
*
* Description: This function is called to return current task name.
*
* Arguments  : void
*
* Returns    : return task name if current task is running , otherwise return const "Noname"
*
*********************************************************************************************************
*/
const char *wm_os_internal_get_task_name(void)
{
    const char *pcTaskName;
    const char *pcNoTask = "Noname";

    if (xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED) {
        pcTaskName = (const char *)pcTaskGetTaskName(NULL);
    } else {
        pcTaskName = pcNoTask;
    }
    return pcTaskName;
}

/*
*********************************************************************************************************
*
* Description: This function is called to change task name.
*
* Arguments  : task     is a pointer to the task handle
*              name     task name value
*
* Returns    : void
*
*********************************************************************************************************
*/
void wm_os_internal_set_task_name(wm_os_task_t *task, const char *name)
{
    vTaskSetName(*task, name);
}

/*
*********************************************************************************************************
*
* Description: This function is called to do context switch.
*
* Arguments  : void
*
* Returns    : void
*
*********************************************************************************************************
*/
void wm_os_internal_task_yield(void)
{
    taskYIELD();
}

/*
*********************************************************************************************************
*
* Description: This function is called to query maximum priority of OS.
*
* Arguments  : void
*
* Returns    : maximum priority value
*
*********************************************************************************************************
*/
uint32_t wm_os_internal_task_get_max_priority(void)
{
    return configMAX_PRIORITIES - 1;
}

/*
*********************************************************************************************************
*
* Description: This function is called to suspend task scheduling.
*
* Suspends the scheduler without disabling interrupts.  Context switches will
* not occur while the scheduler is suspended.
*
* After calling vTaskSuspendAll () the calling task will continue to execute
* without risk of being swapped out until a call to xTaskResumeAll () has been
* made.
*
* API functions that have the potential to cause a context switch (for example,
* vTaskDelayUntil(), xQueueSend(), etc.) must not be called while the scheduler
* is suspended.
*********************************************************************************************************
*/
void wm_os_internal_task_schedule_lock(void)
{
    vTaskSuspendAll();
}

/*
*********************************************************************************************************
*
* Description: This function is called to resume task scheduling.
*
* Resumes scheduler activity after it was suspended by a call to
* vTaskSuspendAll().
*
* xTaskResumeAll() only resumes the scheduler.  It does not unsuspend tasks
* that were previously suspended by a call to vTaskSuspend(). So taskYIELD function will be used to switch
* context
*********************************************************************************************************
*/
void wm_os_internal_task_schedule_unlock(void)
{
    BaseType_t xreturn;

    xreturn = xTaskResumeAll();

    if (!xreturn)
        taskYIELD();
}

/*
*********************************************************************************************************
*                                  CREATE A MUTUAL EXCLUSION SEMAPHORE
*
* Description: This function creates a mutual exclusion semaphore.
*
* Arguments  :mutex          is a pointer to the event control clock (OS_EVENT) associated with the
*                            created mutex.
*
*
* Returns    :WM_OS_STATUS_SUCCESS         if the call was successful.
*             WM_OS_STATUS_ERROR
*
*********************************************************************************************************
*/
#if (1 == configUSE_RECURSIVE_MUTEXES)
wm_os_status_t wm_os_internal_recursive_mutex_create(wm_os_mutex_t **mutex)
{
    wm_os_status_t os_status;

    *mutex = xSemaphoreCreateRecursiveMutex();

    if (*mutex != NULL)
        os_status = WM_OS_STATUS_SUCCESS;
    else
        os_status = WM_OS_STATUS_ERROR;

    return os_status;
}

/*
*********************************************************************************************************
*                                          DELETE A MUTEX
*
* Description: This function deletes a mutual recursive semaphore and readies all tasks pending on the it.
*
* Arguments  : mutex        is a pointer to the event control block associated with the desired mutex.
*
* Returns    : WM_OS_STATUS_SUCCESS             The call was successful and the mutex was deleted
*                            WM_OS_STATUS_ERROR        error
*
* Note(s)    : 1) This function must be used with care.  Tasks that would normally expect the presence of
*                 the mutex MUST check the return code of OSMutexPend().
*
*              2) This call can potentially disable interrupts for a long time.  The interrupt disable
*                 time is directly proportional to the number of tasks waiting on the mutex.
*
*              3) Because ALL tasks pending on the mutex will be readied, you MUST be careful because the
*                 resource(s) will no longer be guarded by the mutex.
*
*              4) IMPORTANT: In the 'OS_DEL_ALWAYS' case, we assume that the owner of the Mutex (if there
*                            is one) is ready-to-run and is thus NOT pending on another kernel object or
*                            has delayed itself.  In other words, if a task owns the mutex being deleted,
*                            that task will be made ready-to-run at its original priority.
*********************************************************************************************************
*/
wm_os_status_t wm_os_internal_recursive_mutex_delete(wm_os_mutex_t *mutex)
{
    vSemaphoreDelete((QueueHandle_t)mutex);

    return WM_OS_STATUS_SUCCESS;
}

/*
*********************************************************************************************************
*                                  PEND ON MUTUAL RECURSIVE SEMAPHORE
*
* Description: This function waits for a mutual recursive semaphore.
*
* Arguments  : mutex        is a pointer to the event control block associated with the desired
*                            mutex.
*
*              wait_time       is an optional timeout period (in clock ticks).  If non-zero, your task will
*                            wait for the resource up to the amount of time specified by this argument.
*                            If you specify WM_OS_WAIT_TIME_MAX, however, your task will wait forever at the specified
*                            mutex or, until the resource becomes available.
*
*
*
* Returns    : WM_OS_STATUS_SUCCESS        The call was successful and your task owns the mutex
*              WM_OS_STATUS_ERROR
*
*********************************************************************************************************
*/
wm_os_status_t wm_os_internal_recursive_mutex_acquire(wm_os_mutex_t *mutex, uint32_t wait_time)
{
    uint8_t error;
    wm_os_status_t os_status;
    portBASE_TYPE pxHigherPriorityTaskWoken = pdFALSE;
    unsigned int time;

    if (WM_OS_WAIT_TIME_MAX == wait_time)
        time = portMAX_DELAY;
    else
        time = wait_time;
    uint8_t isrcount = 0;

    isrcount = wm_os_internal_get_isr_count();
    if (isrcount > 0) {
        error = xSemaphoreTakeFromISR((QueueHandle_t)mutex, &pxHigherPriorityTaskWoken);
        if ((pdTRUE == pxHigherPriorityTaskWoken) && (1 == isrcount)) {
            portYIELD_FROM_ISR(pdTRUE);
        }
    } else {
        error = xSemaphoreTakeRecursive((QueueHandle_t)mutex, time);
    }

    if (error == pdPASS)
        os_status = WM_OS_STATUS_SUCCESS;
    else
        os_status = WM_OS_STATUS_ERROR;

    return os_status;
}

/*
*********************************************************************************************************
*                                  POST TO A MUTUAL RECURSIVE SEMAPHORE
*
* Description: This function signals a mutual recursive semaphore
*
* Arguments  : mutex              is a pointer to the event control block associated with the desired
*                                  mutex.
*
* Returns    : WM_OS_STATUS_SUCCESS             The call was successful and the mutex was signaled.
*              WM_OS_STATUS_ERROR
*********************************************************************************************************
*/
wm_os_status_t wm_os_internal_recursive_mutex_release(wm_os_mutex_t *mutex)
{
    uint8_t error;
    wm_os_status_t os_status;
    portBASE_TYPE pxHigherPriorityTaskWoken = pdFALSE;
    uint8_t isrcount                        = 0;

    isrcount = wm_os_internal_get_isr_count();
    if (isrcount > 0) {
        error = xSemaphoreGiveFromISR((QueueHandle_t)mutex, &pxHigherPriorityTaskWoken);
        if ((pdTRUE == pxHigherPriorityTaskWoken) && (1 == isrcount)) {
            portYIELD_FROM_ISR(pxHigherPriorityTaskWoken);
        }
    } else {
        error = xSemaphoreGiveRecursive((QueueHandle_t)mutex);
    }
    if (error == pdPASS)
        os_status = WM_OS_STATUS_SUCCESS;
    else
        os_status = WM_OS_STATUS_ERROR;

    return os_status;
}
#endif

/*
*********************************************************************************************************
*                                  CREATE A MUTUAL EXCLUSION SEMAPHORE
*
* Description: This function creates a mutual exclusion semaphore.
*
* Arguments  : mutex          is a pointer to the event control clock (OS_EVENT) associated with the
*                            created mutex.
*
*
* Returns    :WM_OS_STATUS_SUCCESS         if the call was successful.
*                 WM_OS_STATUS_ERROR
*
*********************************************************************************************************
*/
#if (1 == configUSE_MUTEXES)
wm_os_status_t wm_os_internal_mutex_create(wm_os_mutex_t **mutex)
{
    wm_os_status_t os_status;

    *mutex = xSemaphoreCreateMutex();

    if (*mutex != NULL)
        os_status = WM_OS_STATUS_SUCCESS;
    else
        os_status = WM_OS_STATUS_ERROR;

    return os_status;
}

/*
*********************************************************************************************************
*                                          DELETE A MUTEX
*
* Description: This function deletes a mutual exclusion semaphore and readies all tasks pending on the it.
*
* Arguments  : mutex        is a pointer to the event control block associated with the desired mutex.
*
* Returns    : WM_OS_STATUS_SUCCESS             The call was successful and the mutex was deleted
*                            WM_OS_STATUS_ERROR        error
*
* Note(s)    : 1) This function must be used with care.  Tasks that would normally expect the presence of
*                 the mutex MUST check the return code of OSMutexPend().
*
*              2) This call can potentially disable interrupts for a long time.  The interrupt disable
*                 time is directly proportional to the number of tasks waiting on the mutex.
*
*              3) Because ALL tasks pending on the mutex will be readied, you MUST be careful because the
*                 resource(s) will no longer be guarded by the mutex.
*
*              4) IMPORTANT: In the 'OS_DEL_ALWAYS' case, we assume that the owner of the Mutex (if there
*                            is one) is ready-to-run and is thus NOT pending on another kernel object or
*                            has delayed itself.  In other words, if a task owns the mutex being deleted,
*                            that task will be made ready-to-run at its original priority.
*********************************************************************************************************
*/
wm_os_status_t wm_os_internal_mutex_delete(wm_os_mutex_t *mutex)
{
    vSemaphoreDelete((QueueHandle_t)mutex);

    return WM_OS_STATUS_SUCCESS;
}

/*
*********************************************************************************************************
*                                  PEND ON MUTUAL EXCLUSION SEMAPHORE
*
* Description: This function waits for a mutual exclusion semaphore.
*
* Arguments  : mutex        is a pointer to the event control block associated with the desired
*                            mutex.
*
*              wait_time       is an optional timeout period (in clock ticks).  If non-zero, your task will
*                            wait for the resource up to the amount of time specified by this argument.
*                            If you specify WM_OS_WAIT_TIME_MAX, however, your task will wait forever at the specified
*                            mutex or, until the resource becomes available.
*
*
*
* Returns    : WM_OS_STATUS_SUCCESS        The call was successful and your task owns the mutex
*                  WM_OS_STATUS_ERROR
*
* Note(s)    : 1) The task that owns the Mutex MUST NOT pend on any other event while it owns the mutex.
*
*              2) You MUST NOT change the priority of the task that owns the mutex
*********************************************************************************************************
*/
//不可在中断中调用
wm_os_status_t wm_os_internal_mutex_acquire(wm_os_mutex_t *mutex, uint32_t wait_time)
{
    uint8_t error;
    wm_os_status_t os_status;
    portBASE_TYPE pxHigherPriorityTaskWoken = pdFALSE;
    unsigned int time;

    if (WM_OS_WAIT_TIME_MAX == wait_time)
        time = portMAX_DELAY;
    else
        time = wait_time;
    uint8_t isrcount = 0;

    isrcount = wm_os_internal_get_isr_count();
    if (isrcount > 0) {
        error = xSemaphoreTakeFromISR((QueueHandle_t)mutex, &pxHigherPriorityTaskWoken);
        if ((pdTRUE == pxHigherPriorityTaskWoken) && (1 == isrcount)) {
            portYIELD_FROM_ISR(pdTRUE);
        }
    } else {
        error = xSemaphoreTake((QueueHandle_t)mutex, time);
    }

    if (error == pdPASS)
        os_status = WM_OS_STATUS_SUCCESS;
    else
        os_status = WM_OS_STATUS_ERROR;

    return os_status;
}

/*
*********************************************************************************************************
*                                  POST TO A MUTUAL EXCLUSION SEMAPHORE
*
* Description: This function signals a mutual exclusion semaphore
*
* Arguments  : mutex              is a pointer to the event control block associated with the desired
*                                  mutex.
*
* Returns    : WM_OS_STATUS_SUCCESS             The call was successful and the mutex was signaled.
*               WM_OS_STATUS_ERROR
*********************************************************************************************************
*/
wm_os_status_t wm_os_internal_mutex_release(wm_os_mutex_t *mutex)
{
    uint8_t error;
    wm_os_status_t os_status;
    portBASE_TYPE pxHigherPriorityTaskWoken = pdFALSE;
    uint8_t isrcount                        = 0;

    isrcount = wm_os_internal_get_isr_count();
    if (isrcount > 0) {
        error = xSemaphoreGiveFromISR((QueueHandle_t)mutex, &pxHigherPriorityTaskWoken);
        if ((pdTRUE == pxHigherPriorityTaskWoken) && (1 == isrcount)) {
            portYIELD_FROM_ISR(pxHigherPriorityTaskWoken);
        }
    } else {
        error = xSemaphoreGive((QueueHandle_t)mutex);
    }
    if (error == pdPASS)
        os_status = WM_OS_STATUS_SUCCESS;
    else
        os_status = WM_OS_STATUS_ERROR;

    return os_status;
}
#endif

/*
*********************************************************************************************************
*                                           CREATE A SEMAPHORE
*
* Description: This function creates a semaphore.
*
* Arguments  :sem   is a pointer to the event control block (OS_EVENT) associated with the
*                            created semaphore
*    cnt           is the initial value for the semaphore.  If the value is 0, no resource is
*                            available (or no event has occurred).  You initialize the semaphore to a
*                            non-zero value to specify how many resources are available (e.g. if you have
*                            10 resources, you would initialize the semaphore to 10).
*
* Returns    : WM_OS_STATUS_SUCCESS The call was successful
*   WM_OS_STATUS_ERROR
*********************************************************************************************************
*/
#if (1 == configUSE_COUNTING_SEMAPHORES)
wm_os_status_t wm_os_internal_sem_create(wm_os_sem_t **sem, uint32_t cnt)
{
    wm_os_status_t os_status;

    *sem = xSemaphoreCreateCounting(configSEMAPHORE_MAX_VALUE, cnt);

    if (*sem != NULL)
        os_status = WM_OS_STATUS_SUCCESS;
    else
        os_status = WM_OS_STATUS_ERROR;

    return os_status;
}

/*
*********************************************************************************************************
*                                         RESET A SEMAPHORE
*
* Description: This function reset a semaphore.
*
* Arguments  : sem        is a pointer to the event control block associated with the desired
*                            semaphore.
*
* Returns    : always WM_HAL_SUCCESS
*
* Note       : FreeRTOS does not support xQueueResetFromISR, a workaround solution try to acquire the semphore
               UNTIL it returns ERROR
*********************************************************************************************************
*/
wm_os_status_t wm_os_internal_sem_reset(wm_os_sem_t *sem)
{
    wm_os_status_t os_status;

    while (1) {
        os_status = wm_os_internal_sem_acquire(sem, 0);
        if (os_status != WM_OS_STATUS_SUCCESS) {
            break;
        }
    }

    return WM_OS_STATUS_SUCCESS;
}

/*
*********************************************************************************************************
*                                         DELETE A SEMAPHORE
*
* Description: This function deletes a semaphore and readies all tasks pending on the semaphore.
*
* Arguments  : sem        is a pointer to the event control block associated with the desired
*                            semaphore.
*
* Returns    : always WM_OS_STATUS_SUCCESS
*
*********************************************************************************************************
*/
wm_os_status_t wm_os_internal_sem_delete(wm_os_sem_t *sem)
{
    vSemaphoreDelete((QueueHandle_t)sem);

    return WM_OS_STATUS_SUCCESS;
}

/*
*********************************************************************************************************
*                                           PEND ON SEMAPHORE
*
* Description: This function waits for a semaphore.
*
* Arguments  : sem        is a pointer to the event control block associated with the desired
*                            semaphore.
*
*              wait_time       is an optional timeout period (in clock ticks).  If non-zero, your task will
*                            wait for the resource up to the amount of time specified by this argument.
*                            If you specify WM_OS_WAIT_TIME_MAX, however, your task will wait forever at the specified
*                            semaphore or, until the resource becomes available (or the event occurs).
*
* Returns    : WM_OS_STATUS_SUCCESS
*   WM_OS_STATUS_ERROR
*********************************************************************************************************
*/
//该函数不可用于中断服务程序中
wm_os_status_t wm_os_internal_sem_acquire(wm_os_sem_t *sem, uint32_t wait_time)
{
    uint8_t error;
    wm_os_status_t os_status;
    unsigned int time;

    if (WM_OS_WAIT_TIME_MAX == wait_time)
        time = portMAX_DELAY;
    else
        time = wait_time;
    portBASE_TYPE pxHigherPriorityTaskWoken = pdFALSE;
    uint8_t isrcount                        = 0;

    isrcount = wm_os_internal_get_isr_count();
    if (isrcount > 0) {
        error = xSemaphoreTakeFromISR((QueueHandle_t)sem, &pxHigherPriorityTaskWoken);
        if ((pdTRUE == pxHigherPriorityTaskWoken) && (1 == isrcount)) {
            portYIELD_FROM_ISR(pxHigherPriorityTaskWoken);
        }
    } else {
        error = xSemaphoreTake((QueueHandle_t)sem, time);
    }
    if (error == pdPASS)
        os_status = WM_OS_STATUS_SUCCESS;
    else
        os_status = WM_OS_STATUS_ERROR;

    return os_status;
}

wm_os_status_t wm_os_internal_sem_acquire_ms(wm_os_sem_t *sem, uint32_t wait_time_ms)
{
    return wm_os_internal_sem_acquire(sem, WM_OS_WAIT_TIME_MAX == wait_time_ms ? wait_time_ms : pdMS_TO_TICKS(wait_time_ms));
}

uint16_t wm_os_internal_sem_get_count(wm_os_sem_t *sem)
{
    return (uint16_t)uxSemaphoreGetCount((QueueHandle_t)sem);
}

/*
*********************************************************************************************************
*                                         POST TO A SEMAPHORE
*
* Description: This function signals a semaphore
*
* Arguments  : sem        is a pointer to the event control block associated with the desired
*                            semaphore.
*
* Returns    : WM_OS_STATUS_SUCCESS
*   WM_OS_STATUS_ERROR
*********************************************************************************************************
*/
wm_os_status_t wm_os_internal_sem_release(wm_os_sem_t *sem)
{
    uint8_t error;
    wm_os_status_t os_status;
    portBASE_TYPE pxHigherPriorityTaskWoken = pdFALSE;
    uint8_t isrcount                        = 0;

    isrcount = wm_os_internal_get_isr_count();
    if (isrcount > 0) {
        error = xSemaphoreGiveFromISR((QueueHandle_t)sem, &pxHigherPriorityTaskWoken);
        if ((pdTRUE == pxHigherPriorityTaskWoken) && (1 == isrcount)) {
            portYIELD_FROM_ISR(pxHigherPriorityTaskWoken);
        }
    } else {
        error = xSemaphoreGive((QueueHandle_t)sem);
    }
    if (error == pdPASS)
        os_status = WM_OS_STATUS_SUCCESS;
    else
        os_status = WM_OS_STATUS_ERROR;

    return os_status;
}
#endif

// ========================================================================= //
// Message Passing            //
// ========================================================================= //

/*
*********************************************************************************************************
*                                        CREATE A MESSAGE QUEUE
*
* Description: This function creates a message queue if free event control blocks are available.
*
* Arguments  : queue     is a pointer to the event control clock (OS_EVENT) associated with the
*                           created queue
*
*             item_size     size of each element
*             queue_size    is the number of elements in the storage area
*
*
* Returns    : WM_OS_STATUS_SUCCESS
*   WM_OS_STATUS_ERROR
*********************************************************************************************************
*/
wm_os_status_t wm_os_internal_queue_create_ex(wm_os_queue_t **queue, uint32_t item_size, uint32_t queue_size)
{
    wm_os_status_t os_status;
    uint32_t queuesize          = 10;
    void *queue_start           = NULL;
    StaticQueue_t *xStaticQueue = NULL;

    if (queue_size) {
        queuesize = queue_size;
    }
    xStaticQueue = wm_os_internal_malloc(sizeof(StaticQueue_t) + queuesize * (item_size));
    if (xStaticQueue == NULL) {
        return WM_OS_STATUS_ERROR;
    }
    queue_start = (void *)(((char *)xStaticQueue) + sizeof(StaticQueue_t));

    *queue = xQueueCreateStatic(queuesize, item_size, queue_start, xStaticQueue);

    if (*queue != NULL)
        os_status = WM_OS_STATUS_SUCCESS;
    else
        os_status = WM_OS_STATUS_ERROR;

    return os_status;
}

/*
*********************************************************************************************************
*                                        CREATE A MESSAGE QUEUE
*
* Description: This function creates a message queue if free event control blocks are available.
*
* Arguments  : queue     is a pointer to the event control clock (OS_EVENT) associated with the
*                           created queue
*
*
*             queue_size    is the number of elements in the storage area
*
*
* Returns    : WM_OS_STATUS_SUCCESS
*   WM_OS_STATUS_ERROR
*********************************************************************************************************
*/
wm_os_status_t wm_os_internal_queue_create(wm_os_queue_t **queue, uint32_t queue_size)
{
    return wm_os_internal_queue_create_ex(queue, sizeof(void *), queue_size);
}

/*
*********************************************************************************************************
*                                        DELETE A MESSAGE QUEUE
*
* Description: This function deletes a message queue and readies all tasks pending on the queue.
*
* Arguments  : queue        is a pointer to the event control block associated with the desired
*                            queue.
*
*
* Returns    : WM_OS_STATUS_SUCCESS
*   WM_OS_STATUS_ERROR
*********************************************************************************************************
*/
wm_os_status_t wm_os_internal_queue_delete(wm_os_queue_t *queue)
{
    if (queue != NULL) {
        vQueueDelete((QueueHandle_t)queue);
        wm_os_internal_free((QueueHandle_t)queue);
    }
    return WM_OS_STATUS_SUCCESS;
}
/*
*********************************************************************************************************
*                                        QUERY MESSAGE QUEUE EMPTY OR NOT
*
* Description: This function querys a message queue is empty or not.
*
* Arguments  : queue        is a pointer to the event control block associated with the desired
*                            queue.
*
*
* Returns    : 1 the message queue is empty;
*      0 otherwise;
*********************************************************************************************************
*/

uint8_t wm_os_internal_queue_is_empty(wm_os_queue_t *queue)
{
    uint8_t empty;

    vPortEnterCritical();
    empty = xQueueIsQueueEmptyFromISR((QueueHandle_t)queue);
    vPortExitCritical();

    return empty;
}

/*
*********************************************************************************************************
*                                        POST MESSAGE TO A QUEUE
*
* Description: This function sends a message to the back of a queue
*
* Arguments  : queue        is a pointer to the event control block associated with the desired queue
*
*              msg          is a pointer to the message to send.
*
* Returns    : WM_OS_STATUS_SUCCESS
*      WM_OS_STATUS_ERROR
*********************************************************************************************************
*/
wm_os_status_t wm_os_internal_queue_send(wm_os_queue_t *queue, void *msg)
{
    uint8_t error;
    wm_os_status_t os_status;
    portBASE_TYPE pxHigherPriorityTaskWoken = pdFALSE;
    uint8_t isrcount                        = 0;

    isrcount = wm_os_internal_get_isr_count();
    if (isrcount > 0) {
        error = xQueueSendFromISR((QueueHandle_t)queue, &msg, &pxHigherPriorityTaskWoken);
        if ((pdTRUE == pxHigherPriorityTaskWoken) && (1 == isrcount)) {
            portYIELD_FROM_ISR(pxHigherPriorityTaskWoken);
        }
    } else {
        error = xQueueSend((QueueHandle_t)queue, &msg, 0);
    }

    if (error == pdPASS)
        os_status = WM_OS_STATUS_SUCCESS;
    else {
        os_status = WM_OS_STATUS_ERROR;
    }

    return os_status;
}

wm_os_status_t wm_os_internal_queue_forever_send(wm_os_queue_t *queue, void *msg)
{
    uint8_t error;
    wm_os_status_t os_status;
    portBASE_TYPE pxHigherPriorityTaskWoken = pdFALSE;
    uint8_t isrcount                        = 0;

    isrcount = wm_os_internal_get_isr_count();
    if (isrcount > 0) {
        error = xQueueSendFromISR((QueueHandle_t)queue, &msg, &pxHigherPriorityTaskWoken);
        if ((pdTRUE == pxHigherPriorityTaskWoken) && (1 == isrcount)) {
            portYIELD_FROM_ISR(pxHigherPriorityTaskWoken);
        }
    } else {
        error = xQueueSend((QueueHandle_t)queue, &msg, WM_OS_WAIT_TIME_MAX);
    }

    if (error == pdPASS)
        os_status = WM_OS_STATUS_SUCCESS;
    else {
        os_status = WM_OS_STATUS_ERROR;
    }

    return os_status;
}

/*
*********************************************************************************************************
*                                        POST MESSAGE TO A QUEUE
*
* Description: This function sends a message to the back of a queue
*
* Arguments  : queue        is a pointer to the event control block associated with the desired queue
*
*              msg          is a pointer to the message to send. It will copy the size specified in
                            wm_os_internal_queue_create_ex
*
* Returns    : WM_OS_STATUS_SUCCESS
*      WM_OS_STATUS_ERROR
*********************************************************************************************************
*/
wm_os_status_t wm_os_internal_queue_send_ex(wm_os_queue_t *queue, void *msg)
{
    uint8_t error;
    wm_os_status_t os_status;
    portBASE_TYPE pxHigherPriorityTaskWoken = pdFALSE;
    uint8_t isrcount                        = 0;

    isrcount = wm_os_internal_get_isr_count();
    if (isrcount > 0) {
        error = xQueueSendFromISR((QueueHandle_t)queue, msg, &pxHigherPriorityTaskWoken);
        if ((pdTRUE == pxHigherPriorityTaskWoken) && (1 == isrcount)) {
            portYIELD_FROM_ISR(pxHigherPriorityTaskWoken);
        }
    } else {
        error = xQueueSend((QueueHandle_t)queue, msg, 0);
    }

    if (error == pdPASS)
        os_status = WM_OS_STATUS_SUCCESS;
    else {
        os_status = WM_OS_STATUS_ERROR;
    }

    return os_status;
}

/*
*********************************************************************************************************
*                                        POST MESSAGE TO THE BACK OF A QUEUE
*
* Description: This function sends a message to the tail of a queue
*
* Arguments  : queue        is a pointer to the queue control block associated with the desired queue
*
*              msg          is a pointer to the message to send.
*
* Returns    : WM_OS_STATUS_SUCCESS
*      WM_OS_STATUS_ERROR
*********************************************************************************************************
*/
wm_os_status_t wm_os_internal_queue_send_to_back(wm_os_queue_t *queue, void *msg)
{
    uint8_t error;
    wm_os_status_t os_status;
    portBASE_TYPE pxHigherPriorityTaskWoken = pdFALSE;
    uint8_t isrcount                        = 0;

    isrcount = wm_os_internal_get_isr_count();
    if (isrcount > 0) {
        error = xQueueSendToBackFromISR((QueueHandle_t)queue, &msg, &pxHigherPriorityTaskWoken);
        if ((pdTRUE == pxHigherPriorityTaskWoken) && (1 == isrcount)) {
            portYIELD_FROM_ISR(pxHigherPriorityTaskWoken);
        }
    } else {
        error = xQueueSendToBack((QueueHandle_t)queue, &msg, 0);
    }

    if (error == pdPASS)
        os_status = WM_OS_STATUS_SUCCESS;
    else {
        os_status = WM_OS_STATUS_ERROR;
    }

    return os_status;
}

/*
*********************************************************************************************************
*                                        POST MESSAGE TO THE FRONT OF A QUEUE
*
* Description: This function sends a message to the head of a queue
*
* Arguments  : queue        is a pointer to the queue control block associated with the desired queue
*
*              msg          is a pointer to the message to send.
*
* Returns    : WM_OS_STATUS_SUCCESS
*           WM_OS_STATUS_ERROR
*********************************************************************************************************
*/
wm_os_status_t wm_os_internal_queue_send_to_front(wm_os_queue_t *queue, void *msg)
{
    uint8_t error;
    wm_os_status_t os_status;
    portBASE_TYPE pxHigherPriorityTaskWoken = pdFALSE;
    uint8_t isrcount                        = 0;

    isrcount = wm_os_internal_get_isr_count();
    if (isrcount > 0) {
        error = xQueueSendToFrontFromISR((QueueHandle_t)queue, &msg, &pxHigherPriorityTaskWoken);
        if ((pdTRUE == pxHigherPriorityTaskWoken) && (1 == isrcount)) {
            portYIELD_FROM_ISR(pxHigherPriorityTaskWoken);
        }
    } else {
        error = xQueueSendToFront((QueueHandle_t)queue, &msg, 0);
    }

    if (error == pdPASS)
        os_status = WM_OS_STATUS_SUCCESS;
    else {
        os_status = WM_OS_STATUS_ERROR;
    }

    return os_status;
}
/*
*********************************************************************************************************
*                                        QUERY QUEUE AVAILABLE SPACE
*
* Description: This function is used to retrieve the number of free spaces available in a queue.
* This is equal to the number of items that can be sent to the queue before the queue becomes full
* if no items are removed.
*
* @param xQueue A handle to the queue being queried.
*
* @return The number of spaces available in the queue.
*
*********************************************************************************************************
*/
uint32_t wm_os_internal_queue_space_available(wm_os_queue_t *queue)
{
    return (uint32_t)uxQueueSpacesAvailable(queue);
}

/*
*********************************************************************************************************
*                                        REMOVE A ELEMENT FROM THE QUEUE
*
* Description: This function is called to remove a element from the queue
*
* Arguments  : queue        is a pointer to the queue control block associated with the desired queue
*
*               msg         is a pointer to the element to be removed.
*
* Returns    : WM_OS_STATUS_SUCCESS
*           WM_OS_STATUS_ERROR
*********************************************************************************************************
*/

wm_os_status_t wm_os_internal_queue_remove(wm_os_queue_t *queue, void *msg)
{
    void *tmp_ev;
    BaseType_t ret;
    int i;
    int count;
    BaseType_t woken, woken2;
    /*
     * XXX We cannot extract element from inside FreeRTOS queue so as a quick
     * workaround we'll just remove all elements and add them back except the
     * one we need to remove. This is silly, but works for now - we probably
     * better use counting semaphore with os_queue to handle this in future.
     */

    if (wm_os_internal_get_isr_count() > 0) {
        woken = pdFALSE;

        count = uxQueueMessagesWaitingFromISR((QueueHandle_t)queue);
        for (i = 0; i < count; i++) {
            ret = xQueueReceiveFromISR((QueueHandle_t)queue, &tmp_ev, &woken2);
            if (ret != pdPASS)
                return WM_OS_STATUS_ERROR;
            woken |= woken2;

            if (tmp_ev == msg) {
                continue;
            }

            ret = xQueueSendToBackFromISR((QueueHandle_t)queue, &tmp_ev, &woken2);
            if (ret != pdPASS)
                return WM_OS_STATUS_ERROR;
            woken |= woken2;
        }

        portYIELD_FROM_ISR(woken);
    } else {
        vPortEnterCritical();

        count = uxQueueMessagesWaiting((QueueHandle_t)queue);
        for (i = 0; i < count; i++) {
            ret = xQueueReceive((QueueHandle_t)queue, &tmp_ev, 0);
            if (ret != pdPASS)
                return WM_OS_STATUS_ERROR;

            if (tmp_ev == msg) {
                continue;
            }

            ret = xQueueSendToBack((QueueHandle_t)queue, &tmp_ev, 0);
            if (ret != pdPASS)
                return WM_OS_STATUS_ERROR;
        }

        vPortExitCritical();
    }
    return 0;
}

/*
*********************************************************************************************************
*                                     PEND ON A QUEUE FOR A MESSAGE
*
* Description: This function waits for a message to be sent to a queue
*
* Arguments  : queue        is a pointer to the queue handle associated with the desired queue
*
*      msg  is a pointer to the message received
*
*              wait_time     is an optional timeout period (in clock ticks).  If non-zero, your task will
*                            wait for a message to arrive at the queue up to the amount of time
*                            specified by this argument.  If you specify WM_OS_WAIT_TIME_MAX, however, your task will wait
*                            forever at the specified queue or, until a message arrives.
*
* Returns    : WM_OS_STATUS_SUCCESS
*      WM_OS_STATUS_ERROR
*********************************************************************************************************
*/
wm_os_status_t wm_os_internal_queue_receive(wm_os_queue_t *queue, void **msg, uint32_t wait_time)
{
    uint8_t error;
    wm_os_status_t os_status;
    unsigned int xTicksToWait;
    portBASE_TYPE pxHigherPriorityTaskWoken = pdFALSE;
    uint8_t isrcount                        = 0;

    if (WM_OS_WAIT_TIME_MAX == wait_time)
        xTicksToWait = portMAX_DELAY;
    else
        xTicksToWait = wait_time;
    isrcount = wm_os_internal_get_isr_count();
    if (isrcount > 0) {
        error = xQueueReceiveFromISR((QueueHandle_t)queue, msg, &pxHigherPriorityTaskWoken);
        if ((pdTRUE == pxHigherPriorityTaskWoken) && (1 == isrcount)) {
            portYIELD_FROM_ISR(pxHigherPriorityTaskWoken);
        }
    } else {
        error = xQueueReceive((QueueHandle_t)queue, msg, xTicksToWait);
    }

    if (error == pdPASS)
        os_status = WM_OS_STATUS_SUCCESS;
    else
        os_status = WM_OS_STATUS_ERROR;

    return os_status;
}

/*
*********************************************************************************************************
*                                     PEND ON A QUEUE FOR A MESSAGE
*
* Description: This function is a simplified encapsulation function wm_os_internal_queue_receive
*              and the only exception is wait_time_ms is in millisecond unit
*
* Returns    : WM_OS_STATUS_SUCCESS
*      WM_OS_STATUS_ERROR
*********************************************************************************************************
*/

wm_os_status_t wm_os_internal_queue_receive_ms(wm_os_queue_t *queue, void **msg, uint32_t wait_time_ms)
{
    return wm_os_internal_queue_receive(queue, msg,
                                        WM_OS_WAIT_TIME_MAX == wait_time_ms ? wait_time_ms : pdMS_TO_TICKS(wait_time_ms));
}

/*
*********************************************************************************************************
*                                     PEEK A ELEMENT FROM A QUEUE
* Description:  This function receive an item from a queue without removing the item from the queue.
*               The item is received by copy so a buffer of adequate size must be
*               provided.  The number of bytes copied into the buffer was defined when
*               the queue was created.
*
* Arguments  : queue        is a pointer to the queue handle associated with the desired queue
*
*      msg  is a pointer to the message received
*
*              wait_time     is an optional timeout period (in clock ticks).  If non-zero, your task will
*                            wait for a message to arrive at the queue up to the amount of time
*                            specified by this argument.  If you specify WM_OS_WAIT_TIME_MAX, however, your task will wait
*                            forever at the specified queue or, until a message arrives.

* Successfully received items remain on the queue so will be returned again
* by the next call, or a call to xQueueReceive().
* Returns    : WM_OS_STATUS_SUCCESS
*      WM_OS_STATUS_ERROR
*********************************************************************************************************
*/

wm_os_status_t wm_os_internal_queue_peek(wm_os_queue_t *queue, void **msg, uint32_t wait_time)
{
    uint8_t error;
    wm_os_status_t os_status;
    unsigned int xTicksToWait;
    uint8_t isrcount = 0;

    if (WM_OS_WAIT_TIME_MAX == wait_time)
        xTicksToWait = portMAX_DELAY;
    else
        xTicksToWait = wait_time;
    isrcount = wm_os_internal_get_isr_count();
    if (isrcount > 0) {
        error = xQueuePeekFromISR((QueueHandle_t)queue, msg);
    } else {
        error = xQueuePeek((QueueHandle_t)queue, msg, xTicksToWait);
    }

    if (error == pdPASS)
        os_status = WM_OS_STATUS_SUCCESS;
    else
        os_status = WM_OS_STATUS_ERROR;

    return os_status;
}

/*
*********************************************************************************************************
*                                             FLUSH QUEUE
*
* Description : This function is used to flush the contents of the message queue.
*
* Arguments  : queue        is a pointer to the queue handle associated with the desired queue
*
* Returns    : WM_OS_STATUS_SUCCESS
*      WM_OS_STATUS_ERROR
*At present, no use for freeRTOS
*********************************************************************************************************
*/
wm_os_status_t wm_os_internal_queue_flush(wm_os_queue_t *queue)
{
    return WM_OS_STATUS_SUCCESS;
}

/*
*********************************************************************************************************
*                                        CREATE A MESSAGE MAILBOX
*
* Description: This function creates a message mailbox if free event control blocks are available.
*
* Arguments  : mailbox  is a pointer to the event control clock (OS_EVENT) associated with the
*                                created mailbox
*
*   mailbox_start          is a pointer to a message that you wish to deposit in the mailbox.  If
*                            you set this value to the NULL pointer (i.e. (void *)0) then the mailbox
*                            will be considered empty.
*
*   mailbox_size
*
*   msg_size
*
Returns    : WM_OS_STATUS_SUCCESS
*   WM_OS_STATUS_ERROR
*********************************************************************************************************
*/
#if (1 == configUSE_MAILBOX)
wm_os_status_t wm_os_internal_mailbox_create(wm_os_mailbox_t **mailbox, uint32_t mailbox_size)
{
    wm_os_status_t os_status;
    uint32_t mbox_size = 1;

    if (mailbox_size) {
        mbox_size = mailbox_size;
    }

    *mailbox = xQueueCreate(mbox_size, sizeof(void *));

    if (*mailbox != NULL)
        os_status = WM_OS_STATUS_SUCCESS;
    else
        os_status = WM_OS_STATUS_ERROR;

    return os_status;
}

/*
*********************************************************************************************************
*                                         DELETE A MAIBOX
*
* Description: This function deletes a mailbox and readies all tasks pending on the mailbox.
*
* Arguments  : mailbox        is a pointer to the event control block associated with the desired
*                            mailbox.
*
*
Returns    : WM_OS_STATUS_SUCCESS
*   WM_OS_STATUS_ERROR
*********************************************************************************************************
*/
wm_os_status_t wm_os_internal_mailbox_delete(wm_os_mailbox_t *mailbox)
{
    vQueueDelete((QueueHandle_t)mailbox);

    return WM_OS_STATUS_SUCCESS;
}

/*
*********************************************************************************************************
*                                       POST MESSAGE TO A MAILBOX
*
* Description: This function sends a message to a mailbox
*
* Arguments  : mailbox        is a pointer to the event control block associated with the desired mailbox
*
*              msg          is a pointer to the message to send.  You MUST NOT send a NULL pointer.
*
Returns    : WM_OS_STATUS_SUCCESS
*   WM_OS_STATUS_ERROR
*********************************************************************************************************
*/
wm_os_status_t wm_os_internal_mailbox_send(wm_os_mailbox_t *mailbox, void *msg)
{
    uint8_t error;
    wm_os_status_t os_status;
    portBASE_TYPE pxHigherPriorityTaskWoken = pdFALSE;
    uint8_t isrcount                        = 0;

    isrcount = wm_os_internal_get_isr_count();
    if (isrcount > 0) {
        error = xQueueSendFromISR((QueueHandle_t)mailbox, &msg, &pxHigherPriorityTaskWoken);
        if ((pdTRUE == pxHigherPriorityTaskWoken) && (1 == isrcount)) {
            vTaskSwitchContext();
        }
    } else {
        error = xQueueSend((QueueHandle_t)mailbox, &msg, 0);
    }

    if (error == pdPASS)
        os_status = WM_OS_STATUS_SUCCESS;
    else {
        os_status = WM_OS_STATUS_ERROR;
    }

    return os_status;
}

/*
*********************************************************************************************************
*                                      PEND ON MAILBOX FOR A MESSAGE
*
* Description: This function waits for a message to be sent to a mailbox
*
* Arguments  : mailbox        is a pointer to the event control block associated with the desired mailbox
*
*      msg   is a pointer to the message received
*
*              wait_time       is an optional timeout period (in clock ticks).  If non-zero, your task will
*                            wait for a message to arrive at the mailbox up to the amount of time
*                            specified by this argument.  If you specify WM_OS_WAIT_TIME_MAX, however, your task will wait
*                            forever at the specified mailbox or, until a message arrives.
*
*Returns    : WM_OS_STATUS_SUCCESS
*   WM_OS_STATUS_ERROR
*********************************************************************************************************
*/
wm_os_status_t wm_os_internal_mailbox_receive(wm_os_mailbox_t *mailbox, void **msg, uint32_t wait_time)
{
    uint8_t error;
    wm_os_status_t os_status;
    unsigned int xTicksToWait;
    portBASE_TYPE pxHigherPriorityTaskWoken = pdFALSE;
    uint8_t isrcount                        = 0;

    if (WM_OS_WAIT_TIME_MAX == wait_time)
        xTicksToWait = portMAX_DELAY;
    else
        xTicksToWait = wait_time;

    isrcount = wm_os_internal_get_isr_count();
    if (isrcount > 0) {
        error = xQueueReceiveFromISR((QueueHandle_t)mailbox, msg, &pxHigherPriorityTaskWoken);
        if ((pdTRUE == pxHigherPriorityTaskWoken) && (1 == isrcount)) {
            portYIELD_FROM_ISR(pxHigherPriorityTaskWoken);
        }
    } else {
        error = xQueueReceive((QueueHandle_t)mailbox, msg, xTicksToWait);
    }

    if (error == pdPASS)
        os_status = WM_OS_STATUS_SUCCESS;
    else
        os_status = WM_OS_STATUS_ERROR;

    return os_status;
}
#endif

/*
*********************************************************************************************************
*                                           CREATE AN EVENT GROUP
*
* Description: This function creates an event group.
*
* Arguments  :event   is a pointer to the event handle
*
* Returns    : WM_OS_STATUS_SUCCESS If the event group was created
*              WM_OS_STATUS_ERROR   otherwise;
*********************************************************************************************************
*/

wm_os_status_t wm_os_internal_event_create(wm_os_event_t **event)
{
    EventGroupHandle_t eh = xEventGroupCreate();

    if (eh) {
        *event = eh;
        return WM_OS_STATUS_SUCCESS;
    } else {
        return WM_OS_STATUS_ERROR;
    }
}

/*
*********************************************************************************************************
*                                           DELETE AN EVENT GROUP
*
* Description: Delete an event group that was previously created by a call to
* wm_os_internal_event_create().  Tasks that are blocked on the event group will be
* unblocked and obtain 0 as the event group's value.
*
* Arguments  :event   is a pointer to the event handle
*
* Returns    : Always success.
*********************************************************************************************************
*/
wm_os_status_t wm_os_internal_event_delete(wm_os_event_t *event)
{
    if (event) {
        vEventGroupDelete(event);
    }

    return WM_OS_STATUS_SUCCESS;
}

/*
*********************************************************************************************************
*                                           RETRIEVE AN EVENT SETS FROM THE EVENT GROUP
*
* Description:This function waits for one or more bits to be set within a previously created event group.
*
* Arguments  :event   is a pointer to the event handle
*             wait_event  A bitwise value that indicates the bit or bits to test
*                        inside the event group.  For example, to wait for bit 0 and/or bit 2 set
*                        wait_event to 0x05.  To wait for bits 0 and/or bit 1 and/or bit 2 set
*                        wait_event to 0x07.  Etc.
*                        wait_event that are set within the event group will be cleared before
*                        xEventGroupWaitBits() returns if the wait condition was met (if the function
*                        returns for a reason other than a timeout).
*             return_event A bitwise value returned when the event group wait condition was met;
*             equal     If equal is set to true then xEventGroupWaitBits() will return when either all the bits in wait_event
*                       are set or the specified block time expires.  If equal is set to
*                       false then xEventGroupWaitBits() will return when any one of the bits set
*                       in wait_event is set or the specified block time expires.
*             wait_time The maximum amount of time (specified in 'ticks') to wait
*                       for one/all (depending on the xWaitForAllBits value) of the bits specified by
*                       wait_event to become set.
*
* Returns    : Always success.
*********************************************************************************************************
*/
wm_os_status_t wm_os_internal_event_get(wm_os_event_t *event, uint32_t wait_event, uint32_t *return_event, bool equal,
                                        uint32_t wait_time)
{
    EventBits_t evbs;
    uint8_t isrcount = 0;

    isrcount = wm_os_internal_get_isr_count();
    if (isrcount > 0) {
        evbs = xEventGroupGetBitsFromISR(event);
    } else {
        evbs = xEventGroupWaitBits(event, wait_event, pdTRUE, equal ? pdTRUE : pdFALSE, wait_time);
    }

    if (return_event)
        *return_event = evbs;

    return WM_OS_STATUS_SUCCESS;
}

/*
*********************************************************************************************************
*                                           PUT AN EVENT SET INTO THE EVENT GROUP
*
* Description: Set bits within an event group.
*
*              Setting bits in an event group will automatically unblock tasks that are
*              blocked waiting for the bits.
*
* Arguments  :event     is a pointer to the event handle.
*
*             event_bits   A bitwise value that indicates the bit or bits to set.
*                          For example, to set bit 3 only, set uxBitsToSet to 0x08.  To set bit 3
*                          and bit 0 set uxBitsToSet to 0x20.
*
* Returns    : Always success.
*********************************************************************************************************
*/
wm_os_status_t wm_os_internal_event_put(wm_os_event_t *event, uint32_t event_bits)
{
    uint8_t isrcount                    = 0;
    BaseType_t xHigherPriorityTaskWoken = pdFALSE, xResult;

    isrcount = wm_os_internal_get_isr_count();

    if (isrcount > 0) {
        xResult = xEventGroupSetBitsFromISR(event, event_bits, &xHigherPriorityTaskWoken);
        if ((pdPASS == xResult) && (1 == isrcount)) {
            portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
        }
    } else {
        xEventGroupSetBits(event, event_bits);
    }

    return WM_OS_STATUS_SUCCESS;
}

/*
*********************************************************************************************************
*                                           RETRIEVT EVENT SET FROM THE EVENT GROUP
*
* Description: Returns the current value of the bits in an event group.
*
* Arguments  :event   is a pointer to the event handle.
*
*             event_bits A bitwise value that indicates the bit or bits to set in the current event group.
*
* Returns    : Always success.
*********************************************************************************************************
*/
wm_os_status_t wm_os_internal_event_get_bits(wm_os_event_t *event, uint32_t *event_bits)
{
    uint8_t isrcount = 0;
    uint32_t curr_bits;

    isrcount = wm_os_internal_get_isr_count();
    if (isrcount > 0) {
        curr_bits = xEventGroupGetBitsFromISR(event);
    } else {
        curr_bits = xEventGroupGetBits(event);
    }

    if (event_bits) {
        *event_bits = curr_bits;
    }

    return WM_OS_STATUS_SUCCESS;
}
/*
*********************************************************************************************************
*                                           CLEAR EVENT SET FROM THE EVENT GROUP
*
* Description: Clear bits within an event group..
*
* Arguments  :event   is a pointer to the event handle.
*
*             event_bits A bitwise value that indicates the bit or bits to be cleared in the current event group.
*
* Returns    : Always success.
*********************************************************************************************************
*/
wm_os_status_t wm_os_internal_event_clear_bits(wm_os_event_t *event, uint32_t event_bits)
{
    uint8_t isrcount = 0;

    isrcount = wm_os_internal_get_isr_count();
    if (isrcount > 0) {
        xEventGroupClearBitsFromISR(event, event_bits);
    } else {
        xEventGroupClearBits(event, event_bits);
    }

    return WM_OS_STATUS_SUCCESS;
}

/*
*********************************************************************************************************
*                                         GET CURRENT SYSTEM TIME
*
* Description: This function is used by your application to obtain the current value of the 32-bit
*              counter which keeps track of the number of clock ticks.
*
* Arguments  : none
*
* Returns    : The current value of OSTime
*********************************************************************************************************
*/
uint32_t wm_os_internal_get_time(void)
{
    return xTaskGetTickCountFromISR();
}
/*
*********************************************************************************************************
*                                         GET CURRENT SYSTEM TIME IN MS UNIT
*
* Description: This function is used by your application to obtain the current value of the 32-bit
*              counter which keeps track of the number of millisecond.
*
* Arguments  : none
*
* Returns    : The current value of OSTime in millisecond unit
*********************************************************************************************************
*/
uint32_t wm_os_internal_get_time_ms(void)
{
    return xTaskGetTickCountFromISR() * portTICK_PERIOD_MS;
}

/**********************************************************************************************************
* Description: Disable interrupts by preserving the state of interrupts.
*
* Arguments  : none
*
* Returns    : none
***********************************************************************************************************/
void wm_os_internal_set_critical(void)
{
    vPortEnterCritical();
}

/**********************************************************************************************************
* Description: Enable interrupts by preserving the state of interrupts.
*
* Arguments  : none
*
* Returns    : none
***********************************************************************************************************/
void wm_os_internal_release_critical(void)
{
    vPortExitCritical();
}

typedef struct StaticTimerBuffer {
    StaticTimer_t xTimer;
    wm_os_timer_callback callback;
    void *callback_arg;
} StaticTimerBuffer_t;

static void prvTimerCallback(TimerHandle_t xExpiredTimer)
{
    StaticTimerBuffer_t *pTimer = (StaticTimerBuffer_t *)xExpiredTimer;
    //printf("pTimer %p, callback %p\n", pTimer, pTimer->callback);
    if (pTimer->callback) {
        pTimer->callback(xExpiredTimer, pTimer->callback_arg);
    }
}

/*
************************************************************************************************************************
*                                                   CREATE A TIMER
*
* Description: This function is called by your application code to create a timer.
*
* Arguments  : timer A pointer to an OS_TMR data structure.This is the 'handle' that your application
*      will use to reference the timer created.
*
*          callback      Is a pointer to a callback function that will be called when the timer expires.  The
*                               callback function must be declared as follows:
*                               void MyCallback (OS_TMR *ptmr, void *p_arg);
*
*              callback_arg  Is an argument (a pointer) that is passed to the callback function when it is called.
*
*               period        The 'period' being repeated for the timer.
*                               If you specified 'OS_TMR_OPT_PERIODIC' as an option, when the timer expires, it will
*                               automatically restart with the same period.
*
*       repeat       if repeat needed, it should TRUE.
*
*              pname         Is a pointer to an ASCII string that is used to name the timer.  Names are useful for
*                               debugging.
*
*Returns    : WM_OS_STATUS_SUCCESS
*   WM_OS_STATUS_ERROR
************************************************************************************************************************
*/
wm_os_status_t wm_os_internal_timer_create(wm_os_timer_t **timer, wm_os_timer_callback callback, void *callback_arg,
                                           uint32_t period, bool repeat, char *name)
{
    wm_os_status_t os_status;
    wm_os_timer_t *ptmr         = NULL;
    StaticTimerBuffer_t *pTimer = NULL;

    if (0 == period)
        period = 1;
#if configUSE_TIMERS
#if 0
 *timer = (TimerHandle_t)xTimerCreateExt( (signed char *)name, period, repeat, NULL, callback, callback_arg );
#else
    pTimer = wm_os_internal_malloc(sizeof(StaticTimerBuffer_t));
    if (pTimer == NULL) {
        return WM_OS_STATUS_ERROR;
    }
    memset(pTimer, 0, sizeof(StaticTimerBuffer_t));
    pTimer->callback     = callback;
    pTimer->callback_arg = callback_arg;
    ptmr                 = xTimerCreateStatic(
        (const char *const)name, /* Text name for the task.  Helps debugging only.  Not used by FreeRTOS. */
        period,                  /* The period of the timer in ticks. */
        repeat,                  /* This is an auto-reload timer. */
        (void *)NULL, /* The variable incremented by the test is passed into the timer callback using the timer ID. */
        prvTimerCallback, /* The function to execute when the timer expires. */
        &pTimer->xTimer); /* The buffer that will hold the software timer structure. */

#endif
#endif
    if (ptmr) {
        if (timer)
            *timer = ptmr;
        os_status = WM_OS_STATUS_SUCCESS;
    } else {
        wm_os_internal_free(pTimer);
        pTimer    = NULL;
        os_status = WM_OS_STATUS_ERROR;
    }

    return os_status;
}

wm_os_status_t wm_os_internal_timer_create_ms(wm_os_timer_t **timer, wm_os_timer_callback callback, void *callback_arg,
                                              uint32_t period_ms, bool repeat, char *name)
{
    return wm_os_internal_timer_create(timer, callback, callback_arg,
                                       WM_OS_WAIT_TIME_MAX == period_ms ? period_ms : pdMS_TO_TICKS(period_ms), repeat, name);
}

/*
************************************************************************************************************************
*                                                   START A TIMER
*
* Description: This function is called by your application code to start a timer.
*
* Arguments  : timer          Is a pointer to an OS_TMR
*
************************************************************************************************************************
*/
void wm_os_internal_timer_start(wm_os_timer_t *timer)
{
    portBASE_TYPE pxHigherPriorityTaskWoken = pdFALSE;
    uint8_t isrcount                        = 0;

    isrcount = wm_os_internal_get_isr_count();
    if (isrcount > 0) {
#if configUSE_TIMERS
        xTimerStartFromISR((TimerHandle_t)timer, &pxHigherPriorityTaskWoken);
#endif
        if ((pdTRUE == pxHigherPriorityTaskWoken) && (1 == isrcount)) {
            portYIELD_FROM_ISR(pxHigherPriorityTaskWoken);
        }
    } else {
#if configUSE_TIMERS
        xTimerStart((TimerHandle_t)timer, 0); //no block time
#endif
    }
}

/*
************************************************************************************************************************
*                                                   CHANGE A TIMER WAIT TIME
*
* Description: This function is called by your application code to change a timer wait time.
*
* Arguments  : timer          Is a pointer to an OS_TMR
*
*   ticks   is the wait time
************************************************************************************************************************
*/
void wm_os_internal_timer_change(wm_os_timer_t *timer, uint32_t ticks)
{
    portBASE_TYPE pxHigherPriorityTaskWoken = pdFALSE;
    uint8_t isrcount                        = 0;

    if (0 == ticks)
        ticks = 1;

    isrcount = wm_os_internal_get_isr_count();
    if (isrcount > 0) {
#if configUSE_TIMERS
        xTimerChangePeriodFromISR((TimerHandle_t)timer, ticks, &pxHigherPriorityTaskWoken);
        xTimerStartFromISR((TimerHandle_t)timer, &pxHigherPriorityTaskWoken);
#endif
        if ((pdTRUE == pxHigherPriorityTaskWoken) && (1 == isrcount)) {
            portYIELD_FROM_ISR(pxHigherPriorityTaskWoken);
        }
    } else {
#if configUSE_TIMERS
        xTimerChangePeriod((TimerHandle_t)timer, ticks, 0);
        xTimerStart((TimerHandle_t)timer, 0);
#endif
    }
}

/*
************************************************************************************************************************
*                                                   CHANGE A TIMER WAIT TIME
*
* Description: This function is called by your application code to change a timer wait time.
*
* Arguments  : timer          Is a pointer to an OS_TMR
*
*      ms     Is the wait time, unit: milliseconds
************************************************************************************************************************
*/
void wm_os_internal_timer_change_ms(wm_os_timer_t *timer, uint32_t ms)
{
    wm_os_internal_timer_change(timer, WM_OS_WAIT_TIME_MAX == ms ? ms : pdMS_TO_TICKS(ms));
}

/*
************************************************************************************************************************
*                                                   STOP A TIMER
*
* Description: This function is called by your application code to stop a timer.
*
* Arguments  : timer          Is a pointer to the timer to stop.
*
************************************************************************************************************************
*/
void wm_os_internal_timer_stop(wm_os_timer_t *timer)
{
    portBASE_TYPE pxHigherPriorityTaskWoken = pdFALSE;
    uint8_t isrcount                        = 0;

    isrcount = wm_os_internal_get_isr_count();
    if (isrcount > 0) {
#if configUSE_TIMERS
        xTimerStopFromISR((TimerHandle_t)timer, &pxHigherPriorityTaskWoken);
#endif
        if ((pdTRUE == pxHigherPriorityTaskWoken) && (1 == isrcount)) {
            portYIELD_FROM_ISR(pxHigherPriorityTaskWoken);
        }
    } else {
#if configUSE_TIMERS
        xTimerStop((TimerHandle_t)timer, 0);
#endif
    }
}

/*
************************************************************************************************************************
*                                                   RESET A TIMER
*
* Description: This function is called by your application code to reset a timer.
*
* Arguments  : timer          Is a pointer to the timer to stop.
*
************************************************************************************************************************
*/
void wm_os_internal_timer_reset(wm_os_timer_t *timer)
{
    portBASE_TYPE pxHigherPriorityTaskWoken = pdFALSE;
    uint8_t isrcount                        = 0;

    isrcount = wm_os_internal_get_isr_count();
    if (isrcount > 0) {
#if configUSE_TIMERS
        xTimerResetFromISR((TimerHandle_t)timer, &pxHigherPriorityTaskWoken);
#endif
        if ((pdTRUE == pxHigherPriorityTaskWoken) && (1 == isrcount)) {
            portYIELD_FROM_ISR(pxHigherPriorityTaskWoken);
        }
    } else {
#if configUSE_TIMERS
        xTimerReset((TimerHandle_t)timer, 0);
#endif
    }
}

/*
************************************************************************************************************************
*
* Description: Queries a timer to see if it is active or dormant.
*
* A timer will be dormant if:
*     1) It has been created but not started, or
*     2) It is an expired one-shot timer that has not been restarted.
*
* Timers are created in the dormant state.  The xTimerStart(), xTimerReset(),
* xTimerStartFromISR(), xTimerResetFromISR(), xTimerChangePeriod() and
* xTimerChangePeriodFromISR() API functions can all be used to transition a timer into the
* active state.
*
* Arguments  : timer  The timer being queried.
*
* @return 0 will be returned if the timer is dormant.  1 will be returned if the timer is active.
************************************************************************************************************************
*/
uint8_t wm_os_internal_timer_active(wm_os_timer_t *timer)
{
    return (uint8_t)xTimerIsTimerActive((TimerHandle_t)timer);
}

/*
************************************************************************************************************************
 *
 * Description:This function returns the time in ticks at which the timer will expire.  If this is less
 * than the current tick count then the expiry time has overflowed from the
 * current time.
 *
 * @param xTimer The handle of the timer being queried.
 *
 * @return If the timer is running then the time in ticks at which the timer
 * will next expire is returned.  If the timer is not running then the return
 * value is undefined.
 ************************************************************************************************************************
*/
uint32_t wm_os_internal_timer_expirytime(wm_os_timer_t *timer)
{
    return (uint32_t)xTimerGetExpiryTime((TimerHandle_t)timer);
}

/*
************************************************************************************************************************
*                                                   Delete A TIMER
*
* Description: This function is called by your application code to delete a timer.
*
* Arguments  : timer          Is a pointer to the timer to delete.
*
************************************************************************************************************************
*/
wm_os_status_t wm_os_internal_timer_delete(wm_os_timer_t *timer)
{
    int ret = 0;
    wm_os_status_t os_status;
    /* xTimer is already active - delete it. */
    ret = xTimerDelete((TimerHandle_t)timer, 10);
    if (pdPASS == ret)
        os_status = WM_OS_STATUS_SUCCESS;
    else
        os_status = WM_OS_STATUS_ERROR;

    if (timer) {
        /* free StaticTimerBuffer_t */
        wm_os_internal_free(timer);
    }

    return os_status;
}

/*
*********************************************************************************************************
*                                       DELAY TASK 'n' TICKS
*
* Description: This function is called to delay execution of the currently running task until the
*              specified number of system ticks expires.  This, of course, directly equates to delaying
*              the current task for some time to expire.  No delay will result If the specified delay is
*              0.  If the specified delay is greater than 0 then, a context switch will result.
*
* Arguments  : ticks     is the time delay that the task will be suspended in number of clock 'ticks'.
*                        Note that by specifying 0, the task will not be delayed.
*
* Returns    : none
*********************************************************************************************************
*/
void wm_os_internal_time_delay(uint32_t ticks)
{
    vTaskDelay(ticks);
}
/*
*********************************************************************************************************
*                                       DELAY TASK 'n' MS
*
* Description: This function is called to delay execution of the currently running task until the
*              specified number of millisecond expires.  This, of course, directly equates to delaying
*              the current task for some time to expire.  No delay will result If the specified delay is
*              0.  If the specified delay is greater than 0 then, a context switch will result.
*
* Arguments  : ticks     is the time delay that the task will be suspended in number of millisecond 'ms'.
*                        Note that by specifying 0, the task will not be delayed.
*
* Returns    : none
*********************************************************************************************************
*/

void wm_os_internal_time_delay_ms(uint32_t ms)
{
    vTaskDelay(pdMS_TO_TICKS(ms));
}

/*
*********************************************************************************************************
*                                       task stat info
*
* Description: This function is used to display stat info
* Arguments  :
*
* Returns    : none
*********************************************************************************************************
*/
void wm_os_internal_disp_task_stat_info(void)
{
    char *buf = NULL;

    buf = wm_os_internal_malloc(1024);
    if (NULL == buf)
        return;
#if configUSE_TRACE_FACILITY
    vTaskList((char *)buf);
#endif
    printf("\n%s", buf);
    wm_os_internal_free(buf);
    buf = NULL;
}

/*
*********************************************************************************************************
*                                       display sys runtime stats info
*
* Description: This function to display sys runtime stats info
* Arguments  : None
* Returns    : None
*********************************************************************************************************
*/
void wm_os_internal_disp_sys_runtime_stats_info(void)
{
#if CONFIG_FREERTOS_ENABLE_RUN_TIME_STATS
    char *buf = NULL;

    buf = wm_os_internal_malloc(2048);
    if (NULL == buf)
        return;
    memset(buf, 0x00, 2048);

    vTaskGetRunTimeStats(buf);
    printf("\n%s", buf);
    wm_os_internal_free(buf);
    buf = NULL;
#endif
}

/*
*********************************************************************************************************
*                                     OS INIT function
*
* Description: This function is used to init os common resource
*
* Arguments  : None;
*
* Returns    : None
*********************************************************************************************************
*/
void wm_os_internal_init(void *arg)
{
    wm_drv_irq_attach_hw_vector(WM_IRQ_SYS_TICK, coret_irq_handler);
    wm_drv_irq_enable(WM_IRQ_SYS_TICK);
}

/*
*********************************************************************************************************
*                                     OS scheduler start function
*
* Description: This function is used to start task schedule
*
* Arguments  : None;
*
* Returns    : None
*********************************************************************************************************
*/
void wm_os_internal_start_scheduler(void)
{
    vTaskStartScheduler();
}

/*
*********************************************************************************************************
*                                     Get OS TYPE
*
* Description: This function is used to get OS type
*
* Arguments  : None;
*
* Returns    : WM_OS_TYPE
*              WM_OS_UCOSII = 0,
*            wm_os_internal_freeRTOS = 1,
*********************************************************************************************************
*/
int wm_os_internal_get_type(void)
{
    return (int)wm_os_internal_freeRTOS;
}

/*
*********************************************************************************************************
*                                     OS tick handler
*
* Description: This function is  tick handler.
*
* Arguments  : None;
*
* Returns    : None
*********************************************************************************************************
*/
void wm_os_internal_time_tick(void *p)
{
}

static uint32_t CK_IN_INTRP(void)
{
    uint32_t vec = 0;
    asm volatile("mfcr    %0, psr \n"
                 "lsri    %0, 16\n"
                 "sextb   %0\n"
                 : "=r"(vec)
                 :);

    if (vec >= 32 || (vec == 10)) {
        return 1;
    } else {
        return 0;
    }
}

/**
 * @brief           get isr count
 *
 * @param[in]       None
 *
 * @retval          count
 *
 * @note            None
 */
//extern int portGET_IPSR(void);
uint8_t wm_os_internal_get_isr_count(void)
{
    //    return intr_counter;
    //return (portGET_IPSR() > 13);
    return (uint8_t)CK_IN_INTRP();
}

int csi_kernel_intrpt_enter(void)
{
    return 0;
}

int csi_kernel_intrpt_exit(void)
{
    portYIELD_FROM_ISR(pdTRUE);
    return 0;
}

#ifdef CONFIG_HEAP_USE_TRACING
void *wm_os_internal_calloc_tracing(size_t nelem, size_t elsize, const char *file, int line)
{
    void *ptr;
    size_t size;

    size = nelem * elsize;
    ptr  = wm_heap_caps_alloc_tracing(size, WM_HEAP_CAP_DEFAULT, file, line);
    if (ptr)
        memset(ptr, 0, size);

    return ptr;
}

#else  //CONFIG_HEAP_USE_TRACING

void *wm_os_internal_calloc(size_t nelem, size_t elsize)
{
    void *ptr;
    size_t size;

    size = nelem * elsize;
    ptr  = wm_heap_caps_alloc(size, WM_HEAP_CAP_DEFAULT);
    if (ptr)
        memset(ptr, 0, size);

    return ptr;
}

void *wm_os_internal_calloc_tracing(size_t nelem, size_t elsize, const char *file, int line)
{
    return wm_os_internal_calloc(nelem, elsize);
}
#endif //CONFIG_HEAP_USE_TRACING

void *malloc(size_t size)
{
    return wm_os_internal_malloc(size);
}

void free(void *ptr)
{
    wm_os_internal_free(ptr);
}

void *realloc(void *ptr, size_t size)
{
    return wm_os_internal_realloc(ptr, size);
}

void *calloc(size_t nelem, size_t elsize)
{
    return wm_os_internal_calloc(nelem, elsize);
}
