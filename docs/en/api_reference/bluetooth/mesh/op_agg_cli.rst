.. _bluetooth_mesh_models_op_agg_cli:

Opcode Aggregator Client
#########################

The Opcodes Aggregator Client model is a foundation model defined by the Bluetooth Mesh
specification. It is an optional model, enabled through the ``CONFIG_BT_MESH_OP_AGG_CLI``
option.

Introduced in Bluetooth Mesh Protocol Specification version 1.1, the Opcode Aggregator Client model supports 
the functionality of sending a series of Access Layer messages to nodes that support the :ref:`bluetooth_mesh_models_op_agg_srv` model.

The Opcodes Aggregator Client model communicates with the Opcodes Aggregator Server model using 
either the target node's device key or an application key configured by the Configuration Client.

If present, the Opcodes Aggregator Client model must only be instantiated on the primary element.

Upon initialization, the Opcode Aggregator Client model is implicitly bound to the device key. It should be bound to 
the same application key used by the client models that generate the message sequence.

To be able to aggregate a message from a client model, it should support an asynchronous API, for
example through callbacks.

API Reference
*************

.. doxygengroup:: bt_mesh_op_agg_cli
   :project: wm-iot-sdk-apis
   :members:
