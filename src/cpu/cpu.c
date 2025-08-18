#include "cpu.h"
#include "instructions.h"
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
        case 0x00: cpu->PC += 1;break;  // NOP
        
        case 0x3E: mvi(&cpu->A, cpu); break; // MVI A, data
        case 0x06: mvi(&cpu->B, cpu); break;  // MVI B, data
        case 0x0E: mvi(&cpu->C, cpu); break;  // MVI C, data
        case 0x16: mvi(&cpu->D, cpu); break;  // MVI D, data
        case 0x1E: mvi(&cpu->E, cpu); break;  // MVI E, data
        case 0x26: mvi(&cpu->H, cpu); break;  // MVI H, data
        case 0x2E: mvi(&cpu->L, cpu); break;  // MVI L, data

        case 0x80: add(cpu->B, cpu); break;  // ADD B
        case 0x81: add(cpu->C, cpu); break;  // ADD C
        case 0x82: add(cpu->D, cpu); break;  // ADD D
        case 0x83: add(cpu->E, cpu); break;  // ADD E
        case 0x84: add(cpu->H, cpu); break;  // ADD H
        case 0x85: add(cpu->L, cpu); break;  // ADD L
        case 0x86: add(cpu->memory[(cpu->H << 8) | cpu->L], cpu); break;  // ADD M (HL)
        case 0x87: add(cpu->A, cpu); break;  // ADD A

        case 0xC6: adi(cpu); break;  // ADI
        case 0xCE: aci(cpu); break;  // ACI
        case 0xC3: jmp(cpu); break;  // JMP

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
        execute_instruction(cpu, opcode);
    }
}