#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "multiboot.h"
#include "video/video.h"
#include "video/bitmap_font.h"

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

void kernel_main(struct multiboot_info* multiboot_info, int magic_number) {
	/* Check that bootloader is multiboot compliant */
	if (magic_number != 0x2BADB002) {
		return;
	}

	video_multiboot_init(multiboot_info);
	bitmap_font_init();

	/* Disable VGA teletype cursor if in teletype mode */
	if (video_get_params()->mode == VIDEO_MODE_TTY) {
		outb(0x3D4, 0x0A);
		outb(0x3D5, 0x20);
	}
}
