
.. _nvs:

NVS
=======

概述
--------

非易失性存储（NVS） 是一种使用 flash 来存储 key-value 键值对数据的功能模块，本文将介绍 NVS 的一些基本概念。

基本原理
----------
NVS 库通过调用 wm_nvs API 使用主 flash 的部分空间，具体使用 flash 分区表中名称为 nvs 的分区来存储数据。

.. note::
    NVS 最适合存储一些较小的数据，大数据的存储可以考虑文件系统方式。

键值对
----------

NVS 的操作对象为键值对，其中键是 ``ASCII`` 字符串，当前支持的最大长度为 15 个字符。存储值可以是以下几种类型：

- 整数型： ``uint8_t`` 、 ``int8_t`` 、 ``uint16_t`` 、 ``int16_t`` 、 ``uint32_t`` 、 ``int32_t`` 、 ``uint64_t`` 和  ``int64_t`` ；
- 浮点数： ``double`` 类型的浮点数；
- 字符串：以 ``\0`` 结尾的字符串；最大存储 4031 个字节的有效字符；
- 小二进制数据：最大长度 4032 字节；
- 大二进制对象( BLOB )：支持分片存储，总共最大支持 512064 字节；


.. note::
    存储字符串类型的数据时，会把末尾的 ``\0`` 字符也进行存储，即实际存储的长度为 strlen 函数获取到的长度加 1； 在使用接口获取字符串时，
    给的 buffer 长度需要比实际字符个数多 1，否则会返回无效参数。

.. note::
    NVS 虽然可以支持 BLOB 数据大小到 512064 字节，但从读写效率上看，NVS 更适合存储小数据。

.. note::
    如果 NVS 分区调整过，包括缩小分区，扩大分区，改变分区位置，则应擦除分区内容。



Group分组
----------

每个数据都属于某个分组，不同的业务存储信息可以使用不同的分组，分组包括分组名称和分组 ID 值，分组名称和 ID 值也会作为键值对存储到分区中。所以分组名称
最长也只能是 15 个字符，分组最大个数为 62 个。


键值
----------

分组和键值合起来区分不同的存储数据，相同分组下，键值名称必须唯一，不同分组中的键值名称可以相同。

读取值时会执行数据类型检查。如果读取操作预期的数据类型与对应键的数据类型不匹配，则返回错误。

在写入时，如果已经存在相同的分组和键值，则会覆盖原来的值。


Handle
----------

NVS 数据读写都通过 handle 进行，需要先打开 handle 才能读写，打开时需要指定分区名称，分组名称，打开方式。
打开的handle会占用一定内存资源，所有使用完成后需要关闭。即使 open 的参数相同，也会打开一个全新的 handle 。 最大支持 64 个 handle。


NVS迭代器
---------------------
迭代器允许用于遍历 NVS 中的数据，可以指定获取某个分组名称，类型遍历，或者指定 handle 和类型遍历。

使用以下函数，可执行相关操作：

* ``wm_nvs_entry_find``：创建一个不透明迭代器句柄,并让迭代器指向第一条记录，用于后续调用 ``wm_nvs_entry_next`` , ``wm_nvs_entry_info`` 和 ``wm_nvs_entry_data`` 函数；

* ``wm_nvs_entry_next``：让迭代器指向下一个键值对；

* ``wm_nvs_entry_info``：返回当前键值对的信息；

* ``wm_nvs_entry_data``：返回当前键值对的数据；

* ``wm_nvs_release_iterator``：释放迭代器句柄；

总的来说，所有通过 ``wm_nvs_entry_find()`` 获得的迭代器（包括 NULL 迭代器）都必须使用 ``wm_nvs_release_iterator()`` 释放。


下面 example 中，遍历 EXAMPLE_NVS_GROP 分组中全部数据记录。开始调用 wm_nvs_entry_find 时，iterator 内部 Context 指向满足条件的第一条记录，在循环中处理完一条记录后，
调用 wm_nvs_entry_next 跳到下一条记录，当 wm_nvs_entry_next 返回的 iterator 为 NULL　时，表示已经处理完成。

.. code:: C

    /*Traversal items using iterators*/
    void example_nvs_iterator(wm_nvs_handle_t handle)
    {
        wm_nvs_iterator_t iterator;
        unsigned char buf[32];
        size_t len;
        wm_nvs_entry_t info;

        wm_log_info("Start iterator.");

        /* Get item iterator by group name */
        if (wm_nvs_entry_find(WM_NVS_DEF_PARTITION, EXAMPLE_NVS_GROUP, WM_NVS_TYPE_ANY, &iterator) != WM_ERR_SUCCESS) {
            return;
        }

        /* Processing items */
        while (iterator) {

            /* Get item name, type, data length */
            wm_nvs_entry_info(iterator, &info);

            len = sizeof(buf);

            /* Read item data */
            if (wm_nvs_entry_data(iterator, buf, &len) == WM_ERR_SUCCESS) {
                wm_log_info("%s,len=%d", info.key, (int)len);
            }

            /* Goto next item*/
            wm_nvs_entry_next(&iterator);
        }

        wm_nvs_release_iterator(iterator);
    }


.. note::
    iterator 这套接口，主要用在需要遍历全部或者某个分组数据时使用。如果在调用时已经知道数据记录名称和类型时，建议使用对应的 API 接口去获取。


实现功能
--------------
为了用户使用的多样性，我们能实现多种功能：

- NVS 初始化分区:

    ``wm_nvs_init``, ``wm_nvs_deinit``

    ``wm_nvs_init`` 用于初始化 NVS 分区，需要确保分区表中已经存在要初始化的分区。

    调用 ``wm_nvs_deinit`` 进行反初始化，调用时会释放所有资源，包括打开的 handle , 再次初始化，原来打开的 handle 也是不能使用的。

- NVS 擦除分区:

    ``wm_nvs_erase``

    如果分区已经初始化，需要先调用 ``wm_nvs_deinit`` 再进行擦除操作, 通常在恢复出厂设置时调用该接口，调用后最好重启系统。


- NVS 打开，关闭读写操作句柄:

    ``wm_nvs_open``, ``wm_nvs_close``, 每次打开需要消耗 48 字节内存，不使用后需要关闭

- 字符串写入与读取:

    ``wm_nvs_set_str``, ``wm_nvs_get_str``

- 小二进制数据写入与读取:

    ``wm_nvs_set_binary``, ``wm_nvs_get_binary``

    不支持分片存储，最大只能存储 4032 字节，适合存储较小的二进制数据。

- 大二进制对象写入与读取:

    ``wm_nvs_set_blob``, ``wm_nvs_get_blob``

    支持分片存储，最多可分 127 片， 每片最大 4032 字节，最小 512 字节，最后一个分片不受最小 512 字节限制，总大小不超过 512064。

- 8 位、16 位、32 位、64 位整数的写入与读取:

    ``wm_nvs_set_i8``, ``wm_nvs_get_i8``

    ``wm_nvs_set_i16``, ``wm_nvs_get_i16``

    ``wm_nvs_set_i32``, ``wm_nvs_get_i32``

    ``wm_nvs_set_i64``, ``wm_nvs_get_i64``

    ``wm_nvs_set_u8``, ``wm_nvs_get_u8``

    ``wm_nvs_set_u16``, ``wm_nvs_get_u16``

    ``wm_nvs_set_u32``, ``wm_nvs_get_u32``

    ``wm_nvs_set_u64``, ``wm_nvs_get_u64``

- 浮点数的读写:

    ``wm_nvs_set_float``, ``wm_nvs_get_float``

- 遍历 NVS 数据项并打印:

    ``wm_nvs_print``

-  获取数据项的类型与大小:

    ``wm_nvs_get_info``

-  NVS 数据项删除:

    ``wm_nvs_del_key``

-  NVS 删除分组:

    ``wm_nvs_del_group``

- NVS 遍历接口:

    ``wm_nvs_entry_find`` ： 创建迭代器，让迭代器指向第一条记录

    ``wm_nvs_entry_next`` ： 移动到下一条记录

    ``wm_nvs_entry_info`` ： 根据迭代器获取记录的键值，类型，数据长度

    ``wm_nvs_entry_data`` ： 根据迭代器获取记录的数据

    ``wm_nvs_release_iterator`` ： 释放迭代器

- 支持 **磨损平衡**
- 支持 **掉电保护**
- 支持 **Hash快速读写**

.. warning::
    ``wm_nvs_erase`` 执行后，将格式化 nvs 的分区，所有存储数据将清空，如果已经 init 分区，需要先 deinit 分区后才能擦除。请谨慎使用。


配置方法
-------------

在项目中要调整 NVS 大小时，可以调整分区表中名称为 ``nvs`` 的分区，起始地址和大小都可以调整。具体配置如下：

::

    # name,         offset,        size,       flag
    nvs,            0x1F0000,      0x8000,     0x0

详细配置请参考 :ref:`分区表 <partition_table>` 章节。

.. note::
    在数据写入时，有时需要对已经删除数据进行回收，为预防回收过程中断电导致数据丢失，需要一个专门的 sector 做回收使用，所以 size 大小至少要配置 2 个 sector, 即至少 0x2000。



分区内存占用
-------------
::

    分区 sector 数:    sector_num
    group 数:          group_num
    打开的 handle 数:  handle_num
    存储数据总数:      item_num

    全部内存占用字节数计算如下：

    all = 144 + 42 * sector_num  + 56 * group_num + 48 * handle_num + 4 * item_num


模块配置
-------------

.. note::
    CONFIG_NVS_VER_NUM 修改后，重启会重置分区，数据将全部丢失。


应用实例
-------------

使用 NVS 基本示例请参照 :ref:`examples/storage <storage_example>`
