#include <stdio.h>
#include <stdlib.h>
#include <input.h>
#include <arch/zx.h>

void draw_line_c(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1) {
    int16_t dx = abs(x1 - x0);
    int8_t sx = x0 < x1 ? 1 : -1;
    int16_t dy = -abs(y1 - y0);
    int8_t sy = y0 < y1 ? 1 : -1;
    int16_t error = dx + dy;

    while (1) {
        // plot(x0, y0)
        if (y0 < 192) {
            uint8_t* screen_addr = zx_py2saddr(y0) + (x0 >> 3);
            uint8_t bit_pos = x0 & 0x07;
            uint8_t mask = 1 << (7 - bit_pos);
            uint8_t pixel_byte = *screen_addr;
            pixel_byte |= mask;
            *screen_addr = pixel_byte;
        }

        int16_t e2 = 2 * error;
        if (e2 >= dy) {
            if (x0 == x1) break;
            error += dy;
            x0 += sx;
        }
        if (e2 <= dx) {
            if (y0 == y1) break;
            error += dx;
            y0 += sy;
        }
    }
}
