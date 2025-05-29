include_directories(../)
add_executable(TEXE test.cpp)
target_link_libraries(TEXE blockrs)
add_test(TEST_EXE TEXE)