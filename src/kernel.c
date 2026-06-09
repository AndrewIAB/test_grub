#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "multiboot.h"
#include "video/video.h"
#include "video/bitmap_font.h"
#include "debug_print.h"

#define MAX(X, Y) ((X) < (Y) ? (Y) : (X))
#define MIN(X, Y) ((X) < (Y) ? (X) : (Y))

/* Low level IO */
uint8_t inb(uint16_t);
void outb(uint16_t, uint8_t);
uint16_t inw(uint16_t);
void outw(uint16_t, uint16_t);

/* Generate strings from integers */
const char* struint(unsigned int);
const char* struint_bin(unsigned int, int);
const char* struint_hex(unsigned int, int);

void blit_bitmap(int x, int y, const bitmap_t* bitmap, int scale);
extern video_color_t blit_bitmap_fg;
extern video_color_t blit_bitmap_bg;

void kernel_main(multiboot_info_t* multiboot_info, int magic_number) {
	/* Check that bootloader is multiboot compliant */
	if (magic_number != 0x2BADB002) {
		return;
	}

	video_multiboot_init(multiboot_info);
	bitmap_font_init();
	debug_print_init();

	debug_print_set_color(0xF0);

	/* Disable VGA teletype cursor if in teletype mode */
	if (video_get_params()->mode == VIDEO_MODE_TTY) {
		outb(0x3D4, 0x0A);
		outb(0x3D5, 0x20);
	}

	if ((multiboot_info->flags & MULTIBOOT_INFO_MEM_MAP) == 0) {
		debug_print("No grub memory map!!");
		return;
	}

	multiboot_memory_map_t* mmap_table = (void*)multiboot_info->mmap_addr;
	size_t mmap_table_len = multiboot_info->mmap_length / sizeof(multiboot_memory_map_t);

	for (size_t i = 0; i < mmap_table_len; i++) {
		debug_print(struint_hex(mmap_table[i].addr, 8));
		debug_print("    ");
		debug_print(struint_hex(mmap_table[i].len, 8));
		debug_print("    ");
		debug_print(struint_hex(mmap_table[i].type, 8));
		debug_putchar('\n');
	}

	//bitmap_t* bitmap = &(bitmap_font[10]);
	//if (video_get_params()->mode == VIDEO_MODE_INDEXED)
	//	bitmap++;
	//blit_bitmap(0, 0, bitmap, 1);

	//const video_params_t* params = video_get_params();
}
