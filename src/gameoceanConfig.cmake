add_library(gameocean STATIC IMPORTED)
find_library(GAMEOCEAN_LIBRARY "server" HINTS "${CMAKE_CURRENT_LIST_DIR}/../../")
set_target_properties(gameocean PROPERTIES IMPORTED_LOCATION "${GAMEOCEAN_LIBRARY}")
add_definitions(-DSERVER)