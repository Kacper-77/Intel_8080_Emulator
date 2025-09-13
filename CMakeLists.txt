cmake_minimum_required(VERSION 3.15)
project(Intel_8080 C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED True)

set(SOURCES
    src/main.c
    src/cpu/cpu.c
    src/utils/helpers.c
    src/decoder/decoder.c
    src/cpu/data_transfer.c
    src/cpu/arithmetic.c
    src/cpu/logical.c
    src/cpu/branch_stack.c
    src/cpu/control_io.c
    src/utils/io_simulation/ports.c
)

add_executable(emulator8080 ${SOURCES})