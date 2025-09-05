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
        0x26, 0x30,
        0x2E, 0x00,
        // 0xF3,
        // 0xEF,
        0x3E, 0x05, // MVI A 
        0x06, 0x0A, // MVI B
        0x80,       // ADD B
        0x00,       // NOP
        
        0x21, 0x34, 0x12, // LXI H, 0x1234
        0x11, 0x11, 0x11, // LXI D, 0x1111
        0x19,             // DAD D
        0x76              // HLT
    };

    uint8_t interrupt_handler[] = {
        0x3E, 0xFF, // MVI A, 0xFF
        0xC9        // RET
    };

    uint8_t trap_handler[] = {
    0x96,
    0xC9            // RET
};

    cpu_load_program(&cpu, program, sizeof(program), 0x0000);
    memcpy(&cpu.memory[0x28], interrupt_handler, sizeof(interrupt_handler));
    memcpy(&cpu.memory[0x24], trap_handler, sizeof(trap_handler));
    cpu.memory[0x3000] = 55;

    bool interrupt_triggered = false;
    bool trigerred = false;

    while (cpu_emulate(&cpu)) {
        if (cpu.cycles > 20 && !interrupt_triggered) {
            request_interrupt(&cpu, 0xEF);
            interrupt_triggered = true;
        }

        if (cpu.cycles > 50 && !trigerred) {
            trigger_trap(&cpu);
            trigerred = true;
            printf("TRAP HERE\n");
        }
    }
    
    printf("A: %u\n", cpu.A);
    printf("B: %u\n", cpu.B);
    printf("Cycles sum: %llu\n", cpu.cycles);

    printf("H: %02X\n", cpu.H);
    printf("L: %02X\n", cpu.L);
    printf("HL: %04X\n", (cpu.H << 8) | cpu.L);
    printf("CY: %u\n", cpu.flags.CY);

    return 0;
}
