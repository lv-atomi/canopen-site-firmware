#ifndef UTILS_H
#define UTILS_H
/* 常用的gpio初始化，pwm初始化，端口封装结构等 */

#include "at32f403a_407_board.h"
#include <stdint.h>

typedef struct {
  gpio_type *port;
  uint16_t pin;
} IOPort;

typedef struct{
  IOPort port;
  tmr_type * tmr;
  tmr_channel_select_type channel;
  uint8_t complementary;
} PWMPort;


void tmr_clock_enable(tmr_type *tmr_x);
void init_pwm_output(PWMPort *devport, uint32_t freq, uint16_t duty);
void init_pwm_input(PWMPort * devport);

#endif
