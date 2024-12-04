.. _bt_mesh_proxy:

代理
#####

代理功能允许电话等传统设备通过 GATT 访问 Bluetooth Mesh 网络。仅当设置了 ``CONFIG_BT_MESH_GATT_Proxy`` 选项时，才会在中编译代理功能。代理功能状态由 :ref:`bluetooth_mesh_models_cfg_srv` 控制，初始值可以用 :c:member:`bt_mesh_cfg_srv.gatt_Proxy` 设置。

启用代理功能的节点可以使用网络标识和节点标识进行播发，这由 :ref:`bluetooth_mesh_models_cfg_cli` 控制。

GATT 代理状态指示是否支持代理功能。

专用代理服务器
****************

支持代理功能和 :func:`blueooth_mesh_models_priv_beacon_srv` 模型的节点可以使用专用网络标识和专用节点标识类型进行播发，这些类型由 :func:`blueooth _mesh_models_prif_beacon_cli` 控制。通过使用这组标识类型进行广告，节点允许传统设备通过 GATT 连接到网络，同时维护网络的隐私。

Private GATT Proxy 状态指示是否支持 Private Proxy 功能。

代理委托书的征求
******************

在节点上禁用 GATT 代理和专用 GATT Proxy 状态的情况下，传统设备无法连接到它
然而，可以请求支持 :func:`blueooth_mesh_od_srv` 的节点来公布可连接的广告事件，
而不启用专用 GATT 代理状态。要请求节点，传统设备可以通过调用 :func:`bt_mesh_proxy_assic` 函数来发送请求 PDU。
要启用此功能，必须使用 ``CONFIG_BT_MESH_PROXY_SOLICITATION`` 选项集编译设备。

请求 PDU 是包含代理请求 UUID 的非网状、不可连接、无方向的广告消息，
使用传统设备要连接到的子网的网络密钥进行加密。PDU 包含传统设备的源地址和序列号。
序列号由传统设备维护，并且对于发送的每个新请求 PDU 递增。

支持请求 PDU 接收的每个节点都持有自己的请求重放保护列表（SRPL）。
SRPL 通过存储节点处理的有效请求 PDU 的请求序列号（SSEQ）和请求源（SSRC）对来保护请求机制免受重播攻击。
更新SRPL和存储对持久存储的更改之间的延迟由 ``CONFIG_BT_MESH_RPL_STORE_TIMEOUT`` 定义。

请求 PDU RPL 配置模型 :func:`bluetooth_mesh_srpl_cli` 和 :func:`blueooth_mesh_srpl_srv` 提供保存和清除 srpl 条目的功能。支持请求 PDU RPL 配置客户端模型的节点可以通过调用 :func:`bt_mesh_sol_PDU_RPL_clear` 函数来清除目标上的 SRPL 部分。使用应用程序密钥加密请求 PDU RPL 配置客户端和服务器之间的通信，因此，可以在网络中的任何设备上实例化请求 PDU RPC 配置客户端。

当节点接收到请求 PDU 并成功对其进行身份验证时，它将开始播发具有专用网络标识类型的可连接广告。播发的持续时间可以由按需专用代理客户端模型配置

API 参考
*************

.. doxygengroup:: bt_mesh_proxy
