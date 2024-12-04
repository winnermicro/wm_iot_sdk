.. _bluetooth_mesh_models_rpr_cli:

Remote Provisioning Client
##########################

The Remote Provisioning Client model is a foundation model defined by the Bluetooth
mesh specification. It is enabled with the
``CONFIG_BT_MESH_RPR_CLI`` option.

Introduced in the Bluetooth Mesh Protocol Specification version 1.1, 
the Remote Configuration Client model provides the capability to remotely configure devices into a mesh network. 
It performs the Node Configuration Protocol Interface (NCPI) process by interacting with mesh nodes that support the
:ref:`bluetooth_mesh_models_rpr_srv` model.

The Remote Provisioning Client model communicates with a Remote Provisioning Server model
using the device key of the node containing the target Remote Provisioning Server model instance.

If present, the Remote Provisioning Client model must be instantiated on the primary
element.

Scanning
********

The scanning procedure is used to scan for unprovisioned devices located nearby the Remote
Provisioning Server. The Remote Provisioning Client starts a scan procedure by using the
:c:func:`bt_mesh_rpr_scan_start` call:

.. code-block:: C

      static void rpr_scan_report(struct bt_mesh_rpr_cli *cli,
                  const struct bt_mesh_rpr_node *srv,
                  struct bt_mesh_rpr_unprov *unprov,
                  struct net_buf_simple *adv_data)
      {

      }

      struct bt_mesh_rpr_cli rpr_cli = {
         .scan_report = rpr_scan_report,
      };

      const struct bt_mesh_rpr_node srv = {
         .addr = 0x0004,
         .net_idx = 0,
         .ttl = BT_MESH_TTL_DEFAULT,
      };

      struct bt_mesh_rpr_scan_status status;
      uint8_t *uuid = NULL;
      uint8_t timeout = 10;
      uint8_t max_devs = 3;

      bt_mesh_rpr_scan_start(&rpr_cli, &srv, uuid, timeout, max_devs, &status);

The example above demonstrates pseudocode used to initiate a scanning process on a target Remote Configuration Server node. 
This process will start a multi-device scan for 10 seconds, with the generated scan report containing up to three unconfigured devices.
If a UUID parameter is specified, the same process will only scan for devices with the corresponding UUID. Upon completion of the process,
the server sends a scan report that will be handled in the client's  :c:struct:`bt_mesh_rpr_cli.scan_report` callback .

Furthermore, the Remote Configuration Client model also supports extended scanning using the :c:struct:`bt_mesh_rpr_scan_start_ext`  call. 
Extended scanning complements regular scanning by allowing the Remote Configuration Server to report additional data for specific devices. 
The Remote Configuration Server will use active scanning to request scan responses from unconfigured devices (if supported by the unconfigured devices).

Pre-Provisioning
*******************

The Remote Configuration Client initiates the provisioning process by using the :c:struct:`bt_mesh_provision_Remote` call：

.. code-block:: C

      struct bt_mesh_rpr_cli rpr_cli;

      const struct bt_mesh_rpr_node srv = {
         .addr = 0x0004,
         .net_idx = 0,
         .ttl = BT_MESH_TTL_DEFAULT,
      };

      uint8_t uuid[16] = { 0xaa };
      uint16_t addr = 0x0006;
      uint16_t net_idx = 0;

      bt_mesh_provision_remote(&rpr_cli, &srv, uuid, net_idx, addr);


The example above demonstrates pseudocode for remotely configuring a device through a Remote Configuration Server node. 
This process attempts to provide the device with the corresponding UUID and assigns address 0x0006 to its primary element using the network key located at index zero.


.. note::

    During remote configuration, the same :c:struct:`bt_mesh_prov` callbacks as in regular provisioning are triggered. For detailed information, refer to the section :ref:`bluetooth_mesh_provisioning` .

Re-Provisioning
******************

Beyond scanning and configuration capabilities, the Remote Configuration Client also offers support for the  :ref:`bluetooth_mesh_models_rpr_srv`  model. 
This is provided through the Node Configuration Protocol Interface (NPPI), which supports the following three procedures:

* Device Key Refresh Procedure: Used to change the device key of the target node without the need to reconfigure the node.
* Node Address Refresh Procedure: Used to change both the device key and unicast address of the node.
* Node Composition Refresh Procedure: Used to change the device key and add or remove models or functionalities of the node.


The three NPPI procedures can be initiated using the :c:struct:`bt_mesh_reprovision_remote` call:

.. code-block:: C

      struct bt_mesh_rpr_cli rpr_cli;
      struct bt_mesh_rpr_node srv = {
         .addr = 0x0006,
         .net_idx = 0,
         .ttl = BT_MESH_TTL_DEFAULT,
      };

      bool composition_changed = false;
      uint16_t new_addr = 0x0009;

      bt_mesh_reprovision_remote(&rpr_cli, &srv, new_addr, composition_changed);


The example above demonstrates pseudocode used to trigger the Node Address Refresh Procedure on a target node. The specific procedure is not directly selected but chosen based on other input parameters. In this example,
we can see that the current unicast address of the Target is 0x0006, and the new address is set to 0x0009.
If the two addresses are the same and the  ``composition_changed``  flag is set to true, the code will trigger the Node Composition Refresh Procedure. 
If the two addresses are the same and the  ``composition_changed``  flag is set to false, the code will trigger the Device Key Refresh Procedure.

API Reference
*************

.. doxygengroup:: bt_mesh_rpr_cli
   :project: wm-iot-sdk-apis
   :members:
