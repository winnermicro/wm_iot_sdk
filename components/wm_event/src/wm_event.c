#include <string.h>
#include "wmsdk_config.h"
#include "wm_error.h"
#include "wm_task_config.h"
#include "wm_component.h"
#include "wm_types.h"
#include "wm_event.h"
#include "wm_list.h"
#include "wm_osal.h"

#define LOG_TAG "event"
#include "wm_log.h"

#define WM_EVENT_DEBUG 0

#if WM_EVENT_DEBUG
#define WM_EV_LOG_I wm_log_info
#define WM_EV_LOG_E wm_log_error
#else
#define WM_EV_LOG_I(...) (void)0
#define WM_EV_LOG_E(...) (void)0
#endif

#define WM_EVENT_TASK_NAME "event"

/*status*/
enum wm_ev_status_e {
    WM_EV_ST_NOT_INIT = 0, /* Not init             */
    WM_EV_ST_INITED   = 1, /* inited               */
};

/*operation type*/
enum wm_ev_op_type_e {
    MSG_ADD = 1,     /* add callback         */
    MSG_REMOVE,      /* remove callback      */
    MSG_EXIT,        /* exit task            */
    MSG_EVENT = 0x10 /* normal event service */
};

/*callback information*/
struct wm_cb_item_t {
    struct dl_list list;        /* double link list     */
    wm_event_callback callback; /* callback             */
    wm_event_group_t group;     /* event group          */
    int ev;                     /* event type           */
    void *priv;                 /* user praivate data   */
};

/*event header*/
struct wm_ev_header_t {
    enum wm_ev_op_type_e op; /* operation type       */
    wm_event_group_t group;  /* event group          */
    int ev;                  /* event type           */
    union {
        struct /* for external event   */
        {
            void *data;  /* event data           */
            size_t size; /* event data size      */
        };

        struct /* for internal use     */
        {
            wm_event_callback cb; /* callback             */
            void *priv;           /* user private data    */
        };
    };
};

static volatile enum wm_ev_status_e g_ev_status = WM_EV_ST_NOT_INIT; /* ev status        */
static wm_os_queue_t *g_ev_queue                = NULL;              /* ev queue         */
static struct dl_list g_ev_list;                                     /* callback list    */

static void wm_event_proc(struct wm_ev_header_t *ev)
{
    struct wm_cb_item_t *entry;
    struct wm_cb_item_t *entry_next;

    switch (ev->op) {
        case MSG_EVENT:
        {
            /*prcess module's event*/
            dl_list_for_each_safe(entry, entry_next, &(g_ev_list), struct wm_cb_item_t, list)
            {
                WM_EV_LOG_I("loop ev=%s,%d cb=%s,%d\n", (ev->group ? ev->group : "NULL"), ev->ev,
                            (entry->group ? entry->group : "NULL"), entry->ev);

                if ((entry->group == WM_EVENT_ANY_GROUP || entry->group == ev->group) &&
                    (entry->ev == WM_EVENT_ANY_TYPE || entry->ev == ev->ev)) {
                    WM_EV_LOG_I("call back %s %d\n", (entry->group ? entry->group : "NULL"), ev->size);
                    entry->callback(ev->group, ev->ev, ev->data, entry->priv);
                }
            }
            break;
        }
        case MSG_ADD:
        {
            /*check group and callback exist or not*/
            dl_list_for_each_safe(entry, entry_next, &(g_ev_list), struct wm_cb_item_t, list)
            {
                WM_EV_LOG_E("entry: g=%p,%s,ev=%d,cb=%p,priv=%p", entry->group, (entry->group ? entry->group : "NULL"),
                            entry->ev, entry->callback, entry->priv);
                WM_EV_LOG_E("add: g=%p,%s,ev=%d,cb=%p,priv=%p", ev->group, (ev->group ? ev->group : "NULL"), ev->ev, ev->cb,
                            ev->priv);

                if (entry->group == ev->group && entry->callback == ev->cb && entry->ev == ev->ev && entry->priv == ev->priv) {
                    WM_EV_LOG_E("callback is exist,not add.");
                    return;
                }
            }

            /*add callback to list*/
            entry = wm_os_internal_malloc(sizeof(*entry));
            if (!entry) {
                WM_EV_LOG_E("add event %s failed", (entry->group ? entry->group : "NULL"));
                break;
            }
            dl_list_init(&entry->list);

            entry->group    = (wm_event_group_t)ev->group;
            entry->ev       = ev->ev;
            entry->callback = (wm_event_callback)(ev->cb);
            entry->priv     = (void *)(ev->priv);

            dl_list_add_tail(&(g_ev_list), &entry->list);

            WM_EV_LOG_I("add cb %s ok", (entry->group ? entry->group : "NULL"));
            break;
        }

        case MSG_REMOVE:
        {
            /*remove callback from list*/
            dl_list_for_each_safe(entry, entry_next, &(g_ev_list), struct wm_cb_item_t, list)
            {
                if (ev->cb == entry->callback && ev->group == entry->group && ev->ev == entry->ev && ev->priv == entry->priv) {
                    dl_list_del(&entry->list);
                    wm_os_internal_free(entry);
                    WM_EV_LOG_I("remove cb %s ok", (entry->group ? entry->group : "NULL"));
                    return;
                }
            }
            break;
        }

        case MSG_EXIT:
        {
            WM_EV_LOG_I("do exit");
            /*exit, delete all callback from list*/
            dl_list_for_each_safe(entry, entry_next, &(g_ev_list), struct wm_cb_item_t, list)
            {
                dl_list_del(&entry->list);
                wm_os_internal_free(entry);
            }
            break;
        }

        default:
            break;
    }
}

static void wm_event_task(void *data)
{
    wm_os_queue_t *tmp_queue;
    wm_os_sem_t *sync_sem_new = NULL;
    struct wm_ev_header_t *ev;

    while (g_ev_status == WM_EV_ST_INITED) {
        if (wm_os_internal_queue_receive(g_ev_queue, (void **)&ev, WM_OS_WAIT_TIME_MAX) == WM_OS_STATUS_SUCCESS) {
            /*process event*/
            WM_EV_LOG_I("proc event");
            wm_event_proc(ev);
            if (ev->op == MSG_EXIT) {
                /*EXIT*/
                sync_sem_new = (wm_os_sem_t *)ev->priv;
                wm_os_internal_free(ev);
                break;
            } else {
                wm_os_internal_free(ev);
            }
        }
    }

    WM_EV_LOG_I("clear queue");

    tmp_queue  = g_ev_queue;
    g_ev_queue = NULL; /*do not send any ev to queue any more*/

    /*deinit, remove all ev and delete queue*/
    while (wm_os_internal_queue_receive(tmp_queue, (void **)&ev, 0) == WM_OS_STATUS_SUCCESS) {
        wm_os_internal_free(ev);
    }
    wm_os_internal_queue_delete(tmp_queue);

    if (sync_sem_new) {
        wm_os_internal_sem_release(sync_sem_new);
    }

    WM_EV_LOG_I("exit ev task ok!");

    wm_os_internal_task_del(NULL);
}

static int wm_event_send_internal_ev(enum wm_ev_op_type_e op, wm_event_group_t group, int event, wm_event_callback cb,
                                     void *priv)
{
    struct wm_ev_header_t *ev = NULL;

    if ((op == MSG_ADD || op == MSG_REMOVE) && !cb) {
        /*callback must valid*/
        return WM_ERR_INVALID_PARAM;
    }

    ev = wm_os_internal_malloc(sizeof(*ev));
    if (!ev) {
        return WM_ERR_NO_MEM;
    }

    /*pack items to ev*/
    ev->op    = op;
    ev->group = group;
    ev->ev    = event;
    ev->cb    = cb;
    ev->priv  = priv;

    WM_EV_LOG_I("send %d", op);

    if (g_ev_queue && wm_os_internal_queue_send(g_ev_queue, ev) == WM_OS_STATUS_SUCCESS) {
        /*send OK*/
        return WM_ERR_SUCCESS;
    } else {
        /*send fail*/
        wm_os_internal_free(ev);
        return WM_ERR_FAILED;
    }
}

int wm_event_add_callback(wm_event_group_t group, int event, wm_event_callback callback, void *priv)
{
    return wm_event_send_internal_ev(MSG_ADD, group, event, callback, priv);
}

int wm_event_remove_callback(wm_event_group_t group, int event, wm_event_callback callback, void *priv)
{
    return wm_event_send_internal_ev(MSG_REMOVE, group, event, callback, priv);
}

int wm_event_send(wm_event_group_t group, int event, void *data, size_t size)
{
    struct wm_ev_header_t *ev = NULL;

    if (!group || (!data && size > 0) || (data && size == 0)) {
        return WM_ERR_INVALID_PARAM;
    }

    ev = wm_os_internal_malloc(sizeof(*ev) + (data ? size : 0));
    if (!ev) {
        return WM_ERR_NO_MEM;
    }

    /*pack ev header*/
    ev->op    = MSG_EVENT;
    ev->group = group;
    ev->ev    = event;
    ev->size  = size;

    if (data) {
        ev->data = ((char *)ev) + sizeof(*ev);
        /*copy data to the end of header*/
        memcpy(ev->data, data, size);
    } else {
        ev->data = NULL;
        ev->size = 0;
    }

    WM_EV_LOG_I("send ev %s %d,size=%d", group, event, (int)size);

    if (g_ev_queue && wm_os_internal_queue_send(g_ev_queue, ev) == WM_OS_STATUS_SUCCESS) {
        /*send OK*/
        return WM_ERR_SUCCESS;
    } else {
        /*send fail*/
        wm_os_internal_free(ev);
        WM_EV_LOG_E("send fail");
        return WM_ERR_FAILED;
    }
}

int wm_event_init(void)
{
    WM_EV_LOG_I("wm event start init.\n");

    if (g_ev_status) {
        WM_EV_LOG_I("init before.\n");
        return WM_ERR_ALREADY_INITED; /*init before*/
    }

    g_ev_status = WM_EV_ST_INITED;

    if (wm_os_internal_queue_create(&g_ev_queue, CONFIG_WM_EVENT_QUEUE_SIZE) != WM_OS_STATUS_SUCCESS) {
        WM_EV_LOG_E("create queue before.\n");
        return WM_ERR_FAILED;
    }

    /*
        TASK stack and priority configed by the following two configurations
            CONFIG_WM_EVENT_TASK_STACK_SIZE
            CONFIG_WM_EVENT_TASK_PRIO
    */

    dl_list_init(&g_ev_list);

    if (wm_os_internal_task_create(NULL, WM_EVENT_TASK_NAME, wm_event_task, NULL, WM_TASK_EVENT_STACK, WM_TASK_EVENT_PRIO, 0) !=
        WM_OS_STATUS_SUCCESS) {
        /*create task fail*/
        wm_os_internal_queue_delete(g_ev_queue);
        g_ev_status = WM_EV_ST_NOT_INIT;
        WM_EV_LOG_E("create task fail.\n");
        return WM_ERR_FAILED;

    } else {
        /*init OK!*/
        WM_EV_LOG_I("wm event init ok.\n");
        return WM_ERR_SUCCESS;
    }
}

int wm_event_deinit(void)
{
    int ret                   = WM_ERR_FAILED;
    wm_os_sem_t *sync_sem_new = NULL;

    WM_EV_LOG_I("start deinit.\n");

    if (g_ev_status == WM_EV_ST_INITED) {
        wm_os_internal_sem_create(&sync_sem_new, 0);

        if (wm_event_send_internal_ev(MSG_EXIT, WM_EVENT_ANY_GROUP, -1, NULL, (void *)sync_sem_new) == WM_ERR_SUCCESS) {
            /*wait task end.*/
            wm_os_internal_sem_acquire(sync_sem_new, WM_OS_WAIT_TIME_MAX);
            ret = WM_ERR_SUCCESS;
        }
        wm_os_internal_sem_delete(sync_sem_new);
        g_ev_status = WM_EV_ST_NOT_INIT;
    }

    WM_EV_LOG_I("end deinit.\n");

    return ret;
}

WM_COMPONEN_INIT_2(wm_event_init);
