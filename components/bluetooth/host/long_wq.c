/* long_work.c - Workqueue intended for long-running operations. */

/*
 * Copyright (c) 2022 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <kernel.h>
#include "wm_component.h"

#define BT_LONG_WQ_USE_SYS_QUEUE 

#ifndef BT_LONG_WQ_USE_SYS_QUEUE
static struct k_work_q bt_long_wq;
#endif

int bt_long_wq_schedule(struct k_work_delayable *dwork, k_timeout_t timeout)
{
#ifdef BT_LONG_WQ_USE_SYS_QUEUE
    return k_work_reschedule(dwork, timeout);
#else
	return k_work_schedule_for_queue(&bt_long_wq, dwork, timeout);
#endif
}

int bt_long_wq_reschedule(struct k_work_delayable *dwork, k_timeout_t timeout)
{
#ifdef BT_LONG_WQ_USE_SYS_QUEUE
    return k_work_reschedule(dwork, timeout);
#else
	return k_work_reschedule_for_queue(&bt_long_wq, dwork, timeout);
#endif
}

int bt_long_wq_submit(struct k_work *work)
{
#ifdef BT_LONG_WQ_USE_SYS_QUEUE
    return k_work_submit(work);
#else
	return k_work_submit_to_queue(5, &bt_long_wq, work);
#endif
}

#ifndef BT_LONG_WQ_USE_SYS_QUEUE

static int long_wq_init(void)
{

	const struct k_work_queue_config cfg = {.name = "BT_LW_WQ"};

	k_work_queue_init(&bt_long_wq);

	k_work_queue_start(&bt_long_wq, NULL,
			   CONFIG_BT_LONG_WQ_STACK_SIZE,
			   CONFIG_BT_LONG_WQ_PRIO, &cfg);

	return 0;
}
WM_COMPONEN_INIT_2(long_wq_init);
#endif

//SYS_INIT(long_wq_init, POST_KERNEL, CONFIG_BT_LONG_WQ_INIT_PRIO);
