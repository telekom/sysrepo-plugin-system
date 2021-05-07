add_executable(general_utest 
	tests/general_utest.c 
	src/utils/ntp/server_list.c
	src/utils/memory.c)
target_link_libraries(general_utest
	${CMOCKA_LIBRARIES}
	${SYSREPO_LIBRARIES}
	${LIBYANG_LIBRARIES}
	"-Wl,--wrap=clock_settime"
	"-Wl,--wrap=time"
	"-Wl,--wrap=sysinfo"
	"-Wl,--wrap=uname"
	"-Wl,--wrap=readlink"
	"-Wl,--wrap=getenv"
	"-Wl,--wrap=access"
	)
add_test(NAME general_utest COMMAND general_utest)
