#cmake_minimum_required(VERSION 3.26)
#project(mosaic_maker)
#
#set(CMAKE_CXX_STANDARD 17)
#
#add_compile_definitions(LODEPNG_NO_COMPILE_CPP)
#
#add_executable(mosaic_maker main.cpp
#        image_util.h
#        image_util.c
#        external/lodepng/lodepng.h
#        external/lodepng/lodepng.c
#        MosaicImage.cpp
#)

cmake_minimum_required(VERSION 3.26)
project(mosaic_maker)

set(CMAKE_CXX_STANDARD 17)

add_compile_definitions(LODEPNG_NO_COMPILE_CPP)

# Find SDL2 package using CONFIG mode
find_package(SDL2 CONFIG REQUIRED)

# Optionally define SDL_MAIN_HANDLED on Windows
if(WIN32)
    add_definitions(-DSDL_MAIN_HANDLED)
endif()

add_executable(mosaic_maker
        main.cpp
        image_util.h
        image_util.c
        external/lodepng/lodepng.h
        external/lodepng/lodepng.c
        MosaicImage.cpp
)

# Link SDL2 libraries
target_link_libraries(mosaic_maker PRIVATE
        $<TARGET_NAME_IF_EXISTS:SDL2::SDL2main>
        $<IF:$<TARGET_EXISTS:SDL2::SDL2>,SDL2::SDL2,SDL2::SDL2-static>
)
