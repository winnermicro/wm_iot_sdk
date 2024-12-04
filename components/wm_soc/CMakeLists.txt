list(APPEND ADD_INCLUDE "include"
                        "${chip_family_type}/include"
                        )

aux_source_directory(${chip_family_type}/src ADD_SRCS)

register_component()
