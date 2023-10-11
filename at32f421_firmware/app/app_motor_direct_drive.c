#include "motor.h"
#include "pwm.h"
#include "log.h"
#include "portdef_motor_direct_drive.h"



int main(){
  nvic_priority_group_config(NVIC_PRIORITY_GROUP_4);
  system_clock_config();
  at32_board_init();
  uart_print_init(115200);
  
  init_motor_brush(&motor);
  init_pwm_input(&speed_set);
  init_pwm_output(&speed_sense, 1000, 50);

  uint32_t freq=0;
  uint8_t duty=0;
  uint8_t speed=0;
  
  while(1){
    pwm_input_freq_duty_sense(&speed_set, &freq, &duty);
    log_printf("freq:%ld, duty:%u\n", freq, duty);
    pwm_output_update_duty(&speed_sense, 30);
    motor_set_speed(&motor, speed);
    delay_ms(50);
  }
}


