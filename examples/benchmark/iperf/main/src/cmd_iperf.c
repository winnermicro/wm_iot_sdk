#include "wmsdk_config.h"
#include <stdio.h>
#include "wm_cli.h"
#include "iperf_api.h"
#include "wm_netif.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define LOG_TAG "iperf3"
#include "wm_log.h"

#define IPERF_USE_TASK 1

static volatile bool s_iperf_started = false;

static void cmd_iperf_task(void *parameters)
{
    struct iperf_test *test = parameters;
    s_iperf_started         = true;

    switch (iperf_get_test_role(test)) {
        case 's':
        {
            while (1) {
                int rc;

                rc = iperf_run_server(test);
                if (rc < 0) {
                    wm_log_error("error - %s", iperf_strerror(i_errno));
                    break;
                }

                iperf_free_all_stream(test);
            }

            break;
        }
        case 'c':
        {
            if (iperf_run_client(test) < 0) {
                wm_log_error("error - %s", iperf_strerror(i_errno));
            }

            break;
        }
        default:
        {
            iperf_usage();
            break;
        }
    }

    iperf_free_test(test);

    s_iperf_started = false;
#if IPERF_USE_TASK
    vTaskDelete(NULL);
#endif
}

static void cmd_iperf(int argc, char *argv[])
{
    struct iperf_test *test;

    if (wm_netif_get_netif(WM_NETIF_TYPE_WIFI_STA) == NULL && wm_netif_get_netif(WM_NETIF_TYPE_WIFI_AP) == NULL) {
        wm_log_error("network is not ready, please start network connection");
        return;
    }

    if (s_iperf_started != false) {
        wm_log_error("iperf is running.");
        return;
    }

    test = iperf_new_test();
    if (!test) {
        wm_log_error("create new test error - %s", iperf_strerror(i_errno));
        return;
    }

    iperf_defaults(test); /* sets defaults */

    if (iperf_parse_arguments(test, argc, argv) < 0) {
        wm_log_error("parameter error - %s", iperf_strerror(i_errno));
        iperf_usage();
        return;
    }

#if IPERF_USE_TASK
    xTaskCreate(cmd_iperf_task, "iperf", 512, test, 3, NULL);
#else
    cmd_iperf_task(test);
#endif
}
WM_CLI_CMD_DEFINE(iperf3, cmd_iperf, iperf cmd, iperf3<args...> --iperf cmd);
