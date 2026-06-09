#include <stdint.h>
#include <stddef.h>

#include "video/video.h"
#include "video/bitmap_font.h"
#include "debug_print.h"

static const uint8_t VGA_COLORS[16][3] = {
	{0x00, 0x00, 0x00},
	{0x00, 0x00, 0xAA},
	{0x00, 0xAA, 0x00},
	{0x00, 0xAA, 0xAA},
	{0xAA, 0x00, 0x00},
	{0xAA, 0x00, 0xAA},
	{0xAA, 0x55, 0x00},
	{0xAA, 0xAA, 0xAA},
	{0x55, 0x55, 0x55},
	{0x55, 0x55, 0xFF},
	{0x55, 0xFF, 0x55},
	{0x55, 0xFF, 0xFF},
	{0xFF, 0x55, 0x55},
	{0xFF, 0x55, 0xFF},
	{0xFF, 0xFF, 0x55},
	{0xFF, 0xFF, 0xFF}
};

void* memset (void*, register int, register size_t);
void* memcpy(void*, const void*, size_t);

void blit_bitmap(int x, int y, const bitmap_t* bitmap, int scale);
extern video_color_t blit_bitmap_fg;
extern video_color_t blit_bitmap_bg;

size_t debug_print_columns;
size_t debug_print_rows;
size_t debug_print_index;

uint8_t debug_print_tty_style;
video_color_t debug_print_bmp_fg;
video_color_t debug_print_bmp_bg;

video_color_t debug_print_bmp_colors[16];

//int debug_print_bmp_spacing = 2;
int debug_print_bmp_margin = 2;

void (*debug_putchar)(char);
void (*debug_print)(const char*);

void debug_putchar_tty(char);
void debug_print_tty(const char*);
void debug_putchar_bmp(char);
void debug_print_bmp(const char*);

void debug_print_init() {
	const video_params_t* params = video_get_params();
	debug_print_index = 0;

	if (params->mode == VIDEO_MODE_TTY) {
		debug_print_columns = params->width;
		debug_print_rows = params->height;

		debug_putchar = debug_putchar_tty;
		debug_print = debug_print_tty;
	}
	else {
		debug_print_columns = params->width / (bitmap_font_width);
		debug_print_rows = params->height / bitmap_font_height;

		debug_putchar = debug_putchar_bmp;
		debug_print = debug_print_bmp;

		for (int i = 0; i < 16; i++);
	}

	debug_print_set_color(0x0F);
}

void debug_print_set_color(uint8_t color) {
	if (debug_putchar == debug_putchar_tty) {
		debug_print_tty_style = color;
	}
	else {
		const uint8_t* fg = VGA_COLORS[color & 0xF];
		const uint8_t* bg = VGA_COLORS[color >> 4];
		debug_print_bmp_fg = get_color(fg[0], fg[1], fg[2]);
		debug_print_bmp_bg = get_color(bg[0], bg[1], bg[2]);
	}
}

void debug_print_scroll_tty() {
	const video_params_t* params = video_get_params();
	memcpy(
			params->framebuffer,
			(char*)params->framebuffer + params->pitch,
			params->pitch * (params->height - 1));
	memset(
			(char*)params->framebuffer + (params->height - 1) * params->pitch,
			0,
			params->pitch);
	debug_print_index -= debug_print_columns;
}

void debug_ins_tty(char c) {
	const video_params_t* params = video_get_params();
	((uint16_t*)params->framebuffer)[debug_print_index] = (debug_print_tty_style << 8) | c;
}

void debug_putchar_tty(char c) {
	switch (c) {
		case '\n':
			debug_print_index -= debug_print_index % debug_print_columns;
			debug_print_index += debug_print_columns;
			break;
		case '\b':
			if (debug_print_index > 0) {
				debug_print_index --;
			}
			debug_ins_tty(' ');
			break;
		default:
			debug_ins_tty(c);
			debug_print_index ++;
			break;
	}

	if (debug_print_index >= debug_print_columns * debug_print_rows) {
		debug_print_scroll_tty();
	}
}

void debug_print_tty(const char* str) {
	while (*str != 0) {
		debug_putchar_tty(*str);
		str++;
	}
}

void debug_print_scroll_bmp() {
	const video_params_t* params = video_get_params();
	memcpy(
			params->framebuffer,
			(char*)params->framebuffer + params->pitch * bitmap_font_height,
			params->pitch * (params->height - bitmap_font_height));
	memset(
			(char*)params->framebuffer + (params->height - bitmap_font_height) * params->pitch,
			0,
			params->pitch * bitmap_font_height);
	debug_print_index -= debug_print_columns;
}

void debug_ins_bmp(char c) {
	int x = (debug_print_index % debug_print_columns) * (bitmap_font_width) + debug_print_bmp_margin;
	int y = (debug_print_index / debug_print_columns) * bitmap_font_height + debug_print_bmp_margin;
	blit_bitmap_fg = debug_print_bmp_fg;
	blit_bitmap_bg = debug_print_bmp_bg;
	blit_bitmap(x, y, &(bitmap_font[c]), 1);
}

void debug_putchar_bmp(char c) {
	switch (c) {
		case '\n':
			debug_print_index -= debug_print_index % debug_print_columns;
			debug_print_index += debug_print_columns;
			break;
		case '\b':
			if (debug_print_index > 0) {
				debug_print_index --;
			}
			debug_ins_bmp(' ');
			break;
		default:
			debug_ins_bmp(c);
			debug_print_index ++;
			break;
	}

	if (debug_print_index >= debug_print_columns * debug_print_rows) {
		debug_print_scroll_bmp();
	}
}

void debug_print_bmp(const char* str) {
	while (*str != 0) {
		debug_putchar_bmp(*str);
		str++;
	}
}

