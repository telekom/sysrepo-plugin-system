if(SYSTEMD_LIBRARIES AND SYSTEMD_INCLUDE_DIRS)
    set(SYSTEMD_FOUND TRUE)
else()
    find_path(
        SYSTEMD_INCLUDE_DIR
        NAMES systemd/sd-bus.h
        PATHS /usr/include /usr/local/include /opt/local/include /sw/include ${CMAKE_INCLUDE_PATH} ${CMAKE_INSTALL_PREFIX}/include
    )

    find_library(
        SYSTEMD_LIBRARY
        NAMES systemd
        PATHS /usr/lib /usr/lib64 /usr/local/lib /usr/local/lib64 /opt/local/lib /sw/lib ${CMAKE_LIBRARY_PATH} ${CMAKE_INSTALL_PREFIX}/lib
    )

    if(SYSTEMD_INCLUDE_DIR AND SYSTEMD_LIBRARY)
        set(SYSTEMD_FOUND TRUE)
    else(SYSTEMD_INCLUDE_DIR AND SYSTEMD_LIBRARY)
        set(SYSTEMD_FOUND FALSE)
    endif(SYSTEMD_INCLUDE_DIR AND SYSTEMD_LIBRARY)

    set(SYSTEMD_INCLUDE_DIRS ${SYSTEMD_INCLUDE_DIR})
    set(SYSTEMD_LIBRARIES ${SYSTEMD_LIBRARY})
endif()
