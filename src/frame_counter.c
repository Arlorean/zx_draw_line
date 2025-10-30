#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <im2.h>
#include <intrinsic.h>
#include <z80.h>


static bool initialized = 0;
static uint32_t frame_counter = 0;

IM2_DEFINE_ISR(frame_counter_isr) {
    frame_counter++;
}

#define TABLE_HIGH_BYTE        ((unsigned int)0xfc)
#define JUMP_POINT_HIGH_BYTE   ((unsigned int)0xfb)

#define UI_256                 ((unsigned int)256)

#define TABLE_ADDR             ((void*)(TABLE_HIGH_BYTE*UI_256))
#define JUMP_POINT             ((unsigned char*)( (unsigned int)(JUMP_POINT_HIGH_BYTE*UI_256) + JUMP_POINT_HIGH_BYTE ))

static void initialize(void) {
    /* Set up the interrupt vector table */
    im2_init( TABLE_ADDR );

    memset( TABLE_ADDR, JUMP_POINT_HIGH_BYTE, 257 );

    z80_bpoke( JUMP_POINT,   195 );
    z80_wpoke( JUMP_POINT+1, (unsigned int)frame_counter_isr );
}

void start_frame_counter(void) {
    if (!initialized) {
        initialized = 1;
        initialize();
    }

    frame_counter = 0;
    resume_frame_counter();
}

void pause_frame_counter(void) {
    intrinsic_di();
}

void resume_frame_counter(void) {
    uint32_t temp = frame_counter;
    intrinsic_ei();
    intrinsic_halt();
    frame_counter = temp;
}

uint32_t stop_frame_counter(void) {
    pause_frame_counter();
    return frame_counter;
}