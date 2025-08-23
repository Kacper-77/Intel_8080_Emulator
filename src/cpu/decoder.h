#pragma once
#include "cpu.h"

typedef void (*InstructionFunc)(uint8_t opcode, CPU8080 *cpu);

extern bool custom_cycle;

void execute_instruction(CPU8080 *cpu, uint8_t opcode);
void init_instruction_table(void);
extern InstructionFunc instruction_table[256];