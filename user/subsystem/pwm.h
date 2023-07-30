#ifndef PWM_H
#define PWM_H

#include "gpio.h"
#include "board.h"
#include <stdint.h>

typedef struct{
  IOPort port;
  tmr_type * tmr;
  tmr_channel_select_type channel;
  uint8_t complementary;
} PWMPort;

void tmr_clock_enable(tmr_type *tmr_x);
void init_pwm_output(PWMPort *devport, uint32_t freq, uint16_t duty);
void init_pwm_input(PWMPort * devport);
void pwm_output_update_duty(PWMPort * devport, uint8_t duty);
void pwm_output_update_freq(PWMPort *devport, uint32_t freq);
void pwm_input_freq_duty_sense(PWMPort * devport, uint32_t * freq, uint8_t * duty);
#endif
