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

find_package(SPH2D_FIO)
target_link_libraries(SPH2D_Drawer PRIVATE ${SPH2D_FIO_LIBRARIES})
target_include_directories(SPH2D_Drawer PRIVATE ${SPH2D_FIO_INCLUDE_DIRECTORIES})

install(TARGETS SPH2D_Drawer DESTINATION ${SPH2D_DRAWER_INSTALL_DESTINATION})
install(DIRECTORY ${CMAKE_SOURCE_DIR}/fonts DESTINATION ${SPH2D_DRAWER_INSTALL_DESTINATION})