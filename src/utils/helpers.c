#include "helpers.h"
#include <stdio.h>

uint8_t my_parity(uint8_t value) {
    value ^= value >> 4;
    value ^= value >> 2;
    value ^= value >> 1;
    return ~value & 1;
}

void update_flags_after_add(uint16_t result, uint8_t operand, CPU8080 *cpu, bool include_cy) {
    bool old_cy = cpu->flags.CY;

    cpu->flags.Z  = ((result & 0xFF) == 0);  // Zero
    cpu->flags.S  = (result & 0x80) != 0;  // Sign (bit 7)
    cpu->flags.P  = my_parity(result & 0xFF);  // swapped with __builtin_parity
    cpu->flags.CY = (result > 0xFF);  // Carry
    if (include_cy) {
        cpu->flags.AC = ((cpu->A & 0x0F) + (operand & 0x0F) + old_cy) > 0x0F;
    } else {
        cpu->flags.AC = ((cpu->A & 0x0F) + (operand & 0x0F)) > 0x0F; // Auxiliary Carry
    }
}

void update_flags_after_inr_dcr(uint8_t result, uint8_t previous ,CPU8080 *cpu, bool is_increment) {
    cpu->flags.Z  = ((result & 0xFF) == 0);
    cpu->flags.S  = (result & 0x80) != 0;
    cpu->flags.P  = my_parity(result & 0xFF);
    if (is_increment) {
        cpu->flags.AC = ((previous & 0x0F) + 1) > 0x0F;
    } else {
        cpu->flags.AC = ((previous & 0x0F) - 1) < 0;
    }
}

void update_flags_after_sub(uint16_t a, uint8_t value, uint8_t borrow, CPU8080 *cpu) {
    uint16_t result = a - value - borrow;
    
    cpu->flags.Z = ((result & 0xFF) == 0);
    cpu->flags.S = ((result & 0x80) != 0);
    cpu->flags.P = my_parity(result & 0xFF);
    cpu->flags.CY = (a < (value + borrow));
    cpu->flags.AC = ((a & 0x0F) < ((value + borrow) & 0x0F));
}

void update_flags_logical(uint16_t result, CPU8080 *cpu, bool cleared) {
    cpu->flags.Z  = ((result & 0xFF) == 0);
    cpu->flags.S  = (result & 0x80) != 0;
    cpu->flags.P  = my_parity(result & 0xFF);
    cpu->flags.CY = 0;
    cpu->flags.AC = cleared ? 0 : 1;
}

uint16_t get_rpair(uint8_t high, uint8_t low) {
    return (high << 8) | low;
}

void set_rpair(uint8_t value, uint8_t *high, uint8_t *low) {
    *high = (value >> 8) & 0xFF;
    *low = value & 0xFF;
}

uint16_t fetch_addr(CPU8080 *cpu) {
    return (cpu->memory[cpu->PC + 2] << 8) | cpu->memory[cpu->PC + 1];
}

void set_return_addr(uint16_t return_addr, CPU8080 *cpu) {
    cpu->SP -= 2;
    check_stack_bounds(cpu);
    cpu->memory[cpu->SP] = return_addr & 0xFF;
    cpu->memory[cpu->SP + 1] = (return_addr >> 8) & 0xFF;
}

uint16_t fetch_return_addr(CPU8080 *cpu) {
    return (cpu->memory[cpu->SP + 1] << 8) | cpu->memory[cpu->SP];
}