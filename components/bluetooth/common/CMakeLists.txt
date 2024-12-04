# SPDX-License-Identifier: Apache-2.0

set(ADD_INCLUDE "")
set(ADD_PRIVATE_INCLUDE "../")
set(ADD_DEFINITIONS "")						
list(APPEND ADD_SRCS "addr.c"
                     "dummy.c"
                     "bt_str.c"
                     )

if(CONFIG_BT_RPA)
	list(APPEND ADD_SRCS "rpa.c"
						)
endif()

register_component()
