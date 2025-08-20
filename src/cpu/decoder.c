#include "decoder.h"
#include "instructions.h"
#include <stdio.h>

// MVI r, byte
void wrapper_mvi_b(CPU8080 *cpu) { mvi(&cpu->B, cpu); }
void wrapper_mvi_c(CPU8080 *cpu) { mvi(&cpu->C, cpu); }
void wrapper_mvi_d(CPU8080 *cpu) { mvi(&cpu->D, cpu); }
void wrapper_mvi_e(CPU8080 *cpu) { mvi(&cpu->E, cpu); }
void wrapper_mvi_h(CPU8080 *cpu) { mvi(&cpu->H, cpu); }
void wrapper_mvi_l(CPU8080 *cpu) { mvi(&cpu->L, cpu); }
void wrapper_mvi_a(CPU8080 *cpu) { mvi(&cpu->A, cpu); }
void wrapper_mvi_m(CPU8080 *cpu) { mvi(NULL, cpu); }

// ADD r
void wrapper_add_b(CPU8080 *cpu) { add(cpu->B, cpu); }
void wrapper_add_c(CPU8080 *cpu) { add(cpu->C, cpu); }
void wrapper_add_d(CPU8080 *cpu) { add(cpu->D, cpu); }
void wrapper_add_e(CPU8080 *cpu) { add(cpu->E, cpu); }
void wrapper_add_h(CPU8080 *cpu) { add(cpu->H, cpu); }
void wrapper_add_l(CPU8080 *cpu) { add(cpu->L, cpu); }
void wrapper_add_a(CPU8080 *cpu) { add(cpu->A, cpu); }
void wrapper_add_m(CPU8080 *cpu) { add(cpu->memory[(cpu->H << 8) | cpu->L], cpu); }

// INR r
void wrapper_inr_b(CPU8080 *cpu) { inr(&cpu->B, cpu, false); }
void wrapper_inr_c(CPU8080 *cpu) { inr(&cpu->C, cpu, false); }
void wrapper_inr_d(CPU8080 *cpu) { inr(&cpu->D, cpu, false); }
void wrapper_inr_e(CPU8080 *cpu) { inr(&cpu->E, cpu, false); }
void wrapper_inr_h(CPU8080 *cpu) { inr(&cpu->H, cpu, false); }
void wrapper_inr_l(CPU8080 *cpu) { inr(&cpu->L, cpu, false); }
void wrapper_inr_a(CPU8080 *cpu) { inr(&cpu->A, cpu, false); }
void wrapper_inr_m(CPU8080 *cpu) { inr(NULL, cpu, true); }

// DCR r
void wrapper_dcr_b(CPU8080 *cpu) { dcr(&cpu->B, cpu, false); }
void wrapper_dcr_c(CPU8080 *cpu) { dcr(&cpu->C, cpu, false); }
void wrapper_dcr_d(CPU8080 *cpu) { dcr(&cpu->D, cpu, false); }
void wrapper_dcr_e(CPU8080 *cpu) { dcr(&cpu->E, cpu, false); }
void wrapper_dcr_h(CPU8080 *cpu) { dcr(&cpu->H, cpu, false); }
void wrapper_dcr_l(CPU8080 *cpu) { dcr(&cpu->L, cpu, false); }
void wrapper_dcr_a(CPU8080 *cpu) { dcr(&cpu->A, cpu, false); }
void wrapper_dcr_m(CPU8080 *cpu) { dcr(NULL, cpu, true); }

// MOV B, *
void wrapper_mov_b_b(CPU8080 *cpu) { mov(&cpu->B, &cpu->B, cpu); }
void wrapper_mov_b_c(CPU8080 *cpu) { mov(&cpu->B, &cpu->C, cpu); }
void wrapper_mov_b_d(CPU8080 *cpu) { mov(&cpu->B, &cpu->D, cpu); }
void wrapper_mov_b_e(CPU8080 *cpu) { mov(&cpu->B, &cpu->E, cpu); }
void wrapper_mov_b_h(CPU8080 *cpu) { mov(&cpu->B, &cpu->H, cpu); }
void wrapper_mov_b_l(CPU8080 *cpu) { mov(&cpu->B, &cpu->L, cpu); }
void wrapper_mov_b_m(CPU8080 *cpu) { mov(&cpu->B, NULL, cpu); }
void wrapper_mov_b_a(CPU8080 *cpu) { mov(&cpu->B, &cpu->A, cpu); }

// MOV C, *
void wrapper_mov_c_b(CPU8080 *cpu) { mov(&cpu->C, &cpu->B, cpu); }
void wrapper_mov_c_c(CPU8080 *cpu) { mov(&cpu->C, &cpu->C, cpu); }
void wrapper_mov_c_d(CPU8080 *cpu) { mov(&cpu->C, &cpu->D, cpu); }
void wrapper_mov_c_e(CPU8080 *cpu) { mov(&cpu->C, &cpu->E, cpu); }
void wrapper_mov_c_h(CPU8080 *cpu) { mov(&cpu->C, &cpu->H, cpu); }
void wrapper_mov_c_l(CPU8080 *cpu) { mov(&cpu->C, &cpu->L, cpu); }
void wrapper_mov_c_m(CPU8080 *cpu) { mov(&cpu->C, NULL, cpu); }
void wrapper_mov_c_a(CPU8080 *cpu) { mov(&cpu->C, &cpu->A, cpu); }

// MOV D, *
void wrapper_mov_d_b(CPU8080 *cpu) { mov(&cpu->D, &cpu->B, cpu); }
void wrapper_mov_d_c(CPU8080 *cpu) { mov(&cpu->D, &cpu->C, cpu); }
void wrapper_mov_d_d(CPU8080 *cpu) { mov(&cpu->D, &cpu->D, cpu); }
void wrapper_mov_d_e(CPU8080 *cpu) { mov(&cpu->D, &cpu->E, cpu); }
void wrapper_mov_d_h(CPU8080 *cpu) { mov(&cpu->D, &cpu->H, cpu); }
void wrapper_mov_d_l(CPU8080 *cpu) { mov(&cpu->D, &cpu->L, cpu); }
void wrapper_mov_d_m(CPU8080 *cpu) { mov(&cpu->D, NULL, cpu); }
void wrapper_mov_d_a(CPU8080 *cpu) { mov(&cpu->D, &cpu->A, cpu); }

// MOV E, *
void wrapper_mov_e_b(CPU8080 *cpu) { mov(&cpu->E, &cpu->B, cpu); }
void wrapper_mov_e_c(CPU8080 *cpu) { mov(&cpu->E, &cpu->C, cpu); }
void wrapper_mov_e_d(CPU8080 *cpu) { mov(&cpu->E, &cpu->D, cpu); }
void wrapper_mov_e_e(CPU8080 *cpu) { mov(&cpu->E, &cpu->E, cpu); }
void wrapper_mov_e_h(CPU8080 *cpu) { mov(&cpu->E, &cpu->H, cpu); }
void wrapper_mov_e_l(CPU8080 *cpu) { mov(&cpu->E, &cpu->L, cpu); }
void wrapper_mov_e_m(CPU8080 *cpu) { mov(&cpu->E, NULL, cpu); }
void wrapper_mov_e_a(CPU8080 *cpu) { mov(&cpu->E, &cpu->A, cpu); }

// MOV H, *
void wrapper_mov_h_b(CPU8080 *cpu) { mov(&cpu->H, &cpu->B, cpu); }
void wrapper_mov_h_c(CPU8080 *cpu) { mov(&cpu->H, &cpu->C, cpu); }
void wrapper_mov_h_d(CPU8080 *cpu) { mov(&cpu->H, &cpu->D, cpu); }
void wrapper_mov_h_e(CPU8080 *cpu) { mov(&cpu->H, &cpu->E, cpu); }
void wrapper_mov_h_h(CPU8080 *cpu) { mov(&cpu->H, &cpu->H, cpu); }
void wrapper_mov_h_l(CPU8080 *cpu) { mov(&cpu->H, &cpu->L, cpu); }
void wrapper_mov_h_m(CPU8080 *cpu) { mov(&cpu->H, NULL, cpu); }
void wrapper_mov_h_a(CPU8080 *cpu) { mov(&cpu->H, &cpu->A, cpu); }

// MOV L, *
void wrapper_mov_l_b(CPU8080 *cpu) { mov(&cpu->L, &cpu->B, cpu); }
void wrapper_mov_l_c(CPU8080 *cpu) { mov(&cpu->L, &cpu->C, cpu); }
void wrapper_mov_l_d(CPU8080 *cpu) { mov(&cpu->L, &cpu->D, cpu); }
void wrapper_mov_l_e(CPU8080 *cpu) { mov(&cpu->L, &cpu->E, cpu); }
void wrapper_mov_l_h(CPU8080 *cpu) { mov(&cpu->L, &cpu->H, cpu); }
void wrapper_mov_l_l(CPU8080 *cpu) { mov(&cpu->L, &cpu->L, cpu); }
void wrapper_mov_l_m(CPU8080 *cpu) { mov(&cpu->L, NULL, cpu); }
void wrapper_mov_l_a(CPU8080 *cpu) { mov(&cpu->L, &cpu->A, cpu); }

// MOV M, *
void wrapper_mov_m_b(CPU8080 *cpu) { mov(NULL, &cpu->B, cpu); }
void wrapper_mov_m_c(CPU8080 *cpu) { mov(NULL, &cpu->C, cpu); }
void wrapper_mov_m_d(CPU8080 *cpu) { mov(NULL, &cpu->D, cpu); }
void wrapper_mov_m_e(CPU8080 *cpu) { mov(NULL, &cpu->E, cpu); }
void wrapper_mov_m_h(CPU8080 *cpu) { mov(NULL, &cpu->H, cpu); }
void wrapper_mov_m_l(CPU8080 *cpu) { mov(NULL, &cpu->L, cpu); }
void wrapper_mov_m_a(CPU8080 *cpu) { mov(NULL, &cpu->A, cpu); }

// MOV A, *
void wrapper_mov_a_b(CPU8080 *cpu) { mov(&cpu->A, &cpu->B, cpu); }
void wrapper_mov_a_c(CPU8080 *cpu) { mov(&cpu->A, &cpu->C, cpu); }
void wrapper_mov_a_d(CPU8080 *cpu) { mov(&cpu->A, &cpu->D, cpu); }
void wrapper_mov_a_e(CPU8080 *cpu) { mov(&cpu->A, &cpu->E, cpu); }
void wrapper_mov_a_h(CPU8080 *cpu) { mov(&cpu->A, &cpu->H, cpu); }
void wrapper_mov_a_l(CPU8080 *cpu) { mov(&cpu->A, &cpu->L, cpu); }
void wrapper_mov_a_m(CPU8080 *cpu) { mov(&cpu->A, NULL, cpu); }
void wrapper_mov_a_a(CPU8080 *cpu) { mov(&cpu->A, &cpu->A, cpu); }



Instruction instruction_table[] = {
    // MVI r, byte
    {0x06, wrapper_mvi_b}, {0x0E, wrapper_mvi_c}, {0x16, wrapper_mvi_d},
    {0x1E, wrapper_mvi_e}, {0x26, wrapper_mvi_h}, {0x2E, wrapper_mvi_l},
    {0x36, wrapper_mvi_m}, {0x3E, wrapper_mvi_a},

    // ADD r
    {0x80, wrapper_add_b}, {0x81, wrapper_add_c}, {0x82, wrapper_add_d},
    {0x83, wrapper_add_e}, {0x84, wrapper_add_h}, {0x85, wrapper_add_l},
    {0x86, wrapper_add_m}, {0x87, wrapper_add_a},

    // ADI byte
    {0xC6, adi},

    // ACI byte
    {0xCE, aci},

    // JMP addr
    {0xC3, jmp},

    // HLT
    {0x76, hlt},

    // NOP
    {0x00, nop},

    // INR r / INR M
    {0x04, wrapper_inr_b}, {0x0C, wrapper_inr_c}, {0x14, wrapper_inr_d},
    {0x1C, wrapper_inr_e}, {0x24, wrapper_inr_h}, {0x2C, wrapper_inr_l},
    {0x34, wrapper_inr_m}, {0x3C, wrapper_inr_a},

    // DCR r / DCR M
    {0x05, wrapper_dcr_b}, {0x0D, wrapper_dcr_c}, {0x15, wrapper_dcr_d},
    {0x1D, wrapper_dcr_e}, {0x25, wrapper_dcr_h}, {0x2D, wrapper_dcr_l},
    {0x35, wrapper_dcr_m}, {0x3D, wrapper_dcr_a},

    // MOV r1, r2
    {0x40, wrapper_mov_b_b}, {0x41, wrapper_mov_b_c}, {0x42, wrapper_mov_b_d}, {0x43, wrapper_mov_b_e},
    {0x44, wrapper_mov_b_h}, {0x45, wrapper_mov_b_l}, {0x46, wrapper_mov_b_m}, {0x47, wrapper_mov_b_a},

    {0x48, wrapper_mov_c_b}, {0x49, wrapper_mov_c_c}, {0x4A, wrapper_mov_c_d}, {0x4B, wrapper_mov_c_e},
    {0x4C, wrapper_mov_c_h}, {0x4D, wrapper_mov_c_l}, {0x4E, wrapper_mov_c_m}, {0x4F, wrapper_mov_c_a},

    {0x50, wrapper_mov_d_b}, {0x51, wrapper_mov_d_c}, {0x52, wrapper_mov_d_d}, {0x53, wrapper_mov_d_e},
    {0x54, wrapper_mov_d_h}, {0x55, wrapper_mov_d_l}, {0x56, wrapper_mov_d_m}, {0x57, wrapper_mov_d_a},

    {0x58, wrapper_mov_e_b}, {0x59, wrapper_mov_e_c}, {0x5A, wrapper_mov_e_d}, {0x5B, wrapper_mov_e_e},
    {0x5C, wrapper_mov_e_h}, {0x5D, wrapper_mov_e_l}, {0x5E, wrapper_mov_e_m}, {0x5F, wrapper_mov_e_a},

    {0x60, wrapper_mov_h_b}, {0x61, wrapper_mov_h_c}, {0x62, wrapper_mov_h_d}, {0x63, wrapper_mov_h_e},
    {0x64, wrapper_mov_h_h}, {0x65, wrapper_mov_h_l}, {0x66, wrapper_mov_h_m}, {0x67, wrapper_mov_h_a},

    {0x68, wrapper_mov_l_b}, {0x69, wrapper_mov_l_c}, {0x6A, wrapper_mov_l_d}, {0x6B, wrapper_mov_l_e},
    {0x6C, wrapper_mov_l_h}, {0x6D, wrapper_mov_l_l}, {0x6E, wrapper_mov_l_m}, {0x6F, wrapper_mov_l_a},

    {0x70, wrapper_mov_m_b}, {0x71, wrapper_mov_m_c}, {0x72, wrapper_mov_m_d}, {0x73, wrapper_mov_m_e},
    {0x74, wrapper_mov_m_h}, {0x75, wrapper_mov_m_l}, {0x77, wrapper_mov_m_a},

    {0x78, wrapper_mov_a_b}, {0x79, wrapper_mov_a_c}, {0x7A, wrapper_mov_a_d}, {0x7B, wrapper_mov_a_e},
    {0x7C, wrapper_mov_a_h}, {0x7D, wrapper_mov_a_l}, {0x7E, wrapper_mov_a_m}, {0x7F, wrapper_mov_a_a},
};

void execute_instruction(CPU8080 *cpu, uint8_t opcode) {
    for (size_t i = 0; i < sizeof(instruction_table)/sizeof(instruction_table[0]); i++) {
        if (instruction_table[i].opcode == opcode) {
            instruction_table[i].func(cpu);
            return;
        }
    }
    printf("Unknown opcode: 0x%02X\n", opcode);
}