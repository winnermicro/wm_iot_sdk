.. _bluetooth_mesh_models_op_agg_cli:

操作码聚合器客户端
#########################

操作码聚合器客户端模型是 Bluetooth Mesh 规范定义的基础模型。它是一种可选模型，通过 ``CONFIG_BT_MESH_OP_AGG_CLI`` 选项启用。

操作码聚合器客户端模型在 Bluetooth Mesh 协议规范版本1.1中引入，用于支持将一系列访问层消息发送到支持 :ref:`bluetooth_mesh_models_op_agg_srv` 模型的节点的功能。

操作码聚合器客户端模型使用目标节点的设备密钥或配置客户端配置的应用程序密钥与操作码聚合服务器模型通信。

如果存在，则只能在主元素上实例化操作码聚合器客户端模型。

操作码聚合器客户端模型在初始化时隐式绑定到设备密钥。它应该绑定到与用于生成消息序列的客户端模型相同的应用程序密钥。

为了能够聚合来自客户端模型的消息，它应该支持异步 API，例如通过回调。

API 参考
*************

.. doxygengroup:: bt_mesh_op_agg_cli
   :project: wm-iot-sdk-apis
   :members:
