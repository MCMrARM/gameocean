cmake_minimum_required(VERSION 3.2)
project(lbclient)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

find_package(OpenGL)

#set(SOURCE_FILES ../clion/opengl.h)
include_directories(../clion/)
include_directories(../libs/)

add_executable(lbclient ${SOURCE_FILES} App.cpp App.h)
target_link_libraries(lbclient ../libs/)