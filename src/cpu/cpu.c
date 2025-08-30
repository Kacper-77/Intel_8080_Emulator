#include "cpu.h"
#include "instructions.h"
#include "../decoder/decoder.h"
#include <stdio.h>
#include <string.h>


const uint8_t t_states[256] = {
    4, 10, 7, 5, 5, 7, 7, 4, 4, 10, 7, 5, 5, 7, 4, 4,             // 0x00 - 0x0F
    4, 10, 7, 5, 5, 7, 4, 4, 4, 10, 7, 5, 5, 7, 4, 4,             // 0x10 - 0x1F
    4, 10, 7, 5, 5, 7, 4, 4, 4, 10, 7, 5, 5, 7, 4, 4,             // 0x20 - 0x2F
    4, 10, 7, 5, 5, 7, 4, 4, 4, 10, 7, 5, 5, 7,7, 4,              // 0x30 - 0x3F
    5, 5, 5, 5, 5, 5, 7, 5, 5, 5, 5, 5, 5, 5, 7, 5,               // 0x40 - 0x4F
    5, 5, 5, 5, 5, 5, 7, 5, 5, 5, 5, 5, 5, 5, 7, 5,               // 0x50 - 0x5F
    5, 5, 5, 5, 5, 5, 7, 5, 5, 5, 5, 5, 5, 5, 7, 5,               // 0x60 - 0x6F
    7, 7, 7, 7, 7, 7, 7, 5, 5, 5, 5, 5, 5, 5, 7, 5,               // 0x70 - 0x7F
    4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,               // 0x80 - 0x8F
    4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,               // 0x90 - 0x9F
    4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,               // 0xA0 - 0xAF
    4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,               // 0xB0 - 0xBF
    5, 10, 10, 10, 10, 11, 11, 11, 5, 10, 10, 10, 10, 11, 11, 11, // 0xC0 - 0xCF
    5, 10, 10, 10, 10, 11, 11, 11, 5, 10, 10, 10, 10, 11, 11, 11, // 0xD0 - 0xDF
    5, 10, 10, 10, 18, 11, 11, 11, 5, 10, 10, 10, 10, 11, 11, 11, // 0xE0 - 0xEF
    5, 10, 10, 10, 10, 11, 11, 11, 5, 10, 10, 10, 10, 11, 11, 11  // 0xF0 - 0xFF
};

void cpu_init(CPU8080 *cpu) {
    memset(cpu, 0, sizeof(CPU8080));
    cpu->SP = 0xFFFF;
    cpu->PC = 0x0000;
    cpu->halted = false;

    cpu->registers[0] = &cpu->B;
    cpu->registers[1] = &cpu->C;
    cpu->registers[2] = &cpu->D;
    cpu->registers[3] = &cpu->E;
    cpu->registers[4] = &cpu->H;
    cpu->registers[5] = &cpu->L;
    cpu->registers[6] = NULL;
    cpu->registers[7] = &cpu->A;
}

void cpu_load_program(CPU8080 *cpu, const uint8_t *program, uint16_t size, uint16_t load_addr) {
    memcpy(&cpu->memory[load_addr], program, size);
    cpu->PC = load_addr;
}

void cpu_emulate(CPU8080 *cpu) {
    while (!cpu->halted) {
        uint8_t opcode = cpu->memory[cpu->PC];
        printf("PC: 0x%04X, Opcode: 0x%02X, Cycles: %llu\n", cpu->PC, opcode, cpu->cycles);
        execute_instruction(cpu, opcode);
    }
}