if(CONFIG_COMPONENT_RF_ENABLED)
    list(APPEND ADD_INCLUDE "include"
                            )

	set(__source_code_path ${CMAKE_CURRENT_SOURCE_DIR}/rf)

	is_path_component(__exist_source_code ${__source_code_path})

	if(NOT __exist_source_code)
		if(USE_CMAKE_BUILD_DEBUG)
			message(STATUS "rf use lib")
		endif()
		list(APPEND ADD_STATIC_LIB "${SDK_PATH}/components/wm_soc/${CONFIG_CHIP_FAMILY_NAME}/lib/librf.a")
	endif()

    register_component()

	if(__exist_source_code)
		if(USE_CMAKE_BUILD_DEBUG)
			message(STATUS "rf use source code: ${__source_code_path}")
		endif()
		add_subdirectory(${__source_code_path} rf)
	endif()
endif()