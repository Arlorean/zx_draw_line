#include <input.h>
#include <stdio.h>
#include <stdint.h>
#include <arch/zx.h>
#include "draw_line.h"
#include "frame_counter.h"
#include "tests.h"

#define tostring(id) #id
#define nameof(id) tostring(id)

void print_frames(uint32_t frames);
uint32_t run_tests(uint8_t from, uint8_t to, void (*draw_line_func)(uint8_t, uint8_t, uint8_t, uint8_t),const char* function);

void main(void) {
	zx_border(INK_YELLOW);
	zx_cls(INK_BLACK|PAPER_WHITE);
	zx_border(INK_GREEN);

	uint32_t frames = 0;
	frames = run_tests(0, 25, draw_line_asm, nameof(draw_line_asm));
	//frames = run_tests(0, 32, draw_line_c, nameof(draw_line_c));
	print_frames(frames);

	zx_border(INK_RED);
	in_wait_key();
	zx_border(INK_WHITE);
}


#pragma printf "%c%d%s" // printf format specifiers used

#define printInk(k)          printf("\x10%c", '0'+(k))
#define printPaper(k)        printf("\x11%c", '0'+(k))
#define printAt(row, col)    printf("\x16%c%c", (col)+1, (row)+1)

char buffer[32];

void print_frames(uint32_t frames) {
	printPaper(1);
    printInk(7);

	printAt(22,1);
    snprintf(buffer, sizeof(buffer), "%5d frames", frames);
    printf("         %s         ", buffer);
}

void print_test_info(uint8_t index, const char* function) {
	printPaper(1);
	printInk(7);

	printAt(1,16);
	snprintf(buffer, sizeof(buffer), " %s ", function);
	printf("%s", buffer);

	printAt(22,1);
	const char* test_name = get_test_name(index);
	snprintf(buffer, sizeof(buffer), " Running:%20s ", test_name);
	printf("%s", buffer);
}

uint32_t run_tests(uint8_t from, uint8_t to, void (*draw_line_func)(uint8_t, uint8_t, uint8_t, uint8_t), const char* function) {
	set_test_draw_line_func(draw_line_func);

	start_frame_counter();
	pause_frame_counter();

	for (uint8_t i = from; i <= to; i++) {
		print_test_info(i, function);

		resume_frame_counter();
		run_test(i);
		pause_frame_counter();

		print_test_info(i, function);
	}

	return stop_frame_counter();
}