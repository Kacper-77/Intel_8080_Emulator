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
void ani(uint8_t opcode, CPU8080 *cpu);
void ori(uint8_t opcode, CPU8080 *cpu);
void xri(uint8_t opcode, CPU8080 *cpu);
void rrc(uint8_t opcode, CPU8080 *cpu);
void rlc(uint8_t opcode, CPU8080 *cpu);
void rar(uint8_t opcode, CPU8080 *cpu);
void ral(uint8_t opcode, CPU8080 *cpu);
void daa(uint8_t opcode, CPU8080 *cpu);
void lda(uint8_t opcode, CPU8080 *cpu);
void sta(uint8_t opcode, CPU8080 *cpu);
void shld(uint8_t opcode, CPU8080 *cpu);
void lhld(uint8_t opcode, CPU8080 *cpu);
void xchg(uint8_t opcode, CPU8080 *cpu);
void sphl(uint8_t opcode, CPU8080 *cpu);
void xthl(uint8_t opcode, CPU8080 *cpu);
void cma(uint8_t opcode, CPU8080 *cpu);
void cmc(uint8_t opcode, CPU8080 *cpu);
void stc(uint8_t opcode, CPU8080 *cpu);
void pchl(uint8_t opcode, CPU8080 *cpu);

void mov_generic(uint8_t opcode, CPU8080 *cpu);
void mvi_generic(uint8_t opcode, CPU8080 *cpu);
void add_adc_generic(uint8_t opcode, CPU8080 *cpu);
void inr_generic(uint8_t opcode, CPU8080 *cpu);
void dcr_generic(uint8_t opcode, CPU8080 *cpu);
void lxi_generic(uint8_t opcode, CPU8080 *cpu);
void sub_sbb_generic(uint8_t opcode, CPU8080 *cpu);
void rst_generic(uint8_t opcode, CPU8080 *cpu);
void push_generic(uint8_t opcode, CPU8080 *cpu);
void pop_generic(uint8_t opcode, CPU8080 *cpu);
void cmp_generic(uint8_t opcode, CPU8080 *cpu);
void dad_generic(uint8_t opcode, CPU8080 *cpu);
void inx_generic(uint8_t opcode, CPU8080 *cpu);
void dcx_generic(uint8_t opcode, CPU8080 *cpu);
void ana_generic(uint8_t opcode, CPU8080 *cpu);
void ora_generic(uint8_t opcode, CPU8080 *cpu);
void xra_generic(uint8_t opcode, CPU8080 *cpu);
void ldax_generic(uint8_t opcode, CPU8080 *cpu);
void stax_generic(uint8_t opcode, CPU8080 *cpu);

void jmp_conditional(uint8_t opcode, CPU8080 *cpu);
void call_conditional(uint8_t opcode, CPU8080 *cpu);
void ret_conditional(uint8_t opcode, CPU8080 *cpu);