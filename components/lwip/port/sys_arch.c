#include "lwip/opt.h"
#include "lwip/debug.h"
#include "lwip/sys.h"
#include "lwip/stats.h"
#include "arch/sys_arch.h"

static sys_sem_t g_sys_arch_protect_lock = NULL;

#if LWIP_NETCONN_SEM_PER_THREAD

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "wm_types.h"

sys_sem_t *sys_lwip_netconn_thread_sem_get(void)
{
	sys_sem_t *sem = (sys_sem_t *)pvTaskSemGet(NULL);

	if (*sem == NULL)
	{
		if (sys_sem_new(sem, 0) != ERR_OK)
		{
			LWIP_ASSERT("invalid lwip_netconn_thread_sem!", (*sem != NULL));
		}

		vTaskSemSet(NULL, (void *)(*sem));
	}

	return sem;
}

void sys_lwip_netconn_thread_sem_del(void *tcb)
{
	sys_sem_t * sem = (sys_sem_t *)pvTaskSemGet(tcb);

	if (*sem != NULL)
	{
		sys_sem_free(sem);

		vTaskSemSet(tcb, NULL);
	}
}
#endif

/**
 * \brief Initialize the sys_arch layer.
 */
void sys_init(void)
{
    sys_sem_new(&g_sys_arch_protect_lock, 1);
}

#ifdef CONFIG_LWIP_ENABLE_DEINIT
void sys_deinit(void)
{
    sys_sem_free(&g_sys_arch_protect_lock);
}
#endif

u32_t sys_now(void)
{
	return xTaskGetTickCount() * portTICK_PERIOD_MS;
}

/**
 * \brief Creates and returns a new semaphore.
 *
 * \param sem Pointer to the semaphore.
 * \param count Initial state of the semaphore.
 *
 * \return ERR_OK for OK, other value indicates error.
 */
err_t sys_sem_new(sys_sem_t *sem, u8_t count)
{
    *sem = xSemaphoreCreateBinary();

    if (*sem == SYS_SEM_NULL) {
        LWIP_ASSERT("sys_sem_new: out of mem", 0);
        return ERR_MEM;
    }

    if (count == 1) {
        BaseType_t ret = xSemaphoreGive(*sem);
        LWIP_ASSERT("sys_sem_new: initial give failed", ret == pdTRUE);
        (void)ret;
    }

	return ERR_OK;
}

/**
 * \brief Frees a semaphore created by sys_sem_new.
 *
 * \param sem Pointer to the semaphore.
 */
void sys_sem_free(sys_sem_t *sem)
{
	vSemaphoreDelete(*sem);
    *sem = SYS_SEM_NULL;
}

/**
 * \brief Signals (or releases) a semaphore.
 *
 * \param sem Pointer to the semaphore.
 */
void sys_sem_signal(sys_sem_t *sem)
{
    BaseType_t ret = xSemaphoreGive(*sem);
    LWIP_ASSERT("sys_sem_signal: sane return value",
         (ret == pdTRUE) || (ret == errQUEUE_FULL));
    (void)ret;
}

/**
 * \brief Blocks the thread while waiting for the semaphore to be signaled.
 * Note that there is another function sys_sem_wait in sys.c, but it is a wrapper
 * for the sys_arch_sem_wait function. Please note that it is important for the
 * semaphores to return an accurate count of elapsed milliseconds, since they are
 * used to schedule timers in lwIP.
 *
 * \param sem Pointer to the semaphore.
 * \param timeout The timeout parameter specifies how many milliseconds the
 * function should block before returning; if the function times out, it should
 * return SYS_ARCH_TIMEOUT. If timeout=0, then the function should block
 * indefinitely. If the function acquires the semaphore, it should return how
 * many milliseconds expired while waiting for the semaphore. 
 *
 * \return SYS_ARCH_TIMEOUT if times out, ERR_MEM for semaphore erro otherwise
 * return the milliseconds expired while waiting for the semaphore.
 */
u32_t sys_arch_sem_wait(sys_sem_t *sem, u32_t timeout)
{
    BaseType_t ret;
    //u32_t tick_start, tick_stop, tick_elapsed;

    //tick_start = wm_os_internal_get_time();

    if (!timeout) {
        ret = xSemaphoreTake(*sem, portMAX_DELAY);
        LWIP_ASSERT("taking semaphore failed", ret == pdTRUE);
    } else {
        TickType_t timeout_ticks = timeout / portTICK_RATE_MS;
        //if(timeout && timeout_ticks == 0)
        //    timeout_ticks = 1;
        ret = xSemaphoreTake(*sem, timeout_ticks);
    if (ret == errQUEUE_EMPTY) {
        return SYS_ARCH_TIMEOUT;
    }
        LWIP_ASSERT("taking semaphore failed", ret == pdTRUE);
    }

#if 0
    tick_stop = wm_os_internal_get_time();
    // Take care of wrap-around.
    if( tick_stop >= tick_start )
        tick_elapsed = tick_stop - tick_start;
    else
        tick_elapsed = portMAX_DELAY - tick_start + tick_stop;

    return tick_elapsed * portTICK_RATE_MS;
#else
    return 0;
#endif
}

#ifndef sys_sem_valid
/**
 * \brief Check if a sempahore is valid/allocated.
 *
 * \param sem Pointer to the semaphore.
 *
 * \return Semaphore number on valid, 0 for invalid.
 */
int sys_sem_valid(sys_sem_t *sem)
{
	if (*sem == SYS_SEM_NULL)
		return 0;

	return 1;
}

#endif

#ifndef sys_sem_set_invalid
/**
 * \brief Set a semaphore invalid.
 *
 * \param sem Pointer to the semaphore.
 */
void sys_sem_set_invalid(sys_sem_t *sem)
{
	*sem = SYS_SEM_NULL;
}
#endif

/**
 * \brief Creates an empty mailbox for maximum "size" elements. Elements stored
 * in mailboxes are pointers. 
 *
 * \param mBoxNew Pointer to the new mailbox.
 * \param size Maximum "size" elements.
 *
 * \return ERR_OK if successfull or ERR_MEM on error.
 */
err_t sys_mbox_new(sys_mbox_t *mbox, int size)
{
    *mbox = xQueueCreate(size, sizeof(void *));

    if (*mbox == SYS_MBOX_NULL) {
        LWIP_ASSERT("fail to new *mbox", 0);
        return ERR_MEM;
    }

    return ERR_OK;
}

/**
 * \brief Deallocates a mailbox.
 * If there are messages still present in the mailbox when the mailbox is
 * deallocated, it is an indication of a programming error in lwIP and the
 * developer should be notified.
 *
 * \param mbox Pointer to the new mailbox.
 */
void sys_mbox_free(sys_mbox_t *mbox)
{
    LWIP_ASSERT( "sys_mbox_free ", *mbox != SYS_MBOX_NULL );      

	vQueueDelete(*mbox);
	*mbox = SYS_MBOX_NULL;
}

/**
 * \brief Posts the "msg" to the mailbox. This function have to block until the
 * "msg" is really posted.
 *
 * \param mbox Pointer to the mailbox.
 * \param msg Pointer to the message to be post.
 */
void sys_mbox_post(sys_mbox_t *mbox, void *msg)
{
    xQueueSendToBack(*mbox, &msg, portMAX_DELAY);
}

/**
 * \brief Try to posts the "msg" to the mailbox.
 *
 * \param mbox Pointer to the mailbox.
 * \param msg Pointer to the message to be post.
 *
 * \return ERR_MEM if the mailbox is full otherwise ERR_OK if the "msg" is posted.
 */
err_t sys_mbox_trypost(sys_mbox_t *mbox, void *msg)
{
    if (pdTRUE == xQueueSend(*mbox, &msg, 0))
        return ERR_OK;

    return ERR_MEM;    
}

/**
 * \brief Blocks the thread until a message arrives in the mailbox, but does not
 * block the thread longer than "timeout" milliseconds (similar to the
 * sys_arch_sem_wait() function).
 *
 * \param mbox Pointer to the mailbox.
 * \param msg A result parameter that is set by the function (i.e., by doing
 * "*msg = ptr"). The "msg" parameter maybe NULL to indicate that the message
 * should be dropped.
 * \timeout 0 indicates the thread should be blocked until a message arrives.
 *
 * \return Number of milliseconds spent waiting or SYS_ARCH_TIMEOUT if there was
 * a timeout. Or ERR_MEM if invalid pointer to message box.
 */
u32_t sys_arch_mbox_fetch(sys_mbox_t *mbox, void **msg, u32_t timeout)
{
    BaseType_t ret;
    void *msg_dummy;
    //u32_t tick_start, tick_stop, tick_elapsed;

    if (msg == NULL) {
        msg = &msg_dummy;
    }

    //tick_start = wm_os_internal_get_time();

    if (timeout == 0) {
        ret = xQueueReceive(*mbox, &(*msg), portMAX_DELAY);
        LWIP_ASSERT("mbox fetch failed", ret == pdTRUE);
    } else {
    TickType_t timeout_ticks = timeout / portTICK_RATE_MS;
    //if(timeout && timeout_ticks == 0)
    //    timeout_ticks = 1;
    ret = xQueueReceive(*mbox, &(*msg), timeout_ticks);
    if (ret == errQUEUE_EMPTY) {
        *msg = NULL;
        return SYS_ARCH_TIMEOUT;
    }
        LWIP_ASSERT("mbox fetch failed", ret == pdTRUE);
    }

#if 0
    tick_stop = wm_os_internal_get_time();
    // Take care of wrap-around.
    if( tick_stop >= tick_start )
        tick_elapsed = tick_stop - tick_start;
    else
        tick_elapsed = portMAX_DELAY - tick_start + tick_stop;

    return tick_elapsed * portTICK_RATE_MS;
#else
    return 0;
#endif
}

#ifndef sys_mbox_valid
/**
 * \brief Check if an mbox is valid/allocated.
 *
 * \param mbox Pointer to the mailbox.
 *
 * \return Mailbox for valid, 0 for invalid.
 */
int sys_mbox_valid(sys_mbox_t *mbox)
{
    if (*mbox == SYS_MBOX_NULL)
        return 0;
    else 
        return 1;
}
#endif

#ifndef sys_mbox_set_invalid
/**
 * \brief Set an mbox invalid.
 *
 * \param mbox Pointer to the mailbox.
 */
void sys_mbox_set_invalid(sys_mbox_t *mbox)
{
	*mbox = SYS_MBOX_NULL;
}

#endif
/**
 * \brief Instantiate a thread for lwIP. Both the id and the priority are
 * system dependent.
 *
 * \param name Pointer to the thread name.
 * \param thread Thread function.
 * \param arg Argument will be passed into the thread().
 * \param stacksize Stack size of the thread.
 * \param prio Thread priority.
 *
 * \return The id of the new thread.
 */
sys_thread_t sys_thread_new(const char *name, lwip_thread_fn thread, void *arg,
		int stacksize, int prio)
{
    wm_os_task_t task = NULL;

    wm_os_internal_task_create(&task, name, thread, arg, stacksize, prio, 0);

    return task;
}

/**
 * \brief Protect the system.
 *
 * \return 1 on success.
 */
sys_prot_t sys_arch_protect(void)
{
#if 1
    return !sys_arch_sem_wait(&g_sys_arch_protect_lock, 0);
#else
    return wm_os_internal_set_critical();
#endif
}

/**
 * \brief Unprotect the system.
 *
 * \param pval Protect value.
 */
void sys_arch_unprotect(sys_prot_t pval)
{
#if 1
    sys_sem_signal(&g_sys_arch_protect_lock);        
#else
    wm_os_internal_release_critical(pval);
#endif
}
