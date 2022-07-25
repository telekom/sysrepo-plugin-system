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
add_executable(
    system_utest

    tests/system_utest.c
)

target_link_libraries(
    system_utest

    ${CMOCKA_LIBRARIES}
    ${SYSREPO_LIBRARIES}
    ${LIBYANG_LIBRARIES}
    ${SYSTEMD_LIBRARIES}
    ${PLUGIN_LIRBARY_NAME}

    "-Wl,--wrap=gethostname"
    "-Wl,--wrap=sethostname"
)

add_test(NAME system_utest COMMAND system_utest)