/**
 * @file wm_atcmd_tcp.h
 *
 * @brief Default TCP AT Command
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

#ifndef __WM_ATCMD_TCP_H__
#define __WM_ATCMD_TCP_H__

#ifdef __cplusplus
extern "C" {
#endif

#if CONFIG_WM_ATCMD_TCP_ENABLE

#define WM_ATCMD_CIPV6_USAGE    "AT+CIPV6?\r\n"

#define WM_ATCMD_CIPSTATE_USAGE "AT+CIPSTATE?\r\n"

#define WM_ATCMD_CIPDOMAIN_USAGE                        \
    "AT+CIPDOMAIN=<\"domain name\">[,<ip network>]\r\n" \
    "example:AT+CIPDOMAIN=\"www.baidu.com\",2\r\n"

#define WM_ATCMD_CIPMUX_USAGE \
    "AT+CIPMUX?\r\n"          \
    "AT+CIPMUX=<mode>\r\n"    \
    "example:AT+CIPMUX=0\r\n"

#define WM_ATCMD_CIPSERVER_USAGE                                    \
    "AT+CIPSERVER?\r\n"                                             \
    "AT+CIPSERVER=<mode>[,<param2>][,<\"type\">][,<CA enable>]\r\n" \
    "example1:AT+CIPSERVER=1,80\r\n"                                \
    "example2:AT+CIPSERVER=1,443,\"SSL\",1\r\n"                     \
    "example3:AT+CIPSERVER=0,1\r\n"

#define WM_ATCMD_CIPSERVERMAXCONN_USAGE \
    "AT+CIPSERVERMAXCONN?\r\n"          \
    "AT+CIPSERVERMAXCONN=<num>\r\n"     \
    "example:AT+CIPSERVERMAXCONN=3\r\n"

#define WM_ATCMD_CIPSTART_USAGE                                                                                 \
    "TCP Connect:\r\n"                                                                                          \
    "AT+CIPSTART=<\"type\">,<\"remote host\">,<remote port>[,<keep_alive>][,<\"local IP\">]\r\n"                \
    "AT+CIPSTART=<link ID>,<\"type\">,<\"remote host\">,<remote port>[,<keep_alive>][,<\"local IP\">]\r\n"      \
    "example1:AT+CIPSTART=\"TCP\",\"192.168.10.110\",8080\r\n"                                                  \
    "UDP Connect:\r\n"                                                                                          \
    "AT+CIPSTART=<\"type\">,<\"remote host\">,<remote port>[,<local port>,<mode>,<\"local IP\">]\r\n"           \
    "AT+CIPSTART=<link ID>,<\"type\">,<\"remote host\">,<remote port>[,<local port>,<mode>,<\"local IP\">]\r\n" \
    "example2:AT+CIPSTART=\"UDP\",\"192.168.101.110\",8080,1002,2\r\n"                                          \
    "SSL Connect:\r\n"                                                                                          \
    "AT+CIPSTART=<\"type\">,<\"remote host\">,<remote port>[,<keep_alive>,<\"local IP\">]\r\n"                  \
    "AT+CIPSTART=<link ID>,<\"type\">,<\"remote host\">,<remote port>[,<keep_alive>,<\"local IP\">]\r\n"        \
    "example3:AT+CIPSTART=\"SSL\",\"www.baidu.com\",8443\r\n"

#define WM_ATCMD_CIPSTARTEX_USAGE                                                                       \
    "TCP Connect:\r\n"                                                                                  \
    "AT+CIPSTARTEX=<\"type\">,<\"remote host\">,<remote port>[,<keep_alive>][,<\"local IP\">]\r\n"      \
    "example1:AT+CIPSTARTEX=\"TCP\",\"192.168.10.110\",8080\r\n"                                        \
    "UDP Connect:\r\n"                                                                                  \
    "AT+CIPSTARTEX=<\"type\">,<\"remote host\">,<remote port>[,<local port>,<mode>,<\"local IP\">]\r\n" \
    "example2:AT+CIPSTARTEX=\"UDP\",\"192.168.101.110\",8080,1002,2\r\n"                                \
    "SSL Connect:\r\n"                                                                                  \
    "AT+CIPSTARTEX=<\"type\">,<\"remote host\">,<remote port>[,<keep_alive>,<\"local IP\">]\r\n"        \
    "example3:AT+CIPSTARTEX=\"SSL\",\"www.baidu.com\",8443\r\n"

#define WM_ATCMD_CIPCLOSE_USAGE  \
    "AT+CIPCLOSE=<link ID>\r\n"  \
    "example1:AT+CIPCLOSE=0\r\n" \
    "example2:AT+CIPCLOSE=5\r\n"

#define WM_ATCMD_CIPSEND_USAGE                                              \
    "AT+CIPSEND=<length>\r\n"                                               \
    "AT+CIPSEND=[<link ID>,]<length>[,<\"remote host\">,<remote port>]\r\n" \
    "AT+CIPSEND\r\n"                                                        \
    "example1:AT+CIPSEND=10\r\n"                                            \
    "example2:AT+CIPSEND=1,10\r\n"

#define WM_ATCMD_CIPRECVMODE_USAGE \
    "AT+CIPRECVMODE?\r\n"          \
    "AT+CIPRECVMODE=<mode>\r\n"    \
    "example:AT+CIPRECVMODE=1\r\n"

#define WM_ATCMD_CIPRECVDATA_USAGE       \
    "AT+CIPRECVDATA=<link_id>,<len>\r\n" \
    "example:AT+CIPRECVDATA=0,100\r\n"

#define WM_ATCMD_CIPRECVLEN_USAGE "AT+CIPRECVLEN?\r\n"

#define WM_ATCMD_PING_USAGE  \
    "AT+PING=<\"host\">\r\n" \
    "example:AT+PING=\"www.baidu.com\"\r\n"

#define WM_ATCMD_CIPMODE_USAGE \
    "AT+CIPMODE?\r\n"          \
    "AT+CIPMODE=<mode>\r\n"    \
    "example:AT+CIPMODE=1\r\n"

#define WM_ATCMD_CIPSENDL_USAGE    ""

#define WM_ATCMD_CIPSENDLCFG_USAGE ""

#define WM_ATCMD_CIPSENDEX_USAGE                                              \
    "AT+CIPSENDEX=<link ID>,<length>\r\n"                                     \
    "AT+CIPSENDEX=[<link ID>,]<length>[,<\"remote host\">,<remote port>]\r\n" \
    "example:AT+CIPSENDEX=2,10\r\n"

#define WM_ATCMD_CIFSR_USAGE "AT+CIFSR\r\n"

#define WM_ATCMD_CIPSTO_USAGE \
    "AT+CIPSTO?\r\n"          \
    "AT+CIPSTO=<time>\r\n"    \
    "example:AT+CIPSTO=10\r\n"

#define WM_ATCMD_CIPFWVER_USAGE ""

#define WM_ATCMD_CIUPDATE_USAGE ""

#define WM_ATCMD_CIPDINFO_USAGE \
    "AT+CIPDINFO?\r\n"          \
    "AT+CIPDINFO=<mode>\r\n"    \
    "example:AT+CIPDINFO=1\r\n"

#define WM_ATCMD_CIPRECONNINTV_USAGE  \
    "AT+CIPRECONNINTV?\r\n"           \
    "AT+CIPRECONNINTV=<interval>\r\n" \
    "example:AT+CIPRECONNINTV=10\r\n"

#define WM_ATCMD_CIPDNS_USAGE                                 \
    "AT+CIPDNS?\r\n"                                          \
    "AT+CIPDNS=<enable>[,<\"DNS IP1\">][,<\"DNS IP2\">]\r\n"  \
    "example1:AT+CIPDNS=1,\"208.67.222.222\",\"8.8.8.8\"\r\n" \
    "example2:AT+CIPDNS=0\r\n"

#define WM_ATCMD_CIPTCPOPT_USAGE                                                              \
    "AT+CIPTCPOPT?\r\n"                                                                       \
    "AT+CIPTCPOPT=[<so_linger>],[<tcp_nodelay>],[<so_sndtimeo>][,<keep_alive>]\r\n"           \
    "AT+CIPTCPOPT=<link ID>,[<so_linger>],[<tcp_nodelay>],[<so_sndtimeo>][,<keep_alive>]\r\n" \
    "example:AT+CIPTCPOPT=2,2,1,1000,50\r\n"

#define WM_ATCMD_CIPSSLCCONF_USAGE                                          \
    "AT+CIPSSLCCONF?\r\n"                                                   \
    "AT+CIPSSLCCONF=<auth_mode>[,<pki_number>][,<ca_number>]\r\n"           \
    "AT+CIPSSLCCONF=<link ID>,<auth_mode>[,<pki_number>][,<ca_number>]\r\n" \
    "example:AT+CIPSSLCCONF=2,0,0,0\r\n"

#define WM_ATCMD_CIPSSLCCN_USAGE                   \
    "AT+CIPSSLCCN?\r\n"                            \
    "AT+CIPSSLCCN=<\"common name\">\r\n"           \
    "AT+CIPSSLCCN=<link ID>,<\"common name\">\r\n" \
    "example:AT+CIPSSLCCN=\"www.baidu.com\"\r\n"

#define WM_ATCMD_CIPSSLCSNI_USAGE           \
    "AT+CIPSSLCSNI?\r\n"                    \
    "AT+CIPSSLCSNI=<\"sni\">\r\n"           \
    "AT+CIPSSLCSNI=<link ID>,<\"sni\">\r\n" \
    "example:AT+CIPSSLCSNI=\"baidu.com\"\r\n"

#define WM_ATCMD_CIPSSLCALPN_USAGE                                                \
    "AT+CIPSSLCALPN?\r\n"                                                         \
    "AT+CIPSSLCALPN=<counts>[,<\"alpn\">][,<\"alpn\">][,<\"alpn\">]\r\n"          \
    "AT+CIPSSLCALPN=<link ID>,<counts>[,<\"alpn\">][,<\"alpn\">[,<\"alpn\">]\r\n" \
    "example1:AT+CIPSSLCALPN=2,\"ssl-ca.cn\",\"ssl-ca.us\"\r\n"                   \
    "example2:AT+CIPSSLCALPN=0\r\n"

#define WM_ATCMD_CIPSSLCPSK_USAGE                      \
    "AT+CIPSSLCPSK?\r\n"                               \
    "AT+CIPSSLCPSK=<\"psk\">,<\"hint\">\r\n"           \
    "AT+CIPSSLCPSK=<link ID>,<\"psk\">,<\"hint\">\r\n" \
    "example:AT+CIPSSLCPSK=\"436c69656e745f6964656e74697479\",\"123456\"\r\n"

#define WM_ATCMD_CIPSNTPCFG_USAGE                                                              \
    "AT+CIPSNTPCFG?\r\n"                                                                       \
    "AT+CIPSNTPCFG=<enable>,<timezone>[,<SNTP server1>][,<SNTP server2>][,<SNTP server3>]\r\n" \
    "example1:AT+CIPSNTPCFG=1,8,\"cn.ntp.org.cn\",\"ntp.sjtu.edu.cn\"\r\n"                     \
    "example2:AT+CIPSNTPCFG=1,1245,\"0.pool.ntp.org\",\"time.google.com\"\r\n"

#define WM_ATCMD_CIPSNTPTIME_USAGE                                                                                         \
    "AT+CIPSNTPTIME?\r\n"                                                                                                  \
    "example:AT+CWMODE=1\r\nAT+CWJAP=\"1234567890\",\"1234567890\"\r\nAT+CIPSNTPCFG=1,8,\"cn.ntp.org.cn\",\"ntp.sjtu.edu." \
    "cn\"\r\nAT+CIPSNTPTIME?\r\n"

#define WM_ATCMD_CIPSNTPINTV_USAGE         \
    "AT+CIPSNTPINTV?\r\n"                  \
    "AT+CIPSNTPINTV=<interval second>\r\n" \
    "example:AT+CIPSNTPINTV=3600\r\n"

int wm_atcmd_cipv6(wm_at_type_e type, int argc, char **argv);
int wm_atcmd_cipstate(wm_at_type_e type, int argc, char **argv);
int wm_atcmd_cipdomain(wm_at_type_e type, int argc, char **argv);
int wm_atcmd_cipmux(wm_at_type_e type, int argc, char **argv);
int wm_atcmd_cipserver(wm_at_type_e type, int argc, char **argv);
int wm_atcmd_cipservermaxconn(wm_at_type_e type, int argc, char **argv);
int wm_atcmd_cipstart(wm_at_type_e type, int argc, char **argv);
int wm_atcmd_cipstartex(wm_at_type_e type, int argc, char **argv);
int wm_atcmd_cipclose(wm_at_type_e type, int argc, char **argv);
int wm_atcmd_cipsend(wm_at_type_e type, int argc, char **argv);
int wm_atcmd_ciprecvmode(wm_at_type_e type, int argc, char **argv);
int wm_atcmd_ciprecvdata(wm_at_type_e type, int argc, char **argv);
int wm_atcmd_ciprecvlen(wm_at_type_e type, int argc, char **argv);
int wm_atcmd_ping(wm_at_type_e type, int argc, char **argv);
int wm_atcmd_cipmode(wm_at_type_e type, int argc, char **argv);
int wm_atcmd_cipsendl(wm_at_type_e type, int argc, char **argv);
int wm_atcmd_cipsendlcfg(wm_at_type_e type, int argc, char **argv);
int wm_atcmd_cipsendex(wm_at_type_e type, int argc, char **argv);
int wm_atcmd_cifsr(wm_at_type_e type, int argc, char **argv);
int wm_atcmd_cipsto(wm_at_type_e type, int argc, char **argv);
int wm_atcmd_cipfwver(wm_at_type_e type, int argc, char **argv);
int wm_atcmd_ciupdate(wm_at_type_e type, int argc, char **argv);
int wm_atcmd_cipdinfo(wm_at_type_e type, int argc, char **argv);
int wm_atcmd_cipreconnintv(wm_at_type_e type, int argc, char **argv);
int wm_atcmd_cipdns(wm_at_type_e type, int argc, char **argv);
int wm_atcmd_ciptcpopt(wm_at_type_e type, int argc, char **argv);
int wm_atcmd_cipsslcconf(wm_at_type_e type, int argc, char **argv);
int wm_atcmd_cipsslccn(wm_at_type_e type, int argc, char **argv);
int wm_atcmd_cipsslcsni(wm_at_type_e type, int argc, char **argv);
int wm_atcmd_cipsslcalpn(wm_at_type_e type, int argc, char **argv);
int wm_atcmd_cipsslcpsk(wm_at_type_e type, int argc, char **argv);
int wm_atcmd_cipsntpcfg(wm_at_type_e type, int argc, char **argv);
int wm_atcmd_cipsntptime(wm_at_type_e type, int argc, char **argv);
int wm_atcmd_cipsntpintv(wm_at_type_e type, int argc, char **argv);

WM_ATCMD_DEFINE("AT+CIPV6", wm_atcmd_cipv6, WM_ATCMD_CIPV6_USAGE);
WM_ATCMD_DEFINE("AT+CIPSTATE", wm_atcmd_cipstate, WM_ATCMD_CIPSTATE_USAGE);
WM_ATCMD_DEFINE("AT+CIPDOMAIN", wm_atcmd_cipdomain, WM_ATCMD_CIPDOMAIN_USAGE);
WM_ATCMD_DEFINE("AT+CIPMUX", wm_atcmd_cipmux, WM_ATCMD_CIPMUX_USAGE);
WM_ATCMD_DEFINE("AT+CIPSERVER", wm_atcmd_cipserver, WM_ATCMD_CIPSERVER_USAGE);
WM_ATCMD_DEFINE("AT+CIPSERVERMAXCONN", wm_atcmd_cipservermaxconn, WM_ATCMD_CIPSERVERMAXCONN_USAGE);
WM_ATCMD_DEFINE("AT+CIPSTART", wm_atcmd_cipstart, WM_ATCMD_CIPSTART_USAGE);
WM_ATCMD_DEFINE("AT+CIPSTARTEX", wm_atcmd_cipstartex, WM_ATCMD_CIPSTARTEX_USAGE);
WM_ATCMD_DEFINE("AT+CIPCLOSE", wm_atcmd_cipclose, WM_ATCMD_CIPCLOSE_USAGE);
WM_ATCMD_DEFINE("AT+CIPSEND", wm_atcmd_cipsend, WM_ATCMD_CIPSEND_USAGE);
WM_ATCMD_DEFINE("AT+CIPRECVMODE", wm_atcmd_ciprecvmode, WM_ATCMD_CIPRECVMODE_USAGE);
WM_ATCMD_DEFINE("AT+CIPRECVDATA", wm_atcmd_ciprecvdata, WM_ATCMD_CIPRECVDATA_USAGE);
WM_ATCMD_DEFINE("AT+CIPRECVLEN", wm_atcmd_ciprecvlen, WM_ATCMD_CIPRECVLEN_USAGE);
WM_ATCMD_DEFINE("AT+PING", wm_atcmd_ping, WM_ATCMD_PING_USAGE);
WM_ATCMD_DEFINE("AT+CIPMODE", wm_atcmd_cipmode, WM_ATCMD_CIPMODE_USAGE);
WM_ATCMD_DEFINE("AT+CIPSENDL", wm_atcmd_cipsendl, WM_ATCMD_CIPSENDL_USAGE);
WM_ATCMD_DEFINE("AT+CIPSENDLCFG", wm_atcmd_cipsendlcfg, WM_ATCMD_CIPSENDLCFG_USAGE);
WM_ATCMD_DEFINE("AT+CIPSENDEX", wm_atcmd_cipsendex, WM_ATCMD_CIPSENDEX_USAGE);
WM_ATCMD_DEFINE("AT+CIFSR", wm_atcmd_cifsr, WM_ATCMD_CIFSR_USAGE);
WM_ATCMD_DEFINE("AT+CIPSTO", wm_atcmd_cipsto, WM_ATCMD_CIPSTO_USAGE);
WM_ATCMD_DEFINE("AT+CIPFWVER", wm_atcmd_cipfwver, WM_ATCMD_CIPFWVER_USAGE);
WM_ATCMD_DEFINE("AT+CIUPDATE", wm_atcmd_ciupdate, WM_ATCMD_CIUPDATE_USAGE);
WM_ATCMD_DEFINE("AT+CIPDINFO", wm_atcmd_cipdinfo, WM_ATCMD_CIPDINFO_USAGE);
WM_ATCMD_DEFINE("AT+CIPRECONNINTV", wm_atcmd_cipreconnintv, WM_ATCMD_CIPRECONNINTV_USAGE);
WM_ATCMD_DEFINE("AT+CIPDNS", wm_atcmd_cipdns, WM_ATCMD_CIPDNS_USAGE);
WM_ATCMD_DEFINE("AT+CIPTCPOPT", wm_atcmd_ciptcpopt, WM_ATCMD_CIPTCPOPT_USAGE);
WM_ATCMD_DEFINE("AT+CIPSSLCCONF", wm_atcmd_cipsslcconf, WM_ATCMD_CIPSSLCCONF_USAGE);
WM_ATCMD_DEFINE("AT+CIPSSLCCN", wm_atcmd_cipsslccn, WM_ATCMD_CIPSSLCCN_USAGE);
WM_ATCMD_DEFINE("AT+CIPSSLCSNI", wm_atcmd_cipsslcsni, WM_ATCMD_CIPSSLCSNI_USAGE);
WM_ATCMD_DEFINE("AT+CIPSSLCALPN", wm_atcmd_cipsslcalpn, WM_ATCMD_CIPSSLCALPN_USAGE);
WM_ATCMD_DEFINE("AT+CIPSSLCPSK", wm_atcmd_cipsslcpsk, WM_ATCMD_CIPSSLCPSK_USAGE);
WM_ATCMD_DEFINE("AT+CIPSNTPCFG", wm_atcmd_cipsntpcfg, WM_ATCMD_CIPSNTPCFG_USAGE);
WM_ATCMD_DEFINE("AT+CIPSNTPTIME", wm_atcmd_cipsntptime, WM_ATCMD_CIPSNTPTIME_USAGE);
WM_ATCMD_DEFINE("AT+CIPSNTPINTV", wm_atcmd_cipsntpintv, WM_ATCMD_CIPSNTPINTV_USAGE);

#endif //CONFIG_WM_ATCMD_TCP_ENABLE

#ifdef __cplusplus
}
#endif

#endif /* __WM_ATCMD_TCP_H__ */
