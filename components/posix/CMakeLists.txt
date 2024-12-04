if(CONFIG_COMPONENT_POSIX_ENABLED)

	list(APPEND ADD_INCLUDE "include"
							"FreeRTOS-Plus-POSIX/include"
							"FreeRTOS-Plus-POSIX/include/portable"
							)

#	list(APPEND ADD_PRIVATE_INCLUDE ""
#	                                )

	list(APPEND ADD_SRCS "FreeRTOS-Plus-POSIX/source/FreeRTOS_POSIX_clock.c"
	                     "FreeRTOS-Plus-POSIX/source/FreeRTOS_POSIX_mqueue.c"
	                     "FreeRTOS-Plus-POSIX/source/FreeRTOS_POSIX_pthread.c"
	                     "FreeRTOS-Plus-POSIX/source/FreeRTOS_POSIX_pthread_barrier.c"
	                     "FreeRTOS-Plus-POSIX/source/FreeRTOS_POSIX_pthread_cond.c"
	                     "FreeRTOS-Plus-POSIX/source/FreeRTOS_POSIX_pthread_mutex.c"
	                     "FreeRTOS-Plus-POSIX/source/FreeRTOS_POSIX_sched.c"
	                     "FreeRTOS-Plus-POSIX/source/FreeRTOS_POSIX_semaphore.c"
	                     "FreeRTOS-Plus-POSIX/source/FreeRTOS_POSIX_timer.c"
	                     "FreeRTOS-Plus-POSIX/source/FreeRTOS_POSIX_unistd.c"
	                     "FreeRTOS-Plus-POSIX/source/FreeRTOS_POSIX_utils.c"
	                     )

	if (CONFIG_UNIT_TEST_ENABLE_CODE_COVERAGE)
		list(APPEND ADD_DEFINITIONS -fno-profile-arcs -fno-test-coverage)
	endif()

	register_component()

endif()
