#include "log.h"
#include "fan.h"
#include "pwm.h"

void init_fan(PWMFan *devport, uint8_t speed){
  log_printf("abcd");
  ASSERT(speed <= 100);
  init_pwm_input(&devport->speed_sense);
  init_pwm_output(&devport->speed_control, 25000, speed);
}
