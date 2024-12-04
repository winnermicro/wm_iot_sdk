#include "wmsdk_config.h"
#include <stdio.h>

#define LOG_TAG "test"
#include "wm_log.h"

void test(void)
{
    wm_log_info("project name: %s", CONFIG_PROJECT_NAME);
}
