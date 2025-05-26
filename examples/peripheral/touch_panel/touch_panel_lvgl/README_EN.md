# TFT_LCD_LVGL_TOUCH

## Function Overview
This application integrates the LVGL library with touch controller drivers to display real-time touch coordinates on the TFT LCD screen.

## Environmental requirements

1. TFT LCD Display Specifications:
   - Manufacturer: New Vision Microelectronics Inc.
   - Model: NV3041A

The TFT LCD display can be replaced if the following conditions are met:  
   - a) The display device information must be pre-configured in the Device Table  
   - b) The application references the macro 'WM_LVGL_LCD_MODULE_NAME' in wm_drv_tft_lcd_cfg.h, which should match the device name in the Device Table  
      Selection options:  
      - Via menuconfig: `PERIPHERALS -> TFT LCD -> LCD Device`
      - Or directly define compilation switches in prj.config (e.g., "CONFIG_COMPONENT_DRIVER_LCD_NV3041A_SPI=y")  
   - c) The TFT LCD driver must be properly implemented, including all required ops functions registered in dt_hw_table_entry[]

2. Touch Controller:
   - Supported models: FT6336 or XPT2046
   Selection path: `menuconfig -> PERIPHERALS -> Touch driver -> touch Device`

3. Pin connection:

   TFT LCD hardware configuration is stored in the Device Table, including:
   - SDIO device pin definitions
   - SDIO (SPI Mode) clock settings
   - Other TFT LCD display pins
   Device definitions can be queried by device name in the Device Table (reference file: wm_dt_hw.c)

   Below is an example GPIO mapping for W802 and NV3041A (actual configuration should match Device Table):

   |    GPIO Number    | Pin Label | Connection  | NV3041A PIN |
   | ----------------- | -------- | ----------- | ----------- |
   | WM_GPIO_NUM_9     |   PA9    |   CLK       | 11          |
   | WM_GPIO_NUM_10    |   PA10   |   MOSI      | 13          |
   | WM_GPIO_NUM_14    |   PA14   |   CS        | 9           |
   | WM_GPIO_NUM_7     |   PA7    |   RESET     | 15          |
   | WM_GPIO_NUM_2     |   PA2    |   LED       | 36          |
   | WM_GPIO_NUM_12    |   PA12   |   DCX       | 10          |
   | WM_GPIO_NUM_24    |   PB29   |   TE        | 8           |


   Touch controller configurations (default using W802 chip):

   |    GPIO Number    | Pin Label | Connection  | FT6336 PIN |
   | ----------------- | -------- | ----------- | ---------- |
   | WM_GPIO_NUM_1     |   PA1    | I2C_SCL     | 44         |
   | WM_GPIO_NUM_4     |   PA4    | I2C_SDA     | 45         |
   | WM_GPIO_NUM_24    |   PB8    | RESET       | 42         |

   |    GPIO Number    | Pin Label | Connection  | XPT2046 PIN |
   | ----------------- | -------- | ----------- | ----------- |
   | WM_GPIO_NUM_24    |   PB8    | CLK         | 16          |
   | WM_GPIO_NUM_25    |   PB9    | CS          | 15          |
   | WM_GPIO_NUM_28    |   PB12   | MOSI        | 14          |
   | WM_GPIO_NUM_29    |   PB13   | MISO        | 12          |
   | WM_GPIO_NUM_30    |   PB14   | IRQ         | 11          |


## Compile and Burn

Example location:` examples\tft_lcd\tft_lcd_lvgl_touch_panel`

When compiling, please pay attention to selecting VNet BUILD_Type_XXX and confirm that the referenced Device Table matches the development board being tested.
For higher display efficiency, users can set the RCC CPU Clock to a higher value, such as 240MHz, through the device table.

For compilation, burning, and other operations,
please refer to: [Quick Start] (https://doc.winnermicro.net/w800/zh_CN/latest/get_started/index.html)

## Runing Results

```
[I] (207624) main: Last touch: x=136, y=435
[I] (207707) main: Last touch: x=250, y=209
[I] (207740) main: Last touch: x=136, y=435
[I] (207819) main: Last touch: x=137, y=431
[I] (207871) main: Last touch: x=159, y=384
[I] (207904) main: Last touch: x=170, y=363
[I] (207937) main: Last touch: x=176, y=353
[I] (207970) main: Last touch: x=249, y=214
[I] (208002) main: Last touch: x=186, y=339
[I] (208048) main: Last touch: x=191, y=334
[I] (208684) main: Last touch: x=136, y=350
[I] (208814) main: Last touch: x=152, y=324
[I] (208866) main: Last touch: x=264, y=180
[I] (208899) main: Last touch: x=176, y=294
[I] (208932) main: Last touch: x=179, y=289
[I] (210021) main: Last touch: x=295, y=158
[I] (210117) main: Last touch: x=273, y=333
[I] (210181) main: Last touch: x=281, y=333
[I] (210233) main: Last touch: x=287, y=333
[I] (210284) main: Last touch: x=295, y=158
[I] (210338) main: Last touch: x=294, y=150
[I] (210371) main: Last touch: x=282, y=140
[I] (210404) main: Last touch: x=276, y=134
[I] (210437) main: Last touch: x=304, y=265
```
