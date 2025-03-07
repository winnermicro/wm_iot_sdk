SoC Guide
==============

.. hint::
   The current system does not support automatic detection of chip models. Users need to select the correct chip model through menuconfig before compilation. If the selected model does not match the actual device being programmed, it may lead to hardware functionality issues such as pin multiplexing anomalies. After the system programming is completed, the chip model configured during compilation will be displayed during startup.

Chip Comparison 
------------------

.. raw:: html

   <table class="docutils align-center">
   <thead>
   <tr class="row-odd">
   <th class="head wm-mid-padding2"><p class="wm-small-font2">SoC</p></th>
   <th class="head wm-mid-padding2"><p class="wm-small-font2">Features</p></th>
   <th class="head wm-mid-padding2"><p class="wm-small-font2">Package</p></th>
   <th class="head wm-mid-padding2"><p class="wm-small-font2">Flash</p></th>
   <th class="head wm-mid-padding2"><p class="wm-small-font2">Touch Sensor</p></th>
   <th class="head wm-mid-padding2"><p class="wm-small-font2">Wake-up Pin</p></th>
   <th class="head wm-mid-padding2"><p class="wm-small-font2">Available Pins</p></th>
   <th class="head wm-mid-padding2"><p class="wm-small-font2">Reference Materials</p></th>
   </tr>
   </thead>
   <tbody>
   <tr class="row-even">
   <td class="wm-mid-padding2"><p class="wm-small-font2">W800</p></td>
   <td class="wm-mid-padding2"><p class="wm-small-font2">MCU+WiFi+BLE</p></td>
   <td class="wm-mid-padding2"><p class="wm-small-font2">QFN32(4*4mm)</p></td>
   <td class="wm-mid-padding2"><p class="wm-small-font2">2 MBytes</p></td>
   <td class="wm-mid-padding2"><p class="wm-small-font2">None</p></td>
   <td class="wm-mid-padding2"><p class="wm-small-font2">Yes</p></td>
   <td class="wm-mid-padding2"><p class="wm-small-font2">18</p></td>
   <td class="wm-mid-padding2"><p class="wm-small-font2">
      <a href="../../../../download/soc_pdf/w800/chip_specification.pdf">Chip Specification</a><br>
      <a href="../../../../download/soc_pdf/w800/chip_design_guide.pdf">Chip Design Guide</a><br>
      <a href="../../../../download/soc_pdf/w800/hardware_design_guide.zip">Hardware Reference Design</a><br><br>
      <a href="../../../../download/soc_pdf/w800/register_manual.pdf">Chip Register Manual</a></p></td>
   </tr>
   <tr class="row-odd">
   <td class="wm-mid-padding2"><p class="wm-small-font2">W801S</p></td>
   <td class="wm-mid-padding2"><p class="wm-small-font2">MCU+WiFi+BLE</p></td>
   <td class="wm-mid-padding2"><p class="wm-small-font2">QFN56(6*6mm)</p></td>
   <td class="wm-mid-padding2"><p class="wm-small-font2">2 MBytes</p></td>
   <td class="wm-mid-padding2"><p class="wm-small-font2">Yes</p></td>
   <td class="wm-mid-padding2"><p class="wm-small-font2">Yes</p></td>
   <td class="wm-mid-padding2"><p class="wm-small-font2">43</p></td>
   <td class="wm-mid-padding2"><p class="wm-small-font2">
      <a href="../../../../download/soc_pdf/w801s/chip_specification.pdf">Chip Specification</a><br>
      <a href="../../../../download/soc_pdf/w801s/hardware_design_guide.zip">Hardware Reference Design</a><br><br>
   </tr>
   <tr class="row-even">
   <td class="wm-mid-padding2"><p class="wm-small-font2">W802</p></td>
   <td class="wm-mid-padding2"><p class="wm-small-font2">MCU+WiFi+BLE</p></td>
   <td class="wm-mid-padding2">QFN56(6*6mm)</p></td>
   <td class="wm-mid-padding2"><p class="wm-small-font2">None</p></td>
   <td class="wm-mid-padding2"><p class="wm-small-font2">Yes</p></td>
   <td class="wm-mid-padding2"><p class="wm-small-font2">Yes</p></td>
   <td class="wm-mid-padding2"><p class="wm-small-font2">37</p></td>
   <td class="wm-mid-padding2"><p class="wm-small-font2">
      <a href="../../../../download/soc_pdf/w802/chip_specification.pdf">Chip Specification</a><br>
      <a href="../../../../download/soc_pdf/w802/hardware_design_guide.zip">Hardware Reference Design</a><br><br>
   </tr>
   <tr class="row-odd">
   <td class="wm-mid-padding2"><p class="wm-small-font2">W803</p></td>
   <td class="wm-mid-padding2"><p class="wm-small-font2">MCU+WiFi+BLE</p></td>
   <td class="wm-mid-padding2"><p class="wm-small-font2">QFN32(4*4mm)</p></td>
   <td class="wm-mid-padding2"><p class="wm-small-font2">2 MBytes</p></td>
   <td class="wm-mid-padding2"><p class="wm-small-font2">Yes</p></td>
   <td class="wm-mid-padding2"><p class="wm-small-font2">Yes</p></td>
   <td class="wm-mid-padding2"><p class="wm-small-font2">20</p></td>
   <td class="wm-mid-padding2"><p class="wm-small-font2">
      <a href="../../../../download/soc_pdf/w803/chip_specification.pdf">Chip Specification</a><br>
      <a href="../../../../download/soc_pdf/w803/chip_design_guide.pdf">Chip Design Guide</a><br>
      <a href="../../../../download/soc_pdf/w803/hardware_design_guide.zip">Hardware Reference Design</a><br><br>
   </tr>
   <tr class="row-even">
   <td class="wm-mid-padding2"><p class="wm-small-font2">W805</p></td>
   <td class="wm-mid-padding2"><p class="wm-small-font2">MCU（No RF）</p></td>
   <td class="wm-mid-padding2"><p class="wm-small-font2">QFN32(4*4mm)</p></td>
   <td class="wm-mid-padding2"><p class="wm-small-font2">1 MBytes</p></td>
   <td class="wm-mid-padding2"><p class="wm-small-font2">None</p></td>
   <td class="wm-mid-padding2"><p class="wm-small-font2">Yes</p></td>
   <td class="wm-mid-padding2"><p class="wm-small-font2">18</p></td>
   <td class="wm-mid-padding2"><p class="wm-small-font2">
      <a href="../../../../download/soc_pdf/w805/chip_specification.pdf">Chip Specification</a><br>
      <a href="../../../../download/soc_pdf/w805/chip_design_guide.pdf">Chip Design Guide</a><br>
   </tr>
   <tr class="row-odd">
   <td class="wm-mid-padding2"><p class="wm-small-font2">W806</p></td>
   <td class="wm-mid-padding2"><p class="wm-small-font2">MCU（No RF）</p></td>
   <td class="wm-mid-padding2"><p class="wm-small-font2">QFN56(6*6mm)</p></td>
   <td class="wm-mid-padding2"><p class="wm-small-font2">1 MBytes</p></td>
   <td class="wm-mid-padding2"><p class="wm-small-font2">Yes</p></td>
   <td class="wm-mid-padding2"><p class="wm-small-font2">Yes</p></td>
   <td class="wm-mid-padding2"><p class="wm-small-font2">43</p></td>
   <td class="wm-mid-padding2"><p class="wm-small-font2">
      <a href="../../../../download/soc_pdf/w806/chip_specification.pdf">Chip Specification</a><br>
      <a href="../../../../download/soc_pdf/w806/chip_design_guide.pdf">Chip Design Guide</a><br>
   </tr>
   </tbody>
   </table>


Chip comparison differences in detail
-----------------------------------------

.. list-table::
   :widths: 25 50 25 25 25 25 25 25
   :header-rows: 0
   :align: center

   * - Chip function
     - Chip Character
     - W800
     - W801S
     - W802
     - W803
     - W805
     - W806
   
   * - Capsulation
     - ——
     - QFN32 4*4mm
     - QFN56 6*6mm
     - QFN56 6*6mm
     - QFN32 4*4mm
     - QFN32 4*4mm
     - QFN56 6*6mm

   * - SDIO HOST controller
     - Compatible with SD card specification 1.0/1.1/2.0(SDHC)

       Compatible with SDIO memory card specification 1.1.0

       Compatible with MMC specification 2.0 to 4.2 11	

       The interface clock rate can be configured to support the host rate of 0~50MHz	

       Support standard MMC interface		

       Blocks up to 1024 bytes are supported				

       Support soft reset function				

       Automatic Command/Response CRC generation/validation	

       Automatic data CRC generation/validation			

       Configurable timeout detection				

       SPI, 1-bit SD, and 4-bit SD modes are supported		

       Support DMA data transfer
     - √
     - √
     - √
     - √
     - √
     - √

   * - SDIO Device controller
     - Compatible with SDIO card specification 2.0

       Host rate 0~50MHz is supported		

       Blocks up to 1024 bytes are supported	

       Support soft reset function				

       SPI, 1-bit SD, and 4-bit SD modes are supported			
     - √
     - √
     - √
     - √
     - √
     - √

   * - High-speed SPI device controller
     - Compatible with the universal SPI protocol

       Selectable level interrupt signal			

       The maximum speed is 50Mbps				

       Simple frame format, full hardware parsing and DMA				
     - √
     - √
     - √
     - √
     - √
     - √

   * - DMA controller
     - Amba2.0 standard bus interface, 8 DMA channels

       DMA operation based on memory linked list structure is supported	

       The software is configured with 16 hardware request sources		

       1, 4-burst operation mode is supported				

       Support byte, half-word, word operations		

       Source and destination address invariable or sequentially incrementing can be configured or operated in a loop within a predefined address range				

       Synchronize DMA request and DMA response hardware interface timing				
     - √
     - √
     - √
     - √
     - √
     - √

   * - Clock and reset
     - ——
     - √
     - √
     - √
     - √
     - √
     - √

   * - Memory manager
     - ——
     - √
     - √
     - √
     - √
     - √
     - √

   * - WiFi
     - Data Rate: 1~54Mpbs(802.11a/b/g), 6.5~150Mbps(802.11n)

       Data rate: 1 to 54Mpbs (802.11a/b/g), 6.5 to 150 MBPS (802.11n)	

       MCS format: MCS0~MCS7, MCS32(40MHz HT Duplicate mode)				

       Support 40MHz bandwidth non-HT Duplicate mode, 6M ~ 54M				

       Signal bandwidth: 20MHz, 40MHz				

       Modulation modes: DSSS(DBPSK,DQPSK,CCK) and OFDM(BPSK,QPSK,16QAM,64QAM)		

       MIMO-OFDM spatial multiplexing for 1T1R is implemented				

       Supports Short GI mode				

       legacy and Mixed modes are supported				

       It supports the transmission and reception of 20M upper and lower sideband signals under 40MHz bandwidth			

       STBC reception of MCS0 to 7 and 32 is supported				
 
       Supports the Green Field mode				
				
       Support EDCA channel access mode			

       It supports CSMA/CA, NAV and TXOP protection mechanisms		

       Beacon, Mng, VO, VI, BE, BK five way send queue and QoS		

       It supports single and wide multicast frame receiving and sending			

       RTS/CTS, CTS2SELF, Normal ACK, No ACK frame sequences are supported			

       Retransmission mechanism and retransmission rate and power control are supported				

       MPDU hardware aggregation disaggregation with Immediate BlockAck mode is supported			

       Supports RIFS, SIFS, AIFS				

       Support reverse transmission mechanism	

       TSF timing is supported and the software is configurable	

       Supports MIB statistics				
				
       The encryption and decryption throughput is more than 150Mbps		

       Amba2.0 standard bus interface				

       Supports WAPI Security Mode 2.0				

       Supports WEP Secure mode - 64-bit encryption			

       Supports WEP Secure mode - 128-bit encryption			

       Supports TKIP security mode				

       Supports CCMP security mode				

       Supports RF configuration transceivers
     - √
     - √
     - √
     - √
     - ×
     - ×

   * - Bluetooth
     - Supports BT/BLE dual-mode working mode and supports BT/BLE4.2
     - √
     - √
     - √
     - √
     - ×
     - ×

   * - FLASH controller
     - Provide bus access FLASH interface				

       Support SPI, Quad SPI interface external Flash		

       The external Flash can support up to 16MB				

       Support hardware encryption module to encrypt Flash		

       Provides system bus and data bus access arbitration			

       CACHE cache system is implemented to improve FLASH interface access speed		

       Provides compatibility with different QFlash				

       Support SPI, Quad SPI interface external Flash		

       The external Flash can support up to 16MB				

       Support hardware encryption module to encrypt Flash				
     - √
     - √
     - √
     - √
     - √
     - √

   * - RSA encryption module
     - Provides Montgomery(FIOS algorithm)modular multiplication funcation
       
       Support 128-bit to 2048-bit modular multiplication
     - √
     - √
     - √
     - √
     - √
     - √

   * - General purpose hardware encryption module
     - Support SHA1 / MD5 / RC4 DES / 3 DES, AES/CRC/TRNG encryption algorithm	

       DES/3DES supports both ECB and CBC modes				

       AES supports three modes: ECB, CBC and CTR				

       The CRC supports four modes: CRC8, CRC16_MODBUS, CRC16_CCITT and CRC32	

       CRC supports input/output reversals				

       SHA1/MD5/CRC supports continuous multi-packet encryption				

       Built-in true random number generator, also support seed seed to generate pseudo-random numbers				
     - √
     - √
     - √
     - √
     - √
     - √

   * - I2C controller
     - Built-in I²C host controller

       Operating frequency support 100k-400k
     - √
     - √
     - √
     - √
     - √
     - √

   * - Host/Slave SPI controller
     - The transmit and receive paths have FIfos of 8 word depth each				

       master supports four formats of Motorola SPI (CPOL, CPHA), TI timing, macrowire timing	

       slave supports four formats (CPOL, CPHA) that support Motorola SPI.				

       Both full-duplex and half-duplex are supported				

       The main device supports bit transmission, up to 65535bit transmission		

       Slave devices support various transfer modes of byte length				

       The maximum clock frequency of SPI_Clk input from the slave device is 1/6 of the system clock				
     - √
     - √
     - √
     - √
     - √
     - √


   * - UART controller
     - The device side conforms to the APB bus interface protocol		

       Support interrupt or polling mode of operation				

       DMA transfer mode is supported, and 32-byte FIFO exists for both transmit and receive	

       Baud rate programmable				

       5-8bit data length, and parity polarity configurable				

       1 or 2 stop bits are configurable				

       Support RTS/CTS flow control			

       Support Break frame sending and receiving			

       Overrun, parity error, frame error, rx break frame interrupt indication		

       Maximum 16-burst byte DMA operation				
     - √
     - √
     - √
     - √
     - √
     - √

   * - GPIO controller
     - ——
     - √
     - √
     - √
     - √
     - √
     - √

   * - Timer
     - Microsecond and millisecond timing

       Support for six configurable 32-bit counters
     - √
     - √
     - √
     - √
     - √
     - √

   * - The watchdog controller
     - ——
     - √
     - √
     - √
     - √
     - √
     - √


   * - PWM controller
     - 5 channel PWM signal generation function		

       2 channel input signal capture function (PWM0 and PWM4 two channels)	

       The output signal frequency ranges from 3Hz to 160KHz				

       Maximum precision of duty cycle: 1/256				

       Width of the counter inserted into the dead zone: 8bit				
     - √
     - √
     - √
     - √
     - √
     - √

   * - I2S controller
     - Support AMBA APB bus interface, 32bit single read and write operation	

       Support master, slave mode, can work in duplex				

       It supports 8/16/24/32 bit width and the maximum sampling frequency is 192KHz		

       Support mono and stereo modes				

       Compatible with I²S and MSB justified data format, compatible with PCM A/B format		

       DMA request read and write operations are supported. Only word-by-word operations are supported				
     - √
     - √
     - √
     - √
     - √
     - √

   * - 7816/UART controller
     - Compatible with UART and 7816 interface function		

       The device side conforms to the APB bus interface protocol

       Support interrupt or polling mode of operation				

       DMA transfer mode is supported, and 32-byte FIFO exists for both transmit and receive		

       DMA can only operate byte-by-byte, with a maximum 16-burst byte DMA operation				
     - √
     - √
     - √
     - √
     - √
     - √

   * - PSRAM port controller
     - It supports read and write access to external PSRAM				

       It can be configured as SPI and QSPI				

       The SPI/QSPI clock frequency is configurable				

       BURST INC mode access is supported				

       Support PSRAM semi-sleep mode				
     - √
     - √
     - √
     - √
     - √
     - √

   * - ADC
     - 12bit SD-ADC
       
       Supports a maximum sampling rate of 1KHz
     - 2 ways
     - 4 ways
     - 4 ways
     - 2 ways
     - 2 ways
     - 4 ways

   * - Touch Sensor controller
     - Sipport Touch Sensor interrupt

       Report scan results via interrupts		
     - ×
     - 12 ways 
     - 13 ways
     - 10 ways
     - ×
     - 12 ways


