#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

extern void benchmark_dhry_main(void);

static void benchmark_test_task(void *parameters)
{
    benchmark_dhry_main();

    vTaskDelete(NULL);
}

int main(void)
{
    xTaskCreate(benchmark_test_task, "benchmark", 512, NULL, configMAX_PRIORITIES - 1, NULL);

    return 0;
}
