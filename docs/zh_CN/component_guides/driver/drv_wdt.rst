
.. _wdt:

WDT
=============

简介
-------------
    WDT(watchdog timer)，是一个定时器电路，一般有一个输入，叫喂狗(kicking the dog/service the dog)，一个输出到MCU的RST端，MCU正常工作的时候，每隔一段时间输出一个信号到喂狗端，给 WDT清零，如果超过规定的时间不喂狗（一般在程序跑飞时），WDT定时超过，就会给出一个复位信号到MCU，使MCU复位，防止MCU死机。看门狗的作用就是防止程序发生死循环，或者说程序跑飞。


功能列表
-------------

    1. :ref:`初始化 <wdt_init>`——初始化看门狗驱动。
    2. :ref:`启动wdt <wdt_start>`——启动看门狗。
    3. :ref:`关闭wdt <wdt_stop>`——关闭看门狗。
    4. :ref:`设置wdt周期 <set_wdt_counter>`——设置看门狗周期。
    5. :ref:`获取wdt周期 <get_wdt_counter>`——获取看门狗周期。
    6. :ref:`获取wdt剩余时间 <get_wdt_remain>`——获取看门狗剩余时间。
    7. :ref:`喂狗 <wdt_feed>`——喂狗。

功能概述
-------------  

    **设置和获取周期值**：设置获取看门狗周期值，设置后立即生效，清空之前计数值，重新开始。

    **获取剩余时间**：获取看门狗剩余时间，观察看门狗动作。

    **高精度**：看门狗具有高精度的时间控制能力。

    **中断**：定时器支持中断，当触发看门狗后会设置 reboot reason 并复位。

主要功能
-------------

.. _wdt_init:

初始化看门狗
^^^^^^^^^^^^^^^^^^^^
    使用定时器前需调用 ``wm_drv_wdt_init()`` 函数为看门狗分配资源，利用 ``wm_device_t`` 结构体接收定时器设备标识符。示例：

    .. code:: c

        wm_device_t *wdt_dev;
        wdt_dev = wm_drv_wdt_init("wdt");

    第一个参数指定设备名称。

    .. warning:: 看门狗初始化后，如果没有调用 ``wm_drv_wdt_deinit()``，再次调用 ``wm_drv_wdt_init()`` 会返回 ``NULL`` 。

.. _wdt_start:

启动看门狗
^^^^^^^^^^^^^^^^^^^^

    调用函数 ``wm_drv_wdt_start()`` 启动看门狗。示例如下：

    .. code:: c

        wm_device_t *wdt_dev;
        wdt_dev = wm_drv_wdt_init("wdt");
        
        wm_drv_wdt_start(wdt_dev);
        
    第一个参数是一个指向定时器设备的指针，类型为 ``wm_device_t*``。

.. _wdt_stop:

关闭看门狗
^^^^^^^^^^^^^^^^^^^^

    调用函数 ``wm_drv_wdt_stop()`` 关闭看门狗。示例如下：

    .. code:: c

        wm_device_t *wdt_dev;
        wdt_dev = wm_drv_wdt_init("wdt");
        
        wm_drv_wdt_stop(wdt_dev);
        
    第一个参数是一个指向定时器设备的指针，类型为 ``wm_device_t*``。

.. _set_wdt_counter:

设置看门狗周期
^^^^^^^^^^^^^^^^^^^^
   
   调用函数 ``wm_drv_wdt_set_counter_value()`` 设置看门狗周期，单位为微秒。示例如下：

    .. code:: c

        wm_device_t *wdt_dev;
        wdt_dev = wm_drv_wdt_init("wdt");

        wm_drv_wdt_set_counter_value(wdt_dev, 1000 * 1000 * 10); 
    
    第一个参数是一个指向定时器设备的指针，类型为 ``wm_device_t*``。

    第二个参数是一个无符号的32位整数，表示看门狗周期，单位为微秒。在这个例子中，表示看门狗周期为 10 秒。

.. _get_wdt_counter:

获取看门狗周期
^^^^^^^^^^^^^^^^^^^^

    调用函数 ``wm_drv_wdt_get_counter_value()`` 获取看门狗周期值。示例如下：

    .. code:: c

        wm_device_t *wdt_dev;
        int counter_value = 0;
        
        wdt_dev = wm_drv_wdt_init("wdt");
        wm_drv_wdt_get_counter_value(wdt_dev, &counter_value); 

    第一个参数是一个指向看门狗设备的指针，类型为 ``wm_device_t*``。

    第二个参数为要获取的看门狗周期，类型为 ``int``。

.. _get_wdt_remain:

获取看门狗剩余时间
^^^^^^^^^^^^^^^^^^^^

    调用函数 ``wm_drv_wdt_get_remaining_time()`` 获取看门狗剩余时间值。示例如下：

    .. code:: c

        wm_device_t *wdt_dev;
        int remain = 0;
        
        wdt_dev = wm_drv_wdt_init("wdt");
        wm_drv_wdt_get_remaining_time(wdt_dev, &remain); 

    第一个参数是一个指向看门狗设备的指针，类型为 ``wm_device_t*``。

    第二个参数为要获取的看门狗剩余时间，类型为 ``int``。

.. _wdt_feed:

喂狗
^^^^^^^^^^^^^^^^^^^^

    调用函数 ``wm_drv_wdt_feed()`` 喂狗。示例如下：

    .. code:: c

        wm_device_t *wdt_dev;
        wdt_dev = wm_drv_wdt_init("wdt");
        
        wm_drv_wdt_feed(wdt_dev);
        
    第一个参数是一个指向定时器设备的指针，类型为 ``wm_device_t*``。

应用实例
-------------

    使用 WDT 基本示例请参考 :ref:`examples/peripheral/wdt<peripheral_example>`

API参考
-------------
    查找WDT相关API请参考：

    :ref:`label_api_wdt`
