.. _bluetooth_mesh_blob_flash:

BLOB Flash Memory 
##################

The BLOB Flash Memory readers and writer enable the reading and writing of BLOB data to the flash memory partitions 
defined in the flash map API:func:`flash map <flash_map_api>`.


BLOB Flash Reader
*****************

The BLOB Flash Reader interacts with the BLOB Transfer Client to read BLOB data directly from flash memory.
It must be initialized by calling :c:func:`bt_mesh_blob_flash_rd_init` before being passed to the
BLOB Transfer Client. Each BLOB Flash Reader only supports one transfer at the time.


BLOB Flash Writer
*****************

The BLOB Flash Writer interacts with the BLOB Transfer Server to write BLOB data directly to flash.
It must be initialized by calling :c:func:`bt_mesh_blob_flash_rd_init` before being passed to the
BLOB Transfer Server. Each BLOB Flash Writer only supports one transfer at the time, and requires a
block size that is a multiple of the flash page size. If a transfer is started with a block size
lower than the flash page size, the transfer will be rejected.

The BLOB Flash Writer copies block data into a buffer to accommodate blocks that are aligned with
the flash write block size. The buffer data is padded with ``0xff`` if either the start or length of
the chunk is unaligned.

API Reference
*************

.. doxygengroup:: bt_mesh_blob_io_flash
   :project: wm-iot-sdk-apis
