/**
 * @file wm_dt_hw.h
 *
 * @brief Hardware Info Module
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
#ifndef __WM_DT_HW_H__
#define __WM_DT_HW_H__

#include "wmsdk_config.h"
#include "wm_types.h"
#include "wm_soc_cfgs.h"
#if CONFIG_COMPONENT_DRIVER_SEG_LCD_ENABLED
//#include "wm_drv_seg_lcd.h"
#endif /* CONFIG_COMPONENT_DRIVER_SEG_LCD_ENABLED */
#if CONFIG_COMPONENT_DRIVER_CRC_ENABLED
//#include "wm_drv_crc.h"
#endif
#if CONFIG_COMPONENT_DRIVER_CRYPTO_ENABLED
//#include "wm_drv_crypto.h"
#endif
#if CONFIG_COMPONENT_DRIVER_HASH_ENABLED
//#include "wm_drv_hash.h"
#endif
#if CONFIG_COMPONENT_DRIVER_RNG_ENABLED
//#include "wm_drv_rng.h"
#endif
#if CONFIG_COMPONENT_DRIVER_RSA_ENABLED
//#include "wm_drv_rsa.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

#pragma pack(1)

/**
 * @brief Initial Priority Configurate Structure
 * It must be the first member of the parent structure for any device that uses it
 */
typedef struct {
    uint8_t init_level;    /**< 0:The device is initiailzed by application.
                            *   1:The device is initiailzed by system automatically upon boot up */
    uint8_t init_priority; /**< Define the initial priority for the device, range from 0(lowest)to 100(highest),
                            *   same initialization priority value, the system will make a random choice
                            *   for their initialization order */
} wm_dt_hw_init_cfg_t;

typedef struct {
    uint8_t irq_num; /**< @ref wm_irq_no_t */
    uint8_t irq_priority;
} wm_dt_hw_irq_cfg_t;

typedef struct {
    uint8_t pin_num;  /**< @ref wm_gpio_num_t */
    uint8_t pin_mux;  /**< @ref wm_gpio_pin_mux_t */
    uint8_t pin_dir;  /**< @ref wm_gpio_dir_t */
    uint8_t pin_pupd; /**< @ref wm_gpio_pupd_t */
} wm_dt_hw_pin_cfg_t;

typedef struct {
    uint8_t mode;              /**< support 0(CPOL=0,CPHA=0), 1(CPOL=0,CPHA=1), 2(CPOL=1,CPHA=0), 3(CPOL=1,CPHA=1) */
    uint32_t freq;             /**< freq for current device, unit is HZ, freq = FAPB_CLK/ ( 2 x (Divider +1))
                                    range: WM_SPIM_MIN_CLOCK ~ WM_SPIM_MAX_CLOCK*/
    wm_dt_hw_pin_cfg_t pin_cs; /**< the CS GPIO number for select slave device */
} wm_dt_hw_spim_dev_cfg_t;

typedef struct {
    bool extal_clock_en; /**< external mclk enable  */
    uint32_t mclk_hz;    /**< unit hz. mclk=F_I2SCLK/div, div=[2,63];
                                    F_I2SCLK : peripheral clock ,default 160M ;
                                    Note: when use external clk, F_mclk should be configured as 256 * fs where fs is sample frequency.*/
} wm_dt_hw_i2s_cfg_t;

typedef struct {
    bool jack_output_gpio1;  /**< GPIO1 is used to output when jack source detect active */
    bool jack_output_gpio2;  /**< GPIO1 is used to output when jack source detect active */
    bool jack_output_level;  /**< the level output when detect active, should match the SOC GPIO */
    bool jack_detect_rin1;   /**< jack detect source from RIN1 */
    bool jack_detect_gpio1;  /**< The variable is mutually exclusive with the dmic clock */
    bool dmic_clk_out_gpio1; /**< set GPIO1 to DMIC clock output, is mutually exclusive with the jack */
    bool dmic;               /**< true if input with digital mic */
    bool lin1;               /**< true if input with LIN1, also need RIN1 to set */
    bool rin1;               /**< true if input with RIN1, also need LIN1 to set */
    bool lin2;               /**< true if input with LIN2, also need RIN2 to set */
    bool rin2;               /**< true if input with RIN2, also need LIN2 to set */
    bool monoout;            /**< if output with phonejack avaliable */
    bool spkout;             /**< if output with classD speaker avaliable */
    bool i2c;                /**< true if use i2c to control, else SPI */
    uint8_t address;         /**< the I2C address of es8374 */
} wm_dt_hw_codec_es8374_cfg_t;

typedef struct {
    int baudrate;      /**< @ref wm_uart_baudrate_t */
    uint8_t parity;    /**< @ref wm_uart_parity_t */
    uint8_t stop_bits; /**< @ref wm_uart_stop_bits_t */
    uint8_t data_bits; /**< @ref wm_uart_data_bits_t */
    uint8_t flow_ctrl; /**< @ref wm_uart_flowctrl_t */
} wm_dt_hw_uart_cfg_t;

#if CONFIG_COMPONENT_DRIVER_SEG_LCD_ENABLED
typedef struct {
    uint8_t duty_sel; /**< @ref wm_seg_lcd_duty_sel_t */
    uint8_t vlcd_cc;  /**< @ref wm_seg_lcd_vlcd_cc_t */
    uint8_t bias;     /**< @ref wm_seg_lcd_bias_t */
    uint8_t hd;       /**< @ref wm_seg_lcd_hd_t */
    uint32_t frame_freq;
    uint8_t com_num; /**< @ref wm_seg_lcd_com_id_t */
} wm_dt_hw_seg_lcd_cfg_t;
#endif

typedef struct {
    uint32_t clock_hz; /*< Clock frequency supports 1/2, 1/4, 1/6, 1/8, 1/10, 1/12, 1/14, 1/16 of the CPU clock,
                           it is recommended not to exceed 60MHz, and the CPU clock corresponding to
                           WM_RCC_TYPE_CPU can be obtained through the RCC module. */
    wm_sdh_bus_width_t bus_width;
} wm_dt_hw_sdh_cfg_t;

typedef struct {
    wm_adc_channel_t adc_channel;
    wm_adc_gain1_level_t pga_gain1;
    wm_adc_gain2_level_t pga_gain2;
    uint8_t adc_cmp; /**< Only occure after call wm_drv_adc_start_it*/
    int cmp_data;
    uint8_t cmp_pol; /**< 1: an interrupt is generated when adc_result < cmp_value;
                      *   0: an interrupt is generated when adc_result >= cmp_value */
} wm_dt_hw_adc_cfg_t;

typedef struct {
    wm_dt_hw_irq_cfg_t irq_cfg;
    uint8_t pin_num_start;
    uint8_t pin_num_end;
} wm_dt_hw_gpio_irq_cfg_t;

typedef struct {
    wm_gpio_num_t pin_num;
    wm_gpio_pin_mux_t pin_mux;
    wm_gpio_dir_t pin_dir;
    wm_gpio_pupd_t pin_pupd;
    wm_gpio_intr_mode_t int_mode;
} wm_dt_hw_gpio_cfg_t;

typedef struct {
    uint32_t counter_value;
} wm_dt_hw_wdt_cfg_t;

typedef struct {
    uint32_t reg_base;
    uint8_t pin_num_start;
    uint8_t pin_num_end;
} wm_dt_hw_pin_base_t;

typedef struct {
    wm_dt_hw_init_cfg_t init_cfg;
    uint32_t reg_base;
    wm_dt_hw_irq_cfg_t irq_cfg;
    wm_dt_hw_uart_cfg_t uart_cfg;
    uint8_t pin_cfg_count;
    wm_dt_hw_pin_cfg_t *pin_cfg;

    char *dma_device_name;
    char *rcc_device_name;
} wm_dt_hw_uart_t;

typedef struct {
    uint32_t max_clock;
    bool addr_10_bits;
} wm_dt_hw_i2c_cfg_t;

typedef struct {
    wm_dt_hw_init_cfg_t init_cfg;
    uint32_t reg_base;

    wm_dt_hw_i2c_cfg_t i2c_cfg;

    uint8_t pin_cfg_count;
    wm_dt_hw_pin_cfg_t *pin_cfg;

    wm_dt_hw_irq_cfg_t irq_cfg;

    char *dma_device_name;
    char *rcc_device_name;

} wm_dt_hw_i2c_t;

typedef struct {
    wm_dt_hw_init_cfg_t init_cfg;
    uint8_t pin_cfg_count;
    wm_dt_hw_pin_cfg_t *pin_cfg;

    char *i2c_device_name;

    uint32_t speed_hz; /**< Define the clock speed for EEPROM used by i2c interface, unit:Hz,
                        *   This speed must less than max_clock defined in @ref wm_dt_hw_i2c_cfg_t */
    uint32_t size;
    uint16_t i2c_addr;
    uint16_t page_size;
    uint8_t addr_width;
    uint8_t read_only;
    uint16_t max_write_time_ms;
} wm_dt_hw_eeprom_t;

typedef struct {
    uint8_t spi_mode;  /**< Define SPI Mode, 0:spi, 1:qspi */
    uint32_t clock_hz; /**< Define the clock support for pSRAM controller, unit:Hz
                         *   psram clock = CPU clock / div, div is in [3,15] and the max psram clock is 80000000(80MHz)*/
} wm_dt_hw_psram_cfg_t;

typedef struct {
    wm_dt_hw_init_cfg_t init_cfg;
    uint32_t reg_base;
    wm_dt_hw_psram_cfg_t psram_cfg;
    uint8_t pin_cfg_count;
    wm_dt_hw_pin_cfg_t *pin_cfg;
    char *dma_device_name;
    char *rcc_device_name;
} wm_dt_hw_psram_t;

typedef struct {
    wm_dt_hw_init_cfg_t init_cfg;
    uint32_t reg_base;
    wm_dt_hw_irq_cfg_t irq_cfg;
} wm_dt_hw_timer_t;

typedef struct {
    wm_dt_hw_init_cfg_t init_cfg;
    wm_dt_hw_pin_cfg_t *pin_cfg;
    wm_dt_hw_irq_cfg_t irq_cfg;
    uint32_t reg_base;
    uint8_t pin_cfg_count;
    char *dma_device_name;
} wm_dt_hw_pwm_t;

typedef struct {
    wm_dt_hw_init_cfg_t init_cfg;
    wm_dt_hw_irq_cfg_t irq_cfg;
    wm_dt_hw_pin_cfg_t *pin_cfg;
    uint32_t reg_base;
    uint8_t pin_cfg_count;
    char *dma_device_name;
    char *rcc_device_name;
    wm_dt_hw_i2s_cfg_t i2s_cfg;
} wm_dt_hw_i2s_t;

typedef struct {
    wm_dt_hw_init_cfg_t init_cfg;
    wm_dt_hw_codec_es8374_cfg_t es8374_cfg;
    char *i2s_device_name;
    char *gpio_device_name;
    char *i2c_device_name;
    uint8_t gpio1;
    uint8_t gpio2;
} wm_dt_hw_codec_i2s_t;

#if CONFIG_COMPONENT_DRIVER_SEG_LCD_ENABLED
typedef struct {
    wm_dt_hw_init_cfg_t init_cfg;
    uint32_t reg_base;
    wm_dt_hw_seg_lcd_cfg_t seg_lcd_cfg;
    char *rcc_device_name;
} wm_dt_hw_seg_lcd_t;
#endif

typedef struct {
    wm_dt_hw_init_cfg_t init_cfg;
    uint8_t pin_cfg_count;
    wm_dt_hw_pin_cfg_t *pin_cfg;
    char *seg_lcd_device_name;
} wm_dt_hw_gdc0689_t;

typedef struct {
    wm_dt_hw_init_cfg_t init_cfg;
    uint32_t reg_base;
    wm_dt_hw_sdh_cfg_t sdh_cfg;
    wm_dt_hw_irq_cfg_t irq_cfg;
    uint8_t pin_cfg_count;
    wm_dt_hw_pin_cfg_t *pin_cfg;
    char *dma_device_name;
    char *rcc_device_name;
} wm_dt_hw_sdh_t;

typedef struct {
    wm_dt_hw_init_cfg_t init_cfg;
    wm_dt_hw_spim_dev_cfg_t spi_cfg;
    wm_gpio_num_t io_lcd_reset;
    wm_gpio_num_t io_lcd_led;
    wm_gpio_num_t io_lcd_dcx;
    wm_gpio_num_t io_lcd_te;
    char *spi_device_name;
    char *gpio_device_name;
} wm_dt_hw_tft_lcd_spi_t;

typedef struct {
    wm_dt_hw_init_cfg_t init_cfg;
    uint32_t reg_base;
    wm_dt_hw_irq_cfg_t irq_cfg;
} wm_dt_hw_rtc_t;

typedef struct {
    wm_dt_hw_init_cfg_t init_cfg;
    uint32_t reg_base;
    wm_pmu_clock_source_t clk_src; /**< @ref wm_pmu_clock_source_t, the clock source index used by PMU */
    wm_dt_hw_irq_cfg_t irq_cfg;
} wm_dt_hw_pmu_t;

typedef struct {
    wm_dt_hw_init_cfg_t init_cfg;
    uint32_t reg_base;
    uint8_t adc_cfg_count;
    wm_dt_hw_irq_cfg_t irq_cfg;
    wm_dt_hw_adc_cfg_t *adc_cfg;
    uint8_t pin_cfg_count;
    wm_dt_hw_pin_cfg_t *pin_cfg;
    char *dma_device_name;
    char *rcc_device_name;
} wm_dt_hw_adc_t;

typedef struct {
    wm_dt_hw_init_cfg_t init_cfg;
    uint8_t pin_base_count;
    wm_dt_hw_pin_base_t *pin_base;
    uint8_t irq_cfg_count;
    wm_dt_hw_gpio_irq_cfg_t *irq_cfg;
    uint8_t gpio_cfg_count;
    wm_dt_hw_gpio_cfg_t *gpio_cfg;
} wm_dt_hw_gpio_t;

typedef struct {
    wm_dt_hw_init_cfg_t init_cfg;
    uint32_t reg_base;
    wm_dt_hw_wdt_cfg_t wdt_cfg;
    wm_dt_hw_irq_cfg_t irq_cfg;
} wm_dt_hw_wdt_t;

typedef struct {
    wm_dt_hw_init_cfg_t init_cfg;
    uint32_t reg_base;
    uint8_t irq_cfg_count;
    wm_dt_hw_irq_cfg_t irq_cfg;
} wm_dt_hw_dma_t;

typedef struct {
    wm_dt_hw_init_cfg_t init_cfg;
    uint32_t reg_base;
    uint8_t pin_cfg_count;
    wm_dt_hw_pin_cfg_t *pin_cfg;
    wm_dt_hw_irq_cfg_t irq_cfg;
    char *dma_device_name;
    char *rcc_device_name;
} wm_dt_hw_spim_t;

typedef struct {
    wm_rcc_type_t type; /**< @ref wm_rcc_type_t */
    uint32_t clock;     /**< Define the clock for each of bus/controller, unit:Hz,
                         *   The range is same as that defined by \c wm_drv_rcc_config_clock()*/
} wm_dt_hw_rcc_cfg_t;

typedef struct {
    wm_dt_hw_init_cfg_t init_cfg;
    uint32_t reg_base;
    uint8_t rcc_cfg_count;
    wm_dt_hw_rcc_cfg_t *rcc_cfg;
} wm_dt_hw_rcc_t;

typedef struct {
    bool quad_spi;
} wm_dt_hw_flash_cfg_t;

typedef struct {
    wm_dt_hw_init_cfg_t init_cfg;
    uint32_t reg_base;
    wm_dt_hw_flash_cfg_t flash_cfg;
} wm_dt_hw_iflash_t; //internal flash

typedef struct {
    wm_dt_hw_init_cfg_t init_cfg;
    wm_dt_hw_flash_cfg_t flash_cfg;
    char *spi_device_name;
    wm_dt_hw_spim_dev_cfg_t spi_cfg;
} wm_dt_hw_eflash_t; //external flash

typedef struct {
    wm_dt_hw_init_cfg_t init_cfg;

    uint32_t reg_base;

    uint8_t pin_cfg_count;
    wm_dt_hw_pin_cfg_t *pin_cfg;

    wm_dt_hw_irq_cfg_t irq_cfg;

    char *rcc_device_name;
} wm_dt_hw_touch_sensor_t;

typedef struct {
    uint8_t key_num;
    uint32_t threshold;
} wm_dt_hw_touch_button_cfg_t;

typedef struct {
    wm_dt_hw_init_cfg_t init_cfg;

    uint8_t touch_button_cfg_count;
    wm_dt_hw_touch_button_cfg_t *touch_button_cfg;

    uint8_t pin_cfg_count;
    wm_dt_hw_pin_cfg_t *pin_cfg;

    char *touch_sensor_device_name;
} wm_dt_hw_touch_button_t;

#if CONFIG_COMPONENT_DRIVER_CRC_ENABLED
//typedef wm_hal_crc_cfg_t wm_dt_hw_crc_cfg_t;

typedef struct {
    wm_dt_hw_init_cfg_t init_cfg;

    uint32_t reg_base;
    //wm_dt_hw_crc_cfg_t crc_cfg;
    char *rcc_device_name;
    wm_dt_hw_irq_cfg_t irq_cfg;
} wm_dt_hw_crc_t;
#endif

#if CONFIG_COMPONENT_DRIVER_CRYPTO_ENABLED
//typedef wm_hal_cryp_cfg_t wm_dt_hw_cryp_cfg_t;

typedef struct {
    wm_dt_hw_init_cfg_t init_cfg;

    uint32_t reg_base;
    //wm_dt_hw_cryp_cfg_t cryp_cfg;
    char *rcc_device_name;
    wm_dt_hw_irq_cfg_t irq_cfg;
} wm_dt_hw_crypto_t;
#endif

#if CONFIG_COMPONENT_DRIVER_HASH_ENABLED
//typedef wm_hal_hash_cfg_t wm_dt_hw_hash_cfg_t;

typedef struct {
    wm_dt_hw_init_cfg_t init_cfg;

    uint32_t reg_base;
    //wm_dt_hw_hash_cfg_t hash_cfg;
    char *rcc_device_name;
    wm_dt_hw_irq_cfg_t irq_cfg;
} wm_dt_hw_hash_t;
#endif

#if CONFIG_COMPONENT_DRIVER_RNG_ENABLED
//typedef wm_hal_rng_cfg_t wm_dt_hw_rng_cfg_t;

typedef struct {
    wm_dt_hw_init_cfg_t init_cfg;

    uint32_t reg_base;
    //wm_dt_hw_rng_cfg_t rng_cfg;
    char *rcc_device_name;
    wm_dt_hw_irq_cfg_t irq_cfg;
} wm_dt_hw_rng_t;
#endif

#if CONFIG_COMPONENT_DRIVER_RSA_ENABLED
typedef struct {
    wm_dt_hw_init_cfg_t init_cfg;

    uint32_t reg_base;
    char *rcc_device_name;
    wm_dt_hw_irq_cfg_t irq_cfg;
} wm_dt_hw_rsa_t;
#endif

#if CONFIG_COMPONENT_DRIVER_SDIO_SLAVE_ENABLED
typedef struct {
    wm_dt_hw_init_cfg_t init_cfg;
    uint32_t sdio_slave_reg_base;
    uint32_t wrapper_reg_base;
    wm_dt_hw_irq_cfg_t irq_cfg;
    uint8_t pin_cfg_count;
    wm_dt_hw_pin_cfg_t *pin_cfg;
    char *rcc_device_name;
} wm_dt_hw_sdio_slave_t;
#endif

#pragma pack()

#ifdef __cplusplus
}
#endif

#endif /* __WM_DT_HW_H__ */
