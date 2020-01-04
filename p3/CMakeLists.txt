cmake_minimum_required(VERSION 3.15)
project(p3)

set(CMAKE_CXX_STANDARD 99)

include_directories(.)
include_directories(tests)

add_executable(p3
        tests/free2.c
        heapAlloc.c
        )

link_libraries(
        heapAlloc.h
)
