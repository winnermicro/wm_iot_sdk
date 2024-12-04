.. _bluetooth_mesh_lcd_cli:

大型组合数据客户端
#############################

大组合数据客户端模型是 Bluetooth Mesh 规范定义的基础模型。该模型是可选的，并通过 ``CONFIG_BT_MESH_LARGE_COMP_DATA_CLI`` 选项启用。

大合成数据客户端模型在 Bluetooth Mesh 协议规范版本1.1中引入，支持读取不适合配置合成数据状态消息的合成数据页面，以及读取支持  :ref:`bluetooth_mesh_lcd_srv` 模型的节点上模型实例的元数据的功能。

大组合数据客户端模型使用包含目标大组合数据服务器模型实例的节点的设备密钥与大组合数据服务模型通信。

如果存在，则必须仅在主元素上实例化大型组合数据客户端模型。

API 参考
*************

.. doxygengroup:: bt_mesh_large_comp_data_cli
   :project: wm-iot-sdk-apis
   :members:
