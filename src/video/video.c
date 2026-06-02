#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#include "video.h"

void* memcpy(void*, const void*, size_t);

static video_params_t video_params;

void video_set_params(const video_params_t* src) {
	memcpy(&video_params, src, sizeof(video_params_t));
}

const video_params_t* video_get_params() {
	return &video_params;
}

void video_multiboot_init(const multiboot_info_t* multiboot_info) {
	bool fbf_info = (multiboot_info->flags & MULTIBOOT_INFO_FRAMEBUFFER_INFO) != 0;

	/* Bootloader provides NO framebuffer
	 * Assume base VGA text mode */
	if (!fbf_info) {
		video_params.width = 80;
		video_params.height = 25;
		video_params.depth = 2;
		video_params.pitch = 160;
		video_params.mode = VIDEO_MODE_TTY;
		video_params.framebuffer = (void*)0xB8000;
		return;
	}

	/* Gather actual values from multiboot header */

	switch (multiboot_info->framebuffer_type) {
		case MULTIBOOT_FRAMEBUFFER_TYPE_INDEXED:
			video_params.mode = VIDEO_MODE_BMP;
			video_params.palette = (void*)multiboot_info->framebuffer_palette_addr;
			video_params.palette_len = multiboot_info->framebuffer_palette_num_colors;
			break;
		case MULTIBOOT_FRAMEBUFFER_TYPE_RGB:
			video_params.mode = VIDEO_MODE_BMP;
			memcpy(
				&video_params.color_mask,
				&multiboot_info->framebuffer_red_field_position,
				6);
			break;
		
		case MULTIBOOT_FRAMEBUFFER_TYPE_EGA_TEXT:
			video_params.mode = VIDEO_MODE_TTY;
			break;
	}

	video_params.pitch = multiboot_info->framebuffer_pitch;
	video_params.width = multiboot_info->framebuffer_width;
	video_params.height = multiboot_info->framebuffer_height;
	video_params.depth = multiboot_info->framebuffer_bpp / 8;
	video_params.framebuffer = (void*)(uint32_t)multiboot_info->framebuffer_addr;
}
