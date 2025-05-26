/**
 * @file wm_atcmd_default.c
 *
 * @brief Default AT Command
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
#include "wm_osal.h"
#include "wm_atcmd.h"
#include "wm_atcmd_bt.h"
#include "wm_atcmd_http.h"
#include "wm_atcmd_mqtt.h"
#include "wm_atcmd_ota.h"
#include "wm_atcmd_pwm.h"
#include "wm_atcmd_i2c.h"
#include "wm_atcmd_sys.h"
#include "wm_atcmd_tcp.h"
#include "wm_atcmd_wifi.h"
#include "wm_atcmd_spi_master.h"
#include "wm_atcmd_fs.h"
#include "wm_atcmd_ft.h"

#if CONFIG_WM_ATCMD_WIFI_ENABLE
extern void *__real_wm_atcmd_wifi_init(void *ctl);
#endif

#if CONFIG_WM_ATCMD_TCP_ENABLE
extern void *__real_wm_atcmd_tcpip_init(void *ctl);
#endif

#if CONFIG_WM_ATCMD_MQTT_ENABLE
extern void *__real_wm_atcmd_mqtt_init(void *ctl);
#endif

#if CONFIG_WM_ATCMD_HTTP_ENABLE
extern void *__real_wm_atcmd_http_init(void *ctl);
#endif

#if CONFIG_WM_ATCMD_BT_ENABLE
extern void *__real_wm_atcmd_bt_init(void *ctl);
#endif

#if !CONFIG_WM_ATCMD_WIFI_ENABLE || !CONFIG_WM_ATCMD_TCP_ENABLE || !CONFIG_WM_ATCMD_MQTT_ENABLE || \
    !CONFIG_WM_ATCMD_HTTP_ENABLE || !CONFIG_WM_ATCMD_BT_ENABLE
const static char *wm_atcmd_null_ctl = "*";
#endif

void *__wrap_wm_atcmd_wifi_init(void *ctl)
{
#if CONFIG_WM_ATCMD_WIFI_ENABLE
    return __real_wm_atcmd_wifi_init(ctl);
#else
    return (void *)wm_atcmd_null_ctl;
#endif
}

void *__wrap_wm_atcmd_tcpip_init(void *ctl)
{
#if CONFIG_WM_ATCMD_TCP_ENABLE
    return __real_wm_atcmd_tcpip_init(ctl);
#else
    return (void *)wm_atcmd_null_ctl;
#endif
}

void *__wrap_wm_atcmd_mqtt_init(void *ctl)
{
#if CONFIG_WM_ATCMD_MQTT_ENABLE
    return __real_wm_atcmd_mqtt_init(ctl);
#else
    return (void *)wm_atcmd_null_ctl;
#endif
}

void *__wrap_wm_atcmd_http_init(void *ctl)
{
#if CONFIG_WM_ATCMD_HTTP_ENABLE
    return __real_wm_atcmd_http_init(ctl);
#else
    return (void *)wm_atcmd_null_ctl;
#endif
}

void *__wrap_wm_atcmd_bt_init(void *ctl)
{
#if CONFIG_WM_ATCMD_BT_ENABLE
    return __real_wm_atcmd_bt_init(ctl);
#else
    return (void *)wm_atcmd_null_ctl;
#endif
}
