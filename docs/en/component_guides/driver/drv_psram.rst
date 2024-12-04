.. _drv_psram:

PSRAM
========================

Introduction
------------------------

PSRAM (Pseudo-Static Random Access Memory) is a storage technology that combines the advantages of Dynamic RAM (DRAM) and Static RAM (SRAM). The W800 is equipped with a PSRAM controller that supports both SPI and QSPI interfaces, allowing access to external PSRAM devices. It provides bus operations to read, write, and erase PSRAM, with a maximum read/write speed of up to 80MHz.

Feature List
------------------------

1. :ref:`Initialization <psram_init>` — Initialize the PSRAM driver.
2. :ref:`Set Callback Function <psram_callback>` — Configure actions to be executed after data read/write completion.
3. :ref:`IO Control <psram_ioctrl>` — Set the PSRAM operating mode.
4. :ref:`Get PSRAM Size <psram_getsize>` — Retrieve the total capacity of the PSRAM.
5. :ref:`Memory Copy <psram_memcpy>` — Copy data from PSRAM memory to a specified address.

Function Overview
------------------------

 **Operating Modes** The built-in PSRAM of the W800 supports three operating modes, which are described as follows:

 ===================== ========================================================================================================================================================
    Operating Mode         Description
 ===================== ========================================================================================================================================================
 ``SPI Mode``           Features four signal pins: Chip Select (CS), Clock (CLK), Data Input (SI), and Data Output (SO) for standard serial data transmission.
 
 ``QSPI Mode``          Builds upon traditional SPI by adding two additional IO data lines (totaling four data lines: IO0, IO1, IO2, IO3), allowing simultaneous
                        
			            transmission of four bits of data for faster transfer rates suitable for high-performance data transmission needs.
 
 ``Half Sleep Mode``    A low-power mode where only reading is allowed, with data already stored in the PSRAM retained. 
 ===================== ========================================================================================================================================================

 **Transfer Speed**  The PSRAM supports a maximum transfer rate of 80MHz

 **Capacity Configuration** The capacity of PSRAM can be configured to auto, 2M, 4M, or 8M,up to a maximum of 8MB.

 **Memory Management** Supports allocating pSRAM to the heap using the functions ``wm_heap_caps_alloc`` ， ``wm_heap_caps_realloc`` ， ``wm_heap_caps_free``.

  **Supports DMA**  for memory addresses that need to be copied, requiring 4-byte alignment and using DMA for copies exceeding 1K to reduce CPU load.

PSRAM Configuration
------------------------

Setting PSRAM Capacity
^^^^^^^^^^^^^^^^^^^^^^^^

 Before using PSRAM, set ``CONFIG_COMPONENT_DRIVER_PSRAM_ENABLED`` to ``y`` in the project's ``prj.config`` file.
 Alternatively, enable PSRAM and select the desired capacity size in the ``heap`` settings of ``menuconfig`` in the build environment.
 PSRAM memory can be allocated using the ``wm_heap_caps_alloc()`` function and deallocated using the ``wm_heap_caps_realloc()`` function.

Data Transfer Pin Configuration
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

 PSRAM pins are pre-configured and currently configured in the device table.

Main Functions
------------------------

.. _psram_init:

Initialization
^^^^^^^^^^^^^^^^^^^^^^^^

 Before using PSRAM, ``wm_drv_psram_init()`` is called during wm_heap initialization. To use it, simply call ``wm_dt_get_device_by_name`` to obtain the ``wm_device_t`` device. Example:

    .. code:: c

        wm_device_t *psram_dev;
        psram_dev = wm_dt_get_device_by_name("psram0");

  Currently, the W800 supports only one PSRAM, so the first parameter must be ``psram0``.

.. _psram_callback:

Set Callback Function
^^^^^^^^^^^^^^^^^^^^^^^^

After data transfer is complete, a callback function is called. To execute certain operations, use the ``wm_drv_psram_register_callback()`` function. Example:

 .. code:: c

        wm_drv_psram_callback_t psram_callback_example(void){
            wm_log_info("this is psram callback func");
        }

        wm_device_t *psram_dev;
        psram_dev = wm_dt_get_device_by_name("psram0");

        wm_drv_psram_register_callback(psram_dev, psram_callback_example, NULL);

The first parameter is the PSRAM device identifier.

The second parameter is the callback function to be executed after the transfer is complete.

The third parameter is the argument to be passed to the callback function.

.. _psram_ioctrl:

IO Control
^^^^^^^^^^^^^^^^^^^^^^^^

When using PSRAM, configure the IO operating mode using the ``wm_drv_psram_ioctrl()`` function. This function requires four parameters: ``device``, ``cmd``, ``param``, and ``*arg``. Different combinations of ``cmd`` and ``param`` correspond to different working modes:

    ===============================       ===============================           ===============================    
    cmd                                    param                                     Operating Mode
    ===============================       ===============================           ===============================
    WM_DRV_PSRAM_CMD_SET_MODE              WM_DRV_PSRAM_MODE_SPI                    SPI Mode
    WM_DRV_PSRAM_CMD_SET_MODE              WM_DRV_PSRAM_MODE_QSPI                   QSPI Mode
    WM_DRV_PSRAM_CMD_ENABLE_HSM            NULL                                     Half Sleep Mode
    WM_DRV_PSRAM_CMD_DISABLE_HSM           NULL                                     Standard Mode
    ===============================       ===============================           ===============================  

After setting Half Sleep Mode, PSRAM will enter a low-power state and cannot perform read/write operations.

Example of IO control use:

 .. code:: c

      wm_device_t *psram_dev;
      psram_dev = wm_dt_get_device_by_name("psram0");

      wm_drv_psram_ioctrl(psram_dev, WM_DRV_PSRAM_CMD_SET_MODE, WM_DRV_PSRAM_MODE_SPI, NULL);

.. _psram_getsize:

Get PSRAM Size
^^^^^^^^^^^^^^^^^^^^^^^^

To get the size of the PSRAM, call the ``wm_drv_psram_get_size()`` function. Example:

  .. code:: c

        uint32_t size;
        wm_device_t *psram_dev;
        psram_dev = wm_dt_get_device_by_name("psram0");

        wm_drv_psram_get_size(psram_dev, &size);

The value of ``size`` is the size configured for PSRAM .

.. _psram_memcpy:

Memory Copy
^^^^^^^^^^^^^^^^^^^^^^^^

To transfer data from PSRAM, call the ``wm_drv_psram_memcpy_dma()`` function, requiring parameters ``device`` , requiring parameters ``dst`` (destination address), ``src`` (source address) , and  ``size`` (data size). Example:

   .. code:: c

        #define PSRAM_COPY_SIZE 20
        #define PSRAM_DMA_SIZE  1024

        wm_device_t *psram_dev;
        char *p;
        const char *str = "0123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789"
                        "0123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789"
                        "0123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789"
                        "0123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789"
                        "0123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789"
                        "0123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789"
                        "0123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789"
                        "0123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789"
                        "0123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789"
                        "0123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789"
                        "abcdefghijklmnopqrstuvwx";
        psram_dev = wm_dt_get_device_by_name(PSRAM_DEVICE_NAME);
        p = wm_heap_caps_alloc(PSRAM_DMA_SIZE, WM_HEAP_CAP_SPIRAM);

        if (p) {
            wm_drv_psram_memcpy_dma(psram_dev, p, str, PSRAM_DMA_SIZE);
            wm_heap_caps_free(p);
        } else {
            wm_log_error("alloc from psram fail.");
        }

    **There are three conditions for using DMA copy:**

    - Source and destination addresses must be 4-byte aligned.
    - Copy length must not be less than PSRAM_MIN_DMA_SIZE, defaulting is 1024.
    - The device table configures the PSRAM device to use DMA. When calling the API, an idle DMA transfer channel  is requested.

De-initialization
^^^^^^^^^^^^^^^^^^^^^^^^

If PSRAM is no longer needed for data storage, call``wm_drv_psram_deinit()`` to  remove the driver and release allocated resources.

Ways to Use PSRAM Memory
-------------------------------------

 - Global Variable Definition
          
        Variables defined in this way are always occupied and are not default-initialized upon system startup. Initialization is required before use.

  .. code:: c

            ATTRIBUTE_PSRAM static uint8_t psram_buf[64];
	    
 - Using  ``wm_heap_caps_alloc``and``wm_heap_caps_free``
      
        Allocate when needed and free after use.
 
.. code:: c

            uint8_t* psram_buf = wm_heap_caps_alloc(64,WM_HEAP_CAP_SPIRAM);
            if(psram_buf){
                //Here you can use  psram_buf
                wm_heap_caps_free(psram_buf);
            }


Precautions
------------------------

.. note::

    - The global variables defined with the ATTRIBUTE_PSRAM modifier will be placed in the PSRAM, but they will not be initialized by default.
    - The code cannot be executed in the PSRAM.

.. warning::

    - Both the DMA and the CPU access the PSRAM through the SPI pins. Therefore, they cannot access the PSRAM simultaneously. Simultaneous access will cause the system to crash abnormally.

Application Example
------------------------

For a basic example of using PSRAM, please refer to: :ref:`examples/peripheral/psram<peripheral_example>` 

API Reference
------------------------

    To find PSRAM-related APIs, please refer to:

    :ref:`label_api_psram`

