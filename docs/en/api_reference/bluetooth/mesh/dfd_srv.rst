.. _bluetooth_mesh_dfd_srv:

Firmware Distribution Server 
############################

The Firmware Distribution Server model, implemented as part of the :ref:`bluetooth_mesh_dfu` subsystem, fulfills the role of a distribution server. It extends the :ref: `bluetooth_mesh_blob_srv` to receive firmware image 
binary files from initiator nodes. Additionally, it instantiates a. It also instantiates a :ref:`bluetooth_mesh_dfu_cli` for distributing firmware updates across the entire mesh network.

.. note::

   Currently, the Firmware Distribution Server supports out-of-band (OOB) retrieval of firmware
   images over SMP service only.

The Firmware Distribution Server does not have an API of its own, but relies on a Firmware
Distribution Client model on a different device to give it information and trigger image
distribution and upload.

Firmware Slots
**************

The Firmware Distribution Server has the capability to store multiple firmware images for distribution. 
Each slot contains a separate firmware image with metadata and can be distributed to other mesh nodes in the network in any order. 
The content, format, and size of the firmware images are vendor-specific and may contain data from other vendors. Applications should not attempt to execute or modify them.

The slots are managed remotely by a Firmware Distribution Client, which can either upload new slots
or delete old ones. The application is notified of changes to the slots through the Firmware
Distribution Server's callbacks (:cpp:type:`bt_mesh_fd_srv_cb`). While the metadata for each
firmware slot is stored internally, the application must provide a :ref:`bluetooth_mesh_blob_stream`
for reading and writing the firmware image.

API Reference
*************

.. doxygengroup:: bt_mesh_dfd_srv
   :project: wm-iot-sdk-apis
   :members:
