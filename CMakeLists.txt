cmake_minimum_required(VERSION 3.7)
project(randdraw)

set(CMAKE_CXX_STANDARD 11)

# set(CMAKE_VERBOSE_MAKEFILE ON)

set(COMMON_FILES
        Picture.cpp
        Random.cpp
        Shapes.cpp
        ColorExtractor.cpp
        KMeans.cpp

        Picture.h
        Random.h
        Shapes.h
        ColorExtractor.h
        KMeans.h
)

set(RANDDRAW_FILES
        randdraw.cpp

        Program.cpp
        Processor.cpp

        Program.h
        Processor.h
)

add_executable(randdraw ${RANDDRAW_FILES} ${COMMON_FILES})

add_executable(extract-colors extract-colors.cpp ${COMMON_FILES})

