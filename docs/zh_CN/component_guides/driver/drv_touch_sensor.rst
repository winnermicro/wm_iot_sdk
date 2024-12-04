.. _touch_sensor:

触摸传感器
==============

简介
-------------

触摸传感器系统由保护覆盖层、触摸电极、绝缘基板和走线组成，保护覆盖层位于最上层，绝缘基板上设有电极及走线。触摸覆盖层将引起电容变化，根据电容变化，判定触摸有效与否。

功能列表
-------------

    1. :ref:`初始化 <touch_sensor_init>`——初始化触摸传感器驱动。
    2. :ref:`设置阈值 <touch_sensor_set_threshold>`——设置触摸传感器的阈值。
    3. :ref:`获取阈值 <touch_sensor_get_threshold>`——获取触摸传感器的阈值。
    4. :ref:`读取计数 <touch_sensor_read_countnum>`——读取触摸传感器的计数值。
    5. :ref:`启动传感器 <touch_sensor_start>`——启动触摸传感器。
    6. :ref:`停止传感器 <touch_sensor_stop>`——停止触摸传感器。
    7. :ref:`注册回调 <touch_sensor_register_callback>`——注册触摸传感器回调函数。
    8. :ref:`去初始化 <touch_sensor_deinit>`——去初始化触摸传感器驱动。

功能概述
-------------

当前只有 W801S、W802、W803、W806 集成了触摸传感器模块，其工作时每隔一定时间逐次扫描每个触摸按键的状态，在设定的时间窗口内计数并记录下每个按键的状态。
如果超过设定的阈值，则判断该按键被触摸，通过轮询读取或者中断进行应用处理。

当前软件可调参数只有触摸阀值（调节范围：0-127），可根据实际环境调节得到一个最适合值。

W801S、W806 共有 12 个可用的 TOUCH 触摸引脚，两个控制引脚，触摸传感器引脚的关系如下表

.. list-table::
   :widths: 25 25 25 25 25
   :header-rows: 0
   :align: center

   * - 触摸传感器编号（名称）
     - 引脚编号
     - 引脚名称
     - 引脚复用功能
     - 引脚属性

   * - TOUCH1
     - PIN_NUM_9
     - PA9
     - OPT7
     - float/input

   * - TOUCH2
     - PIN_NUM_10
     - PA10
     - OPT7
     - float/input

   * - TOUCH3
     - PIN_NUM_16
     - PB0
     - OPT7
     - float/input

   * - TOUCH4
     - PIN_NUM_17
     - PB1
     - OPT7
     - float/input

   * - TOUCH5
     - PIN_NUM_18
     - PB2
     - OPT7
     - float/input

   * - TOUCH6
     - PIN_NUM_19
     - PB3
     - OPT7
     - float/input

   * - TOUCH7
     - PIN_NUM_20
     - PB4
     - OPT7
     - float/input

   * - TOUCH8
     - PIN_NUM_21
     - PB5
     - OPT7
     - float/input

   * - TOUCH9
     - PIN_NUM_22
     - PB6
     - OPT7
     - float/input

   * - TOUCH10
     - PIN_NUM_23
     - PB7
     - OPT7
     - float/input

   * - TOUCH11
     - PIN_NUM_24
     - PB8
     - OPT7
     - float/input

   * - TOUCH12
     - PIN_NUM_25
     - PB9
     - OPT7
     - float/input

   * - CMOD
     - PIN_NUM_12
     - PA12
     - OPT7
     - float/input

   * - CDC
     - PIN_NUM_14
     - PA14
     - OPT7
     - float/input


W802 共有 13 个可用的 TOUCH 触摸引脚，两个控制引脚，触摸传感器引脚的关系如下表

.. list-table::
   :widths: 25 25 25 25 25
   :header-rows: 0
   :align: center

   * - 触摸传感器编号（名称）
     - 引脚编号
     - 引脚名称
     - 引脚复用功能
     - 引脚属性

   * - TOUCH1
     - PIN_NUM_9
     - PA9
     - OPT7
     - float/input

   * - TOUCH2
     - PIN_NUM_10
     - PA10
     - OPT7
     - float/input

   * - TOUCH3
     - PIN_NUM_16
     - PB0
     - OPT7
     - float/input

   * - TOUCH4
     - PIN_NUM_17
     - PB1
     - OPT7
     - float/input

   * - TOUCH5
     - PIN_NUM_18
     - PB2
     - OPT7
     - float/input

   * - TOUCH6
     - PIN_NUM_19
     - PB3
     - OPT7
     - float/input

   * - TOUCH7
     - PIN_NUM_20
     - PB4
     - OPT7
     - float/input

   * - TOUCH8
     - PIN_NUM_21
     - PB5
     - OPT7
     - float/input

   * - TOUCH9
     - PIN_NUM_22
     - PB6
     - OPT7
     - float/input

   * - TOUCH10
     - PIN_NUM_23
     - PB7
     - OPT7
     - float/input

   * - TOUCH11
     - PIN_NUM_24
     - PB8
     - OPT7
     - float/input

   * - TOUCH12
     - PIN_NUM_25
     - PB9
     - OPT7
     - float/input

   * - TOUCH15
     - PIN_NUM_45
     - PB29
     - OPT7
     - float/input

   * - CMOD
     - PIN_NUM_12
     - PA12
     - OPT7
     - float/input

   * - CDC
     - PIN_NUM_14
     - PA14
     - OPT7
     - float/input


W803 共有 10 个可用的 TOUCH 触摸引脚，两个控制引脚，触摸传感器引脚的关系如下表

.. list-table::
   :widths: 25 25 25 25 25
   :header-rows: 0
   :align: center

   * - 触摸传感器编号（名称）
     - 引脚编号
     - 引脚名称
     - 引脚复用功能
     - 引脚属性

   * - TOUCH3
     - PIN_NUM_16
     - PB0
     - OPT7
     - float/input

   * - TOUCH4
     - PIN_NUM_17
     - PB1
     - OPT7
     - float/input

   * - TOUCH5
     - PIN_NUM_18
     - PB2
     - OPT7
     - float/input

   * - TOUCH6
     - PIN_NUM_19
     - PB3
     - OPT7
     - float/input

   * - TOUCH7
     - PIN_NUM_20
     - PB4
     - OPT7
     - float/input

   * - TOUCH8
     - PIN_NUM_21
     - PB5
     - OPT7
     - float/input

   * - TOUCH9
     - PIN_NUM_22
     - PB6
     - OPT7
     - float/input

   * - TOUCH10
     - PIN_NUM_23
     - PB7
     - OPT7
     - float/input

   * - TOUCH11
     - PIN_NUM_24
     - PB8
     - OPT7
     - float/input

   * - TOUCH12
     - PIN_NUM_25
     - PB9
     - OPT7
     - float/input

   * - CMOD
     - PIN_NUM_12
     - PA12
     - OPT7
     - float/input

   * - CDC
     - PIN_NUM_14
     - PA14
     - OPT7
     - float/input


在触摸传感器电路设计时，为实现更好的触摸效果，请参考以下因素和建议：


.. list-table::
   :widths: 25 25
   :header-rows: 0
   :align: center

   * - 影响因素
     - 建议设计

   * - 触摸面板厚度
     - 3mm 以内

   * - 导光柱
     - 不宜太厚

   * - CMOD 电容
     - 2nF - 10nF

   * - CDC 电容
     - 悬空

   * - 引脚串电阻
     - 不考虑传导骚扰，可取值 1kΩ - 10kΩ

       考虑传导骚扰，建议取值 4.7kΩ - 10kΩ

   * - 触摸与非触摸时的变化率
     - 至少要保证 2.5%，最好大于 3%

   * - 触摸面板与 PCB 之间的间隙
     - 不要有空隙


主要功能
-----------------------

.. _touch_sensor_init:

初始化触摸传感器
^^^^^^^^^^^^^^^^^^

    使用触摸传感器前需要调用 ``wm_drv_touch_sensor_init()`` 函数初始化驱动。示例：

    .. code:: c

        wm_device_t *touch_dev;
        touch_dev = wm_drv_touch_sensor_init("touch_sensor");

.. _touch_sensor_set_threshold:

设置阈值
^^^^^^^^^

    设置触摸传感器的阈值，调用 ``wm_drv_touch_sensor_set_threshold()`` 函数。示例：

    .. code:: c

        wm_device_t *touch_dev;
        touch_dev = wm_drv_touch_sensor_init("touch_sensor");

        wm_drv_touch_sensor_set_threshold(touch_dev, WM_TOUCH_SENSOR_NUM_1, 100);

.. _touch_sensor_get_threshold:

获取阈值
^^^^^^^^^

    获取触摸传感器的阈值，调用 ``wm_drv_touch_sensor_get_threshold()`` 函数。示例：

    .. code:: c

        wm_device_t *touch_dev;
        touch_dev = wm_drv_touch_sensor_init("touch_sensor");

        uint32_t threshold;
        wm_drv_touch_sensor_get_threshold(touch_dev, WM_TOUCH_SENSOR_NUM_1, &threshold);

.. _touch_sensor_read_countnum:

读取计数
^^^^^^^^^

    读取触摸传感器的计数值，调用 ``wm_drv_touch_sensor_read_countnum()`` 函数。示例：

    .. code:: c

        wm_device_t *touch_dev;
        touch_dev = wm_drv_touch_sensor_init("touch_sensor");

        uint32_t countnum;
        wm_drv_touch_sensor_read_countnum(touch_dev, 0, &countnum);

.. _touch_sensor_start:

启动传感器
^^^^^^^^^^^

    启动触摸传感器，调用 ``wm_drv_touch_sensor_start()`` 函数。示例：

    .. code:: c

        wm_device_t *touch_dev;
        touch_dev = wm_drv_touch_sensor_init("touch_sensor");

        wm_drv_touch_sensor_start(touch_dev, WM_TOUCH_SENSOR_NUM_1);

.. _touch_sensor_stop:

停止传感器
^^^^^^^^^^^

    停止触摸传感器，调用 ``wm_drv_touch_sensor_stop()`` 函数。示例：

    .. code:: c

        wm_device_t *touch_dev;
        touch_dev = wm_drv_touch_sensor_init("touch_sensor");

        wm_drv_touch_sensor_stop(touch_dev, WM_TOUCH_SENSOR_NUM_1);

.. _touch_sensor_register_callback:

注册回调
^^^^^^^^^

    注册触摸传感器的回调函数，调用 ``wm_drv_touch_sensor_register_callback()`` 函数。示例：

    .. code:: c

        wm_device_t *touch_dev;
        touch_dev = wm_drv_touch_sensor_init("touch_sensor");

        void touch_callback(wm_touch_sensor_num_t touch_num, void *priv)
        {
        }

        wm_drv_touch_sensor_register_callback(touch_dev, WM_TOUCH_SENSOR_NUM_1, touch_callback, NULL);

.. _touch_sensor_deinit:

去初始化触摸传感器
^^^^^^^^^^^^^^^^^^

    不再需要使用触摸传感器时，调用 ``wm_drv_touch_sensor_deinit()`` 函数反初始化驱动，释放资源。示例：

    .. code:: c

        wm_device_t *touch_dev;
        touch_dev = wm_drv_touch_sensor_init("touch_sensor");

        wm_drv_touch_sensor_deinit(touch_dev);

应用实例
-------------

    使用 touch sensor 基本示例请参考 :ref:`examples/peripheral/touch_sensor<touch_sensor_example>`


API 参考
-------------

   - 触摸按键 :ref:`label_api_touch_button`
