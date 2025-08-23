#include <stdint.h>
#include <stdbool.h>
#include "../cpu/cpu.h"

void update_flags_after_add(uint16_t result, uint8_t operand, CPU8080 *cpu, bool include_cy);
void set_flags_after_inr_dcr(uint8_t result, uint8_t previous, CPU8080 *cpu, bool is_increment);

bool is_zero(CPU8080 *cpu);
bool is_sign(CPU8080 *cpu);
bool is_carry(CPU8080 *cpu);
bool is_parity(CPU8080 *cpu);