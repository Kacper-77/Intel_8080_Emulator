#include "cpu/cpu.h"
#include "decoder/decoder.h"
#include <stdio.h>
#include <string.h>

int main() {
    CPU8080 cpu;
    cpu_init(&cpu);
    init_instruction_table();

    uint8_t program[] = {
        0xFB,       // EI
        // 0xF3,
        0x3E, 0x05, // MVI A, 
        0x06, 0x0A, // MVI B,
        0x80,       // ADD B
        0x00,       // NOP
        0x76        // HLT
    };

    uint8_t interrupt_handler[] = {
        0x3E, 0xFF, // MVI A, 0xFF
        0xC9        // RET
    };

    cpu_load_program(&cpu, program, sizeof(program), 0x0000);
    memcpy(&cpu.memory[0x28], interrupt_handler, sizeof(interrupt_handler));

    bool interrupt_triggered = false;

    while (cpu_emulate(&cpu)) {
        if (cpu.cycles > 20 && !interrupt_triggered) {
            request_interrupt(&cpu, 0xEF);
            interrupt_triggered = true;
        }
    }
    
    printf("A: %u\n", cpu.A);
    printf("B: %u\n", cpu.B);
    printf("Cycles sum: %llu\n", cpu.cycles);

    return 0;
}
