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
find_package(PkgConfig)
pkg_search_module(GTEST REQUIRED gtest_main)

enable_testing()

# IP address classes tests
add_executable(
    ip_test
    ${CMAKE_SOURCE_DIR}/tests/unit/ip_test.cpp
)
target_link_libraries(ip_test PRIVATE ${GTEST_LDFLAGS})
target_compile_options(ip_test PRIVATE ${GTEST_CFLAGS})
add_test(ip_test ip_test)
