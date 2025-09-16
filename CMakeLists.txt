cmake_minimum_required(VERSION 3.15)
project(Intel_8080 C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED True)

set(SOURCES
    src/main.c
    src/cpu/cpu.c
    src/utils/helpers.c
    src/decoder/decoder.c
    src/instructions/data_transfer.c
    src/instructions/arithmetic.c
    src/instructions/logical.c
    src/instructions/branch_stack.c
    src/instructions/control_io.c
    src/io_simulation/ports.c
)

add_executable(emulator8080 ${SOURCES})