# touch_panel

## Function Overview

Start a FreeRTOS task, initialize touch driver chip, configure the basic parameters of the touch chip,
Obtain the touch point in the loop, obtain the actual coordinates touched through the touch point, and display them in the serial port.

How to add a new touch panel:  
   - a) Pre-configure the device information in the Device Table for the touch panel display.  
   - b) The reference application uses the macro 'WM_CFG_TOUCH_PANEL_NAME' from wm_drv_touch_panel.h, which should be configured to match the device name predefined in the Device Table.  
      Select the touch panel device either through:
      - menuconfig -> PERIPHERALS -> Touch Panel -> touch panel
      OR
      - Directly define the compilation switch in prj.config (e.g., "CONFIG_COMPONENT_XPT2046_DRIVER=y")  
   - c) The TFT touch panel driver must be properly adapted, primarily by implementing the driver's ops functions and associating them with the dt_hw_table_entry[] in the Device Table.


## Environmental requirements

1. Touch driver chip:
    - Model: ft6336 or xpt2046
    Select touch driver controller from menuconfig ->PERIPHERALS ->Touch driver ->touch device.

2. Pin connection:
    Hardware configuration for FT6336 and XPT2046 touch controllers is defined in wm_dt_hw.c (default uses W802 chip)
    Note: This is just an example. Users should modify the device table according to their development board and application requirements, then recompile.

   |    GPIO Number    | Pin Label | Connection  | FT6336  PIN |
   | ----------------- | -------- | ----------- | ----------- |
   | WM_GPIO_NUM_1     |   PA1    | I2C_SCL     | 44          |
   | WM_GPIO_NUM_4     |   PA4    | I2C_SDA     | 45          |
   | WM_GPIO_NUM_24    |   PB8    | RESET       | 42          |

   |    GPIO Number    | Pin Label | Connection  | XPT2046 PIN |
   | ----------------- | -------- | ----------- | ----------- |
   | WM_GPIO_NUM_24    |   PB8    | CLK         | 16          |
   | WM_GPIO_NUM_25    |   PB9    | CS          | 15          |
   | WM_GPIO_NUM_28    |   PB12   | MOSI        | 14          |
   | WM_GPIO_NUM_29    |   PB13   | MISO        | 12          |
   | WM_GPIO_NUM_30    |   PB14   | IRQ         | 11          |

## Compile and Burn

Example location:`examples/peripheral/touch_panel/touch_panel_basic`

For compilation, burning, and other operations, please refer to: [Quick Start] (https://doc.winnermicro.net/w800/zh_CN/latest/get_started/index.html)

## Operation results

When touching the ft6336 touchscreen, if there is only one touch point, only Touch coordinate point 1 will be printed
If there are two touch points, touch coordination point 1 and touch coordination point 2 will be printed.

```
[I] (352622) test: Touch coordinate point 1, x-coordinate is 302, y-coordinate is 240.
[I] (352623) test: Touch coordinate point 2, x-coordinate is 251, y-coordinate is 341.
[I] (352724) test: Touch coordinate point 1, x-coordinate is 302, y-coordinate is 240.
[I] (352725) test: Touch coordinate point 2, x-coordinate is 251, y-coordinate is 341.
[I] (352826) test: Touch coordinate point 1, x-coordinate is 302, y-coordinate is 240.
[I] (352827) test: Touch coordinate point 2, x-coordinate is 251, y-coordinate is 341.
[I] (352928) test: Touch coordinate point 1, x-coordinate is 302, y-coordinate is 240.
[I] (352929) test: Touch coordinate point 2, x-coordinate is 243, y-coordinate is 342.
[I] (353030) test: Touch coordinate point 1, x-coordinate is 227, y-coordinate is 355.
[I] (353031) test: Touch coordinate point 2, x-coordinate is 300, y-coordinate is 209.
[I] (353132) test: Touch coordinate point 1, x-coordinate is 285, y-coordinate is 131.
[I] (353133) test: Touch coordinate point 2, x-coordinate is 227, y-coordinate is 355.
[I] (353234) test: Touch coordinate point 1, x-coordinate is 300, y-coordinate is 130.
[I] (353336) test: Touch coordinate point 1, x-coordinate is 205, y-coordinate is 313.
[I] (353438) test: Touch coordinate point 1, x-coordinate is 284, y-coordinate is 156.
[I] (366476) test: Touch coordinate point 1, x-coordinate is 127, y-coordinate is 208.
[I] (366578) test: Touch coordinate point 1, x-coordinate is 127, y-coordinate is 208.
[I] (366680) test: Touch coordinate point 1, x-coordinate is 127, y-coordinate is 208.
[I] (367085) test: Touch coordinate point 1, x-coordinate is 219, y-coordinate is 132.
[I] (367086) test: Touch coordinate point 2, x-coordinate is 144, y-coordinate is 256.
[I] (367187) test: Touch coordinate point 1, x-coordinate is 219, y-coordinate is 132.
[I] (367188) test: Touch coordinate point 2, x-coordinate is 144, y-coordinate is 256.
[I] (367289) test: Touch coordinate point 1, x-coordinate is 219, y-coordinate is 132.
[I] (367290) test: Touch coordinate point 2, x-coordinate is 144, y-coordinate is 256.
[I] (367391) test: Touch coordinate point 1, x-coordinate is 219, y-coordinate is 132.
[I] (367392) test: Touch coordinate point 2, x-coordinate is 144, y-coordinate is 256.
[I] (367493) test: Touch coordinate point 1, x-coordinate is 219, y-coordinate is 132.
[I] (367494) test: Touch coordinate point 2, x-coordinate is 144, y-coordinate is 256.
[I] (367999) test: Touch coordinate point 1, x-coordinate is 176, y-coordinate is 246.
[I] (368000) test: Touch coordinate point 2, x-coordinate is 218, y-coordinate is 134.
[I] (368101) test: Touch coordinate point 1, x-coordinate is 218, y-coordinate is 134.
[I] (368102) test: Touch coordinate point 2, x-coordinate is 176, y-coordinate is 246.
[I] (368203) test: Touch coordinate point 1, x-coordinate is 218, y-coordinate is 134.
[I] (368305) test: Touch coordinate point 1, x-coordinate is 148, y-coordinate is 217.
```