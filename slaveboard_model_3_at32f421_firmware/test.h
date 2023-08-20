#ifndef TEST_H
#define TEST_H

#include "adc.h"
#include "i2c.h"
#include "motor.h"

int test_adc(ADCPort * senses);
int test_brush_motor(MotorUnified *motor);
int test_brushless_motor(MotorUnified * motor);
int test_gpin(IOPort *gpin, uint8_t num_ports);
int test_gpout(IOPort *gpout, uint8_t num_ports);

#endif
