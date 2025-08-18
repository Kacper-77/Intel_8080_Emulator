#include "helpers.h"

void update_flags_after_add(uint16_t result, uint8_t operand, CPU8080 *cpu, bool include_cy) {
    bool old_cy = cpu->flags.CY;

    cpu->flags.Z  = ((result & 0xFF) == 0);  // Zero
    cpu->flags.S  = (result & 0x80) != 0;  // Sign (bit 7)
    cpu->flags.P  = __builtin_parity(result & 0xFF) == 0;  // Parity
    cpu->flags.CY = (result > 0xFF);  // Carry
    if (include_cy) {
        cpu->flags.AC = ((cpu->A & 0xF) + (operand & 0xF) + old_cy) > 0xF;
    } else {
        cpu->flags.AC = ((cpu->A & 0xF) + (operand & 0xF)) > 0xF; // Auxiliary Carry
    }
}