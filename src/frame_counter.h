#ifndef __FRAME_COUNTER_H__
#define __FRAME_COUNTER_H__

extern void start_frame_counter(void);
extern void resume_frame_counter(void);
extern void pause_frame_counter(void);
extern uint32_t stop_frame_counter(void);

#endif /* __FRAME_COUNTER_H__ */