.. _bluetooth_mesh_models_rpr_srv:

远程配置服务器
##########################

远程配置服务器模型是由 Bluetooth mesh 规范定义的基础模型。它通过 ``CONFIG_BT_MESH_RPR_SRV`` 选项启用。

远程配置服务器模型在 蓝牙 Mesh 协议规范 版本1.1中引入，用于支持将设备远程配置到 Mesh 网络中的功能

远程配置服务器没有自己的 API，但依赖于： :c:struct:`bluetooth_mesh_models_rpr_cli` 来控制它。远程配置服务器模型仅接受使用节点的设备密钥加密的消息。

如果存在，则必须在主元素上实例化远程配置服务器模型。

请注意，通过节点配置协议接口（NPPI）过程刷新设备密钥、节点地址或组合数据后，
将触发 :c:struct:`bt_mesh_prov.reprovisioned` 回调。有关更多详细信息，请参阅 :ref:`bluetooth_mesh_models_rpr_cli` 一节。

局限性
-----------

以下限制适用于远程配置服务器型号：

* 不支持使用 PB-GATT 设置未配置的设备。
* 支持所有节点配置协议接口（NPPI）过程。然而，如果设备的组成数据在设备固件更新后发生更改（请参阅固件效果 :c:struct:`blueooth_mesh_dfu_firmware_effect`），则设备不可能保持配置状态。如果设备的组成数据预计会更改，则应取消配置该设备。


API 参考
*************

.. doxygengroup:: bt_mesh_rpr_srv
   :project: wm-iot-sdk-apis
   :members:
