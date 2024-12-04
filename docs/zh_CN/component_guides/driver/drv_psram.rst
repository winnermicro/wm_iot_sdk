.. _drv_psram:

PSRAM
========================

简介
------------------------

    PSRAM（伪静态随机存取存储器，Pseudo-Static Random Access Memory）是一种结合了动态随机存取存储器（DRAM）和静态随机存取存储器（SRAM）优点的存储技术。
    W800 内置了 1 片支持 SPI/QSPI 接口的 PSRAM 控制器，可以访问外置 PSRAM 设备，并提供总线方式的 PSRAM 读写和擦除操作，最高读写速度可达 80MHz。


功能列表
------------------------

    1. :ref:`初始化 <psram_init>`——初始化 PSRAM 驱动。
    2. :ref:`设置回调函数 <psram_callback>`——配置在读写数据完成后需要执行的操作。
    3. :ref:`IO 控制 <psram_ioctrl>`——设置 PSRAM 的工作模式。
    4. :ref:`获取 PSRAM 大小 <psram_getsize>`——获取 PSRAM 的总容量。
    5. :ref:`内存复制 <psram_memcpy>`——将 PSRAM 内存中的数据复制到指定地址。


功能概述
------------------------

    **工作模式** W800 内置 PSRAM 支持3种工作模式，各种模式介绍如下：

    ===================== ============================================================================
    工作模式                模式说明
    ===================== ============================================================================
    ``SPI Mode``          具有片选 CS、时钟 CLK、数据输入 SI、数据输出 SO 四个信号脚，

                          用于标准串行数据传输。

    ``QSPI Mode``         QSPI 在传统 SPI 基础上增加了两条 IO 数据线（总共四条数据线：

                          IO0、IO1、IO2、IO3）， 允许同时传输四位数据，传输速率更快，

                          适用于高性能数据传输需求。

    ``Half Sleep Mode``   半睡眠模式，设备进入低功耗状态，该模式下，只能读不能写，已经

                          存储在 PSRAM 的数据会保持。
    ===================== ============================================================================

    **传输速率** PSRAM 支持的最大传输速率为 80MHz。

    **容量支持** PSRAM 的容量支持 2MB、4MB 或 8MB，最大支持到 8 M。

    **内存管理** 支持使用函数 ``wm_heap_caps_alloc`` ， ``wm_heap_caps_realloc`` ， ``wm_heap_caps_free`` 分配 pSRAM 到 heap 中。

    **支持 DMA** 需要复制的内存地址4字节对齐，大小超过 1K 时，使用 DMA 进行复制，减小 CPU 负担。


PSRAM 使用配置
------------------------

设置 PSRAM
^^^^^^^^^^^^^^^^^^^^^^^^

    在使用 PSRAM 之前，需要在项目工程的 ``prj.config`` 文件中将 ``CONFIG_COMPONENT_DRIVER_PSRAM_ENABLED`` 设置为 ``y``。
    或者，在编译环境中使用 ``menuconfig`` 中的 ``Components/PERIPHERIALS/PSRAM`` 设置启用 PSRAM 。

数据传输管脚配置
^^^^^^^^^^^^^^^^^^^^^^^^

    PSRAM 的引脚已默认配置，目前 pin 脚在设备表中进行配置, 使用如下。


    ===================== =============== ===================================================
    pin GPIO               pin脚           模式说明
    ===================== =============== ===================================================
    ``WM_GPIO_NUM_15``     PA15            SCLK 时钟
    ``WM_GPIO_NUM_43``     PB27            CS 片选
    ``WM_GPIO_NUM_18``     PB2             SIO0
    ``WM_GPIO_NUM_19``     PB3             SIO1
    ``WM_GPIO_NUM_20``     PB4             SIO2
    ``WM_GPIO_NUM_21``     PB5             SIO3
    ===================== =============== ===================================================


主要功能
------------------------

.. _psram_init:

初始化
^^^^^^^^^^^^^^^^^^^^^^^^

    使用 PSRAM 前，wm_heap初始化时已经调用过 ``wm_drv_psram_init()`` ，使用时只需调用 ``wm_dt_get_device_by_name`` 获取 ``wm_device_t`` 设备。示例：

    .. code:: c

        wm_device_t *psram_dev;
        psram_dev = wm_dt_get_device_by_name("psram0");


    目前 W800 只支持 1 片 PSRAM，所有第一个参数只能传入 ``psram0``。

.. _psram_callback:

设置回调函数
^^^^^^^^^^^^^^^^^^^^^^^^

    在数据完成传输后，会调用回调函数，若需要执行某些操作，可以通过 ``wm_drv_psram_register_callback()`` 函数进行设置，参考示例：

    .. code:: c
    
        wm_drv_psram_callback_t psram_callback_example(void){
            wm_log_info("this is psram callback func");
        }

        wm_device_t *psram_dev;
        psram_dev = wm_dt_get_device_by_name("psram0");

        wm_drv_psram_register_callback(psram_dev,psram_callback_example,NULL);

    第一个参数为 PSRAM 设备标识符

    第二个参数为传输完成后的回调函数

    第三个参数为回调函数中需要传入的参数

.. _psram_ioctrl:

IO 控制
^^^^^^^^^^^^^^^^^^^^^^^^

    使用 PSRAM 时需配置好IO的工作模式，使用 ``wm_drv_psram_ioctrl()`` 函数进行设置，需要传入四个参数： ``device`` 、 ``cmd`` 、 ``param`` 和 ``*arg`` ， ``cmd`` 和 ``param`` 不同参数组合对应的工作模式如下：
    
    ===============================         ===============================            ===============================    
    cmd                                     param                                       工作模式
    ===============================         ===============================            ===============================
    WM_DRV_PSRAM_CMD_SET_MODE                WM_DRV_PSRAM_MODE_SPI                      SPI Mode
    WM_DRV_PSRAM_CMD_SET_MODE                WM_DRV_PSRAM_MODE_QSPI                     QSPI Mode
    WM_DRV_PSRAM_CMD_ENABLE_HSM              NULL                                       Half Sleep Mode
    WM_DRV_PSRAM_CMD_DISENABLE_HSM           NULL                                       Standard Mode
    ===============================         ===============================            ===============================  

    在设置半休眠模式后， PSRAM 会进入低功耗模式，不能进行读写操作。

    IO 控制使用实例参考：

    .. code:: c

        wm_device_t *psram_dev;
        psram_dev = wm_dt_get_device_by_name("psram0");

        wm_drv_psram_ioctrl(psram_dev,WM_DRV_PSRAM_CMD_SET_MODE,WM_DRV_PSRAM_MODE_SPI,NULL);


.. _psram_getsize:

获取 PSRAM 大小
^^^^^^^^^^^^^^^^^^^^^^^^

    若需获取 PSRAM 的大小可调用 ``wm_drv_psram_get_size()`` 函数，使用示例如下：

    .. code:: c

        uint32_t size;
        wm_device_t *psram_dev;
        psram_dev = wm_dt_get_device_by_name("psram0");

        wm_drv_psram_get_size(psram_dev,&size);

    ``size`` 的值为 PSRAM 初始时配置的大小。


.. _psram_memcpy:

内存复制
^^^^^^^^^^^^^^^^^^^^^^^^

    若需将 PSRAM 的数据进行转移可调用 ``wm_drv_psram_memcpy_dma()`` 函数，需要依次传入设备标识符  ``device`` 、目的地址 ``dst`` 、源地址 ``src`` 和数据大小 ``size`` ，具体示例如下：

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

    **使用 DMA 复制有三个条件：**

    - 源地址和目标地址必须为 4 字节对齐。
    - 复制长度不得小于 PSRAM_MIN_DMA_SIZE，默认为 1024。
    - 设备表配置了使用 DMA 的 PSRAM 设备。在调用 API 时，将申请一个空闲的 DMA 传输通道。
    - wm_drv_psram_memcpy_dma 函数启动 DMA 传输后，立即返回，这时还没有传输完成。如果这时再次调用，接口将返回 WM_ERR_BUSY。

反初始化
^^^^^^^^^^^^^^^^^^^^^^^^

    若不再需要使用 PSRAM 存储数据，可调用 ``wm_drv_psram_deinit()`` 删除驱动程序，释放已分配的资源。


PSRAM 内存使用的几种方式
------------------------

    - 全局变量方式定义

        这种方式定义的变量一直占用，系统启动不会默认初始化，自己使用前需要注意初始化。

        .. code:: c

            ATTRIBUTE_PSRAM static uint8_t psram_buf[64];



    - 使用 ``wm_heap_caps_alloc`` 和 ``wm_heap_caps_free``

        这种方式使用时分配，用完之后释放。

        .. code:: c

            uint8_t* psram_buf = wm_heap_caps_alloc(64,WM_HEAP_CAP_SPIRAM);
            if(psram_buf){
                //在这里使用 psram_buf
                wm_heap_caps_free(psram_buf);
            }


注意事项
------------------------

.. note::

    - 使用 ATTRIBUTE_PSRAM 修饰定义的全局变量，会放到 PSRAM 中，但不会默认初始化。

    - 不能把代码放到 PSRAM 中执行。

.. warning::

    - DMA 和 CPU 都通过 SPI 引脚访问 PSRAM，因此它们不能同时访问 PSRAM ,同时访问会引起系统异常死机。



应用实例
------------------------

    使用 PSRAM 基本示例请参照: :ref:`examples/peripheral/psram<peripheral_example>`

API 参考
------------------------

    查找 PSRAM 相关 API 请参考：:ref:`label_api_psram`
