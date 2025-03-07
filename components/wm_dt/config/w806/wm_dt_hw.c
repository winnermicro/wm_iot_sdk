/**
 * @file wm_dt_hw.c
 *
 * @brief Device Hardware Information Module
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

#include "wm_dt_hw.h"
#include "wm_dt_op.h"

#include "wm_soc_cfgs.h"
#include "wm_irq.h"

struct wm_drv_ops_structure;

typedef struct wm_drv_ops_structure wm_drv_uart_ops_t;
extern const wm_drv_uart_ops_t wm_drv_uart_ops;

typedef struct wm_drv_ops_structure wm_drv_timer_ops_t;
extern const wm_drv_timer_ops_t wm_drv_timer_ops;

typedef struct wm_drv_ops_structure wm_drv_pwm_ops_t;
extern const wm_drv_pwm_ops_t wm_drv_pwm_ops;

typedef struct wm_drv_ops_structure wm_drv_i2s_ops_t;
extern const wm_drv_i2s_ops_t wm_drv_i2s_ops;

typedef struct wm_drv_ops_structure wm_drv_seg_lcd_ops_t;
extern const wm_drv_seg_lcd_ops_t wm_drv_seg_lcd_ops;

typedef struct wm_drv_ops_structure wm_drv_sdh_sdmmc_ops_t;
extern const wm_drv_sdh_sdmmc_ops_t wm_drv_sdh_sdmmc_ops;

typedef struct wm_drv_ops_structure wm_drv_sdh_spi_ops_t;
extern const wm_drv_sdh_spi_ops_t wm_drv_sdh_spi_ops;

typedef struct wm_drv_ops_structure wm_drv_tft_lcd_ops_t;
extern const wm_drv_tft_lcd_ops_t wm_drv_tft_lcd_nv3041a_ops;

typedef struct wm_drv_ops_structure wm_drv_tft_lcd_ops_t;
extern const wm_drv_tft_lcd_ops_t wm_drv_tft_lcd_st7735_ops;

typedef struct wm_drv_ops_structure wm_drv_tft_lcd_ops_t;
extern const wm_drv_tft_lcd_ops_t wm_drv_tft_lcd_gz035_ops;

typedef struct wm_drv_ops_structure wm_drv_rtc_ops_t;
extern const wm_drv_rtc_ops_t wm_drv_rtc_ops;

typedef struct wm_drv_ops_structure wm_drv_pmu_ops_t;
extern const wm_drv_pmu_ops_t wm_drv_pmu_ops;

typedef struct wm_drv_ops_structure wm_drv_adc_ops_t;
extern const wm_drv_adc_ops_t wm_drv_adc_ops;

typedef struct wm_drv_ops_structure wm_drv_gpio_ops_t;
extern const wm_drv_gpio_ops_t wm_drv_gpio_ops;

typedef struct wm_drv_ops_structure wm_drv_wdt_ops_t;
extern const wm_drv_wdt_ops_t wm_drv_wdt_ops;

typedef struct wm_drv_ops_structure wm_drv_dma_ops_t;
extern const wm_drv_dma_ops_t wm_drv_dma_ops;

typedef struct wm_drv_ops_structure wm_drv_i2c_ops_t;
extern const wm_drv_i2c_ops_t wm_drv_i2c_ops;

typedef struct wm_drv_ops_structure wm_drv_eeprom_ops_t;
extern const wm_drv_eeprom_ops_t wm_drv_eeprom_nv24c0x_ops;

typedef struct wm_drv_ops_structure wm_drv_psram_ops_t;
extern const wm_drv_psram_ops_t wm_drv_psram_ops;

typedef struct wm_drv_ops_structure wm_drv_spim_ops_t;
extern const wm_drv_spim_ops_t wm_drv_spim_ops;

typedef struct wm_drv_ops_structure wm_drv_clock_ops_t;
extern const wm_drv_clock_ops_t wm_drv_rcc_ops;

typedef struct wm_drv_ops_structure wm_drv_flash_ops_t;
extern const wm_drv_flash_ops_t wm_drv_internal_flash_ops;
extern const wm_drv_flash_ops_t wm_drv_external_flash_ops;

typedef struct wm_drv_ops_structure wm_drv_touch_sensor_ops_t;
extern const wm_drv_touch_sensor_ops_t wm_drv_touch_sensor_ops;

typedef struct wm_drv_ops_structure wm_drv_touch_button_ops_t;
extern const wm_drv_touch_sensor_ops_t wm_drv_touch_button_ops;

typedef struct wm_drv_ops_structure wm_drv_crc_ops_t;
extern const wm_drv_crc_ops_t wm_drv_crc_ops;

typedef struct wm_drv_ops_structure wm_drv_cryp_ops_t;
extern const wm_drv_cryp_ops_t wm_drv_crypto_ops;

typedef struct wm_drv_ops_structure wm_drv_hash_ops_t;
extern const wm_drv_hash_ops_t wm_drv_hash_ops;

typedef struct wm_drv_ops_structure wm_drv_rng_ops_t;
extern const wm_drv_rng_ops_t wm_drv_rng_ops;

typedef struct wm_drv_ops_structure wm_drv_rsa_ops_t;
extern const wm_drv_rsa_ops_t wm_drv_rsa_ops;

typedef struct wm_drv_ops_structure wm_drv_codec_i2s_ops_t;
extern const wm_drv_codec_i2s_ops_t wm_drv_codec_i2s_es8374_ops;

typedef struct wm_drv_ops_structure wm_drv_sdio_slave_ops_t;
extern const wm_drv_sdio_slave_ops_t wm_drv_sdio_slave_ops;

#if CONFIG_COMPONENT_DRIVER_UART_ENABLED
const static wm_dt_hw_pin_cfg_t dt_hw_uart0_pin[] = {
    { .pin_num = WM_GPIO_NUM_35, .pin_mux = WM_GPIO_IOMUX_FUN1 },
    { .pin_num = WM_GPIO_NUM_36, .pin_mux = WM_GPIO_IOMUX_FUN1 },
};

const static wm_dt_hw_uart_t dt_hw_uart0 = {
    .init_cfg        = { .init_level = 0, .init_priority = 50 },
    .reg_base        = 0x40010600,
    .irq_cfg         = { .irq_num = WM_IRQ_UART0, .irq_priority = 0 },
    .uart_cfg        = { .baudrate  = WM_UART_BAUDRATE_B115200,
                        .parity    = WM_UART_PARITY_NONE,
                        .stop_bits = WM_UART_STOP_BIT_1,
                        .data_bits = WM_UART_DATA_BIT_8,
                        .flow_ctrl = WM_UART_FLOW_CTRL_DISABLE },
    .pin_cfg_count   = sizeof(dt_hw_uart0_pin) / sizeof(dt_hw_uart0_pin[0]),
    .pin_cfg         = (wm_dt_hw_pin_cfg_t *)dt_hw_uart0_pin,
    .rcc_device_name = "rcc",
};

const static wm_dt_hw_pin_cfg_t dt_hw_uart1_pin[] = {
    { .pin_num = WM_GPIO_NUM_22, .pin_mux = WM_GPIO_IOMUX_FUN1 },
    { .pin_num = WM_GPIO_NUM_23, .pin_mux = WM_GPIO_IOMUX_FUN1 },
};

const static wm_dt_hw_uart_t dt_hw_uart1 = {
    .init_cfg        = { .init_level = 0, .init_priority = 50 },
    .reg_base        = 0x40010800,
    .irq_cfg         = { .irq_num = WM_IRQ_UART1, .irq_priority = 0 },
    .uart_cfg        = { .baudrate  = WM_UART_BAUDRATE_B115200,
                        .parity    = WM_UART_PARITY_NONE,
                        .stop_bits = WM_UART_STOP_BIT_1,
                        .data_bits = WM_UART_DATA_BIT_8,
                        .flow_ctrl = WM_UART_FLOW_CTRL_DISABLE },
    .pin_cfg_count   = sizeof(dt_hw_uart1_pin) / sizeof(dt_hw_uart1_pin[0]),
    .pin_cfg         = (wm_dt_hw_pin_cfg_t *)dt_hw_uart1_pin,
    .rcc_device_name = "rcc",
};

const static wm_dt_hw_pin_cfg_t dt_hw_uart2_pin[] = {
    { .pin_num = WM_GPIO_NUM_18, .pin_mux = WM_GPIO_IOMUX_FUN3 },
    { .pin_num = WM_GPIO_NUM_19, .pin_mux = WM_GPIO_IOMUX_FUN3 },
    { .pin_num = WM_GPIO_NUM_20, .pin_mux = WM_GPIO_IOMUX_FUN2 },
    { .pin_num = WM_GPIO_NUM_21, .pin_mux = WM_GPIO_IOMUX_FUN2 },
};

const static wm_dt_hw_uart_t dt_hw_uart2 = {
    .init_cfg        = { .init_level = 0, .init_priority = 50 },
    .reg_base        = 0x40010A00,
    .uart_cfg        = { .baudrate  = WM_UART_BAUDRATE_B115200,
                        .parity    = WM_UART_PARITY_NONE,
                        .stop_bits = WM_UART_STOP_BIT_1,
                        .data_bits = WM_UART_DATA_BIT_8,
                        .flow_ctrl = WM_UART_FLOW_CTRL_DISABLE },
    .pin_cfg_count   = sizeof(dt_hw_uart2_pin) / sizeof(dt_hw_uart2_pin[0]),
    .pin_cfg         = (wm_dt_hw_pin_cfg_t *)dt_hw_uart2_pin,
    .irq_cfg         = { .irq_num = WM_IRQ_UART24, .irq_priority = 0 },
    .rcc_device_name = "rcc",
};

const static wm_dt_hw_pin_cfg_t dt_hw_uart3_pin[] = {
    { .pin_num = WM_GPIO_NUM_5, .pin_mux = WM_GPIO_IOMUX_FUN1 },
    { .pin_num = WM_GPIO_NUM_6, .pin_mux = WM_GPIO_IOMUX_FUN1 },
};

const static wm_dt_hw_uart_t dt_hw_uart3 = {
    .init_cfg        = { .init_level = 0, .init_priority = 50 },
    .reg_base        = 0x40010c00,
    .irq_cfg         = { .irq_num = WM_IRQ_UART24, .irq_priority = 0 },
    .uart_cfg        = { .baudrate  = WM_UART_BAUDRATE_B115200,
                        .parity    = WM_UART_PARITY_NONE,
                        .stop_bits = WM_UART_STOP_BIT_1,
                        .data_bits = WM_UART_DATA_BIT_8,
                        .flow_ctrl = WM_UART_FLOW_CTRL_DISABLE },
    .pin_cfg_count   = sizeof(dt_hw_uart3_pin) / sizeof(dt_hw_uart3_pin[0]),
    .pin_cfg         = (wm_dt_hw_pin_cfg_t *)dt_hw_uart3_pin,
    .dma_device_name = "dma",
    .rcc_device_name = "rcc",
};

const static wm_dt_hw_pin_cfg_t dt_hw_uart4_pin[] = {
    { .pin_num = WM_GPIO_NUM_8, .pin_mux = WM_GPIO_IOMUX_FUN2 },
    { .pin_num = WM_GPIO_NUM_9, .pin_mux = WM_GPIO_IOMUX_FUN2 },
};

const static wm_dt_hw_uart_t dt_hw_uart4 = {
    .init_cfg        = { .init_level = 0, .init_priority = 50 },
    .reg_base        = 0x40010e00,
    .irq_cfg         = { .irq_num = WM_IRQ_UART24, .irq_priority = 0 },
    .uart_cfg        = { .baudrate  = WM_UART_BAUDRATE_B115200,
                        .parity    = WM_UART_PARITY_NONE,
                        .stop_bits = WM_UART_STOP_BIT_1,
                        .data_bits = WM_UART_DATA_BIT_8,
                        .flow_ctrl = WM_UART_FLOW_CTRL_DISABLE },
    .pin_cfg_count   = sizeof(dt_hw_uart4_pin) / sizeof(dt_hw_uart4_pin[0]),
    .pin_cfg         = (wm_dt_hw_pin_cfg_t *)dt_hw_uart4_pin,
    .dma_device_name = "dma",
    .rcc_device_name = "rcc",
};

const static wm_dt_hw_pin_cfg_t dt_hw_uart5_pin[] = {
    { .pin_num = WM_GPIO_NUM_33, .pin_mux = WM_GPIO_IOMUX_FUN1 },
    { .pin_num = WM_GPIO_NUM_34, .pin_mux = WM_GPIO_IOMUX_FUN1 },
};

const static wm_dt_hw_uart_t dt_hw_uart5 = {
    .init_cfg        = { .init_level = 0, .init_priority = 50 },
    .reg_base = 0x40011000,
    .uart_cfg ={
        .baudrate  = WM_UART_BAUDRATE_B115200,
                        .parity    = WM_UART_PARITY_NONE,
                        .stop_bits = WM_UART_STOP_BIT_1,
                        .data_bits = WM_UART_DATA_BIT_8,
                        .flow_ctrl = WM_UART_FLOW_CTRL_DISABLE,
    },
    .pin_cfg_count = sizeof(dt_hw_uart5_pin) / sizeof(dt_hw_uart5_pin[0]),
    .pin_cfg = (wm_dt_hw_pin_cfg_t*)dt_hw_uart5_pin,
    .irq_cfg = {
        .irq_num = WM_IRQ_UART24,
        .irq_priority = 0
    },
    .rcc_device_name = "rcc",
};
#endif /* CONFIG_COMPONENT_DRIVER_UART_ENABLED */

#if CONFIG_COMPONENT_DRIVER_TIMER_ENABLED
const static wm_dt_hw_timer_t dt_hw_timer0 = {
    .init_cfg = { .init_level = 0,         .init_priority = 50 },
    .reg_base = 0x40011800,
    .irq_cfg  = { .irq_num = WM_IRQ_TIMER, .irq_priority = 0   },
};

const static wm_dt_hw_timer_t dt_hw_timer1 = {
    .init_cfg = { .init_level = 0,         .init_priority = 50 },
    .reg_base = 0x40011800,
    .irq_cfg  = { .irq_num = WM_IRQ_TIMER, .irq_priority = 0   },
};

const static wm_dt_hw_timer_t dt_hw_timer2 = {
    .init_cfg = { .init_level = 0,         .init_priority = 50 },
    .reg_base = 0x40011800,
    .irq_cfg  = { .irq_num = WM_IRQ_TIMER, .irq_priority = 0   },
};

const static wm_dt_hw_timer_t dt_hw_timer3 = {
    .init_cfg = { .init_level = 0,         .init_priority = 50 },
    .reg_base = 0x40011800,
    .irq_cfg  = { .irq_num = WM_IRQ_TIMER, .irq_priority = 0   },
};

const static wm_dt_hw_timer_t dt_hw_timer4 = {
    .init_cfg = { .init_level = 0,         .init_priority = 50 },
    .reg_base = 0x40011800,
    .irq_cfg  = { .irq_num = WM_IRQ_TIMER, .irq_priority = 0   },
};

const static wm_dt_hw_timer_t dt_hw_timer5 = {
    .init_cfg = { .init_level = 0,         .init_priority = 50 },
    .reg_base = 0x40011800,
    .irq_cfg  = { .irq_num = WM_IRQ_TIMER, .irq_priority = 0   },
};
#endif /* CONFIG_COMPONENT_DRIVER_TIMER_ENABLED */

#if CONFIG_COMPONENT_DRIVER_PWM_ENABLED
const static wm_dt_hw_pin_cfg_t dt_hw_pwm_pin[] = {
    { .pin_num = WM_GPIO_NUM_28, .pin_mux = WM_GPIO_IOMUX_FUN2 }, // PWM channel0
    { .pin_num = WM_GPIO_NUM_29, .pin_mux = WM_GPIO_IOMUX_FUN2 }, // PWM channel1
    { .pin_num = WM_GPIO_NUM_30, .pin_mux = WM_GPIO_IOMUX_FUN2 }, // PWM channel2
    { .pin_num = WM_GPIO_NUM_31, .pin_mux = WM_GPIO_IOMUX_FUN2 }, // PWM channel3
    { .pin_num = WM_GPIO_NUM_32, .pin_mux = WM_GPIO_IOMUX_FUN2 }, // PWM channel4
    { .pin_num = WM_GPIO_NUM_24, .pin_mux = WM_GPIO_IOMUX_FUN3 }, // PWM Brake Input
};

const static wm_dt_hw_pwm_t dt_hw_pwm = {
    .init_cfg        = { .init_level = 0, .init_priority = 0 },
    .reg_base        = 0x40011E00,
    .irq_cfg         = { .irq_num = 22,   .irq_priority = 0  },
    .dma_device_name = "dma",
    .pin_cfg_count   = sizeof(dt_hw_pwm_pin) / sizeof(dt_hw_pwm_pin[0]),
    .pin_cfg         = (wm_dt_hw_pin_cfg_t *)dt_hw_pwm_pin,
};
#endif

#if CONFIG_COMPONENT_DRIVER_I2S_ENABLED
const static wm_dt_hw_pin_cfg_t dt_hw_i2s_pin[] = {
    { .pin_num = WM_GPIO_NUM_7,  .pin_mux = WM_GPIO_IOMUX_FUN3 }, //EVB: MCLK
    { .pin_num = WM_GPIO_NUM_8,  .pin_mux = WM_GPIO_IOMUX_FUN4 }, //EVB: BCLK
    { .pin_num = WM_GPIO_NUM_9,  .pin_mux = WM_GPIO_IOMUX_FUN4 }, //EVB: LRCLK
    { .pin_num = WM_GPIO_NUM_10, .pin_mux = WM_GPIO_IOMUX_FUN4 }, //EVB: DO
    { .pin_num = WM_GPIO_NUM_11, .pin_mux = WM_GPIO_IOMUX_FUN4 }, //EVB: DI
};

const static wm_dt_hw_i2s_t dt_hw_i2s = {
    .init_cfg        = { .init_level = 0,         .init_priority = 0  },
    .reg_base        = 0x40012000,
    .irq_cfg         = { .irq_num = WM_IRQ_I2S,   .irq_priority = 0   },
    .dma_device_name = "dma",
    .rcc_device_name = "rcc",
    .pin_cfg_count   = sizeof(dt_hw_i2s_pin) / sizeof(dt_hw_i2s_pin[0]),
    .pin_cfg         = (wm_dt_hw_pin_cfg_t *)dt_hw_i2s_pin,
    .i2s_cfg         = { .extal_clock_en = false, .mclk_hz = 80000000 },
};
#endif

#if CONFIG_COMPONENT_DRIVER_SEG_LCD_ENABLED
const static wm_dt_hw_pin_cfg_t dt_hw_seg_lcd_pin_cfg[] = {
    { .pin_num = WM_GPIO_NUM_41, .pin_mux = WM_GPIO_IOMUX_FUN6 }, /**< COM00 */
    { .pin_num = WM_GPIO_NUM_37, .pin_mux = WM_GPIO_IOMUX_FUN6 }, /**< COM01 */
    { .pin_num = WM_GPIO_NUM_38, .pin_mux = WM_GPIO_IOMUX_FUN6 }, /**< COM02 */
    { .pin_num = WM_GPIO_NUM_43, .pin_mux = WM_GPIO_IOMUX_FUN6 }, /**< COM03 */

    { .pin_num = WM_GPIO_NUM_42, .pin_mux = WM_GPIO_IOMUX_FUN6 }, /**< SEG01 */
    { .pin_num = WM_GPIO_NUM_40, .pin_mux = WM_GPIO_IOMUX_FUN6 }, /**< SEG02 */
    { .pin_num = WM_GPIO_NUM_7,  .pin_mux = WM_GPIO_IOMUX_FUN6 }, /**< SEG03 */
    { .pin_num = WM_GPIO_NUM_8,  .pin_mux = WM_GPIO_IOMUX_FUN6 }, /**< SEG04 */
    { .pin_num = WM_GPIO_NUM_9,  .pin_mux = WM_GPIO_IOMUX_FUN6 }, /**< SEG05 */
    { .pin_num = WM_GPIO_NUM_10, .pin_mux = WM_GPIO_IOMUX_FUN6 }, /**< SEG06 */
    { .pin_num = WM_GPIO_NUM_11, .pin_mux = WM_GPIO_IOMUX_FUN6 }, /**< SEG07 */
    { .pin_num = WM_GPIO_NUM_12, .pin_mux = WM_GPIO_IOMUX_FUN6 }, /**< SEG08 */
    { .pin_num = WM_GPIO_NUM_13, .pin_mux = WM_GPIO_IOMUX_FUN6 }, /**< SEG09 */
    { .pin_num = WM_GPIO_NUM_14, .pin_mux = WM_GPIO_IOMUX_FUN6 }, /**< SEG10 */
    { .pin_num = WM_GPIO_NUM_15, .pin_mux = WM_GPIO_IOMUX_FUN6 }, /**< SEG11 */
    { .pin_num = WM_GPIO_NUM_16, .pin_mux = WM_GPIO_IOMUX_FUN6 }, /**< SEG12 */
    { .pin_num = WM_GPIO_NUM_17, .pin_mux = WM_GPIO_IOMUX_FUN6 }, /**< SEG13 */
    { .pin_num = WM_GPIO_NUM_18, .pin_mux = WM_GPIO_IOMUX_FUN6 }, /**< SEG14 */
    { .pin_num = WM_GPIO_NUM_19, .pin_mux = WM_GPIO_IOMUX_FUN6 }, /**< SEG15 */
    { .pin_num = WM_GPIO_NUM_20, .pin_mux = WM_GPIO_IOMUX_FUN6 }, /**< SEG16 */
    { .pin_num = WM_GPIO_NUM_21, .pin_mux = WM_GPIO_IOMUX_FUN6 }, /**< SEG17 */
    { .pin_num = WM_GPIO_NUM_22, .pin_mux = WM_GPIO_IOMUX_FUN6 }, /**< SEG18 */
    { .pin_num = WM_GPIO_NUM_23, .pin_mux = WM_GPIO_IOMUX_FUN6 }, /**< SEG19 */
    { .pin_num = WM_GPIO_NUM_24, .pin_mux = WM_GPIO_IOMUX_FUN6 }, /**< SEG20 */
    { .pin_num = WM_GPIO_NUM_25, .pin_mux = WM_GPIO_IOMUX_FUN6 }, /**< SEG21 */
    { .pin_num = WM_GPIO_NUM_26, .pin_mux = WM_GPIO_IOMUX_FUN6 }, /**< SEG22 */
    { .pin_num = WM_GPIO_NUM_27, .pin_mux = WM_GPIO_IOMUX_FUN6 }, /**< SEG23 */
    { .pin_num = WM_GPIO_NUM_28, .pin_mux = WM_GPIO_IOMUX_FUN6 }, /**< SEG24 */
    { .pin_num = WM_GPIO_NUM_29, .pin_mux = WM_GPIO_IOMUX_FUN6 }, /**< SEG25 */
    { .pin_num = WM_GPIO_NUM_30, .pin_mux = WM_GPIO_IOMUX_FUN6 }, /**< SEG26 */
    { .pin_num = WM_GPIO_NUM_31, .pin_mux = WM_GPIO_IOMUX_FUN6 }, /**< SEG27 */
    { .pin_num = WM_GPIO_NUM_32, .pin_mux = WM_GPIO_IOMUX_FUN6 }, /**< SEG28 */
    { .pin_num = WM_GPIO_NUM_33, .pin_mux = WM_GPIO_IOMUX_FUN6 }, /**< SEG29 */
    { .pin_num = WM_GPIO_NUM_34, .pin_mux = WM_GPIO_IOMUX_FUN6 }, /**< SEG30 */
    { .pin_num = WM_GPIO_NUM_6,  .pin_mux = WM_GPIO_IOMUX_FUN6 }  /**< SEG31 */
};

const static wm_dt_hw_seg_lcd_t dt_hw_seg_lcd = {
    .init_cfg        = { .init_level = 0, .init_priority = 0 },
    .reg_base = 0x40011C00,
    .seg_lcd_cfg = {
        .duty_sel = 3,
        .vlcd_cc = 0,
        .bias = 2,
        .hd = 0,
        .frame_freq = 60,
        .com_num = 4,
    },
    .pin_cfg_count       = sizeof(dt_hw_seg_lcd_pin_cfg) / sizeof(dt_hw_seg_lcd_pin_cfg[0]),
    .pin_cfg             = (wm_dt_hw_pin_cfg_t *)dt_hw_seg_lcd_pin_cfg,
    .rcc_device_name = "rcc",
};

#endif /* CONFIG_COMPONENT_DRIVER_SEG_LCD_ENABLED */

#if CONFIG_COMPONENT_DRIVER_SDMMC_ENABLED
const static wm_dt_hw_pin_cfg_t dt_hw_sdh_sdmmc_pin_cfg[] = {
    { .pin_num = WM_GPIO_NUM_9,  .pin_mux = WM_GPIO_IOMUX_FUN1 }, /**< CLK */
    { .pin_num = WM_GPIO_NUM_10, .pin_mux = WM_GPIO_IOMUX_FUN1 }, /**< CMD */
    { .pin_num = WM_GPIO_NUM_11, .pin_mux = WM_GPIO_IOMUX_FUN1 }, /**< DAT0 */
    { .pin_num = WM_GPIO_NUM_12, .pin_mux = WM_GPIO_IOMUX_FUN1 }, /**< DAT1 */
    { .pin_num = WM_GPIO_NUM_13, .pin_mux = WM_GPIO_IOMUX_FUN1 }, /**< DAT2 */
    { .pin_num = WM_GPIO_NUM_14, .pin_mux = WM_GPIO_IOMUX_FUN1 }, /**< CD/DAT3 */
};

const static wm_dt_hw_sdh_t dt_hw_sdh_sdmmc = {
    .init_cfg            = { .init_level = 0, .init_priority = 0 },
    .reg_base = 0x40000A00,
    .sdh_cfg = {
        .clock_hz = 40000000,
        .bus_width = 1, /** WM_SDH_BUS_WIDTH_4BITS */
    },
    .irq_cfg = {
        .irq_num = 24, /** WM_IRQ_SIDO_HOST */
        .irq_priority = 0,
    },
    .pin_cfg_count = sizeof(dt_hw_sdh_sdmmc_pin_cfg) / sizeof(dt_hw_sdh_sdmmc_pin_cfg[0]),
    .pin_cfg = (wm_dt_hw_pin_cfg_t*)dt_hw_sdh_sdmmc_pin_cfg,
    .dma_device_name = "dma",
    .rcc_device_name = "rcc",
};
#endif

#if CONFIG_COMPONENT_DRIVER_SDSPI_ENABLED
const static wm_dt_hw_pin_cfg_t dt_hw_sdh_spi_pin_cfg[] = {
    { .pin_num = WM_GPIO_NUM_22, .pin_mux = WM_GPIO_IOMUX_FUN2 }, /**< CLK - PB6 */
    { .pin_num = WM_GPIO_NUM_23, .pin_mux = WM_GPIO_IOMUX_FUN2 }, /**< MOSI(SDIO_CMD) - PB7 */
};

const static wm_dt_hw_sdh_t dt_hw_sdh_spi = {
    .init_cfg            = { .init_level = 0, .init_priority = 0 },
    .reg_base = 0x40000A00,
    .irq_cfg = {
        .irq_num = 24, /** WM_IRQ_SIDO_HOST */
        .irq_priority = 0,
    },
    .pin_cfg_count = sizeof(dt_hw_sdh_spi_pin_cfg) / sizeof(dt_hw_sdh_spi_pin_cfg[0]),
    .pin_cfg = (wm_dt_hw_pin_cfg_t*)dt_hw_sdh_spi_pin_cfg,
    .dma_device_name = "dma",
    .rcc_device_name = "rcc",
};
#endif

#if CONFIG_COMPONENT_DRIVER_TFT_LCD_ENABLED
const static wm_dt_hw_tft_lcd_spi_t dt_hw_nv3041a = {
    .init_cfg = { .init_level = 0, .init_priority = 0 },
    .spi_cfg = {
        .mode = 0,
        .freq = 40000000,               /* Hz clock */
        .pin_cs = {
            .pin_num = WM_GPIO_NUM_27,  /**< CS - PB11 */
            .pin_mux = WM_GPIO_IOMUX_FUN5,
            .pin_dir = WM_GPIO_DIR_OUTPUT,
            .pin_pupd = WM_GPIO_FLOAT,
        },
    },
    .io_lcd_reset = WM_GPIO_NUM_26,     /**< RST - PB10 */
    .io_lcd_dcx = WM_GPIO_NUM_25,       /**< DCX - PB9 */
    .io_lcd_led = WM_GPIO_NUM_24,       /**< LED - PB8 */
    .io_lcd_te = WM_GPIO_NUM_MAX,       /**< TE  - Not Used  */
    .spi_device_name = "sdspi",
    .gpio_device_name="gpio",
};

const static wm_dt_hw_tft_lcd_spi_t dt_hw_st7735 = {
    .init_cfg = { .init_level = 0, .init_priority = 0 },
    .spi_cfg = {
        .mode = 0,
        .freq = 15000000,               /* Hz clock */
        .pin_cs = {
            .pin_num = WM_GPIO_NUM_27,  /**< CS - PB11 */
            .pin_mux = WM_GPIO_IOMUX_FUN5,
            .pin_dir = WM_GPIO_DIR_OUTPUT,
            .pin_pupd = WM_GPIO_FLOAT,
        },
    },
    .io_lcd_reset = WM_GPIO_NUM_26,     /**< RST - PB10 */
    .io_lcd_led = WM_GPIO_NUM_24,       /**< LED - PB8 */
    .io_lcd_dcx = WM_GPIO_NUM_25,       /**< DCX - PB9 */
    .io_lcd_te = WM_GPIO_NUM_MAX,       /**< TE  - Not Used */
    .spi_device_name = "sdspi",
    .gpio_device_name="gpio",
};
#endif

#if CONFIG_COMPONENT_DRIVER_RTC_ENABLED
const static wm_dt_hw_rtc_t dt_hw_rtc = {
    .init_cfg = { .init_level = 0, .init_priority = 0 },
    .reg_base = 0x40000D0C,
    .irq_cfg  = { .irq_num = 29,   .irq_priority = 0  },
};
#endif

#if CONFIG_COMPONENT_DRIVER_PMU_ENABLED
const static wm_dt_hw_pmu_t dt_hw_pmu = {
    .init_cfg = { .init_level = 0, .init_priority = 0 },
    .reg_base = 0x40000D00,
    .clk_src  = WM_PMU_CLOCK_SRC_32K,
    .irq_cfg  = { .irq_num = 29,   .irq_priority = 0  },
};
#endif

#if CONFIG_COMPONENT_DRIVER_ADC_ENABLED
const static wm_dt_hw_adc_cfg_t dt_hw_adc_cfg[] = {
    { .adc_channel = WM_ADC_CHANNEL_0, .pga_gain1 = WM_ADC_GAIN1_LEVEL_0, .pga_gain2 = WM_ADC_GAIN2_LEVEL_0, .adc_cmp = 0 },
    { .adc_channel = WM_ADC_CHANNEL_1, .pga_gain1 = WM_ADC_GAIN1_LEVEL_0, .pga_gain2 = WM_ADC_GAIN2_LEVEL_0, .adc_cmp = 0 },
    { .adc_channel = WM_ADC_CHANNEL_2, .pga_gain1 = WM_ADC_GAIN1_LEVEL_0, .pga_gain2 = WM_ADC_GAIN2_LEVEL_0, .adc_cmp = 0 },
    { .adc_channel = WM_ADC_CHANNEL_3, .pga_gain1 = WM_ADC_GAIN1_LEVEL_0, .pga_gain2 = WM_ADC_GAIN2_LEVEL_0, .adc_cmp = 0 },
};

const static wm_dt_hw_pin_cfg_t dt_hw_adc_pin_cfg[] = {
    { .pin_num = WM_GPIO_NUM_1, .pin_mux = WM_GPIO_IOMUX_FUN6 }, //adc1
    { .pin_num = WM_GPIO_NUM_2, .pin_mux = WM_GPIO_IOMUX_FUN6 }, //adc4
    { .pin_num = WM_GPIO_NUM_3, .pin_mux = WM_GPIO_IOMUX_FUN6 }, //adc3
    { .pin_num = WM_GPIO_NUM_4, .pin_mux = WM_GPIO_IOMUX_FUN6 }, //adc2
};

const static wm_dt_hw_adc_t dt_hw_adc = {
    .init_cfg        = { .init_level = 0, .init_priority = 0 },
    .reg_base        = 0x40010200,
    .adc_cfg_count   = sizeof(dt_hw_adc_cfg) / sizeof(dt_hw_adc_cfg[0]),
    .adc_cfg         = (wm_dt_hw_adc_cfg_t *)&dt_hw_adc_cfg[0],
    .pin_cfg_count   = sizeof(dt_hw_adc_pin_cfg) / sizeof(dt_hw_adc_pin_cfg[0]),
    .pin_cfg         = (wm_dt_hw_pin_cfg_t *)&dt_hw_adc_pin_cfg[0],
    .irq_cfg         = { .irq_num = 11,   .irq_priority = 0  },
    .dma_device_name = "dma",
    .rcc_device_name = "rcc",
};
#endif

#if CONFIG_COMPONENT_DRIVER_GPIO_ENABLED
const static wm_dt_hw_gpio_cfg_t dt_hw_gpio_pin_cfg[] = {
    { .pin_num = WM_GPIO_NUM_38, .pin_mux = WM_GPIO_IOMUX_FUN5, .pin_dir = 0, .pin_pupd = 0, .int_mode = 3 },
    { .pin_num = WM_GPIO_NUM_37, .pin_mux = WM_GPIO_IOMUX_FUN5, .pin_dir = 0, .pin_pupd = 0, .int_mode = 2 },
};

const static wm_dt_hw_gpio_t dt_hw_gpio = {
    .init_cfg       = { .init_level = 0, .init_priority = 0 },
    .gpio_cfg_count = sizeof(dt_hw_gpio_pin_cfg) / sizeof(dt_hw_gpio_pin_cfg[0]),
    .gpio_cfg       = (wm_dt_hw_gpio_cfg_t *)&dt_hw_gpio_pin_cfg[0],
};
#endif

#if CONFIG_COMPONENT_DRIVER_WDT_ENABLED
const static wm_dt_hw_wdt_t dt_hw_wdt = {
    .init_cfg = { .init_level = 0, .init_priority = 0 },
    .reg_base = 0x40011600,
    .wdt_cfg  = { .counter_value = 10000000 }, // 10s
    .irq_cfg  = { .irq_num = 31, .irq_priority = 0 },
};
#endif

#if CONFIG_COMPONENT_DRIVER_DMA_ENABLED
const static wm_dt_hw_dma_t dt_hw_dma = {
    .reg_base      = 0x40000800,
    .init_cfg      = { .init_level = 0, .init_priority = 50 },
    .irq_cfg_count = 1, // Actually, there are 5, currently calculated based on the base value and channel number
    .irq_cfg       = { .irq_num = 4,    .irq_priority = 0   },
};
#endif

#if CONFIG_COMPONENT_DRIVER_I2C_ENABLED
const static wm_dt_hw_pin_cfg_t dt_hw_i2c_pin_cfg[] = {
    { .pin_num = WM_GPIO_NUM_1, .pin_mux = WM_GPIO_IOMUX_FUN2 },
    { .pin_num = WM_GPIO_NUM_4, .pin_mux = WM_GPIO_IOMUX_FUN2 },
};

const static wm_dt_hw_i2c_t dt_hw_i2c = {
    .init_cfg = { .init_level = 0, .init_priority = 0 },
    .reg_base = 0x40010000,
    .i2c_cfg ={
        .max_clock = WM_I2C_SPEED_FAST,
        .addr_10_bits = 0,
    },
    .pin_cfg_count = sizeof(dt_hw_i2c_pin_cfg) / sizeof(dt_hw_i2c_pin_cfg[0]),
    .pin_cfg = (wm_dt_hw_pin_cfg_t*)&dt_hw_i2c_pin_cfg[0],
    .irq_cfg = {
        .irq_num = 10,
        .irq_priority = 0
    },
    .rcc_device_name = "rcc",
};

const static wm_dt_hw_pin_cfg_t dt_hw_eeprom_pin_cfg[] = {
    /**< Write Protection */
    { .pin_num = WM_GPIO_NUM_21, .pin_mux = WM_GPIO_IOMUX_FUN5, .pin_dir = WM_GPIO_DIR_OUTPUT, .pin_pupd = WM_GPIO_FLOAT },
};

const static wm_dt_hw_eeprom_t dt_hw_eeprom0 = {
    /*EVB board NV24C02 2k bits*/
    .init_cfg      = { .init_level = 0, .init_priority = 0 },
    .pin_cfg_count = sizeof(dt_hw_eeprom_pin_cfg) / sizeof(dt_hw_eeprom_pin_cfg[0]), /*set 0 if not use write protection*/
    .pin_cfg       = (wm_dt_hw_pin_cfg_t *)dt_hw_eeprom_pin_cfg,

    .i2c_device_name = "i2c",

    .speed_hz          = WM_I2C_SPEED_FAST,
    .size              = 256,
    .i2c_addr          = 0x52,
    .page_size         = 16,
    .addr_width        = 8,
    .read_only         = 0,
    .max_write_time_ms = 5,
};

const static wm_dt_hw_eeprom_t dt_hw_eeprom1 = {
    /*7# mini board -1 ST24C02WP 2k bits*/
    .init_cfg      = { .init_level = 0, .init_priority = 0 },
    .pin_cfg_count = sizeof(dt_hw_eeprom_pin_cfg) / sizeof(dt_hw_eeprom_pin_cfg[0]),
    .pin_cfg       = (wm_dt_hw_pin_cfg_t *)dt_hw_eeprom_pin_cfg,

    .i2c_device_name = "i2c",

    .speed_hz          = WM_I2C_SPEED_STANDARD,
    .size              = 256,
    .i2c_addr          = 0x50,
    .page_size         = 16,
    .addr_width        = 8,
    .read_only         = 0,
    .max_write_time_ms = 10,
};
#endif

#if CONFIG_COMPONENT_DRIVER_PSRAM_ENABLED
const static wm_dt_hw_pin_cfg_t dt_hw_psram_pin_cfg[] = {
    /**< SCLK */
    { .pin_num = WM_GPIO_NUM_15, .pin_mux = WM_GPIO_IOMUX_FUN1 },
    /**< CS */
    { .pin_num = WM_GPIO_NUM_43, .pin_mux = WM_GPIO_IOMUX_FUN1 },
    /**< SIO0 */
    { .pin_num = WM_GPIO_NUM_18, .pin_mux = WM_GPIO_IOMUX_FUN4 },
    /**< SIO1 */
    { .pin_num = WM_GPIO_NUM_19, .pin_mux = WM_GPIO_IOMUX_FUN4 },
    /**< SIO2 */
    { .pin_num = WM_GPIO_NUM_20, .pin_mux = WM_GPIO_IOMUX_FUN4 },
    /**< SIO3 */
    { .pin_num = WM_GPIO_NUM_21, .pin_mux = WM_GPIO_IOMUX_FUN4 },
};

const static wm_dt_hw_psram_t dt_hw_psram = {
    /**< Board: EVB, Connected with jumper caps beside the psram */
    .init_cfg      = { .init_level = 0, .init_priority = 0 },
    .reg_base = 0x40002200,
    .psram_cfg ={
        .spi_mode = 1,       /**< 0 : spi, 1 qspi */
        .clock_hz = 20000000,/**< 20M Hz */
    },
    .pin_cfg_count = sizeof(dt_hw_psram_pin_cfg) / sizeof(dt_hw_psram_pin_cfg[0]),
    .pin_cfg = (wm_dt_hw_pin_cfg_t*)dt_hw_psram_pin_cfg,
    .dma_device_name = "dma",
    .rcc_device_name = "rcc"
};
#endif

#if CONFIG_COMPONENT_DRIVER_SPIM_ENABLED
const static wm_dt_hw_pin_cfg_t dt_hw_spim_pin_cfg[] = {
    { .pin_num = WM_GPIO_NUM_17, .pin_mux = WM_GPIO_IOMUX_FUN2 }, //clock
    { .pin_num = WM_GPIO_NUM_16, .pin_mux = WM_GPIO_IOMUX_FUN2 }, //di
    { .pin_num = WM_GPIO_NUM_7,  .pin_mux = WM_GPIO_IOMUX_FUN2 }, //do
};

const static wm_dt_hw_spim_t dt_hw_spim = {
    .init_cfg        = {.init_level = 0, .init_priority = 50},
    .reg_base        = 0x40010400,
    .spi_master = true,
    .pin_cfg_count = sizeof(dt_hw_spim_pin_cfg) / sizeof(dt_hw_spim_pin_cfg[0]),
    .pin_cfg = (wm_dt_hw_pin_cfg_t*)dt_hw_spim_pin_cfg,
    .irq_cfg = {
        .irq_num = WM_IRQ_SPI_LS,
        .irq_priority = 0,
    },
    .dma_device_name = "dma",
    .rcc_device_name = "rcc",
};
#endif

#if CONFIG_COMPONENT_DRIVER_RCC_ENABLED
const static wm_dt_hw_rcc_cfg_t dt_hw_rcc_cfg[] = {
    { .type = WM_RCC_TYPE_CPU,  .clock = 80  },
    { .type = WM_RCC_TYPE_WLAN, .clock = 160 },
};

const static wm_dt_hw_rcc_t dt_hw_rcc = {
    .init_cfg      = { .init_level = 0, .init_priority = 50 },
    .reg_base      = 0x40000E00,
    .rcc_cfg_count = sizeof(dt_hw_rcc_cfg) / sizeof(dt_hw_rcc_cfg[0]),
    .rcc_cfg       = (wm_dt_hw_rcc_cfg_t *)&dt_hw_rcc_cfg[0],
};
#endif

#if CONFIG_COMPONENT_DRIVER_INTERNAL_FLASH_ENABLED
const static wm_dt_hw_iflash_t dt_hw_iflash = {
    .init_cfg = {.init_level = 0, .init_priority = 50},
    .reg_base = 0x40002000,
    .flash_cfg = {
        .quad_spi = true,
    },
};
#endif

#if CONFIG_COMPONENT_DRIVER_EXTERNAL_FLASH_ENABLED
const static wm_dt_hw_eflash_t dt_hw_eflash_w25q = {
    .init_cfg = {.init_level = 0, .init_priority = 50},
    .flash_cfg = { .quad_spi = false},
    .spi_device_name = "spim",
    .spi_cfg = {
        .mode = 0, //(CPOL=0,CPHA=0),
        .freq = 20 * 1000000, //20M clock
        .pin_cs = {
            .pin_num = WM_GPIO_NUM_26,
            .pin_mux = WM_GPIO_IOMUX_FUN5,
            .pin_dir = WM_GPIO_DIR_OUTPUT,
            .pin_pupd = WM_GPIO_FLOAT,
        },
    },
};

const static wm_dt_hw_eflash_t dt_hw_eflash_gd25q = {
    .init_cfg = {.init_level = 0, .init_priority = 50},
    .flash_cfg = { .quad_spi = false},
    .spi_device_name = "spim",
    .spi_cfg = {
        .mode = 0, //(CPOL=0,CPHA=0),
        .freq = 20 * 1000000, //20M clock
        .pin_cs = {
            .pin_num = WM_GPIO_NUM_26,
            .pin_mux = WM_GPIO_IOMUX_FUN5,
            .pin_dir = WM_GPIO_DIR_OUTPUT,
            .pin_pupd = WM_GPIO_FLOAT,
        },
    },
};

const static wm_dt_hw_eflash_t dt_hw_eflash_xt25f = {
    .init_cfg = {.init_level = 0, .init_priority = 50},
    .flash_cfg = { .quad_spi = false},
    .spi_device_name = "spim",
    .spi_cfg = {
        .mode = 0, //(CPOL=0,CPHA=0),
        .freq = 10 * 1000000, //10M clock
        .pin_cs = {
            .pin_num = WM_GPIO_NUM_26,
            .pin_mux = WM_GPIO_IOMUX_FUN5,
            .pin_dir = WM_GPIO_DIR_OUTPUT,
            .pin_pupd = WM_GPIO_FLOAT,
        },
    },
};

const static wm_dt_hw_eflash_t dt_hw_eflash_th25q = {
    .init_cfg = {.init_level = 0, .init_priority = 50},
    .flash_cfg = { .quad_spi = false},
    .spi_device_name = "spim",
    .spi_cfg = {
        .mode = 0, //(CPOL=0,CPHA=0),
        .freq = 10 * 1000000, //10M clock
        .pin_cs = {
            .pin_num = WM_GPIO_NUM_26,
            .pin_mux = WM_GPIO_IOMUX_FUN5,
            .pin_dir = WM_GPIO_DIR_OUTPUT,
            .pin_pupd = WM_GPIO_FLOAT,
        },
    },
};

#endif

#if CONFIG_COMPONENT_DRIVER_TOUCH_SENSOR_ENABLED
const static wm_dt_hw_touch_button_cfg_t dt_hw_touch_button_cfg[] = {
    { .key_num = 5, .threshold = 20 }, //pb2
    { .key_num = 3, .threshold = 25 }, //pb0
    { .key_num = 4, .threshold = 30 }, //pb1
};

const static wm_dt_hw_pin_cfg_t dt_hw_touch_button_pin_cfg[] = {
    { .pin_num = WM_GPIO_NUM_18, .pin_mux = WM_GPIO_IOMUX_FUN7 }, //pb2
    { .pin_num = WM_GPIO_NUM_16, .pin_mux = WM_GPIO_IOMUX_FUN7 }, //pb0
    { .pin_num = WM_GPIO_NUM_17, .pin_mux = WM_GPIO_IOMUX_FUN7 }, //pb1
};

const static wm_dt_hw_pin_cfg_t dt_hw_touch_sensor_pin_cfg[] = {
    { .pin_num = WM_GPIO_NUM_12, .pin_mux = WM_GPIO_IOMUX_FUN7 }, //pa12, cmod
    { .pin_num = WM_GPIO_NUM_14, .pin_mux = WM_GPIO_IOMUX_FUN7 }, //pa14, cdc
};

const static wm_dt_hw_touch_sensor_t dt_hw_touch_sensor = {
    .init_cfg        = { .init_level = 0, .init_priority = 50 },
    .reg_base        = 0x40012400,
    .pin_cfg_count   = sizeof(dt_hw_touch_sensor_pin_cfg) / sizeof(dt_hw_touch_sensor_pin_cfg[0]),
    .pin_cfg         = (wm_dt_hw_pin_cfg_t *)&dt_hw_touch_sensor_pin_cfg[0],
    .irq_cfg         = { .irq_num = 18,   .irq_priority = 0   },
    .rcc_device_name = "rcc",
};

const static wm_dt_hw_touch_button_t dt_hw_touch_button = {
    .init_cfg                 = { .init_level = 0, .init_priority = 49 },
    .touch_button_cfg_count   = sizeof(dt_hw_touch_button_cfg) / sizeof(dt_hw_touch_button_cfg[0]),
    .touch_button_cfg         = (wm_dt_hw_touch_button_cfg_t *)&dt_hw_touch_button_cfg[0],
    .pin_cfg_count            = sizeof(dt_hw_touch_button_pin_cfg) / sizeof(dt_hw_touch_button_pin_cfg[0]),
    .pin_cfg                  = (wm_dt_hw_pin_cfg_t *)&dt_hw_touch_button_pin_cfg[0],
    .touch_sensor_device_name = "touch_sensor",
};
#endif /* CONFIG_COMPONENT_DRIVER_TOUCH_SENSOR_ENABLED */

#if CONFIG_COMPONENT_DRIVER_CRC_ENABLED
const static wm_dt_hw_crc_t dt_hw_crc = {
    .reg_base = 0x40000600, .rcc_device_name = "rcc", .init_cfg = { .init_level = 0, .init_priority = 50 }
};
#endif

#if CONFIG_COMPONENT_DRIVER_CRYPTO_ENABLED
const static wm_dt_hw_crypto_t dt_hw_cryp = {
    .reg_base = 0x40000600, .rcc_device_name = "rcc", .init_cfg = { .init_level = 0, .init_priority = 50 }
};
#endif

#if CONFIG_COMPONENT_DRIVER_HASH_ENABLED
const static wm_dt_hw_hash_t dt_hw_hash = {
    .reg_base = 0x40000600, .rcc_device_name = "rcc", .init_cfg = { .init_level = 0, .init_priority = 50 }
};
#endif

#if CONFIG_COMPONENT_DRIVER_RNG_ENABLED
const static wm_dt_hw_rng_t dt_hw_rng = {
    .reg_base = 0x40000600, .rcc_device_name = "rcc", .init_cfg = { .init_level = 0, .init_priority = 50 }
};
#endif

#if CONFIG_COMPONENT_DRIVER_RSA_ENABLED
const static wm_dt_hw_rsa_t dt_hw_rsa = {
    .reg_base = 0x40000000, .rcc_device_name = "rcc", .init_cfg = { .init_level = 0, .init_priority = 50 }
};
#endif

#if CONFIG_COMPONENT_DRIVER_CODEC_ES8374_ENABLED
const static wm_dt_hw_codec_i2s_t dt_hw_es8374 = {
    .init_cfg                 = { .init_level = 0, .init_priority = 49 },
    .i2s_device_name = "i2s",
    .i2c_device_name = "i2c",
    .codec_cfg = {
        .i2c_address = 0x10,
        .in_port = 2,
        .out_port = 3,
        .jack_pin = WM_GPIO_NUM_MAX,
        .pa_pin = WM_GPIO_NUM_MAX,
        .max_gain = 0,
    },
};
#endif

#if CONFIG_COMPONENT_DRIVER_SDIO_SLAVE_ENABLED
const static wm_dt_hw_pin_cfg_t dt_hw_sdio_slave_pin_cfg[] = {
    { .pin_num = WM_GPIO_NUM_22, .pin_mux = WM_GPIO_IOMUX_FUN4 }, /**< CLK */
    { .pin_num = WM_GPIO_NUM_23, .pin_mux = WM_GPIO_IOMUX_FUN4 }, /**< CMD */
    { .pin_num = WM_GPIO_NUM_24, .pin_mux = WM_GPIO_IOMUX_FUN4 }, /**< DAT0 */
    { .pin_num = WM_GPIO_NUM_25, .pin_mux = WM_GPIO_IOMUX_FUN4 }, /**< DAT1 */
    { .pin_num = WM_GPIO_NUM_26, .pin_mux = WM_GPIO_IOMUX_FUN4 }, /**< DAT2 */
    { .pin_num = WM_GPIO_NUM_27, .pin_mux = WM_GPIO_IOMUX_FUN4 }, /**< CD/DAT3 */
};

const static wm_dt_hw_sdio_slave_t dt_hw_sdio_slave = {
    .init_cfg            = { .init_level = 0, .init_priority = 0 },
    .sdio_slave_reg_base = 0x40002400,
    .wrapper_reg_base    = 0x40002800,
    .irq_cfg             = { .irq_num = 0,    .irq_priority = 0  },
    .pin_cfg_count       = sizeof(dt_hw_sdio_slave_pin_cfg) / sizeof(dt_hw_sdio_slave_pin_cfg[0]),
    .pin_cfg             = (wm_dt_hw_pin_cfg_t *)&dt_hw_sdio_slave_pin_cfg[0],
    .rcc_device_name     = "rcc",
};
#endif

const static struct wm_dt_table_entry dt_hw_table_entry[] = {
#if CONFIG_COMPONENT_DRIVER_UART_ENABLED
    { .dev_name = "uart0",        .hw_addr = (void *)&dt_hw_uart0,        .ops_addr = (void *)&wm_drv_uart_ops             },
    { .dev_name = "uart1",        .hw_addr = (void *)&dt_hw_uart1,        .ops_addr = (void *)&wm_drv_uart_ops             },
    { .dev_name = "uart2",        .hw_addr = (void *)&dt_hw_uart2,        .ops_addr = (void *)&wm_drv_uart_ops             },
    { .dev_name = "uart3",        .hw_addr = (void *)&dt_hw_uart3,        .ops_addr = (void *)&wm_drv_uart_ops             },
    { .dev_name = "uart4",        .hw_addr = (void *)&dt_hw_uart4,        .ops_addr = (void *)&wm_drv_uart_ops             },
    { .dev_name = "uart5",        .hw_addr = (void *)&dt_hw_uart5,        .ops_addr = (void *)&wm_drv_uart_ops             },
#endif  /* CONFIG_COMPONENT_DRIVER_UART_ENABLED */
#if CONFIG_COMPONENT_DRIVER_TIMER_ENABLED
    { .dev_name = "timer0",       .hw_addr = (void *)&dt_hw_timer0,       .ops_addr = (void *)&wm_drv_timer_ops            },
    { .dev_name = "timer1",       .hw_addr = (void *)&dt_hw_timer1,       .ops_addr = (void *)&wm_drv_timer_ops            },
    { .dev_name = "timer2",       .hw_addr = (void *)&dt_hw_timer2,       .ops_addr = (void *)&wm_drv_timer_ops            },
    { .dev_name = "timer3",       .hw_addr = (void *)&dt_hw_timer3,       .ops_addr = (void *)&wm_drv_timer_ops            },
    { .dev_name = "timer4",       .hw_addr = (void *)&dt_hw_timer4,       .ops_addr = (void *)&wm_drv_timer_ops            },
    { .dev_name = "timer5",       .hw_addr = (void *)&dt_hw_timer5,       .ops_addr = (void *)&wm_drv_timer_ops            },
#endif  /* CONFIG_COMPONENT_DRIVER_TIMER_ENABLED */
#if CONFIG_COMPONENT_DRIVER_PWM_ENABLED
    { .dev_name = "pwm",          .hw_addr = (void *)&dt_hw_pwm,          .ops_addr = (void *)&wm_drv_pwm_ops              },
#endif  /* CONFIG_COMPONENT_DRIVER_PWM_ENABLED */
#if CONFIG_COMPONENT_DRIVER_I2S_ENABLED
    { .dev_name = "i2s",          .hw_addr = (void *)&dt_hw_i2s,          .ops_addr = (void *)&wm_drv_i2s_ops              },
#endif  /* CONFIG_COMPONENT_DRIVER_I2S_ENABLED */
#if CONFIG_COMPONENT_DRIVER_SEG_LCD_ENABLED
    { .dev_name = "seg_lcd",      .hw_addr = (void *)&dt_hw_seg_lcd,      .ops_addr = (void *)&wm_drv_seg_lcd_ops          },
#endif  /* CONFIG_COMPONENT_DRIVER_SEG_LCD_ENABLED */
#if CONFIG_COMPONENT_DRIVER_SDMMC_ENABLED
    { .dev_name = "sdmmc",        .hw_addr = (void *)&dt_hw_sdh_sdmmc,    .ops_addr = (void *)&wm_drv_sdh_sdmmc_ops        },
#endif  /* CONFIG_COMPONENT_DRIVER_SDMMC_ENABLED */
#if CONFIG_COMPONENT_DRIVER_SDSPI_ENABLED
    { .dev_name = "sdspi",        .hw_addr = (void *)&dt_hw_sdh_spi,      .ops_addr = (void *)&wm_drv_sdh_spi_ops          },
#endif  /* CONFIG_COMPONENT_DRIVER_SDSPI_ENABLED */
#if CONFIG_COMPONENT_DRIVER_TFT_LCD_ENABLED
    { .dev_name = "nv3041a_spi",  .hw_addr = (void *)&dt_hw_nv3041a,      .ops_addr = (void *)&wm_drv_tft_lcd_nv3041a_ops  },
    { .dev_name = "st7735_spi",   .hw_addr = (void *)&dt_hw_st7735,       .ops_addr = (void *)&wm_drv_tft_lcd_st7735_ops   },
#endif  /* CONFIG_COMPONENT_DRIVER_TFT_LCD_ENABLED */
#if CONFIG_COMPONENT_DRIVER_RTC_ENABLED
    { .dev_name = "rtc",          .hw_addr = (void *)&dt_hw_rtc,          .ops_addr = (void *)&wm_drv_rtc_ops              },
#endif  /* CONFIG_COMPONENT_DRIVER_RTC_ENABLED */
#if CONFIG_COMPONENT_DRIVER_PMU_ENABLED
    { .dev_name = "pmu",          .hw_addr = (void *)&dt_hw_pmu,          .ops_addr = (void *)&wm_drv_pmu_ops              },
#endif  /* CONFIG_COMPONENT_DRIVER_PMU_ENABLED */
#if CONFIG_COMPONENT_DRIVER_ADC_ENABLED
    { .dev_name = "adc",          .hw_addr = (void *)&dt_hw_adc,          .ops_addr = (void *)&wm_drv_adc_ops              },
#endif  /* CONFIG_COMPONENT_DRIVER_ADC_ENABLED */
#if CONFIG_COMPONENT_DRIVER_GPIO_ENABLED
    { .dev_name = "gpio",         .hw_addr = (void *)&dt_hw_gpio,         .ops_addr = (void *)&wm_drv_gpio_ops             },
#endif  /* CONFIG_COMPONENT_DRIVER_GPIO_ENABLED */
#if CONFIG_COMPONENT_DRIVER_WDT_ENABLED
    { .dev_name = "wdt",          .hw_addr = (void *)&dt_hw_wdt,          .ops_addr = (void *)&wm_drv_wdt_ops              },
#endif  /* CONFIG_COMPONENT_DRIVER_WDT_ENABLED */
#if CONFIG_COMPONENT_DRIVER_DMA_ENABLED
    { .dev_name = "dma",          .hw_addr = (void *)&dt_hw_dma,          .ops_addr = (void *)&wm_drv_dma_ops              },
#endif  /* CONFIG_COMPONENT_DRIVER_DMA_ENABLED */
#if CONFIG_COMPONENT_DRIVER_I2C_ENABLED
    { .dev_name = "i2c",          .hw_addr = (void *)&dt_hw_i2c,          .ops_addr = (void *)&wm_drv_i2c_ops              },
    { .dev_name = "eeprom0",      .hw_addr = (void *)&dt_hw_eeprom0,      .ops_addr = (void *)&wm_drv_eeprom_nv24c0x_ops   },
    { .dev_name = "eeprom1",      .hw_addr = (void *)&dt_hw_eeprom1,      .ops_addr = (void *)&wm_drv_eeprom_nv24c0x_ops   },
#endif  /* CONFIG_COMPONENT_DRIVER_I2C_ENABLED */
#if CONFIG_COMPONENT_DRIVER_PSRAM_ENABLED
    { .dev_name = "psram",        .hw_addr = (void *)&dt_hw_psram,        .ops_addr = (void *)&wm_drv_psram_ops            },
#endif  /* CONFIG_COMPONENT_DRIVER_PSRAM_ENABLED */
#if CONFIG_COMPONENT_DRIVER_SPIM_ENABLED
    { .dev_name = "spim",         .hw_addr = (void *)&dt_hw_spim,         .ops_addr = (void *)&wm_drv_spim_ops             },
#endif  /* CONFIG_COMPONENT_DRIVER_SPIM_ENABLED */
#if CONFIG_COMPONENT_DRIVER_RCC_ENABLED
    { .dev_name = "rcc",          .hw_addr = (void *)&dt_hw_rcc,          .ops_addr = (void *)&wm_drv_rcc_ops              },
#endif  /* CONFIG_COMPONENT_DRIVER_SPIM_ENABLED */
#if CONFIG_COMPONENT_DRIVER_INTERNAL_FLASH_ENABLED
    { .dev_name = "iflash",       .hw_addr = (void *)&dt_hw_iflash,       .ops_addr = (void *)&wm_drv_internal_flash_ops   },
#endif  /* CONFIG_COMPONENT_DRIVER_INTERNAL_FLASH_ENABLED */
#if CONFIG_COMPONENT_DRIVER_EXTERNAL_FLASH_ENABLED
    { .dev_name = "flash_w25q",   .hw_addr = (void *)&dt_hw_eflash_w25q,  .ops_addr = (void *)&wm_drv_external_flash_ops   },
    { .dev_name = "flash_gd25q",  .hw_addr = (void *)&dt_hw_eflash_gd25q, .ops_addr = (void *)&wm_drv_external_flash_ops   },
    { .dev_name = "flash_xt25f",  .hw_addr = (void *)&dt_hw_eflash_xt25f, .ops_addr = (void *)&wm_drv_external_flash_ops   },
    { .dev_name = "flash_th25q",  .hw_addr = (void *)&dt_hw_eflash_th25q, .ops_addr = (void *)&wm_drv_external_flash_ops   },
#endif  /* CONFIG_COMPONENT_DRIVER_EXTERNAL_FLASH_ENABLED */
#if CONFIG_COMPONENT_DRIVER_TOUCH_SENSOR_ENABLED
    { .dev_name = "touch_sensor", .hw_addr = (void *)&dt_hw_touch_sensor, .ops_addr = (void *)&wm_drv_touch_sensor_ops     },
    { .dev_name = "touch_button", .hw_addr = (void *)&dt_hw_touch_button, .ops_addr = (void *)&wm_drv_touch_button_ops     },
#endif  /* CONFIG_COMPONENT_DRIVER_TOUCH_SENSOR_ENABLED */
#if CONFIG_COMPONENT_DRIVER_CRC_ENABLED
    { .dev_name = "crc",          .hw_addr = (void *)&dt_hw_crc,          .ops_addr = (void *)&wm_drv_crc_ops              },
#endif  /* CONFIG_COMPONENT_DRIVER_CRC_ENABLED */
#if CONFIG_COMPONENT_DRIVER_CRYPTO_ENABLED
    { .dev_name = "crypto",       .hw_addr = (void *)&dt_hw_cryp,         .ops_addr = (void *)&wm_drv_crypto_ops           },
#endif  /* CONFIG_COMPONENT_DRIVER_CRYPTO_ENABLED */
#if CONFIG_COMPONENT_DRIVER_HASH_ENABLED
    { .dev_name = "hash",         .hw_addr = (void *)&dt_hw_hash,         .ops_addr = (void *)&wm_drv_hash_ops             },
#endif  /* CONFIG_COMPONENT_DRIVER_HASH_ENABLED */
#if CONFIG_COMPONENT_DRIVER_RNG_ENABLED
    { .dev_name = "rng",          .hw_addr = (void *)&dt_hw_rng,          .ops_addr = (void *)&wm_drv_rng_ops              },
#endif  /* CONFIG_COMPONENT_DRIVER_RNG_ENABLED */
#if CONFIG_COMPONENT_DRIVER_RSA_ENABLED
    { .dev_name = "rsa",          .hw_addr = (void *)&dt_hw_rsa,          .ops_addr = (void *)&wm_drv_rsa_ops              },
#endif  /* CONFIG_COMPONENT_DRIVER_RSA_ENABLED */
#if CONFIG_COMPONENT_DRIVER_CODEC_ES8374_ENABLED
    { .dev_name = "es8374",       .hw_addr = (void *)&dt_hw_es8374,       .ops_addr = (void *)&wm_drv_codec_i2s_es8374_ops },
#endif  /* CONFIG_COMPONENT_DRIVER_CODEC_ES8374_ENABLED */
#if CONFIG_COMPONENT_DRIVER_SDIO_SLAVE_ENABLED
    { .dev_name = "sdio_slave",   .hw_addr = (void *)&dt_hw_sdio_slave,   .ops_addr = (void *)&wm_drv_sdio_slave_ops       },
#endif  /* CONFIG_COMPONENT_DRIVER_SDIO_SLAVE_ENABLED */
};

WM_DT_TABLE_DEFINE(, (sizeof(dt_hw_table_entry) / sizeof(dt_hw_table_entry[0])), (void *)&dt_hw_table_entry[0]);
