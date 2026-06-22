#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#include "../debug/debug_print.h"
#include "../panic/panic.h"
#include "../seg_offsets.h"
#include "sysint.h"
#include "intexcep.h"

#define GLUE(A, B) A ## B
#define exception_handler(ID, CODE)\
	__attribute__((naked))\
	static void exception_##ID (void) {\
		if (CODE) {\
			__asm__ volatile (\
					"popl %0"\
					: "=r"(exception_code)\
					:);\
		} else {\
			exception_code = 0;\
		}\
		panic_print_header(#ID, INT_##ID, exception_code);\
		panic_print_trace();\
	}

void* pgmap_alloc();

__attribute__((aligned(0x08)))
idt_descriptor_t sysint_idt_descriptor;
__attribute__((aligned(0x10)))
idt_entry_t sysint_idt_table[SYSINT_IDT_TABLE_LEN];

static volatile uint32_t exception_code;

/*
__attribute__((noreturn))
static void exception_handler(void) {
	debug_print_set_color(0x4F);
	debug_print("\nEXCEPTION!!!");
	HLT();
}
*/

exception_handler(DE, false);
exception_handler(DB, false);
exception_handler(NMI, false);
exception_handler(BP, false);
exception_handler(OF, false);
exception_handler(BR, false);
exception_handler(UD, false);
exception_handler(NM, false);
exception_handler(DF, true);
exception_handler(CSO, false);
exception_handler(TS, true);
exception_handler(NP, true);
exception_handler(SS, true);
exception_handler(GP, true);
exception_handler(PF, true);
exception_handler(RESERVED0, false);
exception_handler(MF, false);
exception_handler(AC, true);
exception_handler(MC, false);
exception_handler(XF, false);
exception_handler(VE, false);
exception_handler(CP, true);
exception_handler(RESERVED1, false);
exception_handler(RESERVED2, false);
exception_handler(RESERVED3, false);
exception_handler(RESERVED4, false);
exception_handler(RESERVED5, false);
exception_handler(RESERVED6, false);
exception_handler(HV, false);
exception_handler(VC, true);
exception_handler(SX, true);
exception_handler(RESERVED7, false);


#define IDT_INT_GATE 0xE
#define IDT_TRAP_GATE 0xF

#define set_exception(ID)\
	sysint_idt_table[INT_##ID] = init_entry(exception_##ID, SEGM_SEL(SEG_R0C, 0), IDT_TRAP_GATE);

static inline idt_entry_t init_entry(void (*func)(void), uint16_t segment_selector, uint8_t gate_type) {
	idt_entry_t entry;
	entry.offset_low = (uint32_t)func & 0xffff;
	entry.offset_high = (uint32_t)func >> 16;
	entry.reserved = 0;
	entry.flags = (gate_type & 15) | ((gate_type & 3) << 5) | (1 << 7);
	entry.segment_selector = segment_selector;
	return entry;
}

void sysint_init_table() {
	sysint_idt_descriptor.offset = (uint32_t)sysint_idt_table;
	sysint_idt_descriptor.size = sizeof(sysint_idt_table);

	set_exception(DE);
	set_exception(DB);
	set_exception(NMI);
	set_exception(BP);
	set_exception(OF);
	set_exception(BR);
	set_exception(UD);
	set_exception(NM);
	set_exception(DF);
	set_exception(CSO);
	set_exception(TS);
	set_exception(NP);
	set_exception(SS);
	set_exception(GP);
	set_exception(PF);
	set_exception(RESERVED0);
	set_exception(MF);
	set_exception(AC);
	set_exception(MC);
	set_exception(XF);
	set_exception(VE);
	set_exception(CP);
	set_exception(RESERVED1);
	set_exception(RESERVED2);
	set_exception(RESERVED3);
	set_exception(RESERVED4);
	set_exception(RESERVED5);
	set_exception(RESERVED6);
	set_exception(HV);
	set_exception(VC);
	set_exception(SX);
	set_exception(RESERVED7);
}
