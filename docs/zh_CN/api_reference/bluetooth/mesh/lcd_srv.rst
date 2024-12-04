.. _bluetooth_mesh_lcd_srv:

大型组合数据服务器
#############################

大型组合数据服务器模型是 Bluetooth Mesh 规范定义的基础模型。该模型是可选的，并通过 ``CONFIG_BT_MESH_LARGE_COMP_DATA_SRV`` 选项启用。

大合成数据服务器模型在 Bluetooth Mesh 协议规范版本1.1中引入，用于支持公开不适合配置合成数据状态消息的合成数据页面的功能，并公开模型实例的元数据。

大型组合数据服务器没有自己的 API，并且依赖于 :ref:`bluetooth_mesh_lcd_cli` 来控制它。该模型仅接受使用节点的设备密钥加密的消息。

如果存在，则必须仅在主元素上实例化大型组合数据服务器模型。

模型元数据
===============

大型组合数据服务器模型允许每个模型具有可由大型组合数据客户端模型读取的模型特定元数据的列表。
元数据列表可以通过 :c:member:`bt_mesh_model.metadata` 字段与 :c:struct:`bt_mesh_model` 模型相关联。
元数据列表由 :c:struct:`bt_mesh_models_metadata_entry` 结构定义的一个或多个条目组成。
每个条目包含元数据的长度和 ID，以及指向原始数据的指针。
可以使用 :c:macro:`BT_MESH_MODELS_METADATA_ENTRY` 宏创建条目。 :c:macro:`BT_MESH_MODELS_METADATA_END` 宏标记元数据列表的末尾，
并且必须始终存在。如果模型没有元数据，则可以改用助手宏 :c:macro:`BT_MESH_MODELS_METADATA_NONE` 。

API 参考
*************

.. doxygengroup:: bt_mesh_large_comp_data_srv
   :project: wm-iot-sdk-apis
   :members:
