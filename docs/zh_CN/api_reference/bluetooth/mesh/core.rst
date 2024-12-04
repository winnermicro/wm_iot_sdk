.. _bluetooth_mesh_core:

核心
####

核心提供管理通用蓝牙 Mesh 的功能状态。

.. _bluetooth_mesh_lpn:

低功耗节点
**************

低功耗节点（LPN）角色允许电池供电的设备作为叶节点参与网状网络。
LPN 通过 Friend 节点与网状网络交互，Friend 负责中继定向到 LPN 的任何消息。
LPN 通过保持其无线电关闭来节省电力，并且仅在发送消息或轮询 Friend 节点以获取任何传入消息时唤醒。

无线控制和轮询由网格堆栈自动管理，但 LPN API 允许应用程序通过 :c:func:`bt_mesh_LPN_poll()` 随时触发轮询。LPN 操作参数，包括轮询间隔、轮询事件时序和 Friend 要求，通过 :c:macro:`CONFIG_BT_MESH_LOW_POWER` 选项和相关配置选项进行控制。

将 LPN 功能用于日志记录时，强烈建议仅使用 :c:macro:`CONFIG_LOG_MODE_DEFERRED` 选项。除 Deferred 之外的日志模式可能会在处理日志消息期间导致意外的延迟。这将影响接收延迟和接收窗口的调度，相同的限制适用于 :c:macro:`CONFIG_BT_MESH_FRIEND` 选项。

Replay 保护列表
**********************

Replay Protection List（RPL）用于保存最近从网状网络中的元素接收的序列号，以执行针对重播攻击的保护。

为了在重新启动后保护节点免受重播攻击，它需要在断电之前将整个 RPL 存储在持久性存储中。根据网状网络中的流量，存储最近看到的序列号可能会使闪存迟早磨损。要减轻这种情况，可以使用 :c:macro:`CONFIG_BT_MESH_RPL_STORE_TIMEOUT` 。此选项推迟在持久存储中存储 RPL 条目。

然而，该选项并不能完全解决问题，因为节点可能会在触发存储 RPL 的计时器之前关闭电源。确保消息不能重放，节点可以通过调用 :c:func:`bt_mesh_RPL_pending_store` 随时（或在断电之前的足够时间）启动挂起 RPL 条目的存储。这由节点决定，在这种情况下存储哪些 RPL 条目。

将 :c:macro:`CONFIG_BT_MESH_RPL_STORE_TIMEOUT` 设置为-1允许完全关闭计时器，这有助于显著减少闪存磨损。这将存储 RPL 的责任转移到用户应用程序，并要求从调用此 API 到将所有 RPL 条目写入闪存为止有足够的电源备份。

找到正确的平衡 在 :c:macro:`CONFIG_BT_MESH_RPL_STORE_TIMEOUT` 和调用 :c:func:`bt_mesh_rpl_pending_store` 可能会降低安全风险漏洞和闪存磨损。

.. warning:

   无法启用 :c:macro:`CONFIG_BT_SETTINGS` ，或将 :c:macro:`CONFIG-BT_MESH_RPL_STORE_TIMEOUT` 设置为-1并且不存储重新启动之间的 RPL，将使设备易受重放攻击，并且不会执行规范所需的重放保护。

.. _bluetooth_mesh_persistent_storage:

持久性存储
******************

网格堆栈使用 ``Settings Subsystem`` 来持久存储设备配置。当堆栈配置更改并且需要持久存储更改时，堆栈调度工作项。调度工作项和将其提交到工作队列之间的延迟由 :c:macro:`CONFIG_BT_MESH_STORE_TIMEOUT` 选项定义。一旦计划了数据的存储，就不能重新计划它，直到工作项被处理。在某些情况下例外，如下所述。

当必须存储 IV 索引、序列号或 CDB 配置时，工作项将毫不延迟地提交到工作队列。如果工作项以前被调度，则它将被重新调度，而不会延迟。

Replay 保护列表使用相同的工作项来存储 RPL 条目。如果请求存储 RPL 条目，并且没有等待存储的其他配置，则延迟设置为 :c:macro:`CONFIG_BT_MESH_RPL_STORE_TIMEOUT` 。
如果必须存储其他堆栈配置，则延迟由 :c:macro:`CONFIG_BT_MESH_STORE_TIMEOUT` 选项小于 :c:macro:`CONFIG_BT_MESH_RPL_STORE_TIMEOUT` ，工作项为由 Replay 保护列表计划，则将重新计划工作项。

当工作项正在运行时，堆栈将存储所有挂起的配置，包括 RPL 条目。

工作项执行上下文
===========================

:c:macro:`CONFIG_BT_MESH_SETTINGS_WORKQ` 选项配置从中执行工作项的上下文。此选项由启用默认，并导致使用专用协作线程进行堆栈处理工作项。这允许堆栈在存储堆栈配置时处理其他传入和传出消息，以及提交到系统工作队列的其他工作项。

禁用此选项时，工作项将提交到系统工作队列。这意味着系统工作队列在存储堆栈配置所需的时间内被阻塞。不建议禁用此选项，因为这将使设备在相当长的时间内无响应。

.. _bluetooth_mesh_adv_identity:

广告标识
**********************

所有网格堆栈承载器都使用 :c:macro:`BT_ID_DEFAULT` 局部标识播发数据。该值在网格堆栈实现中预设。当 Bluetooth®Low Energy（LE）和 Bluetooth Mesh 在同一设备上共存时，应用程序应在开始通信之前分配和配置另一个本地标识以用于蓝牙 LE。

API 参考
**************

.. doxygengroup:: bt_mesh
