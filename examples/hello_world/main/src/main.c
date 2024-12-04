#define LOG_TAG "test"
#include "wm_log.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static void test_task(void *parameters)
{
    while (1) {
        wm_log_info("hello world.");

        vTaskDelay(pdMS_TO_TICKS(1000));
    }

    vTaskDelete(NULL);
}

int main(void)
{
    xTaskCreate(test_task, "test", 512, NULL, 5, NULL);

    return 0;
}
