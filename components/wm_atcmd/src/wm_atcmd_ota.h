/**
 * @file wm_atcmd_ota.h
 *
 * @brief Default OTA AT Command
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

#ifndef __WM_ATCMD_OTA_H__
#define __WM_ATCMD_OTA_H__

#ifdef __cplusplus
extern "C" {
#endif

#if CONFIG_WM_ATCMD_OTA_ENABLE

#define WM_ATCMD_OTA_HTTP_USAGE \
    "AT+OTA_HTTP=<\"url\">\r\n" \
    "example1 : AT+OTA_HTTP =\"http://img_example.com/wxxx_ota.img\"\r\n"

int wm_atcmd_ota_http(wm_at_type_e type, int argc, char **argv);

WM_ATCMD_DEFINE("AT+OTA_HTTP", wm_atcmd_ota_http, WM_ATCMD_OTA_HTTP_USAGE);

#endif //CONFIG_WM_ATCMD_OTA_ENABLE

#ifdef __cplusplus
}
#endif

#endif /* __WM_ATCMD_OTA_H__ */
