/**
 * @file wm_atcmd_http.h
 *
 * @brief Default HTTP AT Command
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

#ifndef __WM_ATCMD_HTTP_H__
#define __WM_ATCMD_HTTP_H__

#ifdef __cplusplus
extern "C" {
#endif

#if CONFIG_WM_ATCMD_HTTP_ENABLE

#define WM_ATCMD_HTTPCLIENT_USAGE                                                                                        \
    "AT+HTTPCLIENT=<opt>,<content-type>,<\"url\">,[<\"host\">],[<\"path\">],<transport_type>[,<\"data\">][,<\"http_req_" \
    "header\">][,<\"http_req_header\">][...]\r\n"                                                                        \
    "example1:AT+HTTPCLIENT=2,0,\"http://httpbin.org/get\",\"httpbin.org\",\"/get\",1\r\n"                               \
    "example2:AT+HTTPCLIENT=3,0,\"http://httpbin.org/post\",\"httpbin.org\",\"/post\",1,\"field1=value1&field2=value2\"\r\n"

#define WM_ATCMD_HTTPGETSIZE_USAGE                                     \
    "AT+HTTPGETSIZE=<\"url\">[,<tx size>][,<rx size>][,<timeout>]\r\n" \
    "example:AT+HTTPGETSIZE=\"http://www.baidu.com/img/bdlogo.gif\",1024,1024,2500\r\n"

#define WM_ATCMD_HTTPCGET_USAGE                                     \
    "AT+HTTPCGET=<\"url\">[,<tx size>][,<rx size>][,<timeout>]\r\n" \
    "example:AT+HTTPCGET=\"http://www.baidu.com/img/bdlogo.gif\",1024,1024,2500\r\n"

#define WM_ATCMD_HTTPCPOST_USAGE                                                                         \
    "AT+HTTPCPOST=<\"url\">,<length>[,<http_req_header_cnt>][,<http_req_header>..<http_req_header>]\r\n" \
    "example:AT+HTTPCPOST=\"http://httpbin.org/post\",427,2,\"connection: keep-alive\",\"content-type: application/json\"\r\n"

#define WM_ATCMD_HTTPCPUT_USAGE                                                                         \
    "AT+HTTPCPUT=<\"url\">,<length>[,<http_req_header_cnt>][,<http_req_header>..<http_req_header>]\r\n" \
    "example:AT+HTTPCPUT=\"http://httpbin.org/put\",427,2,\"connection: keep-alive\",\"content-type: application/json\"\r\n"

#define WM_ATCMD_HTTPURLCFG_USAGE    \
    "AT+HTTPURLCFG?\r\n"             \
    "AT+HTTPURLCFG=<url length>\r\n" \
    "example1:AT+HTTPURLCFG=0\r\n"   \
    "example2:AT+HTTPURLCFG=10\r\n"

#define WM_ATCMD_HTTPCHEAD_USAGE        \
    "AT+HTTPCHEAD?\r\n"                 \
    "AT+HTTPCHEAD=<req_header_len>\r\n" \
    "example1:AT+HTTPCHEAD=0\r\n"       \
    "example2:AT+HTTPCHEAD=10\r\n"

int wm_atcmd_httpclient(wm_at_type_e type, int argc, char **argv);
int wm_atcmd_httpgetsize(wm_at_type_e type, int argc, char **argv);
int wm_atcmd_httpcget(wm_at_type_e type, int argc, char **argv);
int wm_atcmd_httpcpost(wm_at_type_e type, int argc, char **argv);
int wm_atcmd_httpcput(wm_at_type_e type, int argc, char **argv);
int wm_atcmd_httpurlcfg(wm_at_type_e type, int argc, char **argv);
int wm_atcmd_httpchead(wm_at_type_e type, int argc, char **argv);

WM_ATCMD_DEFINE("AT+HTTPCLIENT", wm_atcmd_httpclient, WM_ATCMD_HTTPCLIENT_USAGE);
WM_ATCMD_DEFINE("AT+HTTPGETSIZE", wm_atcmd_httpgetsize, WM_ATCMD_HTTPGETSIZE_USAGE);
WM_ATCMD_DEFINE("AT+HTTPCGET", wm_atcmd_httpcget, WM_ATCMD_HTTPCGET_USAGE);
WM_ATCMD_DEFINE("AT+HTTPCPOST", wm_atcmd_httpcpost, WM_ATCMD_HTTPCPOST_USAGE);
WM_ATCMD_DEFINE("AT+HTTPCPUT", wm_atcmd_httpcput, WM_ATCMD_HTTPCPUT_USAGE);
WM_ATCMD_DEFINE("AT+HTTPURLCFG", wm_atcmd_httpurlcfg, WM_ATCMD_HTTPURLCFG_USAGE);
WM_ATCMD_DEFINE("AT+HTTPCHEAD", wm_atcmd_httpchead, WM_ATCMD_HTTPCHEAD_USAGE);

#endif //CONFIG_WM_ATCMD_HTTP_ENABLE

#ifdef __cplusplus
}
#endif

#endif /* __WM_ATCMD_HTTP_H__ */
