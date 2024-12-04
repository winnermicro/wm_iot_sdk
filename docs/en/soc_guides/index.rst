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

