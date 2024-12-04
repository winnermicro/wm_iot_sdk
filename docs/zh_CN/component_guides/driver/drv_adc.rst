
.. _adc:

ADC
=============

简介
-------------

    ADC（Analog-to-Digital Converter，模数转换器）是一种将模拟信号转换为数字信号的硬件模块。在嵌入式系统和微控制器中，ADC用于将传感器或其他模拟输入信号转换为数字数据，以便进行进一步的处理和分析。需要注意，在 W80x 系列芯片中，ADC 的模拟转换电路需要大概 2MS 左右产生一个数据。


功能列表
-------------

    1. :ref:`初始化 <adc_init>`——初始化 ADC 驱动。
    2. :ref:`配置 ADC <adc_cfg>`——配置 ADC 通道和参数。
    3. :ref:`注册中断回调 <adc_register_callback>`——注册定时器中断回调函数。
    4. :ref:`获取单次 ADC 结果 <adc_oneshot>`——获取单次 ADC 转换结果。
    5. :ref:`获取轮询 ADC 结果 <adc_polling>`——获取轮询模式下的 ADC 转换结果。
    6. :ref:`启动 ADC 中断 <adc_start_it>`——启动ADC中断模式。
    7. :ref:`停止 ADC 中断 <adc_stop_it>`——停止ADC中断模式。
    8. :ref:`获取芯片内部温度 <adc_read_temperature>`——读取芯片内部温度。
    9. :ref:`获取芯片电压 <adc_read_vdd>`——读取芯片电压。
    10. :ref:`计算电压值 <adc_cal_voltage>`——根据 ADC 结果计算实际电压值。

功能概述
-------------


    **资源管理**: W800 有 4 个 ADC 独立的输入通道，以及 2 个差分通道，用户可以根据具体情况来管理不同通道。

    **灵活的参数配置**：ADC 支持配置通道数量、采样增益等参数，以适应不同的应用需求。

    **高精度**：ADC 具有高精度的信号转换能力，适用于各种精确测量应用。

    **中断和回调**：ADC 支持中断和回调函数，可以在转换完成时触发回调函数，方便实时处理数据。

    **温度传感**：内置温度传感器，可以通过 ADC 读取芯片内部的温度值。

    **电压检测**：可以通过 ADC 检测芯片的供电电压，方便进行电源管理。

主要功能
-----------------------

.. _adc_init:

初始化ADC
^^^^^^^^^^^^^^^^^^^^

    使用 ADC 前需要调用 ``wm_drv_adc_init()`` 函数为 ADC 分配资源，利用 ``wm_device_t`` 结构体接收 ADC 设备标识符。示例：

    .. code:: c

        wm_device_t *adc_dev;
        adc_dev = wm_drv_adc_init("adc");

    第一个参数指定设备名称，名称在设备表中定义。

    .. warning:: ADC 初始化后，如果没有调用 ``wm_drv_adc_deinit`` ，再次调用 ``wm_drv_adc_init`` 会返回 ``NULL``。

.. _adc_cfg:

配置ADC
^^^^^^^^^^^^^^^^^^^^

    调用函数 ``wm_drv_adc_cfg()`` 配置 ADC 设备的参数。示例如下：

    .. code:: c

        wm_device_t *adc_dev;
        adc_dev = wm_drv_adc_init("adc");

        wm_drv_adc_cfg_t adc_cfg;
        adc_cfg.adc_channel_count = 1;
        /* set cfg */
        ...

        wm_drv_adc_cfg(adc_dev, &adc_cfg);

    第一个参数是一个指向 ADC 设备的指针，类型为 ``wm_device_t*``。

    第二个参数是一个指向包含 ADC 配置参数的结构体的指针，类型为 ``wm_drv_adc_cfg_t``。

.. _adc_register_callback:

注册回调
^^^^^^^^^^^^^^^^^^^^

    调用函数 ``wm_drv_adc_register_callback()`` 注册 ADC 中断回调函数。adc_callback 是一个回调函数，当 ADC 中断触发时会被调用。arg 是一个传递给回调函数的参数，在这个例子中没有具体使用。示例如下：

    .. code:: c

        wm_device_t *adc_dev;
        adc_dev = wm_drv_adc_init("adc");

        void adc_callback(void *arg) {
            printf("ADC interrupt triggered!\n");
        }

        wm_drv_adc_register_callback(adc_dev, WM_ADC_INTR_TYPE_ADC, adc_callback, NULL);

    

    第一个参数是一个指向 ADC 设备的指针，类型为 ``wm_device_t*``。

    第二个参数是中断类型，用于表示不同类型的 ADC 中断，示例中 ``WM_ADC_INTR_TYPE_ADC`` 表示 ADC 转换结束时触发中断，还可设置为 ``WM_ADC_INTR_TYPE_DMA`` ,表示 DMA（直接存储器访问）中断, 需要注意，w800 暂不支持 adc dma 的传输。 ``WM_ADC_INTR_TYPE_COMP`` ,表示比较器中断。 ``WM_ADC_INTR_TYPE_MAX`` ,枚举类型的最大值，用于边界检查。类型为 ``wm_adc_intr_type_t``。

    第三个参数是回调函数，类型为 ``wm_drv_adc_callback_t``。

    第四个参数是传递给回调函数的私有数据，类型为 ``void *``。

.. _adc_oneshot:

生成单次转换结果
^^^^^^^^^^^^^^^^^^^^

    调用函数 ``wm_drv_adc_oneshot()`` 进行单次 ADC 转换，并返回结果。示例如下：

    .. code:: c

        wm_device_t *adc_dev;
        adc_dev = wm_drv_adc_init("adc");

        int32_t result;
        wm_drv_adc_oneshot(adc_dev, 0, &result);

    第一个参数是一个指向 ADC 设备的指针，类型为 ``wm_device_t*``。

    第二个参数是 ADC 通道号，类型为 ``wm_adc_channel_t``。

    第三个参数是指向 ``int32_t`` 类型变量 ``result`` 的指针，用于存储 ADC 转换的结果。

.. _adc_polling:

生成连续转换结果
^^^^^^^^^^^^^^^^^^^^

    调用函数 ``wm_drv_adc_polling()`` 进行 ADC 连续转换，并将结果存储到缓冲区。示例如下：

    .. code:: c

        wm_device_t *adc_dev;
        adc_dev = wm_drv_adc_init("adc");

        int buf[10];
        wm_drv_adc_polling(adc_dev, 0, buf, 10, 1000);

    第一个参数是一个指向 ADC 设备的指针，类型为 ``wm_device_t*``。

    第二个参数是 ADC 通道号，类型为 ``wm_adc_channel_t``。

    第三个参数是指向存储转换结果的缓冲区，用于存储轮询得到的 ADC 数据，类型为 ``int*``。

    第四个参数是缓冲区大小，类型为 ``uint16_t``。

    第五个参数是超时时间，单位为毫秒，指定在超过此时间后停止轮询，类型为 ``uint32_t``。

.. _adc_start_it:

启动ADC中断
^^^^^^^^^^^^^^^^^^^^

    调用函数 ``wm_drv_adc_start_it()`` 启动 ADC 中断，开始中断模式下的转换。示例如下：

    .. code:: c
        
        wm_device_t *adc_dev;
        adc_dev = wm_drv_adc_init("adc");

        wm_drv_adc_start_it(adc_dev, 0);

    第一个参数是一个指向 ADC 设备的指针，类型为 ``wm_device_t*``。

    第二个参数是 ADC 通道号，类型为 ``wm_adc_channel_t``。

    .. warning:: 中断模式会快速的输出adc采集数据，请在对应的中断回调中不要做耗时操作，并且调用 ``wm_drv_adc_cal_voltage()`` 来计算采集电压。

.. _adc_stop_it:

停止ADC中断
^^^^^^^^^^^^^^^^^^^^

    调用函数 ``wm_drv_adc_stop_it()`` 停止 ADC 中断，结束中断模式下的转换。示例如下：

    .. code:: c

        wm_device_t *adc_dev;
        adc_dev = wm_drv_adc_init("adc");
        
        wm_drv_adc_stop_it(adc_dev, 0);

    第一个参数是一个指向 ADC 设备的指针，类型为 ``wm_device_t*``。

    第二个参数是 ADC 通道号，类型为 ``wm_adc_channel_t``。

.. _adc_read_temperature:

读取温度
^^^^^^^^^^^^^^^^^^^^

    调用函数 ``wm_drv_thermal_sensor_read_temperature()`` 读取芯片内的温度值。示例如下：

    .. code:: c

        wm_device_t *adc_dev;
        adc_dev = wm_drv_adc_init("adc");

        int temperature_val;
        wm_drv_thermal_sensor_read_temperature(adc_dev, &temperature_val);

    第一个参数是一个指向 ADC 设备的指针，类型为 ``wm_device_t*``。

    第二个参数是一个指向存储温度值的指针，用于存储读取的温度值，类型为 ``int*``。

.. _adc_read_vdd:

读取电压
^^^^^^^^^^^^^^^^^^^^

    调用函数 ``wm_drv_adc_chip_voltage_read_vdd()`` 读取芯片的供电电压。示例如下：

    .. code:: c

        wm_device_t *adc_dev;
        adc_dev = wm_drv_adc_init("adc");

        uint32_t voltage;
        wm_drv_adc_chip_voltage_read_vdd(adc_dev, &voltage);

    第一个参数是一个指向 ADC 设备的指针，类型为 ``wm_device_t*``。

    第二个参数是一个指向存储电压值的指针，用于存储读取的芯片电压值，类型为 ``uint32_t*``。

.. _adc_cal_voltage:

校准电压
^^^^^^^^^^^^^^^^^^^^

    调用函数 ``wm_drv_adc_cal_voltage()`` 计算芯片电压。示例如下：

    .. code:: c

        wm_device_t *adc_dev;
        adc_dev = wm_drv_adc_init("adc");

        wm_drv_adc_cal_voltage(adc_dev, 3300);

    第一个参数是一个指向 ADC 设备的指针，类型为 ``wm_device_t*``。

    第二个参数是一个表示从 ADC 中断中获取的原始值，类型为 ``int``。这里设置了原始值为 3300。

应用实例
-------------
    使用 ADC 基本示例请参考 :ref:`examples/peripheral/adc<peripheral_example>`

API参考
-------------
    查找 ADC 相关 API 请参考：

    :ref:`label_api_adc`