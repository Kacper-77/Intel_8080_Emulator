#include <stdint.h>
#include <stdbool.h>
#include "../cpu/cpu.h"

void update_flags_after_add(uint16_t result, uint8_t operand, CPU8080 *cpu, bool include_cy);
void update_flags_after_inr_dcr(uint8_t result, uint8_t previous, CPU8080 *cpu, bool is_increment);
void update_flags_after_sub(uint16_t a, uint8_t value, uint8_t borrow, CPU8080 *cpu);

uint16_t get_rpair(uint8_t high, uint8_t low);
void set_rpair(uint8_t value, uint8_t *high, uint8_t *low);