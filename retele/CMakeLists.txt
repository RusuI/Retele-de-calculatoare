cmake_minimum_required(VERSION 3.17)
project(retele C)

set(CMAKE_C_STANDARD 99)

add_executable(retele insert_data.c createDB.cpp cmake-build-debug/data_base.cpp cmake-build-debug/version.c insert_data.c servTcpConc.c testari.c)