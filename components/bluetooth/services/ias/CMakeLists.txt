# SPDX-License-Identifier: Apache-2.0


set(ADD_INCLUDE "")
set(ADD_PRIVATE_INCLUDE "")
set(ADD_DEFINITIONS "")		

if(CONFIG_BT_IAS)
	list(APPEND ADD_SRCS "ias.c"
							)
endif()

if(CONFIG_BT_IAS_CLIENT)
	list(APPEND ADD_SRCS "ias_client.c"
							)
endif()

if(CONFIG_BT_IAS OR CONFIG_BT_IAS_CLIENT)
	#register_component()
endif()