cmake_minimum_required(VERSION 3.15)
project(Intel_8080 C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED True)

file(GLOB_RECURSE SOURCES "src/*/*.c" "src/main.c")

add_executable(emulator8080 ${SOURCES})