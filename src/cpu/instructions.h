#pragma once
#include "cpu.h"

void nop(CPU8080 *cpu);
void mvi(uint8_t *reg, CPU8080 *cpu);
void add(uint8_t value, CPU8080 *cpu);
void adi(CPU8080 *cpu);
void aci(CPU8080 *cpu);
void jmp(CPU8080 *cpu);
void hlt(CPU8080 *cpu);
void inr(uint8_t *reg, CPU8080 *cpu, bool is_memory);
void dcr(uint8_t *reg, CPU8080 *cpu, bool is_memory);
void mov(uint8_t *dest, uint8_t *src, CPU8080 *cpu);