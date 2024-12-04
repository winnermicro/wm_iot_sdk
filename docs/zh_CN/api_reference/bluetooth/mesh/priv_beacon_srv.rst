.. _bluetooth_mesh_models_priv_beacon_srv:

专用信标服务器
#####################

专用信标服务器模型是由蓝牙网状规范定义的基础模型。它通过 ``CONFIG_BT_MESH_PRIV_BEACON_SRV`` 选项启用。

专用信标服务器模型在 Bluetooth Mesh 协议规范版本1.1中引入，并控制 Mesh 节点的专用信标状态、专用 GATT 代理状态和专用节点标识状态。

专用信标功能通过定期随机化信标输入数据，为不同的蓝牙网状信标添加隐私。
这保护网格节点不被网格网络外部的设备跟踪，并隐藏网络的 IV 索引、IV 更新和密钥刷新状态。
必须为设备实例化专用信标服务器，以支持发送专用信标，但节点将在没有它的情况下处理接收的专用信标。

专用信标服务器没有自己的 API，但依赖于 :ref:`bluetooth_mesh_models_priv_beacon_cli` 控制它。专用信标服务器模型仅接受使用节点的设备密钥加密的消息。

应用程序可以通过传递给 :func:`bt_mesh_model_priv_Beacon_srv` 的 :func:`bt_mesh_priv_Beacon_srv` 实例配置专用信标服务器模型的初始参数。
请注意，如果存储的网格节点在设置子系统中更改了此配置，则加载时可能会覆盖初始值。

如果存在，则只能在主元素上实例化专用信标服务器模型。

API 参考
*************

.. doxygengroup:: bt_mesh_priv_beacon_srv
   :project: wm-iot-sdk-apis
   :members:
