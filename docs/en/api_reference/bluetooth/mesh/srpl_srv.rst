.. _bluetooth_mesh_srpl_srv:

PDU RPL Configuration Server Request
#########################################

The PDU RPL Configuration Server model is a foundation model defined by the Bluetooth
mesh specification. The model is enabled if the node has the :ref:`bluetooth_mesh_od_srv` enabled.

The PDU RPL Configuration Server model was introduced in the Bluetooth mesh protocol
specification version 1.1, and manages the Sequence Replay Protection List (SRPL) saved on the
device. The SRPL is used to reject request PDUs that the node has already processed. When the node successfully processes a valid request PDU message, 
the message's SSRC and SSEQ fields are stored in the node's SRPL.

The PDU RPL Configuration Server does not have an API of its own, and relies on a
:ref:`bluetooth_mesh_srpl_cli` to control it. The model only accepts messages encrypted with an
application key as configured by the Configuration Client.

The PDU RPL Configuration Server model, if present, can only be instantiated on the primary element.

Configuration
**************

For the PDU RPL Configuration Server model, the``CONFIG_BT_MESH_PROXY_SRPL_SIZE`` option can be configured to set the size of the
SRPL.

API Reference
*************

.. doxygengroup:: bt_mesh_sol_pdu_rpl_srv
   :project: wm-iot-sdk-apis
   :members:
