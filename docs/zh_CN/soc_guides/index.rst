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

