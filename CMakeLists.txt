cmake_minimum_required(VERSION 3.15)

project(SPH2D_Drawer)
set(CMAKE_CXX_STANDARD 20)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake/")
set(SPH2D_DRAWER_FONTS_DIR "${CMAKE_CURRENT_SOURCE_DIR}/fonts")


if (NOT CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
    # open as submodule
    if (NOT TARGET SPH2D_FIO) 
        find_package(SPH2D_FIO REQUIRED)
    endif()
    set(SPH2D_DRAWER_INSTALL_DESTINATION "SPH2D" CACHE PATH "Install destination")
else() 
    # open independently
    find_package(SPH2D_FIO REQUIRED)
    set(SPH2D_DRAWER_INSTALL_DESTINATION "SPH2D_Drawer" CACHE PATH "Install destination")
endif()

add_subdirectory(src)