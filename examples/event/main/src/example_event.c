
#include "example_event.h"

WM_EVENT_DEFINE_GROUP(EXAMPLE_USER_EV_GROUP);

void start_example_event(void)
{
    struct example_user_event_t user_event = { 1, 2 };

    wm_event_send(EXAMPLE_USER_EV_GROUP, EXAMPLE_EVENT_TYPE1, &user_event, sizeof(user_event));
}
