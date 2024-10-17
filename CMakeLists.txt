cmake_minimum_required(VERSION 3.26)
project(mosaic_maker)

set(CMAKE_CXX_STANDARD 17)

add_compile_definitions(LODEPNG_NO_COMPILE_CPP)

add_executable(mosaic_maker main.cpp
        image_util.h
        image_util.c
        lodepng/lodepng.h
        lodepng/lodepng.c
        MosaicImage.cpp
)
