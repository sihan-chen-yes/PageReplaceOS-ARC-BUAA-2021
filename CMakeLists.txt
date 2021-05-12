cmake_minimum_required(VERSION 3.19)
project(ARCchain)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_EXE_LINKER_FLAGS "-static")
add_executable(ARCchain main.cpp tmp.cpp)