#ifndef __TIMER1_H
#define __TIMER1_H

#include "at32f403a_407.h"

#ifdef __cplusplus
extern "C" {
#endif


#define TIME_1MS  10-1

extern uint16_t TimerCount_1ms;
void Timer1_Init(void);


#endif

