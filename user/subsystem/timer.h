#ifndef __TIMER1_H
#define __TIMER1_H

#include "board.h"

#ifdef __cplusplus
extern "C" {
#endif


#define TIME_1MS  10-1
#define MAX_TICKS 16
typedef void (*tick_cb)(void) ;
  
void Timer_Init(void);
uint32_t get_ticks(void);
void timer_add_tick(tick_cb, uint32_t interval_ms);
void timer_pause(void);
void timer_resume(void);
uint32_t ticks_diff(uint32_t *last_ticks);

#endif

