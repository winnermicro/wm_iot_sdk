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
#include "wm_types.h"

#include <string.h>
#include "wm_drv_gpio.h"
#include "cJSON/cJSON.h"

#include "clock.h"
#include "shadow_demo_main.h"
#include "pwm_led.h"
#include "temperature_sensor.h"
#include "aws_device_attr.h"

#define LOG_TAG "aws_device"
#include "wm_log.h"

typedef const char *(*get_attch_name)(void);
typedef void (*init_attch)(void **ptr);
typedef int (*set_attch)(void *ptr, const char *data, size_t len);
typedef int (*get_attch)(void *ptr, char *data, size_t max_len);

typedef struct {
    void *user_ptr;
    get_attch_name get_name_fun;
    init_attch init_fun;
    set_attch set_fun;
    get_attch get_fun;
} aws_dev_attr_t;

static aws_dev_attr_t s_dev_attr[] = {
    { NULL, get_pwm_led_name,            init_pwm_led,            set_pwm_led, get_pwm_led            },
    { NULL, get_temperature_sensor_name, init_temperature_sensor, NULL,        get_temperature_sensor },
};

static const size_t s_dev_attr_size = sizeof(s_dev_attr) / sizeof(s_dev_attr[0]);

void aws_device_init(void)
{
    for (int i = 0; i < s_dev_attr_size; i++) {
        if (s_dev_attr[i].init_fun != NULL) {
            s_dev_attr[i].init_fun(&(s_dev_attr[i].user_ptr));
        }
    }
}

void aws_device_update(void)
{
    char tmp_buf[128];
    int ret;

    // creat root JSON object
    cJSON *root = cJSON_CreateObject();

    // creat state JSON object
    cJSON *state = cJSON_CreateObject();
    cJSON_AddItemToObject(root, "state", state);

    cJSON *reported = cJSON_CreateObject();
    cJSON_AddItemToObject(state, "reported", reported);

    for (int i = 0; i < s_dev_attr_size; i++) {
        if (s_dev_attr[i].get_name_fun && s_dev_attr[i].get_fun) {
            ret = s_dev_attr[i].get_fun(s_dev_attr[i].user_ptr, tmp_buf, sizeof(tmp_buf));
            if (ret == WM_ERR_SUCCESS) {
                cJSON_AddStringToObject(reported, s_dev_attr[i].get_name_fun(), tmp_buf);
            } else {
                wm_log_error("call %s get_fun fail", s_dev_attr[i].get_name_fun());
            }
        }
    }

    snprintf(tmp_buf, sizeof(tmp_buf), "%06u", Clock_GetTimeMs() % 1000000);
    cJSON_AddStringToObject(root, "clientToken", tmp_buf);

    // JSON object to string
    char *json_string = cJSON_Print(root);

    if (json_string && WM_ERR_SUCCESS == aws_publish_default_shadow(json_string, strlen(json_string))) {
        wm_log_debug("aws update msg success:%s", json_string);
    } else {
        wm_log_error("aws update msg fail:%s", json_string);
    }

    // Release resources
    cJSON_Delete(root);
    free(json_string);
}

void aws_device_attr_set(const char *data, size_t len)
{
    int ret;

    cJSON *root = cJSON_ParseWithLength(data, len);
    if (root == NULL) {
        const char *error_ptr = cJSON_GetErrorPtr();
        if (error_ptr != NULL) {
            wm_log_error("Error before: %s", error_ptr);
        }
        return;
    }

    const cJSON *state = cJSON_GetObjectItemCaseSensitive(root, "state");
    if (state == NULL) {
        cJSON_Delete(root);
        return;
    }

    const cJSON *reported = cJSON_GetObjectItemCaseSensitive(state, "reported");
    if (reported == NULL) {
        cJSON_Delete(root);
        return;
    }

    cJSON *reported_child = reported->child;

    while (reported_child != NULL) {
        if (cJSON_IsString(reported_child) && reported_child->string != NULL && reported_child->valuestring != NULL) {
            int i = 0;

            for (; i < s_dev_attr_size; i++) {
                if (s_dev_attr[i].get_name_fun && !strcmp(s_dev_attr[i].get_name_fun(), reported_child->string)) {
                    if (s_dev_attr[i].set_fun) {
                        ret = s_dev_attr[i].set_fun(s_dev_attr[i].user_ptr, reported_child->valuestring,
                                                    strlen(reported_child->valuestring));
                        if (ret != WM_ERR_SUCCESS) {
                            wm_log_error("call %s set_fun fail", s_dev_attr[i].get_name_fun());
                        }
                    } else {
                        wm_log_warn("not found %s set_fun", s_dev_attr[i].get_name_fun());
                    }
                    break;
                }
            }

            if (i == s_dev_attr_size) {
                wm_log_warn("not found %s", reported_child->string);
            }
        }

        reported_child = reported_child->next;
    }

    cJSON_Delete(root);
}

void aws_device_attr_change(const char *item, const char *value)
{
    char tmp_buf[128];

    assert(item && value);

    // creat root JSON object
    cJSON *root = cJSON_CreateObject();

    // creat state JSON object
    cJSON *state = cJSON_CreateObject();
    cJSON_AddItemToObject(root, "state", state);

    cJSON *reported = cJSON_CreateObject();
    cJSON_AddItemToObject(state, "reported", reported);
    cJSON_AddStringToObject(reported, item, value);

    snprintf(tmp_buf, sizeof(tmp_buf), "%06u", Clock_GetTimeMs() % 1000000);
    cJSON_AddStringToObject(root, "clientToken", tmp_buf);

    // JSON object to string
    char *json_string = cJSON_Print(root);

    if (json_string && WM_ERR_SUCCESS == aws_publish_default_shadow(json_string, strlen(json_string))) {
        wm_log_debug("aws update msg success:%s", json_string);
    } else {
        wm_log_error("aws update msg fail:%s", json_string);
    }

    // Release resources
    cJSON_Delete(root);
    free(json_string);
}
