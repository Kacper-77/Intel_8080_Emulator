#include "instructions.h"
#include "../utils/helpers.h"
#include "stdint.h"
#include <stdio.h>

void mov_generic(uint8_t opcode, CPU8080 *cpu) {
    uint8_t dest_code = (opcode >> 3) & 0x07;
    uint8_t src_code  = opcode & 0x07;
    uint16_t addr = get_rpair(cpu->H, cpu->L);
    
    if (dest_code == 6 && src_code != 6) {
        cpu->memory[addr] = *cpu->registers[src_code];
    } else if (dest_code != 6 && src_code == 6) {
        *cpu->registers[dest_code] = cpu->memory[addr];
    } else {
        *cpu->registers[dest_code] = *cpu->registers[src_code];
    }
    cpu->PC += 1;
}

void mvi_generic(uint8_t opcode, CPU8080 *cpu) {
    uint8_t reg_code = (opcode >> 3) & 0x07;
    uint8_t data = cpu->memory[cpu->PC + 1];

    if (reg_code == 6) {
        uint16_t addr = get_rpair(cpu->H, cpu->L);
        cpu->memory[addr] = data;
    } else {
        *cpu->registers[reg_code] = data;
    }
    cpu->PC += 2;
}

void lxi_generic(uint8_t opcode, CPU8080 *cpu) {
    uint16_t value = fetch_addr(cpu);

    switch (opcode) {
        case 0x01:
            cpu->B = value >> 8;
            cpu->C = value & 0xFF;
            break;
        case 0x11:
            cpu->D = value >> 8;
            cpu->E = value & 0xFF;
            break;
        case 0x21:
            cpu->H = value >> 8;
            cpu->L = value & 0xFF;
            break;
        case 0x31:
            cpu->SP = value;
            break;
    }
    cpu->PC += 3;
}

void lda(uint8_t opcode, CPU8080 *cpu) {
    (void)opcode;
    uint16_t addr = fetch_addr(cpu);
    cpu->A = cpu->memory[addr];

    cpu->PC += 3;
}

void sta(uint8_t opcode, CPU8080 *cpu) {
    (void)opcode;
    uint16_t addr = fetch_addr(cpu);
    cpu->memory[addr] = cpu->A;
    
    cpu->PC += 3;
}

void shld(uint8_t opcode, CPU8080 *cpu) {
    (void)opcode;
    uint16_t addr = fetch_addr(cpu);
    cpu->memory[addr] = cpu->L;
    cpu->memory[addr + 1] = cpu->H;

    cpu->PC += 3;
}

void lhld(uint8_t opcode, CPU8080 *cpu) {
    (void)opcode;
    uint16_t addr = fetch_addr(cpu);
    cpu->L = cpu->memory[addr];
    cpu->H = cpu->memory[addr + 1];

    cpu->PC += 3;
}

void ldax_generic(uint8_t opcode, CPU8080 *cpu) {
    uint16_t addr;
    switch (opcode)
    {
    case 0x0A:
        addr = get_rpair(cpu->B, cpu->C);
        cpu->A = cpu->memory[addr];
        break;
    case 0x1A:
        addr = get_rpair(cpu->D, cpu->E);
        cpu->A = cpu->memory[addr];
        break;
    }
    cpu->PC += 1;
}

void stax_generic(uint8_t opcode, CPU8080 *cpu) {
    switch (opcode) {
        case 0x02:
            cpu->memory[get_rpair(cpu->B, cpu->C)] = cpu->A;
            break;
        case 0x12:
            cpu->memory[get_rpair(cpu->D, cpu->E)] = cpu->A;
            break;
    }
    cpu->PC += 1;
}

void xchg(uint8_t opcode, CPU8080 *cpu) {
    (void)opcode;
    uint8_t tempH = cpu->H;
    uint8_t tempL = cpu->L;

    cpu->H = cpu->D;
    cpu->L = cpu->E;
    cpu->D = tempH;
    cpu->E = tempL;

    cpu->PC += 1;
}