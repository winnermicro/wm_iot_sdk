
.. _dma:

DMA
=============

简介
-------------

    DMA（Direct Memory Access）直接存储器访问是一种数据传输方式，DMA 用来提供在外设和存储器之间或者存储器和存储器之间的高速数据传输。无须CPU的干预，通过DMA数据可以快速地移动。DMA的作用就是实现数据的直接传输，而去掉了传统数据传输需要CPU寄存器参与的环节。

功能列表
-------------

    1. :ref:`初始化DMA <dma_init>`——初始化DMA设备。
    2. :ref:`请求DMA通道 <dma_request_ch>`——请求DMA通道。
    3. :ref:`释放DMA通道 <dma_release_ch>`——释放DMA通道。
    4. :ref:`注册DMA回调 <dma_register_callback>`——注册DMA回调函数。
    5. :ref:`注销DMA回调 <dma_unregister_callback>`——注销DMA回调函数。
    6. :ref:`配置DMA传输 <dma_config>`——配置DMA传输参数。
    7. :ref:`启动DMA传输 <dma_start>`——启动DMA传输。
    8. :ref:`停止DMA传输 <dma_stop>`——停止DMA传输。
    9. :ref:`获取DMA状态 <dma_get_status>`——获取DMA传输状态。
    10. :ref:`DMA传输 <dma_transfer>`——配置并启动DMA传输。

功能概述
-------------


    **参数配置** 支持配置源地址、目标地址、传输长度、传输模式等参数。

    **支持多种传输模式** DMA支持单次传输、突发传输等多种传输模式，以适应不同的应用需求。
    
    **总线控制** DMA 控制器响应外设的请求，向 CPU 发出总线请求信号。当 CPU 放弃对总线的控制时，DMA 控制器接管总线控制权。获得总线控制权后，DMA 控制器将地址信号发送到地址总线，并在 DMA 期间发出读写控制信号。

    **传输管理** DMA 控制器决定本次传输的字节数，并判断本次 DMA 传输是否结束。传输结束后，DMA 控制器将总线控制权归还给 CPU。

    **事件** 支持传输完成、传输错误事件，通过注册回调函数处理这些事件。


主要功能
-----------------------

.. _dma_init:

初始化DMA
^^^^^^^^^^^^^^^^^^^^

    调用函数 ``wm_drv_dma_init()`` 初始化DMA设备。初始化过程包括配置DMA的硬件参数和初始化必要的数据结构，使得DMA设备可以正常工作。示例如下：

    .. code:: c

        wm_device_t *dma_dev;
        dma_dev = wm_drv_dma_init("dma");

    其中， ``dma`` 是DMA设备的名称，用于标识设备，需要在设备表中配置。类型为 ``const char *``。

    .. warning:: DMA初始化后，如果没有调用 ``wm_drv_dma_deinit`` , 再次调用 ``wm_drv_dma_init`` 会返回 ``NULL`` 。


.. _dma_request_ch:

请求DMA通道
^^^^^^^^^^^^^^^^^^^^

    调用函数 ``wm_drv_dma_request_ch()`` 请求DMA用于指定DMA传输的源和目标地址及其相关配置。示例如下：

    .. code:: c

        wm_device_t *dma_dev;
        dma_dev = wm_drv_dma_init("dma");

        uint8_t dma_ch = 0;
        wm_drv_dma_request_ch(dma_dev, &ch, DRV_DMA_M2M_EXAMPLE_TO_US);

    第一个参数是DMA设备引用，类型为 ``wm_device_t *``。
    
    第二个参数是用于存储分配的DMA通道号，类型为 ``uint8_t *``。
    
    第三个参数是等待通道分配的超时时间，单位为毫秒，这里设置为 ``DRV_DMA_M2M_EXAMPLE_TO_US`` ，值为 ``0xffffffffUL`` ，类型为 ``uint32_t``。

.. _dma_release_ch:

释放DMA通道
^^^^^^^^^^^^^^^^^^^^

    调用函数 ``wm_drv_dma_release_ch()`` 释放DMA通道，用于中止DMA传输，释放已分配的DMA资源。示例如下：

    .. code:: c

        wm_device_t *dma_dev;
        dma_dev = wm_drv_dma_init("dma");

        uint8_t dma_ch = 0;
        wm_drv_dma_release_ch(dma_dev, ch, DRV_DMA_M2M_EXAMPLE_TO_US);

    第一个参数是DMA设备引用、类型为 ``wm_device_t *``。
    
    第二个参数要释放的DMA通道号和，类型为 ``uint8_t``。
    
    第三个参数是等待释放通道的超时时间，单位为毫秒，这里设置为 ``DRV_DMA_M2M_EXAMPLE_TO_US`` ，值为 ``0xffffffffUL`` ，类型为 ``uint32_t``。

.. _dma_register_callback:

注册DMA完成回调
^^^^^^^^^^^^^^^^^^^^

    调用函数 ``wm_drv_dma_register_callback()`` 注册DMA完成回调函数，回调函数再DMA传输完成时被调用，用于处理DMA传输结束后的操作。示例如下：

    .. code:: c

        wm_device_t *dma_dev;
        dma_dev = wm_drv_dma_init("dma");

        uint8_t dma_ch = 0;
        void dma_callback(wm_dma_ch_t ch, uint32_t sts, void *user_data) {
        }
        wm_drv_dma_register_callback(dma_dev, dma_ch, dma_callback, NULL);

    第一个参数是DMA设备引用、类型为 ``wm_device_t *``。

    第二个参数用于存储DMA通道号，类型为 ``uint8_t``。

    第三个参数在DMA传输完成时被调用，用于处理传输结束后的操作，类型为 ``wm_drv_dma_ch_callback_t``。
    
    第四个参数是用户数据指针，可以传递额外的信息，在本例中未传递任何信息，类型为 ``void *``。

.. _dma_unregister_callback:

注销DMA完成回调
^^^^^^^^^^^^^^^^^^^^

    调用函数 ``wm_drv_dma_unregister_callback()`` 注销DMA完成回调函数。示例如下：

    .. code:: c

        wm_device_t *dma_dev;
        dma_dev = wm_drv_dma_init("dma");

        uint8_t dma_ch = 0;

        wm_drv_dma_unregister_callback(dma_dev, dma_ch);

    第一个参数是DMA设备引用、类型为 ``wm_device_t *``。
    
    第二个参数用于DMA通道号，类型为 ``uint8_t``。

.. _dma_config:

配置DMA传输
^^^^^^^^^^^^^^^^^^^^

    调用函数 ``wm_drv_dma_config()`` 配置DMA传输参数。示例如下：

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

    第一个参数是DMA设备引用、类型为 ``wm_device_t *``。
    
    第二个参数是传输描述符，设置源地址和目标地址，其中 ``src`` 为源数据的起始地址，指向 dma_src_data 数组的起始位置， ``dest`` 为目的地缓冲区的起始地址，指向 dma_dest_buf 数组的起始位置。类型为 ``wm_drv_dma_desc_t *``。
    
    本例中，分别配置了传输字节长度、DMA通道号、数据传输方向、突发传输的大小、源地址递增模式、目的地址递增模式、中断使能、自动重载和DMA模式。设置具体值可参考设备表。

.. note::
   如果您要使用循环模式，即 dma_mode 为 WM_DRV_DMA_WARP_MODE 需要注意如下细节：
        1. 保证搬运长度以及循环的源数据长度和循环的目的数据长度统一四字节对齐。
        2. 搬运长度应大于循环的目的数据长度至少四个字节，即 drv_desc->ctrl.len 大于 drv_desc->ctrl.dest_warp_len 至少四个字节。

.. _dma_start:

启动DMA传输
^^^^^^^^^^^^^^^^^^^^

    调用函数 ``wm_drv_dma_start()`` 启动DMA传输，DMA控制器将根据配置的传输参数开始进行数据传输。示例如下：

    .. code:: c
        
        wm_device_t *dma_dev;
        dma_dev = wm_drv_dma_init("dma");

        uint8_t dma_ch = 0;
        
        wm_drv_dma_start(dma_dev, dma_ch);

    第一个参数是DMA设备引用、类型为 ``wm_device_t *``。
    
    第二个参数是要启动传输的DMA通道号，类型为 ``uint8_t``。
   

.. _dma_stop:

停止DMA传输
^^^^^^^^^^^^^^^^^^^^

    调用函数 ``wm_drv_dma_stop()`` 停止DMA传输，DMA控制器将停止当前的传输操作。示例如下：

    .. code:: c

        wm_device_t *dma_dev;
        dma_dev = wm_drv_dma_init("dma");

        uint8_t dma_ch = 0;

        wm_drv_dma_stop(dma_dev, dma_ch);

    第一个参数是DMA设备引用、类型为 ``wm_device_t *``。

    第二个参数是要停止传输的DMA通道号，类型为 ``uint8_t``。

.. _dma_get_status:

获取DMA状态
^^^^^^^^^^^^^^^^^^^^

    调用函数 ``wm_drv_dma_get_status()`` 获取DMA传输状态。示例如下：

    .. code:: c

        wm_device_t *dma_dev;
        dma_dev = wm_drv_dma_init("dma");

        uint8_t dma_ch = 0;
        
        wm_dma_sts_info_t dma_sts = {
        .sts      = WM_DMA_RUNNIG,
        .xfer_cnt = 0,
        };

        wm_drv_dma_get_status(dma_dev, ch, &dma_sts);
    
    第一个参数是DMA设备引用、类型为 ``wm_device_t *``。

    第二个参数是要查询状态的DMA通道号，类型为 ``uint8_t``。

    第三个参数是指向 ``wm_dma_sts_info_t`` 结构体的指针，用于存储DMA的状态信息。

    本例初始化时设置了DMA的当前状态，并将已传输数据计数置为零。

.. _dma_transfer:

DMA传输
^^^^^^^^^^^^^^^^^^^^

    调用函数 ``wm_drv_dma_transfer()`` 配置并启动DMA传输,实际应用中，根据具体的传输需求，可以调整dma_example_cfg中的各项参数，以满足不同的传输要求示例如下：

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

    第一个参数是DMA设备引用、类型为 ``wm_device_t *``。
    
    第二个参数是传输描述符，设置源地址和目标地址，其中 ``src`` 为源数据的起始地址，指向 dma_src_data 数组的起始位置， ``dest`` 为目的地缓冲区的起始地址，指向 dma_dest_buf 数组的起始位置。类型为 ``wm_drv_dma_desc_t *``。
    
    本例中，分别配置了传输字节长度、DMA通道号、数据传输方向、突发传输的大小、源地址递增模式、目的地址递增模式、中断使能、自动重载和DMA模式。设置具体值可参考设备表。


应用实例
-------------

    使用 DMA 基本示例请参考 :ref:`examples/peripheral/dma<peripheral_example>`

API参考
-------------
    :ref:`label_api_dma`