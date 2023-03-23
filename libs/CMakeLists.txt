cmake_minimum_required(VERSION 3.20)

project(SPH2D_Drawer_LIBS)

add_library(RRColor INTERFACE)
target_include_directories(RRColor INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/RRColor)

add_library(HeatMap STATIC ${CMAKE_CURRENT_SOURCE_DIR}/Heatmap/src/HeatMap.cpp)
target_include_directories(HeatMap PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/Heatmap/include)
target_link_libraries(HeatMap RRColor)