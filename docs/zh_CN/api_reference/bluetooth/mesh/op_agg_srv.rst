.. _bluetooth_mesh_models_op_agg_srv:

操作码聚合器服务器
#########################

操作码聚合器服务器模型是 Bluetooth mesh 规范定义的基础模型。它是一个可选模型，通过 ``CONFIG_BT_MESH_OP_AGG_SRV`` 选项启用。

操作码聚合器服务器模型在 Bluetooth Mesh 协议规范版本1.1中引入，用于支持处理接入层消息序列的功能。

操作码聚合器服务器模型接受使用节点的设备密钥或应用程序密钥加密的消息。

如果存在，则必须仅在主元素上实例化操作码聚合器服务器模型。

目标服务器模型应绑定到用于加密发送到操作码聚合器服务器的访问层消息序列的相同应用程序密钥。

操作码聚合器服务器处理聚合的消息，并将它们发送到相应的模型及其消息处理程序。当前实现假设响应与接收的消息来自相同的执行上下文，并且不允许发送延迟的响应，例如从工作队列发送。

头文件
===============

- wm_iot_sdk/components/bluetooth/include/bluetooth/mesh/op_agg_srv.h
- This header file can be included with:

.. code-block:: c
   :emphasize-lines: 1

   #include "bluetooth/mesh/op_agg_srv.h"

API 参考
===============

.. doxygengroup:: bt_mesh_op_agg_srv
   :project: wm-iot-sdk-apis
   :members:
