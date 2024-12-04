.. _flash:

FLASH 
=============

Introduction
--------------

- FLASH memory, also known as flash storage, is a type of non-volatile memory technology that retains data even when powered off. It supports electronic data erasure and writing while providing fast data reading capabilities.
- WMSDK can support the coexistence of one internal flash and multiple external flashes simultaneously.
- The same set of APIs is used to operate both internal and external flashes.
- It realizes functions such as reading, writing, erasing, and device querying.
- Two writing methods are supported: manual erasure before writing and automatic erasure before writing.
- Three erasure methods are supported: erasure starting from a specified offset, erasure by sector, and erasure of all data on the entire Flash chip.
- Internal flash refers to flash controlled through the flash controller on the chip, while external flash does not go through the flash controller and directly interacts with data via SPI.
- Internal flash supports both single SPI and QSPI modes, while external flash only supports single SPI mode.	
	
Function List
--------------

1. :ref:`initialize flash <init_flash>` -- Initializing Internal or external flash memory。
2. :ref:`read data <read_data>` -- Reading data from flash
3. :ref:`write data <write_data>` -- Writing Data to Flash
4. :ref:`erase data <erase_data>` -- Erasing Data from Flash
5. :ref:`get information <get_info>` -- Obtaining Basic Flash Information
6. :ref:`add internal flash <add_int_flash>` -- Adding Internal Flash
7. :ref:`add external flash <add_ext_flash>` -- Adding External Flash
8. :ref:`notice <note>` -- Precautions



.. _init_flash:

Initializing Flash 
--------------------------

WMSDK supports the coexistence of one internal flash and multiple external flashes on a single chip.
  
Initialization of Internal Flash:
  - To initialize the internal flash, call the flash driver API and pass in the name of the internal flash. 
  - Example: wm_drv_flash_init("iflash"). Upon successful initialization, it returns the device pointer of the internal flash; otherwise, it returns NULL.
  - The initialization of the internal flash is typically called by the system during boot, so users generally do not need to call the initialization operation again.
  - If the internal flash has already been initialized, calling the function again will still return the device pointer of the internal flash normally.
  - Users can also obtain the device pointer of the internal flash by calling wm_dt_get_device_by_name("iflash") and check whether it has been initialized by verifying if the device's state is equal to WM_DEV_ST_INITED.
  - The relevant parameters of the internal flash can be configured in the device table using the configuration structure: wm_dt_hw_iflash_t

Initialization of External Flash:
  - Before initializing an external flash, ensure that an external flash has been connected via SPI and that the driver supports this flash. Refer to :ref:`add external flash <add_ext_flash>` for more information.
  - Since WMSDK supports multiple external flashes, you need to specify the name of the external flash during initialization.
  - Assuming there is an external flash named W25Q, its initialization would be: wm_drv_flash_init("W25Q"). Upon successful execution, it returns the device pointer of the external flash; otherwise, it returns NULL.
  - Each flash has a corresponding device pointer. When performing operations on the flash, you need to pass in the corresponding device pointer so that the driver knows which flash to access.
  - The initialization parameters of the external flash are also configured in the device table using the corresponding structure: wm_dt_hw_eflash_t
      
.. _read_data:



Reading from Flash 
---------------------------

The API for reading from flash memory is wm_drv_flash_read, which supports reading content of any length from any region of the flash.
   
Example code:
    
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
		
Writing to Flash
----------------

Supports two writing methods: with erase and without erase; supports writing at any position in the flash and writing any length.
       
   - Writing with Erase: wm_drv_flash_write_with_erase
   - Writing without Erase：wm_drv_flash_write
	   
Example Code:
    
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


Erase FLASH Data
------------------------
Supports three erasing methods:

   - Erasing any region: wm_drv_flash_erase_region
   - Erasing by sector:wm_drv_flash_erase_sector
   - Erasing the entire flash: wm_drv_flash_erase_chip

Example Code:
    
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
	
Obtain basic information about Flash
---------------------------------------------	
	
Driver Interface:  wm_drv_flash_get_device_info.Currently, the provided information includes: total flash size, size per page, size per sector, manufacturer ID, and device ID.
	
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
	
	
How to Add Internal Flash	
---------------------------
Different chips may have different internal flash models. If the system does not support a specific model by default, users need to add the corresponding internal flash manually.
For internal flash, only one can exist in the system. That is, during system operation, the software will only operate on one flash, and it does not support the simultaneous presence of two or more internal flashes.
For chips like W802, external flash controlled by the flash controller also follows the internal flash process. To add a new flash model, refer to the internal flash addition process.
	
	
Adding internal flash is relatively simple, with the following steps:
    - Connect the corresponding internal flash model hardware.
    - Add a wm_flash_t structure instance in wm_hal_iflash_chips.h.
    - Add the newly added structure instance to the wm_iflash_support_chips[] array.
    - Refer to the currently supported internal flash models, such as wm_iflash_chip_p25q, wm_iflash_chip_th25q, etc.
	
.. _add_ext_flash:	

How to Add External Flash	
---------------------------	
External flash needs to be added by users according to their own needs. Here, SPI-connected external flash is accepted. The external flash acts as an SPI slave device and is mounted on the SPI bus.

Flash Wiring
    - SPI wiring can refer to the SPI Master configuration.
    - Currently, the SPI module has fixed three PINs, which can be modified in the device table (wm_dt_hw_spim_t).
    - The flash module needs to determine the CS PIN used. Currently, any unused general-purpose GPIO can be used as CS, which can be determined in the device table (wm_dt_hw_eflash_t).
    
Driver Addition
    - In the device table, add the relevant information for the corresponding external flash by adding an instance to the wm_dt_hw_eflash_t structure and filling in the relevant information. Refer to dt_hw_eflash_w25q.
    - Add the instantiated flash to the device entry (dt_hw_table_entry). Refer to dt_hw_eflash_w25q.
    - In the driver file (components/drivers/src/ops/w80x/wm_drv_eflash_chips.h), add a new wm_eflash_t instance and fill in the relevant information. Refer to wm_external_flash_chip_p25q.
    - Add the newly added wm_eflash_t instance to the data group wm_external_flash_chips. Refer to wm_external_flash_chip_p25q.
	
After completing the above two operations, the external flash can be accessed through the flash driver API.


.. _note:	
	
Notes
-------------

.. note::
  
    1. When operating the flash, only the flash address needs to be entered, without adding the platform base address.
    2. If the flash is controlled through the flash controller, the maximum supported space is 16 MB. If controlled via the SPI method, there is no space size limit.
	3. During the wm_drv_flash_write_with_erase process, do not power off or restart the board, as there is a risk of flash data loss.
	4. Internal flash has a limitation that a page can only be written 64 times after being erased. If the write count reaches 64, it needs to be erased again to ensure data correctness.
	5. Modifying certain areas of internal flash may cause system anomalies. Before operation, check the partition table and then operate the corresponding areas. For specific partition table operations, refer to :ref:` Partition Table <partition_table>` 
	6. The first 8K of internal flash is system data protected. Users have read permissions but no write or erase permissions.


Application Example
--------------------------

For a basic example of using FLASH, please refer to:ref:`examples/peripheral/flash<peripheral_example>`


API Reference
----------------
:ref:`label_api_flash_drver`

