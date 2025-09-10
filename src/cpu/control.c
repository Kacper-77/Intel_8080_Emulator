#include "instructions.h"
#include "../decoder/decoder.h"

void nop(uint8_t opcode, CPU8080 *cpu) {
    (void)opcode;
    cpu->PC += 1;
}

void hlt(uint8_t opcode, CPU8080 *cpu) {
    custom_cycle = true;
    cpu->cycles += t_states[opcode];
    cpu->halted = true;
}


void ei(uint8_t opcode, CPU8080 *cpu) {
    (void)opcode;
    cpu->interrupt_enabled = true;
    cpu->PC += 1;
}

void di(uint8_t opcode, CPU8080 *cpu) {
    (void)opcode;
    cpu->interrupt_enabled = false;
    cpu->PC += 1;
}