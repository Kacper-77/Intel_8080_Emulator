#include "cpu.h"
#include <stdio.h>
#include <string.h>


void cpu_init(CPU8080 *cpu) {
    memset(cpu, 0, sizeof(CPU8080));
    cpu->SP = 0xFFFF;
    cpu->PC = 0x0000;
    cpu->halted = false;
}

void cpu_load_program(CPU8080 *cpu, const uint8_t *program, uint16_t size, uint16_t load_addr) {
    memcpy(&cpu->memory[load_addr], program, size);
    cpu->PC = load_addr;
}

void execute_instruction(CPU8080 *cpu, uint8_t opcode) {
    switch (opcode) {
        case 0x00:  // NOP
            break;
        default:
            printf("Unknown opcode: 0x%02X\n", opcode);
            cpu->halted = true;
            break;
    }
}

void cpu_emulate(CPU8080 *cpu) {
    while (!cpu->halted) {
        uint8_t opcode = cpu->memory[cpu->PC];
        printf("PC: 0x%04X, Opcode: 0x%02X\n", cpu->PC, opcode);
        cpu->PC += 1;
        execute_instruction(cpu, opcode);
    }
}