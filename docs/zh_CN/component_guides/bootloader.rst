
.. _Bootloader:

Bootloader
==============

引言
--------

概述
~~~~~~~~

本文档对 bootloader 功能及使用说明进行简单描述，供开发者和设计者理解 bootloader 功能。

术语定义
~~~~~~~~~~

.. list-table:: 
   :widths: 25 25 
   :header-rows: 0
   :align: center

   * - 术语
     - 定义

   * - CRC
     - Cyclic Redundancy Check

   * - IMAGE
     - Binary File

   * - MAC
     - Medium Access Control

   * - QFLASH
     - Quad-SPI Flash

   * - RAM
     - Read-Write Memory

   * - ROM 
     - Read-Only Memory

   * - UART
     - Universal asynchronous receiver-transmitter 

   * - MB
     - Mega Byte

   * - KB
     - Kilo Byte


Bootloader 流程图
--------------------

.. figure:: ../../_static/component-guides/bootloader/bootloader.svg
    :align: center
    :alt: bootloader 流程图

Bootloader 功能
----------------

Bootloader 功能说明
~~~~~~~~~~~~~~~~~~~~~~~~

IMAGE 启动部分
^^^^^^^^^^^^^^^

QFlash 工作模式切换
""""""""""""""""""""""

针对未知的 Flash 厂商，当从 ROM 跳转到 QFlash 时，需要切换 QFlash 的工作模式：从 1 线切换到 4 线模式。

IMAGE 完整性校验
""""""""""""""""""""""""""

(1) 对 IMAGE 头进行 CRC 校验，IMAGE 的完整内容进行 CRC 校验。
(2) 如果 wm.py 编译出来的 IMAGE 是被签名的，将会对 IMAGE 的完整内容进行 RSA 签名验证。

代码解压
"""""""""""

针对客户为了节省空间对代码进行压缩的情况，依据使用的压缩算法对代码进行解压。
需要考虑使用那些压缩解压缩算法，及压缩率。
仅支持 XZ 算法。

代码搬移
""""""""""""""

解压后的代码需要放置到指定的运行位置，依据 IMAGE 的 header 信息去确定。

代码解密设置
""""""""""""""

针对代码是加密的情况，需要配置代码的解密信息。

代码跳转运行
""""""""""""""

代码搬移后，确定代码的复位向量地址，从 bootloader 跳转到用户的 image 去运行。
