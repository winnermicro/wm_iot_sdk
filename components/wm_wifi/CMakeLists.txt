if(CONFIG_COMPONENT_WIFI_ENABLED)
	list(APPEND ADD_INCLUDE "include"
	                        )

	list(APPEND ADD_SRCS "src/wm_wifi_init.c"
	                     )

    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/src/wm_wifi_oneshot.c")
        list(APPEND ADD_SRCS "src/wm_wifi_oneshot.c"
                             )
    endif()

    #set(__source_code_path ${CMAKE_CURRENT_LIST_DIR}/macsw)
    set(__source_code_path ${CMAKE_CURRENT_SOURCE_DIR}/macsw)
    is_path_component(__exist_source_code ${__source_code_path})

    if(NOT __exist_source_code)
        if(USE_CMAKE_BUILD_DEBUG)
            message(STATUS "wifi macsw use lib")
        endif()
        list(APPEND ADD_STATIC_LIB "${SDK_PATH}/components/wm_soc/${CONFIG_CHIP_FAMILY_NAME}/lib/libmacsw.a"
                                   )
    endif()

	register_component()

    if(__exist_source_code)
        if(USE_CMAKE_BUILD_DEBUG)
            message(STATUS "wifi macsw use source code: ${__source_code_path}")
        endif()
        add_subdirectory(${__source_code_path} macsw)
    endif()

endif()
