/**
 * @file main.c
 *
 * @brief nvs example
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
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

#include "wm_types.h"
#include "wm_error.h"
#include "wm_nvs.h"

#define LOG_TAG "exam_nvs"
#include "wm_log.h"

#define EXAMPLE_NVS_GROUP "user"

/*Read and write string ending in 0*/
void example_nvs_string(wm_nvs_handle_t handle)
{
    char *str = "123456";
    char buf[strlen(str) + 1];
    size_t len = sizeof(buf);

    wm_nvs_set_str(handle, "key_str", str);
    wm_nvs_get_str(handle, "key_str", buf, &len);

    wm_log_info("Read str:%s\n", buf);
}

/*Read and write binary raw data*/
void example_nvs_binary(wm_nvs_handle_t handle)
{
    unsigned char raw_data[5] = { 0x01, 0x02, 0x03, 0x04, 0x05 };
    unsigned char buf[5];
    size_t blob_len = sizeof(buf);

    wm_nvs_set_binary(handle, "key_bin", raw_data, sizeof(raw_data));
    wm_nvs_get_binary(handle, "key_bin", buf, &blob_len);

    wm_log_dump_info("binary", buf, blob_len);
}

/*Read and write float*/
void example_nvs_float(wm_nvs_handle_t handle)
{
    double value = 1.2345;
    double v;

    wm_nvs_set_float(handle, "key_float", value);
    wm_nvs_get_float(handle, "key_float", &v);

    wm_log_info("Read float=%lf\n", v);
}

/*Read and write integer*/
void example_nvs_integer(wm_nvs_handle_t handle)
{
    int8_t v8w = -120;
    int8_t v8r;

    wm_nvs_set_i8(handle, "key_I8", v8w);
    wm_nvs_get_i8(handle, "key_I8", &v8r);

    wm_log_info("Read I8=%d\n", v8r);
}

/*Traversal items using iterators*/
void example_nvs_iterator(wm_nvs_handle_t handle)
{
    wm_nvs_iterator_t iterator;
    unsigned char buf[32];
    size_t len;
    wm_nvs_entry_t info;

    wm_log_info("Start iterator.");

    if (wm_nvs_entry_find(WM_NVS_DEF_PARTITION, EXAMPLE_NVS_GROUP, WM_NVS_TYPE_ANY, &iterator) != WM_ERR_SUCCESS) {
        return;
    }

    while (iterator) {
        wm_nvs_entry_info(iterator, &info);
        len = sizeof(buf);
        if (wm_nvs_entry_data(iterator, buf, &len) == WM_ERR_SUCCESS) {
            wm_log_info("%s,len=%d", info.key, (int)len);
        }
        wm_nvs_entry_next(&iterator);
    }
    wm_nvs_release_iterator(iterator);
}

int main(void)
{
    int err;
    wm_nvs_handle_t handle;

    wm_log_info("NVS example succeeds in running.");

    /* The 'nvs' partition initialized before, so do not initialize it any more */
    /* open a nvs write read handle */

    err = wm_nvs_open(WM_NVS_DEF_PARTITION, EXAMPLE_NVS_GROUP, WM_NVS_OP_READ_WRITE, &handle);

    wm_log_info("open %s, group=%s err=%d", WM_NVS_DEF_PARTITION, EXAMPLE_NVS_GROUP, err);

    if (err != WM_ERR_SUCCESS) {
        return WM_ERR_FAILED;
    }

    example_nvs_string(handle);
    example_nvs_binary(handle);
    example_nvs_float(handle);
    example_nvs_integer(handle);
    example_nvs_iterator(handle);

    /* close the opend handle */
    wm_nvs_close(handle);

    wm_log_info("NVS example end.");

    return WM_ERR_SUCCESS;
}
