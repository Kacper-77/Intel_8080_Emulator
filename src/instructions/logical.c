#include "instructions.h"
#include "stdint.h"
#include "../utils/helpers.h"

void ana_generic(uint8_t opcode, CPU8080 *cpu) {
    uint8_t reg_code = opcode & 0x07;
    uint8_t value;
    
    if (reg_code == 6) {
        uint16_t addr = get_rpair(cpu->H, cpu->L);
        value = cpu->memory[addr];
    } else {
        value = *cpu->registers[reg_code];
    }

    uint16_t result = cpu->A & value;
    cpu->A = result & 0xFF;
    update_flags_logical(result, cpu, false);
    cpu->PC += 1;
}

void ani(uint8_t opcode, CPU8080 *cpu) {
    (void)opcode;
    uint8_t value = cpu->memory[cpu->PC + 1];

    uint16_t result = cpu->A & value;
    cpu->A = result & 0xFF;
    update_flags_logical(result, cpu, false);
    cpu->PC += 2;
}

void ora_generic(uint8_t opcode, CPU8080 *cpu) {
    uint8_t reg_code = opcode & 0x07;
    uint8_t value;

    if (reg_code == 6) {
        uint16_t addr = get_rpair(cpu->H, cpu->L);
        value = cpu->memory[addr];
    } else {
        value = *cpu->registers[reg_code];
    }

    uint16_t result = cpu->A | value;
    cpu->A = result & 0xFF;
    update_flags_logical(result, cpu, true);
    cpu->PC += 1;
}

void ori(uint8_t opcode, CPU8080 *cpu) {
    (void)opcode;
    uint8_t value = cpu->memory[cpu->PC + 1];

    uint16_t result = cpu->A | value;
    cpu->A = result & 0xFF;
    update_flags_logical(result, cpu, true);
    cpu->PC += 2;
}

void xra_generic(uint8_t opcode, CPU8080 *cpu) {
    uint8_t reg_code = opcode & 0x07;
    uint8_t value;

    if (reg_code == 6) {
        uint16_t addr = get_rpair(cpu->H, cpu->L);
        value = cpu->memory[addr];
    } else {
        value = *cpu->registers[reg_code];
    }

    uint16_t result = cpu->A ^ value;
    cpu->A = result & 0xFF;
    update_flags_logical(result, cpu, true);
    cpu->PC += 1;
}

void xri(uint8_t opcode, CPU8080 *cpu) {
    (void)opcode;
    uint8_t value = cpu->memory[cpu->PC + 1];

    uint16_t result = cpu->A ^ value;
    cpu->A = result & 0xFF;
    update_flags_logical(result, cpu, true);
    cpu->PC += 2;
}

void cmp_generic(uint8_t opcode, CPU8080 *cpu) {
    uint8_t reg_code = opcode & 0x07;
    uint8_t value = (reg_code == 6)
                    ? cpu->memory[get_rpair(cpu->H, cpu->L)]
                    : *cpu->registers[reg_code];
    
    update_flags_after_sub(cpu->A, value, 0, cpu);
    cpu->PC += 1;
}

void cpi(uint8_t opcode, CPU8080 *cpu) {
    (void)opcode;
    uint8_t value = cpu->memory[cpu->PC + 1];

    update_flags_after_sub(cpu->A, value, 0, cpu);
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
    cpu->flags.CY = true;
    cpu->PC += 1;
}