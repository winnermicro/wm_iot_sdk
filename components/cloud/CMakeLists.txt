
if(CONFIG_COMPONENT_AWS_ENABLED)

# common file
    list(APPEND ADD_INCLUDE "aws/aws-iot-device-sdk-embedded-C/platform/include"
                            "aws/port/include"
                            )

    append_srcs_dir(ADD_SRCS "aws/port/src"
                             )

    if(CONFIG_AWS_CORE_MQTT)
        list(APPEND ADD_INCLUDE "aws/aws-iot-device-sdk-embedded-C/libraries/standard/coreMQTT/source/include"
                                "aws/aws-iot-device-sdk-embedded-C/libraries/standard/coreMQTT/source/interface"
                                "aws/aws-iot-device-sdk-embedded-C/libraries/standard/backoffAlgorithm/source/include"
                                )

        append_srcs_dir(ADD_SRCS "aws/aws-iot-device-sdk-embedded-C/libraries/standard/coreMQTT/source"
                                 "aws/aws-iot-device-sdk-embedded-C/libraries/standard/backoffAlgorithm/source"
                                 )

        list(APPEND ADD_INCLUDE "aws/aws-iot-device-sdk-embedded-C/platform/posix/transport/include"
                                )

        list(APPEND ADD_SRCS "aws/aws-iot-device-sdk-embedded-C/platform/posix/transport/src/plaintext_posix.c"
                             "aws/aws-iot-device-sdk-embedded-C/platform/posix/transport/src/sockets_posix.c"
                                 )
    endif()

    if(CONFIG_AWS_CORE_HTTP)
        list(APPEND ADD_INCLUDE "aws/aws-iot-device-sdk-embedded-C/libraries/standard/coreHTTP/source/include"
                                "aws/aws-iot-device-sdk-embedded-C/libraries/standard/coreHTTP/source/interface"
                                "aws/aws-iot-device-sdk-embedded-C/libraries/standard/backoffAlgorithm/source/include"
                                )

        append_srcs_dir(ADD_SRCS "aws/aws-iot-device-sdk-embedded-C/libraries/standard/coreHTTP/source"
                                 "aws/aws-iot-device-sdk-embedded-C/libraries/standard/backoffAlgorithm/source"
                                 )
    endif()

    if(CONFIG_AWS_CORE_JSON)
        list(APPEND ADD_INCLUDE "aws/aws-iot-device-sdk-embedded-C/libraries/standard/coreJSON/source/include"
                                )

        append_srcs_dir(ADD_SRCS "aws/aws-iot-device-sdk-embedded-C/libraries/standard/coreJSON/source"
                                 )
    endif()

    if(CONFIG_AWS_DEVICE_SHADOW)
        list(APPEND ADD_INCLUDE "aws/aws-iot-device-sdk-embedded-C/libraries/aws/device-shadow-for-aws-iot-embedded-sdk/source/include"
                                )

        append_srcs_dir(ADD_SRCS "aws/aws-iot-device-sdk-embedded-C/libraries/aws/device-shadow-for-aws-iot-embedded-sdk/source"
                                 )
    endif()

    if(CONFIG_AWS_JOB)
        list(APPEND ADD_INCLUDE "aws/aws-iot-device-sdk-embedded-C/libraries/aws/jobs-for-aws-iot-embedded-sdk/source/otaJobParser/include"
                                "aws/aws-iot-device-sdk-embedded-C/libraries/aws/jobs-for-aws-iot-embedded-sdk/source/include"
                                )

        append_srcs_dir(ADD_SRCS "aws/aws-iot-device-sdk-embedded-C/libraries/aws/jobs-for-aws-iot-embedded-sdk/source/otaJobParser"
                                 "aws/aws-iot-device-sdk-embedded-C/libraries/aws/jobs-for-aws-iot-embedded-sdk/source"
                                 )
    endif()

    if(CONFIG_AWS_OTA)
        list(APPEND ADD_INCLUDE "aws/aws-iot-device-sdk-embedded-C/libraries/aws/ota-for-aws-iot-embedded-sdk/source/include"
                                "aws/aws-iot-device-sdk-embedded-C/libraries/aws/ota-for-aws-iot-embedded-sdk/source/portable/os"
                                "aws/aws-iot-device-sdk-embedded-C/libraries/aws/ota-for-aws-iot-embedded-sdk/source/dependency/3rdparty/tinycbor/src"
                                )

        append_srcs_dir(ADD_SRCS "aws/aws-iot-device-sdk-embedded-C/libraries/aws/ota-for-aws-iot-embedded-sdk/source"
                                 "aws/aws-iot-device-sdk-embedded-C/libraries/aws/ota-for-aws-iot-embedded-sdk/source/portable/os"
                                 )
    endif()

    register_component()

endif()
