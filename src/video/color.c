#include <stdint.h>
#include <stddef.h>
#include <limits.h>

#include "video.h"

unsigned int manhattan_distance(int x1, int y1, int z1, int x2, int y2, int z2) {
	unsigned int x, y, z;
	x = (x1 > x2) ? x1 - x2 : x2 - x1;
	y = (y1 > y2) ? y1 - y2 : y2 - y1;
	z = (z1 > z2) ? z1 - z2 : z2 - z1;
	return x + y + z;
}

video_color_t get_color_from_palette(unsigned char r, unsigned char g, unsigned char b, const video_palette_list_t* palette_list) {
	unsigned int min_dist = UINT_MAX;
	video_color_t min_color = 0;

	for (int i = 0; i < palette_list->len; i++) {
		const video_palette_entry_t* check = &(palette_list->palette[i]);
		unsigned int dist = manhattan_distance(
			r, g, b,
			check->r, check->g, check->b);
		if (dist < min_dist) {
			min_dist = dist;
			min_color = i;
		}
	}

	return min_color;
}

video_color_t get_color(unsigned char ri, unsigned char gi, unsigned char bi) {
	const video_params_t* params = video_get_params();

	if (params->mode == VIDEO_MODE_INDEXED || params->mode == VIDEO_MODE_TTY) {
		return get_color_from_palette(ri, gi, bi, &params->palette_list);
	}

	uint8_t red_mask = (1 << params->color_mask.rs) - 1;
	uint8_t blue_mask = (1 << params->color_mask.bs) - 1;
	uint8_t green_mask = (1 << params->color_mask.gs) - 1;

	uint8_t r = (unsigned int)ri * (unsigned int)red_mask / 255;
	uint8_t g = (unsigned int)gi * (unsigned int)green_mask / 255;
	uint8_t b = (unsigned int)bi * (unsigned int)blue_mask / 255;

	return
		(r << params->color_mask.ro) |
		(g << params->color_mask.go) |
		(b << params->color_mask.bo);
}
