#ifndef __ARCH_SYS_ARCH_H__
#define __ARCH_SYS_ARCH_H__

#include "wm_osal.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"

#define SYS_MBOX_NULL (void *)0
#define SYS_SEM_NULL  (void *)0

#define sys_arch_mbox_tryfetch(mbox,msg) \
      sys_arch_mbox_fetch(mbox,msg,1)

typedef SemaphoreHandle_t sys_sem_t;
typedef SemaphoreHandle_t sys_mbox_t;
typedef TaskHandle_t sys_thread_t;

typedef unsigned int sys_prot_t;

/* This optional function does a "fast" critical region protection and returns
the previous protection level. This function is only called during very short
critical regions. An embedded system which supports ISR-based drivers might want
to implement this function by disabling interrupts. Task-based systems might want
to implement this by using a mutex or disabling tasking. This function should
support recursive calls from the same task or interrupt. In other words,
sys_arch_protect() could be called while already protected. In that case the
return value indicates that it is already protected. */
extern sys_prot_t sys_arch_protect(void);

/* This optional function does a "fast" set of critical region protection to the
value specified by pval. See the documentation for sys_arch_protect() for more
information. This function is only required if your port is supporting an
operating system. */
extern void sys_arch_unprotect(sys_prot_t pval);

#endif /* __ARCH_SYS_ARCH_H__ */
