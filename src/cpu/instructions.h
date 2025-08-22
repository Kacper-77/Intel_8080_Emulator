#pragma once
#include "cpu.h"

void nop(uint8_t opcode, CPU8080 *cpu);
void mvi(uint8_t *reg, CPU8080 *cpu);
void add(uint8_t *src, CPU8080 *cpu);
void adi(CPU8080 *cpu);
void aci(CPU8080 *cpu);
void jmp(uint8_t opcode, CPU8080 *cpu);
void hlt(uint8_t opcode, CPU8080 *cpu);
void inr(uint8_t *reg, CPU8080 *cpu);
void dcr(uint8_t *reg, CPU8080 *cpu);
void mov(uint8_t *dest, uint8_t *src, CPU8080 *cpu);

void mov_generic(uint8_t opcode, CPU8080 *cpu);
void mvi_generic(uint8_t opcode, CPU8080 *cpu);
void add_generic(uint8_t opcode, CPU8080 *cpu);
void inr_generic(uint8_t opcode, CPU8080 *cpu);
void dcr_generic(uint8_t opcode, CPU8080 *cpu);