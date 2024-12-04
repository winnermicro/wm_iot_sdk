.. _bluetooth_mesh_models_op_agg_srv:

Opcode Aggregator Server
#########################

The Opcodes Aggregator Server model is a foundation model defined by the Bluetooth
mesh specification. It is an optional model, enabled with the
``CONFIG_BT_MESH_OP_AGG_SRV`` option.

Introduced in Bluetooth Mesh Protocol Specification version 1.1, the Opcode Aggregator Server model supports 
the functionality of processing sequences of Access Layer messages.

The Opcodes Aggregator Server model accepts messages encrypted with the node's device key
or the application keys.

If present, the Opcodes Aggregator Server model must only be instantiated on the primary element.

The targeted server models should be bound to the same application key that is used
to encrypt the sequence of access layer messages sent to the Opcodes Aggregator Server.

The Opcode Aggregator Server processes the aggregated messages and forwards them to the corresponding models and their message handlers. 
The current implementation assumes that responses are from the same execution context as the received messages and does not allow for sending delayed responses, 
such as from a work queue.

API Reference
*************

.. doxygengroup:: bt_mesh_op_agg_srv
   :project: wm-iot-sdk-apis
   :members:
