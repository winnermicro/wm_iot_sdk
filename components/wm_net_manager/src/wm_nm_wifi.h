/**
 * @file wm_nm_wifi.h
 *
 * @brief WM net manager Module
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
#ifndef __WM_NM_WIFI_H__
#define __WM_NM_WIFI_H__
#include "wm_nm_def.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    WM_NM_STA_START_CONNECT, /* connect to AP */
    WM_NM_STA_DISCONNECT,    /* disconnect from AP */
    WM_NM_AP_START,          /* start softAP */
    WM_NM_AP_STOP,           /* stop softAP */
    WM_NM_RF_CHANNEL_CHANGE, /* Hardware limitation: Station and softap must be on the same channel */
} wm_nm_msg_type_e;

typedef struct {
    wm_nm_msg_type_e type; /* ref wm_nm_msg_type_e*/
    int channel;           /* RF channel*/
    union {
        struct {
            char ssid[33]; /* station target ssid*/
            char pwd[65];  /* station target password*/
        } sta;
        struct {
            char ssid[33]; /* softAP ssid*/
            char pwd[65];  /* softAP password*/
        } ap;
    };
} wm_nm_msg_t;

/**
  * @brief  Send msg to net manager
  *
  * @param[in] msg: reference wm_nm_msg_t
  *
  * @return
  *    - WM_ERR_SUCCESS: succeed
  *    - WM_ERR_INVALID_PARAM: invalid argument
  *    - others: failed
  */
int wm_nm_core_send_msg(wm_nm_msg_t *msg);

/**
  * @brief  Query wifi station state
  *
  * @return
  *    - reference wm_nm_state_e
  */
wm_nm_state_e wm_nm_get_wifi_station_state_internal(void);

/**
  * @brief  Query wifi softAP state
  *
  * @return
  *    - reference wm_nm_state_e
  */
wm_nm_state_e wm_nm_get_wifi_softap_state_internal(void);

/**
  * @brief  Init NM wifi module
  *
  * @return
  *    - WM_ERR_SUCCESS: succeed
  *    - WM_ERR_INVALID_PARAM: invalid argument
  *    - others: failed
  */
int wm_nm_wifi_init(void);

/**
  * @brief  Wifi station start reconnect
  *
  * @param[in] time_ms: reconnect timer refresh time in milliseconds
  *
  * @return
  *    - WM_ERR_SUCCESS: succeed
  *    - WM_ERR_INVALID_PARAM: invalid argument
  *    - others: failed
  */
int wm_nm_wifi_station_reconnect_start(int time_ms);

/**
  * @brief  Wifi station stop reconnect
  *
  * @return
  *    - WM_ERR_SUCCESS: succeed
  *    - WM_ERR_INVALID_PARAM: invalid argument
  *    - others: failed
  */
int wm_nm_wifi_station_reconnect_stop(void);

/**
  * @brief  Set wifi station state
  *
  * @param[in] state: reference wm_nm_state_e
  *
  */
void wm_nm_set_wifi_station_state_internal(wm_nm_state_e state);

/**
  * @brief  Set wifi softap state
  *
  * @param[in] state: reference wm_nm_state_e
  *
  */
void wm_nm_set_wifi_softap_state_internal(wm_nm_state_e state);

#ifdef __cplusplus
}
#endif

#endif //__WM_NM_WIFI_H__
