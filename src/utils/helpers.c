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

void update_flags_after_inr_dcr(uint8_t result, uint8_t previous ,CPU8080 *cpu, bool is_increment) {
    cpu->flags.Z  = ((result & 0xFF) == 0);
    cpu->flags.S  = (result & 0x80) != 0;
    cpu->flags.P  = __builtin_parity(result & 0xFF) == 0;
    if (is_increment) {
        cpu->flags.AC = ((previous & 0xF) + 1) > 0xF;
    } else {
        cpu->flags.AC = ((previous & 0xF) - 1) < 0;
    }
}

void update_flags_after_sub(uint16_t a, uint8_t value, CPU8080 *cpu) {
    uint16_t result = (uint16_t)a - value;
    
    cpu->flags.Z = ((result & 0xFF) == 0);
    cpu->flags.S = ((result & 0x80) != 0);

    uint8_t parity = result & 0xFF;  // manul method
    uint8_t count = 0;
    for (int i = 0; i < 8; i++) {
        if (parity & (1 << i)) count++;
    }
    cpu->flags.P = ((parity % 2) == 0);
    cpu->flags.CY = (a < value);
    cpu->flags.AC = (((a & 0x0F) - (value & 0x0F)) & 0x10) != 0;
}

bool is_zero(CPU8080 *cpu) { return cpu->flags.Z == 1; }
bool is_sign(CPU8080 *cpu) { return cpu->flags.S == 1; }
bool is_carry(CPU8080 *cpu) { return cpu->flags.CY == 1; }
bool is_parity(CPU8080 *cpu) { return cpu->flags.P == 1; }