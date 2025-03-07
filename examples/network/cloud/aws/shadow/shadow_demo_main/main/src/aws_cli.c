/**
 * @file aws_cli.c
 *
 * @brief The AWS command line file
 *
 */

/**
 *  Copyright 2022-2024 Beijing WinnerMicroelectronics Co.,Ltd.
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */

#include "wmsdk_config.h"
#if CONFIG_COMPONENT_CLI_ENABLED

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "wm_netif.h"
#include "wm_cli.h"
#include "aws_user_def.h"

#define LOG_TAG "aws_cli"
#include "wm_log.h"

extern int shadow_demo_main(int argc, char **argv);

static void aws_task(void *pvParameters)
{
    if (wm_netif_get_netif(WM_NETIF_TYPE_WIFI_STA) == NULL) {
        wm_log_error("network is not ready, please start wifi connection");
    } else {
        shadow_demo_main(0, NULL);
    }

    vTaskDelete(NULL);
}

static void aws_shadow(int argc, char *argv[])
{
    static TaskHandle_t task_handle = NULL;

    if (task_handle != NULL) {
        if (eTaskGetState(task_handle) != eDeleted) {
            wm_log_error("AWS demo task is running");
            return;
        }

        task_handle = NULL;
    }

    xTaskCreate(aws_task, "aws_demo_task", AWS_MQTT_CLI_TASK_STACK_SIZE, NULL, AWS_MQTT_CLI_TASK_PRIORITY, &task_handle);
}

WM_CLI_CMD_DEFINE(aws_shadow, aws_shadow, aws shadow demo, aws_shadow); //cppcheck # [syntaxError]

#endif
