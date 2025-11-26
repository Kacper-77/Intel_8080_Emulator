#include "cpu.h"
#include "../instructions/instructions.h"
#include "../decoder/decoder.h"
#include "../utils/helpers.h"
#include <stdio.h>
#include <string.h>

#define STACK_MIN 0x0000

static const uint8_t t_states[256] = {
    4,  10, 7,  5,  5,  5,  7,  4,  4,  10, 7,  5,  5,  7,  4,  4,  // 0x00 - 0x0F
    4,  10, 7,  5,  5,  7,  4,  4,  4,  10, 7,  5,  5,  7,  4,  4,  // 0x10 - 0x1F
    4,  10, 16, 5,  5,  7,  4,  4,  4,  10, 16, 5,  5,  7,  4,  4,  // 0x20 - 0x2F
    4,  10, 13, 5,  5,  7,  4,  4,  4,  10, 13, 5,  5,  7,  7,  4,  // 0x30 - 0x3F
    5,  5,  5,  5,  5,  5,  7,  5,  5,  5,  5,  5,  5,  5,  7,  5,  // 0x40 - 0x4F
    5,  5,  5,  5,  5,  5,  7,  5,  5,  5,  5,  5,  5,  5,  7,  5,  // 0x50 - 0x5F
    5,  5,  5,  5,  5,  5,  7,  5,  5,  5,  5,  5,  5,  5,  7,  5,  // 0x60 - 0x6F
    7,  7,  7,  7,  7,  7,  7,  5,  5,  5,  5,  5,  5,  5,  7,  5,  // 0x70 - 0x7F
    4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  // 0x80 - 0x8F
    4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  // 0x90 - 0x9F
    4,  4,  4,  4,  4,  4,  7,  4,  4,  4,  4,  4,  4,  4,  7,  4,  // 0xA0 - 0xAF
    4,  4,  4,  4,  4,  4,  7,  4,  4,  4,  4,  4,  4,  4,  4,  4,  // 0xB0 - 0xBF
    11, 10, 10, 10, 17, 11, 11, 11, 11, 10, 10, 10, 17, 17, 11, 11, // 0xC0 - 0xCF
    11, 10, 10, 10, 17, 11, 11, 11, 11, 10, 10, 10, 17, 11, 11, 11, // 0xD0 - 0xDF
    11, 10, 10, 18, 17, 11, 7,  11, 11, 10, 10, 4,  17, 11, 7,  11, // 0xE0 - 0xEF
    11, 10, 10, 4,  17, 11, 7,  11, 11, 5,  10, 4,  17, 11, 7,  11  // 0xF0 - 0xFF
};

void cpu_init(CPU8080 *cpu) {
    memset(cpu, 0, sizeof(CPU8080));
    cpu->PC = 0x0000;
    cpu->SP = 0xFFFE;
    cpu->stack_base = cpu->SP;
    cpu->heap_base = 0x2000;
    cpu->heap_ptr = cpu->heap_base;

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
    if (cpu->pending_interrupt && cpu->interrupt_enabled) {
        cpu->interrupt_enabled = false;
        cpu->pending_interrupt = false;
        uint8_t opcode = cpu->interrupt_opcode;
        execute_instruction(cpu, opcode);
        cpu->is_interrupt = false;
        return true;
    }

    // CPU halted, waiting for interrupt. HLT does not block interrupts.
    if (cpu->halted) return false;

    uint8_t opcode = cpu->memory[cpu->PC];
    // printf("PC: 0x%04X, Opcode: 0x%02X, Cycles: %llu\n", cpu->PC, opcode, cpu->cycles);
    execute_instruction(cpu, opcode);
    return true;
}

void execute_instruction(CPU8080 *cpu, uint8_t opcode) {
    // CP/M exit trap
    if (cpu->PC == 0x0000) {
         printf("\n[CP/M] Program terminated cleanly at PC=%04X (SP=%04X)\n", cpu->PC, cpu->SP);
        cpu->halted = true;
        return;
    }

    // CP/M BDOS trap
    if (cpu->PC == 0x0005) {
        switch(cpu->C) {
            case 2: putchar(cpu->E); fflush(stdout); break;
            case 9: {
                uint16_t addr = (cpu->D << 8) | cpu->E;
                while(cpu->memory[addr] != '$') putchar(cpu->memory[addr++]);
                fflush(stdout);
                break;
            }
        }

        // RET
        cpu->PC = fetch_return_addr(cpu);
        return;
    }

    custom_cycle = false;
    if (instruction_table[opcode]) {
        instruction_table[opcode](opcode, cpu);
        if (!custom_cycle) {
            cpu->cycles += t_states[opcode];
        }
    } else {
        printf("Unknown opcode: 0x%02X\n", opcode);
        cpu->halted = true;
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
    set_return_addr(return_addr, cpu);
    cpu->PC = 0x24; 
}

bool check_heap_bounds(CPU8080 *cpu, uint16_t size) {
    if (cpu->heap_ptr + size >= cpu->stack_base) {
        printf("❌ Heap overflow! heap_ptr=0x%04X + size=0x%04X >= stack_base=0x%04X\n",
               cpu->heap_ptr, size, cpu->stack_base);
        cpu->halted = true;
        return false;
    }
    return true;
}

void* heap_alloc(CPU8080 *cpu, uint16_t size) {
    if (!check_heap_bounds(cpu, size)) return NULL;

    void* ptr = &cpu->memory[cpu->heap_ptr];
    cpu->heap_ptr += size;
    return ptr;
}

void check_stack_bounds(CPU8080 *cpu) {
    if (cpu->SP < STACK_MIN) {
        printf("❌ Stack underflow! SP=%04X\n", cpu->SP);
        cpu->halted = true;
    }
    if (cpu->SP > cpu->stack_base) {
        printf("❌ Stack overflow! SP=%04X (stack_base=%04X)\n", cpu->SP, cpu->stack_base);
        cpu->halted = true;
    }
}
