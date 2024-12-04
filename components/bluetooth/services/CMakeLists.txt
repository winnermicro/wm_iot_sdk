# SPDX-License-Identifier: Apache-2.0

set(ADD_INCLUDE "")
set(ADD_PRIVATE_INCLUDE "")
set(ADD_DEFINITIONS "")		

if(CONFIG_BT_DIS)
	list(APPEND ADD_SRCS "dis.c"
						 )
endif()

if(CONFIG_BT_BAS)
	list(APPEND ADD_SRCS "bas.c")
endif()

if(CONFIG_BT_HRS)
	list(APPEND ADD_SRCS "hrs.c")
endif()

if(CONFIG_BT_TPS)
	list(APPEND ADD_SRCS "tps.c")
endif()

if(CONFIG_BT_WPS)
	list(APPEND ADD_SRCS "wps_over_ble.c")
endif()

if(CONFIG_BT_OTA)
	list(APPEND ADD_SRCS "ota_over_ble.c")
endif()

if(CONFIG_BT_OTS OR CONFIG_BT_OTS_CLIENT)
  add_subdirectory(ots)
endif()

if(CONFIG_BT_IAS OR CONFIG_BT_IAS_CLIENT)
  add_subdirectory(ias)
endif()

register_component()