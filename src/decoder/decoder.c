#include "decoder.h"
#include "../cpu/cpu.h"
#include "../instructions/instructions.h"
#include <stdio.h>

#define SET_RANGE_OP(start, stop, func) do { \
    for (int op = (start); op <= (stop); op++) { \
        instruction_table[op] = (func); \
    } \
} while (0)

#define SET_OP(func, ...) do { \
    uint8_t ops[] = { __VA_ARGS__ }; \
    for (int i = 0; i < sizeof(ops)/sizeof(ops[0]); i++) { \
        instruction_table[ops[i]] = (func); \
    } \
} while (0)

InstructionFunc instruction_table[256] = {0};
bool custom_cycle = false;

void init_instruction_table(void) {
    // All instructions (some are overriden, but it's not a mistake)
    SET_RANGE_OP(0x40, 0x7F, mov_generic);                                      // MOV
    SET_OP(mvi_generic, 0x06, 0x0E, 0x16, 0x1E, 0x26, 0x2E, 0x36, 0x3E);        // MVI
    SET_OP(lxi_generic, 0x01, 0x11, 0x21, 0x31);                                // LXI
    SET_RANGE_OP(0x80, 0x8F, add_adc_generic);                                  // ADD & ADC
    SET_RANGE_OP(0x90, 0x9F, sub_sbb_generic);                                  // SUB & SBB
    SET_OP(inr_generic, 0x04, 0x0C, 0x14, 0x1C, 0x24, 0x2C, 0x34, 0x3C);        // INR
    SET_OP(dcr_generic, 0x05, 0x0D, 0x15, 0x1D, 0x25, 0x2D, 0x35, 0x3D);        // DCR
    SET_OP(rst_generic, 0xC7, 0xCF, 0xD7, 0xDF, 0xE7, 0xEF, 0xF7, 0xFF);        // RST
    SET_OP(push_generic, 0xC5, 0xD5, 0xE5, 0xF5);                               // PUSH
    SET_OP(pop_generic, 0xC1, 0xD1, 0xE1, 0xF1);                                // POP
    SET_RANGE_OP(0xB8, 0xBF, cmp_generic);                                      // CMP
    SET_OP(dad_generic, 0x09, 0x19, 0x29, 0x39);                                // DAD
    SET_OP(inx_generic, 0x03, 0x13, 0x23, 0x33);                                // INX
    SET_OP(dcx_generic, 0x0B, 0x1B, 0x2B, 0x3B);                                // DCX
    SET_RANGE_OP(0xA0, 0xA7, ana_generic);                                      // ANA
    SET_RANGE_OP(0xB0, 0xB7, ora_generic);                                      // ORA
    SET_RANGE_OP(0xA8, 0xAF, xra_generic);                                      // XRA
    SET_OP(jmp_conditional, 0xC2, 0xCA, 0xD2, 0xDA, 0xE2, 0xEA, 0xF2, 0xFA);    // JMP  (conditional)
    SET_OP(call_conditional, 0xC4, 0xCC, 0xD4, 0xDC, 0xE4, 0xEC, 0xF4, 0xFC);   // CALL (conditional)
    SET_OP(ret_conditional, 0xC0, 0xC8, 0xD0, 0xD8, 0xE0, 0xE8, 0xF0, 0xF8);    // RET  (conditional)
    SET_OP(ldax_generic, 0x0A, 0x1A);                                           // LDAX
    SET_OP(stax_generic, 0x02, 0x12);                                           // STAX
    SET_OP(nop, 0x00, 0x08, 0x10, 0x18, 0x20, 0x28, 0x30, 0x38);                // NOP
    
    instruction_table[0x76] = hlt;      // HLT
    instruction_table[0xC3] = jmp;      // JMP
    instruction_table[0xCD] = call;     // CALL
    instruction_table[0xC9] = ret;      // RET
    instruction_table[0xC6] = adi;      // ADI
    instruction_table[0xCE] = aci;      // ACI
    instruction_table[0xFB] = ei;       // EI
    instruction_table[0xF3] = di;       // DI
    instruction_table[0xD6] = sui;      // SUI
    instruction_table[0xDE] = sbi;      // SBI
    instruction_table[0xFE] = cpi;      // CPI
    instruction_table[0xE6] = ani;      // ANI
    instruction_table[0xF6] = ori;      // ORI
    instruction_table[0xEE] = xri;      // XRI
    instruction_table[0x0F] = rrc;      // RRC
    instruction_table[0x07] = rlc;      // RLC
    instruction_table[0x1F] = rar;      // RAR
    instruction_table[0x17] = ral;      // RAL
    instruction_table[0x27] = daa;      // DAA
    instruction_table[0x3A] = lda;      // LDA
    instruction_table[0x32] = sta;      // STA
    instruction_table[0x22] = shld;     // SHLD
    instruction_table[0x2A] = lhld;     // LHLD
    instruction_table[0xEB] = xchg;     // XCHG
    instruction_table[0xF9] = sphl;     // SPHL
    instruction_table[0xE3] = xthl;     // XTHL
    instruction_table[0x2F] = cma;      // CMA
    instruction_table[0x3F] = cmc;      // CMC
    instruction_table[0x37] = stc;      // STC
    instruction_table[0xE9] = pchl;     // PCHL
    
    // I/O
    instruction_table[0xDB] = handle_in;    // IN
    instruction_table[0xD3] = handle_out;   // OUT
}