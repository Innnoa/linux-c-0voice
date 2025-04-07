cmake_minimum_required(VERSION 3.25.2)
project(2_1_1_1)

set(CMAKE_CXX_STANDARD 20)

add_executable(2_1_1_1 main.cpp)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
