list(APPEND ADD_INCLUDE "include")

# aux_source_directory(src ADD_SRCS)
append_srcs_dir(ADD_SRCS "src")

set(__source_code_path ${CMAKE_CURRENT_SOURCE_DIR}/lib)
is_path_component(__exist_source_code ${__source_code_path})

if(NOT __exist_source_code)
    if(USE_CMAKE_BUILD_DEBUG)
        message(STATUS "osal use lib")
    endif()
    list(APPEND ADD_STATIC_LIB "${SDK_PATH}/components/wm_soc/${chip_family_type}/lib/libosal.a"
                               )
endif()

register_component()

if(__exist_source_code)
    if(USE_CMAKE_BUILD_DEBUG)
        message(STATUS "osal use source code: ${__source_code_path}")
    endif()
    add_subdirectory(${__source_code_path})
endif()
