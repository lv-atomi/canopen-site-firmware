#ifndef PORTDEF_IHPLATE_H
#define PORTDEF_IHPLATE_H

#include "board.h"
#include "motor.h"
#include "rs232.h"
#include "pwm.h"
#include "adc.h"

/*
  PB0: led             GPIO output
  PB3: disable motor   GPIO output
  PA9: DEBUG_UART USART1
  PA3: SpeedSense      TMR15_CH2  (MUX0)
  PA4: SpeedControl    TMR14_CH1  (MUX4)
  PA5: Direction
  PA7: MotorPhaseB     TMR1_CH1C(MUX2)/TMR3_CH2(MUX1)
  PA8: MotorPhaseA     TMR1_CH1(MUX2)
*/

extern IOPort mdd_led;
extern IOPort mdd_disable;
extern IOPort mdd_direction;

extern MotorUnified mdd_motor;
extern PWMPort mdd_speed_sense;
extern PWMPort mdd_speed_control;
#endif
