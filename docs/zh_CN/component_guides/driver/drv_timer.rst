
.. _timer:

TIMER
=============

简介
-------------
    Timer（定时器）是一种用于精确时间控制和事件计数的硬件模块。在嵌入式系统中，定时器可以用于产生精确的时间延迟、计数事件、生成定时中断等功能。它们是实现实时操作系统时钟、PWM（脉宽调制）信号生成、脉冲计数以及事件定时的重要工具。

    W800芯片支持多个定时器，每个定时器可以独立配置计数周期、自动重载等参数。定时器模块的灵活性和高精度使其在各种嵌入式应用中广泛使用。


功能列表
-------------

    1. :ref:`初始化 <timer_init>`——初始化定时器驱动。
    2. :ref:`设置计数周期 <timer_set_period>`——设置定时器的计数周期。
    3. :ref:`设置自动重载 <timer_set_auto_reload>`——配置定时器的自动重载模式。
    4. :ref:`启动定时器 <timer_start>`——启动定时器。
    5. :ref:`停止定时器 <timer_stop>`——停止定时器。
    6. :ref:`读取计数值 <timer_get_counter>`——读取定时器的当前计数值。
    7. :ref:`注册中断回调 <timer_register_callback>`——注册定时器中断回调函数。

功能概述
-------------  

    **设置和获取计数值**：强制定时器从起点开始计数，以及如何随时获取计数值。

    **多通道支持**：W800最多支持6个独立的定时器通道。

    **灵活的参数配置**：定时器支持配置时钟源、计数周期、和自动重载等参数。

    **高精度**：定时器具有高精度的时间控制能力，适用于各种精确定时应用。

    **中断和回调**：定时器支持中断和回调函数，可以在定时器溢出或达到特定计数值时触发中断。

主要功能
-------------

.. _timer_init:

初始化定时器
^^^^^^^^^^^^^^^^^^^^
    使用定时器前需调用 ``wm_drv_timer_init()`` 函数为定时器分配资源，利用 ``wm_device_t`` 结构体接收定时器设备标识符。示例：

    .. code:: c

        wm_device_t *timer_dev;
        timer_dev = wm_drv_timer_init("timer0");

    第一个参数指定设备名称，名称在设备表中定义，从 timer0 ~ timer5。

    .. warning:: 定时器初始化后，如果没有调用 ``wm_drv_timer_deinit()``，再次调用 ``wm_drv_timer_init()`` 会返回 ``NULL`` 。

.. _timer_set_period:

设置计数周期
^^^^^^^^^^^^^^^^^^^^
   
   调用函数 ``wm_drv_timer_set_period()`` 设置定时器的计数周期，单位为微秒。示例如下：

    .. code:: c

        wm_device_t *timer_dev;
        timer_dev = wm_drv_timer_init("timer0");

        wm_drv_timer_set_period(timer_dev, 1000); 
    
    第一个参数是一个指向定时器设备的指针，类型为 ``wm_device_t*``。

    第二个参数是一个无符号的32位整数，表示定时器的计数周期，单位为微秒。在这个例子中，1000 表示定时器的计数周期为 1000 微秒（即 1 毫秒）。

.. _timer_set_auto_reload:

设置自动重载
^^^^^^^^^^^^^^^^^^^^

    调用函数 ``wm_drv_timer_set_auto_reload()`` 配置定时器的自动重载模式。示例如下：

    .. code:: c

        wm_device_t *timer_dev;
        timer_dev = wm_drv_timer_init("timer0");

        wm_drv_timer_set_auto_reload(timer_dev, true); 
    
    第一个参数是一个指向定时器设备的指针，类型为 ``wm_device_t*``。

    第二个参数为自动重载选项，类型为 ``bool``,设置为 true 表示启用自动重载功能，设置为 false 表示禁用自动重载功能。

.. _timer_start:

启动定时器
^^^^^^^^^^^^^^^^^^^^

    调用函数 ``wm_drv_timer_start()`` 启动定时器。示例如下：

    .. code:: c

        wm_device_t *timer_dev;
        timer_dev = wm_drv_timer_init("timer0");

        wm_drv_timer_cfg_t timer_cfg = {
        .unit = WM_HAL_TIMER_UNIT_US,
        .period = 1000,
        .auto_reload = true
            };
        wm_drv_timer_start(timer_dev, timer_cfg);
    
    第一个参数是一个指向定时器设备的指针，类型为 ``wm_device_t*``。

    第二个参数用于配置定时器的相关参数，类型为 ``wm_drv_timer_cfg_t``。其中 ``unit`` 为定时器的单位，类型为 wm_hal_timer_unit_t。这里设置为 WM_HAL_TIMER_UNIT_US，表示定时器的单位是微秒（us）。 ``period`` 为定时器的周期，类型为 uint32_t。这里设置为 1000，表示定时器的周期为 1000 微秒（1 毫秒）。 ``auto_reload`` 设置定时器是否自动重载，类型为 bool。这里设置为 true，表示定时器在计数到周期结束时会自动重载并重新开始计数。

.. _timer_stop:

停止定时器
^^^^^^^^^^^^^^^^^^^^

    调用函数 ``wm_drv_timer_stop()`` 停止定时器。示例如下：

    .. code:: c

        wm_device_t *timer_dev;
        timer_dev = wm_drv_timer_init("timer0");

        wm_drv_timer_stop(timer_dev);
    
    其参数是一个指向定时器设备的指针，类型为 ``wm_device_t*``。
    
.. _timer_get_counter:

读取计数值
^^^^^^^^^^^^^^^^^^^^

    调用函数 ``wm_drv_timer_get_counter()`` 读取定时器的当前计数值。示例如下：

    .. code:: c

        wm_device_t *timer_dev;
        timer_dev = wm_drv_timer_init("timer0");

        uint32_t counter;
        wm_drv_timer_get_counter(timer_dev, &counter);

    第一个参数是一个指向定时器设备的指针，类型为 ``wm_device_t*``。

    第二个参数是一个指向 ``uint32_t`` 类型的指针，用于存储当前定时器的计数值。

.. _timer_register_callback:

注册中断回调
^^^^^^^^^^^^^^^^^^^^

    调用函数 ``wm_drv_timer_register_callback()`` 注册定时器中断回调函数。示例如下：

    .. code:: c

        wm_device_t *timer_dev;
        timer_dev = wm_drv_timer_init("timer0");

        void timer_callback(void *arg) {
            printf("Timer interrupt triggered!\n");
        }

        wm_drv_timer_register_callback(timer_dev, timer_callback, NULL);

    第一个参数是一个指向定时器设备的指针，类型为 ``wm_device_t*``。

    第二个参数是回调函数，类型为 ``wm_drv_timer_callback_t``。该函数在定时器中断触发时被调用。

    第三个参数是传递给回调函数的私有数据，类型为 ``void *``。

应用实例
-------------
    使用定时器基本示例请参考 :ref:`examples/peripheral/timer<peripheral_example>`

API参考
-------------
    查找TIMER相关API请参考：

    :ref:`label_api_timer`