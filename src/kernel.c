#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "multiboot.h"
#include "video/video.h"
#include "video/bitmap_font.h"

#define MAX(X, Y) ((X) < (Y) ? (Y) : (X))
#define MIN(X, Y) ((X) < (Y) ? (X) : (Y))

uint8_t inb(uint16_t);
void outb(uint16_t, uint8_t);

void v_putchar(char);
void print(const char*);

const char* struint(unsigned int);
const char* struint_bin(unsigned int, int);
const char* struint_hex(unsigned int, int);

void* memcpy(void* dest, const void* src, size_t len) {
	char* d = dest;
	const char* s = src;
	while (len--)
		*d++ = *s++;
	return dest;
}

void* memset (void *dest, register int val, register size_t len) {
  register unsigned char *ptr = (unsigned char*)dest;
  while (len-- > 0)
    *ptr++ = val;
  return dest;
}


/* TODO:
 * Add foreground and background color pointer, memcpy color into framebuffer.
 * Also add pixel blit to perform bounds check per pixel.
 * */
/*
void put_bitmap(int x, int y, const unsigned char* src, int len, int width, int scale) {
	const video_params_t* params = video_get_params();

	const int x_i = x;

	int bits = len * 8;
	bits -= bits % width;
	src += len - 1;
	unsigned char mask = *src;
	while (bits && y <= params->height) {
		void* framebuffer = params->framebuffer;
		framebuffer += (size_t)(x * params->depth + y * params->pitch);

		unsigned int color = (mask & 128) ? (~0) : (0xff0000);

		int rem_pixels_y = params->height - y;
		int scale_pixels_y = MIN(rem_pixels_y, scale);
			
		int rem_pixels_x = params->width - x;
		int scale_pixels_x = MIN(rem_pixels_x, scale);

		for (int i = 0; i < scale_pixels_y; i++) {
			for (int j = 0; j < scale_pixels_x; j++) {
				memcpy(framebuffer + i * params->pitch + j * params->depth, &color, params->depth);
			}
		}

		bits --;
		mask <<= 1;

		if (bits % 8 == 0) {
			src--;
			mask = *src;
		}
		if (bits % width == 0) {
			x = x_i;
			y += scale;
		}
		else {
			x += scale;
		}
	}
}
*/

void kernel_main(struct multiboot_info* multiboot_info, int magic_number) {
	if (magic_number != 0x2BADB002) {
		return; // Not multiboot
	}

	video_multiboot_init(multiboot_info);
	bitmap_font_init();

	if (video_get_params()->mode == VIDEO_MODE_TTY) {
		// Disable VGA cursor
		outb(0x3D4, 0x0A);
		outb(0x3D5, 0x20);
	}

	//put_bitmap(100, 100, BITMAP_FONT['X' - ' '], sizeof(BITMAP_FONT[0]), 8, 4);

	//print("test123\nnewline\n");
	//print(struint_hex(multiboot_info->framebuffer_addr, 8));
}
