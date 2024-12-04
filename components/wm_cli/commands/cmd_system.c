#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "wmsdk_config.h"
#include "wm_cli.h"
#include "wm_heap.h"
#include "wm_reg_op.h"
#include "wm_log.h"
#include "wm_event.h"
#include "wm_utils.h"
#include "wm_osal.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static void cmd_version(int argc, char *argv[])
{
    wm_cli_printf("sdk version %s, optimize %s, build at %s %s\r\n", CONFIG_BUILD_VERSION, CONFIG_COMPILER_OPTS, __DATE__,
                  __TIME__);
}
WM_CLI_CMD_DEFINE(version, cmd_version, show sdk version info, version-- sdk version info);

static void cmd_reboot(int argc, char *argv[])
{
    wm_system_reboot();
}
WM_CLI_CMD_DEFINE(reboot, cmd_reboot, reboot system, reboot-- reboot system);

static void cmd_task(int argc, char *argv[])
{
    wm_cli_printf("task\t\tstatus\tprio\tminimum\tid");
    wm_os_internal_disp_task_stat_info();
}
WM_CLI_CMD_DEFINE(task, cmd_task, show task list, task-- show task list);

#ifdef CONFIG_FREERTOS_ENABLE_RUN_TIME_STATS
static void cmd_cpu_callback(wm_event_group_t group, int event, void *data, void *priv)
{
    if (group == WM_UTILS_EV && event == WM_UTILS_EVENT_CPU_USAGE) {
        wm_cli_printf("\ntask\t\tticks\t\tusage");
        wm_os_internal_disp_sys_runtime_stats_info();
    }
}

static void cmd_cpu(int argc, char *argv[])
{
    uint32_t duration_ms = 100;

    if (argc == 2)
        duration_ms = atoi(argv[1]);

    wm_event_remove_callback(WM_UTILS_EV, WM_EVENT_ANY_TYPE, cmd_cpu_callback, NULL);
    wm_event_add_callback(WM_UTILS_EV, WM_EVENT_ANY_TYPE, cmd_cpu_callback, NULL);
    if (wm_set_cpu_usage_duration(duration_ms)) {
        wm_event_remove_callback(WM_UTILS_EV, WM_EVENT_ANY_TYPE, cmd_cpu_callback, NULL);
    }
}
WM_CLI_CMD_DEFINE(cpu, cmd_cpu, show cpu usage, cpu[duration_ms]-- show cpu usage);
#endif

static void cmd_free(int argc, char *argv[])
{
    vTaskDelay(pdMS_TO_TICKS(2));
    wm_heap_print_stats();
}
WM_CLI_CMD_DEFINE(free, cmd_free, show free heap size, free-- show free heap size);

#ifdef CONFIG_HEAP_USE_TRACING
static void cmd_heap(int argc, char *argv[])
{
    vTaskDelay(pdMS_TO_TICKS(2));
    wm_heap_print_tracing(NULL);
}
WM_CLI_CMD_DEFINE(heap, cmd_heap, show heap tracing, heap-- show heap tracing);
#endif

static void cmd_log(int argc, char *argv[])
{
    uint8_t i;
    char *log_level_str[6] = { "none", "error", "warn", "info", "debug", "verbose" };

    if (argc > 2)
        return;

    if (argc == 1) {
        wm_cli_printf("log level: %s\r\n", log_level_str[wm_log_get_level()]);
    } else if (argc == 2) {
        for (i = 0; i < (sizeof(log_level_str) / sizeof(char *)); i++) {
            if (!strncasecmp(log_level_str[i], argv[1], strlen(log_level_str[i])) &&
                (strlen(log_level_str[i]) == strlen(argv[1]))) {
                wm_log_set_level(i);
                break;
            }
        }
    }
}
WM_CLI_CMD_DEFINE(log, cmd_log, log level cmd, log[level]-- get / set log level);
