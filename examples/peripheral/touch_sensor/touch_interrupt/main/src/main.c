#include "wm_error.h"
#include "wm_drv_touch_sensor.h"

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"

#define LOG_TAG "touch"
#include "wm_log.h"

static QueueHandle_t que_touch               = NULL;
static wm_device_t *touch_sensor_dev         = NULL;
static wm_touch_sensor_num_t touch_button[3] = { WM_TOUCH_SENSOR_NUM_3, WM_TOUCH_SENSOR_NUM_4, WM_TOUCH_SENSOR_NUM_5 };
static uint32_t touch_threshold[3]           = { 45, 50, 55 };

static void touch_sensor_callback(wm_touch_sensor_num_t touch_num, void *priv)
{
    BaseType_t task_awoken = pdFALSE;

    xQueueSendFromISR(que_touch, &touch_num, &task_awoken);
    if (task_awoken == pdTRUE) {
        portYIELD_FROM_ISR(task_awoken);
    }
}

static void touch_sensor_irq_task(void *pvParameter)
{
    BaseType_t ret;
    wm_touch_sensor_num_t touch_num;

    while (1) {
        ret = xQueueReceive(que_touch, &touch_num, (TickType_t)portMAX_DELAY);
        if (ret != pdTRUE) {
            continue;
        }

        wm_log_raw(WM_LOG_LEVEL_INFO, "touch sensor %d activated\n", touch_num);
    }
}

int main(void)
{
    touch_sensor_dev = wm_drv_touch_sensor_init("touch_sensor");

    if (touch_sensor_dev) {
        que_touch = xQueueCreate(sizeof(touch_button) / sizeof(touch_button[0]), sizeof(wm_touch_sensor_num_t));
        for (uint8_t i = 0; i < sizeof(touch_button) / sizeof(touch_button[0]); i++) {
            wm_drv_touch_sensor_register_callback(touch_sensor_dev, touch_button[i], touch_sensor_callback, NULL);
            wm_drv_touch_sensor_set_threshold(touch_sensor_dev, touch_button[i], touch_threshold[i]);
            wm_drv_touch_sensor_start(touch_sensor_dev, touch_button[i]);
        }

        /* Start task to read count number */
        xTaskCreate(&touch_sensor_irq_task, "touch_irq", 4096, NULL, 5, NULL);
    }

    return 0;
}
