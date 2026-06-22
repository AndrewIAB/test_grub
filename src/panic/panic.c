#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#include "debug_print.h"

#define GRAB_REG(REG, VAR)\
	__asm__ volatile (\
			"mov %%"#REG", %0"\
			: "=r"(VAR)\
			:);

#define SPACES_2 "  "
#define SPACES_4 SPACES_2 SPACES_2
#define SPACES_8 SPACES_4 SPACES_4
#define SPACES_16 SPACES_8 SPACES_8
#define SPACES_32 SPACES_16 SPACES_16
#define _GLUE(A, B) A ## B
#define _EVAL(A, B) _GLUE(A, B)
#define GET_SPACES(N) _EVAL(SPACES_, N)

#define PRINTVALL(TEXT, VAL, LEN)\
	debug_print(TEXT " ");\
	debug_print(struint_hex(VAL, LEN));\
	debug_putchar('\n');
#define PRINTVALR(TEXT, VAL, LEN)\
	eol_print(TEXT " " GET_SPACES(LEN));\
	debug_print_index -= LEN;\
	debug_print(struint_hex(VAL, LEN));

#define PRINTVALC(TEXT, VAL, LEN)\
	debug_print_index += debug_print_columns / 2 - 16;\
	debug_print(TEXT " ");\
	debug_print(struint_hex(VAL, LEN));\
	debug_putchar('\n');

#define PRINTVALC_BIN(TEXT, VAL, LEN)\
	debug_print_index += debug_print_columns / 2 - 16;\
	debug_print(TEXT " ");\
	debug_print(struint_bin(VAL, LEN));\
	debug_putchar('\n');

#define PRINTTRACE(TEXT, I, VAL, LEN)\
	eol_print(TEXT "   : " GET_SPACES(LEN));\
	debug_print_index -= LEN + 5;\
	debug_print(struint_hex(I, 2));\
	debug_print_index += 3;\
	debug_print(struint_hex(VAL, LEN));


extern const char PANIC_ASCII_ART[];

extern size_t debug_print_index;
extern size_t debug_print_columns;
extern size_t debug_print_rows;

/* Generate strings from integers */
const char* struint(unsigned int);
const char* struint_bin(unsigned int, int);
const char* struint_hex(unsigned int, int);

size_t strlen(const char*);

static void eol_print(const char* str) {
	size_t len = strlen(str);
	debug_print_index -= debug_print_index % debug_print_columns;
	debug_print_index += debug_print_columns - len;
	debug_print(str);
}

static volatile uint32_t
	p_eax, p_ebx, p_ecx, p_edx,
	p_esp, p_ebp,
	p_cr0, p_cr2, p_cr3, p_cr4,
	p_cs, p_ds, p_ss, p_es, p_fs, p_gs;
static volatile uint32_t p_stack[16];

static void grab_registers() {
	GRAB_REG(eax, p_eax);
	GRAB_REG(ebx, p_ebx);
	GRAB_REG(ecx, p_ecx);
	GRAB_REG(edx, p_edx);
	GRAB_REG(esp, p_esp);
	GRAB_REG(ebp, p_ebp);
	GRAB_REG(cr0, p_cr0);
	GRAB_REG(cr2, p_cr2);
	GRAB_REG(cr3, p_cr3);
	GRAB_REG(cr4, p_cr4);
	GRAB_REG(cs, p_cs);
	GRAB_REG(ds, p_ds);
	GRAB_REG(ss, p_ss);
	GRAB_REG(es, p_es);
	GRAB_REG(fs, p_fs);
	GRAB_REG(gs, p_gs);

	for (size_t i = 0; i < 16; i++) {
		p_stack[i] = *((uint32_t*)p_esp + i);
	}
}

static void print_trace_variables() {
	debug_print_set_color(0x4F);

	debug_print_index = debug_print_columns;

	PRINTVALL("EAX", p_eax, 8);
	PRINTVALL("EBX", p_ebx, 8);
	PRINTVALL("ECX", p_ecx, 8);
	PRINTVALL("EDX", p_edx, 8);
	PRINTVALL("ESP", p_esp, 8);
	PRINTVALL("EBP", p_ebp, 8);
	PRINTVALL("BFA", (uint32_t)__builtin_frame_address(0), 8);

	debug_print_index = debug_print_columns;

	PRINTVALC_BIN("CR0", p_cr0, 32);
	PRINTVALC_BIN("CR2", p_cr2, 32);
	PRINTVALC_BIN("CR3", p_cr3, 32);
	PRINTVALC_BIN("CR4", p_cr4, 32);
	PRINTVALC("PAGE DIRECTORY", p_cr3 >> 5, 8);

	debug_print_index = debug_print_columns;

	PRINTVALR("CS", p_cs, 4);
	PRINTVALR("DS", p_ds, 4);
	PRINTVALR("SS", p_ss, 4);
	PRINTVALR("ES", p_es, 4);
	PRINTVALR("FS", p_fs, 4);
	PRINTVALR("GS", p_gs, 4);
}

/* Naked attribute only works if you jump into this function */
__attribute__((naked))
void panic_print_trace() {
	grab_registers();

	debug_print_set_color(0x4F);
	debug_print_index = debug_print_columns;
	
	/* Clear screen */
	for (size_t i = 0; i < debug_print_columns * (debug_print_rows - 1); i++) {
		debug_putchar(' ');
	}

	debug_print_set_color(0x3F);
	debug_print_index = debug_print_columns * debug_print_rows - debug_print_columns * 16;
	debug_print(PANIC_ASCII_ART);

	print_trace_variables();

	debug_print_index = debug_print_columns * 8;

	for (size_t i = 0; i < 16; i++) {
		PRINTTRACE("TRACE STACK : SP+", i * 4, p_stack[i] , 8);
	}

	/* TODO: READ DEBUGGER/DWARF INFO */

	debug_print_index = debug_print_columns * 8;

	debug_print("An error has occurred!! Please do not PANIC.\nEnjoy the following diagnostic info.\nHere is a cat to settle your nerves.");

	/* Stop execution */
	__asm__ volatile("cli; hlt"); while(1) {}
	__builtin_unreachable();
}

void panic_print_header(const char* id, uint32_t val, uint32_t code) {
	debug_print_index = 0;

	debug_print_set_color(0x3F);
	debug_print("EXCEPTION: #");
	debug_print(id);

	debug_print(" [");
	debug_print(struint_hex(val, 2));
	debug_print("] (");

	if (code == 0) {
		debug_print("NULL");
	} else {
		debug_print(struint_hex(code, 8));
	}
	debug_putchar(')');
}
