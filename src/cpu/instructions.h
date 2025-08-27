#pragma once
#include "cpu.h"

void nop(uint8_t opcode, CPU8080 *cpu);
void adi(uint8_t opcode, CPU8080 *cpu);
void aci(uint8_t opcode, CPU8080 *cpu);
void jmp(uint8_t opcode, CPU8080 *cpu);
void hlt(uint8_t opcode, CPU8080 *cpu);
void call(uint8_t opcode, CPU8080 *cpu);
void ret(uint8_t opcode, CPU8080 *cpu);

void mov_generic(uint8_t opcode, CPU8080 *cpu);
void mvi_generic(uint8_t opcode, CPU8080 *cpu);
void add_generic(uint8_t opcode, CPU8080 *cpu);
void inr_generic(uint8_t opcode, CPU8080 *cpu);
void dcr_generic(uint8_t opcode, CPU8080 *cpu);
void lxi_generic(uint8_t opcode, CPU8080 *cpu);
void sub_generic(uint8_t opcode, CPU8080 *cpu);