.. _bt_l2cap:

Logical Link Control and Adaptation Protocol (L2CAP)
####################################################

L2CAP layer enables connection-oriented channels which can be enable with the
configuration option ``CONFIG_BT_L2CAP_DYNAMIC_CHANNEL``. This channels
transparently support segmentation and reassembly, 
as well as credit-based flow control, making them suitable for data streaming.

Channels instances are represented by the :c:struct:`bt_l2cap_chan` struct which
contains the callbacks  within the :c:struct:`bt_l2cap_chan_ops` struct to inform
when the channel has been connected, disconnected,or when the encryption has
changed.
In addition to that it also contains the ``recv`` callback which is called
whenever an incoming data has been received. Data received this way can be
marked as processed by returning 0 or using
:c:func:`bt_l2cap_chan_recv_complete` API if processing is asynchronous.

.. note::
  The ``recv`` callback is invoked directly from the RX thread, 
  so it is not advisable to block for an extended period.

For sending data the :c:func:`bt_l2cap_chan_send` API can be used , noting that
it may block if no credits are available, and resuming as soon as more credits
are available.

Servers can be registered using :c:func:`bt_l2cap_server_register` API passing
the :c:struct:`bt_l2cap_server` struct which informs what ``psm`` it should
listen to, the required security level ``sec_level``, and the callback
``accept`` which is called to authorize incoming connection requests and
allocate channel instances.

Client channels can be initiated using the :c:func:`bt_l2cap_chan_connect`
API and disconnected using the :c:func:`bt_l2cap_chan_disconnect` API.
Note that the later can also disconnect channel instances created by  the servers.

API Reference
*************

.. doxygengroup:: bt_l2cap
