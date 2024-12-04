#include "wm_error.h"
#include "wm_drv_touch_sensor.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define LOG_TAG "touch"
#include "wm_log.h"

static wm_device_t *touch_sensor_dev         = NULL;
static wm_touch_sensor_num_t touch_button[3] = { WM_TOUCH_SENSOR_NUM_3, WM_TOUCH_SENSOR_NUM_4, WM_TOUCH_SENSOR_NUM_5 };

/*
  Read count number sensed at all available touch sensor.
 Print out count number in a loop on a serial monitor.
 */
static void touch_sensor_poll_task(void *pvParameter)
{
    uint32_t countnum;

    /* Wait touch sensor init done */
    vTaskDelay(100 / portTICK_PERIOD_MS);
    wm_log_raw(WM_LOG_LEVEL_INFO, "Touch Sensor read, the output format is: \nTouch sensor num:[raw data]\n\n");

    while (1) {
        for (uint8_t i = 0; i < sizeof(touch_button) / sizeof(touch_button[0]); i++) {
            wm_drv_touch_sensor_read_countnum(touch_sensor_dev, touch_button[i], &countnum); // read raw data.
            wm_log_raw(WM_LOG_LEVEL_INFO, "T%d: [%4u] ", touch_button[i], countnum);
        }
        wm_log_raw(WM_LOG_LEVEL_INFO, "\n");
        vTaskDelay(200 / portTICK_PERIOD_MS);
    }
}

int main(void)
{
    touch_sensor_dev = wm_drv_touch_sensor_init("touch_sensor");

    if (touch_sensor_dev) {
        for (uint8_t i = 0; i < sizeof(touch_button) / sizeof(touch_button[0]); i++) {
            wm_drv_touch_sensor_start(touch_sensor_dev, touch_button[i]);
        }

        /* Start task to read count number */
        xTaskCreate(&touch_sensor_poll_task, "touch_poll", 4096, NULL, 5, NULL);
    }

    return 0;
}
