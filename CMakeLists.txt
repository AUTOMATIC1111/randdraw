cmake_minimum_required(VERSION 3.7)
project(randdraw)

set(CMAKE_CXX_STANDARD 11)

# set(CMAKE_VERBOSE_MAKEFILE ON)

set(SOURCE_FILES main.cpp Picture.cpp Picture.h Random.cpp Random.h Shapes.cpp Shapes.h)
add_executable(randdraw ${SOURCE_FILES})
