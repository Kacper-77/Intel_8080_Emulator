#include "instructions.h"
#include "../decoder/decoder.h"
#include "stdint.h"
#include "../utils/helpers.h"
#include "stddef.h"
#include <stdio.h>

static uint16_t fetch_addr(CPU8080 *cpu) {
    return (cpu->memory[cpu->PC + 2] << 8) | cpu->memory[cpu->PC + 1];
}

static void set_return_addr(uint16_t return_addr, CPU8080 *cpu) {
    cpu->memory[cpu->SP - 1] = (return_addr >> 8) & 0xFF;
    cpu->memory[cpu->SP - 2] = return_addr & 0xFF;
    cpu->SP -= 2;
}

static uint16_t fetch_return_addr(CPU8080 *cpu) {
    return (cpu->memory[cpu->SP + 1] << 8) | cpu->memory[cpu->SP];
}

/////////////////////////////////////////////////////////////////////////

void nop(uint8_t opcode, CPU8080 *cpu) {
    (void)opcode;
    cpu->PC += 1;
}

void adi(uint8_t opcode, CPU8080 *cpu) {
    (void)opcode;
    uint8_t value = cpu->memory[cpu->PC + 1];
    uint16_t result = cpu->A + value;

    update_flags_after_add(result, value, cpu, false);

    cpu->A = result & 0xFF;
    cpu->PC += 2;
}

void aci(uint8_t opcode, CPU8080 *cpu) {
    (void)opcode;
    uint8_t value = cpu->memory[cpu->PC + 1];
    uint16_t result = cpu->A + value + cpu->flags.CY;

    update_flags_after_add(result, value, cpu, true);

    cpu->A = result & 0xFF;
    cpu->PC += 2;
}

void jmp(uint8_t opcode, CPU8080 *cpu) {
    (void)opcode;
    uint16_t addr = fetch_addr(cpu);
    cpu->PC = addr;
}

void hlt(uint8_t opcode, CPU8080 *cpu) {
    custom_cycle = true;
    cpu->cycles += t_states[opcode];
    cpu->halted = true;
}

void call(uint8_t opcode, CPU8080 *cpu) {
    (void)opcode;
    uint16_t addr = fetch_addr(cpu);
    set_return_addr(cpu->PC + 3, cpu);

    cpu->PC = addr;
}

void ret(uint8_t opcode, CPU8080 *cpu) {
    (void)opcode;
    uint8_t low = cpu->memory[cpu->SP];
    uint8_t high = cpu->memory[cpu->SP + 1];
    uint16_t addr = (high << 8) | low;

    cpu->SP += 2;
    cpu->PC = addr;
}

void mov_generic(uint8_t opcode, CPU8080 *cpu) {
    uint8_t dest_code = (opcode >> 3) & 0x07;
    uint8_t src_code  = opcode & 0x07;
    uint16_t addr = get_rpair(cpu->H, cpu->L);
    
    if (dest_code == 6 && src_code != 6) {
        cpu->memory[addr] = *cpu->registers[src_code];
    } else if (dest_code != 6 && src_code == 6) {
        *cpu->registers[dest_code] = cpu->memory[addr];
    } else {
        *cpu->registers[dest_code] = *cpu->registers[src_code];
    }

    cpu->PC += 1;
}

void mvi_generic(uint8_t opcode, CPU8080 *cpu) {
    uint8_t reg_code = (opcode >> 3) & 0x07;
    uint8_t data = cpu->memory[cpu->PC + 1];

    if (reg_code == 6) {
        uint16_t addr = get_rpair(cpu->H, cpu->L);
        cpu->memory[addr] = data;
    } else {
        *cpu->registers[reg_code] = data;
    }

    cpu->PC += 2;
}

void add_generic(uint8_t opcode, CPU8080 *cpu) {
    uint8_t value;
    uint8_t reg_code = opcode & 0x07;
    
    if (reg_code == 6) {
        uint16_t addr = get_rpair(cpu->H, cpu->L);
        value = cpu->memory[addr];
    } else {
        value = *cpu->registers[reg_code];
    }

    uint16_t result = cpu->A + value;
    cpu->A = result & 0xFF;
    update_flags_after_add(result, value, cpu, false);

    cpu->PC += 1;
}

void inr_generic(uint8_t opcode, CPU8080 *cpu) {
    uint8_t result, before;
    uint8_t reg_code = (opcode >> 3) & 0x07;

    if (reg_code == 6) {
        uint16_t addr = get_rpair(cpu->H, cpu->L);
        before = cpu->memory[addr];
        result = before + 1;
        cpu->memory[addr] = result;
    } else {
        before = *cpu->registers[reg_code];
        result = before + 1;
        *cpu->registers[reg_code] = result;
    }
    update_flags_after_inr_dcr(result, before, cpu, true);
    cpu->PC += 1;
}

void dcr_generic(uint8_t opcode, CPU8080 *cpu) {
    uint8_t result, before;
    uint8_t reg_code = (opcode >> 3) & 0x07;

    if (reg_code == 6) {
        uint16_t addr = get_rpair(cpu->H, cpu->L);
        before = cpu->memory[addr];
        result = before - 1;
        cpu->memory[addr] = result;
    } else {
        before = *cpu->registers[reg_code];
        result = before - 1;
        *cpu->registers[reg_code] = result;
    }
    update_flags_after_inr_dcr(result, before, cpu, false);
    cpu->PC += 1;
}

void lxi_generic(uint8_t opcode, CPU8080 *cpu) {
    uint16_t value = fetch_addr(cpu);

    switch (opcode) {
        case 0x01:
            cpu->B = value >> 8;
            cpu->C = value & 0xFF;
            break;
        case 0x11:
            cpu->D = value >> 8;
            cpu->E = value & 0xFF;
            break;
        case 0x21:
            cpu->H = value >> 8;
            cpu->L = value & 0xFF;
            break;
        case 0x31:
            cpu->SP = value;
            break;
    }
    cpu->PC += 3;
}

void sub_generic(uint8_t opcode, CPU8080 *cpu) {
    uint8_t reg_code = opcode & 0x07;
    uint8_t value;

    if (reg_code == 6) {
        uint16_t addr = get_rpair(cpu->H, cpu->L);
        value = cpu->memory[addr];
        custom_cycle = true;
        cpu->cycles += 7;
    } else {
        value = *cpu->registers[reg_code];
    }

    uint8_t result = cpu->A - value;
    update_flags_after_sub(cpu->A, value, 0,cpu);
    cpu->A = result;
    cpu->PC += 1;
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

void rst_generic(uint8_t opcode, CPU8080 *cpu) {
    uint16_t return_addr = cpu->PC + (cpu->is_interrupt ? 0 : 1);
    set_return_addr(return_addr, cpu);

    uint8_t value = (opcode >> 3) & 0x07;
    cpu->PC = value * 8;
}

void push_generic(uint8_t opcode, CPU8080 *cpu) {
    uint8_t flags_byte =
    (cpu->flags.S  << 7) |
    (cpu->flags.Z  << 6) |
    (cpu->flags.AC << 4) |
    (cpu->flags.P  << 2) |
    (1 << 1) |
    (cpu->flags.CY << 0);

    switch (opcode) {
        case 0xC5:
            cpu->memory[cpu->SP - 1] = cpu->B;
            cpu->memory[cpu->SP - 2] = cpu->C;
            break;
        case 0xD5:
            cpu->memory[cpu->SP - 1] = cpu->D;
            cpu->memory[cpu->SP - 2] = cpu->E;
            break;
        case 0xE5:
            cpu->memory[cpu->SP - 1] = cpu->H;
            cpu->memory[cpu->SP - 2] = cpu->L;
            break;
        case 0xF5:
            cpu->memory[cpu->SP - 1] = cpu->A;
            cpu->memory[cpu->SP - 2] = flags_byte;
            break;
    }
    cpu->SP -= 2;
    cpu->PC += 1;
}

void pop_generic(uint8_t opcode, CPU8080 *cpu) {
    uint8_t low = cpu->memory[cpu->SP];
    uint8_t high = cpu->memory[cpu->SP + 1];
    cpu->SP += 2;

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

void sui(uint8_t opcode, CPU8080 *cpu) {
    (void)opcode;
    uint8_t value = cpu->memory[cpu->PC + 1];

    uint8_t result = cpu->A - value;
    update_flags_after_sub(cpu->A, value, 0, cpu);
    
    cpu->A = result;
    cpu->PC += 2;
}

void sbi(uint8_t opcode, CPU8080 *cpu) {
    (void)opcode;
    uint8_t value = cpu->memory[cpu->PC + 1];

    uint8_t result = cpu->A - value - cpu->flags.CY;
    update_flags_after_sub(cpu->A, value, cpu->flags.CY, cpu);

    cpu->A = result;
    cpu->PC += 2;
}

void cmp_generic(uint8_t opcode, CPU8080 *cpu) {
    uint8_t reg_code = opcode & 0x07;
    uint8_t value = (reg_code == 6)
                    ? cpu->memory[get_rpair(cpu->H, cpu->L)]
                    : *cpu->registers[reg_code];
    
    update_flags_after_sub(cpu->A, value, 0, cpu);
    cpu->PC += 1;
}

void cpi(uint8_t opcode, CPU8080 *cpu) {
    (void)opcode;
    uint8_t value = cpu->memory[cpu->PC + 1];

    update_flags_after_sub(cpu->A, value, 0, cpu);
    cpu->PC += 2;
}

void dad_generic(uint8_t opcode, CPU8080 *cpu) {
    uint16_t HL = get_rpair(cpu->H, cpu->L);
    uint16_t value;

    switch (opcode) {
        case 0x09: value = (cpu->B << 8) | cpu->C; break;
        case 0x19: value = (cpu->D << 8) | cpu->E; break;
        case 0x29: value = HL; break;
        case 0x39: value = cpu->SP; break;
    }

    uint32_t result = HL + value;
    cpu->H = (result >> 8) & 0xFF;
    cpu->L = result & 0xFF;
    cpu->flags.CY = (result > 0xFFFF);
    cpu->PC += 1;
}

void inx_generic(uint8_t opcode, CPU8080 *cpu) {
    uint16_t value;

    switch (opcode) {
        case 0x03:
            value = get_rpair(cpu->B, cpu->C);
            value++;
            set_rpair(value, &cpu->B, &cpu->C);
            break;
        case 0x13:
            value = get_rpair(cpu->D, cpu->E);
            value++;
            set_rpair(value, &cpu->D, &cpu->E);
            break;
        case 0x23:
            value = get_rpair(cpu->H, cpu->L);
            value++;
            set_rpair(value, &cpu->H, &cpu->L);
            break;
        case 0x33:
            cpu->SP++;
            break;
    }
    cpu->PC += 1;
}

void dcx_generic(uint8_t opcode, CPU8080 *cpu) {
    uint16_t value;

    switch (opcode) {
        case 0x0B:
            value = get_rpair(cpu->B, cpu->C);
            value--;
            set_rpair(value, &cpu->B, &cpu->C);
            break;
        case 0x1B:
            value = get_rpair(cpu->D, cpu->E);
            value--;
            set_rpair(value, &cpu->D, &cpu->E);
            break;
        case 0x2B:
            value = get_rpair(cpu->H, cpu->L);
            value--;
            set_rpair(value, &cpu->H, &cpu->L);
            break;
        case 0x3B:
            cpu->SP--;
            break;
    }
    cpu->PC += 1;
}

void ana_generic(uint8_t opcode, CPU8080 *cpu) {
    uint8_t reg_code = opcode & 0x07;
    uint8_t value;
    
    if (reg_code == 6) {
        uint16_t addr = get_rpair(cpu->H, cpu->L);
        value = cpu->memory[addr];
    } else {
        value = *cpu->registers[reg_code];
    }

    uint16_t result = cpu->A & value;
    cpu->A = result & 0xFF;
    update_flags_logical(result, cpu, false);
    cpu->PC += 1;
}

void ani(uint8_t opcode, CPU8080 *cpu) {
    (void)opcode;
    uint8_t value = cpu->memory[cpu->PC + 1];

    uint16_t result = cpu->A & value;
    cpu->A = result & 0xFF;
    update_flags_logical(result, cpu, true);
    cpu->PC += 2;
}

void ora_generic(uint8_t opcode, CPU8080 *cpu) {
    uint8_t reg_code = opcode & 0x07;
    uint8_t value;

    if (reg_code == 6) {
        uint16_t addr = get_rpair(cpu->H, cpu->L);
        value = cpu->memory[addr];
    } else {
        value = *cpu->registers[reg_code];
    }

    uint16_t result = cpu->A | value;
    cpu->A = result & 0xFF;
    update_flags_logical(result, cpu, true);
    cpu->PC += 1;
}

void ori(uint8_t opcode, CPU8080 *cpu) {
    (void)opcode;
    uint8_t value = cpu->memory[cpu->PC + 1];

    uint16_t result = cpu->A | value;
    cpu->A = result & 0xFF;
    update_flags_logical(result, cpu, true);
    cpu->PC += 2;
}

void xra_generic(uint8_t opcode, CPU8080 *cpu) {
    uint8_t reg_code = opcode & 0x07;
    uint8_t value;

    if (reg_code == 6) {
        uint16_t addr = get_rpair(cpu->H, cpu->L);
        value = cpu->memory[addr];
    } else {
        value = *cpu->registers[reg_code];
    }

    uint16_t result = cpu->A ^ value;
    cpu->A = result & 0xFF;
    update_flags_logical(result, cpu, true);
    cpu->PC += 1;
}

void xri(uint8_t opcode, CPU8080 *cpu) {
    (void)opcode;
    uint8_t value = cpu->memory[cpu->PC + 1];

    uint16_t result = cpu->A ^ value;
    cpu->A = result & 0xFF;
    update_flags_logical(result, cpu, true);
    cpu->PC += 2;
}

void rrc(uint8_t opcode, CPU8080 *cpu) {
    (void)opcode;
    uint8_t lsb = cpu->A & 1;

    cpu->A = (cpu->A >> 1) | (lsb << 7);
    cpu->flags.CY = lsb;

    cpu->PC += 1;
}

void rlc(uint8_t opcode, CPU8080 *cpu) {
    (void)opcode;
    uint8_t msb = (cpu->A & 0x80) >> 7;

    cpu->A = (cpu->A << 1) | msb;
    cpu->flags.CY = msb;

    cpu->PC += 1;
}

void rar(uint8_t opcode, CPU8080 *cpu) {
    (void)opcode;
    uint8_t old_cy = cpu->flags.CY;
    uint8_t lsb = cpu->A & 1;

    cpu->A = (cpu->A >> 1) | (old_cy << 7);
    cpu->flags.CY = lsb;

    cpu->PC += 1;
}

void ral(uint8_t opcode, CPU8080 *cpu) {
    (void)opcode;
    uint8_t old_cy = cpu->flags.CY;
    uint8_t msb = (cpu->A & 0x80) >> 7;

    cpu->A = (cpu->A << 1) | old_cy;
    cpu->flags.CY = msb;

    cpu->PC += 1;
}

void daa(uint8_t opcode, CPU8080 *cpu) {
    (void)opcode;
    uint8_t correction = 0;

    if ((cpu->A & 0x0F) > 9 || cpu->flags.AC) {
        correction = 0x06;
    } 
    if ((cpu->A >> 4) > 9 || cpu->flags.CY || (cpu->A + correction > 0x99)) {
        correction = 0x60;
    }
    uint16_t result = cpu->A + correction;
    update_flags_after_add(result, correction, cpu, false);

    cpu->A = result & 0xFF;
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
        custom_cycle = true;
        cpu->cycles += 7;
        cpu->PC = cpu->PC + 3;
    }
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
        cpu->SP += 2;
        cpu->PC = addr;
    } else {
        custom_cycle = true;
        cpu->cycles += 5;
        cpu->PC += 1;
    }
}

void lda(uint8_t opcode, CPU8080 *cpu) {
    (void)opcode;
    uint16_t addr = fetch_addr(cpu);
    cpu->A = cpu->memory[addr];

    cpu->PC += 3;
}

void sta(uint8_t opcode, CPU8080 *cpu) {
    (void)opcode;
    uint16_t addr = fetch_addr(cpu);
    cpu->memory[addr] = cpu->A;
    
    cpu->PC += 3;
}

void shld(uint8_t opcode, CPU8080 *cpu) {
    (void)opcode;
    uint16_t addr = fetch_addr(cpu);
    cpu->memory[addr] = cpu->L;
    cpu->memory[addr + 1] = cpu->H;

    cpu->PC += 3;
}

void lhld(uint8_t opcode, CPU8080 *cpu) {
    (void)opcode;
    uint16_t addr = fetch_addr(cpu);
    cpu->L = cpu->memory[addr];
    cpu->H = cpu->memory[addr + 1];

    cpu->PC += 3;
}

void ldax_generic(uint8_t opcode, CPU8080 *cpu) {
    uint16_t addr;
    switch (opcode)
    {
    case 0x0A:
        addr = get_rpair(cpu->B, cpu->C);
        cpu->A = cpu->memory[addr];
        break;
    case 0x1A:
        addr = get_rpair(cpu->D, cpu->E);
        cpu->A = cpu->memory[addr];
        break;
    }
    cpu->PC += 1;
}

void stax_generic(uint8_t opcode, CPU8080 *cpu) {
    switch (opcode) {
        case 0x02:
            cpu->memory[get_rpair(cpu->B, cpu->C)] = cpu->A;
            break;
        case 0x12:
            cpu->memory[get_rpair(cpu->D, cpu->E)] = cpu->A;
            break;
    }
    cpu->PC += 1;
}