if(SDBUSCPP_LIBRARIES AND SDBUSCPP_INCLUDE_DIRS)
    set(SDBUSCPP_FOUND TRUE)
else()
    find_path(
        SDBUSCPP_INCLUDE_DIR
        NAMES sdbus-c++/sdbus-c++.h
        PATHS /usr/include /usr/local/include /opt/local/include /sw/include ${CMAKE_INCLUDE_PATH} ${CMAKE_INSTALL_PREFIX}/include
    )

    find_library(
        SDBUSCPP_LIBRARY
        NAMES sdbus-c++
        PATHS /usr/lib /usr/lib64 /usr/local/lib /usr/local/lib64 /opt/local/lib /sw/lib ${CMAKE_LIBRARY_PATH} ${CMAKE_INSTALL_PREFIX}/lib
    )

    if(SDBUSCPP_INCLUDE_DIR AND SDBUSCPP_LIBRARY)
        set(SDBUSCPP_FOUND TRUE)
    else(SDBUSCPP_INCLUDE_DIR AND SDBUSCPP_LIBRARY)
        set(SDBUSCPP_FOUND FALSE)
    endif(SDBUSCPP_INCLUDE_DIR AND SDBUSCPP_LIBRARY)

    set(SDBUSCPP_INCLUDE_DIRS ${SDBUSCPP_INCLUDE_DIR})
    set(SDBUSCPP_LIBRARIES ${SDBUSCPP_LIBRARY})
endif()