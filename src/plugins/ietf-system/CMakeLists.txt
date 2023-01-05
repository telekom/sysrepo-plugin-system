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

set(PLUGIN_LIBRARY_NAME "srplg-ietf-system")
set(PLUGIN_EXECUTABLE_NAME "ietf-system-plugin")

set(
    PLUGIN_SOURCES

    plugin.c
    datastore/running/load.c
    datastore/running/store.c
)

# check for systemd flag
if(DEFINED SYSTEMD_IFINDEX)
    add_compile_definitions(SYSTEMD_IFINDEX=${SYSTEMD_IFINDEX})
else()
    message(SEND_ERROR "No SYSTEMD_IFINDEX value set for default interface index to use with systemd... Unable to build without it")
endif()

# add plugin as a sysrepo-plugind library
add_library(
    ${PLUGIN_LIBRARY_NAME}
    MODULE
    ${PLUGIN_SOURCES}
)

# add plugin as a standalone executable
add_executable(
    ${PLUGIN_EXECUTABLE_NAME}

    ${PLUGIN_SOURCES}
    ${CMAKE_SOURCE_DIR}/src/main.c
)
target_link_libraries(
    ${PLUGIN_EXECUTABLE_NAME}

    # link against the core plugin library
    ${PLUGIN_CORE_LIBRARY_NAME}

    # link other external libraries
    ${SYSREPO_LIBRARIES}
    ${LIBYANG_LIBRARIES}
    ${SRPC_LIBRARIES}
    ${UMGMT_LIBRARIES}
    ${SYSTEMD_LIBRARIES}
)

install(TARGETS ${PLUGIN_LIBRARY_NAME} DESTINATION lib)
install(TARGETS ${PLUGIN_EXECUTABLE_NAME} DESTINATION bin)