if(CONFIG_COMPONENT_FATFS_ENABLED)
    list(APPEND ADD_INCLUDE "include"
                            "src"
                            )

    list(APPEND ADD_SRCS "src/ff.c"
                         "src/ffunicode.c"
                         "src/ffsystem.c"
                         "diskio/wm_diskio.c"
                         )

    if(CONFIG_FATFS_SDMMC_DISK_ENABLE)
        list(APPEND ADD_SRCS "diskio/wm_diskio_sdmmc.c")
    endif()

    if(CONFIG_FATFS_EXTERNAL_FLASH_DISK_ENABLE OR CONFIG_FATFS_INTERNAL_FLASH_DISK_ENABLE)
        list(APPEND ADD_SRCS "diskio/wm_diskio_flash.c")
    endif()

    register_component()
endif()
