#pragma once
#include <stdint.h>
#include <stdbool.h>


typedef struct {
    // 8-bit
    uint8_t A, B, C, D, E, H, L;

    uint8_t *registers[8];

    // 16-bit
    uint16_t SP;
    uint16_t PC;

    struct {
        bool Z;
        bool S;
        bool P;
        bool CY;
        bool AC;
    } flags;

    bool interrupt_enabled;  // EI AND DI
    bool pending_interrupt;
    bool is_interrupt;
    uint8_t interrupt_opcode;

    // 64KB memory
    uint8_t memory[0x10000];
    uint16_t stack_base;
    uint16_t heap_base;
    uint16_t heap_ptr;

    uint64_t cycles;

    bool halted;

} CPU8080;

void cpu_init(CPU8080 *cpu);
bool cpu_emulate(CPU8080 *cpu);
void cpu_load_program(CPU8080 *cpu, const uint8_t *program, uint16_t size, uint16_t load_addr);
extern const uint8_t t_states[256];
void request_interrupt(CPU8080 *cpu, uint8_t rst_opcode);
void trigger_trap(CPU8080 *cpu);
void execute_instruction(CPU8080 *cpu, uint8_t opcode);
bool check_heap_bounds(CPU8080 *cpu, uint16_t size);
void* heap_alloc(CPU8080 *cpu, uint16_t size);