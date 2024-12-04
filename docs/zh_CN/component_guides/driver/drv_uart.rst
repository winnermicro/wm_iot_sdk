
.. _uart:

UART
=============

简介
-------------

    UART(Universal Asynchronous Receiver/Transmitter) 是一种用于串行通信的硬件模块。它是计算机、嵌入式系统、微控制器和其他设备之间进行数据传输的常用接口。UART 允许两台设备通过串行端口以一定的波特率进行数据交换，是实现不同设备之间全双工或半双工数据交换的一种常用且经济的方式。

    W800 芯片支持6个 UART 口（32pin封装的只支持5个 UART 口）。每个 UART 控制器可以独立配置波特率、数据位长度、位顺序、停止位位数、奇偶校验位等参数。可以和不同制造商的 UART 设备兼容。

    UART 口支持的最大波特率为 2M。


功能列表
-------------

    1. :ref:`初始化 <uart_init>`——初始化驱动。
    2. :ref:`设置通信参数 <uart_set>`——设置波特率、奇偶校验，硬件流控、停止位。
    3. :ref:`数据 Write <uart_write>`——写数据到串口驱动的 buffer 中或者直接从串口发送出去。
    4. :ref:`数据 Read <uart_read>`——从串口 buffer 中读数据。
    5. :ref:`数据 Receive <uart_receive>`——从串口驱动中获取数据块的指针和数据大小。
    6. :ref:`硬件流控制 <uart_flowCtrl_func>`——硬件流控。


功能概述
-------------

    **Port** WM800 最多同时支持6路 Uart（32pin 的芯片支持5路）。

    **参数配置** 支持常用的波特率，停止位，奇偶校验，硬件流控，最大支持 2M 波特率。

    **支持 DMA** Uart 支持 DMA 方式，不过只有一路 Uart 可以使用 DMA。

    **接收缓冲 Buffer** 驱动使用缓冲 buffer 接收数据，上层应用可以从 buffer 中读取，也可以直接获取 buffer 中数据块的指针和大小处理数据。

    **发送缓冲 Buffer** 发送的 TX buffer 可以再初始化设置为0，即不使用 buffer ,也可以指定，设置后可以减少写入接口的阻塞时间。

    **事件** 支持接收，发送事件，先收到事件再去读数据，可以避免 read 接口阻塞。

    **流控** 支持硬件 CTS , RTS 流控。


Uart 硬件连线
-------------

    Uart 主要连线如下：

    .. figure:: ../../../_static/component-guides/driver/uart_flow_ctrl.png
        :align: center
        :alt: 流控连线示意图



.. _uart_set:

设置通信参数
-------------

    UART 通信参数在设备表中进行配置,其默认参数如下表所示。

    =========================          ==========================
    变量                               取值
    =========================          ==========================
    baudrate                           WM_UART_BAUDRATE_B115200
    data bits                          WM_UART_DATA_BIT_8
    stop bits                          WM_UART_STOP_BIT_1
    parity                             WM_UART_PARITY_NONE
    flow_ctrl                          WM_UART_FLOW_CTRL_DISABLE
    =========================          ==========================


    UART 通信参数也可以动态配置，调用函数 ``wm_drv_uart_set_config()`` 进行配置。


一次性配置所有参数
^^^^^^^^^^^^^^^^^^^^^

    调用函数 ``wm_drv_uart_set_config()`` 并向其传递 ``wm_drv_uart_config_t`` 结构体， ``wm_drv_uart_config_t`` 结构体应包含所
    有必要的参数。请参考以下示例。

    .. code:: c

        wm_device_t *dev = wm_dt_get_device_by_name("uart2");
        wm_drv_uart_config_t uart_config={
            .baudrate=115200,
            .data_bits=WM_UART_DATA_BIT_8,
            .stop_bits=WM_UART_STOP_BIT_1,
            .parity=WM_UART_PARITY_NONE,
            .flow_ctrl=WM_UART_FLOW_CTRL_DISABLE,
        };
        wm_drv_uart_set_config(dev,&uart_config);

分步依次配置每个参数
^^^^^^^^^^^^^^^^^^^^^^^

    ====================  =================================== ===================================
    查询/配置参数           查询函数                             配置函数
    ====================  =================================== ===================================
    波特率                  ``wm_drv_uart_get_baudrate()``      ``wm_drv_uart_set_baudrate()``
    数据位                  ``wm_drv_uart_get_data_bits()``     ``wm_drv_uart_set_data_bits()``
    停止位                  ``wm_drv_uart_get_stop_bits()``     ``wm_drv_uart_set_stop_bits()``
    奇偶控制                ``wm_drv_uart_get_parity()``        ``wm_drv_uart_set_parity()``
    硬件控制模式            ``wm_drv_uart_get_flow_ctrl()``      ``wm_drv_uart_set_flow_ctrl()``
    ====================  =================================== ===================================

    调用上表中的专用函数，能够单独配置特定参数。如需重新配置某个参数，也可使用这些函数。

    表中每个函数都可使用 ``wm_drv_uart_get_xxx`` 对应项来查看当前设置值。例如，查看当前波特率值，请调用 ``wm_drv_uart_get_baudrate()`` 。


.. _uart_setpin:

通信管脚配置
^^^^^^^^^^^^^

    UART 引脚已默认配置，目前 pin 脚在设备表中进行配置。

DMA 模式配置
^^^^^^^^^^^^^

    UART 支持使用 DMA ,但只有一个 port 可以使用 DMA, 在设备表中配置 uart 时，指定 DMA controler 名称后，就会使用 DMA 传输。

- 中断模式

    在中断方式下，UART 模块硬件 RX fifo 接收到一半，或者 TX fifo 发送快完成时会产生中断请求， CPU 响应中断后执行相应的中断服务程序来处理数据的接收或发送。由于需要使用 CPU 来搬移数据，这种方式会在一定程度上占用 CPU 资源。

- DMA 模式

    DMA（Direct Memory Access）方式允许 UART 模块直接通过 DMA 控制器与内存进行数据交换，而无需 CPU 的直接干预。在数据接收时，DMA 会将 UART 硬件 RX fifo 接收到的数据直接传输到指定的内存地址；在数据发送时，DMA 会将内存中的数据传输到 UART 硬件 TX fifo 缓冲区，由硬件把数据发送出去。这种方式由DMA硬件搬移数据，无需 CPU 参与，可以减轻 CPU 的负担，尤其是在处理大量数据时。


主要功能
-------------

.. _uart_init:

初始化 UART
^^^^^^^^^^^^^

    使用 UART 时前需调用 ``wm_drv_uart_init()`` 函数为 UART 分配资源，利用 ``wm_device_t`` 结构体接收 UART 设备标识符。示例：

    .. code:: c

        wm_device_t *uart_dev;
        uart_dev = wm_drv_uart_init("uart2",1024,0);


    第一个参数指定设备名称，名称在设备表中定义，从 uart0~uart5

    第二个参数是 RX 接收 buffer 大小，其范围是 128 到 65535 ；驱动会把 buffer 分割成4块，轮流接收。这个值的大小需要根据对应 uart 口的应用情况定。一般来说 512~4K 之间， 当接收的数据比较多，处理又可能阻塞时，就需要设置大点。

    第三个参数是 TX buffer, 可以设置 0， 或者 128 到 65535 范围内的值， 设置为0时，驱动直接使用 API 接口的数据 buffer 传输，传输完成后再返回。当不为0时，驱动会把应用需要传输的数据先复制到 buffer 中，等待到能复制完就直接返回，同时启动 DMA 或者 interrupt 方式传输。TX 使用 buffer 会多复制一次，会额外增加 CPU 负担，并占用内存资源，但 buffer 能让 ``wm_drv_uart_write`` 接口快速返回，减少接口调用阻塞时间。

    .. warning:: Uart初始化后，如果没有调用 ``wm_drv_uart_deinit`` , 再次调用 ``wm_drv_uart_init`` 会返回 ``NULL`` 。

.. _uart_write:

发送数据
^^^^^^^^^^^^^

    发送数据准备就绪之后，调用 ``wm_drv_uart_write`` 函数将要发送的数据从串口发送出去。。如果初始化时 TX buffer 为 0 ， 驱动会使用接口中的数据直接发送，全部发送完成后再返回。 如果初始化时 TX buffer 不为 0 ， 调用该接口后会把参数给的数据拷贝到 buffer 中，能拷完就直接返回，不能拷贝完会等待 buffer 变化后继续拷贝，全部拷贝完成后再返回。同时驱动也会同步把缓冲 buffer 中的数据发送出去。


    代码示例如下：

    .. code:: c

        char* test_str = "This is a test string.\n";
        wm_drv_uart_write(uart_dev, (const char*)test_str, strlen(test_str));

.. _uart_read:

Read 方式接收数据
^^^^^^^^^^^^^^^^^^^^

    Read 方式使用接口函数 ``wm_drv_uart_read`` 读取数据，该接口会把驱动缓冲区中的数据拷贝到指定的 buffer 中，如果接收到的数据不够会等待超时，满足下面条件时会返回：

    - 读够接口指定的长度；
    - 等够了指定等待的超时时间；
    - 另一端已经停止发送。


    示例如下：

    .. code:: c

        uint8_t buf[32];
        int len;

        len = wm_drv_uart_read(uart_dev, (uint8_t *)buf, (uint32_t)sizeof(buf), 1000);
        if(len > 0){
            printf("%.*s\n",(int)len,(char*)buf);
        }

.. _uart_receive:

receive_data 方式接收数据
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

    receive_data 方式使用接口函数 ``wm_drv_uart_receive_data`` 直接获取到驱动缓冲 buffer 的数据指针和数据块长度，获取后可以直接处理数据或者拷贝走，处理完成后需要调用 ``wm_drv_uart_release_data`` 接口释放指针，需要保证获取和释放完全匹配。相比 ``wm_drv_uart_read()`` 接口读取数据，该方式无需拷贝，可以直接处理数据。


    示例如下：

    .. code:: c

        uint8_t* pbuf;
        uint32_t len;

        if(wm_drv_uart_receive_data(uart_dev, &pbuf, &len) == WM_ERR_SUCCESS){
            printf("%.*s\n",(int)len,(char*)pbuf);
            wm_drv_uart_release_data(uart_dev, pbuf, len);
        }

事件
^^^^^^^^^^^^^

    Uart 事件有 TX 事件，RX 事件，错误事件三类，定义如下：

    .. code:: c

        typedef enum {
            WM_DRV_UART_TX_DONE,  /**< If tx_buf_size is specified, the Uart driver has sent some buffer data to the hardware fifo.
                                    The user can now write another packet to the uart buffer.
                                    If tx_buf_size is not specified, the Uart driver has sent the written data to the hardware fifo.*/
            WM_DRV_UART_RX_READY, /**< RX data ready ,some data is cached in uart buffer, The user can read it now */
            WM_DRV_UART_RX_ERROR, /**< RX error           */
        } wm_drv_uart_evt_type_t;

    接收事件需要调用接口 ``wm_drv_uart_register_callback`` 注册回调函数后才能接收到， 接收和处理事件不是必须的，但如果需要使用非阻塞的方式接收数据，则需要注册回调并接收事件。正常情况调用 ``wm_drv_uart_read()`` 接口，如果驱动的缓冲 buffer 中没有数据，可能会阻塞，但先收到 ``WM_DRV_UART_RX_READY`` 事件再去读则不会阻塞。

.. _uart_flowCtrl_func:

硬件流控模式
^^^^^^^^^^^^^

    =========================          =============
    flow_ctrl                          模式
    =========================          =============
    WM_UART_FLOW_CTRL_DISABLE          不开启流控
    WM_UART_FLOW_CTRL_RTS              开启RTS
    WM_UART_FLOW_CTRL_CTS              开启CTS
    WM_UART_FLOW_CTRL_RTS_CTS          开启RTS和CTS
    =========================          =============

    W800 UART 支持 RTS/CTS 方式的硬件流控。流控的主要目的是为了防止 UART fifo 里的数据因为软件来不及处理而造成丢失，RTS 和 CTS 是对应使用的。

    UART 模块支持硬件流控制功能，通过RTS（Request To Send）和 CTS（Clear To Send）信号线自动调整数据的发送速率。当接收端缓冲区接近满时，
    CTS 信号会被拉高，通知发送端暂停发送数据；当接收端准备好接收更多数据时，硬件 fifo 数据小于设定值时，CTS 信号会被重新拉低。这种机制有效防
    止了数据丢失和接收端缓冲区溢出的问题。

    在UART通信中使用额外的信号线来控制数据流的传输，防止数据丢失或溢出。硬件流控通过使用 **RTS(请求发送)** 和 **CTS(批准发送)**
    信号来实现，这两条信号线通常由 UART 硬件模块直接控制。如果硬件流控处于禁用状态，可使用函数 ``uart_set_flow_ctrl()`` 设置软件流控，
    需传入 UART 设备标识符和流控参数。

    硬件流控默认没有开启，可以调用 ``wm_drv_uart_set_flow_ctrl`` 或者 ``wm_drv_uart_set_config`` 进行开启


.. _uart_deinit:

反初始化 UART
^^^^^^^^^^^^^

    如不再需要与 UART 建立通信，则可调用 ``wm_drv_uart_deinit()`` 删除驱动程序，释放已分配的资源。


注意事项
-------------

.. note:: DMA 模式在设备表中配置，只能有一个 uart 可以使用 DMA , 比如 uart2,uart3 都配置了 DMA, 但只有先初始化的 port 会使用 DMA 模式传输。

.. warning:: 在使用 DMA 模式，初始化配置 TX buf 大小为0时，wm_drv_uart_write接口不支持发送 flash 地址的数据，比如 const 定义的变量和字符串常量。

.. note:: 当串口波特率很高时，要么接收 buffer 足够大，要么接收 Task 的优先级足够高，否则可能会丢失数据。

应用实例
-------------

    使用 UART 基本示例请参照 :ref:`examples/peripheral/uart<uart_example>`


API 参考
-------------
    查找 UART 相关 API 请参考：

    :ref:`label_api_uart`
