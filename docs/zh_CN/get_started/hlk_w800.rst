
HLK-W800-KIT 入门指南
=========================


准备工作
----------

- HLK-W800-KIT 开发板

- Type-C 数据线

- USB 转串口驱动（`点击下载 CH34X 驱动 <https://doc.winnermicro.net/download/tools/all/CH341SER.zip>`_）

- PC（Windows、Linux 或 macOS）

你可以跳过介绍部分，直接前往 :ref:`应用程序开发<application-program-w800>` 章节。

概述
----------

HLK-W800-KIT 是海凌科电子采用了 W800 制作的带有一个 RGB 三
色灯，集成了 CHT8305C 温湿度传感器的多功能开发板，用户可以在上面学习、研究嵌入式
系统和物联网产品的开发，方便
的创建、开发、属于你的 IOT 产品，快速量产适用于智能家电、智能家居、智能玩具、无线
音视频、工业控制等广泛的物联网产品，节约您的宝贵时间。


功能说明
----------

HLK-W800-KIT 开发板的主要组件、接口及控制方式如下。

.. figure:: ../../_static/get_started/hardware/hlk_w800_kit.png
    :align: center
    :alt: HLK-W800-KIT 开发板布局

HLK-W800-KIT 开发板的主要组件描述见下表（从左上角起顺时针顺序）。

.. list-table:: 
   :widths: 25 25 
   :header-rows: 0
   :align: center

   * - 主要组件
     - 基本介绍

   * - BOOT-Key
     - 下载使用按键，用户按下 BOOT 按键，然后将 USB 线插入到 UART0 口，松开 BOOT 按键后进入下载模式。

   * - LDO
     - 5v 转 3.3v 的 LDO 芯片。

   * - LED
     - 三色 LED，为用户方便调试 PWM。使用时需要将 K1 开关拨到 ON 档，不用时拨到 OFF 档。

   * - Antenna
     - 开发板的板载天线，开发板还支持外置天线。

   * - W800
     - 开发板的主控芯片，具有 WiFi，蓝牙等功能。

   * - I/O
     - W800 的所有管脚都已引出至开发板的排针，用户可以对 W800 进行编程，实现 PWM、ADC、DAC、I2C和I2S等功能，详情请见 :ref:`管脚说明<base-pin-w800>`。

   * - RST-Key
     - 系统复位按键。

   * - TYPE-C
     - 开发板默认的 USB 口，可作为开发板的供电口，或作为连接 PC 和开发板的通信接口。

电源选项
-----------

开发板可任一选用以下三种供电方式：

- Type-C 供电（默认）。
- 5V / GND 管脚供电。
- 3.3V / GND 管脚供电。

.. warning:: 

    上述供电模式 **不可同时连接**，否则可能会损坏开发板或电源。

.. _base-pin-w800:

管脚说明
----------

下表介绍了开发板 I/O 管脚的 **名称** 和 **功能** ，具体布局请见 :ref:`开发板原理图与布局示意图<schematic-diagram-w800>` 章节。

.. list-table:: 
   :widths: 25 25 25 25 25 25 25 25
   :header-rows: 0
   :align: center

   * - 编号
     - 名称
     - 类型
     - 复位后管脚功能
     - 复用功能
     - 最高频率
     - 上下拉能力
     - 驱动能力

   * - 1
     - PB_20
     - I/O
     - UART_RX
     - UART0_RX/PWM1/UART1_CTS/I²C_SCL
     - 10MHz
     - UP/DOWN
     - 12mA

   * - 2
     - PB_19
     - I/O
     - UART_TX
     - UART0_TX/PWM0/UART1_RTS/I²C_SDA
     - 10MHz
     - UP/DOWN
     - 12mA

   * - 3
     - WAKEUP
     - I
     - WAKEUP 唤醒功能
     - 
     - 
     - DOWN
     - 

   * - 4
     - RESET
     - I
     - RESET 复位
     - 
     - 
     - UP
     - 

   * - 5
     - XTAL_OUT
     - O
     - 外部晶振输出
     - 
     - 
     - 
     - 

   * - 6
     - XTAL_IN
     - I
     - 外部晶振输入
     - 
     - 
     - 
     - 

   * - 7
     - AVDD33
     - P
     - 芯片电源，3.3V
     - 
     - 
     - 
     - 

   * - 8
     - ANT
     - I/O
     - 射频天线
     - 
     - 
     - 
     - 

   * - 9
     - AVDD33
     - P
     - 芯片电源，3.3V
     - 
     - 
     - 
     - 

   * - 10
     - AVDD33
     - P
     - 芯片电源，3.3V
     - 
     - 
     - 
     - 

   * - 11
     - AVDD33_AUX
     - P
     - 芯片电源，3.3V
     - 
     - 
     - 
     - 

   * - 12
     - TEST
     - I
     - 测试功能配置管脚
     - 
     - 
     - 
     -

   * - 13
     - BOOTMODE
     - I/O
     - BOOTMODE
     - I²S_MCLK/LSPI_CS/PWM2/I²S_DO
     - 20MHz
     - UP/DOWN
     - 12mA

   * - 14
     - PA_1
     - I/O
     - JTAG_CK
     - JTAG_CK/I²C_SCL/PWM3/I²S_LRCK/ADC0
     - 20MHz
     - UP/DOWN
     - 12mA

   * - 15
     - PA_4
     - I/O
     - JTAG_SWO
     - JTAG_SWO/I²C_SDA/PWM4/I²S_BCK/ADC1
     - 20MHz
     - UP/DOWN
     - 12mA

   * - 16
     - PA_7
     - I/O
     - GPIO, 输入, 高阻
     - PWM4/LSPI_MOSI/I²S_MCK/I²S_DI /Touch0
     - 20MHz
     - UP/DOWN
     - 12mA

   * - 17
     - VDD33IO
     - P
     - IO 电源，3.3V
     - 
     - 
     - 
     -

   * - 18
     - PB_0
     - I/O
     - GPIO, 输入, 高阻
     - PWM0/LSPI_MISO/UART3_TX/PSRAM_CK/Touch3
     - 80MHz
     - UP/DOWN
     - 12mA

   * - 19
     - PB_1
     - I/O
     - GPIO, 输入, 高阻
     - PWM1/LSPI_CK/UART3_RX/PSRAM_CS/Touch4
     - 80MHz
     - UP/DOWN
     - 12mA

   * - 20
     - PB_2
     - I/O
     - GPIO, 输入, 高阻
     - PWM2/LSPI_CK/UART2_TX/PSRAM_D0/Touch5
     - 80MHz
     - UP/DOWN
     - 12mA

   * - 21
     - PB_3
     - I/O
     - GPIO, 输入, 高阻
     - PWM3/LSPI_MISO/UART2_RX/PSRAM_D1/Touch6
     - 80MHz
     - UP/DOWN
     - 12mA

   * - 22
     - PB_4
     - I/O
     - GPIO, 输入, 高阻
     - LSPI_CS/UART2_RTS/UART4_TX/PSRAM_D2/Touch7
     - 80MHz
     - UP/DOWN
     - 12mA

   * - 23
     - PB_5
     - I/O
     - GPIO, 输入, 高阻
     - LSPI_MOSI/UART2_CTS/UART4_RX/PSARM_D3/Touch8
     - 80MHz
     - UP/DOWN
     - 12mA

   * - 24
     - VDD33IO
     - P
     - IO 电源，3.3V
     - 
     - 
     - 
     - 

   * - 25
     - CAP
     - I
     - 外接电容，4.7µF
     - 
     - 
     - 
     - 

   * - 26
     - PB_6
     - I/O
     - GPIO, 输入, 高阻
     - UART1_TX/MMC_CLK/HSPI_CK/SDIO_CK/Touch9
     - 50MHz
     - UP/DOWN
     - 12mA

   * - 27
     - PB_7
     - I/O
     - GPIO, 输入, 高阻
     - UART1_RX/MMC_CMD/HSPI_INT/SDIO_CMD/Touch10
     - 50MHz
     - UP/DOWN
     - 12mA

   * - 28
     - PB_8
     - I/O
     - GPIO, 输入, 高阻
     - I²S_BCK/MMC_D0/PWM_BREAK/SDIO_D0/Touch11
     - 50MHz
     - UP/DOWN
     - 12mA

   * - 29
     - PB_9
     - I/O
     - GPIO, 输入, 高阻
     - I²S_LRCK/MMC_D1/HSPI_CS/SDIO_D1/Touch12
     - 50MHz
     - UP/DOWN
     - 12mA

   * - 30
     - PB_10
     - I/O
     - GPIO, 输入, 高阻
     - I²S_DI/MMC_D2/HSPI_DI/SDIO_D2
     - 50MHz
     - UP/DOWN
     - 12mA

   * - 31
     - VDD33IO
     - P
     - IO 电源，3.3V
     - 
     - 
     - 
     - 

   * - 32
     - PB_11
     - I/O
     - GPIO, 输入, 高阻
     - I²S_DO/MMC_D3/HSPI_DO/SDIO_D3
     - 50MHz
     - UP/DOWN
     - 12mA

   * - 33
     - GND
     - P
     - 接地
     - 
     - 
     - 
     - 

.. _application-program-w800:

应用程序开发
------------

HLK-W800-KIT 上电前，请首先确认开发板完好无损。

现在，请前往快速入门中的 :ref:`搭建编译环境<compiling-environment>` 章节，查看如何设置开发环境，并尝试将示例项目烧录至你的开发板。

.. _schematic-diagram-w800:

开发板原理图与布局示意图
-------------------------

HLK-W800-KIT 开发板的原理图如下。

.. figure:: ../../_static/get_started/hardware/hlk_w800_kit_schematic_diagram.png
    :align: center
    :alt: HLK-W800-KIT 开发板原理图

HLK-W800-KIT 开发板的布局示意图如下。

.. figure:: ../../_static/get_started/hardware/hlk_w800_kit_layout_diagram.png
    :align: center
    :alt: HLK-W800-KIT 开发板布局示意图


参考资料下载
-------------------------

- `点击下载 <../../../../download/board/w800_hlk_board_v1.0.zip>`__
