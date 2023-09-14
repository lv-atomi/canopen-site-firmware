#ifndef TIMER_H
#define TIMER_H

#include <stdint.h>

typedef void (*tick_cb)(void) ;

uint32_t get_ticks(void);
void timer_add_tick(tick_cb);
void timer_pause(void);
void timer_resume(void);
uint32_t ticks_diff(uint32_t *last_ticks);


#endif
