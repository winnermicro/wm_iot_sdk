.. _bluetooth_mesh_cfg:

Runtime Configuration
#####################

The Runtime Configuration API allows applications to directly modify their runtime 
configurations without navigating through the configuration model.

Bluetooth Mesh nodes are typically configured by a central network with a Configuration Client model, 
as referenced by :ref:`bluetooth_mesh_models_cfg_cli`. 
Each mesh node instantiates a Configuration Server model:ref:`bluetooth_mesh_models_cfg_srv` model that the
Configuration Client can communicate with to change the node configuration. In some
cases, the mesh node can't rely on the Configuration Client to detect or determine
local constraints, such as low battery power or changes in topology. For these
scenarios, this API can be used to change the configuration locally.

.. note::
   Runtime configuration changes before the node is provisioned will not be stored
   in the :ref:`persistent storage <bluetooth_mesh_persistent_storage>`.

API Reference
*************

.. doxygengroup:: bt_mesh_cfg
