/**
 * @file wm_atcmd_sys.h
 *
 * @brief Default System AT Command
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

#ifndef __WM_ATCMD_SYS_H__
#define __WM_ATCMD_SYS_H__

#ifdef __cplusplus
extern "C" {
#endif

#if CONFIG_WM_ATCMD_SYS_ENABLE

int wm_atcmd_at(wm_at_type_e type, int argc, char **argv);
int wm_atcmd_rst(wm_at_type_e type, int argc, char **argv);
int wm_atcmd_assert(wm_at_type_e type, int argc, char **argv);
int wm_atcmd_sysram(wm_at_type_e type, int argc, char **argv);

#if CONFIG_COMPONENT_NVS_ENABLED
int wm_atcmd_restore(wm_at_type_e type, int argc, char **argv);
int wm_atcmd_sysstore(wm_at_type_e type, int argc, char **argv);
#endif

WM_ATCMD_DEFINE("AT", wm_atcmd_at, NULL);
WM_ATCMD_DEFINE("AT+RST", wm_atcmd_rst, NULL);
WM_ATCMD_DEFINE("AT+ASSERT", wm_atcmd_assert, NULL);
WM_ATCMD_DEFINE("AT+SYSRAM", wm_atcmd_sysram, NULL);

#if CONFIG_COMPONENT_NVS_ENABLED
WM_ATCMD_DEFINE("AT+RESTORE", wm_atcmd_restore, NULL);
WM_ATCMD_DEFINE("AT+SYSSTORE", wm_atcmd_sysstore, NULL);
#endif

#endif //CONFIG_WM_ATCMD_SYS_ENABLE

#ifdef __cplusplus
}
#endif

#endif /* __WM_ATCMD_SYS_H__ */
