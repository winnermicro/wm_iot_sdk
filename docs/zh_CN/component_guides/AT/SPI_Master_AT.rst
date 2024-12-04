.. _SPIM:

SPI Master AT 命令集
====================

简介
-------------

SPI（Serial Peripheral Interface）是一种常用的串行通信协议，主要用于微控制器和各种外围设备之间的数据传输。它是一种全双工通信方式，允许数据同时在两个方向上传输。
在开始 AT 命令之前，需提前将 SPI master 和 SPI slave 连接好

功能列表
-------------

    1. :ref:`AT+DRVSPIMCONFGPIO <cmd-DRVSPIMCONFGPIO>`: 配置 SPI GPIO
    2. :ref:`AT+DRVSPIMINIT <cmd-DRVSPIMINIT>`: 初始化SPI Master驱动
    3. :ref:`AT+DRVSPIMRD <cmd-DRVSPIMRD>`: 读取 SPI 数据
    4. :ref:`AT+DRVSPIMWR <cmd-DRVSPIMWR>`: 写入 SPI 数据

功能概述
-------------
.. only:: w800

   W800 实现了同步的 SPI 主从功能。其工作时钟为系统内部总线时钟，master 支持 motorola spi 的 4 种格式（CPOL，CPHA），TI 时序，macrowire 时序，
   发送和接收长度没有限制，最高支持 20MHz clock


   W800 共有 3 组 12 个 GPIO 引脚可用于 SPI，其中 CS 固定由 SW 控制，空闲的GPIO引脚都可以作为CS使用，默认使用GPIO26
   SPI 引脚的关系如下表

   .. list-table::
      :widths: 25 25 25 25 25
      :header-rows: 0
      :align: center

      * - SPI 功能
        - 引脚编号
        - 引脚名称
        - 引脚复用功能
        - 引脚属性

      * - MOSI0
        - PIN_NUM_7
        - PA7
        - OPT2
        - output

      * - MOSI1
        - PIN_NUM_21
        - PB5
        - OPT1
        - output

      * - MOSI2
        - PIN_NUM_33
        - PB17
        - OPT1
        - output

      * - MOSI3
        - PIN_NUM_42
        - PB26
        - OPT1
        - output

      * - MISO0
        - PIN_NUM_16
        - PB0
        - OPT2
        - float/input

      * - MISO1
        - PIN_NUM_19
        - PB3
        - OPT2
        - float/input

      * - MISO2
        - PIN_NUM_32
        - PB16
        - OPT3
        - float/input

      * - MISO3
        - PIN_NUM_41
        - PB125
        - OPT1
        - float/input

      * - SCK0
        - PIN_NUM_17
        - PB1
        - OPT2
        - output

      * - SCK1
        - PIN_NUM_18
        - PB2
        - OPT2
        - output

      * - SCK2
        - PIN_NUM_31
        - PB15
        - OPT3
        - output

      * - SCK3
        - PIN_NUM_40
        - PB24
        - OPT1
        - output

      * - CS0
        - PIN_NUM_0
        - PA0
        - OPT5
        - output

      * - CS1
        - PIN_NUM_20
        - PB4
        - OPT5
        - output

      * - CS2
        - PIN_NUM_30
        - PB14
        - OPT5
        - output

      * - CS3
        - PIN_NUM_39
        - PB23
        - OPT5
        - output

.. note::
    需要在SPIM初始化之后调用，不然CLOCK, MISO以及MOSI的配置会被SPIM初始化过程中的默认值覆盖

.. _cmd-DRVSPIMCONFGPIO:

:ref:`AT+DRVSPIMCONFGPIO <cmd-DRVSPIMCONFGPIO>`: 配置 SPI GPIO
-------------------------------------------------------------------------------------------

设置命令
^^^^^^^^^^^

**命令：**

::

    AT+DRVSPIMCONFGPIO=<mosi>,<miso>,<sclk>,<cs>

**响应：**

::

    OK

参数
^^^^^^^^^^

-  **<mosi>**: 主设备到从设备的数据线
-  **<miso>**: 从设备到主设备的数据线
-  **<sclk>**: 主设备生成的时钟信号线
-  **<cs>**:  用于选择特定的从设备


示例
^^^^

::

    AT+DRVSPIMCONFGPIO=7,16,17,0 //配置 mosi：GPIO7, miso：GPIO16, sclk：GPIO17, cs：GPIO0, 

.. _cmd-DRVSPIMINIT:

:ref:`AT+DRVSPIMINIT <cmd-DRVSPIMINIT>`: 初始化 SPI 主机驱动
-------------------------------------------------------------------------------------------

设置命令
^^^^^^^^^^^

**命令：**

::

    AT+DRVSPIMINIT=<clock>,<mode>

**响应：**

::

    OK

参数
^^^^^^^^^^

-  **<clock>**: 时钟速度，目前支持10KHz~20MHz
-  **<mode>**: SPI 模式，范围：0 ~ 3

说明
^^^^^

-  需在 SPI 初始化前配置 SPI GPIO

示例
^^^^

::

    AT+DRVSPIMINIT=2000000,0  //设置 SPI 模式为 0， 2MHz clock


.. _cmd-DRVSPIMRD:

:ref:`AT+DRVSPIMRD <cmd-DRVSPIMRD>`: 读取 SPI 数据
-------------------------------------------------------------------------------------------

设置命令
^^^^^^^^^^^

**命令：**

::

    AT+DRVSPIMRD=<data_len>

**响应：**

::

    OK

参数
^^^^^^^^^^

-  **<data_len>**: 读取数据长度


示例
^^^^

::

    AT+DRVSPIMRD=10  // 读取 10 字节数据



.. _cmd-DRVSPIMWR:

:ref:`AT+DRVSPIMWR <cmd-DRVSPIMWR>`: 写入 SPI 数据
-------------------------------------------------------------------------------------------

设置命令
^^^^^^^^^^^

**命令：**

::

    AT+DRVSPIMWR=<data_len>

**响应：**

::

    OK

参数
^^^^^^^^^^

-  **<data_len>**: 写入数据长度


示例
^^^^

::

    AT+DRVSPIMWR=10  // 写入 10 字节数据

