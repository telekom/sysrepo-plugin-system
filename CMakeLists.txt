cmake_minimum_required(VERSION 3.0)
project(sysrepo-plugin-system C)

include(CompileOptions.cmake)

set(PLUGIN_LIRBARY_NAME srplg-ietf-system)

set(PLUGIN 0 CACHE BOOL "Build a plugin")
option(ENABLE_BUILD_TESTS, "Build tests" OFF)

# local includes
include_directories(
    ${CMAKE_SOURCE_DIR}/src/
    ${CMAKE_SOURCE_DIR}/deps/uthash/include
)

# first - find needed and optional packages
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules")
find_package(SYSREPO REQUIRED)
find_package(LIBYANG REQUIRED)
find_package(SRPC REQUIRED)
find_package(UMGMT REQUIRED)
find_package(LIBSYSTEMD)
find_package(AUGYANG)

# package includes
include_directories(
    ${SYSREPO_INCLUDE_DIRS}
    ${LIBYANG_INCLUDE_DIRS}
    ${SRPC_INCLUDE_DIRS}
    ${UMGMT_INCLUDE_DIRS}
)

# sources
set(
    SOURCES

    src/plugin.c
    src/plugin/ly_tree.c

    # startup
    src/plugin/startup/load.c
    src/plugin/startup/store.c

    # subs
    src/plugin/subscription/change.c
    src/plugin/subscription/operational.c
    src/plugin/subscription/rpc.c

    # data
    src/plugin/data/system/ip_address.c
    src/plugin/data/system/dns_resolver/search.c
    src/plugin/data/system/dns_resolver/search/list.c
    src/plugin/data/system/dns_resolver/server.c
    src/plugin/data/system/dns_resolver/server/list.c
    src/plugin/data/system/ntp/server.c
    src/plugin/data/system/ntp/server/list.c
    src/plugin/data/system/authentication/authorized_key.c
    src/plugin/data/system/authentication/authorized_key/list.c
    src/plugin/data/system/authentication/local_user.c
    src/plugin/data/system/authentication/local_user/list.c

    # system API
    src/plugin/api/system/load.c
    src/plugin/api/system/check.c
    src/plugin/api/system/store.c
    src/plugin/api/system/change.c
    src/plugin/api/system/ntp/load.c
    src/plugin/api/system/ntp/check.c
    src/plugin/api/system/ntp/store.c
    src/plugin/api/system/ntp/change.c
    src/plugin/api/system/dns_resolver/load.c
    src/plugin/api/system/dns_resolver/check.c
    src/plugin/api/system/dns_resolver/store.c
    src/plugin/api/system/dns_resolver/change.c
    src/plugin/api/system/authentication/load.c
    src/plugin/api/system/authentication/check.c
    src/plugin/api/system/authentication/store.c
    src/plugin/api/system/authentication/change.c
)

# build plugin static library
add_library(${PLUGIN_LIRBARY_NAME} STATIC ${SOURCES})
install(TARGETS ${PLUGIN_LIRBARY_NAME} DESTINATION lib)

# link plugin library
target_link_libraries(
    ${PLUGIN_LIRBARY_NAME}
    ${SYSREPO_LIBRARIES}
    ${LIBYANG_LIBRARIES}
    ${SRPC_LIBRARIES}
    ${UMGMT_LIBRARIES}
)

if(NOT PLUGIN)
    add_executable(
        ${CMAKE_PROJECT_NAME}
        src/main.c
    )
    # link executable
    target_link_libraries(
        ${CMAKE_PROJECT_NAME}
        ${PLUGIN_LIRBARY_NAME}
        ${SYSREPO_LIBRARIES}
        ${LIBYANG_LIBRARIES}
        ${SRPC_LIBRARIES}
        ${UMGMT_LIBRARIES}
    )
    install(TARGETS ${CMAKE_PROJECT_NAME} DESTINATION bin)
else()
    # build plugin as a module for sysrepo-plugind
    add_library(
        ${CMAKE_PROJECT_NAME}
        MODULE
        ${SOURCES}
    )
    # remove lib prefix from plugin library
    set_target_properties(${CMAKE_PROJECT_NAME} PROPERTIES PREFIX "")
    # link executable
    target_link_libraries(
        ${CMAKE_PROJECT_NAME}
        ${SYSREPO_LIBRARIES}
        ${LIBYANG_LIBRARIES}
        ${SRPC_LIBRARIES}
        ${UMGMT_LIBRARIES}
    )
endif()

if(SYSTEMD_FOUND)
    if(DEFINED SYSTEMD_IFINDEX)
        add_compile_definitions(SYSTEMD_IFINDEX=${SYSTEMD_IFINDEX})
    else()
        message(SEND_ERROR "No SYSTEMD_IFINDEX value set for default interface index to use with systemd... Unable to build without it")
    endif()

    add_compile_definitions(SYSTEMD)
    target_link_libraries(
        ${CMAKE_PROJECT_NAME}
        ${SYSTEMD_LIBRARIES}
    )
    include_directories(
        ${SYSTEMD_INCLUDE_DIRS}
    )
endif()

# augyang support
if(AUGYANG_FOUND)
    add_compile_definitions(AUGYANG)
else(AUGYANG_FOUND)
    message(WARNING "AUGYANG not found - augeas support will be disabled")
endif()

if(ENABLE_BUILD_TESTS)
    find_package(CMOCKA REQUIRED)
    include(CTest)
    include(tests/Tests.cmake)
endif()