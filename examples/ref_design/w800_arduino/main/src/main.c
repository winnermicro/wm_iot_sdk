/**
 * @file main.c
 *
 * @brief The reference design project running on w800 Arduino board
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

#define LOG_TAG "w800_arduino"
#include "wm_log.h"

#include <stdio.h>
#include <string.h>
#include "wmsdk_config.h"

/*Below include statement can be removed if user no need to get heap, partition table info in this project*/
#include "wm_heap.h"
#include "wm_partition_table.h"

int main(void)
{
    wm_log_raw(WM_LOG_LEVEL_INFO, "heap free size=%d(bytes)\r\n", wm_heap_get_free_size());

    wm_partition_table_print();

    wm_log_raw(WM_LOG_LEVEL_INFO, "\r\n ================================================================================\r\n");
    wm_log_raw(WM_LOG_LEVEL_INFO, " |  W800 Demo Project on W800_ARDUINO_Vx.x Board                                |\r\n");
    wm_log_raw(WM_LOG_LEVEL_INFO, " |                                                                              |\r\n");
    wm_log_raw(WM_LOG_LEVEL_INFO, " |  1. Print 'help' to get overview of commands                                 |\r\n");
    wm_log_raw(WM_LOG_LEVEL_INFO, " |  2. Configure device to station                                              |\r\n");
    wm_log_raw(WM_LOG_LEVEL_INFO, " |                                                                              |\r\n");
    wm_log_raw(WM_LOG_LEVEL_INFO, " |  3. Setup WiFi connection                                                    |\r\n");
    wm_log_raw(WM_LOG_LEVEL_INFO, " |     Suggest preparing an AP router or setting up a hotspot on a phone with   |\r\n");
    wm_log_raw(WM_LOG_LEVEL_INFO, " |     accessible internet. Once the W800 connects to the AP or hotspot,        |\r\n");
    wm_log_raw(WM_LOG_LEVEL_INFO, " |     it will be able to access (ping or use HTTP) internet resources.         |\r\n");
    wm_log_raw(WM_LOG_LEVEL_INFO, " |                                                                              |\r\n");
    wm_log_raw(WM_LOG_LEVEL_INFO, " |  4. Run ifconfig to check ip info                                            |\r\n");
    wm_log_raw(WM_LOG_LEVEL_INFO, " |  5. Run ping to check internet/ethernet link                                 |\r\n");
    wm_log_raw(WM_LOG_LEVEL_INFO, " |  6. Run http to download specific webpage                                    |\r\n");
    wm_log_raw(WM_LOG_LEVEL_INFO, " |     Put following cmd to know more for this activity from webpage            |\r\n");
    wm_log_raw(WM_LOG_LEVEL_INFO, " |     CMD: http client get https://doc.winnermicro.net/download/html/w800.txt  |\r\n");
    wm_log_raw(WM_LOG_LEVEL_INFO, " |  7. Run pwm to control RGB led on W800_ARDUINO_Vx.x Board                    |\r\n");
    wm_log_raw(WM_LOG_LEVEL_INFO, " |                                                                              |\r\n");
    wm_log_raw(WM_LOG_LEVEL_INFO, " ================================================================================\r\n\r\n");

    wm_log_raw(WM_LOG_LEVEL_INFO, " =========================================================\r\n");
    wm_log_raw(WM_LOG_LEVEL_INFO, " | Tech Support                                          |\r\n");
    wm_log_raw(WM_LOG_LEVEL_INFO, " |                                                       |\r\n");
    wm_log_raw(WM_LOG_LEVEL_INFO, " | SDK Introduction Onsite Page:                         |\r\n");
    wm_log_raw(WM_LOG_LEVEL_INFO, " |     https://doc.winnermicro.net                       |\r\n");
    wm_log_raw(WM_LOG_LEVEL_INFO, " |                                                       |\r\n");
    wm_log_raw(WM_LOG_LEVEL_INFO, " | SDK Packag Download Page:                             |\r\n");
    wm_log_raw(WM_LOG_LEVEL_INFO, " |     https://doc.winnermicro.net/download/version      |\r\n");
    wm_log_raw(WM_LOG_LEVEL_INFO, " =========================================================\r\n\r\n");

    return 0;
}
