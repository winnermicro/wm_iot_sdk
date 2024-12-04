/**
 * @file wm_atcmd_mqtt.h
 *
 * @brief Default MQTT AT Command
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

#ifndef __WM_ATCMD_MQTT_H__
#define __WM_ATCMD_MQTT_H__

#ifdef __cplusplus
extern "C" {
#endif

#if CONFIG_WM_ATCMD_MQTT_ENABLE

#define WM_ATCMD_MQTTUSERCFG_USAGE                                                                                        \
    "AT+MQTTUSERCFG?\r\n"                                                                                                 \
    "AT+MQTTUSERCFG=<LinkID>,<scheme>,<\"client_id\">,<\"username\">,<\"password\">,<cert_key_ID>,<CA_ID>,<\"path\">\r\n" \
    "example:AT+MQTTUSERCFG=0,1,\"test\",\"hello\",\"123456\",0,0,\"\"\r\n"

#define WM_ATCMD_MQTTLONGCLIENTID_USAGE         \
    "AT+MQTTLONGCLIENTID=<LinkID>,<length>\r\n" \
    "example:AT+MQTTLONGCLIENTID=0,10\r\n"

#define WM_ATCMD_MQTTLONGUSERNAME_USAGE         \
    "AT+MQTTLONGUSERNAME=<LinkID>,<length>\r\n" \
    "example:AT+MQTTLONGUSERNAME=0,10\r\n"

#define WM_ATCMD_MQTTLONGPASSWORD_USAGE         \
    "AT+MQTTLONGPASSWORD=<LinkID>,<length>\r\n" \
    "example:AT+MQTTLONGPASSWORD=0,10\r\n"

#define WM_ATCMD_MQTTCONNCFG_USAGE                                                                                         \
    "AT+MQTTCONNCFG?\r\n"                                                                                                  \
    "AT+MQTTCONNCFG=<LinkID>,<keepalive>,<disable_clean_session>,<\"lwt_topic\">,<\"lwt_msg\">,<lwt_qos>,<lwt_retain>\r\n" \
    "example:AT+MQTTCONNCFG=0,50,0,\"lwtt\",\"lwtm\",0,0\r\n"

#define WM_ATCMD_MQTTALPN_USAGE                                                     \
    "AT+MQTTALPN?\r\n"                                                              \
    "AT+MQTTALPN=<LinkID>,<alpn_counts>[,<\"alpn\">][,<\"alpn\">][,<\"alpn\">]\r\n" \
    "example:AT+MQTTALPN=0,2,\"mqtt-ca.cn\",\"mqtt-ca.us\"\r\n"

#define WM_ATCMD_MQTTCONN_USAGE                              \
    "AT+MQTTCONN?\r\n"                                       \
    "AT+MQTTCONN=<LinkID>,<\"host\">,<port>,<reconnect>\r\n" \
    "example:AT+MQTTCONN=0,\"192.168.200.2\",8883,1\r\n"

#define WM_ATCMD_MQTTPUB_USAGE                                      \
    "AT+MQTTPUB=<LinkID>,<\"topic\">,<\"data\">,<qos>,<retain>\r\n" \
    "example:AT+MQTTPUB=0,\"123\",\"Hello\",0,0\r\n"

#define WM_ATCMD_MQTTPUBRAW_USAGE                                    \
    "AT+MQTTPUBRAW=<LinkID>,<\"topic\">,<length>,<qos>,<retain>\r\n" \
    "example:AT+MQTTPUBRAW=0,\"123\",25,0,0\r\n"

#define WM_ATCMD_MQTTSUB_USAGE                  \
    "AT+MQTTSUB?\r\n"                           \
    "AT+MQTTSUB=<LinkID>,<\"topic\">,<qos>\r\n" \
    "example:AT+MQTTSUB=0,\"123\",0\r\n"

#define WM_ATCMD_MQTTUNSUB_USAGE            \
    "AT+MQTTUNSUB=<LinkID>,<\"topic\">\r\n" \
    "example:AT+MQTTUNSUB=0,\"123\"\r\n"

#define WM_ATCMD_MQTTCLEAN_USAGE \
    "AT+MQTTCLEAN=<LinkID>\r\n"  \
    "example:AT+MQTTCLEAN=0\r\n"

int wm_atcmd_mqttusercfg(wm_at_type_e type, int argc, char **argv);
int wm_atcmd_mqttlongclientid(wm_at_type_e type, int argc, char **argv);
int wm_atcmd_mqttlongnusername(wm_at_type_e type, int argc, char **argv);
int wm_atcmd_mqttlongpassword(wm_at_type_e type, int argc, char **argv);
int wm_atcmd_mqttconncfg(wm_at_type_e type, int argc, char **argv);
int wm_atcmd_mqttalpn(wm_at_type_e type, int argc, char **argv);
int wm_atcmd_mqttconn(wm_at_type_e type, int argc, char **argv);
int wm_atcmd_mqttpub(wm_at_type_e type, int argc, char **argv);
int wm_atcmd_mqttpubraw(wm_at_type_e type, int argc, char **argv);
int wm_atcmd_mqttsub(wm_at_type_e type, int argc, char **argv);
int wm_atcmd_mqttunsub(wm_at_type_e type, int argc, char **argv);
int wm_atcmd_mqttclean(wm_at_type_e type, int argc, char **argv);

WM_ATCMD_DEFINE("AT+MQTTUSERCFG", wm_atcmd_mqttusercfg, WM_ATCMD_MQTTUSERCFG_USAGE);
WM_ATCMD_DEFINE("AT+MQTTLONGCLIENTID", wm_atcmd_mqttlongclientid, WM_ATCMD_MQTTLONGCLIENTID_USAGE);
WM_ATCMD_DEFINE("AT+MQTTLONGUSERNAME", wm_atcmd_mqttlongnusername, WM_ATCMD_MQTTLONGUSERNAME_USAGE);
WM_ATCMD_DEFINE("AT+MQTTLONGPASSWORD", wm_atcmd_mqttlongpassword, WM_ATCMD_MQTTLONGPASSWORD_USAGE);
WM_ATCMD_DEFINE("AT+MQTTCONNCFG", wm_atcmd_mqttconncfg, WM_ATCMD_MQTTCONNCFG_USAGE);
WM_ATCMD_DEFINE("AT+MQTTALPN", wm_atcmd_mqttalpn, WM_ATCMD_MQTTALPN_USAGE);
WM_ATCMD_DEFINE("AT+MQTTCONN", wm_atcmd_mqttconn, WM_ATCMD_MQTTCONN_USAGE);
WM_ATCMD_DEFINE("AT+MQTTPUB", wm_atcmd_mqttpub, WM_ATCMD_MQTTPUB_USAGE);
WM_ATCMD_DEFINE("AT+MQTTPUBRAW", wm_atcmd_mqttpubraw, WM_ATCMD_MQTTPUBRAW_USAGE);
WM_ATCMD_DEFINE("AT+MQTTSUB", wm_atcmd_mqttsub, WM_ATCMD_MQTTSUB_USAGE);
WM_ATCMD_DEFINE("AT+MQTTUNSUB", wm_atcmd_mqttunsub, WM_ATCMD_MQTTUNSUB_USAGE);
WM_ATCMD_DEFINE("AT+MQTTCLEAN", wm_atcmd_mqttclean, WM_ATCMD_MQTTCLEAN_USAGE);

#endif //CONFIG_WM_ATCMD_MQTT_ENABLE

#ifdef __cplusplus
}
#endif

#endif /* __WM_ATCMD_MQTT_H__ */
