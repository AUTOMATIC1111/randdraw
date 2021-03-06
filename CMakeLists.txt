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
        CIEDE2000.cpp
        ColorExtractorKmeans.cpp
        ColorExtractorRandom.cpp
        ColorExtractorFromPicture.cpp
        VectorStack.cpp

        cxxopts.hpp
        Picture.h
        Random.h
        Shapes.h
        ColorExtractor.h
        KMeans.h
        CIEDE2000.h
        ColorExtractorKmeans.h
        ColorExtractorRandom.h
        ColorExtractorFromPicture.h
        VectorStack.h
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

