#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#include "bitmap.h"
#include "video.h"

#define MAX(X, Y) ((X) < (Y) ? (Y) : (X))
#define MIN(X, Y) ((X) < (Y) ? (X) : (Y))

void* memcpy(void* dest, const void* src, size_t len);
void* memset (void *dest, register int val, register size_t len);

void blit_bitmap(int x, int y, bitmap_t* bitmap, int scale) {
	const video_params_t* params = video_get_params();

	const int x_i = x;

	const uint8_t* src = bitmap->src + bitmap->len - 1;

	int bits = bitmap->len * 8;
	bits -= bits % bitmap->width;
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
		if (bits % bitmap->width == 0) {
			x = x_i;
			y += scale;
		}
		else {
			x += scale;
		}
	}
}
