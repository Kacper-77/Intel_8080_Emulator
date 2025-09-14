#include "cpu/cpu.h"
#include "decoder/decoder.h"
#include <stdio.h>
#include <string.h>
#include "io_simulation/ports.h"

int main() {
    CPU8080 cpu;
    cpu_init(&cpu);
    init_instruction_table();
    preload_keys();

    /*
        COPY and PASTE program HERE
        from test/..
        or write your own :)
        or use this one below
    */

    uint8_t program[] = {
        0xFB,       // EI
        0x26, 0x30,
        0x2E, 0x00,
        // 0xF3,  // DI
        // 0xEF,  // RST
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

    // uint8_t call_target[] = {
    //     0x96, // SUB M
    //     0xC9  // RET
    // };

    uint8_t trap_handler[] = {
    0x96,
    0xC9            // RET
    };

    cpu_load_program(&cpu, program, sizeof(program), 0x0000);
    // memcpy(&cpu.memory[0x0020], call_target, sizeof(call_target));
    memcpy(&cpu.memory[0x28], interrupt_handler, sizeof(interrupt_handler));
    memcpy(&cpu.memory[0x24], trap_handler, sizeof(trap_handler));
    // cpu.memory[0x3000] = 55;
    // cpu.memory[0x0030] = 0x76;

    bool interrupt_triggered = false;
    bool trigerred = false;

    while (cpu_emulate(&cpu)) {
        if (cpu.cycles > 20 && !interrupt_triggered) {
            request_interrupt(&cpu, 0xEF);
            interrupt_triggered = true;
            printf("\nInterrupted here:\n");
        }

        if (cpu.cycles > 50 && !trigerred) {
            trigger_trap(&cpu);
            trigerred = true;
            printf("\nTrap here:\n");
        }
    }
    
    printf("\n=== CPU STATE ===\n");
    printf("A: 0x%02X (%u)\n", cpu.A, cpu.A);
    printf("B: 0x%02X (%u)\n", cpu.B, cpu.B);
    printf("C: 0x%02X (%u)\n", cpu.C, cpu.C);
    printf("D: 0x%02X (%u)\n", cpu.D, cpu.D);
    printf("E: 0x%02X (%u)\n", cpu.E, cpu.E);
    printf("H: 0x%02X (%u)\n", cpu.H, cpu.H);
    printf("L: 0x%02X (%u)\n", cpu.L, cpu.L);
    printf("SP: 0x%04X\n", cpu.SP);
    printf("PC: 0x%04X\n", cpu.PC);
    printf("HL: 0x%04X\n", (cpu.H << 8) | cpu.L);

    printf("\n=== FLAGS ===\n");
    printf("Z (Zero):      %u\n", cpu.flags.Z);
    printf("S (Sign):      %u\n", cpu.flags.S);
    printf("P (Parity):    %u\n", cpu.flags.P);
    printf("CY (Carry):    %u\n", cpu.flags.CY);
    printf("AC (Aux Carry):%u\n", cpu.flags.AC);

    printf("\n=== CYCLES ===\n");
    printf("Total cycles: %llu\n", cpu.cycles);

    return 0;
}
