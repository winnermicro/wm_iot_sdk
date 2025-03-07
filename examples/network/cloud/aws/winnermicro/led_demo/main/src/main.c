/**
 * @file main.c
 *
 * @brief The AWS demo project
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
#include <stdio.h>
#include <string.h>
#include "wmsdk_config.h"

#define LOG_TAG "aws"
#include "wm_log.h"

int main(void)
{
    wm_log_raw_info("\n ===================================================\n");
    wm_log_raw_info(" |       Steps to connect to AWS                   |\n");
    wm_log_raw_info(" |                                                 |\n");
    wm_log_raw_info(" |  1. Print 'help' to gain overview of commands   |\n");
    wm_log_raw_info(" |  2. Configure device to station                 |\n");
    wm_log_raw_info(" |  3. Setup WiFi connection                       |\n");
    wm_log_raw_info(" |  4. Run aws to test AWS function                |\n");
    wm_log_raw_info(" |                                                 |\n");
    wm_log_raw_info(" ===================================================\n\n");

    return 0;
}
