# HSPI Slave

## 功能概述

  此应用程序是使用 `WM IoT SDK` 进行 HSPI 从机通信的示例。它演示了如何使用 `WM IoT SDK` 中的 `wm_drv_hspi_slave` 驱动 API 进行主机和从机之间的数据传输。示例包括同步和异步数据传输,以及命令和数据的接收处理。
  W800 的 HSPI 只支持 single SPI，不支持 Dual SPI, Qual SPI。
  W800 的 HSPI 和 SDIO slave 不能同时使用。


## 环境要求

- W801S_EVB

- stm32_MiniV2.0 作为主机（串口 5用于打印日志和下发命令（TX5 PC12，RX5 PD2）, 使用 USB 线供电，主机端的程序参考当前 example 目录下的 hspi_host_code ）

-  链接线缆，为了提高通信稳定性，尽量使用短线，并且这几根线不要并排或缠绕在一起：

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
  
  W801s、w802、W806还可以选择另外一组GPIO作为HSPI slave，定义是PB12（CLK）、PB13（INT）、PB14（CS）、PB15（MOSI）、PB16（MISO）。可以在device table中选择这两组IO中的一组。
  

## 通信约定

  W80x的HSPI从机接口与主机之间有三种通信情景：

- 主机下发数据

  主机发送功能码 0x03 ，根据从机返回的数据判断从机是否可以接收数据，如果可以，则发送写数据功能码 0x90 ，继而发送数据。

- 主机下发命令

  主机发送功能码 0x03 ，根据从机返回的数据判断从机是否可以接收命令，如果可以，则发送写命令功能码 0x91 ，继而发送命令。

- 主机接收数据

  在收到从机的中断信号以后，主机发送功能码 0x06 ，根据从机返回的数据判断从机是否已经准备好数据，如果准备好，则主机发送功能码 0x02 ，根据从机返回的数据获取从机要上传数据的字节数，主机发送功能码 0x10 ，继而读取数据。

详细说明参见《W801_寄存器手册.pdf》中的 `10.4.2 主机端访问 HSPI 控制器寄存器列表` 和 `10.4.3 高速 SPI 设备控制器接口时序  ` 这两节。

## 编译和烧录

示例位置：`examples\peripheral\hspi_slave`

编译、烧录等操作请参考：[快速入门](https://doc.winnermicro.net/w800/zh_CN/latest/get_started/index.html)

## 运行结果

  给 ST 发送t-hspi_init=(18000000,0)，第一个参数是频率，范围是 1000000 到 36000000 （受限于 STM32 的时钟配置）；第二个参数是 motorola 格式，有4个取值，0(CPOL=0,CPHA=0), 1(CPOL=0,CPHA=1), 2(CPOL=1,CPHA=0), 3(CPOL=1,CPHA=1)。因为W80x的example中 cpol 和 cpha 为 0 ，所以该参数需要设为 0。

  之后给 ST 发送一次 t-high-mspi-r 则可测试 W80x 的 HSPI 作为 slave 的数据发送和命令接收功能。

  给 ST 发送一次 t-high-mspi-s 则可测试 W80x 的 HSPI 作为 slave 的数据接收功能。

  成功运行后，W80x 将输出如下日志

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

  ST 将输出如下日志

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

