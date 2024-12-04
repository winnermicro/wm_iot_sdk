/**
 * @file wm_cjson_demo.c
 *
 * @brief WM_CJSON Module
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
#include <stdlib.h>
#include "wm_types.h"
#include "wm_error.h"

#include "cJSON/cJSON.h"

#define LOG_TAG "cjson_example"
#include "wm_log.h"

int main(void)
{
    char *str = NULL;

    wm_log_info("start cJSON demo...");

    /** Create a JSON data object */
    cJSON *cjson_root = cJSON_CreateObject();

    /** Add a string type JSON data */
    cJSON_AddStringToObject(cjson_root, "Name", "Jack");

    /** Add an integer type JSON data */
    cJSON_AddNumberToObject(cjson_root, "Age", 28);

    /** Add a floating point JSON data */
    cJSON_AddNumberToObject(cjson_root, "Weight", 68);

    /** Create a JSON data object and add it to cjson_root */
    cJSON *cjson_info = cJSON_CreateObject();
    cJSON_AddItemToObject(cjson_info, "Company", cJSON_CreateString("WinnerMicro"));
    cJSON_AddItemToObject(cjson_info, "Department", cJSON_CreateString("SoftwareDepartment"));
    cJSON_AddItemToObject(cjson_info, "Number", cJSON_CreateNumber(8));

    /** Add cjson_info to cjson_root */
    cJSON_AddItemToObject(cjson_root, "Information", cjson_info);

    /** Add an array type JSON data */
    cJSON *cjson_score = cJSON_CreateArray();
    cJSON_AddItemToArray(cjson_score, cJSON_CreateNumber(18));
    cJSON_AddItemToArray(cjson_score, cJSON_CreateNumber(20));
    cJSON_AddItemToArray(cjson_score, cJSON_CreateNumber(23));
    cJSON_AddItemToArray(cjson_score, cJSON_CreateNumber(25));

    /** Add cjson_root to cjson_root */
    cJSON_AddItemToObject(cjson_root, "Score", cjson_score);

    /** Add a JSON data of type ture */
    cJSON_AddTrueToObject(cjson_root, "IsMan");

    /** Add a boolean JSON data with a value of False */
    cJSON_AddNullToObject(cjson_root, "Other");

    /** Print all data of cjson_root */
    str = cJSON_Print(cjson_root);
    if (str != NULL) {
        wm_log_raw_info("%s\n", str);
        free(str);
        str = NULL;
    }

    cJSON_Delete(cjson_root);

    return WM_ERR_SUCCESS;
}
