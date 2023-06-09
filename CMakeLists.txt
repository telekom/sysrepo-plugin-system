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
project(ietf-system-plugin)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/CMakeModules")

set(PLUGIN_LIRBARY_NAME srplg-ietf-system)
set(PLUGIN_EXECUTABLE_NAME ietf-system-plugin)
set(PLUGIN_CORE_LIBRARY_NAME ietf-system-plugin-core)

find_package(PkgConfig REQUIRED)
find_package(SYSREPOCPP REQUIRED)
find_package(LIBYANGCPP REQUIRED)
find_package(SYSREPO REQUIRED)
find_package(LIBYANG REQUIRED)
find_package(UMGMT REQUIRED)
find_package(LIBSYSTEMD REQUIRED)
find_package(LIBSDBUSCPP REQUIRED)

include_directories(
    ${SYSREPOCPP_INCLUDE_DIRS}
    ${LIBYANGCPP_INCLUDE_DIRS}
    ${UMGMT_INCLUDE_DIRS}
    ${CMAKE_SOURCE_DIR}/src
    ${SDBUS_CPP_INCLUDE_DIRS}
)

set(
    CORE_HEADERS

    src/core/context.hpp
    src/core/sub/change.hpp
    src/core/sub/oper.hpp
    src/core/sub/rpc.hpp
    src/core/yang/tree.hpp
    src/core/api.hpp
    src/core/common.hpp
    src/core/sdbus.hpp

    # system-state
    src/core/system-state/platform.hpp
    src/core/system-state/clock.hpp

    # system
    src/core/system/hostname.hpp
    src/core/system/timezone-name.hpp
    src/core/system/auth.hpp
    src/core/system/dns.hpp
    src/core/system/ip.hpp
)

set(
    CORE_SOURCES

    src/core/context.cpp
    src/core/sub/change.cpp
    src/core/sub/oper.cpp
    src/core/sub/rpc.cpp
    src/core/yang/tree.cpp
    src/core/api.cpp

    # system-state
    src/core/system-state/platform.cpp
    src/core/system-state/clock.cpp

    # system
    src/core/system/hostname.cpp
    src/core/system/timezone-name.cpp
    src/core/system/auth.cpp
    src/core/system/dns.cpp
    src/core/system/ip.cpp
)

set(
    PLUGIN_HEADERS

    src/plugin.hpp
)

set(
    PLUGIN_SOURCES

    src/plugin.cpp
)

set(
    EXECUTABLE_SOURCES

    src/main.c
    src/plugin.cpp
)

# check for systemd flag
if(DEFINED SYSTEMD_IFINDEX)
    add_compile_definitions(SYSTEMD_IFINDEX=${SYSTEMD_IFINDEX})
else()
    message(SEND_ERROR "No SYSTEMD_IFINDEX value set for default interface index to use with systemd... Unable to build without it")
endif()

# core library
add_library(
    ${PLUGIN_CORE_LIBRARY_NAME}
    STATIC
    ${CORE_SOURCES}
)

target_compile_options(
    ${PLUGIN_CORE_LIBRARY_NAME}
    PRIVATE
    -fPIC
)

# plugin as an executable
add_executable(
    ${PLUGIN_EXECUTABLE_NAME}
    ${EXECUTABLE_SOURCES}
)

target_link_libraries(
    ${PLUGIN_EXECUTABLE_NAME}

    ${PLUGIN_CORE_LIBRARY_NAME}

    ${SYSREPO_LIBRARIES}
    ${LIBYANG_LIBRARIES}
    ${SYSREPOCPP_LIBRARIES}
    ${LIBYANGCPP_LIBRARIES}
    ${UMGMT_LIBRARIES}
    ${SDBUSCPP_LIBRARIES}
)

# plugin as a library
add_library(
    ${PLUGIN_LIRBARY_NAME}
    MODULE
    ${PLUGIN_SOURCES}
)

target_link_libraries(
    ${PLUGIN_LIRBARY_NAME}

    ${PLUGIN_CORE_LIBRARY_NAME}
    ${SYSREPO_LIBRARIES}
    ${LIBYANG_LIBRARIES}
    ${SYSREPOCPP_LIBRARIES}
    ${LIBYANGCPP_LIBRARIES}
    ${UMGMT_LIBRARIES}
)
