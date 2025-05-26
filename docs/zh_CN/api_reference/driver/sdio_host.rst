.. _sdio_host_api:

===================
SDIO HOST API 参考
===================

该模块分为两个部分 API, 包括 SDH SDMMC 和 SDH SPI

SDH SDMMC
================

SDH SDMMC 头文件
----------------------

- wm_iot_sdk/components/driver/include/wm_drv_sdh_sdmmc.h
- This header file can be included with:

.. code-block:: c
   :emphasize-lines: 1

   #include "wm_drv_sdh_sdmmc.h"

SDH SDMMC 宏定义
----------------------

.. doxygengroup:: WM_DRV_SDH_SDMMC_Macros
    :project: wm-iot-sdk-apis
    :content-only:
    :members:

SDH SDMMC 枚举
----------------------

.. doxygengroup:: WM_DRV_SDH_SDMMC_Enumerations
    :project: wm-iot-sdk-apis
    :content-only:
    :members:

SDH SDMMC 结构体
----------------------

.. doxygengroup:: WM_DRV_SDH_SDMMC_Structures
    :project: wm-iot-sdk-apis
    :content-only:
    :members:

SDH SDMMC 接口
----------------------

.. doxygengroup:: WM_DRV_SDH_SDMMC_Functions
    :project: wm-iot-sdk-apis
    :content-only:

SDH SPI
================

SDH SPI 头文件
----------------------

- wm_iot_sdk/components/driver/include/wm_drv_sdh_spi.h
- This header file can be included with:

.. code-block:: c
   :emphasize-lines: 1

   #include "wm_drv_sdh_spi.h"

SDH SPI 接口
----------------------

.. doxygengroup:: WM_DRV_SDH_SPI_Functions
    :project: wm-iot-sdk-apis
    :content-only: