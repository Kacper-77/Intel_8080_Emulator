#pragma once
#include "stdint.h"

char key_buffer[16];
int key_pos;
int read_pos;
void preload_keys(void);
uint8_t keyboard_read(void);

void monitor_write(uint8_t value);