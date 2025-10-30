#ifndef TESTS_H
#define TESTS_H

#include <stdint.h>

uint8_t get_num_tests(void);
const char *get_test_name(uint8_t index);
void run_test(uint8_t index);

void set_test_draw_line_func(void (*func)(uint8_t, uint8_t, uint8_t, uint8_t));

void run_all_accuracy_tests(void);
void run_all_performance_tests(void);

#endif // TESTS_H