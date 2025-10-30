#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

static void (*draw_line)(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1);

// Test horizontal line (left to right)
void test_horizontal_lr(void) {
    draw_line(10, 50, 100, 50);
}

// Test horizontal line (right to left)
void test_horizontal_rl(void) {
    draw_line(100, 60, 10, 60);
}

// Test vertical line (top to bottom)
void test_vertical_tb(void) {
    draw_line(50, 10, 50, 100);
}

// Test vertical line (bottom to top)
void test_vertical_bt(void) {
    draw_line(60, 100, 60, 10);
}

// Test diagonal line (45 degrees, octant 0)
void test_diagonal_45_oct0(void) {
    draw_line(10, 10, 50, 50);
}

// Test diagonal line (45 degrees, octant 4)
void test_diagonal_45_oct4(void) {
    draw_line(50, 50, 10, 10);
}

// Test diagonal line (-45 degrees, octant 7)
void test_diagonal_neg45_oct7(void) {
    draw_line(10, 50, 50, 10);
}

// Test diagonal line (-45 degrees, octant 3)
void test_diagonal_neg45_oct3(void) {
    draw_line(50, 10, 10, 50);
}

// Test shallow line (dx > dy, octant 0)
void test_shallow_oct0(void) {
    draw_line(70, 10, 120, 30);
}

// Test shallow line (octant 3)
void test_shallow_oct3(void) {
    draw_line(130, 30, 80, 10);
}

// Test steep line (dy > dx, octant 1)
void test_steep_oct1(void) {
    draw_line(70, 40, 90, 90);
}

// Test steep line (octant 6)
void test_steep_oct6(void) {
    draw_line(100, 90, 80, 40);
}

// Test all 8 octants from center point
void test_all_octants(void) {
    uint8_t cx = 128, cy = 96;
    uint8_t len = 40;
    
    draw_line(cx, cy, cx + len, cy);           // East
    draw_line(cx, cy, cx + len, cy + len);     // Southeast
    draw_line(cx, cy, cx, cy + len);           // South
    draw_line(cx, cy, cx - len, cy + len);     // Southwest
    draw_line(cx, cy, cx - len, cy);           // West
    draw_line(cx, cy, cx - len, cy - len);     // Northwest
    draw_line(cx, cy, cx, cy - len);           // North
    draw_line(cx, cy, cx + len, cy - len);     // Northeast
}

// Test single pixel (degenerate line)
void test_single_pixel(void) {
    draw_line(100, 100, 100, 100);
}

// Test two adjacent pixels (horizontal)
void test_two_pixel_h(void) {
    draw_line(110, 100, 111, 100);
}

// Test two adjacent pixels (vertical)
void test_two_pixel_v(void) {
    draw_line(110, 110, 110, 111);
}

// Test screen edges - top edge
void test_edge_top(void) {
    draw_line(0, 0, 255, 0);
}

// Test screen edges - bottom edge
void test_edge_bottom(void) {
    draw_line(0, 191, 255, 191);
}

// Test screen edges - left edge
void test_edge_left(void) {
    draw_line(0, 0, 0, 191);
}

// Test screen edges - right edge
void test_edge_right(void) {
    draw_line(255, 0, 255, 191);
}

// Test screen corners - diagonal across screen
void test_corner_diagonal(void) {
    draw_line(0, 0, 255, 191);
}

// Test overdraw - draw same line multiple times
void test_overdraw_same(void) {
    uint8_t i;
    for (i = 0; i < 5; i++) {
        draw_line(150, 50, 200, 80);
    }
}

// Test overdraw - crossing lines
void test_overdraw_cross(void) {
    draw_line(140, 100, 190, 150);
    draw_line(140, 150, 190, 100);
}

// Test grid pattern for visual accuracy
void test_grid_pattern(void) {
    uint8_t x, y, i;
    
    // Vertical lines
    for (i = 0; i <= 256/32; i++) {
        x = i*32;
        draw_line(x, 0, x, 191);
    } 
    
    // Horizontal lines
    for (y = 0; y < 192; y += 24) {
        draw_line(0, y, 255, y);
    }
}

// Performance test - many short horizontal lines
void test_perf_horizontal(void) {
    uint8_t y;
    for (y = 0; y < 192; y++) {
        draw_line(0, y, 255, y);
    }
}

// Performance test - many short vertical lines
void test_perf_vertical(void) {
    uint8_t x = 0;
    do {
        draw_line(x, 0, x, 191);
        if (x++ == 255) break;
    } while (true);
}

// Performance test - many diagonal lines
void test_perf_diagonal_x(void) {
    uint8_t i;
    for (i = 0; i < 192; i += 2) {
        draw_line(0, i, 255, 191 - i);
    }
}

// Performance test - many diagonal lines
void test_perf_diagonal_y(void) {
    uint8_t x = 0;
    do {
        draw_line(x, 0, 255-x, 191);
        if (x == 254) break;
        x += 2;
    } while (true);
}

// Performance test - worst case (steep lines)
void test_perf_steep(void) {
    uint8_t x;
    for (x = 0; x < 255; x += 5) {
        draw_line(x, 0, x + 1, 191);
    }
}

// Performance test - worst case (shallow lines)
void test_perf_shallow(void) {
    uint8_t y;
    for (y = 0; y < 191; y += 5) {
        draw_line(0, y, 255, y + 1);
    }
}

// Performance test - random-looking lines (deterministic)
void test_perf_random(void) {
    uint8_t i;
    uint8_t x0, y0, x1, y1;
    
    for (i = 0; i < 100; i++) {
        x0 = (i * 37) & 0xFF;
        y0 = (i * 23) % 192;
        x1 = (i * 89) & 0xFF;
        y1 = (i * 67) % 192;
        draw_line(x0, y0, x1, y1);
    }
}

// Star pattern - good visual test
void test_star_pattern(void) {
    uint8_t cx = 128, cy = 96;
    uint8_t i;
    
    for (i = 0; i < 16; i++) {
        uint8_t angle = i * 16;
        // Simple approximation for star points
        uint8_t x = cx + ((angle < 128) ? 60 : -60);
        uint8_t y = cy + ((angle & 64) ? 40 : -40);
        draw_line(cx, cy, x, y);
    }
}

// Box pattern test
void test_box_pattern(void) {
    uint8_t x, y;
    uint8_t size = 20;
    
    for (y = 20; y < 170; y += size + 5) {
        for (x = 20; x < 240; x += size + 5) {
            draw_line(x, y, x + size, y);           // Top
            draw_line(x + size, y, x + size, y + size); // Right
            draw_line(x + size, y + size, x, y + size); // Bottom
            draw_line(x, y + size, x, y);           // Left
        }
    }
}

// Test function pointer type
typedef void (*test_func)(void);

// Test registry structure
typedef struct {
    const char *name;
    test_func func;
    uint8_t is_performance;
} test_entry;

// Test registry
test_entry tests[] = {
    // Accuracy tests
    {"Horizontal L->R", test_horizontal_lr, 0},
    {"Horizontal R->L", test_horizontal_rl, 0},
    {"Vertical T->B", test_vertical_tb, 0},
    {"Vertical B->T", test_vertical_bt, 0},
    
    {"Diagonal 45° Oct0", test_diagonal_45_oct0, 0},
    {"Diagonal 45° Oct4", test_diagonal_45_oct4, 0},
    {"Diagonal -45° Oct7", test_diagonal_neg45_oct7, 0},
    {"Diagonal -45° Oct3", test_diagonal_neg45_oct3, 0},

    {"Shallow Oct0", test_shallow_oct0, 0},
    {"Shallow Oct3", test_shallow_oct3, 0},
    {"Steep Oct1", test_steep_oct1, 0},
    {"Steep Oct6", test_steep_oct6, 0},

    {"All 8 Octants", test_all_octants, 0},
    {"Single Pixel", test_single_pixel, 0},
    {"Two Pixels H", test_two_pixel_h, 0},
    {"Two Pixels V", test_two_pixel_v, 0},

    {"Edge Top", test_edge_top, 0},
    {"Edge Bottom", test_edge_bottom, 0},
    {"Edge Left", test_edge_left, 0},
    {"Edge Right", test_edge_right, 0},

    {"Corner Diagonal", test_corner_diagonal, 0},
    {"Overdraw Same", test_overdraw_same, 0},
    {"Overdraw Cross", test_overdraw_cross, 0},
    {"Grid Pattern", test_grid_pattern, 0},
    {"Star Pattern", test_star_pattern, 0},
    {"Box Pattern", test_box_pattern, 0},
    
    // Performance tests
    {"PERF: Horizontal", test_perf_horizontal, 1},
    {"PERF: Vertical", test_perf_vertical, 1},
    {"PERF: Diagonal X", test_perf_diagonal_x, 1},
    {"PERF: Diagonal Y", test_perf_diagonal_y, 1},
    {"PERF: Steep", test_perf_steep, 1},
    {"PERF: Shallow", test_perf_shallow, 1},
    {"PERF: Random", test_perf_random, 1},
    
    {NULL, NULL, 0} // Sentinel
};

void set_test_draw_line_func(void (*func)(uint8_t, uint8_t, uint8_t, uint8_t)) {
    draw_line = func;
}

const char *get_test_name(uint8_t index) {
    return tests[index].name;
}

uint8_t get_num_tests(void) {
    uint8_t count = 0;
    while (tests[count].name != NULL) {
        count++;
    }
    return count;
}

// Run a single test
void run_test(uint8_t index) {
    tests[index].func();
}

// Run all accuracy tests
void run_all_accuracy_tests(void) {
    uint8_t i;
    for (i = 0; tests[i].name != NULL; i++) {
        if (!tests[i].is_performance) {
            run_test(i);
        }
    }
}

// Run all performance tests
void run_all_performance_tests(void) {
    uint8_t i;
    for (i = 0; tests[i].name != NULL; i++) {
        if (tests[i].is_performance) {
            run_test(i);
        }
    }
}