/*
 * WPA Supplicant / Example program entrypoint
 * Copyright (c) 2003-2005, Jouni Malinen <j@w1.fi>
 *
 * This software may be distributed under the terms of the BSD license.
 * See README for more details.
 */

#include "includes.h"

#include "common.h"
#include "utils/eloop.h"
#include "wpa_supplicant_i.h"

#include "wmsdk_config.h"
#include "wm_task_config.h"
#include "wm_osal.h"

struct wpas_task_arg {
    wm_os_sem_t *sync_sem;
    struct wpa_global *global;
};

static wm_os_sem_t *g_exit_sync_sem = NULL;

static void wpa_supplicant_task_exit_fun(void *arg)
{
    if (g_exit_sync_sem)
    {
        wm_os_internal_sem_release(g_exit_sync_sem);
        g_exit_sync_sem = NULL;
    }
}

static void wpa_supplicant_task(void *parameters)
{
    struct wpas_task_arg *task_arg = parameters;
    wm_os_sem_t *sync_sem = task_arg->sync_sem;
    struct wpa_global *global = task_arg->global;

    if (sync_sem)
    {
        if (wm_os_internal_sem_release(sync_sem))
        {
            printf("wpas sync failed\n");
        }
    }

    wm_os_internal_free(parameters);

    wpa_supplicant_run(global);

	wpa_supplicant_deinit(global);

	wm_os_internal_task_del_ex(NULL, wpa_supplicant_task_exit_fun, NULL);
}

static void wpa_supplicant_exit_trigger(void *eloop_ctx, void *user_ctx)
{
    eloop_terminate();
}

void wm_wpa_supplicant_deinit(void *sync_sem)
{
    g_exit_sync_sem = sync_sem;

    /* register eloop trigger wpas task exit */
    eloop_register_timeout(0, 0, wpa_supplicant_exit_trigger, NULL, NULL);

    //eloop_terminate();
}

void wm_wpa_supplicant_init(void *sync_sem)
{
	struct wpa_interface iface;
	int exitcode = 0;
#ifdef CONFIG_AP
    int i;
#endif
	struct wpa_params params;
	struct wpa_global *global;
	struct wpas_task_arg *task_arg = NULL;

	os_memset(&params, 0, sizeof(params));

#if CONFIG_WPA_SUPPLICANT_LOG_LEVEL_EXCESSIVE
	params.wpa_debug_level = MSG_EXCESSIVE;
#elif CONFIG_WPA_SUPPLICANT_LOG_LEVEL_MSGDUMP
    params.wpa_debug_level = MSG_MSGDUMP;
#elif CONFIG_WPA_SUPPLICANT_LOG_LEVEL_DEBUG
	params.wpa_debug_level = MSG_DEBUG;
#elif CONFIG_WPA_SUPPLICANT_LOG_LEVEL_WARNING
	params.wpa_debug_level = MSG_WARNING;
#elif CONFIG_WPA_SUPPLICANT_LOG_LEVEL_ERROR
	params.wpa_debug_level = MSG_ERROR;
//#elif CONFIG_WPA_SUPPLICANT_LOG_LEVEL_INFO
#else
	params.wpa_debug_level = MSG_INFO;
#endif

#if CONFIG_WPA_SUPPLICANT_ENABLE_DEBUG_SHOW_KEYS
	params.wpa_debug_show_keys = 1;
#endif

	global = wpa_supplicant_init(&params);
	if (global == NULL)
	{
        wpa_printf(MSG_ERROR, "Failed to init wpa_supplicant");
		return;
    }

#ifdef CONFIG_AP
    for (i = 0; i < 2; i++)
    {
#endif
    	os_memset(&iface, 0, sizeof(iface));
#ifdef CONFIG_AP
        if (0 == i)
#endif
    	    iface.ifname = CONFIG_IFACE_NAME_STA;
#ifdef CONFIG_AP
        else
            iface.ifname = CONFIG_IFACE_NAME_AP;
#endif
        iface.driver = CONFIG_DRIVER_WM_WIFI_NAME;
    	/* TODO: set interface parameters */

    	if (wpa_supplicant_add_iface(global, &iface, NULL) == NULL)
    	{
    		exitcode = -1;
#ifdef CONFIG_AP
            break;
#endif
        }
#ifdef CONFIG_AP
    }
#endif

    task_arg = wm_os_internal_malloc(sizeof(*task_arg));
    if (task_arg)
    {
        task_arg->sync_sem = sync_sem;
        task_arg->global = global;
    }
    else
    {
        exitcode = -1;
    }

	if (exitcode == 0)
		exitcode = wm_os_internal_task_create(NULL, "wpa_supplicant", 
		                    wpa_supplicant_task, task_arg, 
		                    WM_TASK_WPA_SUPPLICANT_STACK/*CONFIG_WPA_SUPPLICANT_TASK_STACK_SIZE*/, 
		                    WM_TASK_WPA_SUPPLICANT_PRIO/*CONFIG_WPA_SUPPLICANT_TASK_PRIO*/, 0);
    else
	    wpa_supplicant_deinit(global);

	if (exitcode)
	{
        wpa_printf(MSG_ERROR, "Failed to start wpa_supplicant");

        if (task_arg)
        {
            wm_os_internal_free(task_arg);
        }
	}
}
