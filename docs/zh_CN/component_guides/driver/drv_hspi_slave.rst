.. _drv_hspi_slave:

HSPI Slave
=============

简介
-------------

    HSPI Slave 模块提供了 W80x 作为 HSPI 从设备与主机进行通信的功能。

    - **HSPI Slave:** 是一种用于嵌入式系统的接口，它允许 W80x 作为从设备通过 HSPI 接口与主机进行通信
    - **Zero Copy:** 数据传输采用 zero copy 方式，减少内存拷贝开销，提高传输效率；
                     发送数据所需缓存由用户去申请，接收数据或命令的缓存已经由驱动申请好。

功能列表
-------------

    - 支持同步和异步数据发送功能
    - 支持异步数据接收功能
    - 支持异步命令接收功能
        - 同步传输: 阻塞等待传输完成
        - 异步传输: 立即返回，通过回调通知完成

硬件配置
-------------

通过设备树(DT)配置以下参数:

- 引脚复用配置
- 中断配置

主要功能
-------------

异步接收数据或命令
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

    调用 ``wm_drv_hspi_slave_rx_cmd`` 接收命令，调用 ``wm_drv_hspi_slave_rx_data`` 接收数据，示例：

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


    hspi_slave_callback 和 user_func 是需要用户实现的函数，在回调函数 hspi_slave_callback 中或者通过信号量等方式在回调函数之外，调用 wm_drv_hspi_slave_rx_data 将数据接收到 rx_buf，
    调用 wm_drv_hspi_slave_rx_cmd 将数据接收到 cmd_buf。

    .. note::

        接收数据或命令的缓存已经由驱动申请好，由于采用 zero copy 方式传输数据，应用层需要确保接收到的数据或命令需要及时处理，否则可能被新接收的内容覆盖

同步发送
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

    调用 ``wm_drv_hspi_slave_tx_data_sync`` 同步发送，示例：

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


    user_func 是需要用户实现的函数

    .. note::

        发送数据所需缓存由用户去申请，并设置 ``WM_HEAP_CAP_SHARED`` 标志，由于采用 zero copy 方式传输数据，应用层需要确保发送数据在传输完成前不能释放或修改。


异步发送
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

    调用 ``wm_drv_hspi_slave_tx_data_async`` 异步发送，示例：

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


    hspi_slave_callback 和 user_func 是需要用户实现的函数，在回调函数 hspi_slave_callback 中通过信号量等方式方式通知发送完成。

    .. note::

        发送数据所需缓存由用户去申请，并设置 ``WM_HEAP_CAP_SHARED`` 标志，由于采用 zero copy 方式传输数据，应用层需要确保发送数据在传输完成前不能释放或修改。

注意事项
-------------

    - 传输数据或命令时，缓冲区必须使用 ``wm_heap_caps_alloc()`` 分配，并设置 ``WM_HEAP_CAP_SHARED`` 标志；其中
      发送数据所需缓存由用户去申请，接收数据或命令的缓存已经由驱动申请好。
    - 发送数据最大长度为 8192 字节
    - 接收数据最大长度为 4096 字节
    - 命令接收缓冲区大小为 256 字节
    - 回调函数在中断上下文执行，应尽量简短
    - 由于采用 zero copy 方式传输数据,应用层需要确保:

      - 发送数据在传输完成前不能释放或修改
      - 接收到的数据或命令需要及时处理，否则可能被新接收的内容覆盖

应用实例
------------------

    使用 HSPI Slave 基本示例请参照 :ref:`examples/peripheral/hspi_slave<peripheral_example>`

API参考
-------------
:ref:`API参考 <hspi_slave_api>`
