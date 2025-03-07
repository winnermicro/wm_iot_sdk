# add target after link target
set(ELF2BIN_TOOL ${CMAKE_OBJCOPY} -O binary)

set(CONFIG_BIN_BOOTLOADER_RUN_ADDRESS 0x0)
math(EXPR CONFIG_BIN_BOOTLOADER_RUN_ADDRESS "${CONFIG_BIN_BOOTLOADER_HEADER_ADDRESS} + ${CONFIG_BIN_APP_HEADER_ADDRESS_SIZE}" OUTPUT_FORMAT HEXADECIMAL)

set(CONFIG_BIN_APP_RUN_ADDRESS 0x0)
math(EXPR CONFIG_BIN_APP_RUN_ADDRESS "${CONFIG_BIN_APP_HEADER_ADDRESS} + ${CONFIG_BIN_APP_HEADER_ADDRESS_SIZE}" OUTPUT_FORMAT HEXADECIMAL)

foreach(flag CONFIG_BOOTLOADER_SKIP_ESC_CHECK CONFIG_BOOTLOADER_SKIP_UPGRADE_CHECK CONFIG_BOOTLOADER_SKIP_FIRMWARE_INTEGRITY_CHECK)
    if("${${flag}}" STREQUAL "y")
        set(${flag} "1")
    else()
        set(${flag} "0")
    endif()
endforeach()
math(EXPR BOOTLOADER_CONFIG_FLAGS "${CONFIG_BOOTLOADER_LOG_LEVEL} | (${CONFIG_BOOTLOADER_SKIP_ESC_CHECK} << 3) | (${CONFIG_BOOTLOADER_SKIP_UPGRADE_CHECK} << 4) | (${CONFIG_BOOTLOADER_SKIP_FIRMWARE_INTEGRITY_CHECK} << 5)" OUTPUT_FORMAT HEXADECIMAL)

set(CONFIG_BUILD_BOOTLOADER_NEXT_HEADER ${CONFIG_BIN_APP_HEADER_ADDRESS})

add_custom_command(TARGET ${project_elf} POST_BUILD
                   COMMAND ${PACK_TOOL} --input-binary ${CMAKE_BINARY_DIR}/${PARTITION_TABLE_NAME}/${PARTITION_TABLE_NAME}.bin --output-name ${CMAKE_BINARY_DIR}/${PARTITION_TABLE_NAME}/${PARTITION_TABLE_NAME}.img --compress-type 0 --image-type 2 --image-header ${CONFIG_PARTITION_TABLE_HEADER} --run-address ${PARTITION_TABLE_ADDR} --update-address 0 --next-image-header 0 --upd-no 0
                   DEPENDS ${project_elf}
                   COMMENT "generate partition table image..."
                   )

add_custom_command(TARGET ${project_elf} POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/${BOOTLOADER_NAME}
                   COMMAND ${PACK_TOOL} --input-binary ${SDK_PATH}/components/${BOOTLOADER_NAME}/${CONFIG_CHIP_FAMILY_NAME}/${BOOTLOADER_NAME}.bin --output-name ${CMAKE_BINARY_DIR}/${BOOTLOADER_NAME}/${BOOTLOADER_NAME}.img --compress-type 0 --image-type 0 --image-header ${CONFIG_BIN_BOOTLOADER_HEADER_ADDRESS} --run-address ${CONFIG_BIN_BOOTLOADER_RUN_ADDRESS} --update-address ${CONFIG_BIN_BOOTLOADER_OTA_ADDRESS} --next-image-header ${CONFIG_BUILD_BOOTLOADER_NEXT_HEADER} --upd-no 0
                   DEPENDS ${project_elf}
                   COMMENT "generate bootloader image..."
                   )

add_custom_command(TARGET ${project_elf} POST_BUILD
                   COMMAND ${ELF2BIN_TOOL} ${CMAKE_BINARY_DIR}/${project_elf} ${CMAKE_BINARY_DIR}/${project_bin} ${CONFIG_BIN_APP_LENGTH}
                   DEPENDS ${project_elf}
                   COMMENT "generate bin..."
                   )

add_custom_command(TARGET ${project_elf} POST_BUILD
                   COMMAND ${PACK_TOOL} --input-binary ${CMAKE_BINARY_DIR}/${project_bin} --output-name ${project_img}.img --compress-type 0 --image-type 1 --image-header ${CONFIG_BIN_APP_HEADER_ADDRESS} --run-address ${CONFIG_BIN_APP_RUN_ADDRESS} --update-address ${CONFIG_BIN_APP_OTA_ADDRESS} --next-image-header 0 --upd-no 0 --version-string ${CONFIG_BUILD_VERSION} --bootloader-config-flags ${BOOTLOADER_CONFIG_FLAGS}
                   DEPENDS ${project_elf}
                   COMMENT "generate image..."
                   )

if(CONFIG_COMPONENT_OTA_ENABLED)
    add_custom_command(TARGET ${project_elf} POST_BUILD
                       COMMAND ${PACK_TOOL} --input-binary ${project_img}.img --output-name ${project_img}_ota.img --compress-type 16 --image-type 1 --image-header ${CONFIG_BIN_APP_HEADER_ADDRESS} --run-address ${CONFIG_BIN_APP_RUN_ADDRESS} --update-address ${CONFIG_BIN_APP_OTA_ADDRESS} --next-image-header 0 --upd-no 0 --version-string ${CONFIG_BUILD_VERSION}
                       DEPENDS ${project_elf}
                       COMMENT "generate ota image..."
                       )
endif()

set(all_custom_fls "")
if(all_custom_images)
    list(GET all_custom_images 0 all_custom_fls)
    list(LENGTH all_custom_images custom_image_count)
    if(custom_image_count)
        math(EXPR custom_image_count "${custom_image_count} - 1")
    endif()
    set(custom_temp_fls "${CMAKE_BINARY_DIR}/${CUSTOM_BIN_PATH}/__custom_bin_temp__.fls")
    foreach(custom_image_index RANGE 1 ${custom_image_count})
        list(GET all_custom_images ${custom_image_index} next_custom_image)
        add_custom_command(TARGET ${project_elf} POST_BUILD
                           COMMAND ${CMAKE_COMMAND} -E cat ${all_custom_fls} ${next_custom_image} > ${custom_temp_fls}
                           DEPENDS ${project_elf}
                           COMMENT "generate fls..."
                           )
        set(all_custom_fls "${custom_temp_fls}")
    endforeach()
endif()

add_custom_command(TARGET ${project_elf} POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E cat ${all_custom_fls} ${all_fatfs_images} ${CMAKE_BINARY_DIR}/${BOOTLOADER_NAME}/${BOOTLOADER_NAME}.img ${project_img}.img ${CMAKE_BINARY_DIR}/${PARTITION_TABLE_NAME}/${PARTITION_TABLE_NAME}.img > ${project_img}.fls
                   DEPENDS ${project_elf}
                   COMMENT "generate fls..."
                   )

include(${SDK_PATH}/tools/cmake/target.prompt.cmake)
