cmake_minimum_required(VERSION 3.15)

project(RRGame)

find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)
find_package(SDL2_ttf REQUIRED)


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
target_include_directories(RRGame PRIVATE
    ${SDL2_INCLUDE_DIR} ${SDL2_IMAGE_INCLUDE_DIR} ${SDL2_TTF_INCLUDE_DIR}
)
target_link_libraries(RRGame PRIVATE
    ${SDL2_LIBRARIES} ${SDL2_IMAGE_LIBRARIES} ${SDL2_TTF_LIBRARIES}
    RRColor
)