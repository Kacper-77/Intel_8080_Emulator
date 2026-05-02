#include "instructions.h"
#include "stdint.h"
#include "../utils/helpers.h"

void ana_generic(uint8_t opcode, CPU8080 *cpu) {
    uint8_t reg_code = opcode & 0x07;
    uint8_t val = (reg_code == 6)
                    ? cpu->memory[get_rpair(cpu->H, cpu->L)]
                    : *cpu->registers[reg_code];
    uint8_t oldA = cpu->A;
    uint8_t result = oldA & val;
    cpu->A = result;

    // Set flags
    update_flags_logical(oldA, val, result, cpu, '&');
    
    cpu->PC += 1;
}

void ani(uint8_t opcode, CPU8080 *cpu) {
    (void)opcode;
    uint8_t oldA = cpu->A;
    uint8_t value = cpu->memory[cpu->PC + 1];
    uint8_t result = cpu->A & value;
    cpu->A = result;

    // Set flags
    update_flags_logical(oldA, value, result, cpu, '&');

    cpu->PC += 2;
}

void ora_generic(uint8_t opcode, CPU8080 *cpu) {
    uint8_t reg_code = opcode & 0x07;
    uint8_t value = (reg_code == 6)
                    ? cpu->memory[get_rpair(cpu->H, cpu->L)]
                    : *cpu->registers[reg_code];
    uint8_t oldA = cpu->A;
    uint8_t result = oldA | value;
    cpu->A = result;
    update_flags_logical(oldA, value, result, cpu, '|');
    cpu->PC += 1;
}

void ori(uint8_t opcode, CPU8080 *cpu) {
    (void)opcode;
    uint8_t value = cpu->memory[cpu->PC + 1];
    uint8_t result = cpu->A | value;
    cpu->A = result;
    set_ZSP(result, cpu);
    cpu->flags.CY = 0;
    cpu->flags.AC = 0;
    cpu->PC += 2;
}

void xra_generic(uint8_t opcode, CPU8080 *cpu) {
    uint8_t reg_code = opcode & 0x07;
    uint8_t value = (reg_code == 6)
                    ? cpu->memory[get_rpair(cpu->H, cpu->L)]
                    : *cpu->registers[reg_code];
    uint8_t oldA = cpu->A;
    uint8_t result = oldA ^ value;
    cpu->A = result;
    update_flags_logical(oldA, value, result, cpu, '^');
    cpu->PC += 1;
}

void xri(uint8_t opcode, CPU8080 *cpu) {
    (void)opcode;
    uint8_t value = cpu->memory[cpu->PC + 1];
    uint8_t result = cpu->A ^ value;
    cpu->A = result;
    set_ZSP(result, cpu);
    cpu->flags.CY = 0;
    cpu->flags.AC = 0;
    cpu->PC += 2;
}

void cmp_generic(uint8_t opcode, CPU8080 *cpu) {
    uint8_t reg_code = opcode & 0x07;
    uint8_t value = (reg_code == 6)
                    ? cpu->memory[get_rpair(cpu->H, cpu->L)]
                    : *cpu->registers[reg_code];

    int16_t diff = (int16_t)cpu->A - (int16_t)value;
    uint8_t res8 = (uint8_t)diff;

    // Set flags
    cpu->flags.CY = (diff >> 8) & 1;
    cpu->flags.AC = ((~(cpu->A ^ res8 ^ value)) & 0x10) != 0;
    set_ZSP(res8, cpu);

    cpu->PC += 1;
}

void cpi(uint8_t opcode, CPU8080 *cpu) {
    (void)opcode;
    uint8_t value = cpu->memory[cpu->PC + 1];
    int16_t diff = (int16_t)cpu->A - (int16_t)value;
    uint8_t res8 = (uint8_t)diff;

    // Set flags
    cpu->flags.CY = (diff >> 8) & 1;
    cpu->flags.AC = ((~(cpu->A ^ res8 ^ value)) & 0x10) != 0;
    set_ZSP(res8, cpu);

    cpu->PC += 2;
}


void rrc(uint8_t opcode, CPU8080 *cpu) {
    (void)opcode;
    uint8_t lsb = cpu->A & 1;

    cpu->A = (cpu->A >> 1) | (lsb << 7);
    cpu->flags.CY = lsb;

    cpu->PC += 1;
}

void rlc(uint8_t opcode, CPU8080 *cpu) {
    (void)opcode;
    uint8_t msb = (cpu->A & 0x80) >> 7;

    cpu->A = (cpu->A << 1) | msb;
    cpu->flags.CY = msb;

    cpu->PC += 1;
}

void rar(uint8_t opcode, CPU8080 *cpu) {
    (void)opcode;
    uint8_t old_cy = cpu->flags.CY;
    uint8_t lsb = cpu->A & 1;

    cpu->A = (cpu->A >> 1) | (old_cy << 7);
    cpu->flags.CY = lsb;

    cpu->PC += 1;
}

void ral(uint8_t opcode, CPU8080 *cpu) {
    (void)opcode;
    uint8_t old_cy = cpu->flags.CY;
    uint8_t msb = (cpu->A & 0x80) >> 7;

    cpu->A = (cpu->A << 1) | old_cy;
    cpu->flags.CY = msb;

    cpu->PC += 1;
}

void cma(uint8_t opcode, CPU8080 *cpu) {
    (void)opcode;
    cpu->A = ~cpu->A;
    cpu->PC += 1;
}

void cmc(uint8_t opcode, CPU8080 *cpu) {
    (void)opcode;
    cpu->flags.CY = !cpu->flags.CY;
    cpu->PC += 1;
}

void stc(uint8_t opcode, CPU8080 *cpu) {
    (void)opcode;
    cpu->flags.CY = 1;
    cpu->PC += 1;
}
