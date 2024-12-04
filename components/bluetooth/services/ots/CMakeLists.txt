# SPDX-License-Identifier: Apache-2.0

set(ADD_INCLUDE "")
set(ADD_PRIVATE_INCLUDE "")
set(ADD_DEFINITIONS "")		

if(CONFIG_BT_OTS)
	list(APPEND ADD_SRCS "ots.c"
						"ots_l2cap.c"
						"ots_obj_manager.c"
						"ots_oacp.c"
						"ots_olcp.c")
endif()
if(CONFIG_BT_OTS_DIR_LIST_OBJ)
	list(APPEND ADD_SRCS "ots_dir_list.c")
endif()
if(CONFIG_BT_OTS_CLIENT)
	list(APPEND ADD_SRCS "ots_client.c"
						"ots_l2cap.c"
						)
endif()

if(CONFIG_BT_OTS OR CONFIG_BT_OTS_CLIENT)
	register_component()
endif()	