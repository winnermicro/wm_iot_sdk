/**
 * @file llm_cli_cmd.c
 *
 * @brief The LLM CLI Command
 *
 */

/**
 *  Copyright 2022-2025 Beijing WinnerMicroelectronics Co.,Ltd.
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

#include "wm_dt.h"
#include "wm_dt_hw.h"
#include "wm_soc_cfgs.h"
#include <stdint.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include "wmsdk_config.h"
#include "wm_error.h"
#include "wm_cli.h"
#include "wm_nm_api.h"

/* the define for LLM, user need to apply their own API Keys from LLM offical website*/
/*deepseek llm define*/
#define WM_LLM_DS_URL    "https://api.deepseek.com/v1/chat/completions"
#define WM_LLM_DS_KEY    "Bearer sk-90a31215c9b9443cba1840e3461742f7"
#define WM_LLM_DS_MODEL  "deepseek-chat"

/*doubao llm define*/
#define WM_LLM_ARK_URL   "https://ark.cn-beijing.volces.com/api/v3/chat/completions"
#define WM_LLM_ARK_KEY   "Bearer 8bccdf3b-3012-4418-8e54-93af90cdedad"
#define WM_LLM_ARK_MODEL "doubao-lite-128k-240828"

extern int wm_llm_post(char *url, char *model, char *apikey, char *role, char *content);

static void llm_cmd_usage(const char *basename)
{
    wm_cli_printf("\r\n"
                  "Usage\r\n"
                  "  %s [options]\r\n"
                  "\r\n"
                  "Options:\r\n"
                  "  ds <contents>   send contents to deepseek llm, max len < %d Bytes\r\n"
                  "  db <contents>   send contents to doubao llm, max len < %d Bytes\r\n"
                  "  help            print help and exit\r\n",
                  basename, CONFIG_CLI_LINE_SIZE - 8, CONFIG_CLI_LINE_SIZE - 8);
}

static void cmd_llm(int argc, char *argv[])
{
    int cont_len = 0;
    char *pcnt   = NULL;

    if (argc < 3) {
        llm_cmd_usage(argv[0]);
    }

    //treat all parameters except first 2, as the content
    for (int i = 2; i < argc; i++) {
        cont_len += strlen(argv[i]) + 1; // +1 for '\0' or space
    }

    pcnt = (char *)malloc(cont_len);
    if (!pcnt) {
        return;
    }
    memset(pcnt, 0, cont_len);

    for (int i = 2; i < argc; i++) {
        strcat(pcnt, argv[i]);
        if (i != argc - 1) {
            strcat(pcnt, " ");
        }
    }

    if (!strcmp(argv[0], "llm")) {
        if (wm_nm_get_wifi_station_state() != WM_NM_WIFI_STA_GOT_IP) {
            wm_cli_printf("wifi is disconnect, please do wifi connect first");
            goto EXIT;
        }

        if (pcnt != NULL && strlen(pcnt) > 0) {
            if (!strcmp(argv[1], "ds")) {
                wm_llm_post(WM_LLM_DS_URL, WM_LLM_DS_MODEL, WM_LLM_DS_KEY, "user", pcnt);
                goto EXIT;
            } else if (!strcmp(argv[1], "db")) {
                wm_llm_post(WM_LLM_ARK_URL, WM_LLM_ARK_MODEL, WM_LLM_ARK_KEY, "user", pcnt);
                goto EXIT;
            }
        }
    }

    llm_cmd_usage(argv[0]);

EXIT:
    if (pcnt) {
        free(pcnt);
    }
}

WM_CLI_CMD_DEFINE(llm, cmd_llm, llm message send, LLM<model><contents> --send contents to specific LLM);
