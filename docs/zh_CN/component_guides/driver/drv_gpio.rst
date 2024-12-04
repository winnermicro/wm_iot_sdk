
.. _gpio:

GPIO
=============

简介
-------------

    GPIO (General-porpose input/output)是通用输入输出端口的简称。可以通过软件控制其输出和输入。对于输入，可以通过pullmode来配置默认电压，对于输出，可以通过写寄存器让这个引脚输出高电位或者低电位。


功能列表
-------------

    1. :ref:`初始化 <gpio_init>`——初始化驱动。
    2. :ref:`配置方向 <gpio_set_dir>`——设置GPIO引脚的方向(输入或输出)。
    3. :ref:`设置pullmode <gpio_set_pullmode>`——配置GPIO引脚的上拉、下拉或浮空模式。
    4. :ref:`读取数据 <gpio_data_get>`——从GPIO引脚读取数据。
    5. :ref:`写入数据 <gpio_data_set>`——写数据到GPIO引脚，其中1为拉高，0为拉低。
    6. :ref:`引脚复用 <gpio_iomux_func_sel>`——选择IOMUX引脚复用。
    7. :ref:`使能中断 <gpio_enable_isr>`——使能GPIO中断。
    8. :ref:`设置中断模式 <gpio_set_intr_mode>`——配置GPIO中断模式。
    
功能概述
-------------

    W800具有48个GPIO引脚，通过IO MUX可配置复用关系，具体关系请参考PINMUX章节，其中OPT1-OPT4为数字复用，OPT5为GPIO，OPT6-OPT7为模拟复用，需注意：当作为模拟复用的时候，该引脚需要被配置为输入悬空。

主要功能
-----------------------

.. _gpio_init:

初始化GPIO
^^^^^^^^^^^^^^^^^^^^

    使用GPIO前需要调用 ``wm_drv_gpio_init`` 函数用于初始化GPIO设备。利用 ``wm_device_t``，示例代码如下：

    .. code:: c

        wm_device_t *gpio_device;
        gpio_device = wm_drv_gpio_init("gpio");

    其参数指定设备名称，名称在设备表中定义，和设备表中的名称要保持一致。

    .. warning:: GPIO初始化后，如果没有调用 ``wm_drv_gpio_deinit`` , 再次调用 ``wm_drv_gpio_init`` 会返回 ``NULL`` 。

.. _gpio_set_dir:

配置方向    
-------------
    GPIO方向配置用于设置引脚为输入或输出模式。使用 ``wm_drv_gpio_set_dir`` 函数来设置引脚方向，示例代码如下：

    .. code:: c

        wm_drv_gpio_set_dir(WM_GPIO_NUM_0, WM_GPIO_DIR_INPUT);
        wm_drv_gpio_set_dir(WM_GPIO_NUM_0, WM_GPIO_DIR_OUTPUT);
    
    第一个参数是引脚编号，第二个参数是方向，方向可以是 ``WM_GPIO_DIR_INPUT`` 或 ``WM_GPIO_DIR_OUTPUT``。

.. _gpio_set_pullmode:

配置pullmode
-------------

    GPIO拉模式配置用于设置引脚为上拉、下拉或浮空模式。使用 ``wm_drv_gpio_set_pullmode`` 函数来配置引脚拉模式，示例代码如下：

    .. code:: c

        wm_drv_gpio_set_pullmode(WM_GPIO_NUM_0, WM_GPIO_PULL_UP);
        wm_drv_gpio_set_pullmode(WM_GPIO_NUM_0, WM_GPIO_PULL_DOWN);

    第一个参数是引脚编号，第二个参数是PULLMODE，PULLMODE可以是 ``WM_GPIO_PULL_UP``、 ``WM_GPIO_PULL_DOWN`` 或 ``WM_GPIO_FLOAT``。

.. _gpio_data_get:

读取数据
^^^^^^^^^^^^^^^^^^^^

    使用 ``wm_drv_gpio_data_get`` 函数从GPIO引脚读取数据。此函数返回引脚的当前电平，示例代码如下：

    .. code:: c

        int value = wm_drv_gpio_data_get(WM_GPIO_NUM_0);
    
    其参数为引脚编号。
    
.. _gpio_data_set:

写入数据
^^^^^^^^^^^^^^^^^^^^

    使用 ``wm_drv_gpio_data_set`` 函数将数据写入GPIO引脚。此函数将引脚设置为高电平，示例代码如下：

    .. code:: c

        wm_drv_gpio_data_set(WM_GPIO_NUM_0);

    使用 ``wm_drv_gpio_data_reset`` 函数将引脚设置为低电平，示例代码如下：

    .. code:: c

        wm_drv_gpio_data_reset(WM_GPIO_NUM_0);
    
    其参数为引脚编号。

.. _gpio_iomux_func_sel:

选择引脚复用
^^^^^^^^^^^^^^^^^^^^

    使用 ``wm_drv_gpio_iomux_func_sel`` 函数选择GPIO引脚的复用功能，示例代码如下：

    .. code:: c

        wm_gpio_num_t pin = WM_GPIO_NUM_21;
        wm_gpio_pin_mux_t func = WM_GPIO_IOMUX_FUN5;

        wm_drv_gpio_iomux_func_sel(pin, func);

    第一个参数是引脚编号，第二个参数是功能模式，可以参考功能概述中表格或者在 ``wm_io_mux.h`` 查询。

.. _gpio_enable_isr:

使能中断
^^^^^^^^^^^^^^^^^^^^

    使用 ``wm_drv_gpio_enable_isr`` 函数使能GPIO引脚的中断，示例代码如下：

    .. code:: c

        wm_drv_gpio_enable_isr(WM_GPIO_NUM_21);
    
    其参数为引脚编号。

.. _gpio_disable_isr:

禁用中断
^^^^^^^^^^^^^^^^^^^^

    使用 ``wm_drv_gpio_disable_isr`` 函数禁用GPIO引脚的中断，示例代码如下：

    .. code:: c

        wm_drv_gpio_disable_isr(WM_GPIO_NUM_21);
    
    其参数为引脚编号。

.. _gpio_set_intr_mode:

设置中断模式
^^^^^^^^^^^^^^^^^^^^

    使用 ``wm_drv_gpio_set_intr_mode`` 函数配置GPIO引脚的中断触发方式，包括下降沿触发、上升沿触发、双边沿触发、低电平触发和高电平触发。示例代码如下：

    .. code:: c

        wm_drv_gpio_set_intr_mode(WM_GPIO_NUM_21, WM_GPIO_IRQ_TRIG_HIGH_LEVEL);

    第一个参数是引脚编号，第二个参数是中断模式，中断模式可以是 ``WM_GPIO_IRQ_TRIG_FALLING_EDGE``、 ``WM_GPIO_IRQ_TRIG_RISING_EDGE`` 、 ``WM_GPIO_IRQ_TRIG_DOUBLE_EDGE`` 、 ``WM_GPIO_IRQ_TRIG_LOW_LEVEL`` 或 ``WM_GPIO_IRQ_TRIG_HIGH_LEVEL``。

应用实例
-------------------

    使用 GPIO 基本示例请参照 :ref:`examples/peripheral/gpio<peripheral_example>`



API参考
-------------
    查找GPIO相关API请参考：

    :ref:`label_api_gpio`