#include "cpu/cpu.h"
#include "decoder/decoder.h"
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>

#define MAX_CYCLES_PER_TEST 1000000ULL

typedef void (*RunCallback)(CPU8080 *cpu, uint64_t cycles);

static bool run_until_halt_or_limit(CPU8080 *cpu, uint64_t max_cycles_delta, RunCallback cb) {
    uint64_t start = cpu->cycles;
    while (cpu_emulate(cpu)) {
        if (cb) cb(cpu, cpu->cycles - start);
        if ((cpu->cycles - start) > max_cycles_delta) {
            printf("❌ Test timeout: exceeded %llu cycles\n", (unsigned long long)max_cycles_delta);
            return false;
        }
    }
    return true;
}

static void dump_cpu_state(CPU8080 *cpu) {
    printf("PC=0x%04X SP=0x%04X A=0x%02X B=0x%02X C=0x%02X D=0x%02X E=0x%02X H=0x%02X L=0x%02X\n",
           cpu->PC, cpu->SP, cpu->A, cpu->B, cpu->C, cpu->D, cpu->E, cpu->H, cpu->L);
    printf("Flags: Z=%u S=%u P=%u CY=%u AC=%u\n",
           cpu->flags.Z, cpu->flags.S, cpu->flags.P, cpu->flags.CY, cpu->flags.AC);
    printf("Cycles: %llu\n", cpu->cycles);
}

typedef bool (*CheckFn)(CPU8080 *cpu);

static bool run_test(const char *name,
                     const uint8_t *program, size_t prog_size,
                     const uint8_t *install_at, size_t install_size, uint16_t install_addr,
                     RunCallback cb, CheckFn check) {
    CPU8080 cpu;
    cpu_init(&cpu);
    init_instruction_table();

    cpu_load_program(&cpu, program, prog_size, 0x0000);

    if (install_at && install_size > 0) {
        if (install_addr + install_size <= 0x10000) {
            memcpy(&cpu.memory[install_addr], install_at, install_size);
        } else {
            printf("❌ install region out of bounds\n");
            return false;
        }
    }

    bool ran = run_until_halt_or_limit(&cpu, MAX_CYCLES_PER_TEST, cb);
    if (!ran) {
        printf("[%s] ❌ runtime error (timeout)\n", name);
        dump_cpu_state(&cpu);
        return false;
    }

    bool pass = check(&cpu);
    if (pass) {
        printf("[%s] ✅ PASS\n", name);
    } else {
        printf("[%s] ❌ FAIL\n", name);
        dump_cpu_state(&cpu);
    }
    return pass;
}

/* -----------------------
   Concrete checks & progs
   ----------------------- */

// 1) ADD test
static bool check_add(CPU8080 *cpu) {
    return cpu->A == 0x0F && cpu->B == 0x0A;
}
static const uint8_t prog_add[] = {
    0x3E, 0x05, // MVI A,0x05
    0x06, 0x0A, // MVI B,0x0A
    0x80,       // ADD B
    0x76        // HLT
};

// 2) LXI + DAD
static bool check_lxi_dad(CPU8080 *cpu) {
    uint16_t hl = ((uint16_t)cpu->H << 8) | cpu->L;
    return hl == 0x2345;
}
static const uint8_t prog_lxi_dad[] = {
    0x21, 0x34, 0x12, // LXI H,0x1234
    0x11, 0x11, 0x11, // LXI D,0x1111
    0x19,             // DAD D
    0x76              // HLT
};

// 3) CALL/RET
static bool check_call_ret(CPU8080 *cpu) {
    return cpu->A == 0x42;
}
static const uint8_t prog_call_ret[] = {
    0xCD, 0x06, 0x00, // CALL 0x0006
    0x76,             // HLT
    0x00, 0x00,       // filler
    0x3E, 0x42, 0xC9  // at 0x0006: MVI A,0x42; RET
};

// 4) INR
static bool check_inr(CPU8080 *cpu) {
    return cpu->A == 0x00 && cpu->flags.Z == 1;
}
static const uint8_t prog_inr[] = {
    0x3E, 0xFF, // MVI A,0xFF
    0x3C,       // INR A
    0x76        // HLT
};

// 5) Interrupt (EI + RST1)
static bool check_interrupt(CPU8080 *cpu) {
    return cpu->A == 0xEE;
}
static const uint8_t prog_interrupt[] = {
    0xFB, // EI
    0x00, // NOP
    0x00, // NOP
    0x76  // HLT
};
static const uint8_t interrupt_handler_area[] = {
    0x3E, 0xEE, 0xC9 // at 0x0008: MVI A,0xEE; RET
};
static void cb_interrupt(CPU8080 *cpu, uint64_t cycles) {
    // Robust triggering: request when interrupt enabled OR when sufficient cycles passed
    // so we don't miss it due to t-state jumps. Only request once.
    static bool requested = false;
    const uint64_t FORCE_AFTER = 20; // if interrupt_enable never set, force after this many cycles
    if (requested) return;

    // If CPU implementation sets interrupt_enabled true only after EI semantics,
    // we prefer to wait for that. If not set in reasonable time, force it.
    if (cpu->interrupt_enabled) {
        request_interrupt(cpu, 0xCF); // RST 1
        requested = true;
        printf("\n[cb_interrupt] requested interrupt (interrupt_enabled==true)\n");
        return;
    }
    if (cycles >= FORCE_AFTER) {
        request_interrupt(cpu, 0xCF);
        requested = true;
        printf("\n[cb_interrupt] forced interrupt after %llu cycles\n", (unsigned long long)cycles);
    }
}

// 6) Trap (trigger_trap -> 0x24)
static bool check_trap(CPU8080 *cpu) {
    return cpu->A == 0x99;
}
static const uint8_t prog_trap[] = {
    0x3E, 0x01, // MVI A,0x01
    0x00,       // NOP
    0x00,       // NOP
    0x76        // HLT
};
static const uint8_t trap_handler_area[] = {
    0x3E, 0x99, 0xC9 // at 0x0024: MVI A,0x99; RET
};
static void cb_trap(CPU8080 *cpu, uint64_t cycles) {
    // Fire trap once when cycles pass threshold (>=) to avoid skipping due to t-state jumps.
    static bool triggered = false;
    const uint64_t TRAP_AFTER = 12;
    if (triggered) return;
    if (cycles >= TRAP_AFTER) {
        trigger_trap(cpu);
        triggered = true;
        printf("\n[cb_trap] triggered trap at cycles=%llu\n", (unsigned long long)cycles);
    }
}

/* Main: run tests */
int main(void) {
    printf("=== 8080 Emulator Self-test Harness (robust) ===\n");

    int total = 0, passed = 0;

    total++;
    if (run_test("ADD (MVI/ADD)", prog_add, sizeof(prog_add),
                 NULL, 0, 0, NULL, check_add)) passed++;

    total++;
    if (run_test("LXI + DAD", prog_lxi_dad, sizeof(prog_lxi_dad),
                 NULL, 0, 0, NULL, check_lxi_dad)) passed++;

    total++;
    if (run_test("CALL / RET", prog_call_ret, sizeof(prog_call_ret),
                 NULL, 0, 0, NULL, check_call_ret)) passed++;

    total++;
    if (run_test("INR (overflow->zero)", prog_inr, sizeof(prog_inr),
                 NULL, 0, 0, NULL, check_inr)) passed++;

    total++;
    if (run_test("Interrupt (EI + RST1)", prog_interrupt, sizeof(prog_interrupt),
                 interrupt_handler_area, sizeof(interrupt_handler_area), 0x0008,
                 cb_interrupt, check_interrupt)) passed++;

    total++;
    if (run_test("Trap (trigger_trap -> 0x24)", prog_trap, sizeof(prog_trap),
                 trap_handler_area, sizeof(trap_handler_area), 0x0024,
                 cb_trap, check_trap)) passed++;

    printf("\n=== Summary: %d/%d tests passed ===\n", passed, total);
    if (passed == total) {
        printf("All tests PASSED — emulator basic functionality looks good.\n");
    } else {
        printf("Some tests FAILED — inspect failing tests output above.\n");
    }

    return (passed == total) ? 0 : 1;
}
