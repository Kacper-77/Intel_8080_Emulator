#include "helpers.h"
#include <stdio.h>

#define ALU_DEBUG 0

#if ALU_DEBUG
#define LOG_ALU(fmt, ...) printf(fmt, ##__VA_ARGS__)
#else
#define LOG_ALU(fmt, ...) (void)0
#endif

// DEBUG HELPER
static void print_flags(const CPU8080* cpu) {
    printf("  [PC=%04X Z=%d S=%d P=%d CY=%d AC=%d ]\n",
            cpu->PC, cpu->flags.Z, cpu->flags.S, cpu->flags.P, cpu->flags.CY, cpu->flags.AC);
}

// Set Z, S and P flags - method is universal
void set_ZSP(uint8_t result, CPU8080* cpu) {
    uint8_t res = (result) & 0xFF;
    cpu->flags.Z  = (((res) & 0xFF) == 0);
    cpu->flags.S  = ((res) & 0x80) != 0;
    cpu->flags.P  = my_parity((res) & 0xFF);
}

// Check parity of value
static bool my_parity(uint8_t value) {
    value ^= value >> 4;
    value ^= value >> 2;
    value ^= value >> 1;
    return ~value & 1;
}

// Check carry by position (bit_no)
bool is_carry(int bit_no, uint8_t a, uint8_t val, bool cy) {
  int16_t result = a + val + cy;
  int16_t carry = result ^ a ^ val;
  return carry & (1 << bit_no);
}

// Update A and set flags
void alu_do_add(uint8_t a, uint8_t val, uint8_t carry_in, CPU8080 *cpu) {
    uint16_t sum = (uint16_t)a + (uint16_t)val + (uint16_t)carry_in;
    uint8_t result = (uint8_t)sum;

    // CY and AC before ZSP!
    cpu->flags.CY = (is_carry(8, a, val, carry_in) != 0);
    cpu->flags.AC = (is_carry(4, a, val, carry_in) != 0);

    // Z, S, P
    set_ZSP(result, cpu);

    cpu->A = result;
    LOG_ALU("[ADD] A=%02X val=%02X carry_in=%d -> result=%02X  ",
        a, val, carry_in, result);
    if (ALU_DEBUG) print_flags(cpu);
}

// Update A and set flags
void alu_do_sub(uint8_t a, uint8_t val, uint8_t borrow_in, CPU8080 *cpu) {
    uint8_t inv_borrow = borrow_in ? 0 : 1; // !borrow!
    alu_do_add(a, (uint8_t)~val, inv_borrow, cpu);

    cpu->flags.CY = !cpu->flags.CY;
    LOG_ALU("[SUB] A=%02X val=%02X borrow_in=%d  ",
        a, val, borrow_in);
    if (ALU_DEBUG) print_flags(cpu);
}

// Update register and set flags
void update_flags_after_inr_dcr(uint8_t result, uint8_t before, CPU8080 *cpu, bool is_inr) {
    set_ZSP(result, cpu);
    if (is_inr) {
        cpu->flags.AC = (result & 0xF) == 0;
    } else {
        cpu->flags.AC = !((result & 0xF) == 0xF);
    }
    // CY not affected!
    LOG_ALU("[INR/DCR] result=%02X before=%02X  ",
        result, before);
    if (ALU_DEBUG) print_flags(cpu);
}

// Update flags after logical instructions
void update_flags_logical(uint8_t a, uint8_t value, uint8_t result, CPU8080 *cpu, char op) {
    set_ZSP(result, cpu);
    cpu->flags.CY = 0;

    if (op == '&') {  // ANA / ANI only
        cpu->flags.AC = ((a | value) & 0x08) != 0;
    } else {
        cpu->flags.AC = 0;
    }
    LOG_ALU("[LOGICAL] A=%02X val=%02X result=%d  ",
        a, value, result);
    if (ALU_DEBUG) print_flags(cpu);
}

// Get pair of registers
uint16_t get_rpair(uint8_t high, uint8_t low) {
    return (high << 8) | low;
}

// Set pair of registers
void set_rpair(uint16_t value, uint8_t *high, uint8_t *low) {
    *high = (value >> 8) & 0xFF;
    *low = value & 0xFF;
}

// Get address immediately
uint16_t fetch_addr(CPU8080 *cpu) {
    return cpu->memory[cpu->PC + 1] | (cpu->memory[cpu->PC + 2] << 8);
}

// Set return address - STACK!
void set_return_addr(uint16_t addr, CPU8080 *cpu) {
    cpu->SP--;
    cpu->memory[cpu->SP] = (addr >> 8) & 0xFF; // hi byte
    cpu->SP--;
    cpu->memory[cpu->SP] = addr & 0xFF;        // lo byte
}

// Get return address from STACK!
uint16_t fetch_return_addr(CPU8080 *cpu) {
    uint16_t lo = cpu->memory[cpu->SP++];
    uint16_t hi = cpu->memory[cpu->SP++];
    return (hi << 8) | lo;
}