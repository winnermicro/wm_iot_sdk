.. _flash:

FLASH 
=============

简介
-------------

    - FLASH 存储器，也称为闪存，是一种非易失性存储技术，能够在断电的情况下保持数据不丢失，并支持电子方式进行数据的擦除和写入，同时提供快速的数据读取能力。
    - WMSDK 可同时支持一颗内部 flash 和多颗外部 flash 共存。
    - 使用同一套 API 操作内部 flash 和外部 flash。
    - 实现了读、写、擦除、设备查询等功能。
    - 支持两种方式写：写入前手动擦除、写入前自动擦除。
    - 支持三种方式擦除：从指定偏移量开始擦除、按扇区为单位擦除、擦除整个 flash 芯片上的所有数据。
    - 内部 flash 指的是通过 chip 上的 flash controller 方式控制的 flash， 外部是指 flash 没有经过 flash controller，直接通过 SPI 进行数据交互。
    - 内部 flash 支持 single SPI， QSPI mode， 外部 flash 只支持 single SPI mode。


功能列表
-------------

1. :ref:`initialize flash <init_flash>` -- 如何初始化内部或者外部 flash。
2. :ref:`read data <read_data>` -- 从 flash 中读取数据。
3. :ref:`write data <write_data>` -- 写入数据到 flash。
4. :ref:`erase data <erase_data>` -- 擦除 flash 中的数据。
5. :ref:`get information <get_info>` -- 获取 flash 基本信息。
6. :ref:`add internal flash <add_int_flash>` -- 如何添加内部flash。
7. :ref:`add external flash <add_ext_flash>` -- 如何添加外部flash。
8. :ref:`notice <note>` -- 注意事项。



.. _init_flash:

初始化 flash
------------------

    WMSDK 支持在一款芯片上，同时存在一颗内部 flash 以及多颗外部 flash

    内部 flash 的初始化：
        - 调用 flash driver API，并传入内部 flash 名称
        - 示例： wm_drv_flash_init("iflash")，初始化成功返回内部 flash 的 device pointer，失败返回 NULL
        - 内部 flash 的初始化在开机会被系统调用，用户一般无需再调用初始化操作
        - 如果内部 flash 已经被初始化，再次调用仍然可以正常返回内部 flash 的device pointer
        - 用户也可以通过 wm_dt_get_device_by_name("iflash") 获取到内部 flash 的device pointer，并通过 device 的 state 是否等于 WM_DEV_ST_INITED 来判断是否已经被初始化
        - 内部 flash 的相关参数可通过 device table 中配置，配置结构体： wm_dt_hw_iflash_t  

    外部 flash 的初始化：
        - 外部 flash 初始化前，需确保已经通过 SPI 接入了一颗外部 flash，并在驱动做添加了该 flash 的支持，可参考 :ref:`add external flash <add_ext_flash>`
        - 由于 WMSDK 同时支持多颗外部 flash，所以初始化的时候需指定外部 flash 名称
        - 假设有一颗为 W25Q 的外部 flash，则其初始化为： wm_drv_flash_init("W25Q")，执行成功返回外部 flash 的 device pointer，否则返回 NULL
        - 每一颗 flash 都会有一个对应的 device pointer， 在对 flash 进行操作时，需要传入对应的 device pointer，driver 才知道应该访问那一颗 flash
        - 外部 flash 的初始化参数也是通过 device table  进行配置，对应的结构体：wm_dt_hw_eflash_t


.. _read_data:

读取 flash
------------

    读取 flash 的 API： wm_drv_flash_read，支持读取 flash 任意区域，任意长度的内容

    示例代码如下：

    .. code:: c

        uint8_t *read_buf = NULL;
        uint32_t read_len = 121;
        uint32_t read_addr = 0x2000;
        int ret = WM_ERR_SUCCESS;
        wm_device_t *flash_dev = NULL;

        flash_dev = wm_dt_get_device_by_name("iflash");
        if (!flash_dev){
            printf("get iflash device fail\n");
            return -1;
        }

        if (flash_dev->state != WM_DEV_ST_INITED){
            flash_dev = wm_drv_flash_init("iflash");
            if (!flash_dev){
                printf("init fail\n");
                return -1;
            }
        }

        read_buf = malloc(read_len);
        if (read_buf){
            ret = wm_drv_flash_read(flash_dev, read_addr, read_buf, read_len);
            if (ret == WM_ERR_SUCCESS){
                printf("read flash succ\n");
            }
            else{
                printf("read flash fail\n");
            }
            free(read_buf);
        }
        else{
            printf("alloc read buf fail\n");
        }


.. _write_data:

写入 flash
------------

    支持带擦除的写入和不带擦除的写入两种方式； 支持在 flash 任意位置写入以及写入任意长度

        - 写入前擦除： wm_drv_flash_write_with_erase
        - 不带擦除： wm_drv_flash_write


    示例代码如下：

    .. code:: c

        uint8_t *wr_buf = NULL;
        uint32_t wr_len = 126;
        uint32_t wr_addr = 0x5001;
        int ret = WM_ERR_SUCCESS;
        wm_device_t *flash_dev = NULL;

        flash_dev = wm_dt_get_device_by_name("iflash");
        if (!flash_dev){
            printf("get iflash device fail\n");
            return -1;
        }

        if (flash_dev->state != WM_DEV_ST_INITED){
            flash_dev = wm_drv_flash_init("iflash");
            if (!flash_dev){
                printf("init fail\n");
                return -1;
            }
        }

        wr_buf = malloc(wr_len);
        if (wr_buf){
            for (int i = 0; i < wr_len; i++){
                wr_buf[i] = 0xA5;
            }

            ret = wm_drv_flash_write(flash_dev, wr_addr, wr_buf, wr_len);
            if (ret == WM_ERR_SUCCESS){
                printf("write flash succ\n");
            }
            else{
                printf("write flash fail\n");
            }

            ret = wm_drv_flash_write_with_erase(flash_dev, wr_addr, wr_buf, wr_len);
            if (ret == WM_ERR_SUCCESS){
                printf("write with erase flash succ\n");
            }
            else{
                printf("write with erase flash fail\n");
            }

            free(wr_buf);
        }
        else{
            printf("alloc write buf fail\n");
        }


.. _erase_data:

擦除 flash
------------

    支持三种方式擦除

        - 擦除任意区域：wm_drv_flash_erase_region
        - 按段擦除： wm_drv_flash_erase_sector
        - 擦除整颗flash： wm_drv_flash_erase_chip


    示例代码如下：

    .. code:: c

        uint32_t er_len = 2048;
        uint32_t er_addr = 0x5001;
        uint32_t er_sec = 10;
        uint32_t er_sec_cnt = 2;
        int ret = WM_ERR_SUCCESS;
        wm_device_t *flash_dev = NULL;

        flash_dev = wm_dt_get_device_by_name("iflash");
        if (!flash_dev){
            printf("get iflash device fail\n");
            return -1;
        }

        if (flash_dev->state != WM_DEV_ST_INITED){
            flash_dev = wm_drv_flash_init("iflash");
            if (!flash_dev){
                printf("init fail\n");
                return -1;
            }
        }


        ret = wm_drv_flash_erase_region(flash_dev, er_addr, er_len);
        if (ret == WM_ERR_SUCCESS){
            printf("erase region succ\n");
        }
        else{
            printf("erase region fail\n");
        }

        ret = wm_drv_flash_erase_sector(flash_dev, er_sec, er_sec_cnt);
        if (ret == WM_ERR_SUCCESS){
            printf("erase sector succ\n");
        }
        else{
            printf("erase sector fail\n");
        }


        wm_drv_flash_erase_chip(flash_dev);


.. _get_info:

获取 flash 基本信息
-------------------------

    驱动接口： wm_drv_flash_get_device_info，目前提供的信息有：flash 总大小，每个页的大小，每个段的大小，厂商 ID，设备 ID

    .. code:: c

        wm_drv_flash_info_t info = {0};
        int ret = WM_ERR_SUCCESS;
        wm_device_t *flash_dev = NULL;

        flash_dev = wm_dt_get_device_by_name("iflash");
        if (!flash_dev){
            printf("get iflash device fail\n");
            return -1;
        }

        if (flash_dev->state != WM_DEV_ST_INITED){
            flash_dev = wm_drv_flash_init("iflash");
            if (!flash_dev){
                printf("init fail\n");
                return -1;
            }
        }

        ret = wm_drv_flash_get_device_info(flash_dev, &info);
        if (ret == WM_ERR_SUCCESS){
            printf("flash info as below\n");
            printf("flash_size: %dMB\n", info.flash_size / (1024 * 1024));
            printf("page_size: %d byte\n", info.page_size);
            printf("sector_size: %d byte\n", info.sector_size);
            printf("vendor_id: 0x%x\n", info.vendor_id);
            printf("unique_id:%x%x%x%x\n", info.unique_id[0], info.unique_id[1], info.unique_id[2], info.unique_id[3]);
        }
        else{
            printf("get flash info fail\n");
        }

        return 0;


.. _add_int_flash:

如何添加内部 flash
---------------------

    不同的芯片内部 flash 的型号可能不一样，如果遇到系统中没有默认支持的型号，则需要用户自行添加对应的内部 flash。

    对于内部 flash，只能有一颗存在系统当中，即在系统运行时，SW 只会操作一颗 flash，不支持同时存在两颗或者两颗以上的内部 flash。

    对于类似 W802 这样的芯片，通过 flash controller 控制的外挂 flash，走的也是内部 flash 流程，添加新的 flash 型号，需要参考内部 flash 添加流程。


    内部 flash 添加比较简单，步骤如下
        - 硬件接入对应的内部 flash 型号
        - 在 wm_hal_iflash_chips.h 中添加一个 wm_flash_t 结构体实例
        - 将新添加的结构体示例，添加到 wm_iflash_support_chips[] 数组当中
        - 具体可参考目前默认支持的内部 flash 型号，如：wm_iflash_chip_p25q, wm_iflash_chip_th25q...


.. _add_ext_flash:

如何添加外部 flash
---------------------

    外部 flash 需要用户根据自己的需求自行添加。这里接收通过 SPI 方式外接 flash。外接的 flash 作为 SPI 的 slave 设备，挂载到 SPI 上。

    flash 接线
        - SPI 接线方式可参考 :ref:`label_component_driver_spim`
        - 目前 SPI 模块本身已经固定了 3 根 PIN 脚，可在 device table(wm_dt_hw_spim_t) 中修改
        - flash 模块需确定自己用的CS PIN 脚， 目前任意空闲的通用 GPIO 都可以作为 CS，可在 device table (wm_dt_hw_eflash_t) 中确定 CS PIN 脚

    驱动添加
        - 在 device table 中，添加对应外部 flash 的相关信息，对结构体： wm_dt_hw_eflash_t 增加一个实例并填写相关信息，可参考： dt_hw_eflash_w25q
        - 将实例化的 flash 添加到 device entry (dt_hw_table_entry) 中, 可参考： dt_hw_eflash_w25q
        - 在驱动文件： components\driver\src\ops\w80x\wm_drv_eflash_chips.h 中新增加一个 wm_eflash_t 实例并填写相关信息， 可参考：wm_external_flash_chip_p25q
        - 将新增加的 wm_eflash_t 实例添加到数据组 wm_external_flash_chips 中，可参考：wm_external_flash_chip_p25q

    以上两个操作完成，便可通过 flash drvier 的 API 访问外部 flash


.. _note:

注意事项
------------------

.. note::

    1. 操作 flash，只需输入 flash 的地址即可，无需加入平台基地址
    2. 如果通过 flash controller 方式控制 flash，最大支持空间为 16 MB，如果使用 SPI 方式控制，则无空间大小限制
    3. 运行 wm_drv_flash_write_with_erase 过程中，板子不要关机或者重启，否则会有 flash 数据丢失的风险
    4. 内部 flash 存在一个 page 被擦除后，只能写 64 次的限制， 如果写的次数达到了 64 次，需再次擦除才能保证数据的正确性 
    5. 如果修改了内部 flash 某些区域，可能会造成系统异常，操作之前可查看分区表，然后再操作对应的区域。 分区表具体操作见 :ref:`分区表 <partition_table>` 章节
    6. 内部 flash 前 8K 为系统数据，加了保护，用户拥有读取权限，没有写入或者擦除权限

应用实例
-------------

    使用 FLASH 基本示例请参照 :ref:`examples/peripheral/flash<flash_example>`

API参考
-------------
:ref:`label_api_flash_drver`

