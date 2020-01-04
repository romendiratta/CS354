cmake_minimum_required(VERSION 3.15)
project(p2B)

set(CMAKE_CXX_STANDARD 99)

include_directories(.)

add_executable(p2B
        hill1.txt
        hill2.txt
        hill3.txt
        hill4.txt
        hill_climb.c)
