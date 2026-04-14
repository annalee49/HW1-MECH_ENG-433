#include "font.h"
#include "ssd1306.h"
#include <stddef.h>  // for NULL if needed

void drawChar(unsigned char x, unsigned char y, unsigned char color, char c) {
    if (c < 32 || c > 127) {
        c = '?';  // fallback
    }
    int index = c - 32;

    for (int col = 0; col < 5; col++) {
        unsigned char line = ASCII[index][col];
        for (int row = 0; row < 8; row++) {
            unsigned char pixel_on = (line >> row) & 1;
            ssd1306_drawPixel(x + col, y + row, pixel_on ? color : 0);
        }
    }
}
