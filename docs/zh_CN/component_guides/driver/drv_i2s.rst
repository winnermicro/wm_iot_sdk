:orphan:

.. _i2s:

I2S
=============

简介
-------------

I2S（Inter-IC Sound）是一种串行、同步通信协议，通常用于传输两个数字音频设备之间的音频数据。本模块支持多种数据格式和通信标准，适用于多种音频传输场景。

功能列表
-------------

- 支持多种接口:I2S 接口、PCM 接口。
- 支持主/从模式。
- 过零检测。
- 支持 FIFO 功能。
- 支持采样位宽 8/16/24/32 bits。
- 支持立体声音频传输。
- 支持多种数据格式，包括 I2S、MSB Justified、PCM A/B。
- 支持中断和 DMA。
- 静音功能。

功能概述
-------------

.. figure:: ../../../_static/component-guides/driver/I2S.drawio.svg
    :align: center

.. note::
   上图中分别是发送和接收过程，在收发双方时钟信号不兼容情况下，用外部 CONTROLLER 来提供同步时钟供双方使用

.. figure:: ../../../_static/component-guides/driver/I2Sinterface.drawio.svg
    :align: center

**模块支持多种功能选择**

**模式选择**

- master、slave

**传输方向**

- 输入、输出、双工

**协议标准**

- I2S、MSB、PCMA、PCMB

**帧格式**

每个通道位宽为8bit，支持的帧格式：

- 8/16/24/32 bits

**通道类型**

- 立体声（双声道）、单声道


**采样率**

- 8KHz、11.025KHz、16KHz、22.05KHz、24KHz、32KHz、44.1KHz、47.25KHz、48KHz、50KHz、50.04KHz、96KHz、192KHz

使用配置
-------------

参数配置
^^^^^^^^^^^^^

调用结构体 ``wm_drv_i2s_cfg_t`` 来配置

.. code:: C

    typedef struct {
        enum wm_i2s_mode mode;
        enum wm_i2s_dir dir;
        enum wm_i2s_std std;
        enum wm_i2s_fmt fmt;
        enum wm_i2s_chan_type ctype;
        enum wm_i2s_xfer_type xtype;
        uint32_t sample_rate_hz;
        uint32_t rx_pkt_size;
        uint32_t rx_ready_th;
        uint32_t tx_ready_th;
    } wm_drv_i2s_cfg_t;


**mode**

| 工作模式：主模式、从模式

**dir**

| 传输方向：输入、输出、双工

**std**

| 协议标准：I2S、MSB、PCMA、PCMB

**fmt**

| 帧格式标准：8/16/24/32 bits

.. note::
    每个通道位宽为8bit

**ctype**

| 通道类型：立体声（双声道）、单声道

**xtype**

| 数据传输类型：当前仅支持 DMA 方式

**sample_rate_hz**

| 采样率：8KHz、11.025KHz、16KHz、22.05KHz、24KHz、32KHz、44.1KHz、47.25KHz、48KHz、50KHz、50.04KHz、96KHz、192KHz

**rx_pkt_size**

| 作为输入时，每一包数据最大长度

**rx_ready_th**

| 作为输入时 driver 层缓冲阀值，即缓存数据大于该阀值时才会将缓存的报文串链后调用用户注册的 rxready callback 递交给用户

**tx_ready_th**

| 作为输出时 driver 层缓冲阀值，即缓存数据大于该阀值时才会真正开始发送


.. figure:: ../../../_static/component-guides/driver/I2Streshold.drawio.svg
    :align: center

.. note::
   rx/tx treshold设置原则：内存越大，延迟要求越低，可以设置的值越大，从而可以兼容的链路抖动（出现数据阻塞，突发）情况


**用户可以动态调用IOCTL API进行修改的参数**

.. code:: C

    typedef struct {
        enum wm_drv_i2s_ioctl_cmd cmd;
        bool tx; /* tx/rx select for this setting */
        union {
            bool mclk_en; /**< true if enable mclk */
            bool mute;    /**< true if enable mute */
            bool inverse; /**< true if enable clock inverse */
            bool lzc;     /**< true if enable left zero cross check */
            bool rzc;     /**< true if enable right zero cross check */
            bool left;    /**< if set to left channel when work in mono mode */
        } u;
    } wm_drv_i2s_ioctl_args_t;


**cmd**

| 指定要执行的 IOCTL 命令，包括：

- ``WM_DRV_I2S_CMD_SET_MUTE``: 启用或禁用 TX 或 RX 的静音功能。当启用时，相应的通道将不会传输或接收音频数据
- ``WM_DRV_I2S_CMD_SET_INVERSE``: 设置时钟相位为反转模式。这通常用于与某些特定硬件的兼容性，以确保时钟信号的相位正确
- ``WM_DRV_I2S_CMD_CLS_FIFO``: 清除 TX 或 RX 的 FIFO 缓冲区。这有助于在需要时重置或清除缓冲区中的数据
- ``WM_DRV_I2S_CMD_SET_MONO_CH``: 在单声道模式下设置使用的通道。这允许选择是使用左声道还是右声道的数据
- ``WM_DRV_I2S_CMD_SET_LZC``: 启用或禁用左声道的零点交叉检查。零点交叉检查是一种技术，用于确保音频信号在交叉零点时的相位连续性
- ``WM_DRV_I2S_CMD_SET_RZC``: 启用或禁用右声道的零点交叉检查，与左声道类似

**tx**

| 指示该设置是应用于 TX（发送）还是 RX（接收）

**u**

| 根据不同的 cmd 值，可以存储不同的布尔值参数：

- mclk_en: 当 cmd 需要启用或禁用主时钟时使用
- mute: 当 cmd 是设置静音时使用
- inverse: 当 cmd 是设置时钟相位反转时使用
- lzc: 当 cmd 是启用或禁用左声道零点交叉检查时使用
- rzc: 当 cmd 是启用或禁用右声道零点交叉检查时使用
- left: 当 cmd 是设置单声道模式下的通道时使用，选择是否使用左声道

DMA 配置
^^^^^^^^^^^^^

该模块利用 DMA 来实现高效收发，即 I2S 模块会依赖 DMA 的 HW,故需要 DMA 有足够的可用的 channel(2S 每个方向占用一个 DMA channel)

时钟配置
^^^^^^^^^^^^^

I2S 中包含多种时钟：

- BCLK（串行时钟，也叫位时钟）：对应数字音频的每一位数据。
- WS/LRCLK（字段选择，用于选择左右声道）：为 “0” 表示正在传输的是左声道的数据，为“1”表示正在传输的是右声道的数据。
- SD（串行数据）：用二进制补码来表示音频数据（数据传输从高位到低位）。

| I2S 的时钟上级是 CLK_PERI，该时钟由 DPLL 输出的 480MHz 分频得出
| 正常工作情况下应固定为 3 分频，即 CLK_PERI 时钟为 160MHz。
| 由此时钟再进行分频可得到 80MHz 与 40MHz，分别给到加密模块与接口模块。

主要功能
-------------

发送数据功能
^^^^^^^^^^^^^

**起始条件:**

- I2S 模块初始化

**相关时序API:**

- 调用 ``wm_drv_i2s_write_async`` 异步发送I2S数据，返回成功表示已经被 I2S 驱动成功处理，否则需要用户释放
- 根据需要调用 ``wm_drv_i2s_write_pause`` 暂停 I2S 设备的写操作，此接口不会复位 HW，在 resume 可以快速进行发送
- 根据需要调用 ``wm_drv_i2s_write_resume`` 恢复 I2S 设备的写操作，重新开始异步发送数据
- 根据需要调用 ``wm_drv_i2s_write_stop`` 停止 I2S 设备的写操作
- 根据需要调用 ``wm_drv_i2s_register_write_cb`` 用于注册回调函数，以便在异步写操作成功完成时被调用

**结果:**

- 成功返回 ``WM_ERR_SUCCESS`` ,失败返回对应错误码

接收数据功能
^^^^^^^^^^^^^

**起始条件:**

- I2S 模块初始化

**相关时序API:**

- 调用 ``wm_drv_i2s_read_async`` 异步从 I2S 设备读取数据，返回成功表示已经被 I2S 驱动成功处理，否则需要用户释放
- 根据需要调用 ``wm_drv_i2s_read_pause`` 暂停 I2S 设备的写操作，此接口不会复位 HW，在 resume 可以快速进行接收
- 根据需要调用 ``wm_drv_i2s_read_resume`` 恢复 I2S 设备的读操作，重新开始异步接收数据
- 根据需要调用 ``wm_drv_i2s_read_stop`` 停止 I2S 设备的读操作
- 根据需要调用 ``wm_drv_i2s_register_read_cb`` 用于注册回调函数，以便在异步读操作成功完成时被调用



**结果:**

- 成功返回 ``WM_ERR_SUCCESS`` ,失败返回对应错误码

I2S设备查询功能
^^^^^^^^^^^^^^^^^

**起始条件:**

- I2S 模块初始化

**相关时序API:**

- 调用 ``wm_drv_i2s_dump_cfgs`` 查询I2S设备的配置信息并打印

**结果:**

- 返回设备信息

注意事项
-------------

| I2S 是一个流式设备，对数据流稳定性和时延有较高要求。在实现场景中，要求数据的粒度可调，以适应不同链路的需求

应用实例
-------------

.. code:: C

    int main(void)
    {
        int ret;
        uint8_t *buf;
        wm_drv_i2s_cfg_t cfg;

        memset(&cfg, 0x0, sizeof(cfg));

        cfg.mode           = WM_I2S_MODE_MASTER;
        cfg.dir            = WM_I2S_DIR_OUT;
        cfg.std            = WM_I2S_STD_I2S;
        cfg.fmt            = WM_I2S_FMT_16BIT;
        cfg.ctype          = WM_I2S_CHAN_TYPE_STEREO;
        cfg.xtype          = WM_I2S_XFER_DMA;
        cfg.sample_rate_hz = 8000;

        wm_device_t *i2s_device = wm_drv_i2s_init("i2s", &cfg);
        if (i2s_device == NULL) {
            wm_log_error("I2S driver init Failed!\n");
            return WM_ERR_FAILED;
        }

        wm_drv_i2s_register_write_cb(i2s_device, txdone_cb);
        wm_drv_i2s_register_read_cb(i2s_device, rxready_cb);

        buf = calloc(1, block_size);

        ret = wm_drv_i2s_write_async(i2s_device, (void *)buf, block_size);
        if (ret != WM_ERR_SUCCESS) {
            wm_log_error("I2S driver init Failed!\n");
            free_buf(buf);
            return WM_ERR_FAILED;
        }

        while (send_pkts == 0) {
            vTaskDelay(pdMS_TO_TICKS(1000));
        }

        wm_log_info("I2S send 1 pkt done!\n");

        ret = wm_drv_i2s_write_stop(i2s_device);
        if (ret != WM_ERR_SUCCESS) {
            wm_log_error("I2S driver stop Failed!\n");
            free_buf(buf);
            return WM_ERR_FAILED;
        }

        ret = wm_drv_i2s_deinit(i2s_device);
        if (ret != WM_ERR_SUCCESS) {
            wm_log_error("I2S driver deinit Failed!\n");
            free_buf(buf);
            return WM_ERR_FAILED;
        }

        return WM_ERR_SUCCESS;
    }


API参考
-------------

    查找TIMER相关API请参考：

    :ref:`label_api_i2s`
