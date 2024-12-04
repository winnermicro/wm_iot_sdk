# SPDX-License-Identifier: Apache-2.0

if(CONFIG_COMPONENT_BT_ENABLED)						
	set(ADD_INCLUDE "")
	set(ADD_PRIVATE_INCLUDE "")
	set(ADD_DEFINITIONS "")		
	list(APPEND ADD_SRCS "src/kernel.c"
						 "src/poll.c"
						 "src/util.c"
						 "src/mem_slab.c"
						 "src/drv_hci.c"
						 "src/atomic_c.c"
						 "src/net/buf.c"
						 "src/net/buf_simple.c"
						 )					
	register_component()
endif()
