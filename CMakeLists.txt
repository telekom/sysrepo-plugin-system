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
cmake_minimum_required(VERSION 3.0)
project(sysrepo-plugin-system C)

include(CompileOptions.cmake)

set(PLUGIN_CORE_LIBRARY_NAME "srplg-ietf-system-core")

option(ENABLE_BUILD_TESTS, "Build tests" OFF)

# local includes
include_directories(
    ${CMAKE_SOURCE_DIR}/src/
    ${CMAKE_SOURCE_DIR}/deps/uthash/include
)

# find needed and optional packages
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules")
find_package(SYSREPO REQUIRED)
find_package(LIBYANG REQUIRED)
find_package(SRPC REQUIRED)
find_package(UMGMT REQUIRED)
find_package(LIBSYSTEMD REQUIRED)
find_package(AUGYANG)

# package includes
include_directories(
    ${SYSREPO_INCLUDE_DIRS}
    ${LIBYANG_INCLUDE_DIRS}
    ${SRPC_INCLUDE_DIRS}
    ${UMGMT_INCLUDE_DIRS}
    ${SYSTEMD_INCLUDE_DIRS}
)

# sources
set(
    CORE_SOURCES

    src/core/common.c
    src/core/ly_tree.c

    # startup
    src/core/startup/load.c
    src/core/startup/store.c

    # subs
    src/core/subscription/change.c
    src/core/subscription/operational.c
    src/core/subscription/rpc.c

    # data
    src/core/data/system/ip_address.c
    src/core/data/system/dns_resolver/search.c
    src/core/data/system/dns_resolver/search/list.c
    src/core/data/system/dns_resolver/server.c
    src/core/data/system/dns_resolver/server/list.c
    src/core/data/system/ntp/server.c
    src/core/data/system/ntp/server/list.c
    src/core/data/system/authentication/authorized_key.c
    src/core/data/system/authentication/authorized_key/list.c
    src/core/data/system/authentication/local_user.c
    src/core/data/system/authentication/local_user/list.c

    # system API
    src/core/api/system/load.c
    src/core/api/system/check.c
    src/core/api/system/store.c
    src/core/api/system/change.c
    src/core/api/system/ntp/load.c
    src/core/api/system/ntp/check.c
    src/core/api/system/ntp/store.c
    src/core/api/system/ntp/change.c
    src/core/api/system/dns_resolver/load.c
    src/core/api/system/dns_resolver/check.c
    src/core/api/system/dns_resolver/store.c
    src/core/api/system/dns_resolver/change.c
    src/core/api/system/authentication/load.c
    src/core/api/system/authentication/check.c
    src/core/api/system/authentication/store.c
    src/core/api/system/authentication/change.c
)

# build plugin static library
add_library(${PLUGIN_CORE_LIBRARY_NAME} STATIC ${CORE_SOURCES})
install(TARGETS ${PLUGIN_CORE_LIBRARY_NAME} DESTINATION lib)

# add main plugin to the build process
add_subdirectory("src/plugins/ietf-system")

# augyang support
if(AUGYANG_FOUND)
    add_subdirectory("src/plugins/ietf-system-augeas")
else(AUGYANG_FOUND)
    message(WARNING "AUGYANG not found - disabled build of the augeas specific plugin")
endif()

# unit testing
if(ENABLE_BUILD_TESTS)
    find_package(CMOCKA REQUIRED)
    include(CTest)
    include(tests/unit/Tests.cmake)
endif()