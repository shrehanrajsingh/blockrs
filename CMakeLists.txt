cmake_minimum_required(VERSION 3.10.0)
project(blockrs VERSION 0.1.0 LANGUAGES C CXX)

enable_testing()

add_subdirectory(tests)

add_library(blockrs
    header.hpp

    utils/vec.hpp utils/vec.cpp
    utils/str.hpp utils/str.cpp

    blockrs.hpp blockrs.cpp)

