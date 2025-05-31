cmake_minimum_required(VERSION 3.10.0)
project(blockrs VERSION 0.1.0 LANGUAGES C CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

enable_testing()

find_package(OpenSSL REQUIRED)

# add_definitions(-DSECP256K1_ENABLE_MODULE_RECOVERY)
# set(SECP256K1_ENABLE_MODULE_RECOVERY ON CACHE BOOL "Enable recovery module")
# set(SECP256K1_BUILD_STATIC ON CACHE BOOL "Build static lib")
# set(SECP256K1_BUILD_SHARED OFF CACHE BOOL "Don't build shared lib")
# set(SECP256K1_BUILD_EXAMPLES OFF CACHE BOOL "Don't build examples")
# set(SECP256K1_BUILD_TESTS OFF CACHE BOOL "Don't build tests")

find_library(SECP256K1_LIB secp256k1 REQUIRED)

find_path(SECP256K1_INCLUDE_DIR
    NAMES secp256k1.h
    PATHS /usr/local/include /usr/include
)

add_subdirectory(tests)
# add_subdirectory(lib/secp256k1)

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

    lib/tiny_sha3/sha3.h lib/tiny_sha3/sha3.c

    blockchain/node.hpp blockchain/node.cpp
    blockchain/blocknetwork.hpp blockchain/blocknetwork.cpp
    blockchain/transaction.hpp blockchain/transaction.cpp
    blockchain/wallet.hpp blockchain/wallet.cpp
    blockchain/nodeserver.hpp blockchain/nodeserver.cpp
    blockchain/bnserver.hpp blockchain/bnserver.cpp

    blockrs.hpp blockrs.cpp)

target_include_directories(
    blockrs PUBLIC
    ${OPENSSL_INCLUDE_DIR}
    ${SECP256K1_INCLUDE_DIR}
    # lib/secp256k1/include
    # /usr/local/include
    lib/tiny_sha3
)

target_link_libraries(blockrs ${SECP256K1_LIB} OpenSSL::Crypto)