list(APPEND ADD_INCLUDE "include"
                        "src/${chip_family_type}/wm_ll/include"
                        )

aux_source_directory(src/${chip_family_type}/wm_ll/src ADD_SRCS)

if(${CONFIG_COMPONENT_DRIVER_IRQ_ENABLED})
    list(APPEND ADD_SRCS "src/${chip_family_type}/wm_hal_irq.c"
                         )
endif()

if(${CONFIG_COMPONENT_DRIVER_UART_ENABLED})
    list(APPEND ADD_SRCS "src/${chip_family_type}/wm_hal_uart.c"
                         )
endif()

if(${CONFIG_COMPONENT_DRIVER_TIMER_ENABLED})
    list(APPEND ADD_SRCS "src/${chip_family_type}/wm_hal_timer.c"
                         )
endif()

if(${CONFIG_COMPONENT_DRIVER_PWM_ENABLED})
    list(APPEND ADD_SRCS "src/${chip_family_type}/wm_hal_pwm.c"
                         )
endif()

if(${CONFIG_COMPONENT_DRIVER_I2S_ENABLED})
    list(APPEND ADD_SRCS "src/${chip_family_type}/wm_hal_i2s.c"
                         )
endif()

if(${CONFIG_COMPONENT_DRIVER_SEG_LCD_ENABLED})
    list(APPEND ADD_SRCS "src/${chip_family_type}/wm_hal_seg_lcd.c"
                         )
endif()

if(CONFIG_COMPONENT_DRIVER_SDMMC_ENABLED OR CONFIG_COMPONENT_DRIVER_SDSPI_ENABLED)
    list(APPEND ADD_SRCS "src/${chip_family_type}/wm_hal_sdh.c"
                         )
endif()

if(${CONFIG_COMPONENT_DRIVER_RTC_ENABLED})
    list(APPEND ADD_SRCS "src/${chip_family_type}/wm_hal_rtc.c"
                         )
endif()

if(${CONFIG_COMPONENT_DRIVER_PMU_ENABLED})
    list(APPEND ADD_SRCS "src/${chip_family_type}/wm_hal_pmu.c"
                         )
endif()

if(${CONFIG_COMPONENT_DRIVER_ADC_ENABLED})
    list(APPEND ADD_SRCS "src/${chip_family_type}/wm_hal_adc.c"
                         )
endif()

if(${CONFIG_COMPONENT_DRIVER_GPIO_ENABLED})
    list(APPEND ADD_SRCS "src/${chip_family_type}/wm_hal_gpio.c"
                         )
endif()

if(${CONFIG_COMPONENT_DRIVER_WDT_ENABLED})
    list(APPEND ADD_SRCS "src/${chip_family_type}/wm_hal_wdt.c"
                         )
endif()

if(${CONFIG_COMPONENT_DRIVER_DMA_ENABLED})
    list(APPEND ADD_SRCS "src/${chip_family_type}/wm_hal_dma.c"
                         )
endif()

if(${CONFIG_COMPONENT_DRIVER_I2C_ENABLED})
    list(APPEND ADD_SRCS "src/${chip_family_type}/wm_hal_i2c.c"
                         )
endif()

if(${CONFIG_COMPONENT_DRIVER_PSRAM_ENABLED})
    list(APPEND ADD_SRCS "src/${chip_family_type}/wm_hal_psram.c"
                         )
endif()

if(${CONFIG_COMPONENT_DRIVER_SPIM_ENABLED})
    list(APPEND ADD_SRCS "src/${chip_family_type}/wm_hal_spi_master.c"
                         )
endif()

if(${CONFIG_COMPONENT_DRIVER_RCC_ENABLED})
    list(APPEND ADD_SRCS "src/${chip_family_type}/wm_hal_rcc.c"
                         )
endif()

if(CONFIG_COMPONENT_DRIVER_INTERNAL_FLASH_ENABLED OR CONFIG_COMPONENT_DRIVER_EXTERNAL_FLASH_ENABLED)
    list(APPEND ADD_SRCS "src/${chip_family_type}/wm_hal_flash.c"
                         )
endif()

if(${CONFIG_COMPONENT_DRIVER_TOUCH_SENSOR_ENABLED})
    list(APPEND ADD_SRCS "src/${chip_family_type}/wm_hal_touch_sensor.c"
                         )
endif()

if(${CONFIG_COMPONENT_DRIVER_CRC_ENABLED})
    list(APPEND ADD_SRCS "src/${chip_family_type}/wm_hal_crc.c"
                         )
endif()

if(${CONFIG_COMPONENT_DRIVER_CRYPTO_ENABLED})
    list(APPEND ADD_SRCS "src/${chip_family_type}/wm_hal_crypto.c"
                         )
endif()

if(${CONFIG_COMPONENT_DRIVER_HASH_ENABLED})
    list(APPEND ADD_SRCS "src/${chip_family_type}/wm_hal_hash.c"
                         )
endif()

if(${CONFIG_COMPONENT_DRIVER_RNG_ENABLED})
    list(APPEND ADD_SRCS "src/${chip_family_type}/wm_hal_rng.c"
                         )
endif()

if(${CONFIG_COMPONENT_DRIVER_RSA_ENABLED})
    list(APPEND ADD_SRCS "src/${chip_family_type}/wm_hal_rsa.c"
                         )
endif()


if(${CONFIG_COMPONENT_DRIVER_SDIO_SLAVE_ENABLED})
    list(APPEND ADD_SRCS "src/${chip_family_type}/wm_hal_sdio_slave.c"
                         )
endif()

set(not_touch_sensor 0)
if(${CONFIG_BUILD_TYPE_W800})
    set(not_touch_sensor 1)
elseif(${CONFIG_BUILD_TYPE_W801})
    set(not_touch_sensor 1)
elseif(${CONFIG_BUILD_TYPE_W805})
    set(not_touch_sensor 1)
endif()
if(${not_touch_sensor})
        list(REMOVE_ITEM ADD_SRCS "src/${chip_family_type}/wm_hal_touch_sensor.c"
                                  )
endif()

register_component()
