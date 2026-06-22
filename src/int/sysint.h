#pragma once

#include "idt.h"

#define SYSINT_IDT_TABLE_LEN 32

extern idt_descriptor_t sysint_idt_descriptor;
extern idt_entry_t sysint_idt_table[SYSINT_IDT_TABLE_LEN];

void sysint_init_table();
