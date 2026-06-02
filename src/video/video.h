#pragma once

#include "../multiboot.h"

#define VIDEO_MODE_BMP 0
#define VIDEO_MODE_TTY 1

/* Can be  directly translated into uint32_t if colors are formatted as
 * standard 32-bit RGB with the highest byte for padding,
 * or can be memcpy-d into place if standard 24-bit RGB */
typedef struct __attribute__((packed)) {
	unsigned char b, g, r, _pad;
} video_palette_entry_t;

/* Directly mirrors GRUB's video color masks */
typedef struct {
	unsigned char ro, rs, go, gs, bo, bs;
} video_color_mask_t;

/* Loosely mirrors GRUB's video parameters */
typedef struct {
	int width, height, depth, pitch, mode;
	void* framebuffer;
	union {
		struct {
			video_palette_entry_t* palette;
			int palette_len;
		};
		video_color_mask_t color_mask;
	};
} video_params_t;

/* Type to fit largest compatible video color */
typedef uint32_t video_color_t;

void video_set_params(const video_params_t*);
const video_params_t* video_get_params();

void video_multiboot_init(const multiboot_info_t*);
