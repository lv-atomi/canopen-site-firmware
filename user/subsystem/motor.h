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

typedef struct {
  MotorBrush brush;
  MotorBrushless brushless;
  uint8_t is_brushless;
} MotorUnified;

void init_motor_brush(MotorUnified *motor);
void init_motor_brushless(MotorUnified *motor);
void motor_set_speed(MotorUnified *motor, int8_t speed);
uint8_t motor_speed_sense(MotorUnified *motor);

#endif
