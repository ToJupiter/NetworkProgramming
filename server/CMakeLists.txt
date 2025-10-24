cmake_minimum_required(VERSION 3.10.0)
project(Server VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(SERVER_SOURCES server.cpp)

# Build sqlite3 as static lib
add_library(sqlite3 STATIC thirdparty/sqlite3/sqlite3.c)
target_include_directories(sqlite3 PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/sqlite3)
set_target_properties(sqlite3 PROPERTIES LINKER_LANGUAGE C)

# header only sqlite_modern
add_library(sqlite_modern_cpp INTERFACE)
target_include_directories(sqlite_modern_cpp INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/sqlite_modern_cpp/hdr)
target_link_libraries(sqlite_modern_cpp INTERFACE sqlite3)


add_executable(server ${SERVER_SOURCES})
target_link_libraries(server PRIVATE sqlite3 sqlite_modern_cpp)
target_include_directories(server PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/sqlite3)




