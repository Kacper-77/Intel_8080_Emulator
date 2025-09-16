#include "instructions.h"
#include "../decoder/decoder.h"
#include "../io_simulation/ports.h"

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

uint8_t io_in(uint8_t port) {
    switch (port) {
        case 0x01: return keyboard_read();
        default: return 0xFF;
    }
}

void handle_in(uint8_t opcode, CPU8080 *cpu) {
    (void)opcode;
    uint8_t port = cpu->memory[cpu->PC + 1];
    cpu->A = io_in(port);
    cpu->PC += 2;
}

void io_out(uint8_t port, uint8_t value) {
    switch (port) {
        case 0x02: monitor_write(value); break;
    }
}

void handle_out(uint8_t opcode, CPU8080 *cpu) {
    (void)opcode;
    uint8_t port = cpu->memory[cpu->PC + 1];
    io_out(port, cpu->A);
    cpu->PC += 2;
}
