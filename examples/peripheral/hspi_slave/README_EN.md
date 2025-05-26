# HSPI Slave

## Overview

This application is an example of HSPI slave communication using the `WM IoT SDK`. It demonstrates how to use the `wm_drv_hspi_slave` driver APIs from the `WM IoT SDK` for data transfer between host and slave. The example includes both synchronous and asynchronous data transfers, as well as command and data reception handling.

The HSPI of W800 only supports single SPI and does not support Dual SPI or QSPI.

The HSPI and SDIO slave of the W800 cannot be used simultaneously.

## Requirements

Hardware environment that supports HSPI slave functionality and a HSPI host device are required.

- W801S_EVB
- The STM32_MiniV2.0 board acts as the host (UART5 used for log and send control commands (TX5 on PC12, RX5 on PD2), powered via USB cable，the host-end code refers to the code in the current example directory under `hspi_host_code`.)
- Connection cables, to enhance communication stability, use short cables whenever possible, and avoid laying these cables side by side or coiling them together.

  ```
              ┌──────────────────┐            ┌────────────────────┐
    log & ctrl│                  │            │                    │ log
      ────────┤PC12(TX5)  PA4(CS)├────────────┤PB9(CS)        uart0├─────────────
              │                  │            │                    │ power supply
      ────────┤PD2(RX5)  PA5(CLK)├────────────┤PB6(CLK)            │
              │                  │            │                    │
      ────────┤USB      PA6(MISO)├────────────┤PB11(MISO)          │
  power supply│                  │            │                    │
              │         PA7(MOSI)├────────────┤PB10(MOSI)          │
              │                  │            │                    │
              │    PA2(INT input)├────────────┤PB7(INT output)     │
              │                  │            │                    │
              │               GND├────────────┤GND                 │
              └─stm32_MiniV2.0───┘            └───W80x─────────────┘
  ```
  

The W801s, W802, and W806 can also select another set of GPIOs as the HSPI slave. The definitions are PB12 (CLK), PB13 (INT), PB14 (CS), PB15 (MOSI), and PB16 (MISO). One of these two sets of IOs can be selected in the device table.

## Communication Protocol

The HSPI slave interface of the W80x has three communication scenarios with the host:

- **Host sends data**

  The host sends the function code 0x03 and determines whether the slave can receive data based on the data returned by the slave. If it can, the host sends the write data function code 0x90 and then sends the data.

- **Host sends command**

  The host sends the function code 0x03 and determines whether the slave can receive the command based on the data returned by the slave. If it can, the host sends the write command function code 0x91 and then sends the command.

- **Host receives data**

  After receiving the interrupt signal from the slave, the host sends the function code 0x06 and determines whether the slave has prepared the data based on the data returned by the slave. If it is ready, the host sends the function code 0x02 and obtains the number of bytes of data that the slave wants to upload based on the data returned by the slave. The host then sends the function code 0x10 and reads the data.

For detailed information, refer to sections `10.4.2 Host-end Access to HSPI Controller Register List` and `10.4.3 High-Speed SPI Device Controller Interface Timing` in the 《W801_register manual.pdf》.

## Building and Flashing

Example Location： `examples\peripheral\hspi_slave`

For compilation, flashing, and other operations, please refer to: [Quick Start Guide](https://doc.winnermicro.net/w800/en/latest/get_started/index.html)


## Running Result

  Send `t-hspi_init=(18000000,0)` to ST. The first parameter is the frequency, ranging from 1,000,000 to 36,000,000 (limited by the clock configuration of STM32). The second parameter is in Motorola format, which has four possible values:0 (CPOL=0, CPHA=0), 1 (CPOL=0, CPHA=1), 2 (CPOL=1, CPHA=0), 3 (CPOL=1, CPHA=1). Since both `cpol` and `cpha` are set to 0 in the W80x example, this parameter needs to be set to 0.

  After that, sending `t-high-mspi-r` to ST once will test the data transmission and commands reception functions of W80x's HSPI as a slave.

  Sending `t-high-mspi-s` to ST once will test the data reception function of W80x's HSPI as a slave.

  After successful execution, W80x will output the following log:

```
[I] (22) main: ver: 2.4-alpha.1 build at Apr 25 2025 18:12:24
[I] (22) main: boot reason 0
[I] (22) main: heap size 272544(266.16KB)
[I] (23) main: flash size 0x100000(1MB)
[I] (23) example: HSPI slave demo start
[I] (24) example: Initializing hspi slave device...
[I] (24) DRV HSPI SLAVE: [wm_drv_hspi_slave_pin_cfg:217] Pin configuration completed successfully
[I] (25) DRV HSPI SLAVE: [wm_drv_hspi_slave_buf_desc_init:306] Buffer descriptor initialization completed successfully
[I] (25) DRV HSPI SLAVE: [wm_drv_ops_hspi_slave_init:462] Driver initialization completed successfully
[I] (25) example: Successfully initialized hspi slave device
[I] (25) example: Waiting for rx events...
[I] (14915) example: RX cmd complete
[I] (14915) example: dump [[RX CMD:]]
0x0000   5a 00 05 01 60 00 00 00                           Z...`...
[I] (14918) example: ASYNC TX: 1024 bytes
[I] (18323) example: RX data complete
[I] (18323) example: HSPI slave RX OK
```

  ST will output the following log:

```
STM32_Mini V2.0 board layout:
tested uart:
Tx1 PA9, RX1 PA10
console uart:
TX5 PC12, RX5 PD2(uart5 used for print log)

Sequence  Command             Description
------------------------------------------------------------------------------------
1         t-lspi_init         Init spi; For example t-lspi_init=(0,1000000,0), set spi as (0-slave, 1-master), clk 1MHZ, Motorola format 0
2         t-mspi-s            Test low SPI Master sending data,for example t-mspi-s
3         t-mspi-r            Test low SPI Master receiving data,for example t-mspi-r
4         t-hspi_init         Init spi; For example t-hspi_init=(1000000,0), set spi clk 1MHZ, Motorola format 0
5         t-high-mspi-s       Test high SPI Master sending data,for example t-high-mspi-s
6         t-high-mspi-r       Test high SPI Master receiving data,for example t-high-mspi-r
7         t-uartloopback      Test uart loopback; For example t-uartloopback=(9600,0,0,1),baudrate 9600 ,parity none, 1 stop bit and 1 start send
8         t-uartrcvrate       Test uart rcv rate; For example t-uartrcvrate=(9600,0,0),baudrate 9600 ,parity none and 1 stop bit
9         t-uartsendrate      Test uart send rate; For example t-uartsendrate=(9600,0,0,1),baudrate 9600, parity none, 1 stop bit and 1 start send
10        demohelp            Display Help information
------------------------------------------------------------------------------------

[CMD]t-hspi_init
cs--PA4, ck--PA5, miso--PA6, mosi--PA7, int--PA2;
set ST HSPI as master clk 36000000HZ(actual 36000000HZ), Motorola format 0

[CMD]t-high-mspi-r
rate 553887 Bytes/s(1024B/1848us)
ST HSPI master RX OK 1

[CMD]t-high-mspi-s
rate 567509 Bytes/s(1024B/1804us)
ST HSPI master send 1
```
