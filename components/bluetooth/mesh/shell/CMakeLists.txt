# SPDX-License-Identifier: Apache-2.0

if(CONFIG_BT_MESH_SHELL)
	set(ADD_DEFINITIONS "")		


	list(APPEND ADD_SRCS "shell.c"
						"utils.c")


	if(CONFIG_BT_MESH_SHELL_HEALTH_CLI)
		list(APPEND ADD_SRCS "health.c")
	endif()

	if(CONFIG_BT_MESH_SHELL_CFG_CLI)
		list(APPEND ADD_SRCS "cfg.c")
	endif()

	if(CONFIG_BT_MESH_SHELL_LARGE_COMP_DATA_CLI)
		list(APPEND ADD_SRCS "large_comp_data.c")
	endif()

	if(CONFIG_BT_MESH_SHELL_OP_AGG_CLI)
		list(APPEND ADD_SRCS "op_agg.c")
	endif()
	
	if(CONFIG_BT_MESH_SHELL_PRIV_BEACON_CLI)
		list(APPEND ADD_SRCS "priv_beacon.c")
	endif()

	if(CONFIG_BT_MESH_SHELL_RPR_CLI)
		list(APPEND ADD_SRCS "rpr.c")
	endif()
	
	if(CONFIG_BT_MESH_SHELL_SAR_CFG_CLI)
		list(APPEND ADD_SRCS "sar.c")
	endif()	
	
	if(CONFIG_BT_MESH_SHELL_BLOB_CLI OR CONFIG_BT_MESH_SHELL_BLOB_SRV OR
		CONFIG_BT_MESH_SHELL_BLOB_IO_FLASH)
		list(APPEND ADD_SRCS "blob.c")
	endif()

	if(CONFIG_BT_MESH_SHELL_DFU_SLOT OR CONFIG_BT_MESH_SHELL_DFU_METADATA OR
		CONFIG_BT_MESH_SHELL_DFU_CLI OR CONFIG_BT_MESH_SHELL_DFU_SRV)
		list(APPEND ADD_SRCS "dfu.c")
	endif()

	if(CONFIG_BT_MESH_SHELL_DFD_SRV)
		list(APPEND ADD_SRCS "dfd.c")
	endif()

	if(CONFIG_BT_MESH_OD_PRIV_PROXY_CLI)
		list(APPEND ADD_SRCS "od_priv_proxy.c")
	endif()

	if(CONFIG_BT_MESH_SOL_PDU_RPL_CLI)
		list(APPEND ADD_SRCS "sol_pdu_rpl.c")
	endif()

	register_component()
endif()

