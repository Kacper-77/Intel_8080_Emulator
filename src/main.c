#include "cpu/cpu.h"
#include <stdio.h>

int main() {
    CPU8080 cpu;
    cpu_init(&cpu);

    uint8_t program[] = {
        0x00, // NOP
        0x3E, // MVI A
        0xC3, // JMP
        0x80, // ADD
        0xC6, // ADI
        0xCE, //ACI
        0x76,  // HLT
        0x06  // MVI B
    };

    cpu_load_program(&cpu, program, sizeof(program), 0x0F09);
    cpu_emulate(&cpu);

    return 0;
}
