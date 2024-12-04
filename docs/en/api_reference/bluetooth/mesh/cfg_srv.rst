.. _bluetooth_mesh_models_cfg_srv:

Configuration Server
####################

The Configuration Server model is a fundamental model defined by the Bluetooth Mesh
specification. The Configuration Server model controls most parameters of the
mesh node. It does not have an API of its own, but relies on the
:ref:`bluetooth_mesh_models_cfg_cli` to control it.

.. note::
   The :c:struct:`bt_mesh_cfg_srv` structure has been deprecated. The initial
   values for relaying, beaconing, friending, network transmission, and relay 
   retransmissions should be set through Kconfig., and the Heartbeat functionality should be
   managed through the :ref:`bluetooth_mesh_heartbeat` API.

The Configuration Server model is mandatory on all Bluetooth Mesh nodes, and
must only be instantiated on the primary element.

API Reference
*************

.. doxygengroup:: bt_mesh_cfg_srv
