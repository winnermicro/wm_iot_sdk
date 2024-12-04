# SPDX-License-Identifier: Apache-2.0

# SPDX-License-Identifier: Apache-2.0
if(CONFIG_SETTINGS)
	set(ADD_INCLUDE "")
	set(ADD_PRIVATE_INCLUDE "include")
	set(ADD_DEFINITIONS "")		

	list(APPEND ADD_SRCS "src/settings_store.c"
						 "src/settings.c"
						 "src/settings_init.c"
						 "src/settings_line.c")
	if(CONFIG_SETTINGS_RUNTIME)
		list(APPEND ADD_SRCS "src/settings_runtime.c")
	endif()
	if(CONFIG_SETTINGS_FS)
		list(APPEND ADD_SRCS "src/settings_file.c")
	endif()	
	if(CONFIG_SETTINGS_FILE)
		list(APPEND ADD_SRCS "src/settings_file.c")
	endif()
	if(CONFIG_SETTINGS_FCB)
		list(APPEND ADD_SRCS "src/settings_fcb.c")
	endif()	
	if(CONFIG_SETTINGS_NVS_KV)
		list(APPEND ADD_SRCS "src/settings_kv.c")
	endif()		
	if(CONFIG_SETTINGS_NVS)
		list(APPEND ADD_SRCS "src/settings_nvs.c")
	endif()		
	if(CONFIG_SETTINGS_NONE)
		list(APPEND ADD_SRCS "src/settings_none.c")
	endif()	
	if(CONFIG_SETTINGS_SHELL)
		list(APPEND ADD_SRCS "src/settings_shell.c")
	endif()		
	register_component()
endif()
