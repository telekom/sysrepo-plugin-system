cmake_minimum_required(VERSION 3.0)
project(sysrepo-plugin-system C)

include(CompileOptions.cmake)

set(PLUGIN 0 CACHE BOOL "Build a plugin")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
option(ENABLE_BUILD_TESTS, "Build tests" OFF)

include_directories(
    ${CMAKE_SOURCE_DIR}/src/
    ${CMAKE_SOURCE_DIR}/deps/uthash/include
)

set(SOURCES
    src/system.c
    src/ly_tree.c
    src/startup.c
    src/dns_resolver.c
    src/utils/memory.c

    # subs
    src/subscription/change.c
    src/subscription/operational.c
    src/subscription/rpc.c
)

if (NOT PLUGIN)
	set(SOURCES ${SOURCES} src/main.c)
endif()

# get sysrepo version
find_package(PkgConfig)
if (PKG_CONFIG_FOUND)
    execute_process(COMMAND ${PKG_CONFIG_EXECUTABLE} "--modversion" "sysrepo" OUTPUT_VARIABLE SYSREPO_VERSION)
    if(SYSREPO_VERSION)
        # strip new line from string
        string(STRIP ${SYSREPO_VERSION} SYSREPO_VERSION)
        if(${SYSREPO_VERSION} VERSION_LESS "1.0.0")
            message(FATAL_ERROR "${PROJECT_NAME} requires at least libsysrepo verision 1.0.0")
        endif()
    endif()
endif()

if(PLUGIN)
    add_library(${CMAKE_PROJECT_NAME} MODULE ${SOURCES})
    install(TARGETS ${CMAKE_PROJECT_NAME} DESTINATION lib)
else()
    add_executable(${CMAKE_PROJECT_NAME} ${SOURCES})
    install(TARGETS ${CMAKE_PROJECT_NAME} DESTINATION bin)
endif()

set_target_properties(${CMAKE_PROJECT_NAME} PROPERTIES OUTPUT_NAME ${CMAKE_PROJECT_NAME} PREFIX "")

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules")
find_package(SYSREPO REQUIRED)
find_package(LIBYANG REQUIRED)
find_package(SRPC REQUIRED)
find_package(LIBSYSTEMD)
find_package(AUGYANG)

target_link_libraries(
    ${PROJECT_NAME}
    ${SYSREPO_LIBRARIES}
    ${LIBYANG_LIBRARIES}
    ${SRPC_LIBRARIES}
)

include_directories(
    ${SYSREPO_INCLUDE_DIRS}
    ${LIBYANG_INCLUDE_DIRS}
    ${SRPC_INCLUDE_DIRS}
)

if(SYSTEMD_FOUND)
    if(DEFINED SYSTEMD_IFINDEX)
        add_compile_definitions(SYSTEMD_IFINDEX=${SYSTEMD_IFINDEX})
    else()
        message(SEND_ERROR "No SYSTEMD_IFINDEX value set for default interface index to use with systemd... Unable to build without it")
    endif()
    add_compile_definitions(SYSTEMD)
    target_link_libraries(${CMAKE_PROJECT_NAME} ${SYSTEMD_LIBRARIES})
    include_directories(${SYSTEMD_INCLUDE_DIRS})    
endif()

# augyang support
if(AUGYANG_FOUND)
    add_compile_definitions(AUGYANG)
else(AUGYANG_FOUND)
    message(WARNING "AUGYANG not found - augeas support will be disabled")
endif()

if(ENABLE_BUILD_TESTS)
	find_package(CMOCKA REQUIRED)
    include (CTest)
    include(tests/Tests.cmake)
endif()