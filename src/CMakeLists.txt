cmake_minimum_required(VERSION 3.2)

project(lbclient)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

find_package(OpenGL REQUIRED)
find_package(ZLIB REQUIRED)
find_package(PNG REQUIRED)
find_package(Threads REQUIRED)
find_package(GLEW REQUIRED)
find_package(PkgConfig REQUIRED)

pkg_search_module(GLFW REQUIRED glfw3)

#set(SOURCE_FILES ../clion/opengl.h)
include_directories(${ZLIB_INCLUDE_DIRS})
include_directories(${PNG_INCLUDE_DIRS})
include_directories(${OPENGL_INCLUDE_DIRS})
include_directories(${GLEW_INCLUDE_DIRS})
include_directories(../clion/)
include_directories(../libs/glm)
include_directories(common/)
include_directories(client/)

file(GLOB_RECURSE COMMON_SOURCES "common/*.cpp")
file(GLOB_RECURSE SERVER_SOURCES "server/*.cpp")
file(GLOB_RECURSE CLIENT_SOURCES "client/*.cpp")

add_executable(server ${COMMON_SOURCES} ${SERVER_SOURCES})
target_link_libraries(server ${ZLIB_LIBRARY} ${CMAKE_THREAD_LIBS_INIT})

add_executable(lbclient ${COMMON_SOURCES} ${CLIENT_SOURCES} client_linux/LinuxApp.cpp client_linux/Main.cpp)
target_link_libraries(lbclient ${ZLIB_LIBRARY} ${CMAKE_THREAD_LIBS_INIT} ${OPENGL_LIBRARIES} ${GLFW_STATIC_LIBRARIES} ${GLEW_LIBRARIES} ${PNG_LIBRARIES})
