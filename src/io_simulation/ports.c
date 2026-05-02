#include "ports.h"
#include <stdio.h>

char key_buffer[16] = { 0 };
int key_pos = 0;
int read_pos = 0;

void preload_keys(void) {
    key_buffer[key_pos++] = 'H';
    key_buffer[key_pos++] = 'i';
}

uint8_t keyboard_read(void) {
    if (read_pos < key_pos) {
        return key_buffer[read_pos++];
    }
    return 0x00;
}

void monitor_write(uint8_t value) {
    putchar(value);
    putchar('\n');
}
