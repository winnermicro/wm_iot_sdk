/*
 * Copyright (c) 2023, Winner Micro IC, Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @file
 *
 * @brief Public kernel APIs.
 */

#ifndef WM_INCLUDE_KERNEL_H_
#define WM_INCLUDE_KERNEL_H_

#if !defined(_ASMLANGUAGE)
#include <errno.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <toolchain.h>
#include <sys/sflist.h>
#include <sys/slist.h>
#include <sys/dlist.h>
#include <sys_clock.h>
#include <sys/atomic.h>

#include "wm_osal.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Kernel APIs
 * @defgroup kernel_apis Kernel APIs
 * @{
 * @}
 */

struct k_mutex;
struct k_sem;
struct k_queue;
struct k_fifo;
struct k_lifo;
struct k_timer;
struct k_event;

struct k_thread {
    wm_os_task_t task;
};

typedef int z_arch_esf_t;
typedef struct k_thread *k_tid_t;
typedef int ssize_t;
typedef uint32_t _stack_element_t;
typedef sys_dlist_t _wait_q_t;

#if !defined(__off_t_defined) && !defined(_OFF_T_DECLARED)
typedef int off_t;
#define __off_t_defined
#define _OFF_T_DECLARED
#endif

//#define KERNEL_MEMORY_DEBUG
//#define KERNEL_MEMORY_LEAKAGE
//#define CONFIG_NET_BUF_POOL_USAGE
//#define CONFIG_NET_BUF_LOG

#ifndef __aligned
#define __aligned(x) __attribute__((__aligned__(x)))
#endif

#ifndef __must_check
#define __must_check
#endif

#define _NOINIT_SECTION_NAME noinit

#define K_PRIO_COOP(x)       x

#define __noinit             __in_section_unique(_NOINIT_SECTION_NAME)
#define __noinit_named(name) __in_section_unique_named(_NOINIT_SECTION_NAME, name)

/**
 * @defgroup thread_apis Thread APIs
 * @ingroup kernel_apis
 * @{
 */

#endif /* !_ASMLANGUAGE */

/*
 * Thread user options. May be needed by assembly code. Common part uses low
 * bits, arch-specific use high bits.
 */

/**
 * @brief system thread that must not abort
 * */
#define K_ESSENTIAL      (BIT(0))

/**
 * @brief user mode thread
 *
 * This thread has dropped from supervisor mode to user mode and consequently
 * has additional restrictions
 */
#define K_USER           (BIT(2))

/**
 * @brief Inherit Permissions
 *
 * @details
 * Indicates that the thread being created should inherit all kernel object
 * permissions from the thread that created it. No effect if
 * @kconfig{CONFIG_USERSPACE} is not enabled.
 */
#define K_INHERIT_PERMS  (BIT(3))

/**
 * @brief Callback item state
 *
 * @details
 * This is a single bit of state reserved for "callback manager"
 * utilities (p4wq initially) who need to track operations invoked
 * from within a user-provided callback they have been invoked.
 * Effectively it serves as a tiny bit of zero-overhead TLS data.
 */
#define K_CALLBACK_STATE (BIT(4))

/* end - thread options */

#if !defined(_ASMLANGUAGE)

typedef _stack_element_t k_thread_stack_t;

inline void k_call_stacks_analyze(void)
{
}

#define K_THREAD_STACK_DEFINE(sym, size) _stack_element_t sym[size]
#define K_THREAD_STACK_SIZEOF(sym)       sizeof(sym)

#define K_KERNEL_STACK_DEFINE            K_THREAD_STACK_DEFINE
#define K_KERNEL_STACK_SIZEOF            K_THREAD_STACK_SIZEOF

/**
 * @brief Create a thread.
 *
 * This routine initializes a thread, then schedules it for execution.
 *
 * The new thread may be scheduled for immediate execution or a delayed start.
 * If the newly spawned thread does not have a delayed start the kernel
 * scheduler may preempt the current thread to allow the new thread to
 * execute.
 *
 *
 * @return ID of new thread.
 *
 */
typedef void (*k_thread_entry_t)(void *arg);

k_tid_t k_thread_create(struct k_thread *new_thread, k_thread_stack_t *stack, size_t stack_size, k_thread_entry_t entry,
                        void *arg, int prio, uint32_t options, k_timeout_t delay);

void k_thread_delete(struct k_thread *new_thread);

/**
 * @brief Abort a thread.
 *
 * This routine permanently stops execution of @a thread. The thread is taken
 * off all kernel queues it is part of (i.e. the ready queue, the timeout
 * queue, or a kernel object wait queue). However, any kernel resources the
 * thread might currently own (such as mutexes or memory blocks) are not
 * released. It is the responsibility of the caller of this routine to ensure
 * all necessary cleanup is performed.
 *
 * After k_thread_abort() returns, the thread is guaranteed not to be
 * running or to become runnable anywhere on the system.  Normally
 * this is done via blocking the caller (in the same manner as
 * k_thread_join()), but in interrupt context on SMP systems the
 * implementation is required to spin for threads that are running on
 * other CPUs.  Note that as specified, this means that on SMP
 * platforms it is possible for application code to create a deadlock
 * condition by simultaneously aborting a cycle of threads using at
 * least one termination from interrupt context.  Zephyr cannot detect
 * all such conditions.
 *
 * @param thread ID of thread to abort.
 */
void k_thread_abort(k_tid_t thread);

/**
 * @brief Get thread ID of the current thread.
 *
 * @return ID of current thread.
 *
 */
__attribute_const__ static inline void *k_current_get(void)
{
    return wm_os_internal_task_id();
}

/**
* @brief Set current thread name
*
* Set the name of the thread to be used when @kconfig{CONFIG_THREAD_MONITOR}
* is enabled for tracing and debugging.
*
* @param kit Thread to set name, or NULL to set the current thread
* @param str Name string
* @retval 0 on success
* @retval -EFAULT Memory access error with supplied string
* @retval -ENOSYS Thread name configuration option not enabled
* @retval -EINVAL Thread name too long
*/
int k_thread_name_set(struct k_thread *kit, const char *str);

/**
 * @brief Put the current thread to sleep.
 *
 * This routine puts the current thread to sleep for @a duration,
 * specified as a k_timeout_t object.
 *
 * @note if @a timeout is set to K_FOREVER then the thread is suspended.
 *
 * @param timeout Desired duration of sleep.
 *
 * @return Zero if the requested time has elapsed or the number of milliseconds
 * left to sleep, if thread was woken up by \ref k_wakeup call.
 */
int32_t k_sleep(k_timeout_t timeout);

/**
 * @brief Put the current thread to sleep.
 *
 * This routine puts the current thread to sleep for @a duration milliseconds.
 *
 * @param ms Number of milliseconds to sleep.
 *
 * @return Zero if the requested time has elapsed or the number of milliseconds
 * left to sleep, if thread was woken up by \ref k_wakeup call.
 */
static inline int32_t k_msleep(int32_t ms)
{
    wm_os_internal_time_delay_ms(ms);
    return 0;
}

/**
 * @brief Yield the current thread.
 *
 * This routine causes the current thread to yield execution to another
 * thread of the same or higher priority. If there are no other ready threads
 * of the same or higher priority, the routine returns immediately.
 */
void k_yield(void);

/**
 * @brief Wake up a sleeping thread.
 *
 * This routine prematurely wakes up @a thread from sleeping.
 *
 * If @a thread is not currently sleeping, the routine has no effect.
 *
 * @param thread ID of thread to wake.
 */
void k_wakeup(k_tid_t thread);

/**
 * @brief Start an inactive thread
 *
 * If a thread was created with K_FOREVER in the delay parameter, it will
 * not be added to the scheduling queue until this function is called
 * on it.
 *
 * @param thread thread to start
 */
void k_thread_start(k_tid_t thread);

/**
 * @brief Suspend a thread.
 *
 * This routine prevents the kernel scheduler from making @a thread
 * the current thread. All other internal operations on @a thread are
 * still performed; for example, kernel objects it is waiting on are
 * still handed to it.  Note that any existing timeouts
 * (e.g. k_sleep(), or a timeout argument to k_sem_take() et. al.)
 * will be canceled.  On resume, the thread will begin running
 * immediately and return from the blocked call.
 *
 * If @a thread is already suspended, the routine has no effect.
 *
 * @param thread ID of thread to suspend.
 */
void k_thread_suspend(k_tid_t thread);

/**
 * @brief Resume a suspended thread.
 *
 * This routine allows the kernel scheduler to make @a thread the current
 * thread, when it is next eligible for that role.
 *
 * If @a thread is not currently suspended, the routine has no effect.
 *
 * @param thread ID of thread to resume.
 */
void k_thread_resume(k_tid_t thread);

/** @} */

/**
 * @addtogroup isr_apis
 * @{
 */

/**
 * @brief Determine if code is running at interrupt level.
 *
 * This routine allows the caller to customize its actions, depending on
 * whether it is a thread or an ISR.
 *
 * @note isr_ok
 *
 * @return false if invoked by a thread.
 * @return true if invoked by an ISR.
 */
extern bool k_is_in_isr(void);
/**
 * @}
 */

/**
 * @addtogroup clock_apis
 * @{
 */

#define K_NO_WAIT    Z_TIMEOUT_NO_WAIT
#define K_FOREVER    Z_FOREVER

/**
 * @brief Generate timeout delay from system ticks.
 *
 * This macro generates a timeout delay that instructs a kernel API
 * to wait up to @a t ticks to perform the requested operation.
 *
 * @param t Duration in system ticks.
 *
 * @return Timeout delay value.
 */
#define K_TICKS(t)   Z_TIMEOUT_TICKS(t)

/**
 * @brief Generate timeout delay from milliseconds.
 *
 * This macro generates a timeout delay that instructs a kernel API
 * to wait up to @a ms milliseconds to perform the requested operation.
 *
 * @param ms Duration in milliseconds.
 *
 * @return Timeout delay value.
 */
#define K_MSEC(ms)   Z_TIMEOUT_TICKS((ms * HZ / MSEC_PER_SEC))

/**
 * @brief Generate timeout delay from seconds.
 *
 * This macro generates a timeout delay that instructs a kernel API
 * to wait up to @a s seconds to perform the requested operation.
 *
 * @param s Duration in seconds.
 *
 * @return Timeout delay value.
 */
#define K_SECONDS(s) K_MSEC((s) * MSEC_PER_SEC)

/**
 * @brief Generate timeout delay from minutes.

 * This macro generates a timeout delay that instructs a kernel API
 * to wait up to @a m minutes to perform the requested operation.
 *
 * @param m Duration in minutes.
 *
 * @return Timeout delay value.
 */
#define K_MINUTES(m) K_SECONDS((m) * 60)

/**
 * @brief Generate timeout delay from hours.
 *
 * This macro generates a timeout delay that instructs a kernel API
 * to wait up to @a h hours to perform the requested operation.
 *
 * @param h Duration in hours.
 *
 * @return Timeout delay value.
 */
#define K_HOURS(h)   K_MINUTES((h) * 60)

/**
 * @}
 */

/**
 * @cond INTERNAL_HIDDEN
 */

struct k_timer {
    /* wait queue for the (single) thread waiting on this timer */
    wm_os_timer_t *timer;

    /* runs in ISR context */
    void (*expiry_fn)(struct k_timer *timer);

    /* timer period */
    uint32_t period;

    /* timer mark for start time of every period*/
    uint32_t start;

    /* timer status */
    uint32_t status;

    /* user-specific data, also used to support legacy features */
    void *user_data;
};

/**
 * INTERNAL_HIDDEN @endcond
 */

/**
 * @defgroup timer_apis Timer APIs
 * @ingroup kernel_apis
 * @{
 */

/**
 * @typedef k_timer_expiry_t
 * @brief Timer expiry function type.
 *
 * A timer's expiry function is executed by the system clock interrupt handler
 * each time the timer expires. The expiry function is optional, and is only
 * invoked if the timer has been initialized with one.
 *
 * @param timer     Address of timer.
 */
typedef void (*k_timer_expiry_t)(struct k_timer *timer);

/**
 * @brief Initialize a timer.
 *
 * This routine initializes a timer, prior to its first use.
 *
 * @param timer     Address of timer.
 * @param expiry_fn Function to invoke each time the timer expires.
 * @param user_data   Function parameter of expiry_fn.
 */

#ifdef KERNEL_MEMORY_DEBUG
void k_timer_init_debug(struct k_timer *timer, k_timer_expiry_t expiry_fn, void *user_data, const char *file, int line);

void k_timer_deinit_debug(struct k_timer *timer, const char *file, int line);

#define k_timer_deinit(a)     k_timer_deinit_debug(a, __FILE__, __LINE__)
#define k_timer_init(a, b, c) k_timer_init_debug(a, b, c, __FILE__, __LINE__)
#else
void k_timer_init(struct k_timer *timer, k_timer_expiry_t expiry_fn, void *user_data);

void k_timer_deinit(struct k_timer *timer);
#endif

/**
 * @brief Start a timer.
 *
 * This routine starts a timer, and resets its status to zero. The timer
 * begins counting down using the specified duration and period values.
 *
 * Attempting to start a timer that is already running is permitted.
 * The timer's status is reset to zero and the timer begins counting down
 * using the new duration and period values.
 *
 * @param timer     Address of timer.
 * @param duration  Initial timer duration.
 * @param period    Timer period.
 */
void k_timer_start(struct k_timer *timer, uint32_t period);

/**
 * @brief Stop a timer.
 *
 * This routine stops a running timer prematurely. The timer's stop function,
 * if one exists, is invoked by the caller.
 *
 * Attempting to stop a timer that is not running is permitted, but has no
 * effect on the timer.
 *
 * @note The stop handler has to be callable from ISRs if @a k_timer_stop is to
 * be called from ISRs.
 *
 * @note isr_ok
 *
 * @param timer     Address of timer.
 */
void k_timer_stop(struct k_timer *timer);

/**
 * @brief Get next expiration time of a timer, in system ticks
 *
 * This routine returns the future system uptime reached at the next
 * time of expiration of the timer, in units of system ticks.  If the
 * timer is not running, current system time is returned.
 *
 * @param timer The timer object
 * @return Uptime of expiration, in ticks
 */
k_ticks_t k_timer_expires_ticks(struct k_timer *timer);

/**
 * @brief Get time remaining before a timer next expires, in system ticks
 *
 * This routine computes the time remaining before a running timer
 * next expires, in units of system ticks.  If the timer is not
 * running, it returns zero.
 */
k_ticks_t k_timer_remaining_ticks(struct k_timer *timer);

/**
 * @brief Get time remaining before a timer next expires.
 *
 * This routine computes the (approximate) time remaining before a running
 * timer next expires. If the timer is not running, it returns zero.
 *
 * @param timer     Address of timer.
 *
 * @return Remaining time (in milliseconds).
 */
static inline uint32_t k_timer_remaining_get(struct k_timer *timer)
{
    return (k_timer_remaining_ticks(timer)) * MSEC_PER_SEC / HZ;
}

/** @} */

/**
 * @addtogroup clock_apis
 * @ingroup kernel_apis
 * @{
 */
/**
 * @brief Get system uptime, in system ticks.
 *
 * This routine returns the elapsed time since the system booted, in
 * ticks (c.f. @kconfig{CONFIG_SYS_CLOCK_TICKS_PER_SEC}), which is the
 * fundamental unit of resolution of kernel timekeeping.
 *
 * @return Current uptime in ticks.
 */
uint32_t k_uptime_ticks(void);

/**
 * @brief Get system uptime.
 *
 * This routine returns the elapsed time since the system booted,
 * in milliseconds.
 *
 * @note
 *    While this function returns time in milliseconds, it does
 *    not mean it has millisecond resolution. The actual resolution depends on
 *    @kconfig{CONFIG_SYS_CLOCK_TICKS_PER_SEC} config option.
 *
 * @return Current uptime in milliseconds.
 */
static inline uint32_t k_uptime_get(void)
{
    return wm_os_internal_get_time_ms();
}

static inline uint32_t k_uptime_delta(uint32_t *reftime)
{
    uint32_t uptime, delta;

    uptime = k_uptime_get();
    if (uptime >= *reftime) {
        delta = uptime - *reftime;
    } else {
        delta = uptime + (0xFFFFFFFF - *reftime);
    }

    *reftime = uptime;

    return delta;
}

/**
 * @}
 */

struct k_queue {
    wm_os_queue_t *queue;
    sys_dlist_t poll_events;
};

/**
 * @defgroup queue_apis Queue APIs
 * @ingroup kernel_apis
 * @{
 */

/**
 * @brief Initialize a queue.
 *
 * This routine initializes a queue object, prior to its first use.
 *
 * @param queue Address of the queue.
 */

#ifdef KERNEL_MEMORY_DEBUG
void k_queue_init_debug(struct k_queue *queue, const char *file, int line);

void k_queue_deinit_debug(struct k_queue *queue, const char *file, int line);

#define k_queue_init(a)   k_queue_init_debug(a, __FILE__, __LINE__)
#define k_queue_deinit(a) k_queue_deinit_debug(a, __FILE__, __LINE__)

#else
void k_queue_init(struct k_queue *queue);

void k_queue_deinit(struct k_queue *queue);
#endif
/**
 * @brief Append an element to the end of a queue.
 *
 * This routine appends a data item to @a queue. A queue data item must be
 * aligned on a word boundary, and the first word of the item is reserved
 * for the kernel's use.
 *
 * @note isr_ok
 *
 * @param queue Address of the queue.
 * @param data Address of the data item.
 */
extern int k_queue_append(struct k_queue *queue, void *data);

/**
 * @brief Prepend an element to a queue.
 *
 * This routine prepends a data item to @a queue. A queue data item must be
 * aligned on a word boundary, and the first word of the item is reserved
 * for the kernel's use.
 *
 * @note isr_ok
 *
 * @param queue Address of the queue.
 * @param data Address of the data item.
 */
extern int k_queue_prepend(struct k_queue *queue, void *data);

/**
 * @brief Get an element from a queue.
 *
 * This routine removes first data item from @a queue. The first word of the
 * data item is reserved for the kernel's use.
 *
 * @note @a timeout must be set to K_NO_WAIT if called from ISR.
 *
 * @note isr_ok
 *
 * @param queue Address of the queue.
 * @param timeout Non-negative waiting period to obtain a data item
 *                or one of the special values K_NO_WAIT and
 *                K_FOREVER.
 *
 * @return Address of the data item if successful; NULL if returned
 * without waiting, or waiting period timed out.
 */
void *k_queue_get(struct k_queue *queue, k_timeout_t timeout);

/**
 * @brief Remove an element from a queue.
 *
 * This routine removes data item from @a queue. The first word of the
 * data item is reserved for the kernel's use. Removing elements from k_queue
 * rely on sys_slist_find_and_remove which is not a constant time operation.
 *
 * @note @a timeout must be set to K_NO_WAIT if called from ISR.
 *
 * @note isr_ok
 *
 * @param queue Address of the queue.
 * @param data Address of the data item.
 *
 * @return true if data item was removed
 */
bool k_queue_remove(struct k_queue *queue, void *data);

/**
 * @brief Query a queue to see if it has data available.
 *
 * Note that the data might be already gone by the time this function returns
 * if other threads are also trying to read from the queue.
 *
 * @note isr_ok
 *
 * @param queue Address of the queue.
 *
 * @return Non-zero if the queue is empty.
 * @return 0 if data is available.
 */
int k_queue_is_empty(struct k_queue *queue);

/**
 * @brief Peek element at the head of queue.
 *
 * Return element from the head of queue without removing it.
 *
 * @param queue Address of the queue.
 *
 * @return Head element, or NULL if queue is empty.
 */
void *k_queue_peek_head(struct k_queue *queue);

/** @} */

/**
 * @defgroup event_apis Event APIs
 * @ingroup kernel_apis
 * @{
 */

/**
 * Event Structure
 * @ingroup event_apis
 */

struct k_event {
    wm_os_event_t *event;
};

/**
 * @brief Initialize an event object
 *
 * This routine initializes an event object, prior to its first use.
 *
 * @param event Address of the event object.
 */

#ifdef KERNEL_MEMORY_DEBUG
void k_event_init_debug(struct k_event *event, const char *file, int line);
void k_event_deinit_debug(struct k_event *event, const char *file, int line);

#define k_event_init(a)   k_event_init_debug(a, __FILE__, __LINE__)
#define k_event_deinit(a) k_event_deinit_debug(a, __FILE__, __LINE)
#else
void k_event_init(struct k_event *event);
void k_event_deinit(struct k_event *event);
#endif
/**
 * @brief Post one or more events to an event object
 *
 * This routine posts one or more events to an event object. All tasks waiting
 * on the event object @a event whose waiting conditions become met by this
 * posting immediately unpend.
 *
 * Posting differs from setting in that posted events are merged together with
 * the current set of events tracked by the event object.
 *
 * @param event Address of the event object
 * @param events Set of events to post to @a event
 *
 * @retval Previous value of the events in @a event
 */
uint32_t k_event_post(struct k_event *event, uint32_t events);

/**
 * @brief Set the events in an event object
 *
 * This routine sets the events stored in event object to the specified value.
 * All tasks waiting on the event object @a event whose waiting conditions
 * become met by this immediately unpend.
 *
 * Setting differs from posting in that set events replace the current set of
 * events tracked by the event object.
 *
 * @param event Address of the event object
 * @param events Set of events to set in @a event
 *
 * @retval Previous value of the events in @a event
 */
uint32_t k_event_set(struct k_event *event, uint32_t events);

/**
 * @brief Clear the events in an event object
 *
 * This routine clears (resets) the specified events stored in an event object.
 *
 * @param event Address of the event object
 * @param events Set of events to clear in @a event
 *
 * @retval Previous value of the events in @a event
 */
uint32_t k_event_clear(struct k_event *event, uint32_t events);

/**
 * @brief Wait for any of the specified events
 *
 * This routine waits on event object @a event until any of the specified
 * events have been delivered to the event object, or the maximum wait time
 * @a timeout has expired. A thread may wait on up to 32 distinctly numbered
 * events that are expressed as bits in a single 32-bit word.
 *
 * @note The caller must be careful when resetting if there are multiple threads
 * waiting for the event object @a event.
 *
 * @param event Address of the event object
 * @param events Set of desired events on which to wait
 * @param reset If true, clear the set of events tracked by the event object
 *              before waiting. If false, do not clear the events.
 * @param timeout Waiting period for the desired set of events or one of the
 *                special values K_NO_WAIT and K_FOREVER.
 *
 * @retval set of matching events upon success
 * @retval 0 if matching events were not received within the specified time
 */
uint32_t k_event_wait(struct k_event *event, uint32_t events, bool reset, k_timeout_t timeout);

/** @} */

struct k_fifo {
    struct k_queue _queue;
};

/**
 * @cond INTERNAL_HIDDEN
 */

/**
 * INTERNAL_HIDDEN @endcond
 */
#define _K_FIFO_INITIALIZER(obj)                   \
    {                                              \
        ._queue = _K_QUEUE_INITIALIZER(obj._queue) \
    }
#define K_FIFO_INITIALIZER  _K_FIFO_INITIALIZER

/**
 * @defgroup fifo_apis FIFO APIs
 * @ingroup kernel_apis
 * @{
 */
#define K_FIFO_DEFINE(name) struct k_fifo name __in_section(_k_queue, static, name) = _K_FIFO_INITIALIZER(name)

/**
 * @brief Initialize a FIFO queue.
 *
 * This routine initializes a FIFO queue, prior to its first use.
 *
 * @param fifo Address of the FIFO queue.
 */
#ifdef KERNEL_MEMORY_DEBUG
void k_fifo_init_debug(struct k_fifo *fifo, const char *file, int line);
void k_fifo_deinit_debug(struct k_fifo *fifo, const char *file, int line);
#define k_fifo_init(a)   k_fifo_init_debug(a, __FILE__, __LINE__)
#define k_fifo_deinit(a) k_fifo_deinit_debug(a, __FILE__, __LINE__)
#else
#define k_fifo_init(fifo)   ({ k_queue_init(&(fifo)->_queue); })

#define k_fifo_deinit(fifo) ({ k_queue_deinit(&(fifo)->_queue); })
#endif
/**
 * @brief Add an element to a FIFO queue.
 *
 * This routine adds a data item to @a fifo. A FIFO data item must be
 * aligned on a word boundary, and the first word of the item is reserved
 * for the kernel's use.
 *
 * @note isr_ok
 *
 * @param fifo Address of the FIFO.
 * @param data Address of the data item.
 */

#define k_fifo_put(fifo, data) ({ k_queue_append(&(fifo)->_queue, data); })

/**
 * @brief Get an element from a FIFO queue.
 *
 * This routine removes a data item from @a fifo in a "first in, first out"
 * manner. The first word of the data item is reserved for the kernel's use.
 *
 * @note @a timeout must be set to K_NO_WAIT if called from ISR.
 *
 * @note isr_ok
 *
 * @param fifo Address of the FIFO queue.
 * @param timeout Waiting period to obtain a data item,
 *                or one of the special values K_NO_WAIT and K_FOREVER.
 *
 * @return Address of the data item if successful; NULL if returned
 * without waiting, or waiting period timed out.
 */
#define k_fifo_get(fifo, timeout)                             \
    ({                                                        \
        void *fg_ret = k_queue_get(&(fifo)->_queue, timeout); \
        fg_ret;                                               \
    })

/**
 * @brief Query a FIFO queue to see if it has data available.
 *
 * Note that the data might be already gone by the time this function returns
 * if other threads is also trying to read from the FIFO.
 *
 * @note isr_ok
 *
 * @param fifo Address of the FIFO queue.
 *
 * @return Non-zero if the FIFO queue is empty.
 * @return 0 if data is available.
 */
#define k_fifo_is_empty(fifo) k_queue_is_empty(&(fifo)->_queue)

/**
 * @brief Peek element at the head of a FIFO queue.
 *
 * Return element from the head of FIFO queue without removing it. A usecase
 * for this is if elements of the FIFO object are themselves containers. Then
 * on each iteration of processing, a head container will be peeked,
 * and some data processed out of it, and only if the container is empty,
 * it will be completely remove from the FIFO queue.
 *
 * @param fifo Address of the FIFO queue.
 *
 * @return Head element, or NULL if the FIFO queue is empty.
 */
#define k_fifo_peek_head(fifo)                              \
    ({                                                      \
        void *fph_ret = k_queue_peek_head(&(fifo)->_queue); \
        fph_ret;                                            \
    })

/** @} */

#define _K_QUEUE_INITIALIZER(obj) \
    {                             \
        0                         \
    }
#define K_QUEUE_INITIALIZER _K_QUEUE_INITIALIZER

struct k_lifo {
    struct k_queue _queue;
};
/**
 * @cond INTERNAL_HIDDEN
 */

#define _K_LIFO_INITIALIZER(obj)                   \
    {                                              \
        ._queue = _K_QUEUE_INITIALIZER(obj._queue) \
    }

#define Z_LIFO_INITIALIZER _K_LIFO_INITIALIZER
/**
 * INTERNAL_HIDDEN @endcond
 */

/**
 * @defgroup lifo_apis LIFO APIs
 * @ingroup kernel_apis
 * @{
 */

#ifdef KERNEL_MEMORY_DEBUG
void k_lifo_init_debug(struct k_lifo *lifo, const char *file, int line);
void k_lifo_deinit_debug(struct k_lifo *lifo, const char *file, int line);
#define k_lifo_init(a)   k_lifo_init_debug(a, __FILE__, __LINE__)
#define k_lifo_deinit(a) k_lifo_deinit_debug(a, __FILE__, __LINE__)
#else

#define k_lifo_init(lifo)   ({ k_queue_init(&(lifo)->_queue); })
#define k_lifo_deinit(lifo) ({ k_queue_deinit(&(lifo)->_queue); })
#endif

#define k_lifo_put(lifo, data) ({ k_queue_prepend(&(lifo)->_queue, data); })

#define k_lifo_get(lifo, timeout)                          \
    ({                                                     \
        void *ret = k_queue_get(&(lifo)->_queue, timeout); \
        ret;                                               \
    })

/** @} */

/**
 * @defgroup mutex_apis Mutex APIs
 * @ingroup kernel_apis
 * @{
 */

/**
 * Mutex Structure
 * @ingroup mutex_apis
 */
struct k_mutex {
    wm_os_mutex_t *mutex;
    sys_dlist_t poll_events;
};

/**
 * @brief Statically define and initialize a mutex.
 *
 * The mutex can be accessed outside the module where it is defined using:
 *
 * @code extern struct k_mutex <name>; @endcode
 *
 * @param name Name of the mutex.
 */

/**
 * @brief Initialize a mutex.
 *
 * This routine initializes a mutex object, prior to its first use.
 *
 * Upon completion, the mutex is available and does not have an owner.
 *
 * @param mutex Address of the mutex.
 *
 * @retval 0 Mutex object created
 *
 */

#ifdef KERNEL_MEMORY_DEBUG
int k_mutex_init_debug(struct k_mutex *mutex, const char *file, int line);
int k_mutex_deinit_debug(struct k_mutex *mutex, const char *file, int line);
#define k_mutex_init(a)   k_mutex_init_debug(a, __FILE__, __LINE__)
#define k_mutex_deinit(a) k_mutex_deinit_debug(a, __FILE__, __LINE__)
#else
int k_mutex_init(struct k_mutex *mutex);
int k_mutex_deinit(struct k_mutex *mutex);
#endif

/**
 * @brief Lock a mutex.
 *
 * This routine locks @a mutex. If the mutex is locked by another thread,
 * the calling thread waits until the mutex becomes available or until
 * a timeout occurs.
 *
 * A thread is permitted to lock a mutex it has already locked. The operation
 * completes immediately and the lock count is increased by 1.
 *
 * Mutexes may not be locked in ISRs.
 *
 * @param mutex Address of the mutex.
 * @param timeout Waiting period to lock the mutex,
 *                or one of the special values K_NO_WAIT and
 *                K_FOREVER.
 *
 * @retval 0 Mutex locked.
 * @retval -EBUSY Returned without waiting.
 * @retval -EAGAIN Waiting period timed out.
 */
int k_mutex_lock(int pos, struct k_mutex *mutex, k_timeout_t timeout);

/**
 * @brief Unlock a mutex.
 *
 * This routine unlocks @a mutex. The mutex must already be locked by the
 * calling thread.
 *
 * The mutex cannot be claimed by another thread until it has been unlocked by
 * the calling thread as many times as it was previously locked by that
 * thread.
 *
 * Mutexes may not be unlocked in ISRs, as mutexes must only be manipulated
 * in thread context due to ownership and priority inheritance semantics.
 *
 * @param mutex Address of the mutex.
 *
 * @retval 0 Mutex unlocked.
 * @retval -EPERM The current thread does not own the mutex
 * @retval -EINVAL The mutex is not locked
 *
 */
int k_mutex_unlock(struct k_mutex *mutex);

/**
 * @}
 */

/**
 * @cond INTERNAL_HIDDEN
 */

struct k_sem {
    wm_os_sem_t *sem;
    unsigned int count;
    unsigned int limit;
    sys_dlist_t poll_events;
};

/**
 * INTERNAL_HIDDEN @endcond
 */

/**
 * @defgroup semaphore_apis Semaphore APIs
 * @ingroup kernel_apis
 * @{
 */

#define Z_SEM_INITIALIZER(obj, initial_count, count_limit) \
    {                                                      \
    }

/**
 * @brief Initialize a semaphore.
 *
 * This routine initializes a semaphore object, prior to its first use.
 *
 * @param sem Address of the semaphore.
 * @param initial_count Initial semaphore count.
 * @param limit Maximum permitted semaphore count.
 *
 * @see K_SEM_MAX_LIMIT
 *
 * @retval 0 Semaphore created successfully
 * @retval -EINVAL Invalid values
 *
 */
#ifdef KERNEL_MEMORY_DEBUG
int k_sem_init_debug(struct k_sem *sem, uint32_t initial_count, uint32_t limit, const char *file, int line);
void k_sem_deinit_debug(struct k_sem *sem, const char *file, int line);
void k_sem_give_debug(struct k_sem *sem, const char *file, int line);

#define k_sem_init(a, b, c) k_sem_init_debug(a, b, c, __FILE__, __LINE__)
#define k_sem_deinit(a)     k_sem_deinit_debug(a, __FILE__, __LINE__)
#define k_sem_give(a)       k_sem_give_debug(a, __FILE__, __LINE__)
#else
int k_sem_init(struct k_sem *sem, uint32_t initial_count, uint32_t limit);

void k_sem_deinit(struct k_sem *sem);

void k_sem_give(struct k_sem *sem);

#endif

/**
 * @brief Take a semaphore.
 *
 * This routine takes @a sem.
 *
 * @note @a timeout must be set to K_NO_WAIT if called from ISR.
 *
 * @note isr_ok
 *
 * @param sem Address of the semaphore.
 * @param timeout Waiting period to take the semaphore,
 *                or one of the special values K_NO_WAIT and K_FOREVER.
 *
 * @retval 0 Semaphore taken.
 * @retval -EBUSY Returned without waiting.
 * @retval -EAGAIN Waiting period timed out,
 *			or the semaphore was reset during the waiting period.
 */
int k_sem_take(struct k_sem *sem, k_timeout_t timeout);

/**
  * @brief Resets a semaphore's count to zero.
 *
  * This routine sets the count of @a sem to zero.
  * Any outstanding semaphore takes will be aborted
  * with -EAGAIN.
 *
 * @param sem Address of the semaphore.
 */
void k_sem_reset(struct k_sem *sem);

/**
 * @brief Get a semaphore's count.
 *
 * This routine returns the current count of @a sem.
 *
 * @param sem Address of the semaphore.
 *
 * @return Current semaphore count.
 */
unsigned int k_sem_count_get(struct k_sem *sem);

/**
 * @brief Statically define and initialize a semaphore.
 *
 * The semaphore can be accessed outside the module where it is defined using:
 *
 * @code extern struct k_sem <name>; @endcode
 *
 * @param name Name of the semaphore.
 * @param initial_count Initial semaphore count.
 * @param count_limit Maximum permitted semaphore count.
 */
#define K_SEM_DEFINE(name, initial_count, count_limit) \
    STRUCT_SECTION_ITERABLE(k_sem, name) = Z_SEM_INITIALIZER(name, initial_count, count_limit);

/** @} */

struct k_work_queue_config {
    /** The name to be given to the work queue thread.
	 *
	 * If left null the thread will not have a name.
	 */
    const char *name;

    /** Control whether the work queue thread should yield between
	 * items.
	 *
	 * Yielding between items helps guarantee the work queue
	 * thread does not starve other threads, including cooperative
	 * ones released by a work item.  This is the default behavior.
	 *
	 * Set this to @c true to prevent the work queue thread from
	 * yielding between items.  This may be appropriate when a
	 * sequence of items should complete without yielding
	 * control.
	 */
    bool no_yield;
};

struct k_work_q {
    struct k_thread thread;
    struct k_fifo fifo;
};
enum {
    K_WORK_STATE_PENDING,
    K_WORK_STATE_PERIODIC,
};

enum {
    /**
 * @cond INTERNAL_HIDDEN
 */

    /* The atomic API is used for all work and queue flags fields to
	 * enforce sequential consistency in SMP environments.
	 */

    /* Bits that represent the work item states.  At least nine of the
	 * combinations are distinct valid stable states.
	 */
    K_WORK_RUNNING_BIT   = 0,
    K_WORK_CANCELING_BIT = 1,
    K_WORK_QUEUED_BIT    = 2,
    K_WORK_DELAYED_BIT   = 3,

    K_WORK_MASK = BIT(K_WORK_DELAYED_BIT) | BIT(K_WORK_QUEUED_BIT) | BIT(K_WORK_RUNNING_BIT) | BIT(K_WORK_CANCELING_BIT),

    /* Static work flags */
    K_WORK_DELAYABLE_BIT = 8,
    K_WORK_DELAYABLE     = BIT(K_WORK_DELAYABLE_BIT),

    /* Dynamic work queue flags */
    K_WORK_QUEUE_STARTED_BIT = 0,
    K_WORK_QUEUE_STARTED     = BIT(K_WORK_QUEUE_STARTED_BIT),
    K_WORK_QUEUE_BUSY_BIT    = 1,
    K_WORK_QUEUE_BUSY        = BIT(K_WORK_QUEUE_BUSY_BIT),
    K_WORK_QUEUE_DRAIN_BIT   = 2,
    K_WORK_QUEUE_DRAIN       = BIT(K_WORK_QUEUE_DRAIN_BIT),
    K_WORK_QUEUE_PLUGGED_BIT = 3,
    K_WORK_QUEUE_PLUGGED     = BIT(K_WORK_QUEUE_PLUGGED_BIT),

    /* Static work queue flags */
    K_WORK_QUEUE_NO_YIELD_BIT = 8,
    K_WORK_QUEUE_NO_YIELD     = BIT(K_WORK_QUEUE_NO_YIELD_BIT),

    /**
 * INTERNAL_HIDDEN @endcond
 */
    /* Transient work flags */

    /** @brief Flag indicating a work item that is running under a work
	 * queue thread.
	 *
	 * Accessed via k_work_busy_get().  May co-occur with other flags.
	 */
    K_WORK_RUNNING = BIT(K_WORK_RUNNING_BIT),

    /** @brief Flag indicating a work item that is being canceled.
	 *
	 * Accessed via k_work_busy_get().  May co-occur with other flags.
	 */
    K_WORK_CANCELING = BIT(K_WORK_CANCELING_BIT),

    /** @brief Flag indicating a work item that has been submitted to a
	 * queue but has not started running.
	 *
	 * Accessed via k_work_busy_get().  May co-occur with other flags.
	 */
    K_WORK_QUEUED = BIT(K_WORK_QUEUED_BIT),

    /** @brief Flag indicating a delayed work item that is scheduled for
	 * submission to a queue.
	 *
	 * Accessed via k_work_busy_get().  May co-occur with other flags.
	 */
    K_WORK_DELAYED = BIT(K_WORK_DELAYED_BIT),

    K_WORK_STATE_FLAGS,
};

/**
 * @cond INTERNAL_HIDDEN
 */

struct k_work;
extern struct k_work_q k_sys_work_q;

/**
 * INTERNAL_HIDDEN @endcond
 */

/**
 * @defgroup workqueue_apis Work Queue APIs
 * @ingroup kernel_apis
 * @{
 */

/** @brief The signature for a work item handler function.
 *
 * The function will be invoked by the thread animating a work queue.
 *
 * @param work the work item that provided the handler.
 */

typedef void (*k_work_handler_t)(struct k_work *work);

struct k_work {
    /* The function to be invoked by the work queue thread. */
    k_work_handler_t handler;
    /* The queue on which the work item was last submitted. */
    struct k_work_q *queue;
    uint32_t flags;
    void *priv_data;
};

#define _K_WORK_INITIALIZER(work_handler)                      \
    {                                                          \
        .priv_data = NULL, .handler = work_handler, .flags = 0 \
    }

#define Z_WORK_INITIALIZER                _K_WORK_INITIALIZER

/**
 * @brief Initialize a statically-defined work item.
 *
 * This macro can be used to initialize a statically-defined workqueue work
 * item, prior to its first use. For example,
 *
 * @code static K_WORK_DEFINE(<work>, <work_handler>); @endcode
 *
 * @param work Symbol name for work item object
 * @param work_handler Function to invoke each time work item is processed.
 */
#define K_WORK_DEFINE(work, work_handler) struct k_work work = Z_WORK_INITIALIZER(work_handler)

int k_work_init(struct k_work *work, k_work_handler_t handler);

/** @brief Busy state flags from the work item.
 *
 * A zero return value indicates the work item appears to be idle.
 *
 * @note This is a live snapshot of state, which may change before the result
 * is checked.  Use locks where appropriate.
 *
 * @note isr_ok
 *
 * @param work pointer to the work item.
 *
 * @return a mask of flags K_WORK_DELAYED, K_WORK_QUEUED,
 * K_WORK_RUNNING, and K_WORK_CANCELING.
 */
int k_work_busy_get(const struct k_work *work);

/** @brief Test whether a work item is currently pending.
 *
 * Wrapper to determine whether a work item is in a non-idle dstate.
 *
 * @note This is a live snapshot of state, which may change before the result
 * is checked.  Use locks where appropriate.
 *
 * @note isr_ok
 *
 * @param work pointer to the work item.
 *
 * @return true if and only if k_work_busy_get() returns a non-zero value.
 */
bool k_work_is_pending(const struct k_work *work);

/** @brief Submit a work item to a queue.
 *
 * @param queue pointer to the work queue on which the item should run.  If
 * NULL the queue from the most recent submission will be used.
 *
 * @note isr_ok
 *
 * @param work pointer to the work item.
 *
 * @retval 0 if work was already submitted to a queue
 * @retval 1 if work was not submitted and has been queued to @p queue
 * @retval 2 if work was running and has been queued to the queue that was
 * running it
 * @retval -EBUSY
 * * if work submission was rejected because the work item is cancelling; or
 * * @p queue is draining; or
 * * @p queue is plugged.
 * @retval -EINVAL if @p queue is null and the work item has never been run.
 * @retval -ENODEV if @p queue has not been started.
 */
int k_work_submit_to_queue(int pos, struct k_work_q *queue, struct k_work *work);

/** @brief Submit a work item to the system queue.
*
* @note isr_ok
*
* @param work pointer to the work item.
*
* @return as with k_work_submit_to_queue().
*/
extern int k_work_submit(struct k_work *work);

/** @brief Cancel a work item.
 *
 * This attempts to prevent a pending (non-delayable) work item from being
 * processed by removing it from the work queue.  If the item is being
 * processed, the work item will continue to be processed, but resubmissions
 * are rejected until cancellation completes.
 *
 * If this returns zero cancellation is complete, otherwise something
 * (probably a work queue thread) is still referencing the item.
 *
 * See also k_work_cancel_sync().
 *
 * @note isr_ok
 *
 * @param work pointer to the work item.
 *
 * @return the k_work_busy_get() status indicating the state of the item after all
 * cancellation steps performed by this call are completed.
 */
int k_work_cancel(struct k_work *work);

/** @brief Initialize a work queue structure.
 *
 * This must be invoked before starting a work queue structure for the first time.
 * It need not be invoked again on the same work queue structure.
 *
 * @note isr_ok
 *
 * @param queue the queue structure to be initialized.
 */

#ifdef KERNEL_MEMORY_DEBUG
void k_work_queue_init_debug(struct k_work_q *queue, const char *FILE, int line);
void k_work_queue_deinit_debug(struct k_work_q *queue, const char *file, int line);

#define k_work_queue_init(a)   k_work_queue_init_debug(a, __FILE__, __LINE__)
#define k_work_queue_deinit(a) k_work_queue_deinit_debug(a, __FILE__, __LINE__)

#else
void k_work_queue_init(struct k_work_q *queue);

void k_work_queue_deinit(struct k_work_q *queue);
#endif

/** @brief Initialize a work queue.
 *
 * This configures the work queue thread and starts it running.  The function
 * should not be re-invoked on a queue.
 *
 * @param queue pointer to the queue structure. It must be initialized
 *        in zeroed/bss memory or with @ref k_work_queue_init before
 *        use.
 *
 * @param stack pointer to the work thread stack area.
 *
 * @param stack_size size of the the work thread stack area, in bytes.
 *
 * @param prio initial thread priority
 *
 * @param cfg optional additional configuration parameters.  Pass @c
 * NULL if not required, to use the defaults documented in
 * k_work_queue_config.
 */
void k_work_queue_start(struct k_work_q *queue, k_thread_stack_t *stack, size_t stack_size, int prio,
                        const struct k_work_queue_config *cfg);

struct k_work_delayable {
    struct k_work work;
    struct k_work_q *work_q;
    struct k_timer timer;
};

#define Z_WORK_DELAYABLE_INITIALIZER(work_handler) \
    {                                              \
        .work = {                                  \
            .handler = work_handler,               \
            .flags   = K_WORK_DELAYABLE,           \
        },                                         \
    }

#define K_WORK_DELAYABLE_DEFINE(work, work_handler) struct k_work_delayable work = Z_WORK_DELAYABLE_INITIALIZER(work_handler)

/** @brief Initialize a delayable work structure.
 *
 * This must be invoked before scheduling a delayable work structure for the
 * first time.  It need not be invoked again on the same work structure.  It
 * can be re-invoked to change the associated handler, but this must be done
 * when the work item is idle.
 *
 * @note isr_ok
 *
 * @param dwork the delayable work structure to be initialized.
 *
 * @param handler the handler to be invoked by the work item.
 */

#ifdef KERNEL_MEMORY_DEBUG
void k_work_init_delayable_debug(struct k_work_delayable *dwork, k_work_handler_t handler, const char *file, int line);

void k_work_deinit_delayable_debug(struct k_work_delayable *dwork, const char *file, int line);
#define k_work_init_delayable(a, b) k_work_init_delayable_debug(a, b, __FILE__, __LINE__)
#define k_work_deinit_delayable(a)  k_work_deinit_delayable_debug(a, __FILE__, __LINE__)

#else
void k_work_init_delayable(struct k_work_delayable *dwork, k_work_handler_t handler);

void k_work_deinit_delayable(struct k_work_delayable *dwork);
#endif
static inline k_ticks_t k_work_delayable_expires_get(struct k_work_delayable *dwork)
{
    return k_timer_expires_ticks(&dwork->timer);
}

static inline k_ticks_t k_work_delayable_remaining_get(struct k_work_delayable *dwork)
{
    return k_timer_remaining_ticks(&dwork->timer);
}

/**
* @brief Get the parent delayable work structure from a work pointer.
*
* This function is necessary when a @c k_work_handler_t function is passed to
* k_work_schedule_for_queue() and the handler needs to access data from the
* container of the containing `k_work_delayable`.
*
* @param work Address passed to the work handler
*
* @return Address of the containing @c k_work_delayable structure.
*/
struct k_work_delayable *k_work_delayable_from_work(struct k_work *work);

/** @brief Busy state flags from the delayable work item.
 *
 * @note isr_ok
 *
 * @note This is a live snapshot of state, which may change before the result
 * can be inspected.  Use locks where appropriate.
 *
 * @param dwork pointer to the delayable work item.
 *
 * @return a mask of flags K_WORK_DELAYED, K_WORK_QUEUED, K_WORK_RUNNING, and
 * K_WORK_CANCELING.  A zero return value indicates the work item appears to
 * be idle.
 */
int k_work_delayable_busy_get(const struct k_work_delayable *dwork);

/** @brief Test whether a delayed work item is currently pending.
 *
 * Wrapper to determine whether a delayed work item is in a non-idle state.
 *
 * @note This is a live snapshot of state, which may change before the result
 * can be inspected.  Use locks where appropriate.
 *
 * @note isr_ok
 *
 * @param dwork pointer to the delayable work item.
 *
 * @return true if and only if k_work_delayable_busy_get() returns a non-zero
 * value.
 */
bool k_work_delayable_is_pending(const struct k_work_delayable *dwork);

/** @brief Submit an idle work item to a queue after a delay.
 *
 * Unlike k_work_reschedule_for_queue() this is a no-op if the work item is
 * already scheduled or submitted, even if @p delay is @c K_NO_WAIT.
 *
 * @note isr_ok
 *
 * @param queue the queue on which the work item should be submitted after the
 * delay.
 *
 * @param dwork pointer to the delayable work item.
 *
 * @param delay the time to wait before submitting the work item.  If @c
 * K_NO_WAIT and the work is not pending this is equivalent to
 * k_work_submit_to_queue().
 *
 * @retval 0 if work was already scheduled or submitted.
 * @retval 1 if work has been scheduled.
 * @retval -EBUSY if @p delay is @c K_NO_WAIT and
 *         k_work_submit_to_queue() fails with this code.
 * @retval -EINVAL if @p delay is @c K_NO_WAIT and
 *         k_work_submit_to_queue() fails with this code.
 * @retval -ENODEV if @p delay is @c K_NO_WAIT and
 *         k_work_submit_to_queue() fails with this code.
 */
int k_work_schedule_for_queue(struct k_work_q *queue, struct k_work_delayable *dwork, k_timeout_t delay);

/** @brief Submit an idle work item to the system work queue after a
* delay.
*
* This is a thin wrapper around k_work_schedule_for_queue(), with all the API
* characteristics of that function.
*
* @param dwork pointer to the delayable work item.
*
* @param delay the time to wait before submitting the work item.  If @c
* K_NO_WAIT this is equivalent to k_work_submit_to_queue().
*
* @return as with k_work_schedule_for_queue().
*/
extern int k_work_schedule(struct k_work_delayable *dwork, k_timeout_t delay);

/** @brief Reschedule a work item to a queue after a delay.
*
* Unlike k_work_schedule_for_queue() this function can change the deadline of
* a scheduled work item, and will schedule a work item that is in any state
* (e.g. is idle, submitted, or running).  This function does not affect
* ("unsubmit") a work item that has been submitted to a queue.
*
* @note isr_ok
*
* @param queue the queue on which the work item should be submitted after the
* delay.
*
* @param dwork pointer to the delayable work item.
*
* @param delay the time to wait before submitting the work item.  If @c
* K_NO_WAIT this is equivalent to k_work_submit_to_queue() after canceling
* any previous scheduled submission.
*
* @note If delay is @c K_NO_WAIT ("no delay") the return values are as with
* k_work_submit_to_queue().
*
* @retval 0 if delay is @c K_NO_WAIT and work was already on a queue
* @retval 1 if
* * delay is @c K_NO_WAIT and work was not submitted but has now been queued
*   to @p queue; or
* * delay not @c K_NO_WAIT and work has been scheduled
* @retval 2 if delay is @c K_NO_WAIT and work was running and has been queued
* to the queue that was running it
* @retval -EBUSY if @p delay is @c K_NO_WAIT and
*         k_work_submit_to_queue() fails with this code.
* @retval -EINVAL if @p delay is @c K_NO_WAIT and
*         k_work_submit_to_queue() fails with this code.
* @retval -ENODEV if @p delay is @c K_NO_WAIT and
*         k_work_submit_to_queue() fails with this code.
*/
int k_work_reschedule_for_queue(struct k_work_q *queue, struct k_work_delayable *dwork, k_timeout_t delay);

/** @brief Reschedule a work item to the system work queue after a
 * delay.
 *
 * This is a thin wrapper around k_work_reschedule_for_queue(), with all the
 * API characteristics of that function.
 *
 * @param dwork pointer to the delayable work item.
 *
 * @param delay the time to wait before submitting the work item.
 *
 * @return as with k_work_reschedule_for_queue().
 */
extern int k_work_reschedule(struct k_work_delayable *dwork, k_timeout_t delay);
/** @brief Cancel delayable work.
 *
 * Similar to k_work_cancel() but for delayable work.  If the work is
 * scheduled or submitted it is canceled.  This function does not wait for the
 * cancellation to complete.
 *
 * @note The work may still be running when this returns.  Use
 * k_work_flush_delayable() or k_work_cancel_delayable_sync() to ensure it is
 * not running.
 *
 * @note Canceling delayable work does not prevent rescheduling it.  It does
 * prevent submitting it until the cancellation completes.
 *
 * @note isr_ok
 *
 * @param dwork pointer to the delayable work item.
 *
 * @return the k_work_delayable_busy_get() status indicating the state of the
 * item after all cancellation steps performed by this call are completed.
 */
int k_work_cancel_delayable(struct k_work_delayable *dwork);

/**
 * @cond INTERNAL_HIDDEN
 */

struct z_work_flusher {
    struct k_work work;
    struct k_sem sem;
};

/* Record used to wait for work to complete a cancellation.
 *
 * The work item is inserted into a global queue of pending cancels.
 * When a cancelling work item goes idle any matching waiters are
 * removed from pending_cancels and are woken.
 */
struct z_work_canceller {
    struct k_work *work;
    struct k_sem sem;
};

/**
 * INTERNAL_HIDDEN @endcond
 */

/** @brief A structure holding internal state for a pending synchronous
 * operation on a work item or queue.
 *
 * Instances of this type are provided by the caller for invocation of
 * k_work_flush(), k_work_cancel_sync() and sibling flush and cancel APIs.  A
 * referenced object must persist until the call returns, and be accessible
 * from both the caller thread and the work queue thread.
 *
 * @note If CONFIG_KERNEL_COHERENCE is enabled the object must be allocated in
 * coherent memory; see arch_mem_coherent().  The stack on these architectures
 * is generally not coherent.  be stack-allocated.  Violations are detected by
 * runtime assertion.
 */
struct k_work_sync {
    union {
        struct z_work_flusher flusher;
        struct z_work_canceller canceller;
    };
};
/** @brief Cancel delayable work and wait.
 *
 * Like k_work_cancel_delayable() but waits until the work becomes idle.
 *
 * @note Canceling delayable work does not prevent rescheduling it.  It does
 * prevent submitting it until the cancellation completes.
 *
 * @note Be careful of caller and work queue thread relative priority.  If
 * this function sleeps it will not return until the work queue thread
 * completes the tasks that allow this thread to resume.
 *
 * @note Behavior is undefined if this function is invoked on @p dwork from a
 * work queue running @p dwork.
 *
 * @param dwork pointer to the delayable work item.
 *
 * @param sync pointer to an opaque item containing state related to the
 * pending cancellation.  The object must persist until the call returns, and
 * be accessible from both the caller thread and the work queue thread.  The
 * object must not be used for any other flush or cancel operation until this
 * one completes.  On architectures with CONFIG_KERNEL_COHERENCE the object
 * must be allocated in coherent memory.
 *
 * @retval true if work was not idle (call had to wait for cancellation of a
 * running handler to complete, or scheduled or submitted operations were
 * cancelled);
 * @retval false otherwise
 */
bool k_work_cancel_delayable_sync(struct k_work_delayable *dwork, struct k_work_sync *sync);

/** @} */

/**
 * @defgroup heap_apis Heap APIs
 * @ingroup kernel_apis
 * @{
 */

/**
 * @brief Allocate memory from the heap.
 *
 * This routine provides traditional malloc() semantics. Memory is
 * allocated from the heap memory pool.
 * Allocated memory is aligned on a multiple of pointer sizes.
 *
 * @param size Amount of memory requested (in bytes).
 *
 * @return Address of the allocated memory if successful; otherwise NULL.
 */
extern void *k_malloc(size_t size);

/**
 * @brief Free memory allocated from heap.
 *
 * This routine provides traditional free() semantics. The memory being
 * returned must have been allocated from the heap memory pool.
 *
 * If @a ptr is NULL, no operation is performed.
 *
 * @param ptr Pointer to previously allocated memory.
 */
extern void k_free(void *ptr);

/**
 * @brief Allocate memory from heap, array style
 *
 * This routine provides traditional calloc() semantics. Memory is
 * allocated from the heap memory pool and zeroed.
 *
 * @param nmemb Number of elements in the requested array
 * @param size Size of each array element (in bytes).
 *
 * @return Address of the allocated memory if successful; otherwise NULL.
 */
extern void *k_calloc(size_t nmemb, size_t size);

/** @} */

/* polling API - PRIVATE */

#ifdef CONFIG_POLL
#define _INIT_OBJ_POLL_EVENT(obj) \
    do {                          \
        (obj)->poll_event = NULL; \
    } while (false)
#else
#define _INIT_OBJ_POLL_EVENT(obj) \
    do {                          \
    } while (false)
#endif

/* private - types bit positions */
enum _poll_types_bits {
    /* can be used to ignore an event */
    _POLL_TYPE_IGNORE,

    /* to be signaled by k_poll_signal_raise() */
    _POLL_TYPE_SIGNAL,

    /* semaphore availability */
    _POLL_TYPE_SEM_AVAILABLE,

    /* queue/FIFO/LIFO data availability */
    _POLL_TYPE_DATA_AVAILABLE,

    /* msgq data availability */
    _POLL_TYPE_MSGQ_DATA_AVAILABLE,

    _POLL_NUM_TYPES
};

#define Z_POLL_TYPE_BIT(type) (1U << ((type) - 1U))

/* private - states bit positions */
enum _poll_states_bits {
    /* default state when creating event */
    _POLL_STATE_NOT_READY,

    /* signaled by k_poll_signal_raise() */
    _POLL_STATE_SIGNALED,

    /* semaphore is available */
    _POLL_STATE_SEM_AVAILABLE,

    /* data is available to read on queue/FIFO/LIFO */
    _POLL_STATE_DATA_AVAILABLE,

    /* queue/FIFO/LIFO wait was cancelled */
    _POLL_STATE_CANCELLED,

    /* data is available to read on a message queue */
    _POLL_STATE_MSGQ_DATA_AVAILABLE,

    _POLL_NUM_STATES
};

#define Z_POLL_STATE_BIT(state) (1U << ((state) - 1U))

#define _POLL_EVENT_NUM_UNUSED_BITS                             \
    (32 - (0 + 8                                    /* tag */   \
           + _POLL_NUM_TYPES + _POLL_NUM_STATES + 1 /* modes */ \
           ))

/* end of polling API - PRIVATE */

/**
 * @defgroup poll_apis Async polling APIs
 * @ingroup kernel_apis
 * @{
 */

/* Public polling API */

/* public - values for k_poll_event.type bitfield */
#define K_POLL_TYPE_IGNORE              0
#define K_POLL_TYPE_SIGNAL              Z_POLL_TYPE_BIT(_POLL_TYPE_SIGNAL)
#define K_POLL_TYPE_SEM_AVAILABLE       Z_POLL_TYPE_BIT(_POLL_TYPE_SEM_AVAILABLE)
#define K_POLL_TYPE_DATA_AVAILABLE      Z_POLL_TYPE_BIT(_POLL_TYPE_DATA_AVAILABLE)
#define K_POLL_TYPE_FIFO_DATA_AVAILABLE K_POLL_TYPE_DATA_AVAILABLE
#define K_POLL_TYPE_MSGQ_DATA_AVAILABLE Z_POLL_TYPE_BIT(_POLL_TYPE_MSGQ_DATA_AVAILABLE)

/* public - polling modes */
enum k_poll_modes {
    /* polling thread does not take ownership of objects when available */
    K_POLL_MODE_NOTIFY_ONLY = 0,

    K_POLL_NUM_MODES
};

/* public - values for k_poll_event.state bitfield */
#define K_POLL_STATE_NOT_READY           0
#define K_POLL_STATE_SIGNALED            Z_POLL_STATE_BIT(_POLL_STATE_SIGNALED)
#define K_POLL_STATE_SEM_AVAILABLE       Z_POLL_STATE_BIT(_POLL_STATE_SEM_AVAILABLE)
#define K_POLL_STATE_DATA_AVAILABLE      Z_POLL_STATE_BIT(_POLL_STATE_DATA_AVAILABLE)
#define K_POLL_STATE_FIFO_DATA_AVAILABLE K_POLL_STATE_DATA_AVAILABLE
#define K_POLL_STATE_MSGQ_DATA_AVAILABLE Z_POLL_STATE_BIT(_POLL_STATE_MSGQ_DATA_AVAILABLE)
#define K_POLL_STATE_CANCELLED           Z_POLL_STATE_BIT(_POLL_STATE_CANCELLED)

struct z_poller {
    bool is_polling;
    uint8_t mode;
};

/* public - poll signal object */
struct k_poll_signal {
    /** PRIVATE - DO NOT TOUCH */
    sys_dlist_t poll_events;

    /**
	 * 1 if the event has been signaled, 0 otherwise. Stays set to 1 until
	 * user resets it to 0.
	 */
    unsigned int signaled;

    /** custom result value passed to k_poll_signal_raise() if needed */
    int result;
};

#define K_POLL_SIGNAL_INITIALIZER(obj)                                                      \
    {                                                                                       \
        .poll_events = SYS_DLIST_STATIC_INIT(&obj.poll_events), .signaled = 0, .result = 0, \
    }
/**
 * @brief Poll Event
 *
 */
struct k_poll_event {
    /** PRIVATE - DO NOT TOUCH */
    sys_dnode_t _node;

    /** PRIVATE - DO NOT TOUCH */
    struct z_poller *poller;

    /** optional user-specified tag, opaque, untouched by the API */
    uint32_t tag : 8;

    /** bitfield of event types (bitwise-ORed K_POLL_TYPE_xxx values) */
    uint32_t type : _POLL_NUM_TYPES;

    /** bitfield of event states (bitwise-ORed K_POLL_STATE_xxx values) */
    uint32_t state : _POLL_NUM_STATES;

    /** mode of operation, from enum k_poll_modes */
    uint32_t mode : 1;

    /** unused bits in 32-bit word */
    uint32_t unused : _POLL_EVENT_NUM_UNUSED_BITS;

    /** per-type data */
    union {
        void *obj;
        struct k_poll_signal *signal;
        struct k_sem *sem;
        struct k_fifo *fifo;
        struct k_queue *queue;
        struct k_msgq *msgq;
    };
};

#define K_POLL_EVENT_INITIALIZER(_event_type, _event_mode, _event_obj)                                          \
    {                                                                                                           \
        .poller = NULL, .type = _event_type, .state = K_POLL_STATE_NOT_READY, .mode = _event_mode, .unused = 0, \
        {                                                                                                       \
            .obj = _event_obj,                                                                                  \
        },                                                                                                      \
    }

#define K_POLL_EVENT_STATIC_INITIALIZER(_event_type, _event_mode, _event_obj, event_tag)                          \
    {                                                                                                             \
        .tag = event_tag, .type = _event_type, .state = K_POLL_STATE_NOT_READY, .mode = _event_mode, .unused = 0, \
        {                                                                                                         \
            .obj = _event_obj,                                                                                    \
        },                                                                                                        \
    }
extern void k_sys_poll_sem_init(void);

extern void k_sys_poll_sem_deinit(void);

extern void k_sys_pool_sem_give(int pos);

/**
 * @brief Initialize one struct k_poll_event instance
 *
 * After this routine is called on a poll event, the event it ready to be
 * placed in an event array to be passed to k_poll().
 *
 * @param event The event to initialize.
 * @param type A bitfield of the types of event, from the K_POLL_TYPE_xxx
 *             values. Only values that apply to the same object being polled
 *             can be used together. Choosing K_POLL_TYPE_IGNORE disables the
 *             event.
 * @param mode Future. Use K_POLL_MODE_NOTIFY_ONLY.
 * @param obj Kernel object or poll signal.
 *
 * @return N/A
 */

extern void k_poll_event_init(struct k_poll_event *event, uint32_t type, int mode, void *obj);

/**
 * @brief Wait for one or many of multiple poll events to occur
 *
 * This routine allows a thread to wait concurrently for one or many of
 * multiple poll events to have occurred. Such events can be a kernel object
 * being available, like a semaphore, or a poll signal event.
 *
 * When an event notifies that a kernel object is available, the kernel object
 * is not "given" to the thread calling k_poll(): it merely signals the fact
 * that the object was available when the k_poll() call was in effect. Also,
 * all threads trying to acquire an object the regular way, i.e. by pending on
 * the object, have precedence over the thread polling on the object. This
 * means that the polling thread will never get the poll event on an object
 * until the object becomes available and its pend queue is empty. For this
 * reason, the k_poll() call is more effective when the objects being polled
 * only have one thread, the polling thread, trying to acquire them.
 *
 * When k_poll() returns 0, the caller should loop on all the events that were
 * passed to k_poll() and check the state field for the values that were
 * expected and take the associated actions.
 *
 * Before being reused for another call to k_poll(), the user has to reset the
 * state field to K_POLL_STATE_NOT_READY.
 *
 * When called from user mode, a temporary memory allocation is required from
 * the caller's resource pool.
 *
 * @param events An array of events to be polled for.
 * @param num_events The number of events in the array.
 * @param timeout Waiting period for an event to be ready,
 *                or one of the special values K_NO_WAIT and K_FOREVER.
 *
 * @retval 0 One or more events are ready.
 * @retval -EAGAIN Waiting period timed out.
 * @retval -EINTR Polling has been interrupted, e.g. with
 *         k_queue_cancel_wait(). All output events are still set and valid,
 *         cancelled event(s) will be set to K_POLL_STATE_CANCELLED. In other
 *         words, -EINTR status means that at least one of output events is
 *         K_POLL_STATE_CANCELLED.
 * @retval -ENOMEM Thread resource pool insufficient memory (user mode only)
 * @retval -EINVAL Bad parameters (user mode only)
 */

int k_poll(struct k_poll_event *events, int num_events, k_timeout_t timeout);

/**
 * @brief Initialize a poll signal object.
 *
 * Ready a poll signal object to be signaled via k_poll_signal_raise().
 *
 * @param sig A poll signal.
 *
 * @return N/A
 */

void k_poll_signal_init(struct k_poll_signal *sig);

/**
 * @brief Fetch the signaled state and result value of a poll signal
 *
 * @param sig A poll signal object
 * @param signaled An integer buffer which will be written nonzero if the
 *		   object was signaled
 * @param result An integer destination buffer which will be written with the
 *		   result value if the object was signaled, or an undefined
 *		   value if it was not.
 */
void k_poll_signal_check(struct k_poll_signal *sig, unsigned int *signaled, int *result);

/**
 * @brief Signal a poll signal object.
 *
 * This routine makes ready a poll signal, which is basically a poll event of
 * type K_POLL_TYPE_SIGNAL. If a thread was polling on that event, it will be
 * made ready to run. A @a result value can be specified.
 *
 * The poll signal contains a 'signaled' field that, when set by
 * k_poll_signal_raise(), stays set until the user sets it back to 0 with
 * k_poll_signal_reset(). It thus has to be reset by the user before being
 * passed again to k_poll() or k_poll() will consider it being signaled, and
 * will return immediately.
 *
 * @note The result is stored and the 'signaled' field is set even if
 * this function returns an error indicating that an expiring poll was
 * not notified.  The next k_poll() will detect the missed raise.
 *
 * @param sig A poll signal.
 * @param result The value to store in the result field of the signal.
 *
 * @retval 0 The signal was delivered successfully.
 * @retval -EAGAIN The polling thread's timeout is in the process of expiring.
 */

int k_poll_signal_raise(struct k_poll_signal *sig, int result);

/** @} */

struct k_mem_slab {
    _wait_q_t wait_q;
    uint32_t num_blocks;
    size_t block_size;
    char *buffer;
    char *free_list;
    uint32_t num_used;
#ifdef CONFIG_MEM_SLAB_TRACE_MAX_UTILIZATION
    uint32_t max_used;
#endif
};
/**
 * @cond INTERNAL_HIDDEN
 */

#define Z_MEM_SLAB_INITIALIZER(obj, slab_buffer, slab_block_size, slab_num_blocks)                                  \
    {                                                                                                               \
        .wait_q = SYS_DLIST_STATIC_INIT(&obj.wait_q), .num_blocks = slab_num_blocks, .block_size = slab_block_size, \
        .buffer = slab_buffer, .free_list = NULL, .num_used = 0,                                                    \
    }

#define K_MEM_SLAB_INITIALIZER __DEPRECATED_MACRO Z_MEM_SLAB_INITIALIZER

/**
 * INTERNAL_HIDDEN @endcond
 */

/**
 * @defgroup mem_slab_apis Memory Slab APIs
 * @ingroup kernel_apis
 * @{
 */

/**
 * @brief Statically define and initialize a memory slab in a public (non-static) scope.
 *
 * The memory slab's buffer contains @a slab_num_blocks memory blocks
 * that are @a slab_block_size bytes long. The buffer is aligned to a
 * @a slab_align -byte boundary. To ensure that each memory block is similarly
 * aligned to this boundary, @a slab_block_size must also be a multiple of
 * @a slab_align.
 *
 * The memory slab can be accessed outside the module where it is defined
 * using:
 *
 * @code extern struct k_mem_slab <name>; @endcode
 *
 * @note This macro cannot be used together with a static keyword.
 *       If such a use-case is desired, use @ref K_MEM_SLAB_DEFINE_STATIC
 *       instead.
 *
 * @param name Name of the memory slab.
 * @param slab_block_size Size of each memory block (in bytes).
 * @param slab_num_blocks Number memory blocks.
 * @param slab_align Alignment of the memory slab's buffer (power of 2).
 */
#if 0
#define K_MEM_SLAB_DEFINE(name, slab_block_size, slab_num_blocks, slab_align) \
    char __noinit_named(k_mem_slab_buf_##name) __aligned(WB_UP(slab_align))   \
        _k_mem_slab_buf_##name[(slab_num_blocks) * WB_UP(slab_block_size)];   \
    STRUCT_SECTION_ITERABLE(k_mem_slab, name) =                               \
        Z_MEM_SLAB_INITIALIZER(name, _k_mem_slab_buf_##name, WB_UP(slab_block_size), slab_num_blocks)
#else
#define K_MEM_SLAB_DEFINE(name, slab_block_size, slab_num_blocks, slab_align) static struct k_mem_slab name;

#endif
/**
 * @brief Initialize a memory slab.
 *
 * Initializes a memory slab, prior to its first use.
 *
 * The memory slab's buffer contains @a slab_num_blocks memory blocks
 * that are @a slab_block_size bytes long. The buffer must be aligned to an
 * N-byte boundary matching a word boundary, where N is a power of 2
 * (i.e. 4 on 32-bit systems, 8, 16, ...).
 * To ensure that each memory block is similarly aligned to this boundary,
 * @a slab_block_size must also be a multiple of N.
 *
 * @param slab Address of the memory slab.
 * @param buffer Pointer to buffer used for the memory blocks.
 * @param block_size Size of each memory block (in bytes).
 * @param num_blocks Number of memory blocks.
 *
 * @retval 0 on success
 * @retval -EINVAL invalid data supplied
 *
 */
extern int k_mem_slab_init(struct k_mem_slab *slab, void *buffer, size_t block_size, uint32_t num_blocks);

/**
 * @brief DeInitialize a memory slab.
 *
 * DeInitializes a memory slab.
 *
 * Free the memory slab's buffer contains @a slab_num_blocks memory blocks
 * that are @a slab_block_size bytes long.
 *
 * @retval 0 on success
 * @retval -EINVAL invalid data supplied
 *
 */
extern int k_mem_slab_deinit(struct k_mem_slab *slab);

/**
 * @brief Allocate memory from a memory slab.
 *
 * This routine allocates a memory block from a memory slab.
 *
 * @note @a timeout must be set to K_NO_WAIT if called from ISR.
 * @note When CONFIG_MULTITHREADING=n any @a timeout is treated as K_NO_WAIT.
 *
 * @note isr_ok
 *
 * @param slab Address of the memory slab.
 * @param mem Pointer to block address area.
 * @param timeout Non-negative waiting period to wait for operation to complete.
 *        Use K_NO_WAIT to return without waiting,
 *        or K_FOREVER to wait as long as necessary.
 *
 * @retval 0 Memory allocated. The block address area pointed at by @a mem
 *         is set to the starting address of the memory block.
 * @retval -ENOMEM Returned without waiting.
 * @retval -EAGAIN Waiting period timed out.
 * @retval -EINVAL Invalid data supplied
 */
extern int k_mem_slab_alloc(struct k_mem_slab *slab, void **mem, k_timeout_t timeout);

/**
 * @brief Free memory allocated from a memory slab.
 *
 * This routine releases a previously allocated memory block back to its
 * associated memory slab.
 *
 * @param slab Address of the memory slab.
 * @param mem Pointer to the memory block (as returned by k_mem_slab_alloc()).
 */
extern void k_mem_slab_free(struct k_mem_slab *slab, void *mem);

/**
 * @brief Get the number of used blocks in a memory slab.
 *
 * This routine gets the number of memory blocks that are currently
 * allocated in @a slab.
 *
 * @param slab Address of the memory slab.
 *
 * @return Number of allocated memory blocks.
 */
static inline uint32_t k_mem_slab_num_used_get(struct k_mem_slab *slab)
{
    return slab->num_used;
}

/**
 * @brief Get the number of maximum used blocks so far in a memory slab.
 *
 * This routine gets the maximum number of memory blocks that were
 * allocated in @a slab.
 *
 * @param slab Address of the memory slab.
 *
 * @return Maximum number of allocated memory blocks.
 */
static inline uint32_t k_mem_slab_max_used_get(struct k_mem_slab *slab)
{
#ifdef CONFIG_MEM_SLAB_TRACE_MAX_UTILIZATION
    return slab->max_used;
#else
    ARG_UNUSED(slab);
    return 0;
#endif
}

/**
 * @brief Get the number of unused blocks in a memory slab.
 *
 * This routine gets the number of memory blocks that are currently
 * unallocated in @a slab.
 *
 * @param slab Address of the memory slab.
 *
 * @return Number of unallocated memory blocks.
 */
static inline uint32_t k_mem_slab_num_free_get(struct k_mem_slab *slab)
{
    return slab->num_blocks - slab->num_used;
}

/** @} */

static inline uint32_t k_irq_lock(void)
{
    wm_os_internal_set_critical();
    return 0;
}

static inline void k_irq_unlock(uint32_t key)
{
    (void)key;
    return wm_os_internal_release_critical();
}

static inline uint32_t k_uptime_get_32()
{
    return wm_os_internal_get_time_ms();
}
static inline uint32_t k_ticks_to_ms_floor32(uint32_t ticks)
{
    return ticks * MSEC_PER_SEC / HZ;
}
static inline void k_sched_lock()
{
    return wm_os_internal_task_schedule_lock();
}

static inline void k_sched_unlock()
{
    return wm_os_internal_task_schedule_unlock();
}

extern int k_sys_work_init(void);

extern void k_sys_work_deinit(void);

void k_kernel_lock(void);

void k_kernel_unlock(void);

#ifdef __cplusplus
}
#endif

#endif /* !_ASMLANGUAGE */

#endif /* ZEPHYR_INCLUDE_KERNEL_H_ */
