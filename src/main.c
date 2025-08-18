#include "cpu/cpu.h"
#include <stdio.h>

int main() {
    CPU8080 cpu;
    cpu_init(&cpu);

    uint8_t program[] = {
        0x00, // NOP
        0x3E, 0x05, // MVI A
        0x06, 0x0A,  // MVI B
        0x80, // ADD
        0x76,  // HLT
    };

    cpu_load_program(&cpu, program, sizeof(program), 0x0000);
    cpu_emulate(&cpu);

    printf("A: %u\n", cpu.A);

    return 0;
}
