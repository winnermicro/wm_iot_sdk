# SPDX-License-Identifier: Apache-2.0

if(CONFIG_COMPONENT_BT_ENABLED)
    list(APPEND ADD_INCLUDE ".")
    list(APPEND ADD_INCLUDE "include")
	list(APPEND ADD_INCLUDE "porting/include")								
							
	add_subdirectory(common)

	add_subdirectory(host)

	if(CONFIG_BT_SHELL)
		add_subdirectory(shell)
	endif()	

	if(CONFIG_BT_CONN)
		add_subdirectory(services)
	endif()	

	if(CONFIG_BT_MESH)
		add_subdirectory(mesh)
	endif()	
	
	if(CONFIG_BT_AUDIO)
		add_subdirectory(audio)
	endif()		
	
	if(CONFIG_BT_CRYPTO)
		add_subdirectory(crypto)
	endif()

	add_subdirectory(lib)

	add_subdirectory(tinycrypt)

	add_subdirectory(porting)	

	if(CONFIG_BT_SETTINGS)
		add_subdirectory(settings)
	endif()

	#if(CONFIG_BT_CTLR)
		set(__source_code_path ${CMAKE_CURRENT_SOURCE_DIR}/controller)
		is_path_component(__exist_source_code ${__source_code_path})

		if(NOT __exist_source_code)
			if(USE_CMAKE_BUILD_DEBUG)
				message(STATUS "bluetooth controller use lib")
			endif()
			list(APPEND ADD_STATIC_LIB "${SDK_PATH}/components/wm_soc/${CONFIG_CHIP_FAMILY_NAME}/lib/libcontroller.a")
		endif()

		if(__exist_source_code)
			if(USE_CMAKE_BUILD_DEBUG)
				message(STATUS "bluetooth controller use source code: ${__source_code_path}")
			endif()
			add_subdirectory(${__source_code_path} controller)
		endif()
	#endif()

	register_component()
endif()
