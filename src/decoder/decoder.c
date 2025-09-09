#include "decoder.h"
#include "../cpu/cpu.h"
#include "../cpu/instructions.h"
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

    SET_RANGE_OP(0x40, 0x7F, mov_generic);
    SET_OP(mvi_generic, 0x06, 0x0E, 0x16, 0x1E, 0x26, 0x2E, 0x36, 0x3E);
    SET_OP(lxi_generic, 0x01, 0x11, 0x21, 0x31);
    SET_RANGE_OP(0x80, 0x87, add_generic);
    SET_RANGE_OP(0x90, 0x97, sub_generic);
    SET_OP(inr_generic, 0x04, 0x0C, 0x14, 0x1C, 0x24, 0x2C, 0x34, 0x3C);
    SET_OP(dcr_generic, 0x05, 0x0D, 0x15, 0x1D, 0x25, 0x2D, 0x35, 0x3D);
    SET_OP(rst_generic, 0xC7, 0xCF, 0xD7, 0xDF, 0xE7, 0xEF, 0xF7, 0xFF);
    SET_OP(push_generic, 0xC5, 0xD5, 0xE5, 0xF5);
    SET_OP(pop_generic, 0xC1, 0xD1, 0xE1, 0xF1);
    SET_RANGE_OP(0xB8, 0xBF, cmp_generic);
    SET_OP(dad_generic, 0x09, 0x19, 0x29, 0x39);
    SET_OP(inx_generic, 0x03, 0x13, 0x23, 0x33);
    SET_OP(dcx_generic, 0x0B, 0x1B, 0x2B, 0x3B);
    SET_RANGE_OP(0xA0, 0xA7, ana_generic);
    SET_RANGE_OP(0xB0, 0xB7, ora_generic);
    SET_RANGE_OP(0xA8, 0xAF, xra_generic);
    SET_OP(jmp_conditional, 0xC2, 0xCA, 0xD2, 0xDA, 0xE2, 0xEA, 0xF2, 0xFA);
    SET_OP(call_conditional, 0xC4, 0xCC, 0xD4, 0xDC, 0xE4, 0xEC, 0xF4, 0xFC);
    SET_OP(ret_conditional, 0xC0, 0xC8, 0xD0, 0xD8, 0xE0, 0xE8, 0xF0, 0xF8);
    SET_OP(ldax_generic, 0x0A, 0x1A);
    SET_OP(stax_generic, 0x02, 0x12);

    instruction_table[0x00] = nop;
    instruction_table[0x76] = hlt;
    instruction_table[0xC3] = jmp;
    instruction_table[0xCD] = call;
    instruction_table[0xC9] = ret;
    instruction_table[0xC6] = adi;
    instruction_table[0xCE] = aci;
    instruction_table[0xFB] = ei;
    instruction_table[0xF3] = di;
    instruction_table[0xD6] = sui;
    instruction_table[0xDE] = sbi;
    instruction_table[0xFE] = cpi;
    instruction_table[0xE6] = ani;
    instruction_table[0xF6] = ori;
    instruction_table[0xEE] = xri;
    instruction_table[0x0F] = rrc;
    instruction_table[0x07] = rlc;
    instruction_table[0x37] = rar;
    instruction_table[0x21] = ral;
    instruction_table[0x27] = daa;
    instruction_table[0x3A] = lda;
    instruction_table[0x32] = sta;
    instruction_table[0x22] = shld;
    instruction_table[0x2A] = lhld;
    instruction_table[0xEB] = xchg;
    instruction_table[0xF9] = sphl;
    instruction_table[0xE3] = xthl;
}