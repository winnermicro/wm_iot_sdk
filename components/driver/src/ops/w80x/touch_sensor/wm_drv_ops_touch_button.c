/**
 * @file wm_touch_button.c
 *
 * @brief TOUCH BUTTON Module
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

#include <string.h>
#include "wm_error.h"
#include "wm_reg_op.h"
#include "wm_drv_irq.h"
#include "wm_drv_gpio.h"
#include "wm_drv_touch_sensor.h"
#include "wm_drv_touch_button.h"
#include "wm_dt_hw.h"
#include "wm_osal.h"
#if CONFIG_COMPONENT_RF_ENABLED
#include "wm_rf.h"
#endif
#if CONFIG_COMPONENT_WIFI_ENABLED
#include "wm_wifi_drv.h"
#endif

#define LOG_TAG "touch_button"
#include "wm_log.h"

#if 1 //CONFIG_COMPONENT_WIFI_ENABLED /* || CONFIG_BT*/
#define FEATURE_TOUCH_BUTTON_INTERRUPT_ENABLE 0
#else
#define FEATURE_TOUCH_BUTTON_INTERRUPT_ENABLE 1
#endif

#define TOUCH_BUTTON_DEBUG 0

#if TOUCH_BUTTON_DEBUG
#define TOUCH_BUTTON_PRINTF wm_log_debug
#else
#define TOUCH_BUTTON_PRINTF(...)
#endif

#define TOUCH_BUTTON_RELEASE_MAX_CNT                2

#define TOUCH_BUTTON_NO_TOUCH_MAX                   0x760

#define TOUCH_BUTTON_TRIGGER_PERCENTAGE_BASE_VAL    (1000)

#define TOUCH_BUTTON_CALIBRATION_CAPTURE_TIME       (8)
#define TOUCH_BUTTON_CALIBRATION_NOTOUCH_CHECK_TIME (32)

#if FEATURE_TOUCH_BUTTON_INTERRUPT_ENABLE
#define TOUCH_BUTTON_PRESS_MAX_CNT         2
#define TOUCH_BUTTON_MAX_CALIBRATION_TIMES (20)
#else
#define TOUCH_BUTTON_PRESS_MAX_CNT         4
#define TOUCH_BUTTON_MAX_CALIBRATION_TIMES (32)
#endif

/* full range percentage */
#define TOUCH_BUTTON_FULL_LEVEL             (1000)

/* When all touchkeys are not pressed, enlarge touch loop detect time, unit:ms */
#define TOUCH_BUTTON_NEXT_DETECT_WAIT_TIME  (64)

/* touchkey average calculation max count */
#define TOUCH_BUTTON_BASE_AVERAGE_COUNT     (16)

/* Loop interval, unit:ms */
#define TOUCH_BUTTON_VALUE_READ_INTERVAL    (16)

#define TOUCH_BUTTON_SHORT_PRESS_DEFAULT_MS 500
#define TOUCH_BUTTON_LONG_PRESS_DEFAULT_MS  1000

/* touch button message id */
enum {
    TOUCH_BUTTON_BASE_CALC_MSG    = 0xFF,
    TOUCH_BUTTON_PRESS_DETECT_MSG = 0xFE,
    TOUCH_BUTTON_PRESS_START_MSG  = 0xFD,
    TOUCH_BUTTON_PRESS_STOP_MSG   = 0xFC
};

/* touch button calibration state */
enum {
    TOUCH_BUTTON_CALIBRATION_STATE_NONE    = 0x0,
    TOUCH_BUTTON_CALIBRATION_STATE_ONGOING = 0x1,
    TOUCH_BUTTON_CALIBRATION_STATE_DONE    = 0x2,
};

struct touch_button {
    wm_touch_sensor_num_t sensorno;
    uint32_t threshold;

    volatile int low_cnt;
    volatile int press_cnt;
    volatile uint8_t pressed;
    uint8_t long_pressed;
    uint32_t pressed_time; /* pressed time used to judge short_pressed or long_pressed */

#if FEATURE_TOUCH_BUTTON_INTERRUPT_ENABLE
    int max_val;
    int min_val;
    int min_isr_val;
#else
    int used_base; /* base used to set or clear 'pressed' */
    int new_base;  /* new base to update used_base */
    int accum_val; /* used to calculate used_base or new_base */
    int accum_cnt; /* used to calculate used_base or new_base */
#endif
};

struct touch_button_ctx {
    wm_device_t *touch_dev;

    wm_drv_touch_button_callback_t callback;
    void *callback_priv;

    wm_os_queue_t *queue;
#if FEATURE_TOUCH_BUTTON_INTERRUPT_ENABLE
    wm_os_timer_t *calibrate_timer;
#endif
    wm_os_task_t task;

    volatile uint32_t no_touch_cnt;

    uint32_t long_press_threshold_time;
    uint32_t short_press_threshold_time;

    volatile uint8_t calibration_state;

    uint8_t button_num;
    struct touch_button *buttons;
};

typedef struct {
    int (*register_callback)(wm_device_t *dev, wm_drv_touch_button_callback_t callback, void *priv);
    int (*set_longpress)(wm_device_t *dev, uint32_t threshold_time);
    int (*start)(wm_device_t *dev);
    int (*stop)(wm_device_t *dev);
    int (*init)(wm_device_t *dev);
    int (*deinit)(wm_device_t *dev);
} wm_drv_touch_button_ops_t;

#if CONFIG_COMPONENT_RF_ENABLED
static inline void touch_button_check_rf(void)
{
    uint32_t rfval = wm_rf_read(25);

    if (rfval & WM_BIT(3)) {
    } else {
        wm_rf_write((25 << 17) | rfval | WM_BIT(3));
    }
}
#endif

static inline void touch_button_do_callback(wm_drv_touch_button_callback_t callback, wm_touch_sensor_num_t touch_num,
                                            wm_drv_touch_button_state_t state, void *priv)
{
    if (callback) {
        callback(touch_num, state, priv);
    }
}

static void touch_button_task_exit(void *arg)
{
    uint8_t i;
    struct touch_button_ctx *button_ctx = (struct touch_button_ctx *)arg;

#if FEATURE_TOUCH_BUTTON_INTERRUPT_ENABLE
    wm_os_internal_timer_delete(button_ctx->calibrate_timer);
#endif

    for (i = 0; i < button_ctx->button_num; i++) {
        wm_drv_touch_sensor_stop(button_ctx->touch_dev, button_ctx->buttons[i].sensorno);
        wm_drv_touch_sensor_register_callback(button_ctx->touch_dev, button_ctx->buttons[i].sensorno, NULL, NULL);
    }

    /* maybe touch use button and others, so need deinit touch sensor by user */
    //wm_touch_deinit(&button_ctx->touch_dev);

    wm_os_internal_queue_delete(button_ctx->queue);

    wm_os_internal_free(button_ctx->buttons);

    wm_os_internal_free(button_ctx);
}

#if FEATURE_TOUCH_BUTTON_INTERRUPT_ENABLE
static void touch_button_drv_callback(wm_touch_sensor_num_t touch_num, void *priv)
{
    uint8_t i;
    int err;
    uint32_t ts_cnt                     = 0;
    struct touch_button_ctx *button_ctx = (struct touch_button_ctx *)priv;

    for (i = 0; i < button_ctx->button_num; i++) {
        if (touch_num == button_ctx->buttons[i].sensorno) {
            err = wm_drv_touch_sensor_read_countnum(button_ctx->touch_dev, button_ctx->buttons[i].sensorno, &ts_cnt);
            if (!err) {
                switch (button_ctx->calibration_state) {
                    case TOUCH_BUTTON_CALIBRATION_STATE_ONGOING:
                    {
                        TOUCH_BUTTON_PRINTF("t%d-%d", button_ctx->buttons[i].sensorno, ts_cnt);
                        if (button_ctx->buttons[i].min_isr_val > ts_cnt) {
                            button_ctx->buttons[i].min_isr_val = ts_cnt;
                        }
                        if (button_ctx->buttons[i].max_val < ts_cnt) {
                            button_ctx->buttons[i].max_val = ts_cnt;
                        }
                        break;
                    }
                    case TOUCH_BUTTON_CALIBRATION_STATE_DONE:
                    {
                        if ((ts_cnt < button_ctx->buttons[i].max_val) || (button_ctx->buttons[i].max_val == 0)) {
                            button_ctx->calibration_state = TOUCH_BUTTON_CALIBRATION_STATE_NONE;
                            wm_os_internal_timer_stop(button_ctx->calibrate_timer);
                            wm_os_internal_queue_send(button_ctx->queue, (void *)TOUCH_BUTTON_BASE_CALC_MSG);
                            TOUCH_BUTTON_PRINTF("recalibrating");
                            return;
                        }

                        if (ts_cnt >= button_ctx->buttons[i].max_val *
                                          (TOUCH_BUTTON_TRIGGER_PERCENTAGE_BASE_VAL + button_ctx->buttons[i].threshold) /
                                          TOUCH_BUTTON_TRIGGER_PERCENTAGE_BASE_VAL) {
                            TOUCH_BUTTON_PRINTF("t%d:%u,%d, %d", button_ctx->buttons[i].sensorno, ts_cnt,
                                                button_ctx->buttons[i].max_val, wm_os_internal_get_time_ms());
                            button_ctx->no_touch_cnt = 0;
                            button_ctx->buttons[i].press_cnt++;
                            if (button_ctx->buttons[i].press_cnt > TOUCH_BUTTON_PRESS_MAX_CNT) {
                                if (button_ctx->buttons[i].pressed == 0) {
                                    button_ctx->buttons[i].pressed      = 1;
                                    button_ctx->buttons[i].pressed_time = wm_os_internal_get_time_ms();
                                    TOUCH_BUTTON_PRINTF("key%d press", button_ctx->buttons[i].sensorno);
                                    touch_button_do_callback(button_ctx->callback, button_ctx->buttons[i].sensorno,
                                                             WM_DRV_TOUCH_BUTTON_PRESS, button_ctx->callback_priv);
                                }
                                if (button_ctx->buttons[i].long_pressed == 0) {
                                    if (button_ctx->buttons[i].pressed_time &&
                                        ((wm_os_internal_get_time_ms() - button_ctx->buttons[i].pressed_time) >
                                         button_ctx->long_press_threshold_time)) {
                                        button_ctx->buttons[i].long_pressed = 1;
                                        TOUCH_BUTTON_PRINTF("key%d long pressed", button_ctx->buttons[i].sensorno);
                                        touch_button_do_callback(button_ctx->callback, button_ctx->buttons[i].sensorno,
                                                                 WM_DRV_TOUCH_BUTTON_LONG_PRESSED, button_ctx->callback_priv);
                                    }
                                }
                            }
                            button_ctx->buttons[i].low_cnt = 0;
                        }
                        break;
                    }
                    default:
                    {
                        TOUCH_BUTTON_PRINTF("t%d %u", button_ctx->buttons[i].sensorno, ts_cnt);
                        break;
                    }
                }
            }

            break;
        }
    }
}

static void touch_button_calibration_timer(void *ptmr, void *parg)
{
    uint8_t i       = 0;
    uint32_t regval = 0;
    uint32_t ts_cnt = 0;
    int err;
    struct touch_button_ctx *button_ctx = (struct touch_button_ctx *)parg;

#if CONFIG_COMPONENT_RF_ENABLED
    touch_button_check_rf();
#endif

    if (button_ctx->calibration_state == TOUCH_BUTTON_CALIBRATION_STATE_ONGOING) {
        for (i = 0; i < button_ctx->button_num; i++) {
            regval = wm_drv_irq_save();
            err    = wm_drv_touch_sensor_read_countnum(button_ctx->touch_dev, button_ctx->buttons[i].sensorno, &ts_cnt);
            //TOUCH_BUTTON_PRINTF("i:%d, val:%d, %x", button_ctx->buttons[i].sensorno, ts_cnt, wm_reg_read32(HR_TC_CONFIG+button_ctx->buttons[i].sensorno*4));
            if (!err && ts_cnt) {
                if (button_ctx->buttons[i].max_val < ts_cnt) {
                    button_ctx->buttons[i].max_val = ts_cnt;
                }

                if (button_ctx->buttons[i].min_val > ts_cnt) {
                    button_ctx->buttons[i].min_val = ts_cnt;
                }
            }
            wm_drv_irq_restore(regval);
        }

        button_ctx->no_touch_cnt++;
        //TOUCH_BUTTON_PRINTF("nocnt:%d,%d", no_touch_cnt, TOUCH_BUTTON_MAX_CALIBRATION_TIMES);
        if (button_ctx->no_touch_cnt > TOUCH_BUTTON_MAX_CALIBRATION_TIMES) {
            wm_os_internal_timer_stop(button_ctx->calibrate_timer);
            wm_os_internal_queue_send(button_ctx->queue, (void *)TOUCH_BUTTON_PRESS_DETECT_MSG);
        } else {
            wm_os_internal_timer_change_ms(button_ctx->calibrate_timer, TOUCH_BUTTON_CALIBRATION_CAPTURE_TIME);
        }
    } else if (button_ctx->calibration_state == TOUCH_BUTTON_CALIBRATION_STATE_DONE) {
        button_ctx->no_touch_cnt++;
        if (button_ctx->no_touch_cnt > TOUCH_BUTTON_NO_TOUCH_MAX) {
            button_ctx->no_touch_cnt      = 0;
            button_ctx->calibration_state = TOUCH_BUTTON_CALIBRATION_STATE_NONE;
            wm_os_internal_timer_stop(button_ctx->calibrate_timer);
            wm_os_internal_queue_send(button_ctx->queue, (void *)TOUCH_BUTTON_BASE_CALC_MSG);
            return;
        }

        for (i = 0; i < button_ctx->button_num; i++) {
            button_ctx->buttons[i].low_cnt++;
            if (button_ctx->buttons[i].low_cnt >= TOUCH_BUTTON_RELEASE_MAX_CNT) {
                button_ctx->buttons[i].low_cnt = 0;
                if (button_ctx->buttons[i].pressed) {
                    if (button_ctx->buttons[i].pressed_time &&
                        ((wm_os_internal_get_time_ms() - button_ctx->buttons[i].pressed_time) <=
                         button_ctx->short_press_threshold_time)) {
                        TOUCH_BUTTON_PRINTF("key%d short pressed", button_ctx->buttons[i].sensorno);
                        touch_button_do_callback(button_ctx->callback, button_ctx->buttons[i].sensorno,
                                                 WM_DRV_TOUCH_BUTTON_PRESSED, button_ctx->callback_priv);
                    }
                    touch_button_do_callback(button_ctx->callback, button_ctx->buttons[i].sensorno, WM_DRV_TOUCH_BUTTON_RELEASE,
                                             button_ctx->callback_priv);
                    button_ctx->buttons[i].pressed = 0;
                }
                button_ctx->buttons[i].long_pressed = 0;
                button_ctx->buttons[i].press_cnt    = 0;
            }
        }

        wm_os_internal_timer_change_ms(button_ctx->calibrate_timer, TOUCH_BUTTON_CALIBRATION_NOTOUCH_CHECK_TIME);
    }
}
#endif

static void touch_button_calibrate(struct touch_button_ctx *button_ctx)
{
    uint8_t i, j;
#if FEATURE_TOUCH_BUTTON_INTERRUPT_ENABLE
    uint32_t irq_state;
#endif

    for (j = 0; j < 2; j++) {
        for (i = 0; i < button_ctx->button_num; i++) {
            wm_drv_touch_sensor_stop(button_ctx->touch_dev, button_ctx->buttons[i].sensorno);

#if FEATURE_TOUCH_BUTTON_INTERRUPT_ENABLE
            irq_state = wm_drv_irq_save();
#endif
            button_ctx->buttons[i].press_cnt    = 0;
            button_ctx->buttons[i].pressed      = 0;
            button_ctx->buttons[i].long_pressed = 0;
            //button_ctx->buttons[i].pressed_time = 0;
            //button_ctx->buttons[i].low_cnt      = 0;
#if FEATURE_TOUCH_BUTTON_INTERRUPT_ENABLE
            button_ctx->buttons[i].max_val     = 0;
            button_ctx->buttons[i].min_val     = 0x3FFF;
            button_ctx->buttons[i].min_isr_val = 0x3FFF;
#else
            button_ctx->buttons[i].accum_cnt = 0;
            button_ctx->buttons[i].accum_val = 0;
            button_ctx->buttons[i].used_base = 0;
            button_ctx->buttons[i].new_base  = 0;
#endif
#if FEATURE_TOUCH_BUTTON_INTERRUPT_ENABLE
            wm_drv_irq_restore(irq_state);
            wm_drv_touch_sensor_set_threshold(button_ctx->touch_dev, button_ctx->buttons[i].sensorno, 1);
#endif

            wm_drv_touch_sensor_start(button_ctx->touch_dev, button_ctx->buttons[i].sensorno);
        }

        wm_os_internal_time_delay_ms(TOUCH_BUTTON_VALUE_READ_INTERVAL);
    }

#if FEATURE_TOUCH_BUTTON_INTERRUPT_ENABLE
    button_ctx->no_touch_cnt      = 0;
    button_ctx->calibration_state = TOUCH_BUTTON_CALIBRATION_STATE_ONGOING;

    wm_os_internal_timer_change_ms(button_ctx->calibrate_timer, TOUCH_BUTTON_CALIBRATION_CAPTURE_TIME);
#endif
}

#if !FEATURE_TOUCH_BUTTON_INTERRUPT_ENABLE
static inline void touch_button_moving_average(int ts_cnt, int slide_cnt, int *count, int *avg_val, int *accum_val)
{
    if (count && avg_val && accum_val && slide_cnt) {
        if (*count < slide_cnt) {
            *avg_val = ((*count) * (*avg_val) + ts_cnt) / ((*count) + 1);
            (*count)++;
            *accum_val = (*avg_val) * (*count);
        } else {
            *accum_val = *accum_val - (*accum_val / slide_cnt) + ts_cnt;
            *avg_val   = *accum_val / slide_cnt;
        }
    }
}

static void touch_button_poll(struct touch_button_ctx *button_ctx)
{
    uint8_t i               = 0;
    uint32_t ts_current_val = 0;
    int err;
#if CONFIG_COMPONENT_RF_ENABLED
    uint32_t rfval;
#endif
#if CONFIG_COMPONENT_WIFI_ENABLED
    uint32_t time_ms = 0;
    int ret          = 0;

    /* When touch irq happened, if wifi tx is running, first close rf tx.
       if you have any other rf chips, when touch irq happened, you'd better
       close your rf chip firstly, then check touch press. */
    time_ms = wm_os_internal_get_time_ms();
    ret     = wm_wifi_drv_forbid_tx(16);
    if (ret) {
        for (i = 0; i < button_ctx->button_num; i++) {
            button_ctx->buttons[i].press_cnt = 0;
            button_ctx->buttons[i].accum_cnt = 0;
            button_ctx->buttons[i].low_cnt   = 0;
        }
        wm_wifi_drv_permit_tx();
        TOUCH_BUTTON_PRINTF("time out to return");
        return;
    }
#endif

    /* TO DO  Suspend other rf chip's tx */

#if CONFIG_COMPONENT_WIFI_ENABLED
    /* wait for next touch detection */
    time_ms = wm_os_internal_get_time_ms() - time_ms;
    if (time_ms) {
        wm_os_internal_time_delay_ms(TOUCH_BUTTON_VALUE_READ_INTERVAL * 2 - time_ms % TOUCH_BUTTON_VALUE_READ_INTERVAL);
    } else {
        wm_os_internal_time_delay_ms(TOUCH_BUTTON_VALUE_READ_INTERVAL);
    }
#else
    wm_os_internal_time_delay_ms(TOUCH_BUTTON_VALUE_READ_INTERVAL);
#endif

    /* loop all used touch-sensor to get touch counter */
#if CONFIG_COMPONENT_RF_ENABLED
    rfval = wm_rf_read(25);
    if (rfval & WM_BIT(3)) {
#endif
        for (i = 0; i < button_ctx->button_num; i++) {
            err = wm_drv_touch_sensor_read_countnum(button_ctx->touch_dev, button_ctx->buttons[i].sensorno, &ts_current_val);
            if (!err && ts_current_val >= button_ctx->buttons[i].used_base *
                                              (button_ctx->buttons[i].threshold + TOUCH_BUTTON_FULL_LEVEL) /
                                              TOUCH_BUTTON_FULL_LEVEL) {
                TOUCH_BUTTON_PRINTF("[%d]=>t%d,%d,%d,%d", wm_os_internal_get_time(), button_ctx->buttons[i].sensorno,
                                    button_ctx->buttons[i].used_base,
                                    button_ctx->buttons[i].used_base *
                                        (button_ctx->buttons[i].threshold + TOUCH_BUTTON_FULL_LEVEL) / TOUCH_BUTTON_FULL_LEVEL,
                                    ts_current_val);
                button_ctx->buttons[i].press_cnt++;
                button_ctx->buttons[i].accum_cnt = 0;
                if (button_ctx->buttons[i].press_cnt >= TOUCH_BUTTON_PRESS_MAX_CNT) {
                    if (0 == button_ctx->buttons[i].pressed) {
                        button_ctx->buttons[i].pressed      = 1;
                        button_ctx->buttons[i].long_pressed = 0;
                        button_ctx->buttons[i].pressed_time = wm_os_internal_get_time_ms();
                        TOUCH_BUTTON_PRINTF("key%d press", button_ctx->buttons[i].sensorno);
                        touch_button_do_callback(button_ctx->callback, button_ctx->buttons[i].sensorno,
                                                 WM_DRV_TOUCH_BUTTON_PRESS, button_ctx->callback_priv);
                    }
                    if (button_ctx->buttons[i].long_pressed == 0) {
                        if (button_ctx->buttons[i].pressed_time &&
                            ((wm_os_internal_get_time_ms() - button_ctx->buttons[i].pressed_time) >
                             button_ctx->long_press_threshold_time)) {
                            button_ctx->buttons[i].long_pressed = 1;
                            TOUCH_BUTTON_PRINTF("key%d long pressed", button_ctx->buttons[i].sensorno);
                            touch_button_do_callback(button_ctx->callback, button_ctx->buttons[i].sensorno,
                                                     WM_DRV_TOUCH_BUTTON_LONG_PRESSED, button_ctx->callback_priv);
                        }
                    }
                }
                button_ctx->buttons[i].low_cnt = 0;
            } else {
                if ((0 == button_ctx->buttons[i].pressed) && (0 == button_ctx->buttons[i].press_cnt)) {
                    /* confirm if recalibrate touch sensor */
                    touch_button_moving_average(ts_current_val, TOUCH_BUTTON_BASE_AVERAGE_COUNT,
                                                &button_ctx->buttons[i].accum_cnt, &button_ctx->buttons[i].new_base,
                                                &button_ctx->buttons[i].accum_val);
                    if (button_ctx->buttons[i].accum_cnt == TOUCH_BUTTON_BASE_AVERAGE_COUNT) {
                        button_ctx->buttons[i].used_base = button_ctx->buttons[i].new_base;
                    }
                }

                button_ctx->buttons[i].press_cnt = 0;
                button_ctx->buttons[i].low_cnt++;
                if (button_ctx->buttons[i].low_cnt >= TOUCH_BUTTON_RELEASE_MAX_CNT) {
                    if (button_ctx->buttons[i].pressed) {
                        if (button_ctx->buttons[i].pressed_time &&
                            ((wm_os_internal_get_time_ms() - button_ctx->buttons[i].pressed_time) <=
                             button_ctx->short_press_threshold_time)) {
                            TOUCH_BUTTON_PRINTF("key%d short pressed", button_ctx->buttons[i].sensorno);
                            touch_button_do_callback(button_ctx->callback, button_ctx->buttons[i].sensorno,
                                                     WM_DRV_TOUCH_BUTTON_PRESSED, button_ctx->callback_priv);
                        }
                        touch_button_do_callback(button_ctx->callback, button_ctx->buttons[i].sensorno,
                                                 WM_DRV_TOUCH_BUTTON_RELEASE, button_ctx->callback_priv);
                        button_ctx->buttons[i].long_pressed = 0;
                        button_ctx->buttons[i].pressed      = 0;
                    }
                }
            }
        }
#if CONFIG_COMPONENT_RF_ENABLED
    } else {
        TOUCH_BUTTON_PRINTF("cw25 bit3 changed");
        wm_rf_write((25 << 17) | rfval | WM_BIT(3));
    }
#endif

#if CONFIG_COMPONENT_WIFI_ENABLED
    /* open rf again after touch press detection */
    wm_wifi_drv_permit_tx();
#endif

    /* TO DO... restore other's rf chip's tx */
}
#endif

static void touch_button_task(void *arg)
{
    int err = -1;
    void *msg;
    uint8_t i = 0;
    uint32_t irq_state;
#if !FEATURE_TOUCH_BUTTON_INTERRUPT_ENABLE
    int j;
    uint32_t ts_cnt          = 0;
    uint8_t recal_flag       = 0;
    uint8_t key_released_cnt = 0;
#endif
    struct touch_button_ctx *button_ctx = (struct touch_button_ctx *)arg;

    for (;;) {
        err = wm_os_internal_queue_receive(button_ctx->queue, (void **)&msg, WM_OS_WAIT_TIME_MAX);
        if (err == 0) {
            switch ((int)msg) {
#if FEATURE_TOUCH_BUTTON_INTERRUPT_ENABLE
                case TOUCH_BUTTON_PRESS_START_MSG:
                {
                    touch_button_calibrate(button_ctx);
                    break;
                }
#endif
                case TOUCH_BUTTON_PRESS_STOP_MSG:
                {
#if FEATURE_TOUCH_BUTTON_INTERRUPT_ENABLE
                    wm_os_internal_timer_stop(button_ctx->calibrate_timer);
#endif
                    for (i = 0; i < button_ctx->button_num; i++)
                        wm_drv_touch_sensor_stop(button_ctx->touch_dev, button_ctx->buttons[i].sensorno);
                    //wm_os_internal_queue_flush(button_ctx->queue);
                    break;
                }
                case TOUCH_BUTTON_BASE_CALC_MSG:
                {
#if !FEATURE_TOUCH_BUTTON_INTERRUPT_ENABLE
#if CONFIG_COMPONENT_WIFI_ENABLED
                    /* When touch scan, if wifi tx is running, first suspend rf tx.
                       if you have any other rf chips, when touch scan, you'd
                       better suspend your rf chip firstly, then check touch press. */
                    wm_wifi_drv_forbid_tx(1000);
#endif
#endif
                    touch_button_calibrate(button_ctx);
#if !FEATURE_TOUCH_BUTTON_INTERRUPT_ENABLE
                    for (j = 0; j < TOUCH_BUTTON_MAX_CALIBRATION_TIMES; j++) {
                        for (i = 0; i < button_ctx->button_num; i++) {
                            irq_state = wm_drv_irq_save();
#if CONFIG_COMPONENT_RF_ENABLED
                            touch_button_check_rf();
#endif
                            err = wm_drv_touch_sensor_read_countnum(button_ctx->touch_dev, button_ctx->buttons[i].sensorno,
                                                                    &ts_cnt);
                            wm_drv_irq_restore(irq_state);
                            if (!err && ts_cnt) {
                                touch_button_moving_average(ts_cnt, TOUCH_BUTTON_BASE_AVERAGE_COUNT,
                                                            &button_ctx->buttons[i].accum_cnt,
                                                            &button_ctx->buttons[i].used_base,
                                                            &button_ctx->buttons[i].accum_val);
                            }
                        }
                        wm_os_internal_time_delay_ms(TOUCH_BUTTON_VALUE_READ_INTERVAL);
                    }

#if CONFIG_COMPONENT_WIFI_ENABLED
                    wm_wifi_drv_permit_tx();
#endif
                    recal_flag = 0;
                    for (i = 0; i < button_ctx->button_num; i++) {
                        if (0 == button_ctx->buttons[i].used_base) {
                            recal_flag = 1;
                            break;
                        }
                        button_ctx->buttons[i].accum_cnt = 0;
                        button_ctx->buttons[i].accum_val = 0;
                        TOUCH_BUTTON_PRINTF("t%d,%d,%d", button_ctx->buttons[i].sensorno, button_ctx->buttons[i].used_base,
                                            button_ctx->buttons[i].used_base * button_ctx->buttons[i].threshold /
                                                TOUCH_BUTTON_FULL_LEVEL);
                    }

                    if (1 == recal_flag) {
                        wm_os_internal_queue_send(button_ctx->queue, (void *)TOUCH_BUTTON_BASE_CALC_MSG);
                    } else {
                        wm_os_internal_queue_send(button_ctx->queue, (void *)TOUCH_BUTTON_PRESS_DETECT_MSG);
                    }
#endif
                    break;
                }
                case TOUCH_BUTTON_PRESS_DETECT_MSG:
                {
#if FEATURE_TOUCH_BUTTON_INTERRUPT_ENABLE
                    //wm_drv_irq_disable(18);
                    for (i = 0; i < button_ctx->button_num; i++) {
                        irq_state = wm_drv_irq_save();
                        if (button_ctx->buttons[i].max_val < button_ctx->buttons[i].min_isr_val) {
                            button_ctx->buttons[i].min_isr_val = button_ctx->buttons[i].max_val;
                        }
                        wm_drv_irq_restore(irq_state);

                        TOUCH_BUTTON_PRINTF("t%d:max:%d,isr:%d,min:%d", button_ctx->buttons[i].sensorno,
                                            button_ctx->buttons[i].max_val, button_ctx->buttons[i].min_isr_val,
                                            button_ctx->buttons[i].min_val);
                        wm_drv_touch_sensor_set_threshold(button_ctx->touch_dev, button_ctx->buttons[i].sensorno,
                                                          button_ctx->buttons[i].max_val * button_ctx->buttons[i].threshold /
                                                              TOUCH_BUTTON_TRIGGER_PERCENTAGE_BASE_VAL);
                        TOUCH_BUTTON_PRINTF("t%d:th %d", button_ctx->buttons[i].sensorno,
                                            button_ctx->buttons[i].max_val * button_ctx->buttons[i].threshold /
                                                TOUCH_BUTTON_TRIGGER_PERCENTAGE_BASE_VAL);
                    }
                    button_ctx->no_touch_cnt      = 0;
                    button_ctx->calibration_state = TOUCH_BUTTON_CALIBRATION_STATE_DONE;
                    //wm_drv_irq_enable(18);

                    if (button_ctx->calibrate_timer) {
                        wm_os_internal_timer_change_ms(button_ctx->calibrate_timer,
                                                       TOUCH_BUTTON_CALIBRATION_NOTOUCH_CHECK_TIME);
                    }
#else
                    touch_button_poll(button_ctx);
                    key_released_cnt = 0;
                    for (i = 0; i < button_ctx->button_num; i++) {
                        if ((0 == button_ctx->buttons[i].pressed) &&
                            (TOUCH_BUTTON_BASE_AVERAGE_COUNT == button_ctx->buttons[i].accum_cnt)) {
                            key_released_cnt++;
                        }
                    }

                    if (button_ctx->button_num == key_released_cnt)
                        wm_os_internal_time_delay_ms(TOUCH_BUTTON_NEXT_DETECT_WAIT_TIME);
                    wm_os_internal_queue_send(button_ctx->queue, (void *)TOUCH_BUTTON_PRESS_DETECT_MSG);
#endif
                    break;
                }
                default:
                {
                    break;
                }
            }
        }
    }
}

static int wm_drv_ops_touch_button_reg_callback(wm_device_t *dev, wm_drv_touch_button_callback_t callback, void *priv)
{
    struct touch_button_ctx *button_ctx = (struct touch_button_ctx *)dev->drv;
    int ret                             = WM_ERR_NO_INITED;

    if (button_ctx) {
        button_ctx->callback      = callback;
        button_ctx->callback_priv = priv;
        ret                       = WM_ERR_SUCCESS;
    }

    return ret;
}

static int wm_drv_ops_touch_button_set_longpress(wm_device_t *dev, uint32_t threshold_time)
{
    struct touch_button_ctx *button_ctx = (struct touch_button_ctx *)dev->drv;
    int ret                             = WM_ERR_NO_INITED;

    if (button_ctx) {
        button_ctx->long_press_threshold_time = threshold_time;
        ret                                   = WM_ERR_SUCCESS;
    }

    return ret;
}

static int wm_drv_ops_touch_button_start(wm_device_t *dev)
{
    struct touch_button_ctx *button_ctx = (struct touch_button_ctx *)dev->drv;
    int ret                             = WM_ERR_NO_INITED;

    if (button_ctx) {
#if FEATURE_TOUCH_BUTTON_INTERRUPT_ENABLE
        ret = wm_os_internal_queue_forever_send(button_ctx->queue, (void *)TOUCH_BUTTON_PRESS_START_MSG);
#else
        ret = wm_os_internal_queue_forever_send(button_ctx->queue, (void *)TOUCH_BUTTON_BASE_CALC_MSG);
#endif
        ret = ret == WM_OS_STATUS_SUCCESS ? WM_ERR_SUCCESS : WM_ERR_FAILED;
    }

    return ret;
}

static int wm_drv_ops_touch_button_stop(wm_device_t *dev)
{
    struct touch_button_ctx *button_ctx = (struct touch_button_ctx *)dev->drv;
    int ret                             = WM_ERR_NO_INITED;

    if (button_ctx) {
        ret = wm_os_internal_queue_forever_send(button_ctx->queue, (void *)TOUCH_BUTTON_PRESS_STOP_MSG);
        ret = ret == WM_OS_STATUS_SUCCESS ? WM_ERR_SUCCESS : WM_ERR_FAILED;
    }

    return ret;
}

static int wm_drv_ops_touch_button_init(wm_device_t *dev)
{
    uint8_t i;
    int ret = WM_ERR_SUCCESS;
    wm_device_t *touch_dev;
    struct touch_button_ctx *button_ctx;
    wm_dt_hw_touch_button_t *hw = (wm_dt_hw_touch_button_t *)dev->hw;

    if (dev->drv || (WM_DEV_ST_INITED == dev->state))
        return WM_ERR_ALREADY_INITED;

    if (!hw)
        return WM_ERR_FAILED;

    touch_dev = wm_dt_get_device_by_name(hw->touch_sensor_device_name);
    if (!touch_dev)
        return WM_ERR_NOT_FOUND;

    if (WM_DEV_ST_INITED != touch_dev->state) {
        wm_log_error("touch_sensor not init");
        return WM_ERR_NO_INITED;
    }

    button_ctx = wm_os_internal_malloc(sizeof(*button_ctx));
    if (!button_ctx)
        return WM_ERR_NO_MEM;

    memset(button_ctx, 0, sizeof(*button_ctx));

    button_ctx->touch_dev                  = touch_dev;
    button_ctx->short_press_threshold_time = TOUCH_BUTTON_SHORT_PRESS_DEFAULT_MS;
    button_ctx->long_press_threshold_time  = TOUCH_BUTTON_LONG_PRESS_DEFAULT_MS;
    button_ctx->button_num                 = hw->touch_button_cfg_count;

    button_ctx->buttons = wm_os_internal_malloc(sizeof(struct touch_button) * hw->touch_button_cfg_count);
    if (!button_ctx->buttons) {
        wm_os_internal_free(button_ctx);
        button_ctx = NULL;
        return WM_ERR_NO_MEM;
    }

    memset(button_ctx->buttons, 0, sizeof(struct touch_button) * hw->touch_button_cfg_count);

    for (i = 0; i < hw->touch_button_cfg_count; i++) {
        button_ctx->buttons[i].sensorno  = hw->touch_button_cfg[i].key_num;
        button_ctx->buttons[i].threshold = hw->touch_button_cfg[i].threshold;
    }

    // pinmux
    for (i = 0; i < hw->pin_cfg_count; i++) {
        ret = wm_drv_gpio_iomux_func_sel(hw->pin_cfg[i].pin_num, hw->pin_cfg[i].pin_mux);
        ret |= wm_drv_gpio_set_dir(hw->pin_cfg[i].pin_num, hw->pin_cfg[i].pin_dir);
        ret |= wm_drv_gpio_set_pullmode(hw->pin_cfg[i].pin_num, hw->pin_cfg[i].pin_pupd);
        if (ret)
            break;
    }

    if (!ret)
        ret = wm_os_internal_queue_create(&button_ctx->queue, 32);
#if FEATURE_TOUCH_BUTTON_INTERRUPT_ENABLE
    if (!ret)
        ret = wm_os_internal_timer_create_ms(&button_ctx->calibrate_timer, touch_button_calibration_timer, button_ctx, 100,
                                             false, "touch_button");
#endif
    if (!ret)
        ret = wm_os_internal_task_create(&button_ctx->task, "touch_button", touch_button_task, button_ctx, 1024,
                                         wm_os_internal_task_get_max_priority() - 1, 0);

    if (ret) {
        ret = WM_ERR_NO_MEM;
        goto err;
    }

    for (i = 0; i < hw->touch_button_cfg_count; i++) {
        ret = wm_drv_touch_sensor_set_threshold(touch_dev, hw->touch_button_cfg[i].key_num, hw->touch_button_cfg[i].threshold);
#if FEATURE_TOUCH_BUTTON_INTERRUPT_ENABLE
        ret |= wm_drv_touch_sensor_register_callback(touch_dev, hw->touch_button_cfg[i].key_num, touch_button_drv_callback,
                                                     button_ctx);
#endif
        if (ret)
            break;
    }

    if (ret)
        goto err;

    dev->drv   = button_ctx;
    dev->state = WM_DEV_ST_INITED;

    return WM_ERR_SUCCESS;

err:
    dev->state = WM_DEV_ST_UNINIT;
    dev->drv   = NULL;
    if (button_ctx->queue)
        wm_os_internal_queue_delete(button_ctx->queue);
#if FEATURE_TOUCH_BUTTON_INTERRUPT_ENABLE
    if (button_ctx->calibrate_timer)
        wm_os_internal_timer_delete(button_ctx->calibrate_timer);
#endif
    if (button_ctx->task)
        wm_os_internal_task_del(button_ctx->task);

    wm_os_internal_free(button_ctx->buttons);
    wm_os_internal_free(button_ctx);
    return ret;
}

static int wm_drv_ops_touch_button_deinit(wm_device_t *dev)
{
    int ret;
    struct touch_button_ctx *button_ctx = (struct touch_button_ctx *)dev->drv;

    if (!dev || !dev->drv || !dev->state)
        return WM_ERR_NO_INITED;

    ret = wm_os_internal_task_del_ex(button_ctx->task, touch_button_task_exit, button_ctx);

    if (!ret) {
        dev->drv   = NULL;
        dev->state = WM_DEV_ST_UNINIT;
    }

    return ret == WM_OS_STATUS_SUCCESS ? WM_ERR_SUCCESS : WM_ERR_FAILED;
}

const wm_drv_touch_button_ops_t wm_drv_touch_button_ops = {
    .register_callback = wm_drv_ops_touch_button_reg_callback,
    .set_longpress     = wm_drv_ops_touch_button_set_longpress,
    .start             = wm_drv_ops_touch_button_start,
    .stop              = wm_drv_ops_touch_button_stop,
    .init              = wm_drv_ops_touch_button_init,
    .deinit            = wm_drv_ops_touch_button_deinit,
};
