.. _bluetooth_mesh_heartbeat:

心跳
#########

心跳功能提供了监测 Bluetooth Mesh 节点和确定节点之间距离的功能。

心跳功能通过 :ref:`bluetooth_mesh_models_cfg_srv` 模式

心跳信号消息
===============

心跳消息作为传输控制数据包通过网络发送，并且仅使用网络密钥加密。
心跳消息包含用于发送消息的原始生存时间（TTL）值和节点上活动功能的位字段。
通过这种方式，接收节点可以确定消息必须经过多少个中继才能到达接收器，以及节点支持哪些功能。

可用的心跳信号功能标志：

- :c:macro:`BT_MESH_FEAT_RELAY`
- :c:macro:`BT_MESH_FEAT_PROXY`
- :c:macro:`BT_MESH_FEAT_FRIEND`
- :c:macro:`BT_MESH_FEAT_LOW_POWER`

心跳信号发布
===============

心跳发布通过配置模型控制，可以通过两种方式触发：

定期发布
  该节点定期发布新的心跳消息。可以将发布配置为在一定数量的消息后停止，或无限期地继续。

触发发布
   每次功能更改时，节点都会发布新的心跳消息。可以触发发布的功能集是可配置的。

这两种发布类型可以组合。

心跳信号订阅
===============

可以将节点配置为一次从一个节点订阅心跳消息。要接收心跳消息，源和目标都必须与配置的订阅参数匹配。

可以将节点配置为一次从一个节点订阅心跳消息。要接收心跳消息，源和目标都必须与配置的订阅参数匹配。

使用配置的订阅参数接收的所有心跳都将传递给 :cpp:member:`bt_mesh_hb_cb::recv` 事件处理程序。

当心跳订阅周期结束时，将调用 :cpp:member:`bt_mesh_hb_cb::sub_end` 回调。

API 参考
===============

头文件
-----------

- wm_iot_sdk/components/bluetooth/include/bluetooth/mesh/heartbeat.h
- This header file can be included with:

.. code-block:: c
   :emphasize-lines: 1

   #include "bluetooth/mesh/heartbeat.h"

.. doxygengroup:: bt_mesh_heartbeat
