.. _bluetooth_mesh_cfg:

运行时配置
#####################

运行时配置 API 允许应用程序直接更改其运行时配置，而无需遍历配置模型。

Bluetooth Mesh 节点通常应由具有配置客户端型号的中央网络 :ref:`bluetooth_mesh_models_cfg_cli` 配置。
每个网格节点实例化 :ref:`bluetooth_mesh_models_cfg_srv` 可以与之通信以更改节点配置的配置服务器模型。
在某些情况下，网格节点不能依赖配置客户端来检测或确定本地约束，例如电池电量低或拓扑变化。对于这些场景，
可以使用此 API 在本地更改配置。

.. note::
   配置节点之前的运行时配置更改将不会存储在 :ref:`持久性储存 <bluetooth_mesh_persistent_storage>`.

API 参考
*************

.. doxygengroup:: bt_mesh_cfg
