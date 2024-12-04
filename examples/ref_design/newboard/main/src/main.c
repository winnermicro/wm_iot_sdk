/**
 * @file main.c
 *
 * @brief The reference design project running on new board
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

#define LOG_TAG "NewBoard-Reference"
#include "wm_log.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

/*Below include statement can be removed if user no need to get heap, partition table info in this project*/
#include "wm_heap.h"
#include "wm_partition_table.h"

static void test_task(void *parameters)
{
    while (1) {
        wm_log_info("heap free size=%d(bytes)", wm_heap_get_free_size());

        vTaskDelay(pdMS_TO_TICKS(1000));
    }

    vTaskDelete(NULL);
}

int main(void)
{
    wm_partition_table_print();

    xTaskCreate(test_task, "test", 512, NULL, 5, NULL);

    return 0;
}
