#ifndef __DRAW_LINE_H__
#define __DRAW_LINE_H__

#include <stdint.h>

void draw_line_c(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1);
void draw_line_asm(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1);

#endif /* __DRAW_LINE_H__ */