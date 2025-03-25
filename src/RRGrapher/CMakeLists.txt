cmake_minimum_required(VERSION 3.15)

project(RRGrapher)

add_executable(SPH2D_Drawer
    src/RRGrapher.cpp
    src/RRGrapherInput.cpp
    src/Source.cpp
)
target_link_libraries(SPH2D_Drawer PRIVATE
    RRGame
    RRHeatMap
)

if (NOT TARGET RRSPH_FIO)
    find_package(RRSPH_FIO REQUIRED)
    add_library(RRSPH_FIO INTERFACE)
    target_link_libraries(RRSPH_FIO INTERFACE ${RRSPH_FIO_LIBRARIES})
    target_include_directories(RRSPH_FIO INTERFACE ${RRSPH_FIO_INCLUDE_DIRECTORIES})
endif()

target_link_libraries(SPH2D_Drawer PRIVATE RRSPH_FIO)

install(TARGETS SPH2D_Drawer DESTINATION ${SPH2D_DRAWER_INSTALL_DESTINATION})
install(DIRECTORY ${SPH2D_DRAWER_FONTS_DIR} DESTINATION ${SPH2D_DRAWER_INSTALL_DESTINATION})