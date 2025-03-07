append_srcs_dir(ADD_SRCS "src"
                         "src/aws_src"
                         "src/wm_ota_port"
                         "../../../http/http_common/src"
                            )

list(APPEND ADD_INCLUDE "src"
                        "src/aws_src"
                        "src/wm_ota_port"
                        "../../../http/http_common/include"
                            )
if(EXISTS "../../../cert/internal")

    add_definitions(-DUSE_INTERNAL_CERT=1)

    list(APPEND ADD_EMBEDDED_TEXTFILES "../../../cert/AmazonRootCA1.pem"
                                       "../../../cert/internal/certificate.pem.crt"
                                       "../../../cert/internal/private.pem.key"
                                       )
else()
    list(APPEND ADD_EMBEDDED_TEXTFILES "../../../cert/AmazonRootCA1.pem"
                                       "../../../cert/common/certificate.pem.crt"
                                       "../../../cert/common/private.pem.key"
                                       )
endif()


register_component()


