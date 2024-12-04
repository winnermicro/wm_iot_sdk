/**
 * @file wm_atcmd_fs.h
 *
 * @brief Default FS AT Command
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

#ifndef __WM_ATCMD_FS_H__
#define __WM_ATCMD_FS_H__

#ifdef __cplusplus
extern "C" {
#endif

#if CONFIG_WM_ATCMD_FS_ENABLE

#define WM_ATCMD_FSMOUNT_USAGE    \
    "AT+FSMOUNT=<\"mount\">\r\n"  \
    "example1 : AT+FSMOUNT=1\r\n" \
    "example2 : AT+FSMOUNT=0\r\n"

#define WM_ATCMD_FS_USAGE                                                           \
    "AT+FS=<\"type\">,<\"operation\">,<\"filename\">,<\"offset\">,<\"length\">\r\n" \
    "example1 : AT+FS=0,0,\"filename\"\r\n"                                         \
    "example2 : AT+FS=0,1,\"filename\",100,10\r\n"                                  \
    "example3 : AT+FS=0,2,\"filename\",0,100\r\n"                                   \
    "example4 : AT+FS=0,4,\".\"\r\n"

int wm_atcmd_fsmount(wm_at_type_e type, int argc, char **argv);
int wm_atcmd_fs(wm_at_type_e type, int argc, char **argv);

WM_ATCMD_DEFINE("AT+FSMOUNT", wm_atcmd_fsmount, WM_ATCMD_FSMOUNT_USAGE);
WM_ATCMD_DEFINE("AT+FS", wm_atcmd_fs, WM_ATCMD_FS_USAGE);

#endif //CONFIG_WM_ATCMD_FS_ENABLE

#ifdef __cplusplus
}
#endif

#endif /* __WM_ATCMD_FS_H__ */
