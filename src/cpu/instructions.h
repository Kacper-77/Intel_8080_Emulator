#pragma once
#include "cpu.h"

void nop(uint8_t opcode, CPU8080 *cpu);
void adi(uint8_t opcode, CPU8080 *cpu);
void aci(uint8_t opcode, CPU8080 *cpu);
void jmp(uint8_t opcode, CPU8080 *cpu);
void hlt(uint8_t opcode, CPU8080 *cpu);
void call(uint8_t opcode, CPU8080 *cpu);
void ret(uint8_t opcode, CPU8080 *cpu);
void ei(uint8_t opcode, CPU8080 *cpu);
void di(uint8_t opcode, CPU8080 *cpu);
void sui(uint8_t opcode, CPU8080 *cpu);
void sbi(uint8_t opcode, CPU8080 *cpu);
void cpi(uint8_t opcode, CPU8080 *cpu);

void mov_generic(uint8_t opcode, CPU8080 *cpu);
void mvi_generic(uint8_t opcode, CPU8080 *cpu);
void add_generic(uint8_t opcode, CPU8080 *cpu);
void inr_generic(uint8_t opcode, CPU8080 *cpu);
void dcr_generic(uint8_t opcode, CPU8080 *cpu);
void lxi_generic(uint8_t opcode, CPU8080 *cpu);
void sub_generic(uint8_t opcode, CPU8080 *cpu);
void rst_generic(uint8_t opcode, CPU8080 *cpu);
void push_generic(uint8_t opcode, CPU8080 *cpu);
void pop_generic(uint8_t opcode, CPU8080 *cpu);
void cmp_generic(uint8_t opcode, CPU8080 *cpu);