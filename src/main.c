#include "cpu/cpu.h"
#include "decoder/decoder.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define PROGRAM_FILE "../test/CPUTEST.COM"
#define LOAD_ADDR 0x0100

int main(void) {
    FILE *file = fopen(PROGRAM_FILE, "rb");
    if (!file) {
        perror("Cannot open CPUTEST.COM");
        return 1;
    }

    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    rewind(file);

    uint8_t *buffer = malloc(size);
    if (!buffer) {
        fprintf(stderr, "No memory!\n");
        fclose(file);
        return 1;
    }
    fread(buffer, 1, size, file);
    fclose(file);

    CPU8080 cpu;
    cpu_init(&cpu);
    init_instruction_table();

    cpu_load_program(&cpu, buffer, (uint16_t)size, LOAD_ADDR);
    free(buffer);

    printf("=== 8080 Emulator ===\n");
    printf("Loaded %ld bytes, addr: 0x%04X\n", size, LOAD_ADDR);
    printf("Starting program...\n\n");

    while (!cpu.halted) {
        cpu_emulate(&cpu);
    }

    printf("\n=== End of emulation ===\n");
    printf("Cycles: %llu\n", cpu.cycles);

    return 0;
}
