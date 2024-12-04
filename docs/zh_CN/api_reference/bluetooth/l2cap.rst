.. _bt_l2cap:

逻辑链路控制和适配协议（L2CAP）
####################################################

L2CAP 层支持面向连接的通道，可以使用配置选项 ``CONFIG_BT_L2CAP_DYNAMIC_CHANNEL`` 。
这个频道透明地支持分段和重组，它们也支持信用基于流控制，使其适合于数据流。

通道实例由 :c:struct:`bt_l2cap_chan` 结构表示，该结构包含要通知的： :c:struct:`bt_l2cap_chan_ops` 结构中的回调通道已连接、断开连接或加密已更改。
除此之外，它还包含调用的 ``recv`` 回调每当接收到传入数据时。
以这种方式接收的数据可以是通过返回0或使用将标记为已处理： :c:func:`bt_l2cap_chan_recv_complete` API（如果处理是异步的）。

.. note::
  ``recv`` 回调直接从 RX 线程调用，因此不建议长时间阻塞。

对于发送数据，可以使用 :c:func:`bt_l2cap_chan_send` API，注意
如果没有可用的学分，它可能会阻止，并在获得更多学分后立即恢复可用。

可以使用 :c:func:`bt_l2cap_server_register` API 传递来注册服务器 :c:struct:`btl2cap_server` 结构，通知它应该执行什么 ``psm`` 侦听、所需的安全级别 ``seclevel`` 和回调 ``accept`` ，调用它来授权传入的连接请求，并分配通道实例。

可以使用 :c:func:`bt_l2cap_chan_connect` 启动客户端通道 API，并且可以与 :c:func:`bt_l2cap_chan_disconnect` API 断开连接。
请注意，后者还可以断开服务器创建的通道实例。

API 参考
*************

.. doxygengroup:: bt_l2cap
