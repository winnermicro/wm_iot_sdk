set(ADD_INCLUDE "")
set(ADD_PRIVATE_INCLUDE "include")
set(ADD_DEFINITIONS "")		

list(APPEND ADD_SRCS "src/utils.c")

if(CONFIG_TINYCRYPT_ECC_DH)
	list(APPEND ADD_SRCS "src/ecc_dh.c"
						 "src/ecc.c"
						 )
endif()
if(CONFIG_TINYCRYPT_ECC_DSA)
	list(APPEND ADD_SRCS "src/ecc_dsa.c"
						 "src/ecc.c"
						 )
endif()
if(CONFIG_TINYCRYPT_AES)
	list(APPEND ADD_SRCS "src/aes_decrypt.c"
						 "src/aes_encrypt.c"
						 )
endif()
if(CONFIG_TINYCRYPT_AES_CBC)
	list(APPEND ADD_SRCS "src/cbc_mode.c")
endif()
if(CONFIG_TINYCRYPT_AES_CTR)
	list(APPEND ADD_SRCS "src/ctr_mode.c")
endif()
if(CONFIG_TINYCRYPT_AES_CCM)
	list(APPEND ADD_SRCS "src/ccm_mode.c")
endif()
if(CONFIG_TINYCRYPT_AES_CMAC)
	list(APPEND ADD_SRCS "src/cmac_mode.c")
endif()
if(CONFIG_TINYCRYPT_SHA256)
	list(APPEND ADD_SRCS "src/sha256.c")
endif()
if(CONFIG_TINYCRYPT_SHA256_HMAC)
	list(APPEND ADD_SRCS "src/hmac.c")
endif()
if(CONFIG_TINYCRYPT_SHA256_HMAC_PRNG)
	list(APPEND ADD_SRCS "src/hmac_prng.c")
endif()
if(CONFIG_TINYCRYPT_CTR_PRNG)
	list(APPEND ADD_SRCS "src/ctr_prng.c")
endif()

register_component()
