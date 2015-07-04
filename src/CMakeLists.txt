cmake_minimum_required(VERSION 3.2)

project(lbclient)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

find_package(OpenGL)

#set(SOURCE_FILES ../clion/opengl.h)
include_directories(../clion/)
include_directories(../libs/)
include_directories(common/)
include_directories(client/)

file(GLOB_RECURSE COMMON_SOURCES "common/*.cpp")
file(GLOB_RECURSE SERVER_SOURCES "server/*.cpp")

add_executable(lbclient ${COMMON_SOURCES} ${SERVER_SOURCES})
