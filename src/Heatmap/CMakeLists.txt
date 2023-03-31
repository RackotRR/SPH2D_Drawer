cmake_minimum_required(VERSION 3.15)

project(RRHeatMap)

add_library(RRHeatMap STATIC src/HeatMap.cpp)
target_include_directories(RRHeatMap PUBLIC include)
target_link_libraries(RRHeatMap PUBLIC RRColor)