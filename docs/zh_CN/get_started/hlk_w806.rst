
HLK-W806-KIT  入门指南
=========================


准备工作
----------

硬件：
~~~~~~~~~~~
- HLK-W806 开发板

- Micro USB 数据线

- USB 转串口驱动（`点击下载 CH34X 驱动 <https://doc.winnermicro.net/download/tools/all/CH341SER.zip>`_）

- PC（Windows 或 Linux）

你可以跳过介绍部分，直接前往 :ref:`应用程序开发<application-program-w806>` 章节。


概述
----------
此开发板基于联盛德 W806 的物联网应用 MCU 开发板，提供 RST 复位按键、Boot 升级按键和 USB2.0 接口，扩展
接口支持 Touch、UART、PWM、GPIO、SPI、I2C、I2S、ADC、LCD 等外设，适用于物联网应用开发者评估 W806 芯片及快速搭建应用示例。


功能说明
----------

HLK-W806-KIT  开发板的主要组件、接口及控制方式如下。

.. figure:: ../../_static/get_started/hardware/hlk_w806.png
    :align: center
    :alt: HLK-W806-KIT  开发板布局

HLK-W806-KIT  开发板的主要组件描述见下表（从左上角起顺时针顺序）。

.. list-table:: 
   :widths: 25 25 
   :header-rows: 0
   :align: center

   * - 主要组件
     - 基本介绍

   * - LDO
     - 5v 转 3.3v 的 LDO 芯片。

   * - W806
     - 开发板的主控芯片，具有 WiFi，蓝牙等功能。

   * - RST-Key
     - 系统复位按键。

   * - BOOT
     - 下载使用按键，用户按下 BOOT 按键，然后将 USB 线插入到 UART0 口，松开 BOOT 按键后进入下载模式。

   * - I/O
     - W800 的所有管脚都已引出至开发板的排针，用户可以对 W800 进行编程，实现 PWM、ADC、DAC、I2C和I2S等功能，详情请见 :ref:`管脚说明<base-pin-w806>`。

   * - Micro-USB2.0
     - 开发板默认的 USB 口，可作为开发板的供电口，或作为连接 PC 和开发板的通信接口。

电源选项
-----------

开发板可任一选用以下三种供电方式：

- Micro USB 供电（默认）
- 5V / GND 管脚供电。
- 3.3V / GND 管脚供电。

.. warning:: 

    上述供电模式 **不可同时连接**，否则可能会损坏开发板或电源。

.. _base-pin-w806:

管脚说明
----------

下表介绍了开发板 I/O 管脚的 **名称** 和 **功能** ，具体布局请见 :ref:`开发板原理图与布局示意图<schematic-diagram-w806>` 章节。

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
     - PB_18
     - I/O
     - GPIO, 输入, 高阻
     - UART5_TX/LCD_SEG30
     - 10MHz
     - UP/DOWN
     - 12mA

   * - 2
     - PB_26
     - I/O
     - GPIO, 输入, 高阻
     - LSPI_MOSI/PWM4/LCD_SEG1
     - 20MHz
     - UP/DOWN
     - 12mA

   * - 3
     - PB_25
     - I/O
     - GPIO, 输入, 高阻
     - LSPI_MISO/PWM3/LCD_COM0
     - 20MHz
     - UP/DOWN
     - 12mA

   * - 4
     - PB_24
     - I/O
     - GPIO, 输入, 高阻
     - LSPI_CK/PWM2/LCD_SEG2
     - 20MHz
     - UP/DOWN
     - 12mA

   * - 5
     - PB_22
     - I/O
     - GPIO, 输入, 高阻
     - UART0_CTS/PCM_CK/LCD_COM2
     - 10MHz
     - UP/DOWN
     - 12mA

   * - 6
     - PB_21
     - I/O
     - GPIO, 输入, 高阻
     - UART0_RTS/PCM_SYNC/LCD_COM1
     - 10MHz
     - UP/DOWN
     - 12mA

   * - 7
     - PB_20
     - I/O
     - UART_RX
     - UART0_RX/PWM1/UART1_CTS/I2C_SCL
     - 10MHz
     - UP/DOWN
     - 12mA

   * - 8
     - PB_19
     - I/O
     - UART_TX
     - UART0_TX/PWM0/UART1_RTS/I2C_SDA
     - 10MHz
     - UP/DOWN
     - 12mA

   * - 9
     - WAKEUP
     - I
     - WAKEUP
     - 唤醒功能
     - 
     - DOWN
     - 

   * - 10
     - RESET
     - I
     - RESET
     - 复位
     - 
     - UP
     - 

   * - 11
     - XTAL_OUT
     - O
     - 外部晶振输出
     - 
     - 
     - 
     - 

   * - 12
     - XTAL_IN
     - I
     - 外部晶振输入
     - 
     - 
     - 
     - 

   * - 13
     - VDD33
     - P
     - 芯片电源，3.3V
     - 
     - 
     - 
     - 

   * - 14
     - NC
     - 
     - 
     - 
     - 
     - 
     - 

   * - 15
     - VDD33
     - P
     - 芯片电源，3.3V
     - 
     - 
     - 
     - 

   * - 16
     - VDD33
     - P
     - 芯片电源，3.3V
     - 
     - 
     - 
     - 

   * - 17
     - VDD33
     - P
     - 芯片电源，3.3V
     - 
     - 
     - 
     - 

   * - 18
     - BOOTMODE
     - I/O
     - BOOTMODE
     - I2S_MCLK/LSPI_CS/PWM2/I2S_DO
     - 20MHz
     - UP/DOWN
     - 12mA

   * - 19
     - PA_1
     - I/O
     - JTAG_CK
     - JTAG_CK/I2C_SCL/PWM3/I2S_LRCK/ADC_1
     - 20MHz
     - UP/DOWN
     - 12mA

   * - 20
     - PA_2
     - I/O
     - GPIO, 输入, 高阻
     - UART1_RTS/UART2_TX/PWM0/UART3_RTS/ADC_4
     - 20MHz
     - UP/DOWN
     - 12mA

   * - 21
     - PA_3
     - I/O
     - GPIO, 输入, 高阻
     - UART1_CTS/UART2_RX/PWM1/UART3_CTS/ADC_3
     - 20MHz
     - UP/DOWN
     - 12mA

   * - 22
     - PA_4
     - I/O
     - JTAG_SWO
     - JTAG_SWO/I2C_SDA/PWM4/I2S_BCK/ADC_2
     - 20MHz
     - UP/DOWN
     - 12mA

   * - 23
     - PA_5
     - I/O
     - GPIO, 输入, 高阻
     - UART3_TX/UART2_RTS/PWM_BREAK/UART4_RTS/VRP_EXT
     - 20MHz
     - UP/DOWN
     - 12mA

   * - 24
     - PA_6
     - I/O
     - GPIO, 输入, 高阻
     - UART3_RX/UART2_CTS/NULL/UART4_CTS/LCD_SEG31/VRP_EXT
     - 20MHz
     - UP/DOWN
     - 12mA

   * - 25
     - PA_7
     - I/O
     - GPIO, 输入, 高阻
     - PWM4/LSPI_MOSI/I2S_MCK/I2S_DI/LCD_SEG3/Touch_1
     - 20MHz
     - UP/DOWN
     - 12mA

   * - 26
     - PA_8
     - I/O
     - GPIO, 输入, 高阻
     - PWM_BREAK/UART4_TX/UART5_TX/I2S_BCLK/LCD_SEG4
     - 20MHz
     - UP/DOWN
     - 12mA

   * - 27
     - PA_9
     - I/O
     - GPIO, 输入, 高阻
     - MMC_CLK/UART4_RX/UART5_RX/I2S_LRCLK/LCD_SEG5/TOUCH_2
     - 50MHz
     - UP/DOWN
     - 12mA

   * - 28
     - PA_10
     - I/O
     - GPIO, 输入, 高阻
     - MMC_CMD/UART4_RTS/PWM0/I2S_DO/LCD_SEG6/TOUCH_3
     - 50MHz
     - UP/DOWN
     - 12mA

   * - 29
     - VDD33
     - P
     - 芯片电源，3.3V
     - 
     - 
     - 
     - 

   * - 30
     - PA_11
     - I/O
     - GPIO, 输入, 高阻
     - MMC_DAT0/UART4_CTS/PWM1/I2S_DI/LCD_SEG7
     - 50MHz
     - UP/DOWN
     - 12mA

   * - 31
     - PA_12
     - I/O
     - GPIO, 输入, 高阻
     - MMC_DAT1/UART5_TX/PWM2/LCD_SEG8/TOUCH_14
     - 50MHz
     - UP/DOWN
     - 12mA

   * - 32
     - PA_13
     - I/O
     - GPIO, 输入, 高阻
     - MMC_DAT2/UART5_RX/PWM3/LCD_SEG9
     - 50MHz
     - UP/DOWN
     - 12mA

   * - 33
     - PA_14
     - I/O
     - GPIO, 输入, 高阻
     - MMC_DAT3/UART5_CTS/PWM4/LCD_SEG10/TOUCH_15
     - 50MHz
     - UP/DOWN
     - 12mA

   * - 34
     - PA_15
     - I/O
     - GPIO, 输入, 高阻
     - PSRAM_CK/UART5_RTS/PWM_BREAK/LCD_SEG11
     - 50MHz
     - UP/DOWN
     - 12mA

   * - 35
     - PB_0
     - I/O
     - GPIO, 输入, 高阻
     - PWM0/LSPI_MISO/UART3_TX/PSRAM_CK/LCD_SEG12/Touch_4
     - 80MHz
     - UP/DOWN
     - 12mA

   * - 36
     - PB_1
     - I/O
     - GPIO, 输入, 高阻
     - PWM1/LSPI_CK/UART3_RX/PSRAM_CS/LCD_SEG13/Touch_5
     - 80MHz
     - UP/DOWN
     - 12mA

   * - 37
     - PB_2
     - I/O
     - GPIO, 输入, 高阻
     - PWM2/LSPI_CK/UART2_TX/PSRAM_D0/LCD_SEG14/Touch_6
     - 80MHz
     - UP/DOWN
     - 12mA

   * - 38
     - PB_3
     - I/O
     - GPIO, 输入, 高阻
     - PWM3/LSPI_MISO/UART2_RX/PSRAM_D1/LCD_SEG15/Touch_7
     - 80MHz
     - UP/DOWN
     - 12mA

   * - 39
     - PB_27
     - I/O
     - GPIO, 输入, 高阻
     - PSRAM_CS/UART0_TX/LCD_COM3
     - 80MHz
     - UP/DOWN
     - 12mA

   * - 40
     - PB_4
     - I/O
     - GPIO, 输入, 高阻
     - LSPI_CS/UART2_RTS/UART4_TX/PSRAM_D2/LCD_SEG16/Touch_8
     - 80MHz
     - UP/DOWN
     - 12mA

   * - 41
     - PB_5
     - I/O
     - GPIO, 输入, 高阻
     - LSPI_MOSI/UART2_CTS/UART4_RX/PSRAM_D3/LCD_SEG17/Touch_9
     - 80MHz
     - UP/DOWN
     - 12mA

   * - 42
     - VDD33
     - P
     - 芯片电源，3.3V
     - 
     - 
     - 
     - 

   * - 43
     - CAP
     - I
     - 外接电容，4.7µF
     - 
     - 
     - 
     - 

   * - 44
     - PB_6
     - I/O
     - GPIO, 输入, 高阻
     - UART1_TX/MMC_CLK/HSPI_CK/SDIO_CK/LCD_SEG18/Touch_10
     - 50MHz
     - UP/DOWN
     - 12mA

   * - 45
     - PB_7
     - I/O
     - GPIO, 输入, 高阻
     - UART1_RX/MMC_CMD/HSPI_INT/SDIO_CMD/LCD_SEG19/Touch_11
     - 50MHz
     - UP/DOWN
     - 12mA

   * - 46
     - PB_8
     - I/O
     - GPIO, 输入, 高阻
     - I2S_BCK/MMC_D0/PWM_BREAK/SDIO_D0/LCD_SEG20/Touch_12
     - 50MHz
     - UP/DOWN
     - 12mA

   * - 47
     - PB_9
     - I/O
     - GPIO, 输入, 高阻
     - I2S_LRCK/MMC_D1/HSPI_CS/SDIO_D1/LCD_SEG21/Touch_13
     - 50MHz
     - UP/DOWN
     - 12mA

   * - 48
     - PB_12
     - I/O
     - GPIO, 输入, 高阻
     - HSPI_CK/PWM0/UART5_CTS/I2S_BCLK/LCD_SEG24
     - 50MHz
     - UP/DOWN
     - 12mA

   * - 49
     - PB_13
     - I/O
     - GPIO, 输入, 高阻
     - HSPI_INT/PWM1/UART5_RTS/I2S_LRCLK/LCD_SEG25
     - 50MHz
     - UP/DOWN
     - 12mA

   * - 50
     - PB_14
     - I/O
     - GPIO, 输入, 高阻
     - HSPI_CS/PWM2/LSPI_CS/I2S_DO/LCD_SEG26
     - 50MHz
     - UP/DOWN
     - 12mA

   * - 51
     - PB_15
     - I/O
     - GPIO, 输入, 高阻
     - HSPI_DI/PWM3/LSPI_CK/I2S_DI/LCD_SEG27
     - 50MHz
     - UP/DOWN
     - 12mA

   * - 52
     - PB_10
     - I/O
     - GPIO, 输入, 高阻
     - I2S_DI/MMC_D2/HSPI_DI/SDIO_D2/LCD_SEG22
     - 50MHz
     - UP/DOWN
     - 12mA

   * - 53
     - VDD33
     - P
     - 芯片电源，3.3V
     - 
     - 
     - 
     - 

   * - 54
     - PB_11
     - I/O
     - GPIO, 输入, 高阻
     - I2S_DO/MMC_D3/HSPI_DO/SDIO_D3/LCD_SEG23
     - 50MHz
     - UP/DOWN
     - 12mA

   * - 55
     - PB_16
     - I/O
     - GPIO, 输入, 高阻
     - HSPI_DO/PWM4/LSPI_MISO/UART1_RX/LCD_SEG28
     - 50MHz
     - UP/DOWN
     - 12mA

   * - 56
     - PB_17
     - I/O
     - GPIO, 输入, 高阻
     - UART5_RX/PWM_BREAK/LSPI_MOSI/I2S_MCLK/LCD_SEG29
     - 20MHz
     - UP/DOWN
     - 12mA

   * - 57
     - GND
     - P
     - 芯片底部接地 PAD
     - 
     - 
     - 
     - 
     
.. _application-program-w806:

应用程序开发
------------

HLK-W806-KIT  上电前，请首先确认开发板完好无损。

现在，请前往快速入门中的 :ref:`搭建编译环境<compiling-environment>` 章节，查看如何设置开发环境，并尝试将示例项目烧录至你的开发板。

.. _schematic-diagram-w806:

开发板原理图与布局示意图
-------------------------

HLK-W806-KIT  开发板的原理图如下。

.. figure:: ../../_static/get_started/hardware/hlk_w806_schematic_diagram.png
    :align: center
    :alt: HLK-W806-KIT  开发板原理图

HLK-W806-KIT  开发板的布局示意图如下。

.. figure:: ../../_static/get_started/hardware/hlk_w806_layout_diagram.png
    :align: center
    :alt: HLK-W806-KIT  开发板布局示意图


参考资料下载
-------------------------

- `点击下载 <../../../../download/board/w806_hlk_board_v1.2.zip>`__
