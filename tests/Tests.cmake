#
# telekom / sysrepo-plugin-system
#
# This program is made available under the terms of the
# BSD 3-Clause license which is available at
# https://opensource.org/licenses/BSD-3-Clause
#
# SPDX-FileCopyrightText: 2021 Deutsche Telekom AG
# SPDX-FileContributor: Sartura Ltd.
#
# SPDX-License-Identifier: BSD-3-Clause
#

add_executable(general_utest
	tests/general_utest.c
	src/utils/dns/resolv_conf.c
	src/utils/dns/server.c
	src/utils/dns/search.c
	src/utils/ntp/server_list.c
	src/utils/user_auth/user_authentication.c
	src/utils/memory.c)
target_link_libraries(general_utest
	${CMOCKA_LIBRARIES}
	${SYSREPO_LIBRARIES}
	${LIBYANG_LIBRARIES}
	${SYSTEMD_LIBRARIES}
	"-Wl,--wrap=clock_settime"
	"-Wl,--wrap=time"
	"-Wl,--wrap=sysinfo"
	"-Wl,--wrap=uname"
	"-Wl,--wrap=readlink"
	"-Wl,--wrap=getenv"
	"-Wl,--wrap=access"
	"-Wl,--wrap=fopen"
	"-Wl,--wrap=getpwent"
	"-Wl,--wrap=open"
	"-Wl,--wrap=rename"
	"-Wl,--wrap=remove"
	"-Wl,--wrap=endpwent"
	)
add_test(NAME general_utest COMMAND general_utest)
