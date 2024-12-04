list(APPEND ADD_INCLUDE "."
                        )

list(APPEND ADD_SRCS    "src/main.c"
                        "src/app_fs.c"
                        )

list(APPEND ADD_CUSTOM_FILES "fatfs"
                             "src/bin/fat16_image.bin 0 -1"
                        )

register_component()
