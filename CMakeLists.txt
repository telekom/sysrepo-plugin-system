cmake_minimum_required(VERSION 2.8)
project(sysrepo-plugin-general C)

include(CompileOptions.cmake)

set(PLUGIN 0 CACHE BOOL "Build a plugin")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
option(ENABLE_BUILD_TESTS, "Build tests" OFF)

include_directories(
    ${CMAKE_SOURCE_DIR}/src/
)

set(SOURCES
    src/general.c
    src/utils/memory.c
    src/utils/ntp/server_list.c
)

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

if (ENABLE_BUILD_TESTS)
	find_package(CMOCKA REQUIRED)
endif()

target_link_libraries(
    ${PROJECT_NAME}
    ${SYSREPO_LIBRARIES}
    ${LIBYANG_LIBRARIES}
)

include_directories(
    ${SYSREPO_INCLUDE_DIRS}
    ${LIBYANG_INCLUDE_DIRS}
)

if(ENABLE_BUILD_TESTS)
	if(CMOCKA_FOUND)
		enable_testing()
		include(tests/Tests.cmake)
	else()
		message(STATUS "CMocka hasn't been found, disabling tests")
		set(ENABLE_BUILD_TESTS OFF)
	endif()
endif()
