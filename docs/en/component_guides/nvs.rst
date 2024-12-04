.. _nvs:

NVS
=======

Overview
-----------

Non-Volatile Storage (NVS) is a   functional module that uses flash memory to store key-value data. This article will introduce some basic concepts of NVS.

Fundamental Principles
----------------------------
The NVS library utilizes a portion of the main flash memory by calling the ``wm_nvs API`` API, specifically using the partition named "nvs" in the flash partition table to store data.

.. note::
    NVS is best suited for storing smaller data. For larger data storage, a filesystem approach should be considered.

Key-Value Pairs
------------------

The operating objects of NVS operates on key-value pairs, where the key is an ``ASCII`` string with a current maximum supported length of 15 characters. The stored values can be of the following types:

- Integers:  ``uint8_t`` ,  ``int8_t`` , ``uint16_t`` , ``int16_t`` , ``uint32_t`` , ``int32_t`` , ``uint64_t`` and  ``int64_t`` ;
- Floating point:  ``double`` type floating point numbers
- Strings:  strings terminated with ``\0`` , The maximum number of valid characters is 4031 bytes;
- Small Binary data: The maximum length is 4032 bytes;
- Binary Large OBject(BLOB): It can be divided into multi segment for storage.

.. note::
    When storing string data, the trailing ``\0`` character is also stored, so the actual stored length is the length obtained by the ``strlen`` function plus 1.
    When retrieving a string, the buffer length should be one more than the actual number of characters, otherwise an invalid parameter error will be returned.

.. note::
    Although NVS can support BLOB data size up to 512064 bytes, NVS is more suitable for storing small data in terms of read/write efficiency.

.. note::
    Although NVS provides read and write for integers and floating points, it's also possible to convert integers and floating points to strings for storage in the project.






Grouping
---------------------
Each data belongs to a group. Different business storage information can use different groups. Groups include group names and group ID values. Group names and ID values are also stored in partitions as key value pairs. Therefore, the longest group name can only be 15 characters, and the maximum number of groups is 62.


key-value
---------------------

Grouping and key value combine to distinguish different stored data. Under the same grouping, key value names must be unique, and key value names in different groups can be the same.

Data type checks are performed when the value is read. If the data type expected by the read operation does not match the data type of the corresponding key, an error is returned.

When writing, if the same group and key values already exist, the original values will be overwritten.


Handle
----------
NVS data is read and written through the handle. You need to open the handle before reading and writing. When opening, you need to specify the partition name, group name, and opening mode.

The opened handle will occupy a certain amount of memory resources, and it needs to be closed after all uses are completed. Even if the parameters of open are the same, a new handle will be opened. A maximum of 64 handles are supported.


NVS Iterator
---------------------
Iterator can be used to traverse data in NVS. You can specify to obtain a group name, type traversal, or specify handle and type traversal.

Use the following functions for related operations:

* ``wm_nvs_entry_find``: Creates an opaque iterator handle and positions the iterator at the first record for subsequent calls to ``wm_nvs_entry_next`` , ``wm_nvs_entry_info`` , and  ``wm_nvs_entry_data``  functions.

* ``wm_nvs_entry_next``: Points the iterator to the next key-value pair.

* ``wm_nvs_entry_info``: Returns information about the current key-value pair.

* ``wm_nvs_entry_data``: Returns the data of the current key-value pair.

* ``wm_nvs_release_iterator``: Releases the iterator handle.

In summary, all iterators obtained through ``wm_nvs_entry_find()`` (including NULL iterators) must be released using  ``wm_nvs_release_iterator()`` .


Functional Implementation
-----------------------------
To accommodate diverse user needs, we can implement multiple functions:

- NVS Initialization:

    ``wm_nvs_init``, ``wm_nvs_deinit``
    
    ``wm_nvs_init`` is used to initialize the NVS (Non-Volatile Storage) partition, and it is necessary to ensure that the partition to be initialized already exists in the partition table.

     Calling ``wm_nvs_deinit`` performs de-initialization, which releases all resources, including any open handles. Once de-initialized, any previously opened handles cannot be used again. If you need to re-initialize NVS, you must close or invalidate any existing handles before calling ``wm_nvs_deinit`` again.

- Erase partition:

    ``wm_nvs_erase``

    If the partition has been initialized, you need to call 'wm_nvs_deinit' before erasing. It is usually called when setting factory recovery.
    It is better to reboot the system after calling this interface.

- NVS Open,Close, and Read/Write Operation Handles:

    ``wm_nvs_open``, ``wm_nvs_close``,Each time you open an NVS handle, it consumes 48 bytes of memory. It is necessary to close the handle after use to free up resources.

- String Write and Read:

    ``wm_nvs_set_str``, ``wm_nvs_get_str``

- Small binary Write and Read:

    ``wm_nvs_set_binary``, ``wm_nvs_get_binary``

     It does not support fragmented storage and can only store a maximum of 4032 bytes, making it suitable for storing smaller binary data.
     
-  Large Binary Object Writing and Reading   
    
    ``wm_nvs_set_blob``, ``wm_nvs_get_blob``
    
    Supports fragmented storage with a maximum of 127 fragments. Each fragment can be a maximum of 4032 bytes and a minimum of 512 bytes (except for the last fragment, which is not bound by the minimum 512-byte limit). The total size of all fragments, however, must not exceed 512,064 bytes.
    

- 8-bit, 16-bit, 32-bit, 64-bit Integer Write and Read:

    ``wm_nvs_set_i8``, ``wm_nvs_get_i8``

    ``wm_nvs_set_i16``, ``wm_nvs_get_i16``

    ``wm_nvs_set_i32``, ``wm_nvs_get_i32``

    ``wm_nvs_set_i64``, ``wm_nvs_get_i64``

    ``wm_nvs_set_u8``, ``wm_nvs_get_u8``

    ``wm_nvs_set_u16``, ``wm_nvs_get_u16``

    ``wm_nvs_set_u32``, ``wm_nvs_get_u32``

    ``wm_nvs_set_u64``, ``wm_nvs_get_u64``

- Floating Point Read and Write:

    ``wm_nvs_set_float``, ``wm_nvs_get_float``

- Traverse NVS Data Items and Print:

    ``wm_nvs_print``

- Get Data Item Type and Size:

    ``wm_nvs_get_info``

-  NVS Data Item Deletion:

    ``wm_nvs_del_key``

-  Delete all Items in group:

    ``wm_nvs_del_group``

- NVS Iteration Interface:

    ``wm_nvs_entry_find`` : Create an iterator and positions it at the first record

    ``wm_nvs_entry_next`` : Move the iterator to the next record.

    ``wm_nvs_entry_info`` : Retrieve the key, type, and data length of the current record based on the iterator.

    ``wm_nvs_entry_data`` : Retrieve the data of the current record based on the iterator.

    ``wm_nvs_release_iterator`` : Release the iterator.

- Supports **Wear Leveling** 
- Supports **Power Failure Protection** 
- Supports **Hash Quick Read and Write** 

.. warning::
    After executing ``wm_nvs_erase`` , the NVS partition will be formatted, resulting in the deletion of all stored data. If the partition has already been initialized, it must first be deinitialized before it can be erased. Please use this.


Configuration Method
-----------------------------

In the project, to adjust the size of NVS, you can modify the partition named ``nvs`` in the partition table. The starting address and the size are both adjustable. The specific configuration details are as follows:

::

    # name,         offset,        size,       flag
    nvs,            0x1F0000,      0x8000,     0x0

For detailed configuration, refer to the :ref:`partition table <partition_table>` section.

.. note::
   During data writing, it is sometimes necessary to reclaim space occupied by deleted data. To prevent data loss due to power failure during the reclaiming process, a dedicated sector is required for this purpose. Therefore, the size configuration must include at least two sectors, which translates to a minimum of 0x2000.


Memory consumption by partition
-------------------------------------
::

    The number of sectors:        sector_num
    The number of groups:         group_num
    The number of opened handles: handle_num
    The number of saved items:    item_num

    All memory usage can be calculated using the following formula:

    all = 144 + 42 * sector_num  + 56 * group_num + 48 * handle_num + 4 * item_num

Module Configuration
----------------------

.. note::
    After the CONFIG_NVS_VER_NUM is modified, the partition will be reset after restart, and all data will be lost.

Application Examples
--------------------------
For basic examples of using NVS, please refer to :ref:`examples/storage <storage_example>`
