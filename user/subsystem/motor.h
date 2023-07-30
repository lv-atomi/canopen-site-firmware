#ifndef MOTOR_H
#define MOTOR_H

#include "gpio.h"
#include "pwm.h"
#include "board.h"
#include <stdint.h>

typedef struct {
  IOPort direction;
  PWMPort speed_sense, speed_set;
} MotorBrushless;

typedef struct {
  IOPort disable;
  PWMPort pwm_a, pwm_b;
} MotorBrush;

void init_motor_brush(MotorBrush *mb);
void init_motor_brushless(MotorBrushless *mbl);

#endif
