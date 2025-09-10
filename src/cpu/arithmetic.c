#include "instructions.h"
#include "stdint.h"
#include "../decoder/decoder.h"
#include "../utils/helpers.h"
#include <stdio.h>

void add_generic(uint8_t opcode, CPU8080 *cpu) {
    uint8_t value;
    uint8_t reg_code = opcode & 0x07;

    if (reg_code == 6) {
        uint16_t addr = get_rpair(cpu->H, cpu->L);
        value = cpu->memory[addr];
    } else {
        value = *cpu->registers[reg_code];
    }

    bool include_cy = opcode & 0x08;
    uint16_t result = cpu->A + value + (include_cy ? cpu->flags.CY : 0);
    cpu->A = result & 0xFF;
    update_flags_after_add(result, value, cpu, include_cy);

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

void sub_generic(uint8_t opcode, CPU8080 *cpu) {
    uint8_t reg_code = opcode & 0x07;
    uint8_t value;

    if (reg_code == 6) {
        uint16_t addr = get_rpair(cpu->H, cpu->L);
        value = cpu->memory[addr];
        custom_cycle = true;
        cpu->cycles += 7;
    } else {
        value = *cpu->registers[reg_code];
    }

    uint8_t borrow = (opcode & 0x08) ? cpu->flags.CY : 0;
    uint16_t result = cpu->A - value - borrow;
    update_flags_after_sub(cpu->A, value, borrow, cpu);
    cpu->A = result & 0xFF;
    cpu->PC += 1;
}

void sui(uint8_t opcode, CPU8080 *cpu) {
    (void)opcode;
    uint8_t value = cpu->memory[cpu->PC + 1];

    uint8_t result = cpu->A - value;
    update_flags_after_sub(cpu->A, value, 0, cpu);
    
    cpu->A = result;
    cpu->PC += 2;
}

void sbi(uint8_t opcode, CPU8080 *cpu) {
    (void)opcode;
    uint8_t value = cpu->memory[cpu->PC + 1];

    uint8_t result = cpu->A - value - cpu->flags.CY;
    update_flags_after_sub(cpu->A, value, cpu->flags.CY, cpu);

    cpu->A = result;
    cpu->PC += 2;
}

void inr_generic(uint8_t opcode, CPU8080 *cpu) {
    uint8_t result, before;
    uint8_t reg_code = (opcode >> 3) & 0x07;

    if (reg_code == 6) {
        uint16_t addr = get_rpair(cpu->H, cpu->L);
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
        uint16_t addr = get_rpair(cpu->H, cpu->L);
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

void daa(uint8_t opcode, CPU8080 *cpu) {
    (void)opcode;
    uint8_t correction = 0;

    if ((cpu->A & 0x0F) > 9 || cpu->flags.AC) {
        correction = 0x06;
    } 
    if ((cpu->A >> 4) > 9 || cpu->flags.CY || (cpu->A + correction > 0x99)) {
        correction = 0x60;
    }
    uint16_t result = cpu->A + correction;
    update_flags_after_add(result, correction, cpu, false);

    cpu->A = result & 0xFF;
    cpu->PC += 1;
}

void dad_generic(uint8_t opcode, CPU8080 *cpu) {
    uint16_t HL = get_rpair(cpu->H, cpu->L);
    uint16_t value;

    switch (opcode) {
        case 0x09: value = get_rpair(cpu->B, cpu->C); break;
        case 0x19: value = get_rpair(cpu->D, cpu->E);  break;
        case 0x29: value = HL; break;
        case 0x39: value = cpu->SP; break;
    }

    uint32_t result = HL + value;
    cpu->H = (result >> 8) & 0xFF;
    cpu->L = result & 0xFF;
    cpu->flags.CY = (result > 0xFFFF);
    cpu->PC += 1;
}

void inx_generic(uint8_t opcode, CPU8080 *cpu) {
    uint16_t value;

    switch (opcode) {
        case 0x03:
            value = get_rpair(cpu->B, cpu->C);
            value++;
            set_rpair(value, &cpu->B, &cpu->C);
            break;
        case 0x13:
            value = get_rpair(cpu->D, cpu->E);
            value++;
            set_rpair(value, &cpu->D, &cpu->E);
            break;
        case 0x23:
            value = get_rpair(cpu->H, cpu->L);
            value++;
            set_rpair(value, &cpu->H, &cpu->L);
            break;
        case 0x33:
            cpu->SP++;
            break;
    }
    cpu->PC += 1;
}

void dcx_generic(uint8_t opcode, CPU8080 *cpu) {
    uint16_t value;

    switch (opcode) {
        case 0x0B:
            value = get_rpair(cpu->B, cpu->C);
            value--;
            set_rpair(value, &cpu->B, &cpu->C);
            break;
        case 0x1B:
            value = get_rpair(cpu->D, cpu->E);
            value--;
            set_rpair(value, &cpu->D, &cpu->E);
            break;
        case 0x2B:
            value = get_rpair(cpu->H, cpu->L);
            value--;
            set_rpair(value, &cpu->H, &cpu->L);
            break;
        case 0x3B:
            cpu->SP--;
            break;
    }
    cpu->PC += 1;
}