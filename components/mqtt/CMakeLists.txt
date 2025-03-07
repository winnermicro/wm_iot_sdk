if(CONFIG_COMPONENT_MQTT_ENABLED)
    list(APPEND ADD_INCLUDE "include"
                            "paho.mqtt.c/src"
                            )

    list(APPEND ADD_PRIVATE_INCLUDE "paho.mqtt.c/src"
                                    )

    append_srcs_dir(ADD_SRCS "paho.mqtt.c/src"
                             "src"
                             )

    list(REMOVE_ITEM ADD_SRCS "paho.mqtt.c/src/MQTTAsyncUtils.c"
                              "paho.mqtt.c/src/MQTTAsync.c"
                              "paho.mqtt.c/src/MQTTVersion.c"
                              )

    list(APPEND ADD_DEFINITIONS -DNO_PERSISTENCE=1
                                -DHIGH_PERFORMANCE=1
                                -DUSE_MBEDTLS=1
                                )
    register_component()
endif()
