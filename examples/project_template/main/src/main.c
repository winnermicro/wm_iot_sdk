#include <stdio.h>
#include "wmsdk_config.h"
#include "test.h"

#define LOG_TAG "prj_template"
#include "wm_log.h"

int main(void)
{
#if CONFIG_MENUCONFIG_SUB_EXAMPLE_1
    wm_log_info("use example config 1\r\n");
#endif

#if CONFIG_MENUCONFIG_SUB_EXAMPLE_2
    wm_log_info("use example config 2\r\n");
#endif

    test();

    return 0;
}
