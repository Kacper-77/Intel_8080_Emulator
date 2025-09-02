#pragma once
#include "../cpu/cpu.h"

typedef void (*InstructionFunc)(uint8_t opcode, CPU8080 *cpu);

extern bool custom_cycle;

void init_instruction_table(void);
extern InstructionFunc instruction_table[256];