#include "instructions.h"
#include "../decoder/decoder.h"
#include "stdint.h"
#include "../utils/helpers.h"
#include "stddef.h"

void nop(uint8_t opcode, CPU8080 *cpu) {
    (void)opcode;
    cpu->PC += 1;
}

void adi(uint8_t opcode, CPU8080 *cpu) {
    (void)opcode;
    uint8_t value = cpu->memory[cpu->PC + 1];
    uint16_t result = cpu->A + value;

    update_flags_after_add(result, value, cpu, false);

    cpu->A = result & 0xFF;
    cpu->PC += 2;
}

void aci(uint8_t opcode, CPU8080 *cpu) {
    (void)opcode;
    uint8_t value = cpu->memory[cpu->PC + 1];
    uint16_t result = cpu->A + value + cpu->flags.CY;

    update_flags_after_add(result, value, cpu, true);

    cpu->A = result & 0xFF;
    cpu->PC += 2;
}

void jmp(uint8_t opcode, CPU8080 *cpu) {
    (void)opcode;
    uint16_t addr = cpu->memory[cpu->PC + 1] | (cpu->memory[cpu->PC + 2] << 8);
    cpu->PC = addr;
}

void hlt(uint8_t opcode, CPU8080 *cpu) {
    (void)opcode;
    cpu->halted = true;
}

void mov_generic(uint8_t opcode, CPU8080 *cpu) {
    uint8_t dest_code = (opcode >> 3) & 0x07;
    uint8_t src_code  = opcode & 0x07;
    uint16_t addr = (cpu->H << 8) | cpu->L;
    
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
        cpu->memory[(cpu->H << 8) | cpu->L] = data;
    } else {
        *cpu->registers[reg_code] = data;
    }

    cpu->PC += 2;
}

void add_generic(uint8_t opcode, CPU8080 *cpu) {
    uint8_t value;
    uint8_t reg_code = (opcode >> 3) & 0x07;
    
    if (reg_code == 6) {
        value = cpu->memory[(cpu->H << 8) | cpu->L];
    } else {
        value = *cpu->registers[reg_code];
    }

    uint16_t result = cpu->A + value;
    cpu->A = result & 0xFF;
    update_flags_after_add(result, value, cpu, false);

    cpu->PC += 1;
}

void inr_generic(uint8_t opcode, CPU8080 *cpu) {
    uint8_t result, before;
    uint8_t reg_code = (opcode >> 3) & 0x07;

    if (reg_code == 6) {
        uint16_t addr = (cpu->H << 8) | cpu->L;
        before = cpu->memory[addr];
        result = before + 1;
        cpu->memory[addr] = result;
    } else {
        before = *cpu->registers[reg_code];
        result = before + 1;
        *cpu->registers[reg_code] = result;
    }
    update_flags_after_inr_dcr(result, before, cpu, true);
    cpu->PC += 1;
}

void dcr_generic(uint8_t opcode, CPU8080 *cpu) {
    uint8_t result, before;
    uint8_t reg_code = (opcode >> 3) & 0x07;

    if (reg_code == 6) {
        uint16_t addr = (cpu->H << 8) | cpu->L;
        before = cpu->memory[addr];
        result = before - 1;
        cpu->memory[addr] = result;
    } else {
        before = *cpu->registers[reg_code];
        result = before - 1;
        *cpu->registers[reg_code] = result;
    }
    update_flags_after_inr_dcr(result, before, cpu, false);
    cpu->PC += 1;
}

void lxi_generic(uint8_t opcode, CPU8080 *cpu) {
    uint8_t low = cpu->memory[cpu->PC + 1];
    uint8_t high = cpu->memory[cpu->PC + 2];
    uint16_t value = (high << 8) | low;

    switch (opcode) {
        case 0x01:
            cpu->C = low;
            cpu->B = high;
            break;
        case 0x11:
            cpu->C = low;
            cpu->B = high;
            break;
        case 0x21:
            cpu->C = low;
            cpu->B = high;
            break;
        case 0x31:
            cpu->SP = value;
            break;
    }
    cpu->PC += 3;
}

void sub_generic(uint8_t opcode, CPU8080 *cpu) {
    uint8_t reg_code = opcode & 0x07;
    uint8_t value;

    if (reg_code == 6) {
        value = cpu->memory[(cpu->H << 8) | cpu->L];
        custom_cycle = true;
        cpu->cycles += 7;
    } else {
        value = *cpu->registers[reg_code];
    }

    uint8_t result = cpu->A - value;
    update_flags_after_sub(cpu->A, value, cpu);
    cpu->A = result;
    cpu->PC += 1;
}