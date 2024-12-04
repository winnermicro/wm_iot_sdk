.. _bluetooth_mesh_models_priv_beacon_cli:

专用信标客户端
#####################

私有信标客户端模型是由蓝牙网格规范定义的基础模型。它通过 ``CONFIG_BT_MESH_PRIV_BEACON_CLI`` 选项启用。

专用信标客户端模型在 Bluetooth Mesh Protocol Specification 版本1.1中引入，并提供配置 :ref:`Bluetooth_Mesh_models_priv_Beacon_srv` 模型的功能。

专用信标功能通过定期随机化信标输入数据，为不同的蓝牙网状信标添加隐私。这保护网格节点不被网格网络外部的设备跟踪，并隐藏网络的 IV 索引、IV 更新和密钥刷新状态。

专用信标客户端模型与 :ref:`bluetooth_mesh_models_priv_beacon_srv` 模型，使用目标节点的设备密钥。专用信标客户端模型可以与其他节点上的服务器通信，或者通过本地专用信标服务器模型进行自我配置。

专用信标客户端 API 中的所有配置函数都将 ``net_idx`` 和 ``addr`` 作为其第一个参数。这些应该设置为网络索引和为目标节点提供的主单播地址。

如果存在，则只能在主元素上实例化专用信标客户端模型。

API 参考
*************

.. doxygengroup:: bt_mesh_priv_beacon_cli
   :project: wm-iot-sdk-apis
   :members:
