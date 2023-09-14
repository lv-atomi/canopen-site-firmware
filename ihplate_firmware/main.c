#include "board.h"
#include <stdio.h>

#include "log.h"
#include "gpio.h"
#include "ihplate_base.h"
#include "test.h"


void init_system(void){
  nvic_priority_group_config(NVIC_PRIORITY_GROUP_4);
  system_clock_config();
  at32_board_init();
  uart_print_init(115200);  
}

void main_logic(){
  //adc_config(senses, 2);
  
  /* delay_ms(1000); */
  /* uint16_t position = sense_position(senses, 2); */
  /* i2c.address += position * 2; */
  /* i2c_config(&i2c); */

  /* init_motor_brushless(&motor); */

  log_printf("software_trigger_repeat\n");

  while(1) {
    adc_ordinary_software_trigger_enable(ADC1, TRUE);
    /* uint16_t * sensed = read_adcs(senses, 2); */
    /* printf("sense1:%u sense2:%u\n", sensed[0], sensed[1]); */
    delay_ms(1000);
  }

}

int main(void) {
  init_system();
  /* test_led();			/\* pass *\/ */
  /* test_gpin();			/\* pass, but pulse det not connected to adc *\/ */
  /* test_adc();			/\* adc pass *\/ */
  /* test_pwm();			/\* pass *\/ */
  
}

