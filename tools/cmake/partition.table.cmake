set(PARTITION_TABLE_ADDR 0x0)
#partition table header addr is of no practical use
set(CONFIG_PARTITION_TABLE_HEADER 0x20026000)
set(PARTITION_TABLE_NAME partition_table)
set(PTB2BIN_TOOL ${python} ${SDK_PATH}/tools/wm/partition.py)
math(EXPR PARTITION_TABLE_ADDR "${CONFIG_WM_PARTITION_TABLE_OFFSET} + ${CONFIG_FLASH_BASE_ADDR}" OUTPUT_FORMAT HEXADECIMAL)

set(CONFIG_BIN_APP_HEADER_ADDRESS_SIZE 0x400)

if(${CONFIG_WM_PARTITION_TABLE_NORMAL_APP})
    set(PARTITION_TABLE_DEF_FILE ${SDK_PATH}/components/${PARTITION_TABLE_NAME}/config/partition_table_normal.csv)
elseif(${CONFIG_WM_PARTITION_TABLE_LARGE_APP})
    set(PARTITION_TABLE_DEF_FILE ${SDK_PATH}/components/${PARTITION_TABLE_NAME}/config/partition_table_large.csv)
elseif(${CONFIG_WM_PARTITION_TABLE_WITH_OTA})
    set(PARTITION_TABLE_DEF_FILE ${SDK_PATH}/components/${PARTITION_TABLE_NAME}/config/partition_table_with_ota.csv)
elseif(${CONFIG_WM_PARTITION_TABLE_CUSTOMIZATION})
    if(EXISTS ${PROJECT_SOURCE_DIR}/partition_table_custom.csv)
        set(PARTITION_TABLE_DEF_FILE ${PROJECT_SOURCE_DIR}/partition_table_custom.csv)
    else()
        message(WARNING "The current project does not have a partition_table_custom.csv file, please create this file in the project directory!")
        # set(PARTITION_TABLE_DEF_FILE ${SDK_PATH}/components/${PARTITION_TABLE_NAME}/config/partition_table_custom.csv)
    endif()
endif()


#if(USE_CMAKE_BUILD_DEBUG)
    message(STATUS "Partition table config: ${PARTITION_TABLE_DEF_FILE}")
#    file(READ ${PARTITION_TABLE_DEF_FILE} PARTITION_TABLE_CONTENT)
#    string(REPLACE "\n" ";" PARTITION_TABLE_LINES "${PARTITION_TABLE_CONTENT}")
#    foreach(LINE ${PARTITION_TABLE_LINES})
#        if(NOT "${LINE}" STREQUAL "")
#            message("            ${LINE}")
#        endif()
#    endforeach()
#endif()

execute_process(COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/${PARTITION_TABLE_NAME} RESULT_VARIABLE cmd_res)
if(NOT cmd_res EQUAL 0)
    message(FATAL_ERROR "make partition table dir error")
endif()

execute_process(COMMAND ${PTB2BIN_TOOL} ${PARTITION_TABLE_DEF_FILE} ${CMAKE_BINARY_DIR}/${PARTITION_TABLE_NAME}/${PARTITION_TABLE_NAME}.bin ${PROJECT_BINARY_DIR}/config/wmsdk_config.cmake ${PROJECT_BINARY_DIR}/config/wmsdk_config.h ${CONFIG_BIN_APP_HEADER_ADDRESS_SIZE} RESULT_VARIABLE cmd_res)
if(NOT cmd_res EQUAL 0)
    message(FATAL_ERROR "generate partition table config error")
endif()
