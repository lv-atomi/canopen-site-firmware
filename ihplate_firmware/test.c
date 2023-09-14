#include <stdint.h>
#include <stdio.h>
#include "board.h"
#include "log.h"
#include "pwm.h"
#include "test.h"
#include "adc.h"
#include "gpio.h"
#include "motor.h"
#include "ihplate_base.h"
#include <string.h>

#include "portdef.h"

void test_adc() {
  static ADCPort senses[4];
  memcpy(&senses[0], &IGBTVoltage, sizeof(ADCPort));
  memcpy(&senses[1], &IGBTCurrent, sizeof(ADCPort));
  memcpy(&senses[2], &PlateTemperature, sizeof(ADCPort));
  memcpy(&senses[3], &IGBTTemperature, sizeof(ADCPort));
  /* memcpy(&senses[4], &NoiseDetected, sizeof(ADCPort)); */
  adc_config(senses, 4);
  
  while(1) {
    uint16_t * sensed = read_adcs(senses, 4);
    log_printf("IGBT voltage:%u,  IGBT current:%u,  PlateTemperature:%u,  IGBTTemperature:%u\n",
	       sensed[0], sensed[1], sensed[2], sensed[3]);
    delay_ms(1000);
  }
}


void test_led(){
  init_gpio_output(&led, GPIO_OUTPUT_PUSH_PULL, GPIO_DRIVE_STRENGTH_STRONGER);
  while(1){
    log_printf("led off\n");
    gpio_set(&led, SET);
    delay_ms(500);
    log_printf("led on\n");
    gpio_set(&led, RESET);
    delay_ms(500);
  }
}

void test_gpin(){
  init_gpio_input(&NoiseDetected, GPIO_PULL_UP, GPIO_DRIVE_STRENGTH_STRONGER);
  init_gpio_input(&PulseDetected, GPIO_PULL_UP, GPIO_DRIVE_STRENGTH_STRONGER);
  
  while (1){
    log_printf("noise det: %u, "
	       "pulse det:%u\n",
	       gpio_read(&NoiseDetected),
	       gpio_read(&PulseDetected)
	       );
    delay_ms(500);
  }
}

void test_pwm(){
  init_pwm_output(&IGBTPwm, 10000, 50);
  int8_t dir = 1;
  int8_t duty = 50;
  while(1){
    duty += dir*10;
    if (duty > 100){
      duty = 100;
      dir = - dir;
    }
    if (duty < 0){
      duty = 0;
      dir = - dir;
    }
    log_printf("set duty:%u\n", duty);
    pwm_output_update_duty(&IGBTPwm, duty);
    delay_ms(300);
  }
}
