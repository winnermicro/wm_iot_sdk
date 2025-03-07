list(APPEND ADD_INCLUDE "include"
                        "include/touch_sensor"
                        "include/display/seg_lcd"
                        "include/display/tft_lcd"
                        "include/codec/i2s"
                        "include/eeprom"
                        )

list(APPEND ADD_PRIVATE_INCLUDE "src/ops/${CONFIG_CHIP_FAMILY_NAME}"
                                "src/ops/${CONFIG_CHIP_FAMILY_NAME}/touch_sensor"
                                )

if(${CONFIG_COMPONENT_DRIVER_IRQ_ENABLED})
    list(APPEND ADD_SRCS "src/wm_drv_irq.c"
                         )
endif()

if(${CONFIG_COMPONENT_DRIVER_UART_ENABLED})
    list(APPEND ADD_SRCS "src/wm_drv_uart.c"
                         )
endif()

if(${CONFIG_COMPONENT_DRIVER_TIMER_ENABLED})
    list(APPEND ADD_SRCS "src/wm_drv_timer.c"
                         )
endif()

if(${CONFIG_COMPONENT_DRIVER_PWM_ENABLED})
    list(APPEND ADD_SRCS "src/wm_drv_pwm.c"
                         )
endif()

if(${CONFIG_COMPONENT_DRIVER_I2S_ENABLED})
    list(APPEND ADD_SRCS "src/wm_drv_i2s.c"
                         )
endif()

if(${CONFIG_COMPONENT_DRIVER_SEG_LCD_ENABLED})
    list(APPEND ADD_SRCS "src/wm_drv_seg_lcd.c"
                         "src/display/seg_lcd/wm_gdc0689.c"
                         )
endif()

if(${CONFIG_COMPONENT_DRIVER_SDMMC_ENABLED})
    list(APPEND ADD_SRCS "src/wm_drv_sdh_sdmmc.c"
                         )
endif()

if(${CONFIG_COMPONENT_DRIVER_SDSPI_ENABLED})
    list(APPEND ADD_SRCS "src/wm_drv_sdh_spi.c"
                         )
endif()

if(${CONFIG_COMPONENT_DRIVER_TFT_LCD_ENABLED})
    list(APPEND ADD_SRCS "src/wm_drv_tft_lcd.c"
                         "src/display/tft_lcd/wm_drv_ops_tft_lcd_spi.c"
                         "src/display/tft_lcd/wm_drv_ops_tft_lcd_nv3041a_spi.c"
                         "src/display/tft_lcd/wm_drv_ops_tft_lcd_st7735_spi.c"
                         "src/display/tft_lcd/wm_drv_ops_tft_lcd_gz035_spi.c"
                         )
endif()

if(${CONFIG_COMPONENT_DRIVER_RTC_ENABLED})
    list(APPEND ADD_SRCS "src/wm_drv_rtc.c"
                         )
endif()

if(${CONFIG_COMPONENT_DRIVER_PMU_ENABLED})
    list(APPEND ADD_SRCS "src/wm_drv_pmu.c"
                         )
endif()

if(${CONFIG_COMPONENT_DRIVER_ADC_ENABLED})
    list(APPEND ADD_SRCS "src/wm_drv_adc.c"
                         )
endif()

if(${CONFIG_COMPONENT_DRIVER_GPIO_ENABLED})
    list(APPEND ADD_SRCS "src/wm_drv_gpio.c"
                         )
endif()

if(${CONFIG_COMPONENT_DRIVER_WDT_ENABLED})
    list(APPEND ADD_SRCS "src/wm_drv_wdt.c"
                         )
endif()

if(${CONFIG_COMPONENT_DRIVER_DMA_ENABLED})
    list(APPEND ADD_SRCS "src/wm_drv_dma.c"
                         )
endif()

if(${CONFIG_COMPONENT_DRIVER_I2C_ENABLED})
    list(APPEND ADD_SRCS "src/wm_drv_i2c.c"
                         "src/eeprom/wm_drv_eeprom.c"
                         "src/eeprom/wm_drv_eeprom_nv24c0x.c"
                         )
endif()

if(${CONFIG_COMPONENT_DRIVER_PSRAM_ENABLED})
    list(APPEND ADD_SRCS "src/wm_drv_psram.c"
                         )
endif()

if(${CONFIG_COMPONENT_DRIVER_SPIM_ENABLED})
    list(APPEND ADD_SRCS "src/wm_drv_spi_master.c"
                         )
endif()

if(${CONFIG_COMPONENT_DRIVER_RCC_ENABLED})
    list(APPEND ADD_SRCS "src/wm_drv_rcc.c"
                         )
endif()

if(CONFIG_COMPONENT_DRIVER_INTERNAL_FLASH_ENABLED OR CONFIG_COMPONENT_DRIVER_EXTERNAL_FLASH_ENABLED)
    list(APPEND ADD_SRCS "src/wm_drv_flash.c"
                         )
endif()

if(${CONFIG_COMPONENT_DRIVER_TOUCH_SENSOR_ENABLED})
    list(APPEND ADD_SRCS "src/wm_drv_touch_sensor.c"
                         "src/touch_sensor/wm_drv_touch_button.c"
                         )
endif()

if(${CONFIG_COMPONENT_DRIVER_CRC_ENABLED})
    list(APPEND ADD_SRCS "src/wm_drv_crc.c"
                         )
endif()

if(${CONFIG_COMPONENT_DRIVER_CRYPTO_ENABLED})
    list(APPEND ADD_SRCS "src/wm_drv_crypto.c"
                         )
endif()

if(${CONFIG_COMPONENT_DRIVER_HASH_ENABLED})
    list(APPEND ADD_SRCS "src/wm_drv_hash.c"
                         )
endif()

if(${CONFIG_COMPONENT_DRIVER_RNG_ENABLED})
    list(APPEND ADD_SRCS "src/wm_drv_rng.c"
                         )
endif()

if(${CONFIG_COMPONENT_DRIVER_RSA_ENABLED})
    list(APPEND ADD_SRCS "src/wm_drv_rsa.c"
                         )
endif()

if(${CONFIG_COMPONENT_DRIVER_CODEC_ES8374_ENABLED})
    list(APPEND ADD_SRCS "src/codec/i2s/wm_drv_ops_codec_es8374.c"
                         "src/wm_drv_codec_i2s.c"
                         )
endif()

if(${CONFIG_COMPONENT_DRIVER_SDIO_SLAVE_ENABLED})
    list(APPEND ADD_SRCS "src/wm_drv_sdio_slave.c"
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
    list(REMOVE_ITEM ADD_INCLUDE "include/touch_sensor"
                                 )

    list(REMOVE_ITEM ADD_PRIVATE_INCLUDE "src/ops/${CONFIG_CHIP_FAMILY_NAME}/touch_sensor"
                                         )

        list(REMOVE_ITEM ADD_SRCS "src/wm_drv_touch_sensor.c"
                                  "src/touch_sensor/wm_drv_touch_button.c"
                                  "src/ops/${CONFIG_CHIP_FAMILY_NAME}/wm_drv_ops_touch_sensor.c"
                                  )
endif()

register_component()
