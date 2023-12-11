#include "gpio.h"
#include "motor.h"
#include "pwm.h"
#include "log.h"
#include "portdef_motor_direct_drive.h"
#include <stdint.h>
#include <stdlib.h>

#define AVERAGE_COUNT 5  // for example, average over the last 5 measurements
uint8_t duty_values[AVERAGE_COUNT] = {0};

int main(){
  nvic_priority_group_config(NVIC_PRIORITY_GROUP_4);
  system_clock_config();
  at32_board_init();
  uart_print_init(115200);

  init_gpio_output(&mdd_led, GPIO_OUTPUT_OPEN_DRAIN, GPIO_DRIVE_STRENGTH_STRONGER);
  init_gpio_input(&mdd_direction, GPIO_PULL_UP, GPIO_DRIVE_STRENGTH_STRONGER);

  init_motor_brush(&mdd_motor);
  init_pwm_input(&mdd_speed_control);
  init_gpio_mux(&mdd_speed_control.port, GPIO_OUTPUT_PUSH_PULL,
		GPIO_PULL_UP,
		GPIO_DRIVE_STRENGTH_STRONGER);
  init_pwm_output(&mdd_speed_sense, 1000, 50);

  uint32_t freq=0;
  uint8_t duty=0;
  uint8_t dir=0;
  int8_t speed=1;
  uint8_t counter = 0;
  uint8_t counter_cycle = 20;
  uint8_t dead_timeout=0;
  uint8_t ctl_in, last_ctl_in = 0;
  uint32_t duty_sum=0;
  uint8_t duty_index=0;
  uint32_t last_freq=0;
  uint16_t ic1value, last_ic1value;
  
  while(1){
    pwm_input_freq_duty_sense(&mdd_speed_control, &freq, &duty);
    if ((freq > 20000) || (freq < 500)) duty=0;
    if (dead_timeout==0) duty = 0;
    
    // Update the rolling average
    duty_sum -= duty_values[duty_index];  // subtract the oldest value
    duty_values[duty_index] = duty;       // store the new value
    duty_sum += duty;                     // add the new value

    duty_index++;
    if (duty_index >= AVERAGE_COUNT) {
      duty_index = 0;
    }
    
    uint8_t average_duty = duty_sum / AVERAGE_COUNT;
 
    dir = gpio_read(&mdd_direction);
    log_printf("freq:%ld, duty:%u, dir:%u, avg_duty:%u\n",
	       freq, duty, dir, average_duty);
    pwm_output_update_duty(&mdd_speed_sense, 30);
    speed = average_duty * (dir == 1 ? 1 : -1);
    motor_set_speed(&mdd_motor, speed);
    ctl_in = gpio_read(&mdd_speed_control.port);
    ic1value = tmr_channel_value_get(mdd_speed_control.tmr, TMR_SELECT_CHANNEL_1);
    if ((ctl_in != last_ctl_in) || (ic1value != last_ic1value))
      dead_timeout = 15;
    else
      dead_timeout = dead_timeout == 0 ? 0 : dead_timeout-1;
   
    last_ctl_in = ctl_in;
    last_ic1value = ic1value;
    log_printf("ctl in:%d, dead:%u, ic1value:%d\n", ctl_in, dead_timeout, ic1value);
    
    counter_cycle = ((100-abs(speed)) / 2) + 2;
    /* led */
    if (counter < counter_cycle / 2){
      log_printf("led on, %d/%d\n", counter, counter_cycle);
      gpio_set(&mdd_led, 0);
    } else {
      log_printf("led off, %d/%d\n", counter, counter_cycle);
      gpio_set(&mdd_led, 1);
    }
    counter += 1;
    if (counter > counter_cycle) counter = 0;
    delay_ms(50);
  }
}
