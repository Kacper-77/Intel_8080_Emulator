#include "instructions.h"
#include "stdint.h"
#include "../decoder/decoder.h"
#include "../utils/helpers.h"
#include <stdio.h>

/*
    add: A += reg
    adc: A += reg + carry
*/
void add_adc_generic(uint8_t opcode, CPU8080 *cpu) {
    uint8_t reg_code = opcode & 0x07;
    uint8_t value = (reg_code == 6)
                    ? cpu->memory[get_rpair(cpu->H, cpu->L)]
                    : *cpu->registers[reg_code];
    uint8_t carry_in = (opcode & 0x08) ? cpu->flags.CY : 0;

    alu_do_add(cpu->A, value, carry_in, cpu);
    cpu->PC += 1;
}

// adi: A += value of memory[PC + 1]
void adi(uint8_t opcode, CPU8080 *cpu) {
    (void)opcode;
    uint8_t value = cpu->memory[cpu->PC + 1];
    alu_do_add(cpu->A, value, 0, cpu);
    cpu->PC += 2;
}

// aci: A += value of memory[PC + 1] + carry
void aci(uint8_t opcode, CPU8080 *cpu) {
    (void)opcode;
    uint8_t value = cpu->memory[cpu->PC + 1];
    alu_do_add(cpu->A, value, cpu->flags.CY, cpu);
    cpu->PC += 2;
}

/*
    sub: A -= reg
    sbb: a -= reg - borrow
*/
void sub_sbb_generic(uint8_t opcode, CPU8080 *cpu) {
    uint8_t reg_code = opcode & 0x07;
    uint8_t val = (reg_code == 6)
                    ? cpu->memory[get_rpair(cpu->H, cpu->L)]
                    : *cpu->registers[reg_code];
    uint8_t borrow_in = (opcode & 0x08) ? cpu->flags.CY : 0;

    alu_do_sub(cpu->A, val, borrow_in, cpu);
    cpu->PC += 1;
}

// sui: A -= value of memory[PC + 1]
void sui(uint8_t opcode, CPU8080 *cpu) {
    (void)opcode;
    uint8_t value = cpu->memory[cpu->PC + 1];
    alu_do_sub(cpu->A, value, 0, cpu);
    cpu->PC += 2;
}

// sbi: A -= value of memory[PC + 1] - borrow
void sbi(uint8_t opcode, CPU8080 *cpu) {
    (void)opcode;
    uint8_t value = cpu->memory[cpu->PC + 1];
    alu_do_sub(cpu->A, value, cpu->flags.CY, cpu);
    cpu->PC += 2;
}

/* 
    Increase any reg by 1
    example: B = 0b10 + 1 = 0b11
*/
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

// Decrease any reg by 1
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

/*
    Adjusts the accumulator's binary content
    into Binary-Coded Decimal (BCD) format after an addition.
*/
void daa(uint8_t opcode, CPU8080 *cpu) {
    (void)opcode;
    uint8_t oldA = cpu->A;
    bool cy = cpu->flags.CY;
    uint8_t correction = 0;
    uint8_t lsb = oldA & 0x0F;
    uint8_t msb = oldA >> 4;

    if (cpu->flags.AC || lsb > 9) correction |= 0x06;
    if (cpu->flags.CY || msb > 9 || (msb >= 9 && lsb > 9)) {
        correction |= 0x60;
        cy = true;
    }

    uint16_t sum = (uint16_t)oldA + (uint16_t)correction;
    uint8_t result = (uint8_t)sum;

    cpu->A = result;
    set_ZSP(result, cpu);
    cpu->flags.AC = ((oldA ^ correction ^ result) & 0x10) != 0;
    cpu->flags.CY = cy;

    cpu->PC += 1;
}

/*
    performs 16-bit addition, adding a specified 16-bit register pair (like BC, DE, HL)
    or the Stack Pointer (SP) to the HL register pair (High-Low),
    often used as an accumulator and storing the 16-bit result back into HL,
    setting the Carry flag for multi-byte arithmetic
*/
void dad_generic(uint8_t opcode, CPU8080 *cpu) {
    uint16_t HL = get_rpair(cpu->H, cpu->L);
    uint16_t value = 0;

    switch (opcode) {
        case 0x09: value = get_rpair(cpu->B, cpu->C); break;  // DAD B
        case 0x19: value = get_rpair(cpu->D, cpu->E); break;  // DAD D
        case 0x29: value = HL; break;                         // DAD H
        case 0x39: value = cpu->SP; break;                    // DAD SP
        default: return;
    }

    uint32_t sum = (uint32_t)HL + (uint32_t)value;
    set_rpair((uint16_t)(sum & 0xFFFF), &cpu->H, &cpu->L);
    cpu->flags.CY = (sum & 0x10000) != 0;

    cpu->PC += 1;
}

/*
    Adds 1 to a specified 16-bit register pair (BC, DE, or HL),
    effectively performing a 16-bit addition,
    without affecting any of the 8080's status flags.
*/
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

/*
    Decrements a specified 16-bit register pair by one,
    treating them as a single 16-bit value,
    without affecting any of the 8080's status flags.
*/
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
            check_stack_bounds(cpu);
            break;
    }
    cpu->PC += 1;
}
