#ifndef FAN_H
#define FAN_H

#include "pwm.h"
#include "board.h"
#include <stdint.h>

typedef struct{
  PWMPort speed_sense;
  PWMPort speed_control;
  SPIPort SPI2;
} PWMFan;

void init_fan(PWMFan *devport, uint8_t speed);

#endif
