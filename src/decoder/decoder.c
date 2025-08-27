#include "decoder.h"
#include "../cpu/cpu.h"
#include "../cpu/instructions.h"
#include <stdio.h>

InstructionFunc instruction_table[256] = {0};
bool custom_cycle = false;

void init_instruction_table(void) {
    // MOV r,r'
for (uint8_t opcode = 0x40; opcode <= 0x7F; opcode++) {
    instruction_table[opcode] = mov_generic;
}

// MVI r, data
instruction_table[0x06] = mvi_generic; // MVI B
instruction_table[0x0E] = mvi_generic; // MVI C
instruction_table[0x16] = mvi_generic; // MVI D
instruction_table[0x1E] = mvi_generic; // MVI E
instruction_table[0x26] = mvi_generic; // MVI H
instruction_table[0x2E] = mvi_generic; // MVI L
instruction_table[0x36] = mvi_generic; // MVI M
instruction_table[0x3E] = mvi_generic; // MVI A

// LXI rp, data16
instruction_table[0x01] = lxi_generic; // LXI B
instruction_table[0x11] = lxi_generic; // LXI D
instruction_table[0x21] = lxi_generic; // LXI H
instruction_table[0x31] = lxi_generic; // LXI SP

// ADD r
instruction_table[0x80] = add_generic; // ADD B
instruction_table[0x81] = add_generic; // ADD C
instruction_table[0x82] = add_generic; // ADD D
instruction_table[0x83] = add_generic; // ADD E
instruction_table[0x84] = add_generic; // ADD H
instruction_table[0x85] = add_generic; // ADD L
instruction_table[0x86] = add_generic; // ADD M
instruction_table[0x87] = add_generic; // ADD A

// SUB r
instruction_table[0x90] = sub_generic;
instruction_table[0x91] = sub_generic;
instruction_table[0x92] = sub_generic;
instruction_table[0x93] = sub_generic;
instruction_table[0x94] = sub_generic;
instruction_table[0x95] = sub_generic;
instruction_table[0x96] = sub_generic;
instruction_table[0x97] = sub_generic;

// INR r
instruction_table[0x04] = inr_generic;
instruction_table[0x0C] = inr_generic;
instruction_table[0x14] = inr_generic;
instruction_table[0x1C] = inr_generic;
instruction_table[0x24] = inr_generic;
instruction_table[0x2C] = inr_generic;
instruction_table[0x34] = inr_generic;
instruction_table[0x3C] = inr_generic;

// DCR r
instruction_table[0x05] = dcr_generic;
instruction_table[0x0D] = dcr_generic;
instruction_table[0x15] = dcr_generic;
instruction_table[0x1D] = dcr_generic;
instruction_table[0x25] = dcr_generic;
instruction_table[0x2D] = dcr_generic;
instruction_table[0x35] = dcr_generic;
instruction_table[0x3D] = dcr_generic;

// JMP, CALL, RET
instruction_table[0xC3] = jmp;
instruction_table[0xCD] = call;
instruction_table[0xC9] = ret;

// NOP
instruction_table[0x00] = nop;

// HLT
instruction_table[0x76] = hlt;

// ADI, ACI
instruction_table[0xC6] = adi;
instruction_table[0xCE] = aci;
}

void execute_instruction(CPU8080 *cpu, uint8_t opcode) {
    custom_cycle = false;
    if (instruction_table[opcode]) {
        instruction_table[opcode](opcode, cpu);
        if (!custom_cycle) {
            cpu->cycles += t_states[opcode];
        }
    } else {
        printf("Unknown opcode: 0x%02X\n", opcode);
    }
}