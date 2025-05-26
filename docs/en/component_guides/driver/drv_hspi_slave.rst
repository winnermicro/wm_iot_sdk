.. _drv_hspi_slave:

HSPI Slave
=============

Introduction
-------------

    The HSPI Slave module provides functionality for the W80x to communicate as an HSPI slave device with a host.

    - **HSPI Slave:** An interface for embedded systems that allows the W80x to communicate as a slave device with a host via the HSPI interface.
    - **Zero Copy:** Data transmission uses a zero-copy method to reduce memory copy overhead and improve efficiency.
                     The buffer for sending data must be allocated by the user, while the buffers for receiving data or commands are pre-allocated by the driver.

Feature List
-------------

    - Supports synchronous and asynchronous data transmission.
    - Supports asynchronous data reception.
    - Supports asynchronous command reception.
        - Synchronous transmission: Blocks until transmission is complete.
        - Asynchronous transmission: Returns immediately and notifies completion via callback.

Hardware Configuration
--------------------------

    Configure the following parameters via Device Tree (DT):

    - Pin multiplexing configuration
    - Interrupt configuration

Main Functions
---------------

Asynchronous Reception of Data or Commands
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

    Call ``wm_drv_hspi_slave_rx_cmd`` to receive commands and ``wm_drv_hspi_slave_rx_data`` to receive data. Example:

    .. code:: c

        void hspi_slave_callback(wm_device_t *dev, wm_drv_hspi_slave_event_t event, void *addr, void *priv)
        {
            BaseType_t xHigherPriorityTaskWoken = pdFALSE;

            /* Set corresponding event bits based on event type */
            if (event == WM_DRV_HSPI_SLAVE_EVENT_RX_CMD) {
                void *cmd_buf = NULL;
                wm_drv_hspi_slave_rx_cmd(dev, &cmd_buf, 0);
                /* process cmd_buf */
                // user code
            } else if (event == WM_DRV_HSPI_SLAVE_EVENT_RX) {
                void *rx_buf = NULL;
                wm_drv_hspi_slave_rx_data(dev, &rx_buf, 0);
                /* process rx_buf */
                // user code
            } else if (event == WM_DRV_HSPI_SLAVE_EVENT_TX) {

            }

            /* Yield if higher priority task is woken */
            portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
        }

        void user_func(void)
        {
            wm_device_t *dev;
            wm_drv_hspi_slave_cfg_t drv_cfg;

            /* Initialize HSPI slave device */
            dev = wm_drv_hspi_slave_init("hspi_slave");

            /* config HSPI interface */
            memset(&drv_cfg, 0, sizeof(drv_cfg));
            drv_cfg.cpol = WM_HSPI_CPOL_LOW;
            drv_cfg.cpha = WM_HSPI_CPHA_FIRST_EDGE;
            wm_drv_hspi_slave_config(dev, &drv_cfg);

            /* Register callback if initialization successful */
            ret = wm_drv_hspi_slave_register_callback(dev, (wm_drv_hspi_slave_callback_t)hspi_slave_callback, NULL);
        }


    The functions `hspi_slave_callback` and `user_func` must be implemented by the user.
    In the callback function `hspi_slave_callback` or via semaphores outside the callback,
    call `wm_drv_hspi_slave_rx_data` to receive data into `rx_buf` and `wm_drv_hspi_slave_rx_cmd` to receive commands into `cmd_buf`.

    .. note::

        The buffers for receiving data or commands are pre-allocated by the driver.Due to the zero-copy transmission method, the application layer must ensure that received data or commands are processed promptly, or they may be overwritten by newly received content.

Synchronous Transmission
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

    Call ``wm_drv_hspi_slave_tx_data_sync`` for synchronous transmission. Example:

    .. code:: c

        void user_func(void)
        {
            wm_device_t *dev;
            wm_drv_hspi_slave_cfg_t drv_cfg;

            /* Initialize HSPI slave device */
            dev = wm_drv_hspi_slave_init("hspi_slave");

            /* config HSPI interface */
            memset(&drv_cfg, 0, sizeof(drv_cfg));
            drv_cfg.cpol = WM_HSPI_CPOL_LOW;
            drv_cfg.cpha = WM_HSPI_CPHA_FIRST_EDGE;
            wm_drv_hspi_slave_config(dev, &drv_cfg);

            /* Allocate buffer for test data */
            void *tx_buf = wm_heap_caps_alloc(WM_HSPI_SLAVE_TX_DATA_SIZE, WM_HEAP_CAP_SHARED);

            /* Fill buffer with test pattern */
            // user code

            /* Send data synchronously with 1s timeout */
            ret = wm_drv_hspi_slave_tx_data_sync(dev, tx_buf, WM_HSPI_SLAVE_TX_DATA_SIZE, 1000);

            wm_heap_caps_free(tx_buf);
        }


    The function `user_func` should be implemented by the user.

    .. note::

        The buffer for sending data must be allocated by the user and marked with the ``WM_HEAP_CAP_SHARED`` flag. Due to the zero-copy transmission method, the application layer must ensure that the data being sent is not released or modified until the transmission is complete.

Asynchronous Transmission
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

    Call ``wm_drv_hspi_slave_tx_data_async`` for asynchronous transmission. Example:

    .. code:: c

        /* Event group handle for asynchronizing HSPI events */
        static EventGroupHandle_t event_group;
        void hspi_slave_callback(wm_device_t *dev, wm_drv_hspi_slave_event_t event, void *addr, void *priv)
        {
            BaseType_t xHigherPriorityTaskWoken = pdFALSE;

            /* Set corresponding event bits based on event type */
            if (event == WM_DRV_HSPI_SLAVE_EVENT_RX_CMD) {
            } else if (event == WM_DRV_HSPI_SLAVE_EVENT_RX) {
            } else if (event == WM_DRV_HSPI_SLAVE_EVENT_TX) {
                xEventGroupSetBitsFromISR(event_group, EVENT_TX_BIT, &xHigherPriorityTaskWoken);
            }

            /* Yield if higher priority task is woken */
            portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
        }

        void user_func(void)
        {
            wm_device_t *dev;
            wm_drv_hspi_slave_cfg_t drv_cfg;

            /* Initialize HSPI slave device */
            dev = wm_drv_hspi_slave_init("hspi_slave");

            /* config HSPI interface */
            memset(&drv_cfg, 0, sizeof(drv_cfg));
            drv_cfg.cpol = WM_HSPI_CPOL_LOW;
            drv_cfg.cpha = WM_HSPI_CPHA_FIRST_EDGE;
            wm_drv_hspi_slave_config(dev, &drv_cfg);

            /* Allocate buffer for test data */
            void *tx_buf = wm_heap_caps_alloc(WM_HSPI_SLAVE_TX_DATA_SIZE, WM_HEAP_CAP_SHARED);

            /* Fill buffer with test pattern */
            // user code

            /* Send data asynchronously */
            ret = wm_drv_hspi_slave_tx_data_async(dev, tx_buf, WM_HSPI_SLAVE_TX_DATA_SIZE);

            /* Wait for transmission complete event */
            bits = xEventGroupWaitBits(event_group, EVENT_TX_BIT, pdTRUE, pdTRUE, portMAX_DELAY);
            if (bits & EVENT_TX_BIT) {
                wm_heap_caps_free(tx_buf);
                tx_buf = NULL;
            }
        }


    The functions `hspi_slave_callback` and `user_func` must be implemented by the user.
    In the callback function `hspi_slave_callback`, use semaphores or other methods to notify the completion of transmission.

    .. note::

        The buffer for sending data must be allocated by the user and marked with the ``WM_HEAP_CAP_SHARED`` flag. Due to the zero-copy transmission method, the application layer must ensure that the data being sent is not released or modified until the transmission is complete.

Notes
-------------

    - When transmitting data or commands, buffers must be allocated using ``wm_heap_caps_alloc()`` and marked with the ``WM_HEAP_CAP_SHARED`` flag.
      The buffer for sending data must be allocated by the user, while the buffers for receiving data or commands are pre-allocated by the driver.
    - Maximum length for sending data: 8192 bytes.
    - Maximum length for receiving data: 4096 bytes.
    - Command reception buffer size: 256 bytes.
    - The callback function executes in an interrupt context and should be kept as brief as possible.
    - Due to the zero-copy transmission method, the application layer must ensure:
      - Data being sent is not released or modified until the transmission is complete.
      - Received data or commands are processed promptly, or they may be overwritten by newly received content.

Application Example
------------------------------------

    For basic HSPI Slave usage example, refer to :ref:`examples/peripheral/hspi_slave<peripheral_example>`

API Reference
--------------------------
:ref:`API Reference <hspi_slave_api>`
