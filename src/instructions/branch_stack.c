#include "instructions.h"
#include "stdint.h"
#include <stdio.h>
#include "../utils/helpers.h"
#include "../decoder/decoder.h"

void call(uint8_t opcode, CPU8080 *cpu) {
    (void)opcode;
    uint16_t addr = fetch_addr(cpu);
    set_return_addr(cpu->PC + 3, cpu);

    cpu->PC = addr;
}

void ret(uint8_t opcode, CPU8080 *cpu) {
    (void)opcode;
    uint16_t addr = fetch_return_addr(cpu);
    cpu->PC = addr;
}

void rst_generic(uint8_t opcode, CPU8080 *cpu) {
    uint16_t return_addr = cpu->PC + (cpu->is_interrupt ? 0 : 1);
    set_return_addr(return_addr, cpu);

    uint8_t value = (opcode >> 3) & 0x07;
    cpu->PC = value * 8;
}

void call_conditional(uint8_t opcode, CPU8080 *cpu) {
    uint16_t addr = fetch_addr(cpu);
    bool should_call = false;

    switch (opcode) {
        case 0xC4: should_call = !cpu->flags.Z; break; // CNZ
        case 0xCC: should_call = cpu->flags.Z;  break; // CZ
        case 0xD4: should_call = !cpu->flags.CY;break; // CNC
        case 0xDC: should_call = cpu->flags.CY; break; // CC
        case 0xE4: should_call = !cpu->flags.P; break; // CPO
        case 0xEC: should_call = cpu->flags.P;  break; // CPE
        case 0xF4: should_call = !cpu->flags.S; break; // CP
        case 0xFC: should_call = cpu->flags.S;  break; // CM
        default: return;
    }

    if (should_call) {
        set_return_addr(cpu->PC + 3, cpu);
        cpu->PC = addr;
    } else {
        custom_cycle = true;
        cpu->cycles += 11;
        cpu->PC += 3;
    }
}

void ret_conditional(uint8_t opcode, CPU8080 *cpu) {
    bool should_return = false;

    switch (opcode) {
        case 0xC0: should_return = !cpu->flags.Z; break;  // RNZ
        case 0xC8: should_return =  cpu->flags.Z; break;  // RZ
        case 0xD0: should_return = !cpu->flags.CY; break; // RNC
        case 0xD8: should_return =  cpu->flags.CY; break; // RC
        case 0xE0: should_return = !cpu->flags.P; break;  // RPO
        case 0xE8: should_return =  cpu->flags.P; break;  // RPE
        case 0xF0: should_return = !cpu->flags.S; break;  // RP
        case 0xF8: should_return =  cpu->flags.S; break;  // RM
        default: return;
    }

    if (should_return) {
        uint16_t addr = fetch_return_addr(cpu);
        cpu->PC = addr;
    } else {
        custom_cycle = true;
        cpu->cycles += 5;
        cpu->PC += 1;
    }
}

void pchl(uint8_t opcode, CPU8080 *cpu) {
    (void)opcode;
    cpu->PC = get_rpair(cpu->H, cpu->L);
}

void jmp(uint8_t opcode, CPU8080 *cpu) {
    (void)opcode;
    uint16_t addr = fetch_addr(cpu);
    cpu->PC = addr;
}

void push_generic(uint8_t opcode, CPU8080 *cpu) {
    // PSW flag byte format: S Z 0 AC 0 P 1 CY
    uint8_t flags_byte =
    (cpu->flags.S  << 7) |
    (cpu->flags.Z  << 6) |
    (cpu->flags.AC << 4) |
    (cpu->flags.P  << 2) |
    (1 << 1) |
    (cpu->flags.CY << 0);

    cpu->SP -= 2;
    check_stack_bounds(cpu);

    switch (opcode) {
        case 0xC5:
            cpu->memory[cpu->SP + 1] = cpu->B;
            cpu->memory[cpu->SP] = cpu->C;
            break;
        case 0xD5:
            cpu->memory[cpu->SP + 1] = cpu->D;
            cpu->memory[cpu->SP] = cpu->E;
            break;
        case 0xE5:
            cpu->memory[cpu->SP + 1] = cpu->H;
            cpu->memory[cpu->SP] = cpu->L;
            break;
        case 0xF5:
            cpu->memory[cpu->SP + 1] = cpu->A;
            cpu->memory[cpu->SP] = flags_byte;
            break;
    }
    cpu->PC += 1;
}

void pop_generic(uint8_t opcode, CPU8080 *cpu) {
    uint8_t low = cpu->memory[cpu->SP];
    uint8_t high = cpu->memory[cpu->SP + 1];
    cpu->SP += 2;

    check_stack_bounds(cpu);

    // PSW flag byte format: S Z 0 AC 0 P 1 CY
    switch (opcode) {
        case 0xC1: cpu->C = low; cpu->B = high; break;
        case 0xD1: cpu->E = low; cpu->D = high; break;
        case 0xE1: cpu->L = low; cpu->H = high; break;
        case 0xF1:
            cpu->A = high;
            cpu->flags.S  = (low >> 7) & 1;
            cpu->flags.Z  = (low >> 6) & 1;
            cpu->flags.AC = (low >> 4) & 1;
            cpu->flags.P  = (low >> 2) & 1;
            cpu->flags.CY = (low >> 0) & 1;
            break;
    }
    cpu->PC += 1;
}


void jmp_conditional(uint8_t opcode, CPU8080 *cpu) {
    uint16_t addr = fetch_addr(cpu);
    bool should_jump = false;

    switch (opcode) {
        case 0xCA: should_jump = cpu->flags.Z;  break; // JZ
        case 0xC2: should_jump = !cpu->flags.Z; break; // JNZ
        case 0xDA: should_jump = cpu->flags.CY; break; // JC
        case 0xD2: should_jump = !cpu->flags.CY;break; // JNC
        case 0xFA: should_jump = cpu->flags.S == 0; break; // JP
        case 0xF2: should_jump = cpu->flags.S == 1; break; // JM
        case 0xEA: should_jump = cpu->flags.P;  break; // JPE
        case 0xE2: should_jump = !cpu->flags.P; break; // JPO
        default: return;
    }
    if (should_jump) {
        cpu->PC = addr;
    } else {
        cpu->PC = cpu->PC + 3;
    }
}

void sphl(uint8_t opcode, CPU8080 *cpu) {
    (void)opcode;
    cpu->SP = get_rpair(cpu->H, cpu->L);
    cpu->PC += 1;
}

void xthl(uint8_t opcode, CPU8080 *cpu) {
    (void)opcode;
    uint8_t tempH = cpu->H;
    uint8_t tempL = cpu->L;

    cpu->H = cpu->memory[cpu->SP + 1];
    cpu->L = cpu->memory[cpu->SP];
    cpu->memory[cpu->SP + 1] = tempH;
    cpu->memory[cpu->SP] = tempL;

    cpu->PC += 1;
}