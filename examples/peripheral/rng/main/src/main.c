#define LOG_TAG "rng_example"
#include "wm_log.h"

#include "wmsdk_config.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "wm_drv_rng.h"

static void rng_hard_demo(void)
{
    uint8_t *out = NULL;
    uint16_t i;
    wm_device_t *dev;

    /*initialize rng*/
    dev = wm_drv_rng_init("rng");
    if (dev == NULL) {
        wm_log_error("init fail.");
    }

    out = malloc(1000);

    /* generate random */
    memset(out, 0, 1000);
    wm_drv_rng_read_bytes(dev, out, 5 * sizeof(uint32_t));
    wm_log_raw(WM_LOG_LEVEL_INFO, "RNG out:");
    for (i = 0; i < 5; i++) {
        wm_log_raw(WM_LOG_LEVEL_INFO, "0x%08X ", ((uint32_t *)out)[i]);
    }
    wm_log_raw(WM_LOG_LEVEL_INFO, "\n");

    memset(out, 0, 1000);
    wm_drv_rng_read_bytes(dev, out, 10 * sizeof(uint32_t));
    wm_log_raw(WM_LOG_LEVEL_INFO, "RNG out:");
    for (i = 0; i < 10; i++) {
        wm_log_raw(WM_LOG_LEVEL_INFO, "0x%08X ", ((uint32_t *)out)[i]);
    }
    wm_log_raw(WM_LOG_LEVEL_INFO, "\n");

    wm_drv_rng_deinit(dev);

    free(out);
}

int main(void)
{
    wm_log_info("rng demo.");

    rng_hard_demo();

    while (1) {
        wm_log_info("rng demo.");
        vTaskDelay(pdMS_TO_TICKS(1000));
    }

    return 0;
}
