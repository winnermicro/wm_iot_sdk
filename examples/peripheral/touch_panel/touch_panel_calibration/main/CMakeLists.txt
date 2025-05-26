list(APPEND ADD_INCLUDE "src"
                        )
list(APPEND ADD_SRCS    "src/main.c"
                        "src/tp_cali_on_lcd.c"
                        )

register_component()
