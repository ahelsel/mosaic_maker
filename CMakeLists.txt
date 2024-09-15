cmake_minimum_required(VERSION 3.26)
project(mosaic_maker)

set(CMAKE_CXX_STANDARD 17)

add_executable(mosaic_maker main.cpp
        image_util/image_util.h
        image_util/image_util.c
        image_util/lodepng.h
        image_util/lodepng.c)
