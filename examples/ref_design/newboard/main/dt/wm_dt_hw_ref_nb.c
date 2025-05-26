/**
 * @file wm_dt_hw_ref_nb.c
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

typedef struct wm_drv_ops_structure wm_drv_rtc_ops_t;
extern const wm_drv_rtc_ops_t wm_drv_rtc_ops;

typedef struct wm_drv_ops_structure wm_drv_pmu_ops_t;
extern const wm_drv_pmu_ops_t wm_drv_pmu_ops;

typedef struct wm_drv_ops_structure wm_drv_gpio_ops_t;
extern const wm_drv_gpio_ops_t wm_drv_gpio_ops;

typedef struct wm_drv_ops_structure wm_drv_wdt_ops_t;
extern const wm_drv_wdt_ops_t wm_drv_wdt_ops;

typedef struct wm_drv_ops_structure wm_drv_dma_ops_t;
extern const wm_drv_dma_ops_t wm_drv_dma_ops;

typedef struct wm_drv_ops_structure wm_drv_clock_ops_t;
extern const wm_drv_clock_ops_t wm_drv_rcc_ops;

typedef struct wm_drv_ops_structure wm_drv_flash_ops_t;
extern const wm_drv_flash_ops_t wm_drv_internal_flash_ops;

typedef struct wm_drv_ops_structure wm_drv_crc_ops_t;
extern const wm_drv_crc_ops_t wm_drv_crc_ops;

typedef struct wm_drv_ops_structure wm_drv_hash_ops_t;
extern const wm_drv_hash_ops_t wm_drv_hash_ops;

typedef struct wm_drv_ops_structure wm_drv_rng_ops_t;
extern const wm_drv_rng_ops_t wm_drv_rng_ops;

#if CONFIG_COMPONENT_DRIVER_UART_ENABLED
const static wm_dt_hw_pin_cfg_t dt_hw_uart0_pin[] = {
    { .pin_num = WM_GPIO_NUM_35, .pin_mux = WM_GPIO_IOMUX_FUN1 },
    { .pin_num = WM_GPIO_NUM_36, .pin_mux = WM_GPIO_IOMUX_FUN1 },
};

const static wm_dt_hw_uart_t dt_hw_uart0 = {
    .init_cfg        = { .init_level = 0, .init_priority = 0 },
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
    .init_cfg        = { .init_level = 0, .init_priority = 0 },
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

#if CONFIG_COMPONENT_DRIVER_GPIO_ENABLED
const static wm_dt_hw_gpio_t dt_hw_gpio = {
    .init_cfg = { .init_level = 0, .init_priority = 0 },
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

#if CONFIG_COMPONENT_DRIVER_PSRAM_ENABLED
const static wm_dt_hw_pin_cfg_t dt_hw_psram_pin_cfg[] = {
    /**< board: EVB  */
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
        .clock_hz = 80000000,/**< 80M Hz */
    },
    .pin_cfg_count = sizeof(dt_hw_psram_pin_cfg) / sizeof(dt_hw_psram_pin_cfg[0]),
    .pin_cfg = (wm_dt_hw_pin_cfg_t*)dt_hw_psram_pin_cfg,
    .dma_device_name = "dma",
    .rcc_device_name = "rcc"
};
#endif

#if CONFIG_COMPONENT_DRIVER_RCC_ENABLED
const static wm_dt_hw_rcc_cfg_t dt_hw_rcc_cfg[2] = {
    [0] = { .type = WM_RCC_TYPE_CPU,  .clock = 240 },
    [1] = { .type = WM_RCC_TYPE_WLAN, .clock = 160 },
};

const static wm_dt_hw_rcc_t dt_hw_rcc = {
    .init_cfg      = { .init_level = 0, .init_priority = 50 },
    .reg_base      = 0x40000E00,
    .rcc_cfg_count = 2,
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

#if CONFIG_COMPONENT_DRIVER_CRC_ENABLED
const static wm_dt_hw_crc_t dt_hw_crc = {
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

const static struct wm_dt_table_entry dt_hw_table_entry[] = {
#if CONFIG_COMPONENT_DRIVER_UART_ENABLED
    { .dev_name = "uart0",  .hw_addr = (void *)&dt_hw_uart0,  .ops_addr = (void *)&wm_drv_uart_ops           },
    { .dev_name = "uart1",  .hw_addr = (void *)&dt_hw_uart1,  .ops_addr = (void *)&wm_drv_uart_ops           },
#endif  /* CONFIG_COMPONENT_DRIVER_UART_ENABLED */
#if CONFIG_COMPONENT_DRIVER_TIMER_ENABLED
    { .dev_name = "timer0", .hw_addr = (void *)&dt_hw_timer0, .ops_addr = (void *)&wm_drv_timer_ops          },
    { .dev_name = "timer1", .hw_addr = (void *)&dt_hw_timer1, .ops_addr = (void *)&wm_drv_timer_ops          },
    { .dev_name = "timer2", .hw_addr = (void *)&dt_hw_timer2, .ops_addr = (void *)&wm_drv_timer_ops          },
    { .dev_name = "timer3", .hw_addr = (void *)&dt_hw_timer3, .ops_addr = (void *)&wm_drv_timer_ops          },
    { .dev_name = "timer4", .hw_addr = (void *)&dt_hw_timer4, .ops_addr = (void *)&wm_drv_timer_ops          },
    { .dev_name = "timer5", .hw_addr = (void *)&dt_hw_timer5, .ops_addr = (void *)&wm_drv_timer_ops          },
#endif  /* CONFIG_COMPONENT_DRIVER_TIMER_ENABLED */
#if CONFIG_COMPONENT_DRIVER_RTC_ENABLED
    { .dev_name = "rtc",    .hw_addr = (void *)&dt_hw_rtc,    .ops_addr = (void *)&wm_drv_rtc_ops            },
#endif  /* CONFIG_COMPONENT_DRIVER_RTC_ENABLED */
#if CONFIG_COMPONENT_DRIVER_PMU_ENABLED
    { .dev_name = "pmu",    .hw_addr = (void *)&dt_hw_pmu,    .ops_addr = (void *)&wm_drv_pmu_ops            },
#endif  /* CONFIG_COMPONENT_DRIVER_PMU_ENABLED */
#if CONFIG_COMPONENT_DRIVER_GPIO_ENABLED
    { .dev_name = "gpio",   .hw_addr = (void *)&dt_hw_gpio,   .ops_addr = (void *)&wm_drv_gpio_ops           },
#endif  /* CONFIG_COMPONENT_DRIVER_GPIO_ENABLED */
#if CONFIG_COMPONENT_DRIVER_WDT_ENABLED
    { .dev_name = "wdt",    .hw_addr = (void *)&dt_hw_wdt,    .ops_addr = (void *)&wm_drv_wdt_ops            },
#endif  /* CONFIG_COMPONENT_DRIVER_WDT_ENABLED */
#if CONFIG_COMPONENT_DRIVER_DMA_ENABLED
    { .dev_name = "dma",    .hw_addr = (void *)&dt_hw_dma,    .ops_addr = (void *)&wm_drv_dma_ops            },
#endif  /* CONFIG_COMPONENT_DRIVER_DMA_ENABLED */
#if CONFIG_COMPONENT_DRIVER_PSRAM_ENABLED
    { .dev_name = "psram",  .hw_addr = (void *)&dt_hw_psram,  .ops_addr = (void *)&wm_drv_psram_ops          },
#endif  /* CONFIG_COMPONENT_DRIVER_PSRAM_ENABLED */
#if CONFIG_COMPONENT_DRIVER_RCC_ENABLED
    { .dev_name = "rcc",    .hw_addr = (void *)&dt_hw_rcc,    .ops_addr = (void *)&wm_drv_rcc_ops            },
#endif  /* CONFIG_COMPONENT_DRIVER_SPIM_ENABLED */
#if CONFIG_COMPONENT_DRIVER_INTERNAL_FLASH_ENABLED
    { .dev_name = "iflash", .hw_addr = (void *)&dt_hw_iflash, .ops_addr = (void *)&wm_drv_internal_flash_ops },
#endif  /* CONFIG_COMPONENT_DRIVER_INTERNAL_FLASH_ENABLED */
#if CONFIG_COMPONENT_DRIVER_CRC_ENABLED
    { .dev_name = "crc",    .hw_addr = (void *)&dt_hw_crc,    .ops_addr = (void *)&wm_drv_crc_ops            },
#endif  /* CONFIG_COMPONENT_DRIVER_CRC_ENABLED */
#if CONFIG_COMPONENT_DRIVER_HASH_ENABLED
    { .dev_name = "hash",   .hw_addr = (void *)&dt_hw_hash,   .ops_addr = (void *)&wm_drv_hash_ops           },
#endif  /* CONFIG_COMPONENT_DRIVER_HASH_ENABLED */
#if CONFIG_COMPONENT_DRIVER_RNG_ENABLED
    { .dev_name = "rng",    .hw_addr = (void *)&dt_hw_rng,    .ops_addr = (void *)&wm_drv_rng_ops            },
#endif  /* CONFIG_COMPONENT_DRIVER_RNG_ENABLED */
};

WM_DT_TABLE_DEFINE(, (sizeof(dt_hw_table_entry) / sizeof(dt_hw_table_entry[0])), (void *)&dt_hw_table_entry[0]);