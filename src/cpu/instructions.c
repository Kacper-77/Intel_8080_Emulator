#include "instructions.h"
#include "stdint.h"
#include "../utils/helpers.h"
#include "stddef.h"

void nop(uint8_t opcode, CPU8080 *cpu) {
    (void)opcode;
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

void add(uint8_t *reg, CPU8080 *cpu) {
    uint8_t value;
    if (reg == NULL) {
        value = cpu->A + cpu->memory[(cpu->H << 8) | cpu->L];
    } else
    {
        value = *reg;
    }
    uint16_t result = cpu->A + value;
    cpu->A = result & 0xFF;
    update_flags_after_add(result, value, cpu, false);

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

void jmp(uint8_t opcode, CPU8080 *cpu) {
    (void)opcode;
    uint16_t addr = cpu->memory[cpu->PC + 1] | (cpu->memory[cpu->PC + 2] << 8);
    cpu->PC = addr;
}

void hlt(uint8_t opcode, CPU8080 *cpu) {
    (void)opcode;
    cpu->halted = true;
}

void inr(uint8_t *reg, CPU8080 *cpu) {
    uint8_t result, before;

    if (reg == NULL) {
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

void dcr(uint8_t *reg, CPU8080 *cpu) {
    uint8_t result, before;

    if (reg == NULL) {
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

void mov_generic(uint8_t opcode, CPU8080 *cpu) {
    uint8_t dest_code = (opcode >> 3) & 0x07;
    uint8_t src_code  = opcode & 0x07;

    uint8_t *dest = cpu->registers[dest_code];
    uint8_t *src = cpu->registers[src_code];

    mov(dest, src, cpu);
}

void mvi_generic(uint8_t opcode, CPU8080 *cpu) {
    uint8_t reg_code = (opcode >> 3) & 0x07;

    uint8_t *reg = cpu->registers[reg_code];
    mvi(reg, cpu);
}

void add_generic(uint8_t opcode, CPU8080 *cpu) {
    uint8_t reg_code = (opcode >> 3) & 0x07;
    uint8_t *reg = cpu->registers[reg_code];

    add(reg, cpu);
}

void inr_generic(uint8_t opcode, CPU8080 *cpu) {
    uint8_t reg_code = (opcode >> 3) & 0x07;
    uint8_t *reg = cpu->registers[reg_code];

    inr(reg, cpu);
}

void dcr_generic(uint8_t opcode, CPU8080 *cpu) {
    uint8_t reg_code = (opcode >> 3) & 0x07;
    uint8_t *reg = cpu->registers[reg_code];

    dcr(reg, cpu);
}