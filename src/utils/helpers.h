#include <stdint.h>
#include <stdbool.h>
#include "cpu/cpu.h"

void update_flags_after_add(uint16_t result, uint8_t operand, CPU8080 *cpu, bool include_cy);