#ifndef _EXAMPLE_EVENT_H_
#define _EXAMPLE_EVENT_H_

#include "wm_types.h"
#include "wm_event.h"

#ifdef __cplusplus
extern "C" {
#endif

/*declare user event group */
WM_EVENT_DECLARE_GROUP(EXAMPLE_USER_EV_GROUP);

/*user event sub type*/
enum {
    EXAMPLE_EVENT_TYPE1 = 1,
};

/*user event data info*/
struct exampe_user_event_t {
    int param1;
    int param2;
};

void start_example_event(void);

#ifdef __cplusplus
}
#endif

#endif /*_EXAMPLE_EVENT_H_*/