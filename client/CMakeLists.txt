cmake_minimum_required(VERSION 3.0 FATAL_ERROR)

project(client VERSION 1.0 LANGUAGES C)
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

include_directories(${PROJECT_SOURCE_DIR})
add_executable(client client.c)
