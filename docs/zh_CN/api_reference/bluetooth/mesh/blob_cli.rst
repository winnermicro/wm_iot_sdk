.. _bluetooth_mesh_blob_cli:

BLOB 传输客户端
####################

二进制大型对象（BLOB）传输客户端是 BLOB 传输的发送方。它支持将任何大小的 BLOB 发送到任意数量的目标节点，无论是 Push BLOB 传输模式还是 Pull BLOB 转移模式。

使用
===============

初始化
==============

BLOB 传输客户端在具有一组事件处理程序回调的元素上实例化:

.. code-block:: C

   static const struct bt_mesh_blob_cli_cb blob_cb = {
         /* Callbacks */
   };

   static struct bt_mesh_blob_cli blob_cli = {
         .cb = &blob_cb,
   };

   static const struct bt_mesh_model models[] = {
         BT_MESH_MODEL_BLOB_CLI(&blob_cli),
   };

传输上下文
================

传输功能检索过程和 BLOB 传输都使用 :func:`bt_mesh_BLOB_cli_input` 的实例来确定如何执行传输。BLOB 传输客户端输入结构在用于过程之前，必须至少用目标列表、应用程序密钥和生存时间（TTL）值进行初始化：

.. code-block:: c

   static struct bt_mesh_blob_target targets[3] = {
           { .addr = 0x0001 },
           { .addr = 0x0002 },
           { .addr = 0x0003 },
   };
   static struct bt_mesh_blob_cli_inputs inputs = {
           .app_idx = MY_APP_IDX,
           .ttl = BT_MESH_TTL_DEFAULT,
   };

   sys_slist_init(&inputs.targets);
   sys_slist_append(&inputs.targets, &targets[0].n);
   sys_slist_append(&inputs.targets, &targets[1].n);
   sys_slist_append(&inputs.targets, &targets[2].n);

请注意，传输中的所有 BLOB 传输服务器都必须绑定到所选的应用程序密钥。


逻辑地址
-------------

应用程序可以在上下文结构中额外指定组地址。如果组不是 ``BT_MESH_ADDR_UNASSIGNED`` ，则传输中的消息将发送到组地址，而不是单独发送到每个目标节点。Mesh Manager 必须确保具有 BLOB 传输服务器模型的所有目标节点都订阅该组地址。

使用组地址传输 BLOB 通常可以提高传输速度，因为 BLOB 传输客户端同时将每个消息发送到所有目标节点。
然而，在 Bluetooth Mesh 中向组地址发送大型分段消息通常不如将它们发送到单播地址可靠，因为组没有传输层确认机制。
这可能导致每个块末尾的恢复周期更长，并增加丢失目标节点的风险。
使用组地址进行 BLOB 传输通常只有在目标节点列表广泛的情况下才会有回报，
并且每个寻址策略的有效性在不同的部署和块的大小之间会有很大差异。

传输超时时间
----------------

如果目标节点未能在 BLOB 传输客户端的时间限制内响应确认的消息，则会从传输中删除目标节点。
应用程序可以通过上下文结构为 BLOB 传输客户端提供额外的时间，从而减少这种可能性。
除了20秒的基本时间外，额外时间可以以10秒为增量设置，最长可达182小时。等待时间随传输 TTL 自动调整。

请注意，BLOB 传输客户端仅在以下情况下与传输一起向前移动：

* 所有目标节点都已响应。
* 已从目标节点列表中删除节点。
* BLOB 传输客户端超时。

增加等待时间将增加该延迟。

BLOB 传输功能检索
====================================

通常建议在开始传输之前检索 BLOB 传输功能。该过程使用允许所有目标节点参与传输的最自由的参数集填充来自所有目标节点的传输功能。任何未能响应或使用不兼容的传输参数响应的目标节点都将被丢弃。

根据目标节点在目标节点列表中的顺序对目标节点进行优先级排序。如果发现目标节点与任何先前的目标节点不兼容，例如通过报告不重叠的块大小范围，则将删除该节点。丢失的目标节点将通过 :c:member:`lost_target <bt_mesh_blob_cli_cb.lost_target>` 回调报告。

过程的结束通过 :c:member:`caps<bt_mesh_blob_cli_cb.caps>` 回调发出信号，生成的功能可用于确定 BLOB 传输所需的块和块大小。

BLOB 传输
=============

BLOB 传输通过调用 :c:member:`bt_mesh_BLOB_cli_send` 函数启动，该函数（除了前面提到的传输输入外）需要一组传输参数和 BLOB 流实例。
传输参数包括64位 BLOB ID、BLOB 大小、传输模式、对数表示的块大小和块大小。BLOB ID 是应用程序定义的，
但必须与启动 BLOB 传输服务器的BLOB标识匹配。

传输将一直运行，直到至少一个目标节点的传输成功完成，或者传输被取消。传输的结束通过结束回调与应用程序通信。丢失的目标节点将通过 :c:member:`lost_target <bt_mesh_blob_cli_cb.lost_target>` 回调报告。

头文件
===============

- wm_iot_sdk/components/bluetooth/include/bluetooth/mesh/blob_cli.h
- This header file can be included with:

.. code-block:: c
   :emphasize-lines: 1

   #include "bluetooth/mesh/blob_cli.h"

API 参考
===============

.. doxygengroup:: bt_mesh_blob_cli
   :project: wm-iot-sdk-apis
   :members:
