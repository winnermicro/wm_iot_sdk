# SPDX-License-Identifier: Apache-2.0

if(CONFIG_COMPONENT_BT_ENABLED)
	set(ADD_INCLUDE "")
	set(ADD_PRIVATE_INCLUDE "../"
							"../tinycrypt/include")
	set(ADD_DEFINITIONS "")		

	if(CONFIG_BT_MONITOR)
		list(APPEND ADD_SRCS "monitor.c")
	endif()

	if(CONFIG_BT_TINYCRYPT_ECC)
		list(APPEND ADD_SRCS "hci_ecc.c")
	endif()	

	if(CONFIG_BT_A2DP)
		list(APPEND ADD_SRCS "a2dp.c")
	endif()

	if(CONFIG_BT_AVDTP)
		list(APPEND ADD_SRCS "avdtp.c")
	endif()	
	
	if(CONFIG_BT_RFCOMM)
		list(APPEND ADD_SRCS "rfcomm.c")
	endif()
	
	if(CONFIG_BT_TESTING)
		list(APPEND ADD_SRCS "testing.c")
	endif()
	if(CONFIG_BT_SETTINGS)
		list(APPEND ADD_SRCS "settings.c")
	endif()	
	if(CONFIG_BT_HOST_CCM)
		list(APPEND ADD_SRCS "aes_ccm.c")
	endif()	
	
	if(CONFIG_BT_LONG_WQ)
		list(APPEND ADD_SRCS "long_wq.c")
	endif()
	
	if(CONFIG_BT_BREDR)
		list(APPEND ADD_SRCS "br.c"
							 "keys_br.c"
							 "l2cap_br.c"
							 "sdp.c"
							 "ssp.c")
	endif()
	
	if(CONFIG_BT_HFP_HF)
		list(APPEND ADD_SRCS "hfp_hf.c"
							 "at.c")
	endif()
	
	if(CONFIG_BT_HCI_HOST)
	
		list(APPEND ADD_SRCS "uuid.c"
							 "addr.c"
							 "buf.c"
							 "data.c"
							 "hci_core.c"
							 "hci_common.c"
							 "id.c")
		if(CONFIG_BT_BROADCASTER)
			list(APPEND ADD_SRCS "adv.c")
		endif()

		if(CONFIG_BT_OBSERVER)
			list(APPEND ADD_SRCS "scan.c")
		endif()							

		list(APPEND ADD_SRCS "crypto.c")
		
		if(CONFIG_BT_ECC)
			list(APPEND ADD_SRCS "ecc.c")
		endif()
		
		if(CONFIG_BT_CONN)
			list(APPEND ADD_SRCS "conn.c"
								"l2cap.c"
								"att.c"
								"gatt.c")
			if(CONFIG_BT_SMP)
				list(APPEND ADD_SRCS "smp.c"
									"keys.c")
			else()
				list(APPEND ADD_SRCS "smp_null.c")
			endif()
		endif()
		
		if(CONFIG_BT_ISO)
			list(APPEND ADD_SRCS "iso.c"
								"conn.c")
		endif()		

		if(CONFIG_BT_DF)
			list(APPEND ADD_SRCS "direction.c")
		endif()		
	endif()
	if(CONFIG_BT_SMP_LOG_LEVEL_DBG OR CONFIG_BT_KEYS_LOG_LEVEL_DBG OR CONFIG_BT_LOG_SNIFFER_INFO)
	  message(WARNING "One of these options are enabled:
	  CONFIG_BT_SMP_LOG_LEVEL_DBG CONFIG_BT_KEYS_LOG_LEVEL_DBG CONFIG_BT_LOG_SNIFFER_INFO.
	  Private security keys such as the LTK will be printed out, do not use in
	  production."
		)
	endif()
	if(CONFIG_BT_FIXED_PASSKEY)
	  message(WARNING "CONFIG_BT_FIXED_PASSKEY is enabled
	  A fixed passkey is easy to deduce during the pairing procedure, do not use in
	  production."
		)
	endif()
	if(CONFIG_BT_OOB_DATA_FIXED)
	  message(WARNING "CONFIG_BT_OOB_DATA_FIXED is enabled.
	  A hardcoded OOB data set will be stored in the image, do not use in
	  production."
		)
	endif()
	if(CONFIG_BT_USE_DEBUG_KEYS OR CONFIG_BT_STORE_DEBUG_KEYS)
	  message(WARNING "One or both these options are enabled:
	  CONFIG_BT_USE_DEBUG_KEYS CONFIG_BT_STORE_DEBUG_KEYS.
	  A predefined, publicly available keypair intended for testing will be used.
	  Do not use in production."
		)
	endif()
	if(CONFIG_BT_CONN_DISABLE_SECURITY)
	  message(WARNING "CONFIG_BT_CONN_DISABLE_SECURITY is enabled.
	  Security is disabled for incoming requests for GATT attributes and L2CAP
	  channels that would otherwise require encryption/authentication in order to
	  be accessed.
	  Do not use in production."
		)
	endif()
	
	register_component()
endif()