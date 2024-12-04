/**
 * @file wm_w802_cfgs.h
 *
 * @brief Storage for differentiated hardware configurations specific to 
 * a particular SoC (System on Chip).
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

#ifndef __SOC_802_CFGS_H__
#define __SOC_802_CFGS_H__

/**
 * @defgroup WM_SOC_W802_CFG_TYPEs WM SOC Types
 * @brief WinnerMicro WM SOC Types
 */

/**
 * @addtogroup WM_SOC_W802_CFG_TYPEs
 * @{
 */

typedef enum {
    WM_GPIO_NUM_START = 0,
    WM_GPIO_NUM_0     = WM_GPIO_NUM_START, /*PA0*/
    WM_GPIO_NUM_1     = 1,                 /*PA1*/
    WM_GPIO_NUM_2     = 2,                 /*PA2*/
    WM_GPIO_NUM_3     = 3,                 /*PA3*/
    WM_GPIO_NUM_4     = 4,                 /*PA4*/
    WM_GPIO_NUM_7     = 7,                 /*PA7*/
    WM_GPIO_NUM_9     = 9,                 /*PA9*/
    WM_GPIO_NUM_10    = 10,                /*PA10*/
    WM_GPIO_NUM_12    = 12,                /*PA12*/
    WM_GPIO_NUM_14    = 14,                /*PA14*/
    WM_GPIO_NUM_16    = 16,                /*PB0*/
    WM_GPIO_NUM_17    = 17,                /*PB1*/
    WM_GPIO_NUM_18    = 18,                /*PB2*/
    WM_GPIO_NUM_19    = 19,                /*PB3*/
    WM_GPIO_NUM_20    = 20,                /*PB4*/
    WM_GPIO_NUM_21    = 21,                /*PB5*/
    WM_GPIO_NUM_22    = 22,                /*PB6*/
    WM_GPIO_NUM_23    = 23,                /*PB7*/
    WM_GPIO_NUM_24    = 24,                /*PB8*/
    WM_GPIO_NUM_25    = 25,                /*PB9*/
    WM_GPIO_NUM_26    = 26,                /*PB10*/
    WM_GPIO_NUM_27    = 27,                /*PB11*/
    WM_GPIO_NUM_28    = 28,                /*PB12*/
    WM_GPIO_NUM_29    = 29,                /*PB13*/
    WM_GPIO_NUM_30    = 30,                /*PB14*/
    WM_GPIO_NUM_31    = 31,                /*PB15*/
    WM_GPIO_NUM_32    = 32,                /*PB16*/
    WM_GPIO_NUM_33    = 33,                /*PB17*/
    WM_GPIO_NUM_34    = 34,                /*PB18*/
    WM_GPIO_NUM_35    = 35,                /*PB19*/
    WM_GPIO_NUM_36    = 36,                /*PB20*/
    WM_GPIO_NUM_37    = 37,                /*PB21*/
    WM_GPIO_NUM_38    = 38,                /*PB22*/
    WM_GPIO_NUM_40    = 40,                /*PB24*/
    WM_GPIO_NUM_41    = 41,                /*PB25*/
    WM_GPIO_NUM_42    = 42,                /*PB26*/
    WM_GPIO_NUM_45    = 45,                /*PB29*/
    WM_GPIO_NUM_MAX
} wm_gpio_num_t;

typedef enum {
    WM_TOUCH_SENSOR_NUM_1  = 1,
    WM_TOUCH_SENSOR_NUM_2  = 2,
    WM_TOUCH_SENSOR_NUM_3  = 3,
    WM_TOUCH_SENSOR_NUM_4  = 4,
    WM_TOUCH_SENSOR_NUM_5  = 5,
    WM_TOUCH_SENSOR_NUM_6  = 6,
    WM_TOUCH_SENSOR_NUM_7  = 7,
    WM_TOUCH_SENSOR_NUM_8  = 8,
    WM_TOUCH_SENSOR_NUM_9  = 9,
    WM_TOUCH_SENSOR_NUM_10 = 10,
    WM_TOUCH_SENSOR_NUM_11 = 11,
    WM_TOUCH_SENSOR_NUM_12 = 12,
    WM_TOUCH_SENSOR_NUM_15 = 15,
} wm_touch_sensor_num_t;

/**
 * @}
 */

#endif /* __SOC_802_CFGS_H__ */
