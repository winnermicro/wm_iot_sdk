.. _dma:

DMA
=============

Introduction
--------------

DMA (Direct Memory Access) is a method of data transfer that facilitates high-speed data transmission between peripherals and memory or between different memory locations. It allows data to be moved quickly without the intervention of the CPU. The role of DMA is to achieve direct data transfer, eliminating the need for CPU register involvement in the traditional data transfer process.

Function List
--------------

  1. :ref:`Initialize DMA <dma_init>` — Initialize the DMA device.
  2. :ref:`Request DMA Channel <dma_request_ch>` — Request a DMA channel.
  3. :ref:`Release DMA Channel <dma_release_ch>` — Release a DMA channel.
  4. :ref:`Register DMA Callback <dma_register_callback>` — Register a DMA callback function.
  5. :ref:`Unregister DMA Callback <dma_unregister_callback>` — Unregister a DMA callback function.
  6. :ref:`Configure DMA Transfer <dma_config>` — Configure DMA transfer parameters.
  7. :ref:`Start DMA Transfer <dma_start>` — Start a DMA transfer.
  8. :ref:`Stop DMA Transfer <dma_stop>` — Stop a DMA transfer.
  9. :ref:`Get DMA Status <dma_get_status>` — Get the DMA transfer status.
  10. :ref:`DMA Transfer <dma_transfer>` — Configure and start a DMA transfer.

Function Overview
------------------

**Parameter Configuration** Supports configuring parameters such as source address, destination address, transfer length, transfer mode, etc.

**Support for Multiple Transfer Modes** DMA supports various transfer modes such as single transfer and burst transfer to cater to different application needs.
    
**Bus Control** The DMA controller responds to requests from peripheral by sending a bus request signals to the CPU. When the CPU relinquishes control of the bus, the DMA controller takes over. After obtaining bus control, the DMA controller sends address signals to the address bus and issues read/write control signals during DMA operations.

**Transfer Management** The DMA controller determines the number of bytes to be transferred  in the current operation and checks whether the  DMA transfer is complete. After the transfer, the DMA controller returns bus control to the CPU.

**Events** Supports events such as transfer completion and transfer errors, which can be handled by registering callback functions.  


Key Features
-----------------------

.. _dma_init:

Initialize DMA
^^^^^^^^^^^^^^^^^^^^

Call the function ``wm_drv_dma_init()`` to initialize the DMA device. The initialization process includes configuring the hardware parameters of the DMA and initializing necessary data structures to ensure the DMA device operate normally. An example is as follows:

.. code:: c

        wm_device_t *dma_dev;
        dma_dev = wm_drv_dma_init("dma");

Here, ``dma`` is the name of the DMA device, used to identify the device, and needs to be configured in the device table. The type is ``const char *``.

.. warning:: After initializing DMA , if ``wm_drv_dma_deinit`` is not called, invoking ``wm_drv_dma_init`` again will return ``NULL``.

.. _dma_request_ch:

Request DMA Channel
^^^^^^^^^^^^^^^^^^^^

Call the function ``wm_drv_dma_request_ch()`` to request a DMA channel for the specified DMA transfer source and destination addresses and  related configurations. An example is as follows:

.. code:: c

        wm_device_t *dma_dev;
        dma_dev = wm_drv_dma_init("dma");

        uint8_t dma_ch = 0;
        wm_drv_dma_request_ch(dma_dev, &ch, DRV_DMA_M2M_EXAMPLE_TO_US);

The first parameter is the reference to the DMA device, of type ``wm_device_t *``.
    
The second parameter is used to store the allocated DMA channel number, of type ``uint8_t *``.
    
The third parameter is the timeout period for waiting for channel allocation in milliseconds, set here to ``DRV_DMA_M2M_EXAMPLE_TO_US`` with a value of ``0xffffffffUL``, of type ``uint32_t``.

.. _dma_release_ch:

Release DMA Channel
^^^^^^^^^^^^^^^^^^^^

Call the function ``wm_drv_dma_release_ch()`` to release a DMA channel, used to abort DMA transfers and release allocated DMA resources. An example is as follows:

.. code:: c

        wm_device_t *dma_dev;
        dma_dev = wm_drv_dma_init("dma");

        uint8_t dma_ch = 0;
        wm_drv_dma_release_ch(dma_dev, ch, DRV_DMA_M2M_EXAMPLE_TO_US);

The first parameter is the reference to the DMA device, of type ``wm_device_t *``.
    
The second parameter is the DMA channel number to be released, of type ``uint8_t *``.
    
The third parameter is the timeout period for waiting to release the channel, in milliseconds, set here as ``DRV_DMA_M2M_EXAMPLE_TO_US``, which has a value of ``0xffffffffUL``, of type ``uint32_t``.

.. _dma_register_callback:

Register DMA Completion Callback
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Call the function ``wm_drv_dma_register_callback()`` to register a DMA completion callback function, which is called when the DMA transfer is completed, to handle operations after the DMA transfer ends. An example is as follows:

.. code:: c

    wm_device_t *dma_dev;
    dma_dev = wm_drv_dma_init("dma");

    uint8_t dma_ch = 0;
    void dma_callback(wm_dma_ch_t ch, uint32_t sts, void *user_data) {
    }
    wm_drv_dma_register_callback(dma_dev, dma_ch, dma_callback, NULL);

The first parameter is the reference to the DMA device, of type ``wm_device_t *``.
    
The second parameter is the DMA channel number, of type ``uint8_t *``.
    
The third parameter is the callback function to be called upon DMA transfer completion for handling subsequent operations, of type ``wm_drv_dma_ch_callback_t``.
    
The fourth parameter is a user data pointer, which can pass additional information. In this example, no information is passed, of type ``void *``.

.. _dma_unregister_callback:

Unregister DMA Completion Callback
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Call the function ``wm_drv_dma_unregister_callback()`` to unregister the DMA completion callback function. An example is as follows:

.. code:: c

      wm_device_t *dma_dev;
      dma_dev = wm_drv_dma_init("dma");

      uint8_t dma_ch = 0;

      wm_drv_dma_unregister_callback(dma_dev, dma_ch);

The first parameter is the reference to the DMA device, of type ``wm_device_t *``.
    
The second parameter is the DMA channel number, of type ``uint8_t *``.

.. _dma_config:

Configure DMA Transfer
^^^^^^^^^^^^^^^^^^^^^^^^^^

Call the function ``wm_drv_dma_config()`` to configure the DMA transfer parameters. An example is as follows:

.. code:: c

        wm_device_t *dma_dev;
        dma_dev = wm_drv_dma_init("dma");

        uint8_t dma_ch = 0;
        wm_drv_dma_desc_t dma_example_cfg = {
        .src  = (uint32_t)&dma_src_data[0][0],
        .dest = (uint32_t)&dma_dest_buf[0][0],
        };
        dma_example_cfg.ctrl.len           = DRV_DMA_M2M_EXAMPLE_XFER_LEN;
        dma_example_cfg.ctrl.ch            = dma_ch;
        dma_example_cfg.ctrl.dir           = WM_DRV_DMA_DIR_M2M;
        dma_example_cfg.ctrl.burst_size    = WM_DRV_DMA_BURST_BYTE;
        dma_example_cfg.ctrl.src_inc_mode  = WM_DRV_DMA_ADDR_INC;
        dma_example_cfg.ctrl.dest_inc_mode = WM_DRV_DMA_ADDR_INC;
        dma_example_cfg.ctrl.int_en        = WM_DRV_DMA_CH_INT_ENABLE;
        dma_example_cfg.ctrl.auto_reload   = WM_DRV_DMA_RELOAD_DISABLE;
        dma_example_cfg.ctrl.dma_mode      = WM_DRV_DMA_NORMAL_MODE;

        wm_drv_dma_config(dma_dev, &dma_example_cfg);

The first parameter is the reference to the DMA device, of type ``wm_device_t *``.
    
The second parameter is the transfer descriptor setting the source and destination addresses, where ``src`` is the starting address of the source data pointing to the start of the dma_src_data array, and ``dest`` is the starting address of the destination buffer pointing to the start of the dma_dest_buf array. The type is ``wm_drv_dma_desc_t *``.
    
In this example, parameters such as transfer byte length, DMA channel number, data transfer direction, burst transfer size, source address increment mode, destination address increment mode, interrupt enable, auto-reload, and DMA mode are configured. Specific values can be referenced in the device table.

.. note::
   If you want to use the loop mode, i.e., dma_mode is WM_DRV_DMA_WARP_MODE, pay attention to the following details:
      1.Ensure that the transfer length and the loop source and destination data lengths are aligned to four bytes.
      2.The transfer length should be at least four bytes greater than the destination loop length, i.e., drv_desc->ctrl.len should be at least four bytes greater than drv_desc->ctrl.dest_warp_len.


.. _dma_start:

Start DMA Transfer
^^^^^^^^^^^^^^^^^^^^

Call the function ``wm_drv_dma_start()`` to start the DMA transfer. The DMA controller will begin data transfer based to the configured transfer  parameters. An example is as follows:

.. code:: c
        
        wm_device_t *dma_dev;
        dma_dev = wm_drv_dma_init("dma");

        uint8_t dma_ch = 0;
        
        wm_drv_dma_start(dma_dev, dma_ch);

The first parameter is the reference to the DMA device, of type ``wm_device_t *``.
    
The second parameter is the DMA channel number to start the transfer, of type ``uint8_t *``.

.. _dma_stop:

Stop DMA Transfer
^^^^^^^^^^^^^^^^^^^^

Call the function ``wm_drv_dma_stop()`` to stop the DMA transfer. The DMA controller will cease the current transfer operation. An example is as follows:

    .. code:: c

        wm_device_t *dma_dev;
        dma_dev = wm_drv_dma_init("dma");

        uint8_t dma_ch = 0;

        wm_drv_dma_stop(dma_dev, dma_ch);

The first parameter is the reference to the DMA device, of type ``wm_device_t *``.

The second parameter is the DMA channel number to stop the transfer, of type ``uint8_t *``.

.. _dma_get_status:

Get DMA Status
^^^^^^^^^^^^^^^^^^^^

Call the function ``wm_drv_dma_get_status()`` to obtain the DMA transfer status. An example is as follows:

    .. code:: c

        wm_device_t *dma_dev;
        dma_dev = wm_drv_dma_init("dma");

        uint8_t dma_ch = 0;
        
        wm_dma_sts_info_t dma_sts = {
        .sts      = WM_DMA_RUNNING,
        .xfer_cnt = 0,
        };

        wm_drv_dma_get_status(dma_dev, ch, &dma_sts);
    
The first parameter is the reference to the DMA device, of type ``wm_device_t *``.

The second parameter is the DMA channel number to query the status, of type ``uint8_t *``.

The third parameter is a pointer to the ``wm_dma_sts_info_t`` structure, used to store the DMA status information.

In this example, the current status of the DMA is set during initialization, and the count of transferred data is set to zero.
    
.. _dma_transfer:

DMA Transfer
^^^^^^^^^^^^^^^^^^^^

Call the function ``wm_drv_dma_transfer()`` to configure and start the DMA transfer. In actual applications, you can adjust various parameters in ``dma_example_cfg`` to meet different transfer requirements according to specific needs. An example is as follows:

.. code:: c

        wm_device_t *dma_dev;
        dma_dev = wm_drv_dma_init("dma");

        uint8_t dma_ch = 0;
        wm_drv_dma_desc_t dma_example_cfg = {
        .src  = (uint32_t)&dma_src_data[0][0],
        .dest = (uint32_t)&dma_dest_buf[0][0],
        };
        dma_example_cfg.ctrl.len           = DRV_DMA_M2M_EXAMPLE_XFER_LEN;
        dma_example_cfg.ctrl.ch            = dma_ch;
        dma_example_cfg.ctrl.dir           = WM_DRV_DMA_DIR_M2M;
        dma_example_cfg.ctrl.burst_size    = WM_DRV_DMA_BURST_BYTE;
        dma_example_cfg.ctrl.src_inc_mode  = WM_DRV_DMA_ADDR_INC;
        dma_example_cfg.ctrl.dest_inc_mode = WM_DRV_DMA_ADDR_INC;
        dma_example_cfg.ctrl.int_en        = WM_DRV_DMA_CH_INT_ENABLE;
        dma_example_cfg.ctrl.auto_reload   = WM_DRV_DMA_RELOAD_DISABLE;
        dma_example_cfg.ctrl.dma_mode      = WM_DRV_DMA_NORMAL_MODE;

        wm_drv_dma_transfer(dma_dev, &dma_example_cfg);

The first parameter is the reference to the DMA device, of type ``wm_device_t *``.
    
The second parameter is the transfer descriptor setting the source and destination addresses, where ``src`` is the starting address of the source data pointing to the start of the ``dma_src_data`` array, and ``dest`` is the starting address of the destination buffer pointing to the start of the ``dma_dest_buf`` array. The type is ``wm_drv_dma_desc_t *``.
    
In this example, the transfer byte length, DMA channel number, data transfer direction, burst transfer size, source address increment mode, destination address increment mode, interrupt enable, auto-reload, and DMA mode are configured. Specific values can be referenced from the device table.

Application Example
-----------------------------

For basic examples of using DMA, please refer to: examples/peripheral/dma

API Reference
----------------
:ref:`label_api_dma` 