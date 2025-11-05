#include "helpers.h"
#include <stdio.h>

#define ALU_DEBUG 0

#if ALU_DEBUG
#define LOG_ALU(fmt, ...) printf(fmt, ##__VA_ARGS__)
#else
#define LOG_ALU(fmt, ...) (void)0
#endif

void SET_ZSP(uint8_t result, CPU8080* cpu) {
    uint8_t res = (result) & 0xFF;
    cpu->flags.Z  = (((res) & 0xFF) == 0);
    cpu->flags.S  = ((res) & 0x80) != 0;
    cpu->flags.P  = my_parity((res) & 0xFF);
}

uint8_t my_parity(uint8_t value) {
    value ^= value >> 4;
    value ^= value >> 2;
    value ^= value >> 1;
    return ~value & 1;
}

bool carry(int bit_no, uint8_t a, uint8_t val, bool cy) {
  int16_t result = a + val + cy;
  int16_t carry = result ^ a ^ val;
  return carry & (1 << bit_no);
}

void alu_do_add(uint8_t a, uint8_t val, uint8_t carry_in, CPU8080 *cpu) {
    uint16_t sum = (uint16_t)a + (uint16_t)val + (uint16_t)carry_in;
    uint8_t result = (uint8_t)sum;

    // CY and AC before ZSP!
    cpu->flags.CY = (carry(8, a, val, carry_in) != 0);
    cpu->flags.AC = (carry(4, a, val, carry_in) != 0);

    cpu->flags.Z = (result == 0);
    cpu->flags.S = (result & 0x80) != 0;
    cpu->flags.P = my_parity(result);

    cpu->A = result;
}

void alu_do_sub(uint8_t a, uint8_t val, uint8_t borrow_in, CPU8080 *cpu, bool affect_a) {
    uint8_t inv_borrow = borrow_in ? 0 : 1; // !borrow!
    alu_do_add(a, (uint8_t)~val, inv_borrow, cpu);

    cpu->flags.CY = !cpu->flags.CY;

    if (!affect_a) {
        cpu->A = a;
    }
}

void update_flags_after_inr_dcr(uint8_t result, uint8_t before, CPU8080 *cpu, bool is_inr) {
    SET_ZSP(result, cpu);
    if (is_inr) {
        cpu->flags.AC = (result & 0xF) == 0;
    } else {
        cpu->flags.AC = !((result & 0xF) == 0xF);
    }
    // CY not affected!
}

void update_flags_logical(uint8_t a, uint8_t value, uint8_t result, CPU8080 *cpu, char op) {
    SET_ZSP(result, cpu);
    cpu->flags.CY = 0;

    switch (op) {
        case '&':  // AND / ANI
            cpu->flags.AC = ((a | value) & 0x08) != 0;
            break;
        case '|':  // OR / ORI
        case '^':  // XOR / XRI
        default:
            cpu->flags.AC = 0;
            break;
    }
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