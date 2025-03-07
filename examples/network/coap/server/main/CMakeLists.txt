list(APPEND ADD_SRCS "src/coap_server_exmaple.c"
                     )
list(APPEND ADD_EMBEDDED_TEXTFILES "certs/ca_cert_rsa_sha256.pem"
                                       "certs/srv_cert_rsa_sha256.pem"
                                       "certs/srv_key_rsa.pem"
                                       )

register_component()
