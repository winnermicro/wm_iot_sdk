/**
 * @file wm_io_mux.h
 *
 * @brief  iomux module
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

#ifndef __WM_IO_MUX_H__
#define __WM_IO_MUX_H__

#include "wm_soc_cfgs.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
* The iomux of the W800 chip can be divided into digital multiplexing and analog multiplexing,
* where 1-4 is digital multiplexing, 5 is GPIO, and 6-7 is analog multiplexing. If digital multiplexing is used,
* please set the bit corresponding to the WM_GPIO_FUNC_SHIFT of cfg to 1-4 in the "int wm_hal_gpio_init (wm_gpio_pin'num_t pin, uint32_t cfg)" function.
* and If analog multiplexing is required, do not set  WM_GPIO_FUNC_SHIFT and set cfg to WM_HAL_GPIO-FLOAT | WM_GPIO-INPUT
*/

#define WM_IO_MUX_GPIO0                      WM_GPIO_NUM_0
#define WM_IO_MUX_GPIO0_FUNC_I2S_MCLK        1
#define WM_IO_MUX_GPIO0_FUNC_LSPI_CS         2
#define WM_IO_MUX_GPIO0_FUNC_PWM2            3
#define WM_IO_MUX_GPIO0_FUNC_I2S_DO          4
#define WM_IO_MUX_GPIO0_FUNC_GPIO            5
#define WM_IO_MUX_GPIO0_FUNC_BOOTMODE        6

#define WM_IO_MUX_GPIO1                      WM_GPIO_NUM_1
#define WM_IO_MUX_GPIO1_FUNC_JATG_CK         1
#define WM_IO_MUX_GPIO1_FUNC_I2C_SCL         2
#define WM_IO_MUX_GPIO1_FUNC_PWM3            3
#define WM_IO_MUX_GPIO1_FUNC_I2S_LRCLK       4
#define WM_IO_MUX_GPIO1_FUNC_GPIO            5
#define WM_IO_MUX_GPIO1_FUNC_ADC_0           6

#define WM_IO_MUX_GPIO2                      WM_GPIO_NUM_2
#define WM_IO_MUX_GPIO2_FUNC_UART1_RTS       1
#define WM_IO_MUX_GPIO2_FUNC_UART2_TX        2
#define WM_IO_MUX_GPIO2_FUNC_PWM0            3
#define WM_IO_MUX_GPIO2_FUNC_UART3_RTS       4
#define WM_IO_MUX_GPIO2_FUNC_GPIO            5
#define WM_IO_MUX_GPIO2_FUNC_ADC_3           6

#define WM_IO_MUX_GPIO3                      WM_GPIO_NUM_3
#define WM_IO_MUX_GPIO3_FUNC_UART1_CTS       1
#define WM_IO_MUX_GPIO3_FUNC_UART2_RX        2
#define WM_IO_MUX_GPIO3_FUNC_PWM1            3
#define WM_IO_MUX_GPIO3_FUNC_UART3_CTS       4
#define WM_IO_MUX_GPIO3_FUNC_GPIO            5
#define WM_IO_MUX_GPIO3_FUNC_ADC_2           6

#define WM_IO_MUX_GPIO4                      WM_GPIO_NUM_4
#define WM_IO_MUX_GPIO4_FUNC_JATG_SWO        1
#define WM_IO_MUX_GPIO4_FUNC_I2C_SDA         2
#define WM_IO_MUX_GPIO4_FUNC_PWM4            3
#define WM_IO_MUX_GPIO4_FUNC_I2S_BCK         4
#define WM_IO_MUX_GPIO4_FUNC_GPIO            5
#define WM_IO_MUX_GPIO4_FUNC_ADC_1           6

#define WM_IO_MUX_GPIO5                      WM_GPIO_NUM_5
#define WM_IO_MUX_GPIO5_FUNC_UART3_TX        1
#define WM_IO_MUX_GPIO5_FUNC_UART2_RTS       2
#define WM_IO_MUX_GPIO5_FUNC_PWM_BREAK       3
#define WM_IO_MUX_GPIO5_FUNC_UART4_RTS       4
#define WM_IO_MUX_GPIO5_FUNC_GPIO            5

#define WM_IO_MUX_GPIO6                      WM_GPIO_NUM_6
#define WM_IO_MUX_GPIO6_FUNC_UART3_RX        1
#define WM_IO_MUX_GPIO6_FUNC_UART2_CTS       2
#define WM_IO_MUX_GPIO6_FUNC_UART4_CTS       4
#define WM_IO_MUX_GPIO6_FUNC_GPIO            5
#define WM_IO_MUX_GPIO6_FUNC_LCD_SEG31       6
#define WM_IO_MUX_GPIO6_FUNC_VRP_EXT         7

#define WM_IO_MUX_GPIO7                      WM_GPIO_NUM_7
#define WM_IO_MUX_GPIO7_FUNC_PWM4            1
#define WM_IO_MUX_GPIO7_FUNC_LSPI_MOSI       2
#define WM_IO_MUX_GPIO7_FUNC_I2S_MCK         3
#define WM_IO_MUX_GPIO7_FUNC_I2S_DI          4
#define WM_IO_MUX_GPIO7_FUNC_GPIO            5
#define WM_IO_MUX_GPIO7_FUNC_LCD_SEG3        6

#define WM_IO_MUX_GPIO8                      WM_GPIO_NUM_8
#define WM_IO_MUX_GPIO8_FUNC_PWM_BREAK       1
#define WM_IO_MUX_GPIO8_FUNC_UART4_TX        2
#define WM_IO_MUX_GPIO8_FUNC_UART5_TX        3
#define WM_IO_MUX_GPIO8_FUNC_I2S_BCLK        4
#define WM_IO_MUX_GPIO8_FUNC_GPIO            5
#define WM_IO_MUX_GPIO8_FUNC_LCD_SEG4        6

#define WM_IO_MUX_GPIO9                      WM_GPIO_NUM_9
#define WM_IO_MUX_GPIO9_FUNC_MMC_CLK         1
#define WM_IO_MUX_GPIO9_FUNC_UART4_RX        2
#define WM_IO_MUX_GPIO9_FUNC_UART5_RX        3
#define WM_IO_MUX_GPIO9_FUNC_I2S_LRCLK       4
#define WM_IO_MUX_GPIO9_FUNC_GPIO            5
#define WM_IO_MUX_GPIO9_FUNC_LCD_SEG5        6
#define WM_IO_MUX_GPIO9_FUNC_TOUCH1          7

#define WM_IO_MUX_GPIO10                     WM_GPIO_NUM_10
#define WM_IO_MUX_GPIO10_FUNC_MMC_CMD        1
#define WM_IO_MUX_GPIO10_FUNC_UART4_RTS      2
#define WM_IO_MUX_GPIO10_FUNC_PWM0           3
#define WM_IO_MUX_GPIO10_FUNC_I2S_DO         4
#define WM_IO_MUX_GPIO10_FUNC_GPIO           5
#define WM_IO_MUX_GPIO10_FUNC_LCD_SEG6       6
#define WM_IO_MUX_GPIO10_FUNC_TOUCH2         7

#define WM_IO_MUX_GPIO11                     WM_GPIO_NUM_11
#define WM_IO_MUX_GPIO11_FUNC_MMC_DAT0       1
#define WM_IO_MUX_GPIO11_FUNC_UART4_CTS      2
#define WM_IO_MUX_GPIO11_FUNC_PWM1           3
#define WM_IO_MUX_GPIO11_FUNC_I2S_DI         4
#define WM_IO_MUX_GPIO11_FUNC_GPIO           5
#define WM_IO_MUX_GPIO11_FUNC_LCD_SEG7       6

#define WM_IO_MUX_GPIO12                     WM_GPIO_NUM_12
#define WM_IO_MUX_GPIO12_FUNC_MMC_DAT1       1
#define WM_IO_MUX_GPIO12_FUNC_UART5_TX       2
#define WM_IO_MUX_GPIO12_FUNC_PWM2           3
#define WM_IO_MUX_GPIO12_FUNC_GPIO           5
#define WM_IO_MUX_GPIO12_FUNC_LCD_SEG8       6
#define WM_IO_MUX_GPIO12_FUNC_TOUCH_CAP_CMOD 7

#define WM_IO_MUX_GPIO13                     WM_GPIO_NUM_13
#define WM_IO_MUX_GPIO13_FUNC_MMC_DAT2       1
#define WM_IO_MUX_GPIO13_FUNC_UART5_RX       2
#define WM_IO_MUX_GPIO13_FUNC_PWM3           3
#define WM_IO_MUX_GPIO13_FUNC_GPIO           5
#define WM_IO_MUX_GPIO13_FUNC_LCD_SEG9       6

#define WM_IO_MUX_GPIO14                     WM_GPIO_NUM_14
#define WM_IO_MUX_GPIO14_FUNC_MMC_DAT3       1
#define WM_IO_MUX_GPIO14_FUNC_UART5_CTS      2
#define WM_IO_MUX_GPIO14_FUNC_PWM4           3
#define WM_IO_MUX_GPIO14_FUNC_GPIO           5
#define WM_IO_MUX_GPIO14_FUNC_LCD_SEG10      6
#define WM_IO_MUX_GPIO14_FUNC_TOUCH_CAP_CDC  7

#define WM_IO_MUX_GPIO15                     WM_GPIO_NUM_15
#define WM_IO_MUX_GPIO15_FUNC_PSRAM_CK       1
#define WM_IO_MUX_GPIO15_FUNC_UART5_RTS      2
#define WM_IO_MUX_GPIO15_FUNC_PWM_BREAK      3
#define WM_IO_MUX_GPIO15_FUNC_GPIO           5
#define WM_IO_MUX_GPIO15_FUNC_LCD_SEG11      6

#define WM_IO_MUX_GPIO16                     WM_GPIO_NUM_16
#define WM_IO_MUX_GPIO16_FUNC_PWM0           1
#define WM_IO_MUX_GPIO16_FUNC_LSPI_MISO      2
#define WM_IO_MUX_GPIO16_FUNC_UART3_TX       3
#define WM_IO_MUX_GPIO16_FUNC_PSRAM_CK       4
#define WM_IO_MUX_GPIO16_FUNC_GPIO           5
#define WM_IO_MUX_GPIO16_FUNC_LCD_SEG12      6
#define WM_IO_MUX_GPIO16_FUNC_TOUCH3         7

#define WM_IO_MUX_GPIO17                     WM_GPIO_NUM_17
#define WM_IO_MUX_GPIO17_FUNC_PWM1           1
#define WM_IO_MUX_GPIO17_FUNC_LSPI_CK        2
#define WM_IO_MUX_GPIO17_FUNC_UART3_RX       3
#define WM_IO_MUX_GPIO17_FUNC_PSRAM_CS       4
#define WM_IO_MUX_GPIO17_FUNC_GPIO           5
#define WM_IO_MUX_GPIO17_FUNC_LCD_SEG13      6
#define WM_IO_MUX_GPIO17_FUNC_TOUCH4         7

#define WM_IO_MUX_GPIO18                     WM_GPIO_NUM_18
#define WM_IO_MUX_GPIO18_FUNC_PWM2           1
#define WM_IO_MUX_GPIO18_FUNC_LSPI_CK        2
#define WM_IO_MUX_GPIO18_FUNC_UART2_TX       3
#define WM_IO_MUX_GPIO18_FUNC_PSRAM_D0       4
#define WM_IO_MUX_GPIO18_FUNC_GPIO           5
#define WM_IO_MUX_GPIO18_FUNC_LCD_SEG14      6
#define WM_IO_MUX_GPIO18_FUNC_TOUCH5         7

#define WM_IO_MUX_GPIO19                     WM_GPIO_NUM_19
#define WM_IO_MUX_GPIO19_FUNC_PWM3           1
#define WM_IO_MUX_GPIO19_FUNC_LSPI_MISO      2
#define WM_IO_MUX_GPIO19_FUNC_UART2_RX       3
#define WM_IO_MUX_GPIO19_FUNC_PSRAM_D1       4
#define WM_IO_MUX_GPIO19_FUNC_GPIO           5
#define WM_IO_MUX_GPIO19_FUNC_LCD_SEG15      6
#define WM_IO_MUX_GPIO19_FUNC_TOUCH6         7

#define WM_IO_MUX_GPIO20                     WM_GPIO_NUM_20
#define WM_IO_MUX_GPIO20_FUNC_LSPI_CS        1
#define WM_IO_MUX_GPIO20_FUNC_UART2_RTS      2
#define WM_IO_MUX_GPIO20_FUNC_UART4_TX       3
#define WM_IO_MUX_GPIO20_FUNC_PSRAM_D2       4
#define WM_IO_MUX_GPIO20_FUNC_GPIO           5
#define WM_IO_MUX_GPIO20_FUNC_LCD_SEG16      6
#define WM_IO_MUX_GPIO20_FUNC_TOUCH7         7

#define WM_IO_MUX_GPIO21                     WM_GPIO_NUM_21
#define WM_IO_MUX_GPIO21_FUNC_LSPI_MOSI      1
#define WM_IO_MUX_GPIO21_FUNC_UART2_CTS      2
#define WM_IO_MUX_GPIO21_FUNC_UART4_RX       3
#define WM_IO_MUX_GPIO21_FUNC_PSRAM_D3       4
#define WM_IO_MUX_GPIO21_FUNC_GPIO           5
#define WM_IO_MUX_GPIO21_FUNC_LCD_SEG17      6
#define WM_IO_MUX_GPIO21_FUNC_TOUCH8         7

#define WM_IO_MUX_GPIO22                     WM_GPIO_NUM_22
#define WM_IO_MUX_GPIO22_FUNC_UART1_TX       1
#define WM_IO_MUX_GPIO22_FUNC_MMC_CLK        2
#define WM_IO_MUX_GPIO22_FUNC_HSPI_CK        3
#define WM_IO_MUX_GPIO22_FUNC_SDIO_CK        4
#define WM_IO_MUX_GPIO22_FUNC_GPIO           5
#define WM_IO_MUX_GPIO22_FUNC_LCD_SEG18      6
#define WM_IO_MUX_GPIO22_FUNC_TOUCH9         7

#define WM_IO_MUX_GPIO23                     WM_GPIO_NUM_23
#define WM_IO_MUX_GPIO23_FUNC_UART1_RX       1
#define WM_IO_MUX_GPIO23_FUNC_MMC_CMD        2
#define WM_IO_MUX_GPIO23_FUNC_HSPI_INT       3
#define WM_IO_MUX_GPIO23_FUNC_SDIO_CMD       4
#define WM_IO_MUX_GPIO23_FUNC_GPIO           5
#define WM_IO_MUX_GPIO23_FUNC_LCD_SEG19      6
#define WM_IO_MUX_GPIO23_FUNC_TOUCH10        7

#define WM_IO_MUX_GPIO24                     WM_GPIO_NUM_24
#define WM_IO_MUX_GPIO24_FUNC_I2S_BCK        1
#define WM_IO_MUX_GPIO24_FUNC_MMC_D0         2
#define WM_IO_MUX_GPIO24_FUNC_PWM_BREAK      3
#define WM_IO_MUX_GPIO24_FUNC_SDIO_D0        4
#define WM_IO_MUX_GPIO24_FUNC_GPIO           5
#define WM_IO_MUX_GPIO24_FUNC_LCD_SEG20      6
#define WM_IO_MUX_GPIO24_FUNC_TOUCH11        7

#define WM_IO_MUX_GPIO25                     WM_GPIO_NUM_25
#define WM_IO_MUX_GPIO25_FUNC_I2S_LRCK       1
#define WM_IO_MUX_GPIO25_FUNC_MMC_D1         2
#define WM_IO_MUX_GPIO25_FUNC_HSPI_CS        3
#define WM_IO_MUX_GPIO25_FUNC_SDIO_D1        4
#define WM_IO_MUX_GPIO25_FUNC_GPIO           5
#define WM_IO_MUX_GPIO25_FUNC_LCD_SEG21      6
#define WM_IO_MUX_GPIO25_FUNC_TOUCH12        7

#define WM_IO_MUX_GPIO26                     WM_GPIO_NUM_26
#define WM_IO_MUX_GPIO26_FUNC_I2S_DI         1
#define WM_IO_MUX_GPIO26_FUNC_MMC_D2         2
#define WM_IO_MUX_GPIO26_FUNC_HSPI_DI        3
#define WM_IO_MUX_GPIO26_FUNC_SDIO_D2        4
#define WM_IO_MUX_GPIO26_FUNC_GPIO           5
#define WM_IO_MUX_GPIO26_FUNC_LCD_SEG22      6

#define WM_IO_MUX_GPIO27                     WM_GPIO_NUM_27
#define WM_IO_MUX_GPIO27_FUNC_I2S_DO         1
#define WM_IO_MUX_GPIO27_FUNC_MMC_D3         2
#define WM_IO_MUX_GPIO27_FUNC_HSPI_DO        3
#define WM_IO_MUX_GPIO27_FUNC_SDIO_D3        4
#define WM_IO_MUX_GPIO27_FUNC_GPIO           5
#define WM_IO_MUX_GPIO27_FUNC_LCD_SEG23      6

#define WM_IO_MUX_GPIO28                     WM_GPIO_NUM_28
#define WM_IO_MUX_GPIO28_FUNC_HSPI_CK        1
#define WM_IO_MUX_GPIO28_FUNC_PWM0           2
#define WM_IO_MUX_GPIO28_FUNC_UART5_CTS      3
#define WM_IO_MUX_GPIO28_FUNC_I2S_BCLK       4
#define WM_IO_MUX_GPIO28_FUNC_GPIO           5
#define WM_IO_MUX_GPIO28_FUNC_LCD_SEG24      6

#define WM_IO_MUX_GPIO29                     WM_GPIO_NUM_29
#define WM_IO_MUX_GPIO29_FUNC_HSPI_INT       1
#define WM_IO_MUX_GPIO29_FUNC_PWM1           2
#define WM_IO_MUX_GPIO29_FUNC_UART5_RTS      3
#define WM_IO_MUX_GPIO29_FUNC_I2S_LRCLK      4
#define WM_IO_MUX_GPIO29_FUNC_GPIO           5
#define WM_IO_MUX_GPIO29_FUNC_LCD_SEG25      6

#define WM_IO_MUX_GPIO30                     WM_GPIO_NUM_30
#define WM_IO_MUX_GPIO30_FUNC_HSPI_CS        1
#define WM_IO_MUX_GPIO30_FUNC_PWM2           2
#define WM_IO_MUX_GPIO30_FUNC_LSPI_CS        3
#define WM_IO_MUX_GPIO30_FUNC_I2S_DO         4
#define WM_IO_MUX_GPIO30_FUNC_GPIO           5
#define WM_IO_MUX_GPIO30_FUNC_LCD_SEG26      6

#define WM_IO_MUX_GPIO31                     WM_GPIO_NUM_31
#define WM_IO_MUX_GPIO31_FUNC_HSPI_DI        1
#define WM_IO_MUX_GPIO31_FUNC_PWM3           2
#define WM_IO_MUX_GPIO31_FUNC_LSPI_CK        3
#define WM_IO_MUX_GPIO31_FUNC_I2S_DI         4
#define WM_IO_MUX_GPIO31_FUNC_GPIO           5
#define WM_IO_MUX_GPIO31_FUNC_LCD_SEG27      6

#define WM_IO_MUX_GPIO32                     WM_GPIO_NUM_32
#define WM_IO_MUX_GPIO32_FUNC_HSPI_DO        1
#define WM_IO_MUX_GPIO32_FUNC_PWM4           2
#define WM_IO_MUX_GPIO32_FUNC_LSPI_MISO      3
#define WM_IO_MUX_GPIO32_FUNC_UART1_RX       4
#define WM_IO_MUX_GPIO32_FUNC_GPIO           5
#define WM_IO_MUX_GPIO32_FUNC_LCD_SEG28      6

#define WM_IO_MUX_GPIO33                     WM_GPIO_NUM_33
#define WM_IO_MUX_GPIO33_FUNC_UART5_RX       1
#define WM_IO_MUX_GPIO33_FUNC_PWM_BREAK      2
#define WM_IO_MUX_GPIO33_FUNC_LSPI_MOSI      3
#define WM_IO_MUX_GPIO33_FUNC_I2S_MCLK       4
#define WM_IO_MUX_GPIO33_FUNC_GPIO           5
#define WM_IO_MUX_GPIO33_FUNC_LCD_SEG29      6

#define WM_IO_MUX_GPIO34                     WM_GPIO_NUM_34
#define WM_IO_MUX_GPIO34_FUNC_UART5_TX       1
#define WM_IO_MUX_GPIO34_FUNC_GPIO           5
#define WM_IO_MUX_GPIO34_FUNC_LCD_SEG29      6

#define WM_IO_MUX_GPIO35                     WM_GPIO_NUM_35
#define WM_IO_MUX_GPIO35_FUNC_UART0_TX       1
#define WM_IO_MUX_GPIO35_FUNC_PWM0           2
#define WM_IO_MUX_GPIO35_FUNC_UART1_RTS      3
#define WM_IO_MUX_GPIO35_FUNC_I2C_SDA        4
#define WM_IO_MUX_GPIO35_FUNC_GPIO           5

#define WM_IO_MUX_GPIO36                     WM_GPIO_NUM_36
#define WM_IO_MUX_GPIO36_FUNC_UART0_RX       1
#define WM_IO_MUX_GPIO36_FUNC_PWM1           2
#define WM_IO_MUX_GPIO36_FUNC_UART1_CTS      3
#define WM_IO_MUX_GPIO36_FUNC_I2C_SCL        4
#define WM_IO_MUX_GPIO36_FUNC_GPIO           5

#define WM_IO_MUX_GPIO37                     WM_GPIO_NUM_37
#define WM_IO_MUX_GPIO37_FUNC_UART0_RTS      1
#define WM_IO_MUX_GPIO37_FUNC_PCM_SYNC       2
#define WM_IO_MUX_GPIO37_FUNC_GPIO           5
#define WM_IO_MUX_GPIO37_FUNC_LCD_COM1       6

#define WM_IO_MUX_GPIO38                     WM_GPIO_NUM_38
#define WM_IO_MUX_GPIO38_FUNC_UART0_CTS      1
#define WM_IO_MUX_GPIO38_FUNC_PCM_CK         2
#define WM_IO_MUX_GPIO38_FUNC_GPIO           5
#define WM_IO_MUX_GPIO38_FUNC_LCD_COM2       6

#define WM_IO_MUX_GPIO39                     WM_GPIO_NUM_39
#define WM_IO_MUX_GPIO39_FUNC_LSPI_CS        1
#define WM_IO_MUX_GPIO39_FUNC_PCM_DATA       2
#define WM_IO_MUX_GPIO39_FUNC_GPIO           5
#define WM_IO_MUX_GPIO39_FUNC_LCD_SEG0       6

#define WM_IO_MUX_GPIO40                     WM_GPIO_NUM_40
#define WM_IO_MUX_GPIO40_FUNC_LSPI_CK        1
#define WM_IO_MUX_GPIO40_FUNC_PWM2           2
#define WM_IO_MUX_GPIO40_FUNC_GPIO           5
#define WM_IO_MUX_GPIO40_FUNC_LCD_SEG2       6

#define WM_IO_MUX_GPIO41                     WM_GPIO_NUM_41
#define WM_IO_MUX_GPIO41_FUNC_LSPI_MISO      1
#define WM_IO_MUX_GPIO41_FUNC_PWM3           2
#define WM_IO_MUX_GPIO41_FUNC_GPIO           5
#define WM_IO_MUX_GPIO41_FUNC_LCD_COM0       6

#define WM_IO_MUX_GPIO42                     WM_GPIO_NUM_42
#define WM_IO_MUX_GPIO42_FUNC_LSPI_MOSI      1
#define WM_IO_MUX_GPIO42_FUNC_PWM4           2
#define WM_IO_MUX_GPIO42_FUNC_GPIO           5
#define WM_IO_MUX_GPIO42_FUNC_LCD_SEG1       6

#define WM_IO_MUX_GPIO43                     WM_GPIO_NUM_43
#define WM_IO_MUX_GPIO43_FUNC_PSRAM_CS       1
#define WM_IO_MUX_GPIO43_FUNC_UART0_TX       2
#define WM_IO_MUX_GPIO43_FUNC_GPIO           5
#define WM_IO_MUX_GPIO43_FUNC_LCD_COM3       6

#define WM_IO_MUX_GPIO44                     WM_GPIO_NUM_44
#define WM_IO_MUX_GPIO44_FUNC_GPIO           5

#define WM_IO_MUX_GPIO45                     WM_GPIO_NUM_45
#define WM_IO_MUX_GPIO45_FUNC_GPIO           5

#define WM_IO_MUX_GPIO46                     WM_GPIO_NUM_46
#define WM_IO_MUX_GPIO46_FUNC_GPIO           5

#define WM_IO_MUX_GPIO47                     WM_GPIO_NUM_47
#define WM_IO_MUX_GPIO47_FUNC_GPIO           5

#ifdef __cplusplus
}
#endif

#endif /* __WM_IO_MUX_H__ */
