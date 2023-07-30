#include <stdio.h>
#include "test.h"
#include "adc.h"
#include "gpio.h"
#include "motor.h"
#include "slave_board_base.h"

int test_adc(ADCPort * senses){
  adc_config(senses, 2);
  while(1) {
    adc_ordinary_software_trigger_enable(ADC1, TRUE);
    uint16_t * sensed = read_adcs(senses, 2);
    printf("sense1:%u sense2:%u\n", sensed[0], sensed[1]);
    delay_ms(1000);
  }
}

int test_brush_motor(MotorUnified * motor){
  init_motor_brush(motor);
  
  int8_t speed = 0;
  int8_t direction = 1;
  while (1){
    printf("current speed set:%d\n", speed);
    delay_ms(1000);
    speed += 10 * direction;
    if (speed>=100){
      direction = -1;
      speed=100;
    } else if (speed <=-100){
      speed=-100;
      direction = 1;
    }
    motor_set_speed(motor, speed);
  }
}

int test_brushless_motor(MotorUnified * motor){
  /* init brushless motor */
  init_motor_brushless(motor);
  
  int8_t speed = 0;
  int8_t direction = 1;
  while (1){
    printf("current speed sense:%u & set:%d\n", motor_speed_sense(motor), speed);
    delay_ms(1000);
    speed += 10 * direction;
    if (speed>=100){
      direction = -1;
      speed=100;
    } else if (speed <=-100){
      speed=-100;
      direction = 1;
    }
    motor_set_speed(motor, speed);
  }
}
