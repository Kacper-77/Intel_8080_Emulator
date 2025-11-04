#include "instructions.h"
#include "stdint.h"
#include "../decoder/decoder.h"
#include "../utils/helpers.h"
#include <stdio.h>

void add_adc_generic(uint8_t opcode, CPU8080 *cpu) {
    uint8_t reg_code = opcode & 0x07;
    uint8_t value;

    if (reg_code == 6) {
        uint16_t addr = get_rpair(cpu->H, cpu->L);
        value = cpu->memory[addr];
    } else {
        value = *cpu->registers[reg_code];
    }

    bool include_cy = (opcode & 0x08) != 0;
    uint8_t oldA = cpu->A;
    uint16_t result = (uint16_t)oldA + (uint16_t)value + (include_cy ? cpu->flags.CY : 0);

    update_flags_after_add(result, value, oldA, cpu, include_cy);

    cpu->A = result & 0xFF;
    cpu->PC += 1;
    // printf("ADD/ADC: PC=%04X opcode=%02X A=%02X operand=%02X result=%02X CY=%d AC=%d Z=%d S=%d P=%d\n",
    //     cpu->PC-1, opcode, oldA, value, cpu->A,
    //     cpu->flags.CY, cpu->flags.AC, cpu->flags.Z, cpu->flags.S, cpu->flags.P);
}


void adi(uint8_t opcode, CPU8080 *cpu) {
    uint8_t value = cpu->memory[cpu->PC + 1];
    uint8_t oldA = cpu->A;
    uint16_t result = (uint16_t)oldA + (uint16_t)value;

    update_flags_after_add(result, value, oldA, cpu, false);

    cpu->A = result & 0xFF;
    cpu->PC += 2;

    printf("ADI: PC=%04X opcode=%02X A=%02X operand=%02X result=%02X CY=%d AC=%d Z=%d S=%d P=%d\n",
           cpu->PC-2, opcode, oldA, value, cpu->A,
           cpu->flags.CY, cpu->flags.AC, cpu->flags.Z, cpu->flags.S, cpu->flags.P);
}


void aci(uint8_t opcode, CPU8080 *cpu) {
    uint8_t value = cpu->memory[cpu->PC + 1];
    uint8_t oldA = cpu->A;
    uint16_t result = (uint16_t)oldA + (uint16_t)value + cpu->flags.CY;
    update_flags_after_add(result, value, oldA, cpu, true);
    cpu->A = result & 0xFF;
    cpu->PC += 2;
}


void sub_sbb_generic(uint8_t opcode, CPU8080 *cpu) {
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
    uint8_t oldA = cpu->A;
    uint8_t correction = 0;
    bool set_carry = false;

    if ((oldA & 0x0F) > 9 || cpu->flags.AC)
        correction |= 0x06;

    if (oldA > 0x99 || cpu->flags.CY || ((oldA + correction) > 0x99)) {
        correction |= 0x60;
        set_carry = true;
    }

    uint16_t result = (uint16_t)oldA + (uint16_t)correction;
    cpu->A = result & 0xFF;


    cpu->flags.Z = (cpu->A == 0);
    cpu->flags.S = (cpu->A & 0x80) != 0;
    uint8_t v = cpu->A;
    v ^= v >> 4; v ^= v >> 2; v ^= v >> 1; cpu->flags.P = (~v) & 1;

    cpu->flags.CY = set_carry;
    cpu->flags.AC = ((oldA ^ correction ^ result) & 0x10) != 0;

    cpu->PC += 1;
}


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
    cpu->flags.CY = (sum & 0x10000) != 0;   // <-- poprawne ustawianie carry

    cpu->PC += 1;

    // printf("DAD: PC=%04X opcode=%02X HL=%04X value=%04X => result=%06X CY=%d\n",
    //        cpu->PC, opcode, HL, value, sum, cpu->flags.CY);
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
            check_stack_bounds(cpu);
            break;
    }
    cpu->PC += 1;
}