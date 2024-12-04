#include "wm_error.h"
#include "wm_drv_touch_sensor.h"
#include "wm_drv_touch_button.h"

#define LOG_TAG "touch"
#include "wm_log.h"

static void touch_button_callback(wm_touch_sensor_num_t touch_num, wm_drv_touch_button_state_t state, void *priv)
{
    if (WM_DRV_TOUCH_BUTTON_PRESSED == state)
        wm_log_info("key%d pressed", touch_num);
    else if (WM_DRV_TOUCH_BUTTON_LONG_PRESSED == state) {
        wm_log_info("key%d long pressed", touch_num);
    }
}

int main(void)
{
    wm_device_t *touch_button;

    wm_drv_touch_sensor_init("touch_sensor");
    touch_button = wm_drv_touch_button_init("touch_button");
    wm_drv_touch_button_register_callback(touch_button, touch_button_callback, NULL);
    wm_drv_touch_button_set_longpress(touch_button, 3000);
    wm_drv_touch_button_start(touch_button);

    return 0;
}
