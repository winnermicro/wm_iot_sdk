/**
 * @file wm_atcmd_wifi.h
 *
 * @brief Default WiFi AT Command
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

#ifndef __WM_ATCMD_WIFI_H__
#define __WM_ATCMD_WIFI_H__

#ifdef __cplusplus
extern "C" {
#endif

#if CONFIG_WM_ATCMD_WIFI_ENABLE

#define WM_ATCMD_CWINIT_USAGE \
    "AT+CWINIT?\r\n"          \
    "AT+CWINIT=<init>\r\n"    \
    "example:AT+CWINIT=0\r\n"

#define WM_ATCMD_CWMODE_USAGE               \
    "AT+CWMODE?\r\n"                        \
    "AT+CWMODE=<mode>[,<auto_connect>]\r\n" \
    "example:AT+CWMODE=2,0\r\n"

#define WM_ATCMD_CWSTATE_USAGE "AT+CWSTATE?\r\n"

#define WM_ATCMD_CWJAP_USAGE                                                                                     \
    "AT+CWJAP?\r\n"                                                                                              \
    "AT+CWJAP=[<ssid>],[<pwd>][,<bssid>][,<pci_en>][,<reconn_interval>][,<listen_interval>][,<jap_timeout>]\r\n" \
    "AT+CWJAP\r\n"                                                                                               \
    "example:AT+CWJAP=\"SSID\",\"password\"\r\n"

#define WM_ATCMD_CWRECONNCFG_USAGE                        \
    "AT+CWRECONNCFG?\r\n"                                 \
    "AT+CWRECONNCFG=<interval_second>,<repeat_count>\r\n" \
    "example1:AT+CWRECONNCFG=1,100\r\n"                   \
    "example2:AT+CWRECONNCFG=0,0\r\n"

#define WM_ATCMD_CWLAPOPT_USAGE                                      \
    "AT+CWLAPOPT=<print mask>[,<rssi filter>][,<authmode mask>]\r\n" \
    "example:AT+CWLAPOPT=31,-100,1\r\n"

#define WM_ATCMD_CWLAP_USAGE                            \
    "AT+CWLAP=[<ssid>,<mac>,<channel>,<scan_type>]\r\n" \
    "AT+CWLAP\r\n"                                      \
    "example:AT+CWLAP=\"SSID\",\"42:c1:82:80:2a:15\",6,0\r\n"

#define WM_ATCMD_CWQAP_USAGE "AT+CWQAP\r\n"

#define WM_ATCMD_CWSAP_USAGE                                             \
    "AT+CWSAP?\r\n"                                                      \
    "AT+CWSAP=<ssid>,<pwd>,<chl>,<ecn>[,<max conn>][,<ssid hidden>]\r\n" \
    "example:AT+CWSAP=\"SSID\",\"password\",5,3,2,0\r\n"

#define WM_ATCMD_CWLIF_USAGE "AT+CWLIF\r\n"

#define WM_ATCMD_CWQIF_USAGE \
    "AT+CWQIF\r\n"           \
    "AT+CWQIF=<mac>\r\n"     \
    "example:AT+CWQIF=\"1a:25:08:69:01:0F\"\r\n"

#define WM_ATCMD_CWDHCP_USAGE        \
    "AT+CWDHCP?\r\n"                 \
    "AT+CWDHCP=<operate>,<mode>\r\n" \
    "example:AT+CWDHCP=1,1\r\n"

#define WM_ATCMD_CWDHCPS_USAGE "AT+CWDHCPS?\r\n"

#define WM_ATCMD_CWAUTOCONN_USAGE \
    "AT+CWAUTOCONN=<enable>\r\n"  \
    "example:AT+CWAUTOCONN=1\r\n"

#define WM_ATCMD_CWAPPROTO_USAGE  \
    "AT+CWAPPROTO?\r\n"           \
    "AT+CWAPPROTO=<protocol>\r\n" \
    "example:AT+CWAPPROTO=7\r\n"

#define WM_ATCMD_CWSTAPROTO_USAGE  \
    "AT+CWSTAPROTO?\r\n"           \
    "AT+CWSTAPROTO=<protocol>\r\n" \
    "example:AT+CWSTAPROTO=7\r\n"

#define WM_ATCMD_CIPSTAMAC_USAGE \
    "AT+CIPSTAMAC?\r\n"          \
    "AT+CIPSTAMAC=<mac>\r\n"     \
    "example:AT+CIPSTAMAC=\"18:6d:cd:55:e5:7b\"\r\n"

#define WM_ATCMD_CIPAPMAC_USAGE \
    "AT+CIPAPMAC?\r\n"          \
    "AT+CIPAPMAC=<mac>\r\n"     \
    "example:AT+CIPAPMAC=\"18:fe:35:98:d3:7b\"\r\n"

#define WM_ATCMD_CIPSTA_USAGE                              \
    "AT+CIPSTA?\r\n"                                       \
    "AT+CIPSTA=<\"ip\">[,<\"gateway\">,<\"netmask\">]\r\n" \
    "example:AT+CIPSTA=\"192.168.6.100\",\"192.168.6.1\",\"255.255.255.0\"\r\n"

#define WM_ATCMD_CIPAP_USAGE                              \
    "AT+CIPAP?\r\n"                                       \
    "AT+CIPAP=<\"ip\">[,<\"gateway\">,<\"netmask\">]\r\n" \
    "example:AT+CIPAP=\"192.168.5.1\",\"192.168.5.1\",\"255.255.255.0\"\r\n"

#define WM_ATCMD_CWHOSTNAME_USAGE  \
    "AT+CWHOSTNAME?\r\n"           \
    "AT+CWHOSTNAME=<hostname>\r\n" \
    "example:AT+CWHOSTNAME=\"hostname\"\r\n"

#define WM_ATCMD_CWCOUNTRY_USAGE                                            \
    "AT+CWCOUNTRY?\r\n"                                                     \
    "AT+CWCOUNTRY=<country_code>,<start_channel>,<total_channel_count>\r\n" \
    "example:AT+CWCOUNTRY=\"CN\",1,13\r\n"

#define WM_ATCMD_WEBSERVER_USAGE                                   \
    "AT+WEBSERVER=<enable>,<server_port>,<connection_timeout>\r\n" \
    "example1:AT+WEBSERVER=1,80,50\r\n"                            \
    "example2:AT+WEBSERVER=0\r\n"

int wm_atcmd_cwinit(wm_at_type_e type, int argc, char **argv);
int wm_atcmd_cwmode(wm_at_type_e type, int argc, char **argv);
int wm_atcmd_cwstate(wm_at_type_e type, int argc, char **argv);
int wm_atcmd_cwjap(wm_at_type_e type, int argc, char **argv);
int wm_atcmd_cwreconncfg(wm_at_type_e type, int argc, char **argv);
int wm_atcmd_cwlapopt(wm_at_type_e type, int argc, char **argv);
int wm_atcmd_cwlap(wm_at_type_e type, int argc, char **argv);
int wm_atcmd_cwqap(wm_at_type_e type, int argc, char **argv);
int wm_atcmd_cwsap(wm_at_type_e type, int argc, char **argv);
int wm_atcmd_cwlif(wm_at_type_e type, int argc, char **argv);
int wm_atcmd_cwqif(wm_at_type_e type, int argc, char **argv);
int wm_atcmd_cwdhcp(wm_at_type_e type, int argc, char **argv);
int wm_atcmd_cwdhcps(wm_at_type_e type, int argc, char **argv);
int wm_atcmd_cwautoconn(wm_at_type_e type, int argc, char **argv);
int wm_atcmd_cwapproto(wm_at_type_e type, int argc, char **argv);
int wm_atcmd_cwstaproto(wm_at_type_e type, int argc, char **argv);
int wm_atcmd_cipstamac(wm_at_type_e type, int argc, char **argv);
int wm_atcmd_cipapmac(wm_at_type_e type, int argc, char **argv);
int wm_atcmd_cipsta(wm_at_type_e type, int argc, char **argv);
int wm_atcmd_cipap(wm_at_type_e type, int argc, char **argv);
int wm_atcmd_cwhostname(wm_at_type_e type, int argc, char **argv);
int wm_atcmd_cwcountry(wm_at_type_e type, int argc, char **argv);

#ifdef CONFIG_WM_ATCMD_WEBNET_ENABLE
int wm_atcmd_webnet(wm_at_type_e type, int argc, char **argv);
#endif

WM_ATCMD_DEFINE("AT+CWINIT", wm_atcmd_cwinit, WM_ATCMD_CWINIT_USAGE);
WM_ATCMD_DEFINE("AT+CWMODE", wm_atcmd_cwmode, WM_ATCMD_CWMODE_USAGE);
WM_ATCMD_DEFINE("AT+CWSTATE", wm_atcmd_cwstate, WM_ATCMD_CWSTATE_USAGE);
WM_ATCMD_DEFINE("AT+CWJAP", wm_atcmd_cwjap, WM_ATCMD_CWJAP_USAGE);
WM_ATCMD_DEFINE("AT+CWRECONNCFG", wm_atcmd_cwreconncfg, WM_ATCMD_CWRECONNCFG_USAGE);
WM_ATCMD_DEFINE("AT+CWLAPOPT", wm_atcmd_cwlapopt, WM_ATCMD_CWLAPOPT_USAGE);
WM_ATCMD_DEFINE("AT+CWLAP", wm_atcmd_cwlap, WM_ATCMD_CWLAP_USAGE);
WM_ATCMD_DEFINE("AT+CWQAP", wm_atcmd_cwqap, WM_ATCMD_CWQAP_USAGE);
WM_ATCMD_DEFINE("AT+CWSAP", wm_atcmd_cwsap, WM_ATCMD_CWSAP_USAGE);
WM_ATCMD_DEFINE("AT+CWLIF", wm_atcmd_cwlif, WM_ATCMD_CWLIF_USAGE);
WM_ATCMD_DEFINE("AT+CWQIF", wm_atcmd_cwqif, WM_ATCMD_CWQIF_USAGE);
WM_ATCMD_DEFINE("AT+CWDHCP", wm_atcmd_cwdhcp, WM_ATCMD_CWDHCP_USAGE);
WM_ATCMD_DEFINE("AT+CWDHCPS", wm_atcmd_cwdhcps, WM_ATCMD_CWDHCPS_USAGE);
WM_ATCMD_DEFINE("AT+CWAUTOCONN", wm_atcmd_cwautoconn, WM_ATCMD_CWAUTOCONN_USAGE);
WM_ATCMD_DEFINE("AT+CWAPPROTO", wm_atcmd_cwapproto, WM_ATCMD_CWAPPROTO_USAGE);
WM_ATCMD_DEFINE("AT+CWSTAPROTO", wm_atcmd_cwstaproto, WM_ATCMD_CWSTAPROTO_USAGE);
WM_ATCMD_DEFINE("AT+CIPSTAMAC", wm_atcmd_cipstamac, WM_ATCMD_CIPSTAMAC_USAGE);
WM_ATCMD_DEFINE("AT+CIPAPMAC", wm_atcmd_cipapmac, WM_ATCMD_CIPAPMAC_USAGE);
WM_ATCMD_DEFINE("AT+CIPSTA", wm_atcmd_cipsta, WM_ATCMD_CIPSTA_USAGE);
WM_ATCMD_DEFINE("AT+CIPAP", wm_atcmd_cipap, WM_ATCMD_CIPAP_USAGE);
WM_ATCMD_DEFINE("AT+CWHOSTNAME", wm_atcmd_cwhostname, WM_ATCMD_CWHOSTNAME_USAGE);
WM_ATCMD_DEFINE("AT+CWCOUNTRY", wm_atcmd_cwcountry, WM_ATCMD_CWCOUNTRY_USAGE);

#ifdef CONFIG_WM_ATCMD_WEBNET_ENABLE
WM_ATCMD_DEFINE("AT+WEBSERVER", wm_atcmd_webnet, WM_ATCMD_WEBSERVER_USAGE);
#endif

#endif //CONFIG_WM_ATCMD_WIFI_ENABLE

#ifdef __cplusplus
}
#endif

#endif /* __WM_ATCMD_WIFI_H__ */
