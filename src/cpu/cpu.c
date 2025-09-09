#include "cpu.h"
#include "instructions.h"
#include "../decoder/decoder.h"
#include <stdio.h>
#include <string.h>


const uint8_t t_states[256] = {
    4, 10, 7, 5, 5, 7, 7, 4, 4, 10, 7, 5, 5, 7, 4, 4,             // 0x00 - 0x0F
    4, 10, 7, 5, 5, 7, 4, 4, 4, 10, 7, 5, 5, 7, 4, 4,             // 0x10 - 0x1F
    4, 10, 16, 5, 5, 7, 4, 4, 4, 10, 16, 5, 5, 7, 4, 4,           // 0x20 - 0x2F
    4, 10, 13, 5, 5, 7, 4, 4, 4, 10, 13, 5, 5, 7,7, 4,            // 0x30 - 0x3F
    5, 5, 5, 5, 5, 5, 7, 5, 5, 5, 5, 5, 5, 5, 7, 5,               // 0x40 - 0x4F
    5, 5, 5, 5, 5, 5, 7, 5, 5, 5, 5, 5, 5, 5, 7, 5,               // 0x50 - 0x5F
    5, 5, 5, 5, 5, 5, 7, 5, 5, 5, 5, 5, 5, 5, 7, 5,               // 0x60 - 0x6F
    7, 7, 7, 7, 7, 7, 7, 5, 5, 5, 5, 5, 5, 5, 7, 5,               // 0x70 - 0x7F
    4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,               // 0x80 - 0x8F
    4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,               // 0x90 - 0x9F
    4, 4, 4, 4, 4, 4, 7, 4, 4, 4, 4, 4, 4, 4, 7, 4,               // 0xA0 - 0xAF
    4, 4, 4, 4, 4, 4, 7, 4, 4, 4, 4, 4, 4, 4, 4, 4,               // 0xB0 - 0xBF
    11, 10, 10, 10, 17, 11, 11, 11, 11, 10, 17, 10, 17, 17, 11, 11, // 0xC0 - 0xCF
    11, 10, 10, 10, 17, 11, 11, 11, 11, 10, 10, 10, 17, 11, 11, 11, // 0xD0 - 0xDF
    11, 10, 10, 10, 17, 11, 7, 11, 11, 10, 10, 10, 17, 11, 7, 11,   // 0xE0 - 0xEF
    11, 10, 10, 4, 17, 11, 7, 11, 11, 5, 10, 4, 17, 11, 7, 11       // 0xF0 - 0xFF
};

void cpu_init(CPU8080 *cpu) {
    memset(cpu, 0, sizeof(CPU8080));
    cpu->SP = 0xFFFF;
    cpu->PC = 0x0000;
    cpu->halted = false;
    cpu->interrupt_enabled = false;
    cpu->pending_interrupt = false;
    cpu->interrupt_opcode = 0x00;
    cpu->is_interrupt = false;

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

bool cpu_emulate(CPU8080 *cpu) {
    if (cpu->halted) return false;

    if (cpu->pending_interrupt && cpu->interrupt_enabled) {
        cpu->interrupt_enabled = false;
        cpu->pending_interrupt = false;
        uint8_t opcode = cpu->interrupt_opcode;
        printf("Interrupted here, opcode: 0x%02X\n", opcode);
        execute_instruction(cpu, opcode);
        cpu->is_interrupt = false;
        return true;
    }

    uint8_t opcode = cpu->memory[cpu->PC];
    printf("PC: 0x%04X, Opcode: 0x%02X, Cycles: %llu\n", cpu->PC, opcode, cpu->cycles);
    execute_instruction(cpu, opcode);
    return true;
}

void execute_instruction(CPU8080 *cpu, uint8_t opcode) {
    custom_cycle = false;
    if (instruction_table[opcode]) {
        instruction_table[opcode](opcode, cpu);
        if (!custom_cycle) {
            cpu->cycles += t_states[opcode];
        }
    } else {
        printf("Unknown opcode: 0x%02X\n", opcode);
    }
}

void request_interrupt(CPU8080 *cpu, uint8_t rst_opcode) {
    if (cpu->interrupt_enabled) {
        cpu->pending_interrupt = true;
        cpu->interrupt_opcode = rst_opcode;
        cpu->is_interrupt = true;
    }
}

void trigger_trap(CPU8080 *cpu) {
    uint16_t return_addr = cpu->PC;

    cpu->memory[cpu->SP - 1] = (return_addr >> 8) & 0xFF;
    cpu->memory[cpu->SP - 2] = return_addr & 0xFF;
    cpu->SP -= 2;
    cpu->PC = 0x24;
}