cmake_minimum_required(VERSION 3.0)
project(sysrepo-plugin-system)

include(CompileOptions.cmake)

set(PLUGIN_LIRBARY_NAME srplg-ietf-system)
set(CMAKE_CXX_STANDARD 20)

set(PLUGIN 0 CACHE BOOL "Build a plugin")
option(ENABLE_BUILD_TESTS, "Build tests" OFF)

# local includes
include_directories(
    ${CMAKE_SOURCE_DIR}/src/
    ${CMAKE_SOURCE_DIR}/deps/uthash/include
)

# first - find needed and optional packages
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules")
find_package(SYSREPOCPP REQUIRED)
find_package(LIBYANGCPP REQUIRED)
find_package(SYSREPO REQUIRED)
find_package(LIBYANG REQUIRED)
find_package(UMGMT REQUIRED)
find_package(SRPC REQUIRED)
find_package(SRPCPP REQUIRED)
find_package(LIBSYSTEMD)
find_package(AUGYANG)

# package includes
include_directories(
    ${SYSREPOCPP_INCLUDE_DIRS}
    ${LIBYANGCPP_INCLUDE_DIRS}
    ${SRPCPP_INCLUDE_DIRS}
)

# Plugin library
set(
    PLUGIN_SOURCES

    src/Plugin.cpp
)

add_library(
    ${PLUGIN_LIRBARY_NAME}
    MODULE
    ${PLUGIN_SOURCES}
)

target_link_libraries(
    ${PLUGIN_LIRBARY_NAME}

    ${SYSREPO_LIBRARIES}
    ${LIBYANG_LIBRARIES}
    ${SYSREPOCPP_LIBRARIES}
    ${LIBYANGCPP_LIBRARIES}
    ${SRPC_LIBRARIES}
    ${SRPCPP_LIBRARIES}
)