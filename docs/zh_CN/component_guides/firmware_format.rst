
.. _firmware_format:

固件格式
=====================================

固件可分为单个 .img 文件 和 多个 .img 文件组成的 .fls文件：

- .img 文件为最小的烧录单元，一个 .img 文件可以被烧录至 flash 的某一地址

- .fls 为完整固件包，由多个 .img 文件平铺组成，并不要求 .img 之间的顺序

一般而言，烧录时推荐使用 .fls 文件，使用简单。

.img 文件格式
--------------

Image 由 Header, Body 和数字签名三部分组成（如图）。

.. figure:: ../../_static/component-guides/firmware_format/image_format.png
    :align: center
    :alt: 图3

Image Header
>>>>>>>>>>>>>>>>>>>>>>>>

    W800 Image Header 包含信息：魔术字，Image 属性，Image 启动地址，Image 长度，Image Header 头位置，Image 升级地址，Image CRC 校验，Image 解密信息，数字签名，压缩信息。

.. figure:: ../../_static/component-guides/firmware_format/image_header.png
    :align: center
    :alt: 图3-1

Image Header 各字段描述
::::::::::::::::::::::::::::::::::

.. list-table::
   :widths: 20 30
   :align: center

   * - 字段
     - 描述

   * - magic_no
     - 魔术字，固定值 0xA0FFFF9F

   * - img_attr
     - Img_Attr_Type，IMAGE Attribute

   * - img_addr
     - Image area 在 flash 中的运行位置

   * - img_len
     - Image area 的字节数长度

   * - img_header_addr
     - IMAGE header 在 flash 中的位置

   * - upgrade_img_addr
     - 升级区地址，升级 IMAGE header 在 flash 中存放位置

   * - org_checksum
     - Image body 的 crc32 结果

   * - upd_no
     - 升级版本号，值较大的表示版本较新；
       当版本号为 0xFFFFFFFF 时，可升级任意版本号固件

   * - ver
     - Image 版本号，字符串

   * - next
     - 下一个 image header 在 flash 中的位置（可选）

   * - hd_checksum
     - Image header 的以上字段的 crc32 的值

Image Attribute
::::::::::::::::::::::::::::::::::

.. list-table:: 
   :widths: 10 10 50
   :align: center

   * - 字段
     - Bit
     - 描述

   * - img_type
     - 4
     - 0x0：Bootloader；
       0x1：User Image；
       0xE：ft 测试程序；
       其它值：用户自定义

   * - code_encrypt
     - 1
     - 0：固件明文存储；
       1：固件由客户加密后存储

   * - pricey_sel
     - 3
     - 芯片内置 8 组 RSA 私钥用于解密固件加密的秘钥，
       用户可任选一组使用，取值范围 0~7

   * - signature
     - 1
     - 0：IMAGE 不包含签名部分；
       1：IMAGE 包含 128bytes 签名

   * - reserved
     - 7
     - 保留

   * - gzip_enable
     - 1
     - 0：不使能 GZIP 压缩；
       1：image area 部分为 GZIP 压缩档；（当前不支持 GZIP，仅支持压缩率更高的 XZ）

   * - erase_block_en
     - 1
     - 0：不支持 64KB Block 擦除；
       1：支持 Block 擦除

   * - erase_always
     - 1
     - 0：Sector 或 Block 擦除前检查 flash 是否全 F，
       全 F 的 Sector 或 Block 不进行擦除操作；
       1：总是先擦后写

   * - compress_type
     - 2
     - 0：不压缩
       1：image area 部分为 XZ 压缩档；（目前仅支持压缩率更高的 XZ）
   
   * - reserved
     - 10
     - 保留


Image Body
>>>>>>>>>>>>>>>>>>>>>>>>

.. list-table:: 
   :widths: 30 30 30 30
   :align: center

   * - 加密
     - 压缩
     - Image Body 内容 
     - 用途

   * - X
     - X
     - 原始 Image 内容
     - Bootloader，User Image

   * - X
     - √
     - 原始 Image 压缩后的内容
     - User Image

   * - √
     - X
     - 原始 Image 加密的内容+加密信息
     - Bootloader

   * - √
     - √
     - 原始 Image 加密后压缩的内容+加密信息
     - User Image

**数字签名**
>>>>>>>>>>>>>>>>>>

    如果 Image Header 的属性里的 signature 被置位了，则说明固件带有数字签名。
    
    数字签名是针对 Image Header 和 Image Body 组成的文件。


.fls 文件格式
--------------

.. figure:: ../../_static/component-guides/firmware_format/fls_format.png
    :align: center
    :width: 40% 
    :alt: 图5-1

SoC 会依据 Header 来区分当前烧录的位置。
