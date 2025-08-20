#pragma once
#include "cpu.h"

typedef struct {
    uint8_t opcode;
    void (*func)(CPU8080 *cpu);
} Instruction;

void execute_instruction(CPU8080 *cpu, uint8_t opcode);