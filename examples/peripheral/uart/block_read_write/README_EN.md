# Uart block_read_write

## Overview

After the application starts, perform the following 2 operations:
1. Initialize serial port 1
2. Cyclically read data from serial port 1 blocking mode and write back to serial port 1

## Requirements

1. Pin Connections:.
   The hardware configuration is stored in the Device Table, which contains the pin definitions of the UART device.
   The definition file of Device Table is wm_dt_hw.c, but the details may vary depending on different device table files
   referenced by this project.
   The figure below shows the GPIO serial numbers of the W800, which correspond to an example of the EVB development
   board.For actual use, the configuration should be based on the hardware details specified in the Device Table and the actual hardware implementation.

   | GPIO_NUM | Pin name | Link  | PC Link Pin |
   | -------- |--------- | --------- | --------|
   | WM_GPIO_NUM_22 | PB6   | TX   |   RX     |
   | WM_GPIO_NUM_23 | PB7   | RX   |   TX     |

## Building and Flashing

Sample location: `examples/peripheral/uart/block_read_write`

For operations such as compiling and burning, please refer to: [Quick Start Guide](../../../../../get_started/index. html # build function)

## Running Result

Successfully running will output the following logs

```
D/dt              [0.002] add device iflash
D/ptb             [0.002] partition table base 0xe000
D/ft              [0.002] ft_0 base 0, ft_1 base 1000
D/ft              [0.004] ft init too
D/ft              [0.004] ft init 1
D/main            [0.004] boot reason 0
D/main            [0.004] heap size 273160(266.76KB)
D/main            [0.004] flash size 0x200000(2MB)
I/exam_uart       [0.006] uart block read write demo start.

D/dt              [0.006] add device uart1
```
