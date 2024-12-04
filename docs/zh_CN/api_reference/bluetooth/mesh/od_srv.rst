.. _bluetooth_mesh_od_srv:

按需专用代理服务器
##############################

按需私有代理服务器模型是 Bluetooth Mesh 规范定义的基础模型。它通过 ``CONFIG_BT_MESH_OD_PRIV_PROXY_SRV`` 选项启用。

按需专用代理服务器模型在 Bluetooth Mesh 协议规范版本1.1中引入，并通过管理其按需专用 GATT 代理状态，支持配置作为请求 PDU 接收者的节点的专用网络标识类型的广告。

启用时，还将启用 :ref:`bluetooth_mesh_srpl_srv` 。按需专用代理服务器依赖于节点上存在的 :ref:`bluetooth_mesh_models_priv_beacon_srv` 。

按需专用代理服务器没有自己的 API，并且依赖于 :ref:`bluetooth_mesh_od_cli` 来控制它。按需私有代理服务器模型仅接受使用节点的设备密钥加密的消息。

如果存在，则必须仅在主元素上实例化按需专用代理服务器模型。

API 参考
*************

.. doxygengroup:: bt_mesh_od_priv_proxy_srv
   :project: wm-iot-sdk-apis
   :members:
