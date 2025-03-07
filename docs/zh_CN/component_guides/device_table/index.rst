.. _lable_device_table:

设备表
==============

简介
-------------

设备表当前支持两种使用方式：
    * 使用 TOML 文件配置，这是当前 **推荐** 的使用方式
    * 使用 C 语言文件配置，这是传统的使用方式

对于这两种方式，使用者任选其一即可，默认使用 TOML 文件配置。
当前的设备表配置流程说明如下图：

.. figure:: ../../../_static/component-guides/device_table/wm_device_table.svg
    :align: center
    :alt: 设备表配置

使用 TOML 文件配置
-------------------

`TOML <https://toml.io/cn>`_ 文件是一种文本格式的配置文件，
它的语义明显易于阅读，而且支持添加注释信息，非常方便人类阅读使用。

TOML 文件有用一套完整的语法规范，当前语法详情可参阅：

.. toctree::
   :maxdepth: 1

   TOML v1.0.0-rc.2 <toml_spec_v1.0.0-rc.2>


WM IoT SDK 当前使用的 TOML 配置文件默认位置为 ``components/wm_dt/config/<soc>/device_table.toml``，
当工程根目录下存在 ``device_table.toml`` 文件时，会优先使用工程根目录下的 ``device_table.toml`` 文件。

当用户需要修改配置文件时，不推荐直接修改组件目录中的 ``device_table.toml`` 默认文件，
而是推荐从组件目录中复制一份至工程根目录，修改工程根目录下的 ``device_table.toml`` 文件。

``device_table.toml`` 文件可用任意文本编辑工具进行编辑，但对于部分编辑器可能需要安装插件才能支持语法显示，此处不再介绍。

当启动编译后，构建系统首先会将检查项目根目录下是否有 ``device_table.toml`` 文件，如果有则将此 ``device_table.toml`` 文件复制为 ``build/device_table/device_table.toml``，之后将 **始终** 使用 ``build/device_table/device_table.toml`` 文件作为当前的配置，脚本会将 ``build/device_table/device_table.toml`` 文件转为 C 代码后参与编译，转换的 C 代码在 ``build/device_table/`` 路径下。
如果项目根目录下不存在 ``device_table.toml`` 文件，则选择将 ``components/wm_dt/config/<soc>/device_table.toml`` 复制为 ``build/device_table/device_table.toml``，之后的流程同上。

当开发完成之后，可以使用命令 ``wm.py saveconfig`` 将当前调好的设备表配置保存为工程根目录下的 ``device_table.toml`` 文件。

设备表配置网页配置系统
^^^^^^^^^^^^^^^^^^^^^^^

为了进一步简化对 ``device_table.toml`` 文件的配置，提供了一套网页系统，通过网页用户可以直接在网页进行勾选、输入等操作，而无需关心 TOML 文件规范和 SDK 中的设备属性。

使用时，可执行命令 ``wm.py devconfig`` 自动打开网页。
对于非桌面环境的用户则不能使用此命令，只能选择手动编辑 ``device_table.toml`` 文件或 C 代码方式。

网页中显示的内容，受到 menuconfig 菜单中的 SoC 类型和驱动选项影响，可能因为不同的 SoC 类型或裁剪了驱动而显示不同。

.. note::
    网页配置系统采用保活检查机制，当网页不活跃时间超过 5 秒之后，网页配置系统就会自动停止。
    当浏览器失去焦点、最小化等场景，不同的系统、浏览器可能因为其控制策略会导致保活失效。

TOML 配置文件格式
^^^^^^^^^^^^^^^^^^^^^^^

WM IoT SDK 使用的 ``device_table.toml`` 文件，其中内容由如下格式组成（摘选了部分）：

.. code:: toml

    # WM IoT SDK Device Table
    table_name = "default"

    [[dev]]
    dev_name = "uart0"
    reg_base = 0x40010600
    init_cfg = {init_level = "system", init_priority = 50}
    uart_cfg = {baudrate = 115200, parity = "none", stop_bits = 1, data_bits = 8, flow_ctrl = "none"}
    irq_cfg  = {irq_num = 16, irq_priority = 0}
    pin_cfg  = [{pin = 35, func = "fun1"}, #tx
                {pin = 36, func = "fun1"}, #rx
                {pin = 37, func = "fun1"}, #rts
                {pin = 38, func = "fun1"}] #cts
    dma_device = "dma"

    [[dev]]
    dev_name = "timer0"
    reg_base = 0x40011800
    init_cfg = {init_level = "app", init_priority = 50}
    irq_cfg  = {irq_num = 30, irq_priority = 0}

    [[dev]]
    dev_name = "seg_lcd"
    reg_base = 0x40011C00
    init_cfg = {init_level = "app", init_priority = 50}
    seg_lcd_cfg = {duty_sel = 3, vlcd_cc = 0, bias = 2, hd = 0, frame_freq = 60, com_num = 4}

    [[dev]]
    dev_name = "gdc0689"
    init_cfg = {init_level = "app", init_priority = 50}
    pin_cfg  = [{pin = 42, func = "fun6"},
                {pin = 40, func = "fun6"},
                {pin = 7,  func = "fun6"},
                {pin = 8,  func = "fun6"},
                {pin = 25, func = "fun6"},
                {pin = 26, func = "fun6"},
                {pin = 27, func = "fun6"},
                {pin = 28, func = "fun6"},
                {pin = 29, func = "fun6"}]
    seg_lcd_device = "seg_lcd"


设备表选项介绍
^^^^^^^^^^^^^^^^^^^^^^^

其中公共信息：

    * ``table_name`` 用来指定此配置文件对应的设备表名称，可选项，取值为字符串，

      此选项在使用多套设备表配置的场景时有用，其它情况下可忽略。

    * ``[[dev]]`` 表示一个具体的设备配置，每个设备都由此标记开始。
    * ``dev_name`` 表示该设备的名称，取值为字符串，在代码中使用时该设备以此为标识。
    * ``reg_base`` 表示该设备的寄存器基址，取值为十六进制整数。
    * ``irq_cfg`` 表示该设备中断信息，其包含两个成员选项：
        * ``irq_num`` 表示中断号，取值为十进制整数，取值范围 0 - 31。
        * ``irq_priority`` 表示中断优先级，取值为十进制整数，取值范围 0 - 3，取值越大表示优先级越高。
    * ``init_cfg`` 表示该设备的初始化配置信息，其包含两个成员选项：
        * ``init_level`` 表示初始化方式，字符串类型，

          取值为 ``system`` 表示由系统自动完成该设备初始化，

          取值为 ``app`` 表示由用户自己完成该设备初始化。
        * ``init_priority`` 表示初始化优先级，取值为十进制整数，取值范围 0 - 100，

          取值越大表示优先级越高，优先级高的先初始化。相同优先级的设备在这一优先级被随机初始化。

    .. _label_pincfg:

    * ``pin_cfg`` 表示该设备的的引脚配置信息，其包含四个成员选项：
        * ``pin`` 表示引脚编号，必选项，取值为十进制整数，取值范围 0 - 45。
        * ``fun`` 表示引脚复用功能，必选项，取值为字符串，

          可取值为 ``fun1``、``fun2``、``fun3``、``fun4``、``fun5``、``fun6``、``fun7``。

          具体的复用关系参考 :ref:`pinmux` 。
        * ``dir`` 表示引脚输入输出属性，可选项，取值为字符串，省略时默认为输入属性。

          可取值为 ``input``、``output`` 分别表示输入、输出。
        * ``pupd`` 表示引脚上下拉属性，可选项，取值为字符串，，省略时默认为悬空属性。

          可取值为 ``float``、``pullup``、``pulldown`` 分别表示悬空、上拉、下拉。

可选项可以直接在配置文件中省略，完全不填写。但如果填写了，则填写的格式必须要符合要求。

.. _label_back:

其中各设备独有信息：

    * ``rcc_cfg`` 表示 RCC 配置信息，其具体选项在 :ref:`rcc_config_section` 章节介绍。
    * ``gpio_cfg`` 表示配置 GPIO 配置信息，其具体选项在 :ref:`gpio_config_section` 章节介绍。
    * ``seg_lcd_cfg`` 表示段码屏配置信息，其具体选项在 :ref:`seglcd_config_section` 章节介绍。
    * ``uart_cfg`` 表示串口配置信息，其具体选项在 :ref:`uart_config_section` 章节介绍。
    * ``i2c_cfg`` 表示 I2C 配置信息，其具体选项在 :ref:`i2c_config_section` 章节介绍。
    * ``pmu_cfg`` 表示 PMU 配置信息，其具体选项在 :ref:`pmu_config_section` 章节介绍。
    * ``touch_button_cfg`` 表示触摸按键配置信息，其具体选项 :ref:`touch_button_config_section` 章节介绍。
    * ``adc_cfg`` 表示 ADC 配置信息，其具体选项在 :ref:`adc_config_section` 章节介绍。
    * ``eeprom_cfg`` 表示 EEPROM 配置信息，其具体选项在 :ref:`eeprom_config_section` 章节介绍。
    * ``spi_cfg`` 表示 SPI 配置信息，其具体选项在 :ref:`spi_config_section` 章节介绍。
    * ``i2s_cfg`` 表示 I2S 配置信息，其具体选项在 :ref:`i2s_config_section` 章节介绍。
    * ``sdh_cfg`` 表示 SD/MMC 配置信息，其具体选项在 :ref:`sdmmc_config_section` 章节介绍。
    * ``tftlcd_cfg`` 表示 TFT LCD 配置信息，其具体选项在 :ref:`tftlcd_config_section` 章节介绍。
    * ``wdt_cfg`` 表示看门狗配置信息，其具体选项在 :ref:`wdt_config_section` 章节介绍。
    * ``psram_cfg`` 表示 PSRAM 配置信息，其具体选项在 :ref:`psram_config_section` 章节介绍。
    * ``flash_cfg`` 表示 片内、片外 Flash 配置信息，其具体选项在 :ref:`flash_config_section` 章节介绍。

    * ``dma_device`` 表示该设备需要使用 DMA，并指明了其所用的 DMA 控制器的名称，取值为字符串。
        - 该项存在与否，可以决定是否使用 DMA 功能，如 UART 中省略该配置则 UART 驱动将不会启用 DMA 功能。
    * ``irq_device`` 表示该设备需要使用 IRQ，并指明了其所用的 IRQ 控制器的名称，取值为字符串。
    * ``rcc_device`` 表示该设备需要使用 RCC，并指明了其所用的 RCC 控制器的名称，取值为字符串。
    * ``seg_lcd_device`` 表示该设备需要使用段码屏，并指明了其所用的段码屏控制器的名称，取值为字符串。
    * ``pinmux_device`` 表示该设备需要使用 PINMUX，并指明了其所用的 PINMUX 控制器的名称，取值为字符串。
    * ``touch_sensor_device`` 表示该设备需要使用触摸传感器，并指明了其所用的触摸传感器控制器的名称，取值为字符串。
    * ``i2c_device`` 表示该设备需要使用 I2C，并指明了其所用的 I2C 控制器的名称，取值为字符串。
    * ``spi_device`` 表示该设备需要使用 SPI，并指明了其所用的 SPI 控制器的名称，取值为字符串。
    * ``gpio_device`` 表示该设备需要使用 GPIO，并指明了其所用的 GPIO 控制器的名称，取值为字符串。

.. _rcc_config_section:

:ref:`RCC 配置 <label_back>`
+++++++++++++++++++++++++++++++

对于 RCC 模块，其在配置文件中的配置选项 ``rcc_cfg`` 包含如下选项：

    * ``type`` 表示系统中不同模块的时钟类型，可取值为 ``peripheral``、``wlan``、``cpu``、``adc``、``qflash``、``gpsec``、``rsa``、``apb``。
    * ``clock`` 表示每个总线或控制器的时钟频率，取值为十进制整数，单位为 MHz。
        - SD ADC 时钟：可选值为 1。
        - WLAN 时钟：可选值为 160（只有 160 MHz WiFi 才能正常使用）。
        - CPU 时钟：可选值为 240、160、120、96、80、60、48、40、32、30、24、20、16、15、12、10、8、6、5、4、3、2。
        - QFLASH 时钟：可选值为 80。
        - GPSEC 时钟：可选值为 160。
        - RSA 时钟：可选值为 160。
        - APB 时钟：最高可配值为 40，一般而言不推荐配置更低。

.. _gpio_config_section:

:ref:`GPIO 配置 <label_back>`
++++++++++++++++++++++++++++++++

对于 GPIO 模块，其在配置文件中的配置选项 ``gpio_cfg`` 包含如下选项：

    * ``pin_num``、``pin_mux``、``pin_dir``、``pin_pupd`` 可参考 :ref:`pin_cfg <label_pincfg>` 。
    * ``int_mode`` 表示配置 GPIO 引脚的中断触发模式，可取值为 ``falling_edge``、``rising_edge``、``double_edge``、``low_level``、``high_level``。

.. _seglcd_config_section:

:ref:`SEG LCD 配置 <label_back>`
++++++++++++++++++++++++++++++++++++

对于段码屏设备，其在配置文件中的配置选项 ``seg_lcd_cfg`` 包含如下选项：

    * ``duty_sel`` 表示 LCD显示的占空比，可取值为分数，可取值为 ``static``、``1/2``、``1/3``、``1/4``、``1/5``、``1/6``、``1/7``、``1/8``。
    * ``vlcd_cc`` 表示 LCD 显示的电压选择，取值为浮点数带单位，可取值为 ``2.7v``、``2.9v``、``3.1v``、``3.3v``。
    * ``bias`` 表示 LCD 的偏置电压，可取值为分数，可取值为 ``1/4``、``1/2``、``1/3``、``static``。
    * ``hd`` 表示 LCD 驱动的高电平状态，取值为十进制整数，可取值为 ``low``、    ``high``。
    * ``frame_freq`` 表示 LCD 的刷新帧频，可取值为十进制整数。
    * ``com_num`` 表示 LCD 的公共端数量，可取值为十进制整数，可取值为 ``1``、``2``、``3``、``4``、``5``、``6``、``7``、``8``。

.. _uart_config_section:

:ref:`UART 配置 <label_back>`
++++++++++++++++++++++++++++++++

对于串口设备，其在配置文件中的配置选项 ``uart_cfg`` 包含如下选项：

    * ``baudrate`` 表示串口波特率，取值为十进制整数，可取值为 ``600``、``1200``、``1800``、``2400``、``4800``、``9600``、``19200``、``38400``、``57600``、``115200``、``230400``、``460800``、``921600``、``1000000``、``1250000``、``1500000``、``2000000``。
    * ``parity`` 表示串口奇偶性，取值为字符串，可取值为 ``none``、``even``、``odd``。
    * ``stop_bits`` 表示串口停止位，取值为十进制整数，可取值为 ``1``、``2``。
    * ``data_bits`` 表示串口数据位，取值为十进制整数，可取值为 ``5``、``6``、``7``、``8``。
    * ``flow_ctrl`` 表示串口流控选项，取值为字符串，可取值为 ``none``、``rts``、``cts``、``rts_cts``。

.. _i2c_config_section:

:ref:`I2C 配置 <label_back>`
++++++++++++++++++++++++++++++++

对于 I2C 设备，其在配置文件中的配置选项 ``i2c_cfg`` 包含如下选项：

    * ``max_clock`` 表示 I2C 总线的最大时钟频率（单位：Hz），取值为十进制整数。
    * ``addr_10_bits`` 表示是否启用 10 位 I2C 地址模式，可取值为布尔值： ``true``、``flase``，WM800 系列只支持 7 位地址。

.. _pmu_config_section:

:ref:`PMU 配置 <label_back>`
++++++++++++++++++++++++++++++++

对于 PMU，其在配置文件中包含如下选项：

    * ``clk_src`` 表示时钟源选择，控制系统中不同模块的时钟来源，可取值为 ``internal`` 和 ``external`` 。

        - internal：使用内部 32 KHz 晶振作为时钟源、
        - external：使用外部 40 MHz 晶振通过分频的时钟源。

.. _touch_button_config_section:

:ref:`触摸按钮配置 <label_back>`
++++++++++++++++++++++++++++++++++
对于触摸按键模块，其在配置文件中的配置选项 ``touch_button`` 包含两个成员选项：

    * ``key_num`` 表示触摸按键编号，取值为十进制整数，取值范围为 1 - 12、15。
    * ``threshold`` 表示触摸检测阀值，取值为十进制整数，取值范围为 0 - 127。

.. _adc_config_section:

:ref:`ADC 配置 <label_back>`
++++++++++++++++++++++++++++++++

对于 ADC 设备，其在配置文件中的配置选项 ``adc_cfg`` 包含的成员为：

    * ``adc_channel`` 表示指定 ADC 通道，用于选择要采样的输入信号，可取值为 ``0``、``1``、``2``、``3``、``8``、``9``。

        - 0 表示使用 ADC 通道 0
        - 1 表示使用 ADC 通道 1
        - 2 表示使用 ADC 通道 2
        - 3 表示使用 ADC 通道 3
        - 8 表示使用 ADC 通道 0 和 ADC 通道 1 差分
        - 9 表示使用 ADC 通道 2 和 ADC 通道 3 差分

    * ``pga_gain1`` 表示PGA（可编程增益放大器）第一个增益级别的配置，字符串格式，可取值为 ``level0``、``level1``、``level2``、``level3``、``level4``、``level5``。
    * ``pga_gain2`` 表示PGA（可编程增益放大器）第二个增益级别的配置，字符串格式，可取值为 ``level0``、``level1``、``level2``、``level3``。
    * ``adc_cmp`` 表示启用或禁用 ADC 比较器功能，可取值为布尔值： ``true``、``flase``。
    * ``cmp_data`` 表示比较值，十进制整数，当 ``adc_cmp`` 取值为 ``true`` 时才有效。``adc_cmp`` 取值为 ``false`` 时可省略该参数。
    * ``cmp_pol`` 表示 ADC 比较功能中的中断触发方式，取值为十进制整数 0 和 1。

        - 0 表示 ADC 实际结果 大于等于 cmp_data 时产生 ADC 中断
        - 1 表示 ADC 实际结果 小于 cmp_data 时产生 ADC 中断

.. _eeprom_config_section:

:ref:`EEPROM 配置 <label_back>`
++++++++++++++++++++++++++++++++++++++

对于 EEPROM 设备，其在配置文件中包含如下选项：

    * ``speed_hz`` 表示EEPROM 的 I2C 通信速率，单位赫兹（Hz），取值为十进制整数。
    * ``size`` 表示 EEPROM 的总体容量，单位字节，取值为十进制整数。
    * ``i2c_addr`` 表示 EEPROM 的 I2C 地址，取值为十六进制整数。
    * ``page_size`` 表示定义 EEPROM 页的大小，决定每次写入的字节数，取值为十进制整数。
    * ``addr_width`` 表示设置 EEPROM 地址的宽度，取值为十进制整数。
    * ``read_only`` 表示 EEPROM 是否只读，可取值为布尔值： ``true``、``flase``。
    * ``max_write_time_ms`` 设置 EEPROM 写操作的最大延时，确保写入操作完成，单位毫秒（ms），取值为十进制整数。
    * ``pin_cfg`` 表示所用到特殊引脚（如写保护等），可选配置，如无可省略，格式同 :ref:`pin_cfg <label_pincfg>` 。

.. _spi_config_section:

:ref:`SPI 配置 <label_back>`
++++++++++++++++++++++++++++++

对于使用 SPI 的设备，其在配置文件中的配置选项 ``spi_cfg`` 包含如下选项：

    * ``mode`` 表示示 SPI 的工作模式，控制 SPI 时钟的极性（CPOL）和相位（CPHA），不同的外设可能需要不同的模式来正常通信，可取值为 ``0``、``1``、``2``、``3``。

        - 0 表示 CPOL=0, CPHA=0
        - 1 表示 CPOL=0, CPHA=1
        - 2 表示 CPOL=1, CPHA=0
        - 3 表示 CPOL=1, CPHA=1

    * ``freq`` 表示 SPI 设备的工作频率，单位为赫兹（Hz），取值为十进制整数。
    * ``pin_cs`` 表示该设备的的引脚配置信息，其配置信息参考 :ref:`pin_cfg <label_pincfg>` 。

.. _i2s_config_section:

:ref:`I2S 配置 <label_back>`
++++++++++++++++++++++++++++++

对于 I2S，其在配置文件中的配置选项 ``i2s_cfg`` 包含如下选项：

    * ``extal_clock_en`` 表示是否启用外部时钟源，取值为布尔值，可取值为 ``true``、``false``。
    * ``mclk_hz`` 表示 I2S 接口的主时钟频率（MCLK），单位是赫兹（Hz），取值为十进制整数。

.. _sdmmc_config_section:

:ref:`SD/MMC 配置 <label_back>`
+++++++++++++++++++++++++++++++++

对于 SD/MMC 设备，其在配置文件中的配置选项 ``sdh_cfg`` 包含如下选项：

    * ``speed_hz`` 表示 SD/MMC 的时钟频率，用于确定数据传输的速度，取值为十进制整数，单位赫兹（Hz），支持 1/2、1/4、1/6、1/8、1/10、1/12、1/14、1/16 等倍数的 CPU 时钟频率。
    * ``bus_width`` SD/MMC 总线宽度，取值为十进制整数，可取值为 ``1``、``4``。

.. _tftlcd_config_section:

:ref:`TFT LCD 配置 <label_back>`
+++++++++++++++++++++++++++++++++++++++

对于 TFT LCD，包含如下选项：

    * ``pin_lcd_reset`` 表示 TFT LCD 屏幕的复位引脚，选择用哪个引脚来重置显示模块，取值为 GPIO 引脚编号，可取值为从 ``0`` 到 ``45``。
    * ``pin_lcd_dcx`` 表示 TFT LCD 屏幕的数据/命令选择引脚，选择用哪个引脚来区分，取值为 GPIO 引脚编号，可取值为从 ``0`` 到 ``45``。
    * ``pin_lcd_led`` 表示  TFT LCD 屏幕的背光控制引脚，选择用哪个引脚来控制屏幕的背光亮灭，取值为 GPIO 引脚编号，可取值为从 ``0`` 到 ``45``。

.. _wdt_config_section:

:ref:`WDT 配置 <label_back>`
+++++++++++++++++++++++++++++

对于看门狗，其在配置文件中的配置选项 ``wdt_cfg`` 包含如下选项：

    * ``counter_value`` 该参数表示看门狗定时器的计数值，即定时器触发重置之前的计时周期，取值为十进制整数，单位微秒。

.. _psram_config_section:

:ref:`PSRAM 配置 <label_back>`
+++++++++++++++++++++++++++++++

对于一个伪静态随机存取存储器PSRAM，其在配置文件中的配置选项 ``psram_cfg`` 包含如下选项：

    * ``qspi`` 表示指定是否启用四线模式，可取值为布尔值： ``true``、``flase``。
    * ``clock_hz`` 配置 PSRAM 的时钟频率，单位为赫兹（Hz），取值为十进制整数，其取值为 cpu 主频的 3 - 15 分频。

.. _flash_config_section:

:ref:`FLASH 配置 <label_back>`
+++++++++++++++++++++++++++++++

对于一个 FLASH 存储设备，其在配置文件中的配置选项 ``flash_cfg`` 包含如下选项：

    * ``quad_spi`` 表示是否启用四线模式，可取值为布尔值： ``true``、``flase``。


使用 C 语言文件配置
----------------------

可通过 ``menuconfig`` 菜单中的 ``Components configuration  ---> Device Table  ---> Use c style configuration`` 启用 C 语言文件配置，开启此选项后 TOML 配置文件自动失效。
WM IoT SDK 当前使用的 C 语言配置文件默认位置为 ``components/wm_dt/config/<soc>/wm_dt_hw.c`` 和 ``components/wm_dt/config/<soc>/wm_dt_hw.h``。

当用户需要修改配置文件时，不推荐直接修改组件目录中的默认文件，
而是推荐从组件目录中复制一份至工程目录（例如复制至 ``main/dt`` 目录下），
然后修改工程目录下 ``main`` 组件中的 ``CMakeLists.txt`` 文件，添加

.. code:: cmake

    set(ADD_DT_C_FILES "dt/wm_dt_hw.c")
    set(ADD_DT_H_FILES "dt")

甚至，希望使用多份配置文件时，可为：

.. code:: cmake

    set(ADD_DT_C_FILES "dt/dt_config1.c"
                       "dt/dt_config2.c"
                       )

    set(ADD_DT_H_FILES "dt"
                       )

请注意，在工程目录中添加配置文件时，需保持 ``wm_dt_hw.h`` 文件名称不能变。
``.c`` 文件可以任意改名。
在多份配置文件时，需要修改文件中的配置内部名称，让每份配置的内部名称不同。
如 `dt_config1.c` 配置内部名称为 `my_config1`：

::

    WM_DT_TABLE_DEFINE(my_config1, (sizeof(dt_hw_table_entry) / sizeof(dt_hw_table_entry[0])), (void *)&dt_hw_table_entry[0]);

如 `dt_config2.c` 配置内部名称为 `my_config2`：

::

    WM_DT_TABLE_DEFINE(my_config2, (sizeof(dt_hw_table_entry) / sizeof(dt_hw_table_entry[0])), (void *)&dt_hw_table_entry[0]);


C 语言配置文件格式
^^^^^^^^^^^^^^^^^^^^^^^

WM IoT SDK 使用的 C 语言配置文件，其中内容由如下格式组成（摘选了部分）：

.. code:: c

    typedef struct {
        uint8_t init_level;
        uint8_t init_priority;
    } wm_dt_hw_init_cfg_t;

    typedef struct {
        uint8_t irq_num; /**< @ref wm_irq_no_t */
        uint8_t irq_priority;
    } wm_dt_hw_irq_cfg_t;

    typedef struct {
        uint8_t pin_num;  /**< @ref wm_gpio_num_t */
        uint8_t pin_mux;  /**< @ref wm_gpio_pin_mux_t */
        uint8_t pin_dir;  /**< @ref wm_gpio_dir_t */
        uint8_t pin_pupd; /**< @ref wm_gpio_pupd_t */
    } wm_dt_hw_pin_cfg_t;

    typedef struct {
        int baudrate;      /**< @ref wm_uart_baudrate_t */
        uint8_t parity;    /**< @ref wm_uart_parity_t */
        uint8_t stop_bits; /**< @ref wm_uart_stop_bits_t */
        uint8_t data_bits; /**< @ref wm_uart_data_bits_t */
        uint8_t flow_ctrl; /**< @ref wm_uart_flowctrl_t */
    } wm_dt_hw_uart_cfg_t;

    typedef struct {
        wm_dt_hw_init_cfg_t init_cfg;

        uint32_t reg_base;
        wm_dt_hw_irq_cfg_t irq_cfg;

        wm_dt_hw_uart_cfg_t uart_cfg;

        uint8_t pin_cfg_count;
        wm_dt_hw_pin_cfg_t *pin_cfg;

        char *dma_device_name;
    } wm_dt_hw_uart_t;

    typedef struct {
        wm_dt_hw_init_cfg_t init_cfg;

        uint32_t reg_base;
        wm_dt_hw_irq_cfg_t irq_cfg;
    } wm_dt_hw_timer_t;

.. code:: c

    const static wm_dt_hw_pin_cfg_t dt_hw_uart0_pin[] = {
        { .pin_num = WM_GPIO_NUM_35, .pin_mux = WM_GPIO_IOMUX_FUN1, .pin_dir = WM_GPIO_DIR_INPUT, .pin_pupd = WM_GPIO_FLOAT },
        { .pin_num = WM_GPIO_NUM_36, .pin_mux = WM_GPIO_IOMUX_FUN1, .pin_dir = WM_GPIO_DIR_INPUT, .pin_pupd = WM_GPIO_FLOAT },
        { .pin_num = WM_GPIO_NUM_37, .pin_mux = WM_GPIO_IOMUX_FUN1, .pin_dir = WM_GPIO_DIR_INPUT, .pin_pupd = WM_GPIO_FLOAT },
        { .pin_num = WM_GPIO_NUM_38, .pin_mux = WM_GPIO_IOMUX_FUN1, .pin_dir = WM_GPIO_DIR_INPUT, .pin_pupd = WM_GPIO_FLOAT },
    };

    const static wm_dt_hw_uart_t dt_hw_uart0 = {
        .init_cfg        = { .init_level = 0, .init_priority = 50 },
        .reg_base        = 0x40010600,
        .irq_cfg         = { .irq_num = WM_IRQ_UART0, .irq_priority = 0 },
        .uart_cfg        = { .baudrate  = WM_UART_BAUDRATE_B115200,
                             .parity    = WM_UART_PARITY_NONE,
                             .stop_bits = WM_UART_STOP_BIT_1,
                             .data_bits = WM_UART_DATA_BIT_8,
                             .flow_ctrl = WM_UART_FLOW_CTRL_DISABLE },
        .pin_cfg_count   = sizeof(dt_hw_uart0_pin) / sizeof(dt_hw_uart0_pin[0]),
        .pin_cfg         = (wm_dt_hw_pin_cfg_t *)dt_hw_uart0_pin,
        .dma_device_name = "dma",
    };

    const static wm_dt_hw_uart_t dt_hw_timer0 = {
        .init_cfg    = { .init_level = 0,         .init_priority = 50 },
        .reg_base    = 0x40011800,
        .irq_cfg     = { .irq_num = WM_IRQ_TIMER, .irq_priority = 0   },
    };

C 语言配置文件中的各选项比较直观，此处不在细述含义。

.. note::
    务必注意 ``pin_cfg`` 的设定，避免超过当前 SOC 所支持范围的定义 或 定义冲突。

    比如：所定义的 GPIO_NUM 并非当前 SOC 所支持的范围，1 个 pin 被多个设备定义等场景。

    具体可参考 :ref:`pinmux`


在代码中使用设备表编程
-----------------------

一般在使用时，用户只需使用 ``wm_device_t *wm_dt_get_device_by_name(const char *device_name)`` 从设备表中获取到设备使用即可。

.. code:: c

    typedef struct {
        char *name; /**< device name */

        void *hw; /**< hardware info, ref wm_dt_hw_xxx_t */

        void *ops; /**< device operation interface */
        void *drv; /**< driver context data */

        wm_device_status_t state; /**< device state */

        void *priv; /**< user private data */
    } wm_device_t;

其中，``hw`` 指针指向配置文件中的信息，由各驱动模块自行使用。

对于使用多份配置文件的场景，可通过配置文件的预设的名称进行选择使用，在代码中使用 ``int wm_dt_set_device_table_name(const char *default_name)`` 接口即可。

调用 wm_dt_set_device_table_name 接口需要在尽可能早的流程中执行，这里推荐使用 ``WM_COMPONEN_INIT_0`` 设置一个初始化函数，然后在此初始化函数中调用 wm_dt_set_device_table_name。关于 WM_COMPONEN_INIT_0 请参考 :ref:`label-components_autoinit` 章节。

新增设备
----------------------

使用 TOML 配置文件方式
^^^^^^^^^^^^^^^^^^^^^^^^

在 wmdt.py 中添加新增设备
+++++++++++++++++++++++++++++

需要同时修改 TOML 配置文件和 ``tools/wm/wmdt.py`` 文件。

可在 TOML 文件中，按需添加所需的选项后，在 ``wmdt.py`` 中添加对该新增设备的各选项解析代码。

在 ``wmdt.py`` 中，可修改：
    * 在 ``wm_dt_hw_c_include`` 变量中追加 hw 结构需要包含的头文件，若无则无需添加。
    * 在 ``generate_common`` 方法中追加公共数据结构定义，若无则无需添加。
    * 在 ``generate_file`` 方法中追加新增设备的解析处理代码，可仿照 ``i2s`` 代码。

.. code:: python

    def generate_i2s(self):  # 生成 I2S 配置的核心函数

        members = '''
        typedef struct {
            bool extal_clock_en;     /**< @ref wm_drv_i2s_ioctl_args_t */
            uint32_t mclk_hz;        /**< @ref wm_drv_i2s_ioctl_args_t */
        } wm_dt_hw_i2s_cfg_t;  /* 定义 I2S 特定配置，若无则无需添加 */

        typedef struct {
            wm_dt_hw_init_cfg_t init_cfg;

            uint32_t reg_base;
            wm_dt_hw_irq_cfg_t irq_cfg;

            uint8_t pin_cfg_count;
            wm_dt_hw_pin_cfg_t *pin_cfg;

            wm_dt_hw_i2s_cfg_t i2s_cfg;

            char *dma_device_name;
            char *rcc_device_name;
        } wm_dt_hw_i2s_t;  /* 此部分会添加到头文件内容中 */
        '''
        base_name = "i2s"  # 定义设备基础名称，用于后续匹配和代码生成。

        self.update_dt_hw_h(members)  # 更新设备头文件，将硬件配置结构体追加到头文件内容中。

        self.update_dt_hw_c_ops_def(base_name)  # 更新设备操作定义，关联基础名称和相关配置。

        for item in self.config:
            dev_name = item["dev_name"]
            if base_name == dev_name:  # 遍历 toml 文件中的各设备配置项，根据设备名称匹配生成相应的 C 文件内容。

                self.update_dt_hw_c_pin_cfg(dev_name, item)  # 生成 C 代码中 pin_cfg 结构体，可选项。

                self.update_dt_table(dev_name, base_name, item)  # 更新设备表，将设备名称与 ops 关联起来。
                self.update_dt_hw_c_begin(dev_name)  # 开始生成设备初始化代码部分，必选项。
                self.update_dt_hw_c_init_cfg(item)   # 生成 C 代码结构体中 ini_cfg 部分，必选项。
                self.update_dt_hw_c_irq_cfg(item)    # 生成 C 代码结构体中 irq_cfg 部分，可选项。
                # 对于特定的配置，访问字典项 ["i2s_cfg"] 的键值对，动态生成 C 语言结构体中的 i2s_cfg 代码，可选项。
                self.wm_dt_hw_c = (
                    self.wm_dt_hw_c
                    + "    .i2s_cfg = { .extal_clock_en = "
                    + str(item["i2s_cfg"]["extal_clock_en"]).lower()
                    + ", .extal_clock_hz = "
                    + str(item["i2s_cfg"]["mclk_hz"])
                    + " },\n"
                )
                self.update_dt_hw_c_pin_cfg_link(dev_name)  # 将 C 代码中的结构体中 pin_cfg 指向具体的变量，可选项
                self.update_dt_hw_c_dma_device(item)        # 将 C 代码中的结构体中 dma_device_name 指向具体的名称，可选项
                self.update_dt_hw_c_irq_device(item)        # 将 C 代码中的结构体中 irq_device_name 指向具体的名称，可选项
                self.update_dt_hw_c_rcc_device(item)        # 将 C 代码中的结构体中 rcc_device_name 指向具体的名称，可选项
                self.update_dt_hw_c_end()  # 完成当前设备初始化代码生成，必选项。


.. code:: python

    def generate_file(self):
        ......
        wmdt.generate_i2s()  # 向 generate_file 中添加新增设备的处理代码
        ......

* 对于一些公共的数据结构，脚本已经存在操作方法，可仿照 ``i2s`` 中的处理方式，无需新增代码，如

.. code:: python

    #例如：已定义公共数据结构update_dt_hw_c_rcc_device
    def update_dt_hw_c_rcc_device(self, item):
        if "rcc_device" in item:
            self.wm_dt_hw_c = self.wm_dt_hw_c + "    .rcc_device = \"" + item["rcc_device"] + "\",\n"

    #配置项中直接调用update_dt_hw_c_rcc_device
    self.update_dt_hw_c_rcc_device(item)

当修改完毕后，运行 ``wm.py build`` 命令构建系统就会自动调用 ``wmdt.py`` 脚本将 toml 文件转为 c 文件。
若无错误则会在 ``build/device_table`` 文件夹中生成解析后的 C 语言配置文件，若有错误则可根据提示进行处理即可。

在网页中添加新增设备
+++++++++++++++++++++

网页配置模块使用了 flask 框架，可以使用 jinjia2 模板方便的传递变量进行设备配置，其源码在 ``tools/devconf`` 路径下。
当需要在网页中新增一个设备时，需要作如下操作：

    - 修改 devconf.py 脚本
    - 修改 config_helper.py 脚本
    - 增加 html 网页，可选

以添加新设备 I2S 为例 ：

在 ``config_helper.py`` 的配置管理工具类 ``ConfigHelper`` 中，需要修改：

    * 在 ``__init__`` 中，添加对 I2S 设备的初始化信息，主要实现了：
        - 从 toml 文件中读取配置到本地配置结构中
        - 从 kconfig 文件中读取裁剪配置

.. code:: python

        def __init__(self, *args):
            # 初始化 I2S 配置字典，确定设备包含的参数和默认值
            self.i2s_config_data = {'chip_type':'W800', 'dev_name':'I2S', 'extal_clock_en':0, 'extal_clock_hz':0, 'initlevel':0, 'initpriority':0}

            # 遍历config["dev"] 列表中的每个设备，添加对 I2S 的判断
                    for item in config["dev"]:
                        dev_name = item["dev_name"]
                        ...
                        #在此处添加需要添加的 I2S 设备信息
                        elif dev_name == "i2s":
                            # 获取当前 I2S 设备的配置数据，默认为 i2s_config_data
                            config_data = self.i2s_config_data

                            # 根据配置文件中的 i2s_cfg 配置是否启用外部时钟（extal_clock_en）
                            if item["i2s_cfg"]["extal_clock_en"]:
                                config_data["extal_clock_en"] = 1  # 启用外部时钟
                            else:
                                config_data["extal_clock_en"] = 0  # 禁用外部时钟

                            # 设置外部时钟频率
                            config_data["extal_clock_hz"] = item["i2s_cfg"]["mclk_hz"]

                            # 配置时钟引脚（bclkpinnum）和左右声道时钟引脚（lrclkpinnum）
                            config_data["bclkpinnum"] = item["pin_cfg"][0]["pin"]
                            config_data["lrclkpinnum"] = item["pin_cfg"][1]["pin"]

                            # 定义已知的 MCLK、DI 和 DO 引脚号及其功能
                            mclk_pinnum = [0, 7, 33]
                            mclk_pinfun = ["fun1", "fun3", "fun4"]

                            di_pinnum = [7, 11, 26, 31]
                            di_pinfun = ["fun4", "fun4", "fun1", "fun4"]

                            do_pinnum = [0, 10, 27, 30]
                            do_pinfun = ["fun4", "fun4", "fun1", "fun4"]

                            # 默认禁用 MCLK、DI 和 DO
                            config_data["enable_mclk"] = 0
                            config_data["enable_di"] = 0
                            config_data["enable_do"] = 0

                            # 遍历 pin_cfg 配置，引脚匹配后启用相应的功能
                            for pin_cfg in item["pin_cfg"]:
                                # 如果匹配到 MCLK 引脚和功能，则启用 MCLK
                                for i in range(0, len(mclk_pinnum)):
                                    if pin_cfg["pin"] == mclk_pinnum[i] and pin_cfg["fun"] == mclk_pinfun[i]:
                                        config_data["enable_mclk"] = 1  # 启用 MCLK
                                        config_data["mclkpinnum"] = pin_cfg["pin"]  # 记录 MCLK 引脚
                                        break

                                # 如果匹配到 DI 引脚和功能，则启用 DI
                                for i in range(0, len(di_pinnum)):
                                    if pin_cfg["pin"] == di_pinnum[i] and pin_cfg["fun"] == di_pinfun[i]:
                                        config_data["enable_di"] = 1  # 启用 DI
                                        config_data["dipinnum"] = pin_cfg["pin"]  # 记录 DI 引脚
                                        break

                                # 如果匹配到 DO 引脚和功能，则启用 DO
                                for i in range(0, len(do_pinnum)):
                                    if pin_cfg["pin"] == do_pinnum[i] and pin_cfg["fun"] == do_pinfun[i]:
                                        config_data["enable_do"] = 1  # 启用 DO
                                        config_data["dopinnum"] = pin_cfg["pin"]  # 记录 DO 引脚
                                        break

                            # 加载额外的初始化配置
                            self.load_init_cfg(item, config_data)

                            # 更新 I2S 配置数据
                            self.i2s_config_data = config_data

            # 在默认配置 kconfig 的初始化中，添加 I2S 键值对
            kconfig = {......, "i2s":1, ......}
            with open(self.kconfig_file, "r", encoding="utf-8") as file:
                # 在动态调整的 kconfig 中，添加 I2S 键值对
                kconfig = {......, "i2s":0, ......}
                for line in file:
                    line = line.strip()
                # 添加选中 I2S 设备的选项
                    elif line == "CONFIG_COMPONENT_DRIVER_I2S_ENABLED=y":
                        kconfig["i2s"] = 1

                self.set_devices_kconfig(kconfig)
                self.i2s_config_data["chip_type"] = chip_type #也需要记录 SoC类型

* 在 ``set_devices_kconfig`` 方法中添加：

.. code:: python

        #添加 I2S 的 kconfig 项，以更新self.i2s_config_data
        def set_devices_kconfig(self, kconfig):
            self.i2s_config_data["kconfig"] = kconfig

* 在 ``set_devices_pin`` 方法中，设置引脚复用，如果不涉及引脚的设备，可以省略：

.. code:: python

            def set_devices_pin(self):
            self.pinmux = {}
            self.pinmux["chip_type"] = self.clock_config_data["chip_type"]
            kconfig = self.clock_config_data["kconfig"]
            self.pinmux["pinmux"] = [{} for i in range(46)]
            for item in self.config["dev"]:
                dev_name = item["dev_name"]
                ...
                # 添加 I2S 项 ，以根据 i2s_config_data 中的配置，设置引脚复用
                elif dev_name == "i2s" and kconfig["i2s"]:
                    self.pinmux["pinmux"][item["pin_cfg"][0]["pin"]][dev_name] = "I2S_BCLK"
                    self.pinmux["pinmux"][item["pin_cfg"][1]["pin"]][dev_name] = "I2S_LRCLK"
                    if self.i2s_config_data["enable_mclk"]:
                        self.pinmux["pinmux"].append({dev_name:"I2S_MCLK"})
                    if self.i2s_config_data["enable_di"]:
                        self.pinmux["pinmux"].append({dev_name:"I2S_DI"})
                    if self.i2s_config_data["enable_do"]:
                        self.pinmux["pinmux"].append({dev_name:"I2S_DO"})

* 添加 ``get_i2s_config`` 方法：

.. code:: python

        def get_i2s_config(self, dev_name):
            if dev_name == 'I2S':
                return self.i2s_config_data

* 添加 ``set_i2s_config`` 方法：

.. code:: python

        def set_i2s_config(self, data):
        if data['dev_name'] == 'I2S':
            self.i2s_config_data = data

        if not hasattr(self, 'config'):
            return

        for item in self.config["dev"]:
            if item["dev_name"] == "i2s":
                config_data = self.i2s_config_data
                if config_data["extal_clock_en"]:
                    item["i2s_cfg"]["extal_clock_en"] = True
                else:
                    item["i2s_cfg"]["extal_clock_en"] = False
                item["i2s_cfg"]["mclk_hz"] = config_data["extal_clock_hz"]

                mclk_pinnum = [0, 7, 33]
                mclk_pinfun = ["fun1", "fun3", "fun4"]

                di_pinnum   = [7, 11, 26, 31]
                di_pinfun   = ["fun4", "fun4", "fun1", "fun4"]

                do_pinnum   = [0, 10, 27, 30]
                do_pinfun   = ["fun4", "fun4", "fun1", "fun4"]

                to_pop = []
                for pin_cfg in item["pin_cfg"]:
                    for i in range(0, len(mclk_pinnum)):
                        if pin_cfg["pin"] == mclk_pinnum[i] and pin_cfg["fun"] == mclk_pinfun[i]:
                            to_pop.append(item["pin_cfg"].index(pin_cfg))
                            break
                    for i in range(0, len(di_pinnum)):
                        if pin_cfg["pin"] == di_pinnum[i] and pin_cfg["fun"] == di_pinfun[i]:
                            to_pop.append(item["pin_cfg"].index(pin_cfg))
                            break
                    for i in range(0, len(do_pinnum)):
                        if pin_cfg["pin"] == do_pinnum[i] and pin_cfg["fun"] == do_pinfun[i]:
                            to_pop.append(item["pin_cfg"].index(pin_cfg))
                            break

                for i in reversed(to_pop):
                    item["pin_cfg"].pop(i)

                item["pin_cfg"][0]["pin"] = config_data["bclkpinnum"]
                if config_data["bclkpinnum"] == 4:
                    item["pin_cfg"][0]["fun"] = "fun4"
                elif config_data["bclkpinnum"] == 8:
                    item["pin_cfg"][0]["fun"] = "fun4"
                elif config_data["bclkpinnum"] == 24:
                    item["pin_cfg"][0]["fun"] = "fun1"
                else:#28
                    item["pin_cfg"][0]["fun"] = "fun4"
                item["pin_cfg"][1]["pin"] = config_data["lrclkpinnum"]
                if config_data["lrclkpinnum"] == 1:
                    item["pin_cfg"][1]["fun"] = "fun4"
                elif config_data["lrclkpinnum"] == 9:
                    item["pin_cfg"][1]["fun"] = "fun4"
                elif config_data["lrclkpinnum"] == 25:
                    item["pin_cfg"][1]["fun"] = "fun1"
                else:#29
                    item["pin_cfg"][1]["fun"] = "fun4"

                if config_data["enable_mclk"]:
                    if config_data["mclkpinnum"] == 0:
                        item["pin_cfg"].append({"pin":config_data["mclkpinnum"], "fun":"fun1"})
                    elif config_data["mclkpinnum"] == 7:
                        item["pin_cfg"].append({"pin":config_data["mclkpinnum"], "fun":"fun3"})
                    else:#33
                        item["pin_cfg"].append({"pin":config_data["mclkpinnum"], "fun":"fun4"})

                if config_data["enable_di"]:
                    if config_data["dipinnum"] == 7:
                        item["pin_cfg"].append({"pin":config_data["dipinnum"], "fun":"fun4"})
                    elif config_data["dipinnum"] == 11:
                        item["pin_cfg"].append({"pin":config_data["dipinnum"], "fun":"fun4"})
                    elif config_data["dipinnum"] == 26:
                        item["pin_cfg"].append({"pin":config_data["dipinnum"], "fun":"fun1"})
                    else:#31
                        item["pin_cfg"].append({"pin":config_data["dipinnum"], "fun":"fun4"})

                if config_data["enable_do"]:
                    if config_data["dopinnum"] == 0:
                        item["pin_cfg"].append({"pin":config_data["dopinnum"], "fun":"fun4"})
                    elif config_data["dopinnum"] == 10:
                        item["pin_cfg"].append({"pin":config_data["dopinnum"], "fun":"fun4"})
                    elif config_data["dopinnum"] == 27:
                        item["pin_cfg"].append({"pin":config_data["dopinnum"], "fun":"fun1"})
                    else:#30
                        item["pin_cfg"].append({"pin":config_data["dopinnum"], "fun":"fun4"})

                self.update_init_cfg(config_data, item)

                self.update_config_file() #将本地配置写回 toml 文件中
                break

* 在 ``devconf.py`` 的配置管理工具类 ``ConfigHelper`` 中，需要修改：

    * 在 ``wmdt_index`` 方法中修改：

.. code:: python

    def wmdt_index():
        method = request.method
        if method == 'POST':
            if request.form.get("dev_name2"):
                dev_name = request.form.get("dev_name2")
            ...
            # POST 请求，添加更新 I2S 设备参数的配置
            elif dev_name.startswith('I2S'):
                #data = {'initlevel':0, 'initpriority':0}
                data = configHelper.get_i2s_config(dev_name)
                data['initlevel'] = int(request.form.get("initlevel"))
                data['initpriority'] = int(request.form.get("initpriority"))
                data['extal_clock_en'] = int(request.form.get("extal_clock_en"))
                data['extal_clock_hz'] = int(request.form.get("extal_clock_hz"))
                try:
                    data['enable_mclk'] = int(request.form.get("enable_mclk"))
                except TypeError:
                    data['enable_mclk'] = 0
                try:
                    data['mclkpinnum'] = int(request.form.get("mclkpinnum"))
                except TypeError:
                    data['mclkpinnum'] = 0
                data['bclkpinnum'] = int(request.form.get("bclkpinnum"))
                data['lrclkpinnum'] = int(request.form.get("lrclkpinnum"))
                try:
                    data['enable_di'] = int(request.form.get("enable_di"))
                except TypeError:
                    data['enable_di'] = 0
                try:
                    data['dipinnum'] = int(request.form.get("dipinnum"))
                except TypeError:
                    data['dipinnum'] = 0
                try:
                    data['enable_do'] = int(request.form.get("enable_do"))
                except TypeError:
                    data['enable_do'] = 0
                try:
                    data['dopinnum'] = int(request.form.get("dopinnum"))
                except TypeError:
                    data['dopinnum'] = 0
                data['dev_name'] = dev_name
                configHelper.set_i2s_config(data)

        # GET 时，添加数据 对 I2S 的渲染
        elif dev_name.startswith('I2S'):
            return render_template('index.html', data=configHelper.get_i2s_config(dev_name))

* 在 templates 中对 html 文件进行修改和添加：
    - 修改 devices.html 文件，在左侧设备列表中添加设备名称
    - 修改 status.html 文件，增加对新增设备的状态控制
    - 增加 i2s.html 文件，添加 i2s 独有的界面配置选项
    - 修改 index.html 文件，添加对 i2s.html 文件的包含

当修改完毕后，可以进入项目目录下运行 ``wm.py devconfig`` 命令，通过网页进行设备的配置。


使用 C 语言配置文件方式
^^^^^^^^^^^^^^^^^^^^^^^^

可参考默认的配置文件，仿照 ``uart`` 设备添加新增设备的 hw 结构和 ops 结构即可。

在设备数据结构添加之后，更新设备表，如：

.. code:: c

    const static struct wm_dt_table_entry dt_hw_table_entry[] = {
        { .dev_name = "uart0",  .hw_addr = (void *)&dt_hw_uart0,  .ops_addr = (void *)&wm_drv_uart_ops  },
        { .dev_name = "uart1",  .hw_addr = (void *)&dt_hw_uart1,  .ops_addr = (void *)&wm_drv_uart_ops  },
        { .dev_name = "timer0", .hw_addr = (void *)&dt_hw_timer0, .ops_addr = (void *)&wm_drv_timer_ops },
    };

    WM_DT_TABLE_DEFINE(default, 3, (void *)&dt_hw_table_entry[0]);

其中，使用 ``WM_DT_TABLE_DEFINE(name, count, addr)`` 注册给设备表时，可以通过填写不同的名称，达到设置多份配置文件的目的。

C 语言数据结构的规则
+++++++++++++++++++++

无论采用哪种方式的配置文件，最终使用的 C 语言数据结构中，对于每个设备的 hw 结构体有如下约定：

.. code:: c

    typedef struct {
        wm_dt_hw_init_cfg_t init_cfg;
        /* more ... */
    } wm_dt_hw_xxx_t;

即 hw 结构体中的第一个成员必须为 ``init_cfg``。

``init_cfg`` 表示该设备的初始化配置信息，将在未来版本中生效。其包含两个成员选项：
        * ``init_level`` 表示初始化方式，

          取值为 ``1`` 表示由系统自动完成该设备初始化，

          取值为 ``0`` 表示由用户自己完成该设备初始化。
        * ``init_priority`` 表示系统自动完成该设备初始化时的初始化优先级，取值为十进制整数，取值范围 0 - 100，

          取值越大表示优先级越高，优先级高的先初始化。相同优先级的设备在这一优先级被随机初始化。

对于每个设备的 ops 结构体有如下约定：

.. code:: c

    typedef struct {
        int (*init)(wm_device_t *dev);
        int (*deinit)(wm_device_t *dev);
        /* more ... */
    } wm_drv_xxx_ops_t;

即 ops 结构体中的前两个成员必须为 ``init`` 函数和 ``deinit`` 函数。


API 参考
-------------

可参考 :ref:`label_api_device_table`

设备表的 meunconfig 配置
-------------------------------------------

主要配置如下：

.. list-table::
   :widths: 25 45 25 
   :header-rows: 0
   :align: center

   * - 配置名称
     - 配置描述
     - 默认值

   * - CONFIG_DT_USE_C_STYLE_CONF
     - 是否使用 C 语言文件配置
     - N