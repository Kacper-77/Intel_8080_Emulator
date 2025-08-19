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
        case 0x76: cpu->halted = true; break;  // HLT
        
        case 0x06: mvi(&cpu->B, cpu); break;  // MVI B, data
        case 0x0E: mvi(&cpu->C, cpu); break;  // MVI C, data
        case 0x16: mvi(&cpu->D, cpu); break;  // MVI D, data
        case 0x1E: mvi(&cpu->E, cpu); break;  // MVI E, data
        case 0x26: mvi(&cpu->H, cpu); break;  // MVI H, data
        case 0x2E: mvi(&cpu->L, cpu); break;  // MVI L, data
        case 0x36: mvi(NULL, cpu); break;  // MVI M, data
        case 0x3E: mvi(&cpu->A, cpu); break; // MVI A, data

        // MOV
        case 0x40: mov(&cpu->B, &cpu->B, cpu); break;
        case 0x41: mov(&cpu->B, &cpu->C, cpu); break;
        case 0x42: mov(&cpu->B, &cpu->D, cpu); break;
        case 0x43: mov(&cpu->B, &cpu->E, cpu); break;
        case 0x44: mov(&cpu->B, &cpu->H, cpu); break;
        case 0x45: mov(&cpu->B, &cpu->L, cpu); break;
        case 0x46: mov(&cpu->B, NULL, cpu); break;    // B, M
        case 0x47: mov(&cpu->B, &cpu->A, cpu); break;

        case 0x48: mov(&cpu->C, &cpu->B, cpu); break;
        case 0x49: mov(&cpu->C, &cpu->C, cpu); break;
        case 0x4A: mov(&cpu->C, &cpu->D, cpu); break;
        case 0x4B: mov(&cpu->C, &cpu->E, cpu); break;
        case 0x4C: mov(&cpu->C, &cpu->H, cpu); break;
        case 0x4D: mov(&cpu->C, &cpu->L, cpu); break;
        case 0x4E: mov(&cpu->C, NULL, cpu); break;    // C, M
        case 0x4F: mov(&cpu->C, &cpu->A, cpu); break;

        case 0x50: mov(&cpu->D, &cpu->B, cpu); break;
        case 0x51: mov(&cpu->D, &cpu->C, cpu); break;
        case 0x52: mov(&cpu->D, &cpu->D, cpu); break;
        case 0x53: mov(&cpu->D, &cpu->E, cpu); break;
        case 0x54: mov(&cpu->D, &cpu->H, cpu); break;
        case 0x55: mov(&cpu->D, &cpu->L, cpu); break;
        case 0x56: mov(&cpu->D, NULL, cpu); break;    // D, M
        case 0x57: mov(&cpu->D, &cpu->A, cpu); break;

        case 0x58: mov(&cpu->E, &cpu->B, cpu); break;
        case 0x59: mov(&cpu->E, &cpu->C, cpu); break;
        case 0x5A: mov(&cpu->E, &cpu->D, cpu); break;
        case 0x5B: mov(&cpu->E, &cpu->E, cpu); break;
        case 0x5C: mov(&cpu->E, &cpu->H, cpu); break;
        case 0x5D: mov(&cpu->E, &cpu->L, cpu); break;
        case 0x5E: mov(&cpu->E, NULL, cpu); break;    // E, M
        case 0x5F: mov(&cpu->E, &cpu->A, cpu); break;

        case 0x60: mov(&cpu->H, &cpu->B, cpu); break;
        case 0x61: mov(&cpu->H, &cpu->C, cpu); break;
        case 0x62: mov(&cpu->H, &cpu->D, cpu); break;
        case 0x63: mov(&cpu->H, &cpu->E, cpu); break;
        case 0x64: mov(&cpu->H, &cpu->H, cpu); break;
        case 0x65: mov(&cpu->H, &cpu->L, cpu); break;
        case 0x66: mov(&cpu->H, NULL, cpu); break;    // H, M
        case 0x67: mov(&cpu->H, &cpu->A, cpu); break;

        case 0x68: mov(&cpu->L, &cpu->B, cpu); break;
        case 0x69: mov(&cpu->L, &cpu->C, cpu); break;
        case 0x6A: mov(&cpu->L, &cpu->D, cpu); break;
        case 0x6B: mov(&cpu->L, &cpu->E, cpu); break;
        case 0x6C: mov(&cpu->L, &cpu->H, cpu); break;
        case 0x6D: mov(&cpu->L, &cpu->L, cpu); break;
        case 0x6E: mov(&cpu->L, NULL, cpu); break;    // L, M
        case 0x6F: mov(&cpu->L, &cpu->A, cpu); break;

        case 0x70: mov(NULL, &cpu->B, cpu); break;    // M, B
        case 0x71: mov(NULL, &cpu->C, cpu); break;    // M, C
        case 0x72: mov(NULL, &cpu->D, cpu); break;    // M, D
        case 0x73: mov(NULL, &cpu->E, cpu); break;    // M, E
        case 0x74: mov(NULL, &cpu->H, cpu); break;    // M, H
        case 0x75: mov(NULL, &cpu->L, cpu); break;    // M, L
        case 0x77: mov(NULL, &cpu->A, cpu); break;    // M, A

        case 0x78: mov(&cpu->A, &cpu->B, cpu); break;
        case 0x79: mov(&cpu->A, &cpu->C, cpu); break;
        case 0x7A: mov(&cpu->A, &cpu->D, cpu); break;
        case 0x7B: mov(&cpu->A, &cpu->E, cpu); break;
        case 0x7C: mov(&cpu->A, &cpu->H, cpu); break;
        case 0x7D: mov(&cpu->A, &cpu->L, cpu); break;
        case 0x7E: mov(&cpu->A, NULL, cpu); break;     // A, M
        case 0x7F: mov(&cpu->A, &cpu->A, cpu); break;

        ///////////////////////////////////////////
        case 0x04: inr(&cpu->B, cpu, false); break;  // INR B
        case 0x0C: inr(&cpu->C, cpu, false); break;  // INR C
        case 0x14: inr(&cpu->D, cpu, false); break;  // INR D
        case 0x1C: inr(&cpu->E, cpu, false); break;  // INR E
        case 0x24: inr(&cpu->H, cpu, false); break;  // INR H
        case 0x2C: inr(&cpu->L, cpu, false); break;  // INR L
        case 0x34: inr(NULL, cpu, true);      break;  // INR M
        case 0x3C: inr(&cpu->A, cpu, false); break;  // INR A

        case 0x05: dcr(&cpu->B, cpu, false); break;  // DCR B
        case 0x0D: dcr(&cpu->C, cpu, false); break;  // DCR C
        case 0x15: dcr(&cpu->D, cpu, false); break;  // DCR D
        case 0x1D: dcr(&cpu->E, cpu, false); break;  // DCR E
        case 0x25: dcr(&cpu->H, cpu, false); break;  // DCR H
        case 0x2D: dcr(&cpu->L, cpu, false); break;  // DCR L
        case 0x35: dcr(NULL, cpu, true);      break;  // DCR M
        case 0x3D: dcr(&cpu->A, cpu, false); break;  // DCR A

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