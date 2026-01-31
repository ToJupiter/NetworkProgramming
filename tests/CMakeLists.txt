cmake_minimum_required(VERSION 3.16)
project(QuizTests LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(test_client test_client.cpp)

target_include_directories(test_client PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../common)

# Linux networking libs are part of libc, but ensure for non-Windows
if(NOT WIN32)
    target_link_libraries(test_client PRIVATE)
else()
    message(WARNING "test_client uses POSIX sockets; build it in WSL/Linux.")
endif()
