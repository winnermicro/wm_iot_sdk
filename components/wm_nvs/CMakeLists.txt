if(CONFIG_COMPONENT_NVS_ENABLED)
    list(APPEND ADD_INCLUDE  "include")
    list(APPEND ADD_INCLUDE  "src")

    list(APPEND ADD_SRCS
        "src/wm_nvs.c"
        "src/wm_nvs_partition_manager.c"
        "src/wm_nvs_storage.c"
        "src/wm_nvs_partition.c"
        "src/wm_nvs_porting.c"
        "src/wm_nvs_hash.c"
        "src/wm_nvs_item.c"
        "src/wm_nvs_sector_manager.c"
        "src/wm_nvs_sector.c"
        "src/wm_nvs_handler.c"
        "src/wm_nvs_iterator.c"
        "src/wm_nvs_debug.c"
    )

    register_component()
endif()
