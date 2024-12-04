.. _bluetooth_mesh_sar_cfg:

分段和重组（SAR）
#################################

分段和重组（SAR）提供了一种在网状网络中处理更大的上层传输层消息的方法，目的是提高 Bluetooth mesh 吞吐量。下传输层使用分段和重组机制。

下部传输层定义如何将上部传输层 PDU 分段并重新组装为多个下部传输 PDU，
并将它们发送到对等设备上的下部传输层。如果上部传输 PDU 合适，则它将在单个下部传输 PDU 中发送。
对于不适合单个下部传输 PDU 的较长分组，下部传输层执行分段，将上部传输 PDU 拆分为多个段。

接收设备上的下部传输层在将段向上传递之前将其重新组装为单个上部传输 PDU。
分段消息的传递由接收节点的较低传输层确认，而未分段的消息传递不被确认。
然而，当需要下层传输层的确认时，适合于一个下层传输 PDU 的上层传输 PDU 也可以作为单段分段消息发送。
设置 ``send-rel`` 标志（参见 :c:struct:`bt_mesh_msg_ctx` ）以使用可靠的消息传输并确认单段分段消息。

传输层能够通过其 SAR 机制传输多达32个段，最大消息（PDU）大小为384字节。要配置 Bluetooth Mesh 堆栈的消息大小，请使用以下 Kconfig 选项：

* ``CONFIG_BT_MESH_RX_SEG_MAX`` ，设置传入消息中的最大段数。
* ``CONFIG_BT_MESH_TX_SEG_MAX`` ，设置传出消息中的最大段数。

Kconfig 选项 ``CONFIG_BT_MESH_TX_SEG_MSG_COUNT`` 和 ``CONFIG_MET_MESH_RX_SEG-MSG_COUNT`` 。定义可以同时处理多少传出和传入分段消息。当多个分段消息被发送到同一目的地时，消息被排队并一次发送一个。

传入和传出分段消息共享相同的池来分配其分段。该池大小通过 ``CONFIG_BT_MESH_SEG_BUFS`` Kconfig 选项配置。传入和传出消息都在事务开始时分配段。传出分段消息在接收方确认其分段后立即逐个释放其分段，而传入消息在完全接收到消息后首先释放分段。在定义缓冲区的大小时，请记住这一点。

SAR 不会对每个段的接入层有效负载施加额外的开销。

分段和重组（SAR）配置模型
**************************************************************

通过第1.1版蓝牙网状协议规范，可以使用 SAR 配置模型在网状网络上配置 SAR 行为，如间隔、计时器和重传计数器：

* :ref:`bluetooth_mesh_sar_cfg_cli`
* :ref:`bluetooth_mesh_sar_cfg_srv`

无论节点上是否存在 SAR 配置服务器，以下 SAR 行为都适用。

段的传输由段传输间隔分开(请参阅 `SAR 段间隔步长`_ )。可用于分段和重新组装的其他可配置时间间隔和延迟为：

* 单播重传之间的间隔 (请参阅  `SAR 单播重传间隔步长`_ 和 `SAR 单播重传间隔增量`_ )。
* 多播重传之间的间隔(请参阅 `SAR 多播重传间隔步长`_ )。
* 段接收间隔(请参阅 `SAR 接收机段间隔步长`_ )。
* 确认延迟增量(请参阅 `SAR 确认延迟增量`_ )。

当传输标记为未确认的最后一个段时，较低的传输层启动重传计时器。SAR 单播重传计时器的初始值取决于消息的 TTL 字段的值。如果 TTL 字段值大于 ``0``，则根据以下公式设置计时器的初始值：

.. math::

   unicast~retransmissions~interval~step + unicast~retransmissions~interval~increment \times (TTL - 1)


如果 TTL 字段值为 ``0`` ，则定时器的初始值被设置为单播重传间隔步骤。

SAR 多播重传计时器的初始值设置为多播重传输间隔。

当较低的传输层接收到消息段时，它启动 SAR Discard 计时器。丢弃计时器告知下传输层在丢弃该段所属的分段消息之前等待的时间。 SAR Discard 计时器的初始值是由 `SAR 丢弃超时`_  状态指示的丢弃超时值。

SAR 确认计时器保存为接收的段发送段确认消息之前的时间。SAR 确认计时器的初始值使用以下公式计算：

.. math::

   min(SegN + 0.5 , acknowledgment~delay~increment) \times segment~reception~interval


``SegN`` 字段值标识上部传输 PDU 分段的段总数。

四个计数器与 SAR 行为相关：

* 两个单播重传计数 (请参阅 `SAR 单播重新传输计数`_ 状态和 `无进度计数的 SAR 单播重新传输`_ 状态)
* 多播重新传输计数 (请参阅 `SAR 多播重新传输计数`_ 状态)
* 确认重新传输计数 (请参阅 `SAR 确认重新传输计数`_ 状态)

如果传输中的段数高于 `SAR 段阈值`_ 状态的值，使用 `SAR 确认重新传输计数`_ 状态的值重新传输段确认消息。

.. _bt_mesh_sar_cfg_states:

SAR 状态
**************

定义了两种与分段和重组相关的状态：

* SAR 发射机状态
* SAR 接收机状态

SAR 发射机状态是控制分段消息传输的数量和定时的复合状态。它包括以下状态：

* SAR 段间隔步长
* SAR 单播重新传输计数
* 无进度计数的 SAR 单播重新传输
* SAR 单播重传间隔步长
* SAR 单播重传间隔增量
* SAR 多播重新传输计数
* SAR 多播重传间隔步长

SAR 接收机状态是一种复合状态，它控制分段确认传输的数量和时序，以及分段消息重新组装的丢弃。它包括以下状态：

* SAR 段阈值
* SAR 丢弃超时
* SAR 确认延迟增量
* SAR 确认重新传输计数
* SAR 接收机段间隔步长

SAR 段间隔步长
=========================

SAR 段间隔步长状态保存一个值，该值控制分段消息的段的传输之间的间隔，间隔以毫秒为单位。

使用 ``CONFIG_BT_MESH_SAR_TX_SEG_INT_STEP`` Kconfig 选项设置默认值。然后使用以下公式计算分段传输间隔：

.. math::

   (\mathtt{CONFIG\_BT\_MESH\_SAR\_TX\_SEG\_INT\_STEP} + 1) \times 10~\text{ms}


SAR 单播重新传输计数
=================================

SAR 单播重传计数保存一个值，该值定义分段消息到单播目标的最大重传次数。使用 ``CONFIG_BT_MESH_SAR_TX_UNICAST_RETRANS_COUNT`` Kconfig 选项设置此状态的默认值。

无进度计数的 SAR 单播重新传输
==================================================

该状态保存一个值，该值定义了如果在超时期间没有接收到确认，或者如果接收到具有已确认段的确认，则将发送的单播地址的分段消息的最大重传次数。使用 Kconfig 选项 ``CONFIG_BT_MESH_SAR_TX_UNICAST_RETRANS_WITHOUT_PROG_COUNT`` 设置最大重新传输次数。

SAR 单播重传间隔步长
=========================================

该状态的值控制用于延迟分段消息的未确认段到单播地址的重新传输的间隔步长，间隔步长以毫秒为单位。

使用 ``CONFIG_BT_MESH_SAR_TX_UNICAST_RETRANS_INT_STEP`` Kconfig 选项设置默认值。然后，该值用于使用以下公式计算间隔步长：

.. math::

   (\mathtt{CONFIG\_BT\_MESH\_SAR\_TX\_UNICAST\_RETRANS\_INT\_STEP} + 1) \times 25~\text{ms}


SAR 单播重传间隔增量
==============================================

SAR 单播重传间隔增量保存用于将分段消息的未确认段的重传延迟到单播地址的间隔增量的值，增量以毫秒为单位。

使用 Kconfig 选项 ``CONFIG_BT_MESH_SAR_TX_UNICAST_RETRANS_INT_INC`` 设置默认值。Kconfig 选项值用于使用以下公式计算增量：

.. math::

   (\mathtt{CONFIG\_BT\_MESH\_SAR\_TX\_UNICAST\_RETRANS\_INT\_INC} + 1) \times 25~\text{ms}


SAR 多播重新传输计数
===================================

该状态保存一个值，该值控制分段消息到多播地址的重新传输总数。使用 Kconfig 选项 ``CONFIG_BT_MESH_SAR_TX_MULTICAST_RETRANS_COUNT`` 设置重新传输的总数。

SAR 多播重传间隔步长
===========================================

该状态保存一个值，该值控制分段消息中所有分段重新传输到多播地址的间隔。间隔以毫秒为单位。

使用 Kconfig 选项 ``CONFIG_BT_MESH_SAR_TX_MULTICAST_RETRANS_INT`` 设置用于使用以下公式计算间隔的默认值：

.. math::

   (\mathtt{CONFIG\_BT\_MESH\_SAR\_TX\_MULTICAST\_RETRANS\_INT} + 1) \times 25~\text{ms}


SAR 丢弃超时
===================

该状态的值定义了在接收分段消息的分段之后，下传输层在丢弃该分段消息之前等待的时间（以秒为单位）。使用 Kconfig 选项 ``CONFIG_BT_MESH_SAR_RX_DISCARD_TIMEOUT`` 设置默认值。丢弃超时将使用以下公式计算

.. math::

   (\mathtt{CONFIG\_BT\_MESH\_SAR\_RX\_DISCARD\_TIMEOUT} + 1) \times 5~\text{seconds}


SAR 确认延迟增量
==================================

该状态保存一个值，该值控制用于在接收到新段后延迟确认消息的传输的间隔的延迟增量。增量以段为单位进行测量。

使用 Kconfig 选项 ``CONFIG_BT_MESH_SAR_RX_ACK_DELAY_INC`` 设置默认值。增量值计算为
:math:`\verb|CONFIG_BT_MESH_SAR_RX_ACK_DELAY_INC| + 1.5`.

SAR 段阈值
======================

SAR 段阈值状态保存一个值，该值在用于确认重新传输的分段消息的段数中定义阈值。使用 Kconfig 选项 ``CONFIG_BT_MESH_SAR_RX_SEG_THRESHOLD`` 设置阈值。

当分段消息的段数高于此阈值时，堆栈将额外重新传输每个确认消息，传输次数由 ``CONFIG_BT_MESH_SAR_RX_ACK_RETRANS_COUNT`` 的值给定。

SAR 确认重新传输计数
========================================

SAR 确认重传计数状态控制较低传输层发送的段确认消息的重传次数。它给出了当分段消息中的段大小高于 ``CONFIG_BT_MESH_SAR_RX_SEG_THRESHOLD`` 值时，堆栈将额外发送的确认消息的重新调试总数。

使用 Kconfig 选项 ``CONFIG_BT_MESH_SAR_RX_ACK_RETRANS_COUNT`` 设置此状态的默认值。段确认消息的最大传输次数为
:math:`\verb|CONFIG_BT_MESH_SAR_RX_ACK_RETRANS_COUNT| + 1`.

SAR 接收机段间隔步长
==================================

SAR 接收机段间隔步长定义用于在接收到新段后延迟确认消息的传输的段接收间隔步长，间隔以毫秒为单位测量。

使用 Kconfig 选项 ``CONFIG_BT_MESH_SAR_RX_SEG_INT_STEP`` 设置默认值，并使用以下公式计算间隔：

.. math::

   (\mathtt{CONFIG\_BT\_MESH\_SAR\_RX\_SEG\_INT\_STEP} + 1) \times 10~\text{ms}
