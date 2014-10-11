cmake_minimum_required (VERSION 2.8)

project(kalas)

set(CMAKE_BUILD_TYPE DEBUG)
set(CMAKE_C_FLAGS "-std=c99 -O0 -ggdb")
set(CMAKE_C_FLAGS_DEBUG "-std=c99 -O0 -ggdb")
set(CMAKE_C_FLAGS_RELEASE "-std=c99 -O0 -ggdb")

set(ACMLROOT "/opt/acml6.0.6/gfortran64")

include_directories( "/opt/clBLAS/include" "${ACMLROOT}/include")
link_directories( "/opt/clBLAS/lib64" "${ACMLROOT}/lib")

add_executable(matrix matrix.c)
target_link_libraries(matrix m clBLAS acml)
