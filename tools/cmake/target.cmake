if(NOT USE_CMAKE_PRE_PROCESS)

# add target after link target
if(USE_CMAKE_BUILD_DEBUG)
    add_custom_command(TARGET ${project_elf} POST_BUILD
                       COMMAND ${CMAKE_OBJDUMP} -d ${project_elf} > ${project_img}.lst
                       COMMAND ${CMAKE_NM} -S --demangle --size-sort -s -r ${project_elf} > ${project_img}.nm
                       COMMAND ${CMAKE_READELF} -e -g -t ${project_elf} > ${project_img}.readelf
                       )
endif()

add_custom_command(TARGET ${project_elf} POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E echo "Creating ${chip_type} image..."
                   DEPENDS ${project_elf}
                   COMMENT ""
                   )

#check valid, if invalid set default value
if(NOT CONFIG_BIN_BOOTLOADER_HEADER_ADDRESS)
    set(CONFIG_BIN_BOOTLOADER_HEADER_ADDRESS 0x0)
    set(CONFIG_BIN_BOOTLOADER_LENGTH 0x0)
endif()
if(NOT CONFIG_BIN_APP_HEADER_ADDRESS)
    set(CONFIG_BIN_APP_HEADER_ADDRESS 0x0)
    set(CONFIG_BIN_APP_LENGTH 0x0)
endif()
if(NOT CONFIG_BIN_BOOTLOADER_OTA_ADDRESS)
    set(CONFIG_BIN_BOOTLOADER_OTA_ADDRESS 0x0)
    set(CONFIG_BIN_BOOTLOADER_OTA_LENGTH 0x0)
endif()
if(NOT CONFIG_BIN_APP_OTA_ADDRESS)
    set(CONFIG_BIN_APP_OTA_ADDRESS 0x0)
    set(CONFIG_BIN_APP_OTA_LENGTH 0x0)
endif()

set(PACK_TOOL ${python} ${SDK_PATH}/tools/wm/bin2img.py)

set(all_custom_images "")
list(LENGTH partition_name_list length)
if(length GREATER 0)
    set(CUSTOMFILE2IMG_TOOL ${python} ${SDK_PATH}/tools/wm/customfile2img.py)
    math(EXPR length "${length} - 1")
    foreach(index RANGE 0 ${length})
        list(GET partition_name_list ${index} partition_name)
        set(CUSTOM_BIN_PATH custom_bin)
        set(FILE_LIST ${${partition_name}_files_list})
        execute_process(COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/${CUSTOM_BIN_PATH} RESULT_VARIABLE cmd_res)
        if(NOT cmd_res EQUAL 0)
            message(FATAL_ERROR "make ${CUSTOM_BIN_PATH} dir error")
        endif()
        message(STATUS "Generate custom files image to ${partition_name}")
        execute_process(COMMAND ${CUSTOMFILE2IMG_TOOL} ${PARTITION_TABLE_DEF_FILE} ${partition_name} ${CMAKE_BINARY_DIR}/${CUSTOM_BIN_PATH} ${FILE_LIST} RESULT_VARIABLE cmd_res)
        if(NOT cmd_res EQUAL 0)
            message(FATAL_ERROR "generate ${partition_name} custom files image error")
        endif()
        list(APPEND all_custom_images "${CMAKE_BINARY_DIR}/${CUSTOM_BIN_PATH}/${partition_name}.img")
    endforeach()
endif()

set(BOOTLOADER_NAME bootloader)

if(${CONFIG_CHIP_W80X})
    include(${SDK_PATH}/tools/cmake/target.w80x.cmake)
elseif(${CONFIG_CHIP_W9XX})
    include(${SDK_PATH}/tools/cmake/target.w9xx.cmake)
endif()

# add lib copy
add_custom_target(lib COMMENT "copy lib files"
                  COMMAND ${CMAKE_COMMAND} -E make_directory ${SDK_PATH}/components/wm_soc/${chip_family_type}/lib
                  COMMAND ${CMAKE_COMMAND} -E make_directory ${PROJECT_BINARY_DIR}/wmlib
                  COMMAND ${CMAKE_COMMAND} -E copy_directory ${PROJECT_BINARY_DIR}/wmlib/ ${SDK_PATH}/components/wm_soc/${chip_family_type}/lib/
                  )

else() #USE_CMAKE_PRE_PROCESS

include("${SDK_PATH}/tools/cmake/target.prompt.cmake")

endif() #USE_CMAKE_PRE_PROCESS

# update ld file
add_custom_target(update_ld COMMENT "update ld file"
                  COMMAND ${CMAKE_C_COMPILER} -E -P -I ${CMAKE_BINARY_DIR}/config -I ${SDK_PATH}/components/wm_system/ld ${SDK_PATH}/components/wm_soc/${chip_family_type}/ld/chip_layout.ld.h -o ${CMAKE_BINARY_DIR}/project/chip_layout.ld
                  )

# update partition table
add_custom_target(update_pt COMMENT "update partition table"
                  COMMAND ${PTB2BIN_TOOL} ${PARTITION_TABLE_DEF_FILE} ${CMAKE_BINARY_DIR}/${PARTITION_TABLE_NAME}/${PARTITION_TABLE_NAME}.bin ${PROJECT_BINARY_DIR}/config/wmsdk_config.cmake ${PROJECT_BINARY_DIR}/config/wmsdk_config.h ${CONFIG_BIN_APP_HEADER_ADDRESS_SIZE} 
                  )

# show partition table show_partition_table
add_custom_target(show_pt COMMENT "show partition table"
                  COMMAND ${python} ${SDK_PATH}/tools/wm/partition.py ${PARTITION_TABLE_DEF_FILE}
                  )

add_custom_target(touch_prj COMMENT "touch project"
                  COMMAND ${CMAKE_COMMAND} -E touch ${project_elf_src}
                  )
