#pragma once

#include "bitmap.h"

extern int bitmap_font_width;
extern int bitmap_font_height;

extern bitmap_t bitmap_font[256];
//extern const unsigned char BITMAP_FONT_DATA[95][13];

void bitmap_font_init();
