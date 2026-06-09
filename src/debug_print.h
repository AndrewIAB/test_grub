#pragma once

#include <stdint.h>

void debug_print_init();
void debug_print_set_color(uint8_t);
extern void (*debug_putchar)(char);
extern void (*debug_print)(const char*);
