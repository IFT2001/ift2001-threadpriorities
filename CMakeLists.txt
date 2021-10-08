cmake_minimum_required(VERSION 3.17)
project(ThreadPriorities_c C)

set(CMAKE_C_STANDARD 11)

add_executable(ThreadPriorities_c main.c)

target_link_libraries(${PROJECT_NAME} pthread m)
