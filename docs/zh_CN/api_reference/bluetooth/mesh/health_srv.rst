.. _bluetooth_mesh_models_health_srv:

运行状况服务器
#################

运行状况服务器模型为 :ref:`bluetooth_mesh_models_health_cli` 模型提供注意回调和节点诊断。它主要用于报告网格节点中的故障，并将网格节点映射到其物理位置。

如果存在，则必须在主元素上实例化运行状况服务器模型。

故障
=============

运行状况服务器模型可以报告在设备生命周期内发生的故障列表。
通常，故障是可能改变节点行为的事件或条件，如断电或外围设备故障。
故障分为警告和错误。警告表示接近节点设计承受极限的条件，但不一定会损坏设备。
错误表示超出节点设计限制的情况，并且可能导致设备的无效行为或永久损坏。

故障值 ``0x01`` 到 ``0x7f`` 是为 Bluetooth Mesh 规范保留的，规范定义的故障的完整列表在 :ref:`bluetooth_mesh_health_faults` 中可用。
故障值 ``0x80`` 到 ``0xff`` 特定于供应商。故障列表始终与公司 ID 一起报告，以帮助解释供应商特定的故障。


.. _bluetooth_mesh_models_health_srv_attention:

注意力状态
=============

注意力状态用于使设备通过一些物理行为（如闪烁、播放声音或振动）来唤起对自身的注意力。
注意状态可以在配置期间使用，以让用户知道他们正在配置哪个设备，以及在运行时通过运行状况模型。

启用时，注意状态始终分配一个1到255秒范围内的超时。 :func:`Health Server API` 为应用程序提供了两个回调，以运行其注意调用行为： :func:`bt_mesh_Health_srv_cb.attn_on` 在注意期的开始调用， :func:`bt_mesh _Health _srv_cb.attn_off` 在结束时调用。

可以通过 :c:member:`bt_mesh_health_srv.attn_timer` 查询关注期的剩余时间。

头文件
=============

- wm_iot_sdk/components/bluetooth/include/bluetooth/mesh/health_srv.h
- This header file can be included with:

.. code-block:: c
   :emphasize-lines: 1

   #include "bluetooth/mesh/health_srv.h"

API 参考
=============

.. doxygengroup:: bt_mesh_health_srv

.. _bluetooth_mesh_health_faults:

运行状况故障
=============

Bluetooth Mesh 规范定义的故障值。

.. doxygengroup:: bt_mesh_health_faults
