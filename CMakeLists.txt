cmake_minimum_required(VERSION 3.15)

project(SPH2D_Drawer)
set(CMAKE_CXX_STANDARD 20)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake/")
set(SPH2D_DRAWER_FONTS_DIR "${CMAKE_CURRENT_SOURCE_DIR}/fonts")


if (NOT CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
    # open as submodule
    set(SPH2D_DRAWER_INSTALL_DESTINATION "RRSPH" CACHE PATH "Install destination")
else() 
    # open independently
    set(SPH2D_DRAWER_INSTALL_DESTINATION "SPH2D_Drawer" CACHE PATH "Install destination")
endif()

add_subdirectory(src)