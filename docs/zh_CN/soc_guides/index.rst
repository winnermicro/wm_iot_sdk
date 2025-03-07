SoC 指南
==============

.. hint::
   当前系统不支持自动探测芯片型号。用户在编译前需要通过 menuconfig 选择正确的芯片型号。如果所选型号与实际烧录的设备不匹配，可能会导致引脚复用等硬件功能异常。系统烧录完成后，在启动时会显示编译配置的芯片型号。

芯片比较
--------------

.. raw:: html

   <table class="docutils align-center">
   <thead>
   <tr class="row-odd">
   <th class="head wm-mid-padding"><p>SoC</p></th>
   <th class="head wm-mid-padding"><p>特点</p></th>
   <th class="head wm-mid-padding"><p>封装</p></th>
   <th class="head wm-mid-padding"><p>Flash</p></th>
   <th class="head wm-mid-padding"><p>触摸传感器</p></th>
   <th class="head wm-mid-padding"><p>唤醒引脚</p></th>
   <th class="head wm-mid-padding"><p>可用引脚数量</p></th>
   <th class="head wm-mid-padding"><p>参考资料</p></th>
   </tr>
   </thead>
   <tbody>
   <tr class="row-even">
   <td class="wm-mid-padding"><p>W800</p></td>
   <td class="wm-mid-padding"><p>MCU+WiFi+BLE</p></td>
   <td class="wm-mid-padding"><p>QFN32(4*4mm)</p></td>
   <td class="wm-mid-padding"><p>2 MBytes</p></td>
   <td class="wm-mid-padding"><p>无</p></td>
   <td class="wm-mid-padding"><p>有</p></td>
   <td class="wm-mid-padding"><p>18</p></td>
   <td class="wm-mid-padding"><p>
      <a href="../../../../download/soc_pdf/w800/chip_specification.pdf">芯片规格书</a><br>
      <a href="../../../../download/soc_pdf/w800/chip_design_guide.pdf">芯片设计指导</a><br>
      <a href="../../../../download/soc_pdf/w800/hardware_design_guide.zip">硬件参考设计</a><br><br>
      <a href="../../../../download/soc_pdf/w800/register_manual.pdf">芯片寄存器手册</a></p></td>
   </tr>
   <tr class="row-odd">
   <td class="wm-mid-padding"><p>W801S</p></td>
   <td class="wm-mid-padding"><p>MCU+WiFi+BLE</p></td>
   <td class="wm-mid-padding"><p>QFN56(6*6mm)</p></td>
   <td class="wm-mid-padding"><p>2 MBytes</p></td>
   <td class="wm-mid-padding"><p>有</p></td>
   <td class="wm-mid-padding"><p>有</p></td>
   <td class="wm-mid-padding"><p>43</p></td>
   <td class="wm-mid-padding"><p>
      <a href="../../../../download/soc_pdf/w801s/chip_specification.pdf">芯片规格书</a><br>
      <a href="../../../../download/soc_pdf/w801s/hardware_design_guide.zip">硬件参考设计</a><br><br>
   </tr>
   <tr class="row-even">
   <td class="wm-mid-padding"><p>W802</p></td>
   <td class="wm-mid-padding"><p>MCU+WiFi+BLE</p></td>
   <td class="wm-mid-padding">QFN56(6*6mm)</p></td>
   <td class="wm-mid-padding"><p>无</p></td>
   <td class="wm-mid-padding"><p>有</p></td>
   <td class="wm-mid-padding"><p>有</p></td>
   <td class="wm-mid-padding"><p>37</p></td>
   <td class="wm-mid-padding"><p>
      <a href="../../../../download/soc_pdf/w802/chip_specification.pdf">芯片规格书</a><br>
      <a href="../../../../download/soc_pdf/w802/hardware_design_guide.zip">硬件参考设计</a><br><br>
   </tr>
   <tr class="row-odd">
   <td class="wm-mid-padding"><p>W803</p></td>
   <td class="wm-mid-padding"><p>MCU+WiFi+BLE</p></td>
   <td class="wm-mid-padding"><p>QFN32(4*4mm)</p></td>
   <td class="wm-mid-padding"><p>2 MBytes</p></td>
   <td class="wm-mid-padding"><p>有</p></td>
   <td class="wm-mid-padding"><p>有</p></td>
   <td class="wm-mid-padding"><p>20</p></td>
   <td class="wm-mid-padding"><p>
      <a href="../../../../download/soc_pdf/w803/chip_specification.pdf">芯片规格书</a><br>
      <a href="../../../../download/soc_pdf/w803/chip_design_guide.pdf">芯片设计指导</a><br>
      <a href="../../../../download/soc_pdf/w803/hardware_design_guide.zip">硬件参考设计</a><br><br>
   </tr>
   <tr class="row-even">
   <td class="wm-mid-padding"><p>W805</p></td>
   <td class="wm-mid-padding"><p>MCU（无射频）</p></td>
   <td class="wm-mid-padding"><p>QFN32(4*4mm)</p></td>
   <td class="wm-mid-padding"><p>1 MBytes</p></td>
   <td class="wm-mid-padding"><p>无</p></td>
   <td class="wm-mid-padding"><p>有</p></td>
   <td class="wm-mid-padding"><p>18</p></td>
   <td class="wm-mid-padding"><p>
      <a href="../../../../download/soc_pdf/w805/chip_specification.pdf">芯片规格书</a><br>
      <a href="../../../../download/soc_pdf/w805/chip_design_guide.pdf">芯片设计指导</a><br>
   </tr>
   <tr class="row-odd">
   <td class="wm-mid-padding"><p>W806</p></td>
   <td class="wm-mid-padding"><p>MCU（无射频）</p></td>
   <td class="wm-mid-padding"><p>QFN56(6*6mm)</p></td>
   <td class="wm-mid-padding"><p>1 MBytes</p></td>
   <td class="wm-mid-padding"><p>有</p></td>
   <td class="wm-mid-padding"><p>有</p></td>
   <td class="wm-mid-padding"><p>43</p></td>
   <td class="wm-mid-padding"><p>
      <a href="../../../../download/soc_pdf/w806/chip_specification.pdf">芯片规格书</a><br>
      <a href="../../../../download/soc_pdf/w806/chip_design_guide.pdf">芯片设计指导</a><br>
   </tr>
   </tbody>
   </table>


芯片详细差异比较
---------------------

.. list-table::
   :widths: 25 50 25 25 25 25 25 25
   :header-rows: 0
   :align: center

   * - 芯片功能
     - 特性
     - W800
     - W801S
     - W802
     - W803
     - W805
     - W806

   * - 芯片封装
     - ——
     - QFN32 4*4mm
     - QFN56 6*6mm
     - QFN56 6*6mm
     - QFN32 4*4mm
     - QFN32 4*4mm
     - QFN56 6*6mm

   * - SDIO HOST 控制器
     - 兼容SD卡规范1.0/1.1/2.0(SDHC)

       兼容SDIO内存卡规范1.1.0

       兼容 MMC 规范 2.0~4.2 11 

       可配置接口时钟速率，支持主机速率 0~50MHz

       支持标准 MMC 接口 

       支持最大 1024 字节的 Block 

       支持软复位功能 

       自动 Command/Response CRC 生成/校验

       自动数据 CRC 生成/校验

       可配置 timeout 检测 

       支持 SPI、1 比特 SD 和 4 比特 SD 模式

       支持 DMA 数据传输
     - √
     - √
     - √
     - √
     - √
     - √


   * - SDIO Device 控制器
     - 兼容 SDIO 卡规范 2.0 

       支持主机速率 0~50MHz  

       支持最大 1024 字节的 Block

       支持软复位功能

       支持 SPI、1 比特 SD 和 4 比特 SD 模式
     - √
     - √
     - √
     - √
     - √
     - √

   * - 高速 SPI 设备控制器
     - 兼容通用SPI协议

       可选择的电平中断信号

       最高支持50Mbps速率

       简单的帧格式、全硬件解析与DMA
     - √
     - √
     - √
     - √
     - √
     - √    

   * - DMA 控制器
     - Amba2.0 标准总线接口,7 路 DMA 通道

       支持基于存储器链表结构的 DMA 操作

       软件配置 16 个硬件请求源

       支持 1,4-burst 操作模式

       支持 byte、half-word,word 操作 

       源、目的地址不变或顺序递增可配置或在预定义地址范围内循环操作

       同步 DMA 请求和 DMA 响应硬件接口时序
     - √
     - √
     - √
     - √
     - √
     - √    

   * - 时钟与复位
     - ——
     - √
     - √
     - √
     - √
     - √
     - √ 

   * - 内存管理器
     - ——
     - √
     - √
     - √
     - √
     - √
     - √ 

   * - WiFi
     - 数据速率:1~54Mpbs(802.11a/b/g), 6.5~150Mbps(802.11n)

       MCS 格式:MCS0~MCS7,MCS32(40MHz HT Duplicate 模式)

       支持 40MHz 带宽 non-HT Duplicate 模式,6M~54M

       信号带宽:20MHz, 40MHz

       调制方式:DSSS(DBPSK,DQPSK,CCK)和 OFDM(BPSK,QPSK,16QAM,64QAM)

       实现 1T1R 的 MIMO-OFDM spatial multiplexing

       支持 Short GI 模式

       支持 legacy 模式与 Mixed 模式

       支持 40MHz 带宽下对 20M 上下边带信号的发射接收

       支持 MCS0~7、32 的 STBC 接收

       支持 Green Field 模式

       支持 EDCA 信道接入方式

       支持 CSMA/CA,NAV 与 TXOP 保护机制

       Beacon、Mng、VO、VI、BE、BK 五路发送队列与 QoS

       支持单、广组播帧接收发送

       支持 RTS/CTS,CTS2SELF,Normal ACK,No ACK 帧序列

       支持重传机制以及重传速率和功率控制

       支持 MPDU 硬件聚合解聚合与 Immediate BlockAck 模式

       支持 RIFS,SIFS,AIFS

       支持反向传输机制

       支持 TSF 计时，并且软件可配置

       支持 MIB 统计信息

       满足加解密吞吐率大于 150Mbps

       Amba2.0 标准总线接口

       支持 WAPI 安全模式 2.0

       支持 WEP 安全模式-64 位加密

       支持 WEP 安全模式-128 位加密

       支持 TKIP 安全模式

       支持 CCMP 安全模式

       支持射频配置、收发器
     - √
     - √
     - √
     - √
     - ×
     - ×

   * - 蓝牙
     - 支持 BT/BLE 双模工作模式，支持 BT/BLE4.2 协议   
     - √
     - √
     - √
     - √
     - ×
     - ×

   * - FLASH 控制器
     - 提供总线访问 FLASH 接口 

       支持 SPI、Quad SPI 接口外接 Flash'

       外部 Flash 最大可支持 16MB

       支持硬件加密模块对 Flash 进行加密

       提供系统总线和数据总线访问仲裁

       实现 CACHE 缓存系统提高 FLASH 接口访问速度 

       提供对不同 QFlash 的兼容性

       支持 SPI、Quad SPI 接口外接 Flash

       外部 Flash 最大可支持 16MB

       支持硬件加密模块对 Flash 进行加密
     - √
     - √
     - √
     - √
     - √
     - √ 

   * - RSA 加密模块
     - 提供 Montgomery(FIOS 算法)模乘运算功能

       支持 128 位到 2048 位模乘
     - √
     - √
     - √
     - √
     - √
     - √ 

   * - 通用硬件加密模块
     - 支持 SHA1/MD5/RC4/DES/3DES/AES/CRC/TRNG 加密算法

       DES/3DES 支持 ECB 和 CBC 两种模式

       AES 支持 ECB、CBC 和 CTR 三种模式

       CRC 支持 CRC8、CRC16_MODBUS、CRC16_CCITT 和 CRC32 四种模式

       CRC 支持输入/输出反向

       SHA1/MD5/CRC 支持连续多包加密

       内置真随机数发生器，也支持 seed 种子产生伪随机数
     - √
     - √
     - √
     - √
     - √
     - √ 

   * - I2C 控制器
     - 内置一个 I²C 主控制器 

       工作频率支持100K—400K
     - √
     - √
     - √
     - √
     - √
     - √ 

   * - 主/从SPI控制器
     - 发送和接收通路各有 8 个字深度的 FIFO

       master 支持 Motorola SPI 的 4 种格式(CPOL,CPHA),TI 时序,macrowire 时

       slave 支持支持 Motorola SPI 的 4 种格式(CPOL,CPHA);

       支持全双工和半双工

       主设备支持 bit 传输，最大支持 65535bit 传输

       从设备支持各种长度 byte 的传输模式

       从设备输入的 SPI_Clk 最大时钟频率为系统时钟的 1/6
     - √
     - √
     - √
     - √
     - √
     - √ 

   * - UART 控制器
     - 设备端符合 APB 总线接口协议

       支持中断或轮询工作方式

       支持 DMA 传输模式，发送接收各存在 32-byte FIFO

       波特率可编程

       5-8bit 数据长度，以及 parity 极性可配置

       1 或 2 个 stop 位可配置

       支持 RTS/CTS 流控

       支持 Break 帧发送与接收

       Overrun,parity error,frame error,rx break frame 中断指示

       最大 16-burst byte DMA 操作
     - √
     - √
     - √
     - √
     - √
     - √ 

   * - GPIO 控制器
     - ——
     - √
     - √
     - √
     - √
     - √
     - √ 

   * - 定时器
     - 支持微秒与毫秒计时

       支持六个可配置的 32 位计数器
     - √
     - √
     - √
     - √
     - √
     - √ 
    
   * - 看门狗控制器
     - ——
     - √
     - √
     - √
     - √
     - √
     - √ 


   * - PWM 控制器
     - 5 通道 PWM 信号生成功能

       2 通道输入信号捕获功能(PWM0 和 PWM4 两个通路)

       输出信号频率范围 3Hz~160KHz

       占空比最大精度:1/256

       插入死区的计数器宽度:8bit
     - √
     - √
     - √
     - √
     - √
     - √

   * - I2S 控制器
     - 支持 AMBA APB 总线接口,32bit single 读写操作

       支持主，从模式，可以双工工作

       支持 8/16/24/32 位宽，最高采样频率为 192KHz

       支持单声道和立体声模式

       兼容 I²S 和 MSB justified 数据格式，兼容 PCM A/B 格式 

       支持 DMA 请求读写操作。只支持按字操作
     - √
     - √
     - √
     - √
     - √
     - √

   * - 7816/UART 控制器
     - 兼容 UART 以及 7816 接口功能

       设备端符合 APB 总线接口协议

       支持中断或轮询工作方式

       支持 DMA 传输模式，发送接收各存在 32-byte FIFO

       DMA 只能按字节进行操作，最大 16-burst byte DMA 操
     - √
     - √
     - √
     - √
     - √
     - √

   * - PSRAM 接口控制器
     - 支持对外置 PSRAM 的读写访问

       可配置为 SPI 和 QSPI

       SPI/QSPI 时钟频率可配置

       支持 BURST INC 模式访问

       支持 PSRAM 的半休眠模式
     - √
     - √
     - √
     - √
     - √
     - √

   * - ADC
     - 12比特 SD-ADC
       
       支持最高采样率 1KHz
     - 2路
     - 4路
     - 4路
     - 2路
     - 2路
     - 4路

   * - Touch Sensor触摸按键控制器
     - 支持Touch Sensor 扫描

       通过中断上报扫描结果
     - ×
     - 12路
     - 13路
     - 10路
     - ×
     - 12路   

