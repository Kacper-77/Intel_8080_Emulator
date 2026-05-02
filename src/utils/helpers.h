#pragma once
#include <stdint.h>
#include <stdbool.h>
#include "../cpu/cpu.h"

void alu_do_add(uint8_t a, uint8_t val, uint8_t carry_in, CPU8080 *cpu);
void alu_do_sub(uint8_t a, uint8_t val, uint8_t borrow_in, CPU8080 *cpu);
void update_flags_after_inr_dcr(uint8_t result, uint8_t previous, CPU8080 *cpu, bool is_increment);
void update_flags_logical(uint8_t a, uint8_t value, uint8_t result, CPU8080 *cpu, char op);
bool is_carry(int bit_no, uint8_t a, uint8_t b, bool cy);
void set_ZSP(uint8_t result, CPU8080* cpu);

uint16_t get_rpair(uint8_t high, uint8_t low);
void set_rpair(uint16_t value, uint8_t *high, uint8_t *low);
static bool my_parity(uint8_t value);

uint16_t fetch_addr(CPU8080 *cpu);
void set_return_addr(uint16_t return_addr, CPU8080 *cpu);
uint16_t fetch_return_addr(CPU8080 *cpu);
