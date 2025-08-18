#ifndef INSTRUCTIONS_H
#define INSTRUCTIONS_H

#include "cpu.h"

void mvi(uint8_t *reg, CPU8080 *cpu);
void add(uint8_t value, CPU8080 *cpu);
void adi(CPU8080 *cpu);
void aci(CPU8080 *cpu);
void jmp(CPU8080 *cpu);

#endif