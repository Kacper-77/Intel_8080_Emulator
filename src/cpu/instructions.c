#include "instructions.h"
#include "stdint.h"
#include "../utils/helpers.h"

void mvi(uint8_t *reg, CPU8080 *cpu) {
    *reg = cpu->memory[cpu->PC + 1];
    cpu->PC += 2;
}

void add(uint8_t value, CPU8080 *cpu) {
    uint16_t result = cpu->A + value;

    update_flags_after_add(result, value, cpu, false);

    cpu->A = result & 0xFF;
    cpu->PC += 1;
}

void adi(CPU8080 *cpu) {
    uint8_t value = cpu->memory[cpu->PC + 1];
    uint16_t result = cpu->A + value;

    update_flags_after_add(result, value, cpu, false);

    cpu->A = result & 0xFF;
    cpu->PC += 2;
}

void aci(CPU8080 *cpu) {
    uint8_t value = cpu->memory[cpu->PC + 1];
    uint16_t result = cpu->A + value + cpu->flags.CY;

    update_flags_after_add(result, value, cpu, true);

    cpu->A = result & 0xFF;
    cpu->PC += 2;
}

void jmp(CPU8080 *cpu) {
    uint16_t addr = cpu->memory[cpu->PC + 1] | (cpu->memory[cpu->PC + 2] << 8);
    cpu->PC = addr;
}