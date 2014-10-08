cmake_minimum_required (VERSION 2.8)

project(kalas)

set(CMAKE_BUILD_TYPE DEBUG)
set(CMAKE_C_FLAGS "-std=c99 -O0 -ggdb")
set(CMAKE_C_FLAGS_DEBUG "-std=c99 -O0 -ggdb")
set(CMAKE_C_FLAGS_RELEASE "-std=c99 -O0 -ggdb")

add_executable(matrix matrix.c)
target_link_libraries(matrix m)
