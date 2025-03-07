list(APPEND ADD_INCLUDE "include"
                        "${CONFIG_CHIP_FAMILY_NAME}/include"
                        )

aux_source_directory(${CONFIG_CHIP_FAMILY_NAME}/src ADD_SRCS)

register_component()
