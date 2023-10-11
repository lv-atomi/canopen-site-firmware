#include "at32f421_gpio.h"
#include "gpio.h"
#include "motor.h"
#include "pwm.h"
#include "log.h"
#include "portdef_motor_direct_drive.h"

int main(){
  nvic_priority_group_config(NVIC_PRIORITY_GROUP_4);
  system_clock_config();
  at32_board_init();
  uart_print_init(115200);

  init_gpio_output(&mdd_led, GPIO_OUTPUT_OPEN_DRAIN, GPIO_DRIVE_STRENGTH_STRONGER);
  init_motor_brush(&mdd_motor);
  init_pwm_input(&mdd_speed_control);
  init_gpio_mux(&mdd_speed_control.port, GPIO_OUTPUT_PUSH_PULL,
		GPIO_PULL_UP,
		GPIO_DRIVE_STRENGTH_STRONGER);
  init_pwm_output(&mdd_speed_sense, 1000, 50);

  uint32_t freq=0;
  uint8_t duty=0;
  uint8_t speed=1;
  uint8_t counter = 0;
  uint8_t counter_cycle = 20;
  
  while(1){
    pwm_input_freq_duty_sense(&mdd_speed_control, &freq, &duty);
    log_printf("freq:%ld, duty:%u\n", freq, duty);
    pwm_output_update_duty(&mdd_speed_sense, 30);
    motor_set_speed(&mdd_motor, speed);
    log_printf("ctl in:%d\n", gpio_read(&mdd_speed_control.port));
    counter_cycle = ((100-speed) / 2) + 2;
    /* led */
    if (counter < counter_cycle / 2){
      log_printf("led on, %d\n", counter);
      gpio_set(&mdd_led, 0);
    } else {
      log_printf("led off, %d\n", counter);
      gpio_set(&mdd_led, 1);
    }
    counter += 1;
    if (counter > counter_cycle) counter = 0;
    delay_ms(50);
  }
}
