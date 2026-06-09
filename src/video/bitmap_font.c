#include "bitmap_font.h"
#include "XGA_8x16.h"

int bitmap_font_width = 8;
int bitmap_font_height = 16;

bitmap_t bitmap_font[256];

void bitmap_font_init() {
	for (int i = 0; i < 256; i++) {
		bitmap_font[i].src = &(XGA_8x16[i * 16]);
		bitmap_font[i].width = 8;
		bitmap_font[i].len = 16;
	}
}
