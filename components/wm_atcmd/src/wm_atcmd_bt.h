/**
 * @file wm_atcmd_bt.h
 *
 * @brief Default Bluetooth AT Command
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

#ifndef __WM_ATCMD_BLUETOOTH_H__
#define __WM_ATCMD_BLUETOOTH_H__

#ifdef __cplusplus
extern "C" {
#endif

#if CONFIG_WM_ATCMD_BT_ENABLE

#define WM_ATCMD_BLEVERS_USAGE "AT+BLEVERS?"

#define WM_ATCMD_BLEINIT_USAGE \
    "AT+BLEINIT?\r\n"          \
    "AT+BLEINIT=<init>\r\n"    \
    "example:AT+BLEINIT=1\r\n"

#define WM_ATCMD_BLEADDR_USAGE                   \
    "AT+BLEADDR?\r\n"                            \
    "AT+BLEADDR=<addr_type>[,<random_addr>]\r\n" \
    "example:AT+BLEADDR=1,\"f8:72:23:85:18:7b\"\r\n"

#define WM_ATCMD_BLENAME_USAGE     \
    "AT+BLEBLENAME?\r\n"           \
    "AT+BLENAME=<device_name>\r\n" \
    "example:AT+BLENAME=\"wm_ble\"\r\n"

#define WM_ATCMD_BLESCANPARAM_USAGE                                                                 \
    "AT+BLESCANPARAM?\r\n"                                                                          \
    "AT+BLESCANPARAM=<scan_type>,<own_addr_type>,<filter_policy>,<scan_interval>,<scan_window>\r\n" \
    "example:AT+BLESCANPARAM=0,0,0,60,40\r\n"

#define WM_ATCMD_BLESCAN_USAGE                                            \
    "AT+BLESCAN=<enable>[,<duration>][,<filter_type>,<filter_param>]\r\n" \
    "example1:AT+BLESCAN=1,3,3,-70\r\n"                                   \
    "example2:AT+BLESCAN=0\r\n"

#define WM_ATCMD_BLESCANRSPDATA_USAGE        \
    "AT+BLESCANRSPDATA=<scan_resp_data>\r\n" \
    "example:AT+BLESCANRSPDATA=\"FF03112233\"\r\n"

#define WM_ATCMD_BLEADVPARAM_USAGE                                                                                           \
    "AT+BLEADVPARAM=<adv_int_min>,<adv_int_max>,<adv_type>,<own_addr_type>,<channel_map>[,<adv_filter_policy>][,<peer_addr_" \
    "type>,<peer_addr>][,<primary_phy>][,<secondary_phy>]\r\n"                                                               \
    "example1:AT+BLEADVPARAM=160,160,0,0,7\r\n"                                                                              \
    "example2:AT+BLEADVPARAM=160,160,0,0,7,0,0,\"28:6d:ce:c1:03:28\"\r\n"

#define WM_ATCMD_BLEADVDATA_USAGE  \
    "AT+BLEADVDATA=<adv_data>\r\n" \
    "example:AT+BLEADVDATA=\"0201060902574d\"\r\n"

#define WM_ATCMD_BLEADVDATAEX_USAGE                                             \
    "AT+BLEADVDATAEX?\r\n"                                                      \
    "AT+BLEADVDATAEX=<dev_name>,<uuid>,<manufacturer_data>,<include_power>\r\n" \
    "AT+BLEADVDATAEX=\"WM-PERIPHERAL\",\"1800\",\"070C112233\",1\r\n"

#define WM_ATCMD_BLEADVSTART_USAGE "AT+BLEADVSTART\r\n"

#define WM_ATCMD_BLEADVSTOP_USAGE  "AT+BLEADVSTOP\r\n"

#define WM_ATCMD_BLECONN_USAGE                                             \
    "AT+BLECONN?\r\n"                                                      \
    "AT+BLECONN=<conn_index>,<remote_address>[,<addr_type>,<timeout>]\r\n" \
    "AT+BLECONN=0,\"28:6d:ce:09:36:28\",0,10\r\n"

#define WM_ATCMD_BLECONNPARAM_USAGE                                                       \
    "AT+BLECONNPARAM?\r\n"                                                                \
    "AT+BLECONNPARAM==<conn_index>,<min_interval>,<max_interval>,<latency>,<timeout>\r\n" \
    "AT+BLECONNPARAM=0,12,16,0,300\r\n"

#define WM_ATCMD_BLEDISCONN_USAGE "AT+BLEDISCONN=<conn_index>\r\n"

#define WM_ATCMD_BLEDATALEN_USAGE                   \
    "AT+BLEDATALEN=<conn_index>,<pkt_data_len>\r\n" \
    "example:AT+BLEDATALEN=0,251\r\n"

#define WM_ATCMD_BLECFGMTU_USAGE               \
    "AT+BLECFGMTU?\r\n"                        \
    "AT+BLECFGMTU=<conn_index>,<mtu_size>\r\n" \
    "AT+BLECFGMTU=0,244\r\n"

#define WM_ATCMD_BLEGATTCSVC_USAGE    \
    "AT+BLEGATTCSVC=<conn_index>\r\n" \
    "example:AT+BLEGATTCSVC=0\r\n"

#define WM_ATCMD_BLEGATTCINCLSRV_USAGE                \
    "AT+BLEGATTCINCLSRV=<conn_index>,<srv_index>\r\n" \
    "example:AT+BLEGATTCINCLSRV=0,0\r\n"

#define WM_ATCMD_BLEGATTCSUBS_USAGE                     \
    "AT+BLEGATTCSUBS=<conn_index>,<handle>,<value>\r\n" \
    "example:AT+BLEGATTCSUBS=0,0,1\r\n"

#define WM_ATCMD_BLEGATTCCHAR_USAGE                \
    "AT+BLEGATTCCHAR=<conn_index>,<srv_index>\r\n" \
    "example:AT+BLEGATTCCHAR=0,0\r\n"

#define WM_ATCMD_BLEGATTCRD_USAGE                                            \
    "AT+BLEGATTCRD=<conn_index>,<srv_index>,<char_index>[,<desc_index>]\r\n" \
    "example:AT+BLEGATTCRD=0,1,1\r\n"

#define WM_ATCMD_BLEGATTCWR_USAGE                                                     \
    "AT+BLEGATTCWR=<conn_index>,<srv_index>,<char_index>[,<desc_index>],<length>\r\n" \
    "example:AT+BLEGATTCWR=0,1,1,,3\r\n"

#define WM_ATCMD_BLESECPARAM_USAGE                                                              \
    "AT+BLESECPARAM?\r\n"                                                                       \
    "AT+BLESECPARAM=<auth_req>,<iocap>,<enc_key_size>,<init_key>,<rsp_key>[,<auth_option>]\r\n" \
    "example:AT+BLESECPARAM=1,4,16,3,3,0\r\n"

#define WM_ATCMD_BLEIOCAP_USAGE \
    "AT+BLEIOCAP?\r\n"          \
    "AT+BLEIOCAP=<iocap>"       \
    "example:AT+BLEIOCAP=3\r\n"

#define WM_ATCMD_BLEGATTSSVC_USAGE                                                              \
    "AT+BLEGATTSSVC?\r\n"                                                                       \
    "AT+BLEGATTSSVC=<svc_uuid>,<char_uuid>,<prop>,<perm>[,<char_uuid>,<prop>,<perm>][,...]\r\n" \
    "example:AT+BLEGATTSSVC=FFF0,FFF1,10,3\r\n"                                                 \
    "example:AT+BLEGATTSSVC=FFF2,FFF3,10,3,FFF4,10,12\r\n"

#define WM_ATCMD_BLEGATTSSVCRMV_USAGE \
    "AT+BLEGATTSSVCRMV\r\n"           \
    "example:AT+BLEGATTSSVCRMV=FFF0\r\n"

#define WM_ATCMD_BLEGATTSSETATTR_USAGE "AT+BLEGATTSSETATTR=<attr_handle>,<length>"

#define WM_ATCMD_BLEGATTSNTFY_USAGE    "AT+BLEGATTSNTFY=<char_handle>,<length>"

#define WM_ATCMD_BLEGATTSIND_USAGE     "AT+BLEGATTSIND=<char_handle>,<length>"

#define WM_ATCMD_BLEENC_USAGE              \
    "AT+BLEENC=<conn_index>,<sec_act>\r\n" \
    "example:AT+BLEENC=0,3\r\n"

#define WM_ATCMD_BLEENCRSP_USAGE             \
    "AT+BLEENCRSP=<conn_index>,<accept>\r\n" \
    "example:AT+BLEENCRSP=0,1\r\n"

#define WM_ATCMD_BLEKEYREPLY_USAGE          \
    "AT+BLEKEYREPLY=<conn_index>,<key>\r\n" \
    "example:AT+BLEKEYREPLY=0,568762\r\n"

#define WM_ATCMD_BLECONFREPLY_USAGE              \
    "AT+BLECONFREPLY=<conn_index>,<confirm>\r\n" \
    "example:AT+BLECONFREPLY=0,1\r\n"

#define WM_ATCMD_BLEENCDEV_USAGE "AT+BLEENCDEV?\r\n"

#define WM_ATCMD_BLEENCCLEAR_USAGE                             \
    "AT+BLEENCCLEAR=<enc_dev_index>\r\n"                       \
    "example1(clear a specificed device):AT+BLEENCCLEAR=0\r\n" \
    "example2(clear all bonded device):AT+BLEENCCLEAR\r\n"

#define WM_ATCMD_BLESETKEY_USAGE             \
    "AT+BLESETKEY=<key,range[0,999999]>\r\n" \
    "example:AT+BLESETKEY=675668\r\n"

#define WM_ATCMD_BLEWIFIPROV_USAGE \
    "AT+BLEWIFIPROV=<init>\r\n"    \
    "example:AT+BLEWIFIPROV=1\r\n"

#define WM_ATCMD_BTINIT_USAGE \
    "AT+BTINIT?\r\n"          \
    "AT+BTINIT=<init>\r\n"    \
    "example:AT+BTINIT=1\r\n"

#define WM_ATCMD_BTNAME_USAGE     \
    "AT+BTNAME?\r\n"              \
    "AT+BTNAME=<device_name>\r\n" \
    "example:AT+BTNAME=\"wm_bt\"\r\n"

#define WM_ATCMD_BTSCANMODE_USAGE   \
    "AT+BTSCANMODE=<scan_mode>\r\n" \
    "example:AT+BTSCANMODE=2\r\n"

#define WM_ATCMD_BLESPPS_CFG                                                       \
    "AT+BLESPPSCFG=<svc_uuid>,<tx_uuid>,<prop>,<perm>,<rx_uuid>,<prop>,<perm>\r\n" \
    "example:AT+BLESPPSCFG=FFF0,FFF1,10,03,FFF2,04,02\r\n"

#define WM_ATCMD_BLESPPC_CFG                                                             \
    "AT+BLESPPCCFG=<filter_type>,<filter_type_value>,<svc_uuid>,<tx_uuid>,<rx_uuid>\r\n" \
    "example:AT+BLESPPCCFG=0,\"BLESPP\",FFF0,FFF2,FFF1\r\n"                              \
    "example:AT+BLESPPCCFG=2,FFF0,FFF0,FFF2,FFF1\r\n"

#define WM_ATCMD_BLESPPC_STOP "AT+BLESPPCSTOP"
#define WM_ATCMD_BLESPP_START "AT+BLESPPSTART"

#define WM_ATCMD_BTTEST_USAGE \
    "AT+BTTEST=<enable>\r\n"  \
    "example:AT+BTTEST=1\r\n"

int wm_atcmd_blevers(wm_at_type_e type, int argc, char **argv);
int wm_atcmd_bleinit(wm_at_type_e type, int argc, char **argv);
int wm_atcmd_bleaddr(wm_at_type_e type, int argc, char **argv);
int wm_atcmd_blename(wm_at_type_e type, int argc, char **argv);

#if defined(CONFIG_BT_OBSERVER)
int wm_atcmd_blescanparam(wm_at_type_e type, int argc, char **argv);
int wm_atcmd_blescan(wm_at_type_e type, int argc, char **argv);
int wm_atcmd_blescanrspdata(wm_at_type_e type, int argc, char **argv);
#endif

#if defined(CONFIG_BT_BROADCASTER)
int wm_atcmd_bleadvparam(wm_at_type_e type, int argc, char **argv);
int wm_atcmd_bleadvdata(wm_at_type_e type, int argc, char **argv);
int wm_atcmd_bleadvdataex(wm_at_type_e type, int argc, char **argv);
int wm_atcmd_bleadvstart(wm_at_type_e type, int argc, char **argv);
int wm_atcmd_bleadvstop(wm_at_type_e type, int argc, char **argv);
#endif

#if defined(CONFIG_BT_CENTRAL)
int wm_atcmd_bleconn(wm_at_type_e type, int argc, char **argv);
int wm_atcmd_bleconnparam(wm_at_type_e type, int argc, char **argv);
#endif
int wm_atcmd_bledisconn(wm_at_type_e type, int argc, char **argv);

#if defined(CONFIG_BT_USER_DATA_LEN_UPDATE)
int wm_atcmd_bledatalen(wm_at_type_e type, int argc, char **argv);
#endif

#if defined(CONFIG_BT_CONN)
int wm_atcmd_blecfgmtu(wm_at_type_e type, int argc, char **argv);
#if defined(CONFIG_BT_GATT_CLIENT)
int wm_atcmd_blegattcsvc(wm_at_type_e type, int argc, char **argv);
int wm_atcmd_blegattcsubscribe(wm_at_type_e type, int argc, char **argv);
int wm_atcmd_blegattcrd(wm_at_type_e type, int argc, char **argv);
int wm_atcmd_blegattcwr(wm_at_type_e type, int argc, char **argv);
#endif
#if defined(CONFIG_BT_GATT_DYNAMIC_DB)
int wm_atcmd_blegattssvc(wm_at_type_e type, int argc, char **argv);
int wm_atcmd_blegattssvcrmv(wm_at_type_e type, int argc, char **argv);
int wm_atcmd_blegattssetattr(wm_at_type_e type, int argc, char **argv);
#endif
int wm_atcmd_blegattsntfy(wm_at_type_e type, int argc, char **argv);
int wm_atcmd_blegattsind(wm_at_type_e type, int argc, char **argv);
#if defined(CONFIG_BT_SMP) || defined(CONFIG_BT_BREDR)
int wm_atcmd_blesecparam(wm_at_type_e type, int argc, char **argv);
int wm_atcmd_bleiocap(wm_at_type_e type, int argc, char **argv);
int wm_atcmd_bleenc(wm_at_type_e type, int argc, char **argv);
int wm_atcmd_bleencrsp(wm_at_type_e type, int argc, char **argv);
int wm_atcmd_blekeyreply(wm_at_type_e type, int argc, char **argv);
int wm_atcmd_bleconfreply(wm_at_type_e type, int argc, char **argv);
int wm_atcmd_bleencdev(wm_at_type_e type, int argc, char **argv);
int wm_atcmd_bleencclear(wm_at_type_e type, int argc, char **argv);
#if defined(CONFIG_BT_FIXED_PASSKEY)
int wm_atcmd_blesetkey(wm_at_type_e type, int argc, char **argv);
#endif
#endif
int wm_atcmd_blewifiprov(wm_at_type_e type, int argc, char **argv);
#endif

int wm_atcmd_blesppccfg(wm_at_type_e type, int argc, char **argv);
int wm_atcmd_blesppscfg(wm_at_type_e type, int argc, char **argv);
int wm_atcmd_blesppstart(wm_at_type_e type, int argc, char **argv);
int wm_atcmd_blesppstop(wm_at_type_e type, int argc, char **argv);

WM_ATCMD_DEFINE("AT+BLEVERS", wm_atcmd_blevers, WM_ATCMD_BLEINIT_USAGE)
WM_ATCMD_DEFINE("AT+BLEINIT", wm_atcmd_bleinit, WM_ATCMD_BLEINIT_USAGE)
WM_ATCMD_DEFINE("AT+BLEADDR", wm_atcmd_bleaddr, WM_ATCMD_BLEADDR_USAGE)
WM_ATCMD_DEFINE("AT+BLENAME", wm_atcmd_blename, WM_ATCMD_BLENAME_USAGE)
#if defined(CONFIG_BT_OBSERVER)
WM_ATCMD_DEFINE("AT+BLESCANPARAM", wm_atcmd_blescanparam, WM_ATCMD_BLESCANPARAM_USAGE)
WM_ATCMD_DEFINE("AT+BLESCAN", wm_atcmd_blescan, WM_ATCMD_BLESCAN_USAGE)
WM_ATCMD_DEFINE("AT+BLESCANRSPDATA", wm_atcmd_blescanrspdata, WM_ATCMD_BLESCANRSPDATA_USAGE)
#endif
#if defined(CONFIG_BT_BROADCASTER)
WM_ATCMD_DEFINE("AT+BLEADVPARAM", wm_atcmd_bleadvparam, WM_ATCMD_BLEADVPARAM_USAGE)
WM_ATCMD_DEFINE("AT+BLEADVDATA", wm_atcmd_bleadvdata, WM_ATCMD_BLEADVDATA_USAGE)
WM_ATCMD_DEFINE("AT+BLEADVDATAEX", wm_atcmd_bleadvdataex, WM_ATCMD_BLEADVDATAEX_USAGE)
WM_ATCMD_DEFINE("AT+BLEADVSTART", wm_atcmd_bleadvstart, WM_ATCMD_BLEADVSTART_USAGE)
WM_ATCMD_DEFINE("AT+BLEADVSTOP", wm_atcmd_bleadvstop, WM_ATCMD_BLEADVSTOP_USAGE)
#endif
#if defined(CONFIG_BT_CENTRAL)
WM_ATCMD_DEFINE("AT+BLECONN", wm_atcmd_bleconn, WM_ATCMD_BLECONN_USAGE)
WM_ATCMD_DEFINE("AT+BLECONNPARAM", wm_atcmd_bleconnparam, WM_ATCMD_BLECONNPARAM_USAGE)
#endif
WM_ATCMD_DEFINE("AT+BLEDISCONN", wm_atcmd_bledisconn, WM_ATCMD_BLEDISCONN_USAGE)
#if defined(CONFIG_BT_USER_DATA_LEN_UPDATE)
WM_ATCMD_DEFINE("AT+BLEDATALEN", wm_atcmd_bledatalen, WM_ATCMD_BLEDATALEN_USAGE)
#endif
#if defined(CONFIG_BT_CONN)
WM_ATCMD_DEFINE("AT+BLECFGMTU", wm_atcmd_blecfgmtu, WM_ATCMD_BLECFGMTU_USAGE)
#if defined(CONFIG_BT_GATT_CLIENT)
WM_ATCMD_DEFINE("AT+BLEGATTCSVC", wm_atcmd_blegattcsvc, WM_ATCMD_BLEGATTCSVC_USAGE)
WM_ATCMD_DEFINE("AT+BLEGATTCSUBS", wm_atcmd_blegattcsubscribe, WM_ATCMD_BLEGATTCSUBS_USAGE)
WM_ATCMD_DEFINE("AT+BLEGATTCRD", wm_atcmd_blegattcrd, WM_ATCMD_BLEGATTCRD_USAGE)
WM_ATCMD_DEFINE("AT+BLEGATTCWR", wm_atcmd_blegattcwr, WM_ATCMD_BLEGATTCWR_USAGE)
#endif
#if defined(CONFIG_BT_GATT_DYNAMIC_DB)
WM_ATCMD_DEFINE("AT+BLEGATTSSVC", wm_atcmd_blegattssvc, WM_ATCMD_BLEGATTSSVC_USAGE)
WM_ATCMD_DEFINE("AT+BLEGATTSSVCRMV", wm_atcmd_blegattssvcrmv, WM_ATCMD_BLEGATTSSVCRMV_USAGE)
WM_ATCMD_DEFINE("AT+BLEGATTSSETATTR", wm_atcmd_blegattssetattr, WM_ATCMD_BLEGATTSSETATTR_USAGE)

#endif
WM_ATCMD_DEFINE("AT+BLEGATTSNTFY", wm_atcmd_blegattsntfy, WM_ATCMD_BLEGATTSNTFY_USAGE)
WM_ATCMD_DEFINE("AT+BLEGATTSIND", wm_atcmd_blegattsind, WM_ATCMD_BLEGATTSIND_USAGE)

#if defined(CONFIG_BT_SMP) || defined(CONFIG_BT_BREDR)
WM_ATCMD_DEFINE("AT+BLESECPARAM", wm_atcmd_blesecparam, WM_ATCMD_BLESECPARAM_USAGE)
WM_ATCMD_DEFINE("AT+BLEIOCAP", wm_atcmd_bleiocap, WM_ATCMD_BLEIOCAP_USAGE)
WM_ATCMD_DEFINE("AT+BLEENC", wm_atcmd_bleenc, WM_ATCMD_BLEENC_USAGE)
WM_ATCMD_DEFINE("AT+BLEENCRSP", wm_atcmd_bleencrsp, WM_ATCMD_BLEENCRSP_USAGE)
WM_ATCMD_DEFINE("AT+BLEKEYREPLY", wm_atcmd_blekeyreply, WM_ATCMD_BLEKEYREPLY_USAGE)
WM_ATCMD_DEFINE("AT+BLECONFREPLY", wm_atcmd_bleconfreply, WM_ATCMD_BLECONFREPLY_USAGE)
WM_ATCMD_DEFINE("AT+BLEENCDEV", wm_atcmd_bleencdev, WM_ATCMD_BLEENCDEV_USAGE)
WM_ATCMD_DEFINE("AT+BLEENCCLEAR", wm_atcmd_bleencclear, WM_ATCMD_BLEENCCLEAR_USAGE)
#if defined(CONFIG_BT_FIXED_PASSKEY)
WM_ATCMD_DEFINE("AT+BLESETKEY", wm_atcmd_blesetkey, WM_ATCMD_BLESETKEY_USAGE)
#endif
#endif
WM_ATCMD_DEFINE("AT+BLEWIFIPROV", wm_atcmd_blewifiprov, WM_ATCMD_BLEWIFIPROV_USAGE)

WM_ATCMD_DEFINE("AT+BLESPPCCFG", wm_atcmd_blesppccfg, WM_ATCMD_BLESPPC_CFG)
WM_ATCMD_DEFINE("AT+BLESPPSCFG", wm_atcmd_blesppscfg, WM_ATCMD_BLESPPS_CFG)
WM_ATCMD_DEFINE("AT+BLESPPSTART", wm_atcmd_blesppstart, WM_ATCMD_BLESPP_START)
WM_ATCMD_DEFINE("AT+BLESPPSTOP", wm_atcmd_blesppstop, WM_ATCMD_BLESPPC_STOP)

#endif

#endif //CONFIG_WM_ATCMD_BT_ENABLE

#ifdef __cplusplus
}
#endif

#endif /* __WM_ATCMD_BT_H__ */
