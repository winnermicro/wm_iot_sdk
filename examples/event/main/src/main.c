#include <stdio.h>
#include <stdint.h>

#include "wm_types.h"
#include "wm_event.h"

#include "example_event.h"

#define LOG_TAG "exam_event"
#include "wm_log.h"

static void example_event_callback_all(wm_event_group_t group, int event, void *data, void *priv)
{
    if (group == EXAMPLE_USER_EV_GROUP && event == EXAMPLE_EVENT_TYPE1) {
        struct exampe_user_event_t *p = (struct exampe_user_event_t *)(data);
        wm_log_info("user event in all: %d %d\n", p->param1, p->param2);
    }
}

static void example_event_callback_group(wm_event_group_t group, int event, void *data, void *priv)
{
    if (event == EXAMPLE_EVENT_TYPE1) {
        struct exampe_user_event_t *p = (struct exampe_user_event_t *)(data);
        wm_log_info("user event in group: %d %d\n", p->param1, p->param2);
    }
}

static void example_event_callback_event(wm_event_group_t group, int event, void *data, void *priv)
{
    struct exampe_user_event_t *p = (struct exampe_user_event_t *)(data);
    wm_log_info("user event %d %d\n", p->param1, p->param2);
}

int main(void)
{
    /* filter all group, all event */
    wm_event_add_callback(WM_EVENT_ANY_GROUP, WM_EVENT_ANY_TYPE, example_event_callback_all, NULL);

    /* filter all event in EXAMPLE_USER_EV_GROUP  */
    wm_event_add_callback(EXAMPLE_USER_EV_GROUP, WM_EVENT_ANY_TYPE, example_event_callback_group, NULL);

    /* filter EXAMPLE_EVENT_TYPE1 in EXAMPLE_USER_EV_GROUP  */
    wm_event_add_callback(EXAMPLE_USER_EV_GROUP, EXAMPLE_EVENT_TYPE1, example_event_callback_event, NULL);

    /* start user event module, it will trigger a user event */
    start_example_event();

    return 0;
}
