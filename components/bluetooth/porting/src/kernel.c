/*
 * Copyright (c) 2023 WinnerMicro
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>
#include <stdbool.h>
#include <kernel.h>
#include <assert.h>
#include "wm_osal.h"

#define LOG_TAG "bt_kernel"
#include "wm_log.h"

struct k_work_q k_sys_work_q;
static struct k_mutex k_sys_mutex;

#define __FILE__NAME__ONLY

#ifdef __FILE__NAME__ONLY
#define filename(x) strrchr(x, '/') ? strrchr(x, '/') + 1 : x
#else
#define filename(x) (x)
#endif

#define BTH_MAJOR_VER 0x0
#define BTH_MINOR_VER 0x1
#define BTH_PATCH_VER 0x0

const char BluetoothHostVer[4] = {
    'v', BTH_MAJOR_VER, /* Main version */
    BTH_MINOR_VER,      /* Subversion */
    BTH_PATCH_VER       /* Internal version */
};

k_tid_t k_thread_create(struct k_thread *new_thread, k_thread_stack_t *stack, size_t stack_size, k_thread_entry_t entry,
                        void *arg, int prio, uint32_t options, k_timeout_t delay)
{
    wm_os_status_t status;

    char buf[64] = { 0 };
    sprintf(buf, "bth-%d", stack_size);

    status = wm_os_internal_task_create(&new_thread->task, /*"bth"*/ buf, entry, arg, stack_size, prio, options);

    if (status == WM_OS_STATUS_SUCCESS) {
        if (K_TIMEOUT_EQ(delay, Z_FOREVER)) {
            wm_os_internal_task_suspend(&new_thread->task);
        }
    }

    return new_thread;
}

void k_thread_delete(struct k_thread *new_thread)
{
    if (new_thread == NULL || new_thread->task == NULL) {
        wm_log_warn("wm_os_internal_task_del with illegal param");
        return;
    }

    wm_os_internal_task_del(new_thread->task);
    new_thread->task = NULL;
}

void k_thread_abort(struct k_thread *new_thread)
{
    if (new_thread == NULL || new_thread->task == NULL) {
        wm_log_warn("wm_os_internal_task_del with illegal param");
        return;
    }

    wm_os_internal_task_del(new_thread->task);
    new_thread->task = NULL;
}

int k_thread_name_set(struct k_thread *new_thread, const char *str)
{
    if (new_thread == NULL || new_thread->task == NULL) {
        wm_log_warn("wm_os_internal_set_task_name with illegal param");
        return -EINVAL;
    }

    wm_os_internal_set_task_name(&new_thread->task, str);

    return 0;
}

int32_t k_sleep(k_timeout_t timeout)
{
    wm_os_internal_time_delay_ms(timeout.ticks * MSEC_PER_SEC / HZ);

    return 0;
}

void k_yield(void)
{
    wm_os_internal_task_yield();
}

void k_wakeup(k_tid_t new_thread)
{
    if (new_thread == NULL || new_thread->task == NULL) {
        wm_log_warn("k_wakeup with illegal param");
        return;
    }

    wm_os_internal_task_resume(&new_thread->task);
}

void k_thread_start(k_tid_t new_thread)
{
    if (new_thread == NULL || new_thread->task == NULL) {
        wm_log_warn("k_thread_start with illegal param");
        return;
    }

    wm_os_internal_task_resume(&new_thread->task);
}

void k_thread_suspend(k_tid_t new_thread)
{
    if (new_thread == NULL || new_thread->task == NULL) {
        wm_log_warn("k_thread_suspend with illegal param");
        return;
    }

    wm_os_internal_task_suspend(&new_thread->task);
}

void k_thread_resume(k_tid_t new_thread)
{
    if (new_thread == NULL || new_thread->task == NULL) {
        wm_log_warn("k_thread_resume with illegal param");
        return;
    }

    wm_os_internal_task_resume(&new_thread->task);
}

bool k_is_in_isr(void)
{
    return (wm_os_internal_get_isr_count() > 0) ? true : false;
}

static void k_private_timer_expire(void *ptmr, void *parg)
{
    (void)ptmr;

    struct k_timer *argu = (struct k_timer *)parg;

    if (argu && argu->expiry_fn) {
        argu->expiry_fn(argu->user_data);
    }
}

uint32_t k_uptime_ticks(void)
{
    return wm_os_internal_get_time();
}

#ifdef KERNEL_MEMORY_DEBUG
void k_timer_init_debug(struct k_timer *timer, k_timer_expiry_t expiry_fn, void *user_data, const char *file, int line)
#else
void k_timer_init(struct k_timer *timer, k_timer_expiry_t expiry_fn, void *user_data)
#endif

{
    wm_os_status_t status;

#ifdef KERNEL_MEMORY_DEBUG
    wm_log_warn("alloc timer %s %d\r\n", filename(file), line);
#endif

    if (timer == NULL) {
        wm_log_error("k_timer_init illegal param");
        return;
    }

    timer->user_data = user_data;
    timer->expiry_fn = expiry_fn;

    status = wm_os_internal_timer_create(&timer->timer, k_private_timer_expire, timer, WM_OS_WAIT_TIME_MAX, false, "bth-timer");

    if (status != WM_OS_STATUS_SUCCESS) {
        wm_log_error("k_timer_init failed");
    }
}
#ifdef KERNEL_MEMORY_DEBUG
void k_timer_deinit_debug(struct k_timer *timer, const char *file, int line)
#else
void k_timer_deinit(struct k_timer *timer)
#endif
{
    if (timer == NULL || timer->timer == NULL) {
        wm_log_error("k_timer_deinit illegal param");
        return;
    }

#ifdef KERNEL_MEMORY_DEBUG
    wm_log_warn("free timer %s %d\r\n", filename(file), line);
#endif

    wm_os_internal_timer_delete(timer->timer);
    timer->timer = NULL;
}

void k_timer_start(struct k_timer *timer, uint32_t period)
{
    if (timer == NULL || timer->timer == NULL) {
        wm_log_error("k_timer_start illegal param");
        return;
    }

    timer->period = period;
    timer->start = wm_os_internal_get_time_ms();

    wm_os_internal_timer_change(timer->timer, period);
}

void k_timer_stop(struct k_timer *timer)
{
    if (timer == NULL || timer->timer == NULL) {
        wm_log_error("k_timer_stop illegal param");
        return;
    }

    wm_os_internal_timer_stop(timer->timer);
}

k_ticks_t k_timer_expires_ticks(struct k_timer *timer)
{
    if (timer == NULL || timer->timer == NULL) {
        wm_log_error("k_timer_expires_ticks illegal param");
        return 0;
    }

    return wm_os_internal_timer_expirytime(timer->timer);
}

k_ticks_t k_timer_remaining_ticks(struct k_timer *timer)
{
    if (timer == NULL || timer->timer == NULL) {
        wm_log_error("k_timer_remaining_ticks illegal param");
        return 0;
    }

    k_ticks_t now = wm_os_internal_get_time();
    k_ticks_t exp = wm_os_internal_timer_expirytime(timer->timer);

    if (exp > now) {
        return (exp - now);
    } else {
        return 0;
    }
}

#ifdef KERNEL_MEMORY_DEBUG
int k_sem_init_debug(struct k_sem *sem, unsigned int initial_count, unsigned int limit, const char *file, int line)

#else
int k_sem_init(struct k_sem *sem, unsigned int initial_count, unsigned int limit)
#endif
{
#ifdef KERNEL_MEMORY_DEBUG
    wm_log_warn("alloc sem %s %d\r\n", filename(file), line);
#endif
    wm_os_status_t status;

    if (sem == NULL) {
        wm_log_error("k_sem_init invalid param");
        return -EINVAL;
    }

    status = wm_os_internal_sem_create(&sem->sem, initial_count);

    if (status != WM_OS_STATUS_SUCCESS) {
        wm_log_error("k_sem_init failed");
        return -ENOMEM;
    }

    return 0;
}

#ifdef KERNEL_MEMORY_DEBUG
void k_sem_deinit_debug(struct k_sem *sem, const char *file, int line)
#else
void k_sem_deinit(struct k_sem *sem)
#endif
{
#ifdef KERNEL_MEMORY_DEBUG
    wm_log_warn("free sem %s %d\r\n", filename(file), line);
#endif

    if (sem == NULL || sem->sem == NULL) {
        wm_log_error("k_sem_deinit illegal param");
        return;
    }

    wm_os_internal_sem_delete(sem->sem);
    sem->sem = NULL;
}

#ifdef KERNEL_MEMORY_DEBUG
void k_sem_give_debug(struct k_sem *sem, const char *file, int line)
#else
void k_sem_give(struct k_sem *sem)
#endif
{
    wm_os_status_t status;

#ifdef KERNEL_MEMORY_DEBUG
#ifndef KERNEL_MEMORY_LEAKAGE
    wm_log_warn("sem_give %p %s %d\r\n", sem, filename(file), line);
#endif
#endif

    if (sem == NULL || sem->sem == NULL) {
        wm_log_error("k_sem_give illegal param");
        return;
    }

    status = wm_os_internal_sem_release(sem->sem);

    if (status != WM_OS_STATUS_SUCCESS) {
        //wm_log_warn("sem_give warning");
        //wm_log_warn("@@@sem_give %p %s %d\r\n", sem, filename(file), line);
    }
}

int k_sem_take(struct k_sem *sem, k_timeout_t timeout)
{
    wm_os_status_t status;

    if (sem == NULL || sem->sem == NULL) {
        wm_log_error("k_sem_give illegal param");
        return -EINVAL;
    }

    status = wm_os_internal_sem_acquire(sem->sem, timeout.ticks);

    if (K_TIMEOUT_EQ(timeout, K_FOREVER)) {
        assert(status == WM_OS_STATUS_SUCCESS);
    }

    if (status != WM_OS_STATUS_SUCCESS) {
        return -ETIMEDOUT;
    }

    return 0;
}

void k_sem_reset(struct k_sem *sem)
{
    wm_os_internal_sem_reset(sem->sem);
}

unsigned int k_sem_count_get(struct k_sem *sem)
{
    if (sem == NULL || sem->sem == NULL) {
        wm_log_error("k_sem_count_get illegal param");
        return -EINVAL;
    }

    return wm_os_internal_sem_get_count(sem->sem);
}

#ifdef KERNEL_MEMORY_DEBUG
void k_fifo_init_debug(struct k_fifo *fifo, const char *file, int line)
{
    wm_log_info("%s %d\r\n", filename(file), line);

    if (fifo == NULL) {
        wm_log_error("k_fifo_init illegal param");
        return;
    }

    k_queue_init(&fifo->_queue);
}
void k_fifo_deinit_debug(struct k_fifo *fifo, const char *file, int line)
{
    wm_log_info("%s %d\r\n", filename(file), line);

    if (fifo == NULL || fifo->_queue.queue == NULL) {
        wm_log_error("k_fifo_deinit illegal param fifo %p, fifo->_queue.queue %p", fifo, fifo->_queue.queue);
        return;
    }

    k_queue_deinit(&fifo->_queue);
}
#endif

#ifdef KERNEL_MEMORY_DEBUG
void k_lifo_init_debug(struct k_lifo *lifo, const char *file, int line)
{
    wm_log_info("%s %d\r\n", filename(file), line);

    if (lifo == NULL) {
        wm_log_error("k_lifo_init illegal param");
        return;
    }

    k_queue_init(&lifo->_queue);
}
void k_lifo_deinit_debug(struct k_lifo *lifo, const char *file, int line)
{
    wm_log_info("%s %d\r\n", filename(file), line);

    if (lifo == NULL || lifo->_queue.queue == NULL) {
        wm_log_error("k_fifo_deinit illegal param");
        return;
    }

    k_queue_deinit(&lifo->_queue);
}
#endif

#ifdef KERNEL_MEMORY_DEBUG
void k_queue_init_debug(struct k_queue *queue, const char *file, int line)
#else
void k_queue_init(struct k_queue *queue)
#endif
{
#ifdef KERNEL_MEMORY_DEBUG
    wm_log_warn("alloc queue %s %d\r\n", filename(file), line);
#endif
#define WM_BT_DFLT_QUEUE_SIZE 24
    wm_os_status_t status;

    if (queue == NULL) {
        wm_log_error("k_queue_init illegal param");
        return;
    }

    status = wm_os_internal_queue_create(&queue->queue, WM_BT_DFLT_QUEUE_SIZE);

    if (status != WM_OS_STATUS_SUCCESS) {
        wm_log_error("k_queue_init failed");
        assert(0);
    }

    return;
}

#ifdef KERNEL_MEMORY_DEBUG
void k_queue_deinit_debug(struct k_queue *queue, const char *file, int line)
#else
void k_queue_deinit(struct k_queue *queue)
#endif
{
#ifdef KERNEL_MEMORY_DEBUG
    wm_log_info("free queue %s %d\r\n", filename(file), line);
#endif

    if (queue == NULL || queue->queue == NULL) {
        wm_log_error("k_queue_deinit illegal param");
        return;
    }

    wm_os_internal_queue_delete(queue->queue);
    queue->queue = NULL;
}

int k_queue_append(struct k_queue *queue, void *data)
{
    wm_os_status_t status;

    if (queue == NULL || queue->queue == NULL) {
        wm_log_error("k_queue_append illegal param");
        return -EINVAL;
    }

    status = wm_os_internal_queue_send_to_back(queue->queue, data);

    if (status != WM_OS_STATUS_SUCCESS) {
        wm_log_warn("k_queue_append failed");
        return -EAGAIN;
    }

    return 0;
}

int k_queue_prepend(struct k_queue *queue, void *data)
{
    wm_os_status_t status;

    if (queue == NULL || queue->queue == NULL) {
        wm_log_error("k_queue_prepend illegal param");
        return -EINVAL;
    }

    status = wm_os_internal_queue_send_to_front(queue->queue, data);

    if (status != WM_OS_STATUS_SUCCESS) {
        wm_log_warn("k_queue_prepend failed");
        return -EAGAIN;
    }

    return 0;
}

void *k_queue_get(struct k_queue *queue, k_timeout_t timeout)
{
    wm_os_status_t status;
    void *msg = NULL;

    if (queue == NULL || queue->queue == NULL) {
        return NULL;
    }

    status = wm_os_internal_queue_receive(queue->queue, &msg, timeout.ticks);

    if (status == WM_OS_STATUS_SUCCESS) {
        return msg;
    }

    return msg;
}

bool k_queue_remove(struct k_queue *queue, void *data)
{
    wm_os_status_t status;

    if (queue == NULL || queue->queue == NULL) {
        wm_log_error("k_queue_remove illegal param");
        return false;
    }

    status = wm_os_internal_queue_remove(queue->queue, data);

    if (status != WM_OS_STATUS_SUCCESS) {
        wm_log_error("k_queue_remove failed");
    }

    return (status == WM_OS_STATUS_SUCCESS) ? true : false;
}

int k_queue_is_empty(struct k_queue *queue)
{
    if (queue == NULL || queue->queue == NULL) {
        wm_log_error("wm_os_internal_queue_is_empty illegal param");
        return true;
    }

    return wm_os_internal_queue_is_empty(queue->queue);
}

void *k_queue_peek_head(struct k_queue *queue)
{
    void *msg = NULL;
    wm_os_status_t status;

    if (queue == NULL || queue->queue == NULL) {
        wm_log_error("k_queue_peek_head illegal param");
        return NULL;
    }

    status = wm_os_internal_queue_peek(queue->queue, &msg, WM_OS_WAIT_TIME_MAX);

    if (status != WM_OS_STATUS_SUCCESS) {
        wm_log_error("k_queue_peek_head failed");
    }

    return msg;
}

#ifdef KERNEL_MEMORY_DEBUG
void k_event_init_debug(struct k_event *event, const char *file, int line)
#else
void k_event_init(struct k_event *event)
#endif
{
    wm_os_status_t status;
#ifdef KERNEL_MEMORY_DEBUG
    wm_log_info("alloc event %s %d\r\n", filename(file), line);
#endif

    if (event == NULL) {
        wm_log_error("k_event_init failed");
        return;
    }

    status = wm_os_internal_event_create(&event->event);

    if (status != WM_OS_STATUS_SUCCESS) {
        wm_log_error("wm_os_internal_event_create failed");
    }
}

#ifdef KERNEL_MEMORY_DEBUG
void k_event_deinit_debug(struct k_event *event, const char *file, int line)
#else
void k_event_deinit(struct k_event *event)
#endif
{
    wm_os_status_t status;
#ifdef KERNEL_MEMORY_DEBUG
    wm_log_warn("free event %s %d\r\n", filename(file), line);
#endif

    if (event == NULL || event->event == NULL) {
        wm_log_error("k_event_deinit illegal param");
        return;
    }

    status = wm_os_internal_event_delete(&event->event);

    if (status != WM_OS_STATUS_SUCCESS) {
        wm_log_error("wm_os_internal_event_delete failed");
    }

    event->event = NULL;
}

uint32_t k_event_post(struct k_event *event, uint32_t events)
{
    wm_os_status_t status;
    uint32_t curr_bits = 0, actual_bits = 0;

    if (event == NULL || event->event == NULL) {
        wm_log_error("k_event_post illegal param");
        return 0;
    }

    status = wm_os_internal_event_get_bits(event->event, &curr_bits);

    if (status != WM_OS_STATUS_SUCCESS) {
        wm_log_error("wm_os_internal_event_get_bits failed");
    }

    actual_bits = events | curr_bits;
    status = wm_os_internal_event_put(event->event, actual_bits);

    if (status != WM_OS_STATUS_SUCCESS) {
        wm_log_error("wm_os_internal_event_put failed");
    }

    return curr_bits;
}

uint32_t k_event_set(struct k_event *event, uint32_t events)
{
    wm_os_status_t status;
    uint32_t curr_bits = 0;

    if (event == NULL || event->event == NULL) {
        wm_log_error("k_event_set illegal param");
        return 0;
    }

    status = wm_os_internal_event_get_bits(event->event, &curr_bits);

    if (status != WM_OS_STATUS_SUCCESS) {
        wm_log_error("wm_os_internal_event_get_bits failed");
    }

    status = wm_os_internal_event_put(event->event, events);

    if (status != WM_OS_STATUS_SUCCESS) {
        wm_log_error("wm_os_internal_event_put failed");
    }

    return curr_bits;
}

uint32_t k_event_clear(struct k_event *event, uint32_t events)
{
    uint32_t curr_bits = 0;
    wm_os_status_t status;

    if (event == NULL || event->event == NULL) {
        wm_log_error("k_event_clear illegal param");
        return 0;
    }

    status = wm_os_internal_event_get_bits(event->event, &curr_bits);

    if (status != WM_OS_STATUS_SUCCESS) {
        wm_log_error("wm_os_internal_event_get_bits failed");
    }

    status = wm_os_internal_event_clear_bits(event->event, events);

    if (status != WM_OS_STATUS_SUCCESS) {
        wm_log_error("wm_os_internal_event_clear_bits failed");
    }

    return curr_bits;
}
uint32_t k_event_wait(struct k_event *event, uint32_t events, bool reset, k_timeout_t timeout)
{
    uint32_t curr_bits = 0;
    wm_os_status_t status;

    if (event == NULL || event->event == NULL) {
        wm_log_error("k_event_wait illegal param");
        return 0;
    }

    status = wm_os_internal_event_get(event->event, events, &curr_bits, reset, timeout.ticks);

    if (status != WM_OS_STATUS_SUCCESS && K_TIMEOUT_EQ(timeout, Z_FOREVER)) {
        wm_log_error("wm_os_internal_event_get failed");
    }

    return curr_bits;
}

#ifdef KERNEL_MEMORY_DEBUG
int k_mutex_init_debug(struct k_mutex *mutex, const char *file, int line)
#else
int k_mutex_init(struct k_mutex *mutex)
#endif
{
    wm_os_status_t status;

#ifdef KERNEL_MEMORY_DEBUG
    wm_log_info("alloc mutex %s %d\r\n", filename(file), line);
#endif

    if (mutex == NULL) {
        wm_log_error("k_mutex_init illegal param");
        return -EINVAL;
    }

    status = wm_os_internal_recursive_mutex_create(&mutex->mutex);

    if (status != WM_OS_STATUS_SUCCESS) {
        wm_log_error("wm_os_internal_event_clear_bits failed");
        return -EAGAIN;
    }

    return 0;
}
#ifdef KERNEL_MEMORY_DEBUG
int k_mutex_deinit_debug(struct k_mutex *mutex, const char *file, int line)
#else
int k_mutex_deinit(struct k_mutex *mutex)
#endif
{
#ifdef KERNEL_MEMORY_DEBUG
    wm_log_info("free mutex %s %d\r\n", filename(file), line);
#endif

    if (mutex == NULL || mutex->mutex == NULL) {
        wm_log_error("k_mutex_deinit illegal param");
        return -EINVAL;
    }

    wm_os_internal_recursive_mutex_delete(mutex->mutex);
    mutex->mutex = NULL;

    return 0;
}

int k_mutex_lock(int pos, struct k_mutex *mutex, k_timeout_t timeout)
{
    wm_os_status_t status;

    if (mutex == NULL || mutex->mutex == NULL) {
        wm_log_error("k_mutex_lock illegal param");
        return -EINVAL;
    }

    status = wm_os_internal_recursive_mutex_acquire(mutex->mutex, timeout.ticks);

    return status;
}

int k_mutex_unlock(struct k_mutex *mutex)
{
    wm_os_status_t status;

    if (mutex == NULL || mutex->mutex == NULL) {
        wm_log_error("k_mutex_unlock illegal param");
        return -EINVAL;
    }

    status = wm_os_internal_recursive_mutex_release(mutex->mutex);

    return status;
}

static struct k_work_q sys_work_queue;

static inline void flag_clear(uint32_t *flagp, uint32_t bit)
{
    uint32_t irq;

    irq = k_irq_lock();
    *flagp &= ~BIT(bit);
    k_irq_unlock(irq);
}

static inline void flag_set(uint32_t *flagp, uint32_t bit)
{
    uint32_t irq;

    irq = k_irq_lock();
    *flagp |= BIT(bit);
    k_irq_unlock(irq);
}

static inline bool flag_test(const uint32_t *flagp, uint32_t bit)
{
    uint32_t irq;
    bool ret;

    irq = k_irq_lock();
    ret = (*flagp & BIT(bit)) != 0U;
    k_irq_unlock(irq);

    return ret;
}

static inline bool flag_test_and_clear(uint32_t *flagp, int bit)
{
    uint32_t irq;
    bool ret;

    irq = k_irq_lock();
    ret = (*flagp & BIT(bit)) != 0U;
    *flagp &= ~BIT(bit);
    k_irq_unlock(irq);

    return ret;
}

static inline void flags_set(uint32_t *flagp, uint32_t flags)
{
    *flagp = flags;
}

static inline uint32_t flags_get(const uint32_t *flagp)
{
    return *flagp;
}

static void sys_work_queue_entry(void *parg)
{
    (void)(parg);
    struct k_work *work;

    while (1) {
        work = k_fifo_get(&sys_work_queue.fifo, K_FOREVER);

        if (flag_test_and_clear(&work->flags, K_WORK_QUEUED_BIT)) {
            flag_set(&work->flags, K_WORK_RUNNING_BIT);
            if(work->handler) {
            work->handler(work);
            }
            flag_clear(&work->flags, K_WORK_RUNNING_BIT);
        }

        k_yield();
    }
}

int k_sys_work_init(void)
{
    k_sys_poll_sem_init();

    k_fifo_init(&sys_work_queue.fifo);

    k_mutex_init(&k_sys_mutex);

    k_thread_create(&sys_work_queue.thread, (k_thread_stack_t *)NULL, CONFIG_SYSTEM_WORKQUEUE_STACK_SIZE, sys_work_queue_entry,
                    NULL, CONFIG_SYSTEM_WORKQUEUE_PRIORITY, 0, K_NO_WAIT);
    k_thread_name_set(&sys_work_queue.thread, "sys_worker");

    return 0;
}

void k_sys_work_deinit(void)
{
    k_thread_delete(&sys_work_queue.thread);

    k_fifo_deinit(&sys_work_queue.fifo);

    k_sys_poll_sem_deinit();

    k_mutex_deinit(&k_sys_mutex);
}

int k_work_init(struct k_work *work, k_work_handler_t handler)
{
    if (work == NULL) {
        return -EINVAL;
    }

    work->flags = 0;
    work->handler = handler;

    return 0;
}

int k_work_busy_get(const struct k_work *work)
{
    int ret;

    if (work == NULL) {
        return -EINVAL;
    }

    ret = flags_get(&work->flags) & K_WORK_MASK;

    return ret;
}

int k_work_cancel(struct k_work *work)
{
    int ret;

    if (work == NULL) {
        return -EINVAL;
    }

    if (flag_test_and_clear(&work->flags, K_WORK_QUEUED_BIT)) {
        k_queue_remove(&work->queue->fifo._queue, work);
    }

    ret = flags_get(&work->flags) & K_WORK_MASK;

    return ret;
}

bool k_work_is_pending(const struct k_work *work)
{
    return k_work_busy_get(work) != 0;
}

int k_work_submit(struct k_work *work)
{
    int ret = 0;

    ret = k_work_submit_to_queue(2, &sys_work_queue, work);

    return ret;
}

#ifdef KERNEL_MEMORY_DEBUG
void k_work_queue_init_debug(struct k_work_q *queue, const char *file, int line)
#else
void k_work_queue_init(struct k_work_q *queue)
#endif
{
#ifdef KERNEL_MEMORY_DEBUG
    wm_log_info("%s %d\r\n", filename(file), line);
#endif

    if (queue == NULL) {
        wm_log_warn("k_work_queue_init illegal param");
        return;
    }

    k_fifo_init(&queue->fifo);
}

#ifdef KERNEL_MEMORY_DEBUG
void k_work_queue_deinit_debug(struct k_work_q *queue, const char *file, int line)
#else
void k_work_queue_deinit(struct k_work_q *queue)
#endif
{
#ifdef KERNEL_MEMORY_DEBUG
    wm_log_info("%s %d\r\n", filename(file), line);
#endif

    if (queue == NULL) {
        wm_log_warn("k_work_queue_deinit illegal param");
        return;
    }

    k_fifo_deinit(&queue->fifo);
}

static void bt_work_queue_entry(void *parg)
{
    struct k_work_q *bt_work_queue = (struct k_work_q *)parg;
    struct k_work *work;

    while (1) {
        work = k_fifo_get(&bt_work_queue->fifo, K_FOREVER);

        if (flag_test_and_clear(&work->flags, K_WORK_QUEUED_BIT)) {
            flag_set(&work->flags, K_WORK_RUNNING_BIT);
            work->handler(work);
            flag_clear(&work->flags, K_WORK_RUNNING_BIT);
        }

        k_yield();
    }
}

void k_work_queue_start(struct k_work_q *queue, k_thread_stack_t *stack, size_t stack_size, int prio,
                        const struct k_work_queue_config *cfg)
{
    k_thread_create(&queue->thread, (k_thread_stack_t *)NULL, stack_size, bt_work_queue_entry, (void *)queue, prio, 0,
                    K_NO_WAIT);

    if (cfg && cfg->name) {
        k_thread_name_set(&queue->thread, cfg->name);
    }
}

int k_work_submit_to_queue(int pos, struct k_work_q *queue, struct k_work *work)
{
    int ret = 0;

    if (flag_test(&work->flags, K_WORK_CANCELING_BIT)) {
        /* Disallowed */
        ret = -EBUSY;
    } else if (!flag_test(&work->flags, K_WORK_QUEUED_BIT)) {
        work->queue = queue;
        flag_set(&work->flags, K_WORK_QUEUED_BIT);
        k_fifo_put(&(queue->fifo), (void *)work);
    } else {
        ret = -EPERM;
    }

    return ret;
}

int k_work_cancel_delayable(struct k_work_delayable *dwork)
{
    if (flag_test_and_clear(&dwork->work.flags, K_WORK_DELAYED_BIT)) {
        k_timer_stop(&dwork->timer);
        dwork->work_q = NULL;
    }

    return 0;
}

static void sys_work_timer_expired(struct k_timer *parg)
{
    struct k_work_delayable *dwork = (struct k_work_delayable *)parg;

    if (flag_test_and_clear(&dwork->work.flags, K_WORK_DELAYED_BIT)) {
        k_work_submit_to_queue(3, dwork->work_q, &dwork->work);
    } else {
        wm_log_warn("sys_work_timer_expired, but dwork %p flag is not delayed", dwork);
    }

    dwork->work_q = NULL;
}

#ifdef KERNEL_MEMORY_DEBUG
void k_work_init_delayable_debug(struct k_work_delayable *dwork, k_work_handler_t handler, const char *file, int line)

#else
void k_work_init_delayable(struct k_work_delayable *dwork, k_work_handler_t handler)
#endif
{
#ifdef KERNEL_MEMORY_DEBUG
    wm_log_info("dworker %p %s %d\r\n", dwork, filename(file), line);
#endif

    k_work_init(&dwork->work, handler);
    k_timer_init(&dwork->timer, sys_work_timer_expired, (void *)dwork);
    dwork->work.flags = 0;
    dwork->work_q = NULL;
}

#ifdef KERNEL_MEMORY_DEBUG
void k_work_deinit_delayable_debug(struct k_work_delayable *dwork, const char *file, int line)

#else
void k_work_deinit_delayable(struct k_work_delayable *dwork)
#endif
{
#ifdef KERNEL_MEMORY_DEBUG
    wm_log_warn("dwork %p %s %d\r\n", dwork, filename(file), line);
#endif

    dwork->work.handler = NULL;
    k_timer_deinit(&dwork->timer);
    dwork->timer.timer = NULL;
}

static int k_delayed_work_submit_to_queue(struct k_work_q *work_q, struct k_work_delayable *dwork, k_timeout_t timeout)
{
    int err;

    if (work_q == NULL || dwork == NULL) {
        wm_log_warn("k_delayed_work_submit_to_queue invalid param");
        return -EINVAL;
    }

    if (dwork->work_q && dwork->work_q != work_q) {
        err = -EAGAIN;
        wm_log_warn("delayed work %p submitted to differt queue", dwork);
        goto done;
    }

    if (dwork->work_q == work_q) {
        err = k_work_cancel_delayable(dwork);

        if (err < 0) {
            wm_log_warn("cancel delayable work %p failed", dwork);
            goto done;
        }
    }

    if (timeout.ticks == 0) {
        dwork->work_q = NULL;
        k_work_submit_to_queue(4, work_q, &dwork->work);
    } else {
        if (!flag_test(&dwork->work.flags, K_WORK_DELAYED_BIT)) {
            dwork->work_q = work_q;
            flag_set(&dwork->work.flags, K_WORK_DELAYED_BIT);
            k_timer_start(&dwork->timer, timeout.ticks);
        } else {
            wm_log_warn("work already in delayed state");
        }
    }

    err = 0;
done:
    return err;
}

struct k_work_delayable *k_work_delayable_from_work(struct k_work *work)
{
    return CONTAINER_OF(work, struct k_work_delayable, work);
}

int k_work_schedule_for_queue(struct k_work_q *queue, struct k_work_delayable *dwork, k_timeout_t delay)
{
    return k_delayed_work_submit_to_queue(queue, dwork, delay);
}
int k_work_schedule(struct k_work_delayable *dwork, k_timeout_t delay)
{
    int ret = 0;
    ret = k_delayed_work_submit_to_queue(&sys_work_queue, dwork, delay);
    return ret;
}
int k_work_reschedule_for_queue(struct k_work_q *queue, struct k_work_delayable *dwork, k_timeout_t delay)
{
    if (k_work_delayable_busy_get(dwork) != 0) {
        k_work_cancel_delayable(dwork);
    }

    return k_delayed_work_submit_to_queue(queue, dwork, delay);
}

int k_work_reschedule(struct k_work_delayable *dwork, k_timeout_t delay)
{
    if (k_work_delayable_busy_get(dwork) != 0) {
        k_work_cancel_delayable(dwork);
    }

    return k_delayed_work_submit_to_queue(&sys_work_queue, dwork, delay);
}

int k_work_delayable_busy_get(const struct k_work_delayable *dwork)
{
    return flags_get(&dwork->work.flags);
}

bool k_work_delayable_is_pending(const struct k_work_delayable *dwork)
{
    return k_work_delayable_busy_get(dwork) != 0;
}

bool k_work_cancel_delayable_sync(struct k_work_delayable *dwork, struct k_work_sync *sync)
{
    k_work_cancel_delayable(dwork);
    k_work_cancel(&dwork->work);

    return true;
}

void *k_malloc(size_t size)
{
    return wm_os_internal_malloc(size);
}

void k_free(void *ptr)
{
    wm_os_internal_free(ptr);
}

void *k_calloc(size_t nmemb, size_t size)
{
    return wm_os_internal_calloc(nmemb, size);
}

void k_kernel_lock(void)
{
    k_mutex_lock(5, &k_sys_mutex, K_FOREVER);
}

void k_kernel_unlock(void)
{
    k_mutex_unlock(&k_sys_mutex);
}
