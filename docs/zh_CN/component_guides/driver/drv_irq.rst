
.. _irq:

IRQ
=============

简介
-------------

    IRQ（Interrupt Request）指来自设备的中断请求。驱动提供了一组用于管理硬件和软件中断的函数。该模块提供软硬件中断的注册以及注销接口，启用和禁用中断，并管理中断挂起状态。

功能列表
-------------

    1. :ref:`注册硬件中断向量 <irq_attach_hw_vector>`——附加硬件中断向量并替换软件向量。
    2. :ref:`注销硬件中断向量 <irq_detach_hw_vector>`——分离硬件中断向量。
    3. :ref:`注册软件中断向量 <irq_attach_sw_vector>`——附加软件中断向量和回调函数。
    4. :ref:`注销软件中断向量 <irq_detach_sw_vector>`——分离软件中断向量。
    5. :ref:`禁用并保存中断状态 <irq_save>`——禁用全局中断并保存之前的状态。
    6. :ref:`恢复中断状态 <irq_restore>`——通过之前保存的状态启用全局中断。
    7. :ref:`启用中断 <irq_enable>`——启用指定中断。
    8. :ref:`禁用中断 <irq_disable>`——禁用指定中断。
    9. :ref:`设置中断挂起 <irq_set_pending>`——设置指定中断为挂起状态。
    10. :ref:`清除中断挂起 <irq_clear_pending>`——清除指定中断的挂起状态。
    11. :ref:`设置唤醒中断 <irq_set_wakeup>`——设置指定中断为唤醒中断。

功能概述
-------------

    下图示例了中断向量表的管理

    .. figure:: ../../../_static/component-guides/driver/irq.png
        :align: center
        :alt: 中断向量表示意图


    在中断管理中，提供了一套软件中断的注册机制，原则上对于用户仅需要注册软件表即可，处于性能考虑，也提供了直接写硬件向量表的接口，需注意，当在同一中断向量上注册硬件中断和软件中断时，硬件中断的优先级较高，会覆盖对应的软件中断，导致软件中断失效。如果先注册了硬件中断，再注册软件中断时，软件中断则无法注册。

主要功能
-----------------------

.. _irq_attach_hw_vector:

注册硬件中断向量
^^^^^^^^^^^^^^^^^^^^

    调用函数 ``wm_drv_irq_attach_hw_vector()`` 注册硬件中断向量并替换软件向量。示例如下：

    .. code:: c

        void irq_callback(void) {
        }

        wm_drv_irq_attach_hw_vector(WM_IRQ_I2C, irq_callback);

    第一个参数是中断编号，W800芯片共有32个中断编号，每个中断编号对应W800芯片的一个中断源。类型为 ``wm_irq_no_t``。

    第二个参数是中断回调函数，类型为 ``wm_drv_hw_irq_handle``。

.. _irq_detach_hw_vector:

注销硬件中断向量
^^^^^^^^^^^^^^^^^^^^

    调用函数 ``wm_drv_irq_detach_hw_vector()`` 分离硬件中断向量。示例如下：

    .. code:: c

        wm_drv_irq_detach_hw_vector(WM_IRQ_I2C);

    其参数是中断编号，类型为 ``wm_irq_no_t``。

.. _irq_attach_sw_vector:

注册软件中断向量
^^^^^^^^^^^^^^^^^^^^

    调用函数 ``wm_drv_irq_attach_sw_vector()`` 附加软件中断向量和回调函数。示例如下：

    .. code:: c

        void irq_callback(void *arg) {
        }

        wm_drv_irq_attach_sw_vector(WM_IRQ_I2C, irq_callback, NULL);

    第一个参数是中断编号，类型为 ``wm_irq_no_t``。

    第二个参数是中断回调函数，类型为 ``wm_drv_sw_irq_handle``。

    第三个参数是用户数据，类型为 ``void *``。

.. _irq_detach_sw_vector:

注销软件中断向量
^^^^^^^^^^^^^^^^^^^^

    调用函数 ``wm_drv_irq_detach_sw_vector()`` 分离软件中断向量。示例如下：

    .. code:: c

        wm_drv_irq_detach_sw_vector(WM_IRQ_I2C);

    其参数是中断编号，类型为 ``wm_irq_no_t``。

.. _irq_save:

禁用并保存中断状态
^^^^^^^^^^^^^^^^^^^^

    调用函数 ``wm_drv_irq_save()`` 禁用中断并保存之前的状态。示例如下：

    .. code:: c

        uint32_t irq_state;
        irq_state = wm_drv_irq_save();

    此函数返回之前的中断状态，类型为 ``uint32_t``。

.. _irq_restore:

恢复中断状态
^^^^^^^^^^^^^^^^^^^^

    调用函数 ``wm_drv_irq_restore()`` 通过之前保存的状态启用中断。示例如下：

    .. code:: c

        uint32_t irq_state;
        irq_state = wm_drv_irq_save();

        wm_drv_irq_restore(irq_state);

    其中， ``irq_state`` 用于保存中断状态，可以使用这个保存的状态来恢复中断状态。其参数是之前保存的中断状态，类型为 ``uint32_t``。

.. _irq_enable:

启用中断
^^^^^^^^^^^^^^^^^^^^

    调用函数 ``wm_drv_irq_enable()`` 启用指定中断。示例如下：

    .. code:: c

        wm_drv_irq_enable(WM_IRQ_I2C);

    其参数是中断编号，类型为 ``wm_irq_no_t``。

.. _irq_disable:

禁用中断
^^^^^^^^^^^^^^^^^^^^

    调用函数 ``wm_drv_irq_disable()`` 禁用指定中断。示例如下：

    .. code:: c

        wm_drv_irq_disable(WM_IRQ_I2C);

    其参数是中断编号，类型为 ``wm_irq_no_t``。

.. _irq_set_pending:

设置中断挂起
^^^^^^^^^^^^^^^^^^^^

    调用函数 ``wm_drv_irq_set_pending()`` 设置指定中断为挂起状态。示例如下：

    .. code:: c

        wm_drv_irq_set_pending(WM_IRQ_I2C);

    其参数是中断编号，类型为 ``wm_irq_no_t``。

.. _irq_clear_pending:

清除中断挂起
^^^^^^^^^^^^^^^^^^^^

    调用函数 ``wm_drv_irq_clear_pending()`` 清除指定中断的挂起状态。示例如下：

    .. code:: c

        wm_drv_irq_clear_pending(WM_IRQ_I2C);

    其参数是中断编号，类型为 ``wm_irq_no_t``。

.. _irq_set_wakeup:

设置唤醒中断
^^^^^^^^^^^^^^^^^^^^

    调用函数 ``wm_drv_irq_set_wakeup()`` 设置指定中断为唤醒中断，当设备处于低功耗模式时，唤醒中断可以触发设备从低功耗模式恢复正常运行。示例如下：

    .. code:: c

        wm_drv_irq_set_wakeup(WM_IRQ_I2C);

    其参数是中断编号，类型为 ``wm_irq_no_t``。

API参考
-------------
    查找IRQ相关API请参考：

    :ref:`label_api_irq`