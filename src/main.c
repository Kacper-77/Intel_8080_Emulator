#include "cpu/cpu.h"
#include "decoder/decoder.h"
#include <stdio.h>

int main() {
    CPU8080 cpu;
    cpu_init(&cpu);

    init_instruction_table();

    uint8_t program[] = {
        0x00, // NOP  4
        0x3E, 0x05, // MVI A  7
        0x06, 0x0A,  // MVI B  7
        0x80, // ADD  4
        0x76,  // HLT  7
    };

    cpu_load_program(&cpu, program, sizeof(program), 0x0000);
    cpu_emulate(&cpu);

    printf("A: %u\n", cpu.A);
    printf("B = %u\n", cpu.B);
    printf("Cycles sum: %llu\n", cpu.cycles);

    return 0;
}
