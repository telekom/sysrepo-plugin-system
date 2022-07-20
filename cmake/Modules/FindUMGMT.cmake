if(UMGMT_LIBRARIES AND UMGMT_INCLUDE_DIRS)
    set(UMGMT_FOUND TRUE)
else()
    find_path(
        UMGMT_INCLUDE_DIR
        NAMES umgmt.h
        PATHS /usr/include /usr/local/include /opt/local/include /sw/include ${CMAKE_INCLUDE_PATH} ${CMAKE_INSTALL_PREFIX}/include
    )

    find_library(
        UMGMT_LIBRARY
        NAMES umgmt
        PATHS /usr/lib /usr/lib64 /usr/local/lib /usr/local/lib64 /opt/local/lib /sw/lib ${CMAKE_LIBRARY_PATH} ${CMAKE_INSTALL_PREFIX}/lib
    )

    if(UMGMT_INCLUDE_DIR AND UMGMT_LIBRARY)
        set(UMGMT_FOUND TRUE)
    else(UMGMT_INCLUDE_DIR AND UMGMT_LIBRARY)
        set(UMGMT_FOUND FALSE)
    endif(UMGMT_INCLUDE_DIR AND UMGMT_LIBRARY)

    set(UMGMT_INCLUDE_DIRS ${UMGMT_INCLUDE_DIR})
    set(UMGMT_LIBRARIES ${UMGMT_LIBRARY})
endif()