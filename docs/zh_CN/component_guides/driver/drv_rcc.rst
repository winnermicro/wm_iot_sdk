.. _rcc:

RCC
=============

简介
-------------
RCC 负责管理各个时钟源和时钟分频，为各个外设提供时钟使能。

功能列表
-------------

- 支持开关其它模块的时钟，比如： I2C、UART、SPI、DMA 等。
- 支持复位其它模块时钟。
- 支持 CPU/WLAN/Peripheral 等的频率设置。
- CPU 默认频率 和 WLAN 默认频率 是在 Device table 中定义，也可通过 RCC API 在系统起来后进行修改。


RCC HW 支持的功能
---------------------
.. only:: w800

    请参考寄存器手册的时钟与复位模块章节。


各功能介绍
-------------

开关时钟
^^^^^^^^^^^^^^^^^^^
**相关时序 API:**

- 调用 ``wm_drv_clock_init``    初始化时钟， 如果设备已经初始化，则通过 wm_dt_get_device_by_name 函数来获取设备指针
- 调用 ``wm_drv_clock_enable``  打开对应模块的时钟
- 调用 ``wm_drv_clock_disable`` 关闭对应模块时钟

**支持开关的时钟模块如下**

- I2C, UART, SPI, DMA, RF, TIMER, GPIO, SD_ADC, PWM, LCD, I2S, RSA, 
- GPSEC, SDIOM, QSRAM, BT, TOUCH


时钟复位
^^^^^^^^^^^^^
**相关时序 API:**

- 调用 ``wm_drv_clock_init`` 初始化时钟， 如果设备已经初始化，则通过 wm_dt_get_device_by_name 函数来获取设备指针
- 调用 ``wm_drv_clock_reset`` 复位模块时钟


**结果:**

- 复位成功之后，对应模块回到初始化状态，比如对SPI复位，则SPI模块将会恢复到初始化状态


**可复位的模块:**

- BBP, MAC, SEC, SDIO_AHB, DMA, MEM_MNG, APB, I2C, UART,SPIM, SPIS
- RF, GPIO, TIMER, SAR_ADC, PWM, LCD, I2S, RSA, QPSEC, SDIOM, BT, FLASH, TOUCH
  
.. warning:: 在系统运行起来之后，某些模块不能够再复位，如果强制复位可能会造成系统无法正常运行



时钟频率设置
^^^^^^^^^^^^^^^^^

- 设置对应模块的时钟

**相关时序 API:**

- 调用 ``wm_drv_clock_init`` 初始化时钟， 如果设备已经初始化，则通过 wm_dt_get_device_by_name 函数来获取设备指针
- 调用 ``wm_drv_rcc_config_clock`` 设置指定时钟模块频率
- 调用 ``wm_drv_rcc_get_config_clockk`` 查询指定模块当前时钟频率

**可配置时钟的模块如下:**

- CPU, WLAN, SD_ADC, QFLASH, GPSEC, RSA, PERIPHERAL

.. warning:: 随意设置时钟可能会导致系统无法正常运行，可参考时钟树对相关模块进行频率配置


时钟树
^^^^^^^^^^^^^^^^^
各个模块时钟关系如下图所示

.. only:: w800

    .. figure:: ../../../_static/component-guides/driver/W800_clock_tree.svg
        :align: center
        :scale: 100%
        :alt: SPI clock tree


    .. note::
        1：当 CPU 频率低于 40 MHz时，为了保证 CPU 和 APB 时钟比值为正整数，需要把 WLAN 的时钟频率固定分为 CPU 频率的四分之一，这样保证 CPU 和 APB 的比值时钟为 1.

        2：芯片低功耗模式时，STANDBY 只能使用 RC32K 作为时钟源；Sleep 模式的时候可以选择 RC32 和 40M 分频的 25 KHz作为时钟源。

        3：作为从设备的 SDIO_SLAVE、HSPI、LSPI_SLAVE、I2S_SLAVE 时，时钟主要来自于主设备。


应用实例
-------------

  RCC 基本示例请参照 :ref:`examples/peripheral/rcc<peripheral_example>`

| Clock initial范例

.. code:: C

    dev = wm_drv_clock_init("rcc");
    if (dev != NULL) {
        wm_drv_clock_disable(dev, WM_RCC_CLOCK_ALL);
        wm_drv_clock_enable(dev, WM_RCC_UART0_GATE_EN | WM_RCC_UART1_GATE_EN |
                   WM_RCC_RF_CFG_GATE_EN | WM_RCC_GPIO_GATE_EN |
                   WM_RCC_SD_ADC_GATE_EN | WM_RCC_TOUCH_GATE_EN); //enable UART, RF, GPIO, SD_ADC, TOUCH时钟
        wm_drv_rcc_config_clock(dev, WM_RCC_TYPE_CPU, 240);  //配置 CPU 时钟为240Mhz
    }


API参考
-------------
:ref:`label_api_rcc_drver`