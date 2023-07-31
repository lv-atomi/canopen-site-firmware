#include <stdio.h>
#include "at32f421_gpio.h"
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

int test_gpout(IOPort *gpout, uint8_t num_ports){
  int8_t i;
  for(i=0; i<num_ports; i++){
    init_gpio_output(&gpout[0], GPIO_OUTPUT_PUSH_PULL, GPIO_DRIVE_STRENGTH_STRONGER);
  }

  uint8_t status = 0;
  while (1){
    uint8_t v = status;
    for (i=0; i<num_ports; i++){
      printf("gpout%u: %u", i, v);
      gpio_set(&gpout[i], v);
      v = !v;
      if (i == (num_ports-1))
	printf("\n");
      else
	printf(", ");
    }
    status = !status;
    delay_ms(1000);
  }
}

int test_gpin(IOPort *gpin, uint8_t num_ports){
  int8_t i;
  for(i=0; i<num_ports; i++){
    init_gpio_input(&gpin[0], GPIO_PULL_UP, GPIO_DRIVE_STRENGTH_STRONGER);
  }
  
  while (1){
    for (i=0; i<num_ports; i++){
      printf("gpin%u: %u", i, gpio_read(&gpin[i]));
      if (i == (num_ports-1))
	printf("\n");
      else
	printf(", ");
    }
    delay_ms(1000);
  }
}

