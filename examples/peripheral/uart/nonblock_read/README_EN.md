# Uart nonblock_read

## Overview

After the application starts, perform the following 4 operations:
1. Initialize serial port 1
2. Create message queue
3. Register the callback function to receive the serial port data receiving status callback, and send the status message to task for processing in the callback;
4. Receive messages circularly, read data from serial port 1 in non blocking mode after receiving the message, and write back to serial port 1

Uart receives data in this way. Since it does not block tasks, it can be used in the task of other module.

## Requirements

1. Pin Connections:.
   The hardware configuration is stored in the Device Table, which includes the pin definitions of the UART device.
   The definition file of Device Table is wm_dt_hw.c, but the details may vary depending on the specific device table files referenced within this project.
   The figure below shows the GPIO serial numbers of the W800, which correspond to an example based on the EVB (Evaluation Board) development board. The actual use should be based on the hardware configuration Device Table and the actual hardware use.

   | GPIO_NUM | Pin name | Link  | PC Link Pin |
   | -------- |--------- | --------- | --------|
   | WM_GPIO_NUM_22 | PB6   | TX   |   RX     |
   | WM_GPIO_NUM_23 | PB7   | RX   |   TX     |

## Building and Flashing

Sample location: `examples/peripheral/uart/block_read_write`
For operations such as compiling and burning, please refer to: [Quick Start Guide](../../../../../get_started/index. html # build function)

## Running Result

Upon successful execution, the following logs will be output

```
D/dt              [0.002] add device iflash
D/ptb             [0.002] partition table base 0xe000
D/ft              [0.002] ft_0 base 0, ft_1 base 1000
D/ft              [0.004] ft init too
D/ft              [0.004] ft init 1
D/main            [0.004] boot reason 0
D/main            [0.004] heap size 273152(266.75KB)
D/main            [0.006] flash size 0x200000(2MB)
I/exam_uart       [0.006] uart noblock read demo start.

D/dt              [0.006] add device uart1
```
