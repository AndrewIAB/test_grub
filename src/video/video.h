#pragma once

#include "../multiboot.h"

#define VIDEO_MODE_BMP 0
#define VIDEO_MODE_TTY 1

typedef struct __attribute__((packed)) {
	unsigned char r, g, b;
} video_palette_entry_t;

typedef struct {
	unsigned char ro, rs, go, gs, bo, bs;
} video_color_mask_t;

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

/*  */
typedef uint32_t video_color_t;

void video_set_params(const video_params_t*);
const video_params_t* video_get_params();

void video_multiboot_init(const multiboot_info_t*);
