
.. _codec_i2s:

CODEC_I2S
=============

简介
-------------

    CODEC_I2S 模块，用于驱动外接音频芯片，配合 I2S 模块，完成音频的录制，播放等处理。

    例如，外接顺芯 ES8374 芯片，再外接麦克风，扬声器，耳机，可以进行声音录制，也可以把音频数据从扬声器和耳机播放出去。


    .. figure:: ../../../_static/component-guides/driver/codec_module_h.svg
        :align: center
        :alt: CODEC_I2S 常见使用连线


功能列表
-------------

    1. :ref:`Init <codec_i2s_init>`——初始化 CODEC 驱动。

    2. :ref:`Deinit <codec_i2s_deinit>`——去初始化 CODEC 驱动。

    3. :ref:`Set format <codec_i2s_set_format>`——设置音频数据格式。

    4. :ref:`Start <codec_i2s_start>`——启动音频处理。

    5. :ref:`Stop <codec_i2s_stop>`——停止音频处理。

    6. :ref:`Set mute <codec_i2s_set_mute>`——设置输出静音或者非静音。

    7. :ref:`Set volume <codec_i2s_set_volume>`——设置输出音量。

    8. :ref:`Set mic mute <codec_i2s_set_mic_mute>`——设置输入静音或者非静音。

    9. :ref:`Set mic volume <codec_i2s_set_mic_volume>`——设置输入音量。

    10. :ref:`Set reg <codec_i2s_set_reg>`——直接写 CODEC 芯片寄存器。

    11. :ref:`Get reg <codec_i2s_get_reg>`——直接读 CODEC 芯片寄存器。

功能概述
-------------

    **音频芯片** 目前驱动只支持 ES8374 音频芯片，用户需要使用其他芯片可以参照文档对接。

    **主从设备** 目前驱动中一般是 WM80X 芯片为 Master, ES8374 作为 Slave。

    **传输协议** I2S 和 CODEC 使用标准 I2S 协议传输。

    **工作模式** 支持 IN，OUT， IN + OUT 三种方式：

        IN ：即麦克风从环境中采集声音数据，经 CODEC 芯片放大转换成数字信号，从 I2S 接口传给 WM80X 芯片处理；

        OUT ：WM80X 芯片把声音数据从 I2S 接口传递给 CODEC 音频芯片，转换成模拟信号输出到扬声器或者耳机上；

        IN + OUT ：同时使用录制和播放功能。

    **采样率** 不同 CODEC 芯片能力不一样：

        ES8374 : 支持 8K ~ 96K , 32 位立体声只支持 8K ~ 48K。

    **位深** 使用 ES8374 芯片时，支持 16 位和 32 位，其他位数如 8 位，24 位，可以在上层进行重采样处理，example 中有重采样的参考。

    **声道** 支持单声道和立体声。

    **静音** 输入，输出都提供了静音设置接口。

    **音量** 输入，输出都提供了音量设置接口。

    **寄存器读写** 提供了直接读写 CODEC 芯片寄存器的接口。


.. _codec_i2s_set:

配置
-------------


设备表配置
^^^^^^^^^^^^^^^^^^
    CODEC_I2S 需要在设备表中配置，根据实际使用的 Board 和芯片，按硬件连线和 CODEC 芯片型号进行配置。

编译配置
^^^^^^^^^^^^^^^^^^

    模块使用到 I2C, I2S和具体的 CODEC 芯片，所以需要把相关配置打开

    CONFIG_COMPONENT_DRIVER_I2C_ENABLED=y

    CONFIG_COMPONENT_DRIVER_I2S_ENABLED=y

    CONFIG_COMPONENT_DRIVER_CODEC_ES8374_ENABLED=y

CODEC_I2C 地址
----------------

    WM80X 控制 CODEC 芯片时，使用的是 I2C 协议，CODEC 芯片的 I2C 地址，需要参照手册和硬件连线。I2C 驱动接口使用 7 位地址，不包括读写位，在调用读写接口时，I2C 底层驱动会左移一位再加上读写位。


主要功能
---------------

.. _codec_i2s_init:

初始化 CODEC_I2S
^^^^^^^^^^^^^^^^^^

    使用 CODEC_I2S 之前需调用 ``wm_drv_codec_i2s_init()`` 函数为CODEC_I2S分配资源，利用 ``wm_device_t`` ，示例：

    .. code:: c

        wm_device_t *codec_i2s_dev = wm_dt_get_device_by_name("es8374");
        wm_drv_codec_i2s_init("es8374");


    第一个参数指定设备名称，名称在设备表中定义，目前只有一个 CODEC_I2S 主设备，只能填 "es8374"

    .. warning:: CODEC_I2S 初始化后，如果没有调用 ``wm_drv_codec_i2s_deinit`` , 再次调用 ``wm_drv_codec_i2s_init`` 会返回 ``NULL`` 。

.. _codec_i2s_deinit:

去初始化 CODEC_I2S
^^^^^^^^^^^^^^^^^^

    使用完成 CODEC_I2S 后调用 ``wm_drv_codec_i2s_deinit()`` 释放CODEC_I2S分配的资源，示例：

    .. code:: c

        wm_drv_codec_i2s_dedeinit(codec_i2s_dev);



.. _codec_i2s_set_format:

设置音频格式
^^^^^^^^^^^^^

    初始化后，调用函数 ``wm_drv_codec_i2s_set_format`` 设置音频数据格式

    .. code:: c

        wm_drv_codec_i2s_set_format(codec_i2s_dev, 48000, 16, WM_I2S_CHAN_TYPE_STEREO);


    .. warning:: 根据 I2S 协议规范，同时使用 IN 和 OUT 功能，只能使用相同的格式。

.. _codec_i2s_start:

开始
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

    调用 ``wm_drv_codec_i2s_start`` 开始工作。


    .. code:: c

        wm_drv_codec_i2s_start(codec_i2s_dev);


.. _codec_i2s_stop:

停止
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

    调用 ``wm_drv_codec_i2s_stop`` 开始工作。


    .. code:: c

        wm_drv_codec_i2s_stop(codec_i2s_dev);


.. _codec_i2s_set_mute:

设置输出静音
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

    调用 ``wm_drv_codec_i2s_set_mute`` 设置输出静音。


    .. code:: c

        wm_drv_codec_i2s_set_mute(codec_i2s_dev);


.. _codec_i2s_set_mic_mute:

设置输入静音
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

    调用 ``wm_drv_codec_i2s_set_mic_mute`` 设置输入静音。


    .. code:: c

        wm_drv_codec_i2s_set_mic_mute(codec_i2s_dev);


.. _codec_i2s_set_volume:

设置输出音量
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

    调用 ``wm_drv_codec_i2s_set_volume`` 设置输出音量，音量值从 0 ~ 100。


    .. code:: c

        wm_drv_codec_i2s_set_volume(codec_i2s_dev,90);


.. _codec_i2s_set_mic_volume:

设置输入音量
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

    调用 ``wm_drv_codec_i2s_set_mic_volume`` 设置输入音量，音量值从 0 ~ 100。


    .. code:: c

        wm_drv_codec_i2s_set_mic_volume(codec_i2s_dev,90);


.. _codec_i2s_set_reg:

写 CODEC 寄存器
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

    调用 ``wm_drv_codec_i2s_set_reg`` 写 CODEC 寄存器。


    .. code:: c

        wm_drv_codec_i2s_set_reg(codec_i2s_dev,0x38,20);


.. _codec_i2s_get_reg:

读 CODEC 寄存器
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

    调用 ``wm_drv_codec_i2s_get_reg`` 读 CODEC 寄存器。


    .. code:: c

        int value;
        wm_drv_codec_i2s_get_reg(codec_i2s_dev,0x38,&value);



注意事项
-------------

    .. note::

        音量从0到100，线性映射具体codec的最小db值和最大db值。

        当上层音量改变时，具体codec的db阶数较稍，从而db可能没变化，音量也没有变化。

        db线性变化时，对应的电压和功率放大不是线性的，人耳感官上的声音也不是线性的，要做到感官上的线性音量改变，需要上层使用者加一个映射表进行转换。


    .. warning::

       VDDAC 和 VDDSPK电压尽可能保持一致，避免 Speaker 产生爆音。

应用实例
-------------

    使用 CODEC_I2S 基本示例请参照 :ref:`examples/peripheral/codec_i2s/wav_play_record<peripheral_example>`


API 参考
-------------
    查找 CODEC_I2S 相关 API 请参考：

    :ref:`label_api_codec_i2s`



CODEC_I2S 新驱动开发
=====================

    SDK 中已经包含 ES8374 型号的驱动，针对其他型号的音频芯片，需要添加新的驱动代码。


    CODEC 设备驱动框架


    CODEC_I2S 驱动有统一的接口，定义在 components/driver/include/codec_i2s/wm_drv_codec_i2s.h 中， 文件中 wm_drv_codec_i2s_ops_t 定义的函数指针，就是实现 CODEC_I2S 驱动需要实现的接口。

    CODEC_I2S 的驱动实现都放到 components/driver/src/codec_i2s/ 下，其中 wm_drv_codec_i2s.c 文件实现从统一接口到具体驱动实例的调用，具体驱动实例和设备的关联是在设备表中定义的。


    .. figure:: ../../../_static/component-guides/driver/codec_module_s.svg
        :align: center
        :alt: CODEC_I2S 调用关系

    图中 wm_drv_codec_i2s_xxx.c 就是需要实现的驱动。

    第一步：在设备表中创建设备信息

    下面是 ES8374的设备信息，可参照修改

    .. code:: c

        const static wm_dt_hw_codec_i2s_t dt_hw_es8374 = {
            .init_cfg                 = { .init_level = 0, .init_priority = 49 },
            .i2s_device_name = "i2s",
            .i2c_device_name = "i2c",
            .codec_cfg = {
                .i2c_address = 0x10,        /**< codec i2c 7 bits address   */
                .in_port = 2,               /**< mic port2 input            */
                .out_port = 3,              /**< speaker and headphone use  */
                .jack_pin = WM_GPIO_NUM_MAX,
                .pa_pin = WM_GPIO_NUM_MAX,
                .max_gain = 0,           /**< VPA = 5.0V , VDAC = 3.3V , max_gain = 20 * log(5.0/3.3) = 0 dB */
            },
        };


    .. note::

        设备表的修改方法 可参考相关章节 Pinmux 以及 设备表

        设备表中的 codec_cfg 必须和开发板匹配


    第二步：创建新设备的 驱动主体文件

    参照 wm_drv_codec_i2s_es8374.c 的实现，复制文件 wm_drv_codec_i2s_es8374.c 为 wm_drv_codec_i2s_XXX.c ，
    修改其中的 wm_drv_codec_i2s_es8374_ops 为 wm_drv_codec_i2s_XXX_ops , 然后根据新的 CODEC_I2S型号要求修改
    实现其中的各个接口实现。

    例如：

    .. code:: c

        const wm_drv_codec_i2s_ops_t wm_drv_codec_i2s_xxx_ops = {
            .init   = xxx_init,
            .deinit = xxx_deinit,
            .set_format = xxx_set_format,
            .start = xxx_start,
            .stop  = xxx_stop,
            .set_mute       = xxx_set_mute,
            .set_volume     = xxx_set_volume,
            .set_mic_mute   = xxx_set_mic_mute,
            .set_mic_volume = xxx_set_mic_volume,
            .set_reg = xxx_set_reg,
            .get_reg = xxx_get_reg,
            .dump = xxx_dump,
        };


    第三步： 配置文件完善

    在 components/driver 下的 Kconfig 文件中，增加 CODEC 模块使能选项 COMPONENT_DRIVER_CODEC_XXXX_ENABLED

    并在 CMakeLists.txt 文件中，把新增加的驱动文件加入编译

    下面是 ES8374 的配置：

    .. code::

        config COMPONENT_DRIVER_CODEC_ES8374_ENABLED
            bool "CODEC ES8374"
            default n
            select COMPONENT_DRIVER_I2S_ENABLED
            select COMPONENT_DRIVER_I2C_ENABLED
            help
                Enable codec es8374 driver module.


    .. note:: 新的驱动需要在设备表中关联在某个设备上，这样该设备才能使用新的驱动。
