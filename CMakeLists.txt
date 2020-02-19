cmake_minimum_required(VERSION 3.16)

set(CMAKE_C_STANDARD 90)
project(${PROJECT_NAME} C)
add_executable(${PROJECT_NAME})

add_subdirectory(src)

target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Werror -ansi -pedantic -D_POSIX_C_SOURCE=200112L)