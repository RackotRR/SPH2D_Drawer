cmake_minimum_required(VERSION 3.20)

project(RRGame)

find_package(SDL)
if(NOT SDL_FOUND)
    message(FATAL_ERROR "Can't find SDL")
endif()

find_package(SDL_image)
if(NOT SDL_IMAGE_FOUND)
    message(FATAL_ERROR "Can't find SDL_image")
endif()

find_package(SDL_ttf)
if(NOT SDL_TTF_FOUND)
    message(FATAL_ERROR "Can't find SDL_ttf")
endif()

add_library(RRGame STATIC
    src/RRGameIO_SDL.cpp
    src/RRController.cpp
    src/RRTextOutput.cpp
    src/RRTextureManager.cpp
    src/RRGameTime.cpp
    src/Rectangle.cpp
    src/Vector2.cpp
)
target_include_directories(RRGame PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(RRGame
    SDL::SDL
)