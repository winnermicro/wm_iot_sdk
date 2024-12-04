#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

extern void benchmark_coremark_main(void);

static void benchmark_test_task(void *parameters)
{
    printf("coremark will take a while to execute, please be patient and wait for completion...\n");

    benchmark_coremark_main();

    vTaskDelete(NULL);
}

int main(void)
{
    xTaskCreate(benchmark_test_task, "benchmark", 1024, NULL, configMAX_PRIORITIES - 1, NULL);

    return 0;
}
