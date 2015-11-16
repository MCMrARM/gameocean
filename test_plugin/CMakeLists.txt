cmake_minimum_required(VERSION 2.8)
project(test_plugin)

find_package(gameocean REQUIRED)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

add_library(test_plugin SHARED main.cpp)
target_link_libraries(test_plugin gameocean)