#pragma once

#include "../multiboot.h"

#define VIDEO_MODE_COLOR 0
#define VIDEO_MODE_INDEXED 1
#define VIDEO_MODE_TTY 2

/* Primarily for traversing color palette */
typedef struct __attribute__((packed)) {
	unsigned char r, g, b;
} video_palette_entry_t;

/* Palette list */
typedef struct {
	const video_palette_entry_t* palette;
	int len;
} video_palette_list_t;

/* Directly mirrors GRUB's video color masks */
typedef struct {
	unsigned char ro, rs, go, gs, bo, bs;
} video_color_mask_t;

/* Loosely mirrors GRUB's video parameters */
typedef struct {
	int width, height, depth, pitch, mode;
	void* framebuffer;
	union {
		video_palette_list_t palette_list;
		video_color_mask_t color_mask;
	};
} video_params_t;

/* Type to fit largest compatible video color */
typedef uint32_t video_color_t;

void video_set_params(const video_params_t*);
const video_params_t* video_get_params();

void video_multiboot_init(const multiboot_info_t*);


video_color_t get_color(unsigned char r, unsigned char g, unsigned char b);

