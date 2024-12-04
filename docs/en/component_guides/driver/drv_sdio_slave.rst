.. _drv_sdio_slave:

SDIO Slave
=====================

Introduction
------------------

The SDIO Slave module provides functionality for communicating with a host as an SDIO slave device.

- **SDIO Slave:** An interface for embedded systems that allows devices to communicate with a host via the Secure Digital Input Output (SDIO) interface as a slave device
- **Zero Copy:** Data transfer uses zero copy method to reduce memory copy overhead and improve transfer efficiency

Feature List
------------------

- Supports Function 0 (configuration and control) and Function 1 (data transfer)
- Supports CIS register read/write
- Supports synchronous and asynchronous data transfer
- Supports command reception
- Supports event callback mechanism

Feature Overview
--------------------------

SDIO Slave main features include:

- **CIS Register Access:**
  - Supports reading and writing CIS registers for Function 0 and Function 1
  - Each Function supports up to 128 bytes of CIS data

- **Data Transfer:**
  - Synchronous transfer: Blocks waiting for transfer completion
  - Asynchronous transfer: Returns immediately, notifies completion via callback
  - Maximum transfer buffer is 8192 bytes (TX) and 4096 bytes (RX)
  - Uses zero copy method, application layer must ensure data remains valid until transfer completes

- **Command Processing:**
  - Supports receiving commands from host
  - Command buffer size is 256 bytes

- **Event Callbacks:**
  - Supports registering callback functions for data send/receive completion events
  - Supports command send/receive completion event notifications

Hardware Configuration
--------------------------

Configure the following parameters via Device Tree (DT):

- Pin multiplexing configuration
- Interrupt configuration

Main Functions
--------------------------

CIS Register Configuration
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

**Prerequisites:**

- SDIO Slave device is initialized

**Related APIs:**

- Call ``wm_drv_sdio_slave_read_cis_reg`` to read CIS register
- Call ``wm_drv_sdio_slave_write_cis_reg`` to write CIS register

**Parameters:**

- func: Function number (0-1)
- pos: Register address (0-123)
- val: Register value (32-bit)

**Results:**

- Returns register value on successful read
- Returns ``WM_ERR_SUCCESS`` on successful write
- Returns corresponding error code on failure

Notes
-------------

- Each Function supports up to 128 bytes of CIS data
- CIS registers should be configured based on SDIO card usage, refer to ``cis.c`` file under :ref:`examples/peripheral/sdio_slave<peripheral_example>`
- CIS register configuration should be completed during initialization, modification during runtime is not recommended
- CIS data contains important parameters like manufacturer info, function description, maximum clock frequency
- Incorrect CIS configuration may cause host to fail recognizing slave device

Data Transfer
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

**Prerequisites:**

- SDIO Slave device is initialized
- Event callback function is registered

**Related APIs:**

- Call ``wm_drv_sdio_slave_init`` to initialize device
- Call ``wm_drv_sdio_slave_tx_data_sync`` for synchronous data sending
- Call ``wm_drv_sdio_slave_tx_data_async`` for asynchronous data sending
- Call ``wm_drv_sdio_slave_rx_data`` to receive data

**Results:**

- Returns ``WM_ERR_SUCCESS`` on success
- Returns corresponding error code on failure

Command Processing
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

**Prerequisites:**

- SDIO Slave device is initialized
- Event callback function is registered

**Related APIs:**

- Call ``wm_drv_sdio_slave_rx_cmd`` to receive command

**Results:**

- Returns ``WM_ERR_SUCCESS`` and notifies via callback on success
- Returns corresponding error code on failure

Notes
-------------

- For data transfer, buffer must be allocated using ``wm_heap_caps_alloc()`` with ``WM_HEAP_CAP_SHARED`` flag
- Maximum send data length is 8192 bytes
- Maximum receive data length is 4096 bytes
- Command receive buffer size is 256 bytes
- Callback functions execute in interrupt context, should be kept brief
- Due to zero copy data transfer, application layer must ensure:

  - Send data is not freed or modified before transfer completes
  - Received data is processed promptly

Application Example
------------------------------------

    For basic SDIO Slave usage example, refer to :ref:`examples/peripheral/sdio_slave<peripheral_example>`

API Reference
--------------------------
:ref:`API Reference <sdio_slave_api>`
