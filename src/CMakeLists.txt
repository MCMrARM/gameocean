cmake_minimum_required(VERSION 3.2)

project(gameocean)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

find_package(OpenGL REQUIRED)
find_package(ZLIB REQUIRED)
find_package(PNG REQUIRED)
find_package(Threads REQUIRED)
find_package(GLEW REQUIRED)
find_package(PkgConfig REQUIRED)

#SET(CMAKE_CXX_COMPILER "g++-4.8")
#SET(CMAKE_C_COMPILER "gcc-4.8")

pkg_search_module(GLFW REQUIRED glfw3)

#set(SOURCE_FILES ../clion/opengl.h)
include_directories(${ZLIB_INCLUDE_DIRS})
include_directories(${PNG_INCLUDE_DIRS})
include_directories(../libs/json)
include_directories(common/)
include_directories(server/)

file(GLOB_RECURSE COMMON_SOURCES "common/*.cpp")
file(GLOB_RECURSE SERVER_SOURCES "server/*.cpp")
file(GLOB_RECURSE SERVERAPP_SOURCES "server_app/*.cpp")
#file(GLOB_RECURSE CLIENT_SOURCES "client/*.cpp")
file(GLOB_RECURSE COMMON_HEADERS "common/*.h")
file(GLOB_RECURSE SERVER_HEADERS "server_app/*.h")
file(GLOB_RECURSE SERVERAPP_HEADERS "server/*.h")
#file(GLOB_RECURSE CLIENT_HEADERS "client/*.h")
LIST(APPEND COMMON_SOURCES ${COMMON_HEADERS})
LIST(APPEND SERVER_SOURCES ${SERVER_HEADERS})
LIST(APPEND SERVERAPP_SOURCES ${SERVERAPP_HEADERS})
#LIST(APPEND CLIENT_SOURCES ${CLIENT_HEADERS})

file(GLOB_RECURSE RAKNET_SOURCES "../libs/RakNet/*.cpp")
file(GLOB_RECURSE LIBJSON_SOURCES "../libs/json/*.cpp")

add_library(server SHARED ${COMMON_SOURCES} ${SERVER_SOURCES} ${RAKNET_SOURCES} ${LIBJSON_SOURCES})
target_link_libraries(server ${ZLIB_LIBRARY} ${CMAKE_THREAD_LIBS_INIT} ${PNG_LIBRARIES} ${CMAKE_DL_LIBS})
target_include_directories(server PUBLIC ../libs/RakNet)

#add_executable(client ${COMMON_SOURCES} ${CLIENT_SOURCES} ${LIBJSON_SOURCES} client_linux/LinuxApp.cpp client_linux/Main.cpp)
#target_link_libraries(client ${ZLIB_LIBRARY} ${CMAKE_THREAD_LIBS_INIT} ${OPENGL_LIBRARIES} ${GLFW_STATIC_LIBRARIES} ${GLEW_LIBRARIES} ${PNG_LIBRARIES})
#target_include_directories(client PUBLIC client/)
#target_include_directories(client PUBLIC client_linux/)
#target_include_directories(client PUBLIC ../libs/glm)
#target_include_directories(client PUBLIC ${OPENGL_INCLUDE_DIRS})
#target_include_directories(client PUBLIC ${GLEW_INCLUDE_DIRS})
#target_compile_definitions(client PRIVATE CLIENT)

install(DIRECTORY common/ DESTINATION include FILES_MATCHING PATTERN "*.h")
install(DIRECTORY server/ DESTINATION include FILES_MATCHING PATTERN "*.h")
install(TARGETS server DESTINATION lib)
install(FILES gameoceanConfig.cmake DESTINATION lib/cmake/gameocean)

add_executable(server_app ${SERVERAPP_SOURCES})
target_link_libraries(server_app server)