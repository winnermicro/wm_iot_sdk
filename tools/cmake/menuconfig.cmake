#kconfig ui style
#set(ENV{MENUCONFIG_STYLE} "aquatic selection=fg:white,bg:magenta")

if(EXISTS ${PROJECT_SOURCE_DIR}/prj.config)
    if(USE_CMAKE_BUILD_DEBUG)
        message(STATUS "Find project defaults config (prj.config)")
    endif()
    list(APPEND kconfig_defaults_files_args --defaults "${PROJECT_SOURCE_DIR}/prj.config")
endif()

string(REPLACE ";" " " components_kconfig_files "${kconfig_defaults_files_args}")
string(REPLACE ";" " " components_kconfig_files "${components_kconfig_files}")

if(NOT EXISTS "${PROJECT_BINARY_DIR}/config")
    execute_process(COMMAND ${CMAKE_COMMAND} -E make_directory ${PROJECT_BINARY_DIR}/config RESULT_VARIABLE cmd_res)
    if(NOT cmd_res EQUAL 0)
        message(FATAL_ERROR "mkdir kconfig error")
    endif()
endif()

# add menuconfig target for config
set(generate_menuconfig_cmd ${python} ${SDK_PATH}/tools/wm/kconfig.py
                            --kconfig "${SDK_PATH}/tools/wm/Kconfig"
                            ${kconfig_defaults_files_args}
                            --menuconfig True
                            --env "SDK_PATH=${SDK_PATH}"
                            --env "PROJECT_PATH=${PROJECT_SOURCE_DIR}"
                            --env "HOST_OS_TYPE=${host_os_type}"
                            --output config ${PROJECT_BINARY_DIR}/config/wmsdk.config
                            --output cmake  ${PROJECT_BINARY_DIR}/config/wmsdk_config.cmake
                            --output header ${PROJECT_BINARY_DIR}/config/wmsdk_config.h
                            )
add_custom_target(menuconfig COMMAND ${generate_menuconfig_cmd})

set(generate_config_cmd ${python} ${SDK_PATH}/tools/wm/kconfig.py
                        --kconfig "${SDK_PATH}/tools/wm/Kconfig"
                        ${kconfig_defaults_files_args}
                        --menuconfig False
                        --env "SDK_PATH=${SDK_PATH}"
                        --env "PROJECT_PATH=${PROJECT_SOURCE_DIR}"
                        --env "HOST_OS_TYPE=${host_os_type}"
                        --output config ${PROJECT_BINARY_DIR}/config/wmsdk.config
                        --output cmake  ${PROJECT_BINARY_DIR}/config/wmsdk_config.cmake
                        --output header ${PROJECT_BINARY_DIR}/config/wmsdk_config.h
                        )
execute_process(COMMAND ${generate_config_cmd} RESULT_VARIABLE cmd_res)
if(NOT cmd_res EQUAL 0)
    message(FATAL_ERROR "execute kconfig error")
endif()

if(USE_CMAKE_AUTO_CONF)
    set(auto_conf_cmd ${python} ${SDK_PATH}/tools/wm/autoconf.py
                      --project "${PROJECT_SOURCE_DIR}"
                      --config "${PROJECT_BINARY_DIR}/config/wmsdk.config"
                      --sdk "${SDK_PATH}"
                      )
    execute_process(COMMAND ${auto_conf_cmd} RESULT_VARIABLE cmd_res)
    if(NOT cmd_res EQUAL 0 AND NOT cmd_res EQUAL 1)
        message(WARNING "execute auto conf error")
    endif()
    if(cmd_res EQUAL 1)
        execute_process(COMMAND ${generate_config_cmd} RESULT_VARIABLE cmd_res)
        if(NOT cmd_res EQUAL 0)
            message(WARNING "execute kconfig too error")
        endif()
    endif()
endif()
