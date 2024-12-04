list(APPEND ADD_INCLUDE "."
                        )

list(APPEND ADD_SRCS "src/main.c"
                     )

list(APPEND ADD_EMBEDDED_FILES "src/mqtt_eclipseprojects_io.pem"
                               )

register_component()
