# SPDX-License-Identifier: Apache-2.0

if(CONFIG_BT_MESH)
	set(ADD_INCLUDE ""
					"models/")
	set(ADD_PRIVATE_INCLUDE ".."
	                        "../include"
							"../tinycrypt/include")
	set(ADD_DEFINITIONS "")
    list(APPEND ADD_SRCS "main.c"
						 "cfg.c"
						 "adv.c"
						 "beacon.c"
						 "net.c"
						 "subnet.c"
						 "app_keys.c"
						 "heartbeat.c"
						 "crypto.c"
						 "access.c"
						 "msg.c"
						 "cfg_srv.c"
						 "health_srv.c"
						 "va.c")        
	if(CONFIG_BT_MESH_V1d1)
        list(APPEND ADD_SRCS "transport.c")
	else()
        list(APPEND ADD_SRCS "transport_legacy.c")
	endif()

	if(CONFIG_BT_MESH_ADV_LEGACY)
        list(APPEND ADD_SRCS "adv_legacy.c")
	endif()

	if(CONFIG_BT_MESH_ADV_EXT)
        list(APPEND ADD_SRCS "adv_ext.c")
	endif()

	if(CONFIG_BT_SETTINGS)
        list(APPEND ADD_SRCS "settings.c")
	endif()

	if(CONFIG_BT_MESH_RPL_STORAGE_MODE_SETTINGS)
        list(APPEND ADD_SRCS "rpl.c")
	endif()

	if(CONFIG_BT_MESH_LOW_POWER)
        list(APPEND ADD_SRCS "lpn.c")
	endif()

	if(CONFIG_BT_MESH_FRIEND)
        list(APPEND ADD_SRCS "friend.c")
	endif()

	if(CONFIG_BT_MESH_PROV)
        list(APPEND ADD_SRCS "prov.c")
	endif()
	
	if(CONFIG_BT_MESH_PROV_DEVICE)
        list(APPEND ADD_SRCS "prov_device.c")
	endif()

	if(CONFIG_BT_MESH_PROVISIONER)
        list(APPEND ADD_SRCS "provisioner.c")
	endif()

	if(CONFIG_BT_MESH_PB_ADV)
        list(APPEND ADD_SRCS "pb_adv.c")
	endif()
	
	if(CONFIG_BT_MESH_PB_GATT_COMMON)
        list(APPEND ADD_SRCS "pb_gatt.c")
	endif()
	
	if(CONFIG_BT_MESH_PB_GATT)
        list(APPEND ADD_SRCS "pb_gatt_srv.c")
	endif()
	
	if(CONFIG_BT_MESH_PB_GATT_CLIENT)
        list(APPEND ADD_SRCS "pb_gatt_cli.c")
	endif()
	
	if(CONFIG_BT_MESH_GATT_CLIENT)
        list(APPEND ADD_SRCS "gatt_cli.c")
	endif()
	
	if(CONFIG_BT_MESH_PROXY_CLIENT)
        list(APPEND ADD_SRCS "proxy_cli.c")
	endif()
	
	if(CONFIG_BT_MESH_GATT_PROXY)
        list(APPEND ADD_SRCS "proxy_srv.c")
	endif()
	
	if(CONFIG_BT_MESH_GATT)
        list(APPEND ADD_SRCS "proxy_msg.c")
	endif()
	
    if(CONFIG_BT_MESH_CFG_CLI)
        list(APPEND ADD_SRCS "cfg_cli.c")
    endif()

    if(CONFIG_BT_MESH_HEALTH_CLI)
        list(APPEND ADD_SRCS "health_cli.c")
    endif()	
	
    if(CONFIG_BT_MESH_SAR_CFG_CLI)
        list(APPEND ADD_SRCS "sar_cfg_cli.c")
    endif()
    if(CONFIG_BT_MESH_SAR_CFG_SRV)
        list(APPEND ADD_SRCS "sar_cfg_srv.c")
    endif()	
    if(CONFIG_BT_MESH_SAR_CFG)
        list(APPEND ADD_SRCS "sar_cfg.c")
    endif()	
    if(CONFIG_BT_MESH_OP_AGG_SRV)
        list(APPEND ADD_SRCS "op_agg_srv.c")
    endif()	
    if(CONFIG_BT_MESH_OP_AGG_CLI)
        list(APPEND ADD_SRCS "op_agg_cli.c")
    endif()	
    if(CONFIG_BT_MESH_OP_AGG)
        list(APPEND ADD_SRCS "op_agg.c")
    endif()	
    if(CONFIG_BT_MESH_LARGE_COMP_DATA_SRV)
        list(APPEND ADD_SRCS "large_comp_data_srv.c")
    endif()	
    if(CONFIG_BT_MESH_LARGE_COMP_DATA_CLI)
        list(APPEND ADD_SRCS "large_comp_data_cli.c")
    endif()	
    if(CONFIG_BT_MESH_PRIV_BEACON_SRV)
        list(APPEND ADD_SRCS "priv_beacon_srv.c")
    endif()		
    if(CONFIG_BT_MESH_PRIV_BEACON_CLI)
        list(APPEND ADD_SRCS "priv_beacon_cli.c")
    endif()	
    if(CONFIG_BT_MESH_SELF_TEST)
        list(APPEND ADD_SRCS "test.c")
    endif()	
    if(CONFIG_BT_MESH_CDB)
        list(APPEND ADD_SRCS "cdb.c")
    endif()	
    if(CONFIG_BT_MESH_BLOB_SRV)
        list(APPEND ADD_SRCS "blob_srv.c")
    endif()	
    if(CONFIG_BT_MESH_BLOB_CLI)
        list(APPEND ADD_SRCS "blob_cli.c")
    endif()	
    if(CONFIG_BT_MESH_BLOB_IO_FLASH)
        list(APPEND ADD_SRCS "blob_io_flash.c")
    endif()
    if(CONFIG_BT_MESH_DFU_CLI)
        list(APPEND ADD_SRCS "dfu_cli.c")
    endif()	
    if(CONFIG_BT_MESH_DFU_SRV)
        list(APPEND ADD_SRCS "dfu_srv.c")
    endif()		
    if(CONFIG_BT_MESH_DFD_SRV)
        list(APPEND ADD_SRCS "dfd_srv.c")
    endif()	
    if(CONFIG_BT_MESH_DFU_SLOTS)
        list(APPEND ADD_SRCS "dfu_slot.c")
    endif()	
    if(CONFIG_BT_MESH_DFU_METADATA)
        list(APPEND ADD_SRCS "dfu_metadata.c")
    endif()	
    if(CONFIG_BT_MESH_RPR_CLI)
        list(APPEND ADD_SRCS "rpr_cli.c")
    endif()	
    if(CONFIG_BT_MESH_RPR_SRV)
        list(APPEND ADD_SRCS "rpr_srv.c")
    endif()	
    if(CONFIG_BT_MESH_OD_PRIV_PROXY_CLI)
        list(APPEND ADD_SRCS "od_priv_proxy_cli.c")
    endif()	
    if(CONFIG_BT_MESH_OD_PRIV_PROXY_SRV)
        list(APPEND ADD_SRCS "od_priv_proxy_srv.c")
    endif()	
    if(CONFIG_BT_MESH_SOL_PDU_RPL_CLI)
        list(APPEND ADD_SRCS "sol_pdu_rpl_cli.c")
    endif()	
    if(CONFIG_BT_MESH_OD_PRIV_PROXY_SRV)
        list(APPEND ADD_SRCS "sol_pdu_rpl_srv.c")
    endif()	
    if(CONFIG_BT_MESH_SOLICITATION)
        list(APPEND ADD_SRCS "solicitation.c")
    endif()	
    if(CONFIG_BT_MESH_STATISTIC)
        list(APPEND ADD_SRCS "statistic.c")
    endif()	
    if(CONFIG_BT_MESH_USES_TINYCRYPT)
        list(APPEND ADD_SRCS "crypto_tc.c")
    else()
	    list(APPEND ADD_SRCS "crypto_psa.c")
	endif()
	if(CONFIG_BT_MESH_ONOFF_SRV)
	    list(APPEND ADD_SRCS "models/model_srv.c")
	endif()
	if(CONFIG_BT_MESH_ONOFF_CLI)
	    list(APPEND ADD_SRCS "models/model_cli.c")
	endif()
	if(CONFIG_BT_MESH_ONOFF_CLI OR CONFIG_BT_MESH_ONOFF_SRV)
		list(APPEND ADD_SRCS "models/model_util.c")
	endif()
	if(CONFIG_BT_MESH_SHELL)
		list(APPEND ADD_SRCS "shell/shell.c"
							"shell/utils.c")
	endif()

	if(CONFIG_BT_MESH_SHELL_HEALTH_CLI)
		list(APPEND ADD_SRCS "shell/health.c")
	endif()

	if(CONFIG_BT_MESH_SHELL AND CONFIG_BT_MESH_SHELL_CFG_CLI)
		list(APPEND ADD_SRCS "shell/cfg.c")
	endif()

	if(CONFIG_BT_MESH_SHELL AND CONFIG_BT_MESH_SHELL_LARGE_COMP_DATA_CLI)
		list(APPEND ADD_SRCS "shell/large_comp_data.c")
	endif()

	if(CONFIG_BT_MESH_SHELL AND CONFIG_BT_MESH_SHELL_OP_AGG_CLI)
		list(APPEND ADD_SRCS "shell/op_agg.c")
	endif()

	if(CONFIG_BT_MESH_SHELL AND CONFIG_BT_MESH_SHELL_PRIV_BEACON_CLI)
		list(APPEND ADD_SRCS "shell/priv_beacon.c")
	endif()

	if(CONFIG_BT_MESH_SHELL AND CONFIG_BT_MESH_SHELL_RPR_CLI)
		list(APPEND ADD_SRCS "shell/rpr.c")
	endif()

	if(CONFIG_BT_MESH_SHELL AND CONFIG_BT_MESH_SHELL_SAR_CFG_CLI)
		list(APPEND ADD_SRCS "shell/sar.c")
	endif()	


	if(CONFIG_BT_MESH_SHELL AND CONFIG_BT_MESH_SHELL_BLOB_CLI OR CONFIG_BT_MESH_SHELL_BLOB_SRV OR
		CONFIG_BT_MESH_SHELL_BLOB_IO_FLASH)
		list(APPEND ADD_SRCS "shell/blob.c")
	endif()

	if(CONFIG_BT_MESH_SHELL AND CONFIG_BT_MESH_SHELL_DFU_SLOT OR CONFIG_BT_MESH_SHELL_DFU_METADATA OR
		CONFIG_BT_MESH_SHELL_DFU_CLI OR CONFIG_BT_MESH_SHELL_DFU_SRV)
		list(APPEND ADD_SRCS "shell/dfu.c")
	endif()

	if(CONFIG_BT_MESH_SHELL AND CONFIG_BT_MESH_SHELL_DFD_SRV)
		list(APPEND ADD_SRCS "shell/dfd.c")
	endif()

	if(CONFIG_BT_MESH_SHELL AND CONFIG_BT_MESH_OD_PRIV_PROXY_CLI)
		list(APPEND ADD_SRCS "shell/od_priv_proxy.c")
	endif()

	if(CONFIG_BT_MESH_SHELL AND CONFIG_BT_MESH_SOL_PDU_RPL_CLI)
		list(APPEND ADD_SRCS "shell/sol_pdu_rpl.c")
	endif()

	register_component()
endif()
