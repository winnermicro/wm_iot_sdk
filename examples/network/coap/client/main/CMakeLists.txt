list(APPEND ADD_SRCS "src/coap_client_exmaple.c"
                     )

list(APPEND ADD_EMBEDDED_TEXTFILES "certs/ca_cert_rsa_sha256.pem"
                                       "certs/cert_rsa.pem"
                                       "certs/private_key_rsa.pem"
                                       )

register_component()
