#include "helpers.h"
#include <stdio.h>

#define SET_ZSP(result, cpu) do { \
    uint8_t res = (result) & 0xFF; \
    cpu->flags.Z  = (((res) & 0xFF) == 0); \
    cpu->flags.S  = ((res) & 0x80) != 0; \
    cpu->flags.P  = my_parity((res) & 0xFF); \
} while (0)


uint8_t my_parity(uint8_t value) {
    value ^= value >> 4;
    value ^= value >> 2;
    value ^= value >> 1;
    return ~value & 1;
}

void update_flags_after_add(uint16_t result, uint8_t operand, uint8_t oldA, CPU8080 *cpu, bool include_cy) {
    bool old_cy = cpu->flags.CY;
    uint8_t res8 = result & 0xFF;

    SET_ZSP(res8, cpu);
    cpu->flags.CY = (result > 0xFF);
    cpu->flags.AC = ((oldA & 0x0F) + (operand & 0x0F) + (include_cy ? old_cy : 0)) > 0x0F;
}

void update_flags_after_sub(uint16_t a, uint8_t value, uint8_t borrow, CPU8080 *cpu) {
    uint16_t result = a - value - borrow;
    uint8_t res8 = result & 0xFF;

    SET_ZSP(res8, cpu);
    cpu->flags.CY = (a < (value + borrow));
    cpu->flags.AC = ((a & 0x0F) < ((value & 0x0F) + borrow));
}

void update_flags_after_inr_dcr(uint8_t result, uint8_t before, CPU8080 *cpu, bool is_inr) {
    SET_ZSP(result, cpu);

    if (is_inr) {
        cpu->flags.AC = ((before & 0x0F) + 1) > 0x0F;
    } else {
        cpu->flags.AC = (before & 0x0F) == 0x00;
    }
}

void update_flags_logical(uint16_t result, CPU8080 *cpu, bool cleared) {
    SET_ZSP(result, cpu);
    cpu->flags.CY = 0;
    cpu->flags.AC = cleared ? 0 : 1;
}

uint16_t get_rpair(uint8_t high, uint8_t low) {
    return (high << 8) | low;
}

void set_rpair(uint16_t value, uint8_t *high, uint8_t *low) {
    *high = (value >> 8) & 0xFF;
    *low = value & 0xFF;
}

uint16_t fetch_addr(CPU8080 *cpu) {
    return cpu->memory[cpu->PC + 1] | (cpu->memory[cpu->PC + 2] << 8);
}

void set_return_addr(uint16_t addr, CPU8080 *cpu) {
    cpu->SP--;
    cpu->memory[cpu->SP] = (addr >> 8) & 0xFF; // hi byte
    cpu->SP--;
    cpu->memory[cpu->SP] = addr & 0xFF;        // lo byte
}

uint16_t fetch_return_addr(CPU8080 *cpu) {
    uint16_t lo = cpu->memory[cpu->SP++];
    uint16_t hi = cpu->memory[cpu->SP++];
    return (hi << 8) | lo;
}