
append_srcs_dir(ADD_SRCS "src"
                         "src/aws_src"
                            )

list(APPEND ADD_INCLUDE "src"
                        "src/aws_src"
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
