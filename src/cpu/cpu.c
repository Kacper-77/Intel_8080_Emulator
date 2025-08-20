#include "cpu.h"
#include "instructions.h"
#include "decoder.h"
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

void cpu_emulate(CPU8080 *cpu) {
    while (!cpu->halted) {
        uint8_t opcode = cpu->memory[cpu->PC];
        printf("PC: 0x%04X, Opcode: 0x%02X\n", cpu->PC, opcode);
        execute_instruction(cpu, opcode);
    }
}