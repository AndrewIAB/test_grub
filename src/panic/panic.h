#pragma once

#include <stdint.h>

void panic_print_trace();
void panic_print_header(const char* id, uint32_t val, uint32_t code);
