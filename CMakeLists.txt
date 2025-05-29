cmake_minimum_required(VERSION 3.10.0)
project(blockrs VERSION 0.1.0 LANGUAGES C CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

enable_testing()

add_subdirectory(tests)

if(UNIX)
    set(SERVER_SOURCES 
        server/nw.hpp 
        server/nw.cpp
    )
else()
    set(SERVER_SOURCES 
        server/win32.hpp 
        server/win32.cpp
    )
endif()

add_library(blockrs
    header.hpp

    utils/vec.hpp utils/vec.cpp
    utils/str.hpp utils/str.cpp

    json/json.hpp json/json.cpp

    server/server.hpp server/server.cpp
    server/classes.hpp server/classes.cpp
    server/httpheader.hpp server/httpheader.cpp
    ${SERVER_SOURCES}

    blockrs.hpp blockrs.cpp)

