set(DEVICE_TABLE_NAME device_table)
set(DEVICE_TABLE_DEF_FILE ${SDK_PATH}/components/wm_dt/config/${chip_type}/device_table.toml)
set(DEVICE_TABLE_PRJ_FILE ${PROJECT_SOURCE_DIR}/device_table.toml)
set(DEVICE_TABLE_FILE ${CMAKE_BINARY_DIR}/${DEVICE_TABLE_NAME}/device_table.toml)
set(DEVICE_TABLE_DEF_CFILE wm_dt_hw.c)

if(NOT CONFIG_DT_USE_C_STYLE_CONF)
    set(TOML2BIN_TOOL ${python} ${SDK_PATH}/tools/wm/wmdt.py)

    if(NOT EXISTS ${CMAKE_BINARY_DIR}/${DEVICE_TABLE_NAME})
        execute_process(COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/${DEVICE_TABLE_NAME} RESULT_VARIABLE cmd_res)
        if(NOT cmd_res EQUAL 0)
            message(FATAL_ERROR "make device table dir error")
        endif()
    endif()

    if(NOT EXISTS ${CMAKE_BINARY_DIR}/${DEVICE_TABLE_NAME}/${DEVICE_TABLE_DEF_CFILE})
        execute_process(COMMAND ${CMAKE_COMMAND} -E touch ${CMAKE_BINARY_DIR}/${DEVICE_TABLE_NAME}/${DEVICE_TABLE_DEF_CFILE} RESULT_VARIABLE cmd_res)
        if(NOT cmd_res EQUAL 0)
            message(FATAL_ERROR "touch device table .c error")
        endif()
    endif()

    if(EXISTS ${DEVICE_TABLE_PRJ_FILE})
        set(DEVICE_TABLE_DEF_FILE ${DEVICE_TABLE_PRJ_FILE})
    endif()

    if(NOT EXISTS ${DEVICE_TABLE_FILE})
        file(COPY "${DEVICE_TABLE_DEF_FILE}" DESTINATION "${CMAKE_BINARY_DIR}/${DEVICE_TABLE_NAME}/")
    endif()

    message(STATUS "Device table config: ${DEVICE_TABLE_DEF_FILE}")
    if(USE_CMAKE_BUILD_DEBUG)
        message(STATUS "Device table use: ${DEVICE_TABLE_FILE}")
    endif()

    # for components append_srcs_dir
    execute_process(COMMAND ${TOML2BIN_TOOL} -i ${DEVICE_TABLE_FILE} -c ${PROJECT_BINARY_DIR}/config/wmsdk.config -o ${CMAKE_BINARY_DIR}/${DEVICE_TABLE_NAME} RESULT_VARIABLE cmd_res)
    if(NOT cmd_res EQUAL 0)
        message(FATAL_ERROR "generate device table config error")
    endif()

    add_custom_target(update_dt
                      COMMAND ${TOML2BIN_TOOL} -i ${DEVICE_TABLE_FILE} -c ${PROJECT_BINARY_DIR}/config/wmsdk.config -o ${CMAKE_BINARY_DIR}/${DEVICE_TABLE_NAME}
                      COMMENT "Generate device table"
                      )
else()
    if(EXISTS ${DEVICE_TABLE_FILE})
        file(REMOVE "${DEVICE_TABLE_FILE}")
    endif()

    add_custom_target(update_dt
                      COMMAND ${CMAKE_COMMAND} -E echo ""
                      COMMENT "Generate device table"
                      )
endif()
