.. _bluetooth_mesh_heartbeat:

Heartbeat
#########

The Heartbeat feature provides the capability to monitor Bluetooth Mesh nodes and determine the distance between them.

The Heartbeat feature is configured via :ref:`bluetooth_mesh_models_cfg_srv` model.

Heartbeat Signal Messages
******************************

Heartbeat messages are transmitted as Transport Control Data Packets through the network,
and are encrypted solely using a network key. These messages include the original Time-To-Live (TTL) 
value used for sending the message and a bit-field representing the active features on the node. 
In this way, the receiving node can determine how many relays the message had to pass through to reach the receiver, as well as which features the node supports.

Available Heartbeat feature flags:

- :c:macro:`BT_MESH_FEAT_RELAY`
- :c:macro:`BT_MESH_FEAT_PROXY`
- :c:macro:`BT_MESH_FEAT_FRIEND`
- :c:macro:`BT_MESH_FEAT_LOW_POWER`

Publishing Heartbeat Signals 
*******************************

Heartbeat publication is controlled via the Configuration model and can
be triggered in two ways:

Periodic Publishing
   The node periodically publishes new Heartbeat messages. 
   Publishing can be configured to stop after a certain number of messages or to continue indefinitely.

Triggered Publishing
   The node publishes a new Heartbeat message every time a feature changes. The
   set of features that can trigger a publication is configurable.

The two publication types can be combined.

Subscribing to Heartbeat Signals
**********************************

Nodes can be configured to subscribe to Heartbeat messages from one node at
a time. To receive a Heartbeat message, both the source and destination must
match the configured subscription parameters.

Nodes can be configured to subscribe to Heartbeat messages from one node at a time. 
To receive the messages, both the source and destination must match the configured subscription parameters.

All Heartbeats received with the configured subscription parameters are passed
to the :cpp:member:`bt_mesh_hb_cb::recv` event handler.

When the Heartbeat subscription period ends, the :cpp:member:`bt_mesh_hb_cb::sub_end` callback will be invoked.

API Reference
**************

.. doxygengroup:: bt_mesh_heartbeat
