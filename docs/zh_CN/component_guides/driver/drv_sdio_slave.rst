.. _drv_sdio_slave:

SDIO Slave
=============

简介
-------------

SDIO Slave 模块提供了作为 SDIO 从设备与主机进行通信的功能。

- **SDIO Slave:** 是一种用于嵌入式系统的接口，它允许设备作为从设备通过 Secure Digital Input Output (SDIO) 接口与主机进行通信
- **Zero Copy:** 数据传输采用 zero copy 方式,减少内存拷贝开销,提高传输效率

功能列表
-------------

- 支持 Function 0 (配置和控制功能) 和 Function 1 (数据传输功能)
- 支持 CIS 寄存器读写
- 支持同步和异步数据传输
- 支持命令接收功能
- 支持事件回调机制

功能概述
-------------

SDIO Slave 主要功能包括:

- **CIS 寄存器访问:**
  - 支持读写 Function 0 和 Function 1 的 CIS 寄存器
  - 每个 Function 最多支持 128 字节的 CIS 数据

- **数据传输:**
  - 同步传输: 阻塞等待传输完成
  - 异步传输: 立即返回，通过回调通知完成
  - 最大传输缓冲区为 8192 字节(TX)和 4096 字节(RX)
  - 采用 zero copy 方式,应用层需要保证数据在传输完成前保持有效

- **命令处理:**
  - 支持接收主机发送的命令
  - 命令缓冲区大小为 256 字节

- **事件回调:**
  - 支持注册回调函数处理数据发送/接收完成事件
  - 支持命令发送/接收完成事件通知

硬件配置
-------------

通过设备树(DT)配置以下参数:

- 引脚复用配置
- 中断配置

主要功能
-------------

CIS 寄存器配置
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

**起始条件:**

- SDIO Slave 设备已初始化

**相关API:**

- 调用 ``wm_drv_sdio_slave_read_cis_reg`` 读取 CIS 寄存器
- 调用 ``wm_drv_sdio_slave_write_cis_reg`` 写入 CIS 寄存器

**参数说明:**

- func: 功能号(0-1)
- pos: 寄存器地址(0-123)
- val: 寄存器值(32位)

**结果:**

- 读取成功返回寄存器值
- 写入成功返回 ``WM_ERR_SUCCESS``
- 失败返回对应错误码

注意事项
-------------

- 每个 Function 最多支持 128 字节的 CIS 数据
- 需根据 SDIO 卡的用途来配置 CIS 寄存器，可参照 :ref:`examples/peripheral/sdio_slave<peripheral_example>` 下的 ``cis.c`` 文件
- CIS 寄存器配置需要在初始化阶段完成，运行时不建议修改
- CIS 数据包含卡的制造商信息、功能描述、最大时钟频率等重要参数
- 错误的 CIS 配置可能导致主机无法正确识别从机设备

数据传输
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

**起始条件:**

- SDIO Slave 设备已初始化
- 已注册事件回调函数

**相关API:**

- 调用 ``wm_drv_sdio_slave_init`` 初始化设备
- 调用 ``wm_drv_sdio_slave_tx_data_sync`` 进行同步数据发送
- 调用 ``wm_drv_sdio_slave_tx_data_async`` 进行异步数据发送
- 调用 ``wm_drv_sdio_slave_rx_data`` 接收数据

**结果:**

- 成功返回 ``WM_ERR_SUCCESS``
- 失败返回对应错误码

命令处理
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

**起始条件:**

- SDIO Slave 设备已初始化
- 已注册事件回调函数

**相关API:**

- 调用 ``wm_drv_sdio_slave_rx_cmd`` 接收命令

**结果:**

- 成功返回 ``WM_ERR_SUCCESS`` 并通过回调通知
- 失败返回对应错误码

注意事项
-------------

- 传输数据时，缓冲区必须使用 ``wm_heap_caps_alloc()`` 分配，并设置 ``WM_HEAP_CAP_SHARED`` 标志
- 发送数据最大长度为 8192 字节
- 接收数据最大长度为 4096 字节
- 命令接收缓冲区大小为 256 字节
- 回调函数在中断上下文执行，应尽量简短
- 由于采用 zero copy 方式传输数据,应用层需要确保:

  - 发送数据在传输完成前不能释放或修改
  - 接收到的数据需要及时处理

应用实例
------------------

    使用 SDIO Slave 基本示例请参照 :ref:`examples/peripheral/sdio_slave<peripheral_example>`

API参考
-------------
:ref:`API参考 <sdio_slave_api>`
