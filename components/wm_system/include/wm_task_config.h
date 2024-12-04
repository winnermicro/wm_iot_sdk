#ifndef __WM_TASK_CONFIG_H__
#define __WM_TASK_CONFIG_H__

#include "wmsdk_config.h"
#include "freertos/FreeRTOS.h"

#ifdef __cplusplus
extern "C" {
#endif

#define WM_TASK_PRIO_MAX             (configMAX_PRIORITIES)
#define WM_TASK_PRIO_MIN             (0)

#define WM_TASK_BT_CONTROLLER_PRIO   (WM_TASK_PRIO_MAX - 2)
#define WM_TASK_WIFI_DRV_PRIO        (WM_TASK_PRIO_MAX - 2)
#define WM_TASK_WPA_SUPPLICANT_PRIO  (WM_TASK_PRIO_MAX - 3)
#define WM_TASK_TCPIP_PRIO           (WM_TASK_PRIO_MAX - 5)
#define WM_TASK_EVENT_PRIO           (WM_TASK_PRIO_MAX - 6)
#define WM_TASK_WIFI_ONESHOT_PRIO    (WM_TASK_PRIO_MAX - 7)
#define WM_TASK_NET_MANAGER_PRIO     (WM_TASK_PRIO_MAX - 7)
#define WM_TASK_LOG_PRIO             (WM_TASK_PRIO_MAX - 8)
#define WM_TASK_CLI_PRIO             (WM_TASK_PRIO_MAX - 8)
#define WM_TASK_LOG_PRIO             (WM_TASK_PRIO_MAX - 8)
#define WM_TASK_ATCMD_PRIO           (WM_TASK_PRIO_MAX - 9)
#define WM_TASK_MQTT_CLIENT_PRIO     (WM_TASK_ATCMD_PRIO - 1)
#define WM_TASK_WEBNET_PRIO          (WM_TASK_PRIO_MIN + 1)
#define WM_TASK_POSIX_PRIO           (WM_TASK_PRIO_MIN + 1)
#define WM_TASK_MAIN_PRIO            (WM_TASK_PRIO_MIN + 1)
#define WM_TASK_OTA_HTTP_PRIO        (WM_TASK_ATCMD_PRIO - 1)
#define WM_TASK_EVENT_STACK          (2048)
#define WM_TASK_BT_CONTROLLER_STACK  (512)
#define WM_TASK_WIFI_DRV_STACK       (3072)
#define WM_TASK_WPA_SUPPLICANT_STACK (6144)
#define WM_TASK_TCPIP_STACK          (4096)
#define WM_TASKWEBNET_STACK          (4096)
#define WM_TASK_WIFI_ONESHOT_STACK   (4096)
#define WM_TASK_CLI_STACK            (4096)
#define WM_TASK_LOG_STACK            (1024)
#define WM_TASK_MQTT_CLIENT_STACK    (8192)
#define WM_TASK_POSIX_MIN_STACK      (512)
#define WM_TASK_ATCMD_STACK          (8192)
#define WM_TASK_MAIN_STACK           (4096)
#define WM_TASK_OTA_HTTP_STACK       (4096)
#define WM_TASK_NET_MANAGER_STACK    (2048)

#ifdef __cplusplus
}
#endif

#endif /* __WM_TASK_CONFIG_H__ */
