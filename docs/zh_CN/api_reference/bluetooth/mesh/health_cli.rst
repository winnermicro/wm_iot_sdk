.. _bluetooth_mesh_models_health_cli:

运行状况客户端
#################

运行状况客户端模型与运行状况服务器模型交互，以读取诊断并控制节点的注意状态。

健康客户端 API 中的所有消息传递函数都将 ``cli`` 作为其第一个参数。这是指向要在此函数调用中使用的客户端模型实例的指针。
第二个参数是 ``ctx`` 或消息上下文。消息上下文包含目标节点使用的网络密钥索引、应用密钥索引和单播地址。

运行状况客户端模型是可选的，可以在任何元素上实例化。然而，如果在主元素以外的元素上实例化运行状况客户端模型，则主元素上也必须存在实例。

有关规范定义的故障值的列表，请参阅 :ref:`bluetooth_mesh_health_faults` 。

头文件
-----------

- wm_iot_sdk/components/bluetooth/include/bluetooth/mesh/health_cli.h
- This header file can be included with:

.. code-block:: c
   :emphasize-lines: 1

   #include "bluetooth/mesh/health_cli.h"

API 参考
-----------

.. doxygengroup:: bt_mesh_health_cli
