#include <stdio.h>
#include "wm_types.h"
#include "wm_atcmd.h"

#define LOG_TAG "atcmd"
#include "wm_log.h"

int main(void)
{
    int err;

    err = wm_atcmd_init();
    if (!err) {
        wm_log_info("atcmd init finished.");
    }

    return 0;
}
