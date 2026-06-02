#include <stdint.h>

#define TTY_WIDTH 80
#define TTY_HEIGHT 25

uint8_t vga_print_color = 0x0F;

static char* VGA_START = (void*)0xB8000;

static unsigned int cursor = 0;

void v_inschar(char c) {
	VGA_START[cursor * 2 + 1] = vga_print_color;
	VGA_START[cursor * 2 + 0] = c;
}

void v_putchar(char c) {
	switch (c) {
		case '\n':
			cursor -= cursor % TTY_WIDTH;
			cursor += TTY_WIDTH;
			break;
		case '\b':
			cursor --;
			v_inschar(0);
			break;
		default:
			v_inschar(c);
			cursor ++;
			break;
	}
}

void print(const char* text) {
	while (*text != '\0'){
		v_putchar(*text);
		text ++;
	}
}
