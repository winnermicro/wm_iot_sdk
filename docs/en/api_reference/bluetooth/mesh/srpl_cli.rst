.. _bluetooth_mesh_srpl_cli:

PDU RPL Configuration Client Request
#########################################

The PDU RPL Configuration Client model is a foundation model defined by the Bluetooth
mesh specification. The model is optional, and is enabled through the
``CONFIG_BT_MESH_SOL_PDU_RPL_CLI`` option.

The PDU RPL Configuration Client model was introduced in the Bluetooth Mesh Protocol
Specification version 1.1, and supports the functionality of removing addresses from the
solicitation replay protection list (SRPL) of a node that supports the
:ref:`bluetooth_mesh_srpl_srv` model.

The PDU RPL Configuration Client model communicates with a Solicitation PDU RPL
Configuration Server model using the application keys configured by the Configuration Client.

The PDU RPL Configuration Client model, if present, can only be instantiated on the primary element.

Configuration
**************

The  behavior of the Solicitation PDU RPL Configuration Client model can be configured using the transmission
timeout option ``CONFIG_BT_MESH_SOL_PDU_RPL_CLI_TIMEOUT``. The
``CONFIG_BT_MESH_SOL_PDU_RPL_CLI_TIMEOUT`` controls how long the Solicitation PDU RPL
Configuration Client waits for a response message to arrive in milliseconds. This value can be
changed at runtime using :c:func:`bt_mesh_sol_pdu_rpl_cli_timeout_set`.

API Reference
*************

.. doxygengroup:: bt_mesh_sol_pdu_rpl_cli
   :project: wm-iot-sdk-apis
   :members:
