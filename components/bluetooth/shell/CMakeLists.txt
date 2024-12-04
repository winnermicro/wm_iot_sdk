# SPDX-License-Identifier: Apache-2.0
if(CONFIG_BT_SHELL)
	set(ADD_INCLUDE "")
	set(ADD_PRIVATE_INCLUDE "")
	set(ADD_DEFINITIONS "")		

	list(APPEND ADD_SRCS "bt.c"
						"hci.c"
						 )
	if(CONFIG_BT_CONN)
		list(APPEND ADD_SRCS "gatt.c"
							 )
	endif()

	if(CONFIG_BT_BREDR)
		list(APPEND ADD_SRCS "bredr.c")
	endif()

	if(CONFIG_BT_L2CAP_DYNAMIC_CHANNEL)
		list(APPEND ADD_SRCS "l2cap.c")
	endif()

	if(CONFIG_BT_RFCOMM)
		list(APPEND ADD_SRCS "rfcomm.c")
	endif()

	if(CONFIG_BT_ISO)
		list(APPEND ADD_SRCS "iso.c")
	endif()

	if(CONFIG_BT_IAS)
		list(APPEND ADD_SRCS "ias.c")
	endif()

	if(CONFIG_BT_IAS_CLIENT)
		list(APPEND ADD_SRCS "ias_client.c")
	endif()

	register_component()
endif()
