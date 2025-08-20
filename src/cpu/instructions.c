#include "instructions.h"
#include "stdint.h"
#include "../utils/helpers.h"
#include "stddef.h"

void nop(CPU8080 *cpu) {
    cpu->PC += 1;
}

void mvi(uint8_t *reg, CPU8080 *cpu) {
    uint8_t data = cpu->memory[cpu->PC + 1];

    if (reg == NULL) {
        cpu->memory[(cpu->H << 8) | cpu->L] = data;
    } else {
        *reg = data;
    }

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

void hlt(CPU8080 *cpu) {
    cpu->halted = true;
}

void inr(uint8_t *reg, CPU8080 *cpu, bool is_memory) {
    uint8_t before, result;

    if (is_memory) {
        uint16_t addr = (cpu->H << 8) | cpu->L;
        before = cpu->memory[addr];
        result = before + 1;
        cpu->memory[addr] = result;
    } else {
        before = *reg;
        result = before + 1;
        *reg = result;
    }

    set_flags_after_inr_dcr(result, before, cpu, true);

    cpu->PC += 1;
}

void dcr(uint8_t *reg, CPU8080 *cpu, bool is_memory) {
    uint8_t before, result;

    if (is_memory) {
        uint16_t addr = (cpu->H << 8) | cpu->L;
        before = cpu->memory[addr];
        result = before - 1;
        cpu->memory[addr] = result;
    } else {
        before = *reg;
        result = before - 1;
        *reg = result;
    }

    set_flags_after_inr_dcr(result, before, cpu, false);

    cpu->PC += 1;
}

void mov(uint8_t *dest, uint8_t *src, CPU8080 *cpu) {
    uint16_t HL = (cpu->H << 8) | cpu->L;

    if (dest == NULL && src != NULL) {  
        // MOV M, r
        cpu->memory[HL] = *src;
    } else if (dest != NULL && src == NULL) {  
        // MOV r, M
        *dest = cpu->memory[HL];
    } else { 
        // MOV r1, r2
        *dest = *src;
    }
    
    cpu->PC += 1;
}