
Performance Optimization
============================

Better Performance
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. only:: w800

    * Increase CPU main frequency, up to 240 MHz.

* Modify the default compilation optimization level to -O2 or -O3. Note that increasing the compiler optimization level may result in an increased firmware size and potentially expose previously undetected bugs.

* Move frequently executed code to SRAM to reduce XIP (Execute In Place) execution load latency.

   - Individual functions can be decorated with  ``ATTRIBUTE_IRAM`` , for example, to place the function `my_func`  in IRAM.

     ::

        ATTRIBUTE_IRAM void my_func(void)
        {
            /* do somethings... */
        }
   -  The entire file can be specified in  ``components\wm_system\ld\code_in_ram.ld.h``  to place the file  `my_code.c`  from the `main`  component into IRAM.
     
	 ::

         *libmain.a:my_code.c.obj(.text .text.*) 
		
* Reduce logging overhead by statically adjusting the default log level to ERROR or below during compilation.
* Disable heap memory and additional statistical features in FreeRTOS intended for debugging.
* Properly adjust the priorities of various FreeRTOS tasks..
* Enhance interrupt performance by minimizing the use of interrupt disabling as critical section protection.
* Improve network speed by referring to the iperf example and maximizing the available buffer and configurations in the WiFi driver and TCP/IP stack.
* Re-enable jump table optimization, which improves performance on hot paths in large switch-case code by adding compiler options ``-fjump-tables -ftree-switch-conversion`` during compilation.
* Avoid using floating-point arithmetic (float and double) in your program, as it is always slower than integer arithmetic.

Furthermore, there may be more ways to improve application performance based on usage scenarios, which users should choose flexibly.

For Better Size Optimization:
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

* Disable unused features in various components by selecting to turn them off in menuconfig.
* Choose the -Os optimization level during compilation, which is the default selection.
* Analyze the size occupancy by viewing the ``map`` file after compilation. The map file is located in the project ``build`` folder.
