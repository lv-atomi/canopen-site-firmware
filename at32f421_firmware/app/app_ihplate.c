#include "board.h"
#include <stdio.h>
#include <string.h>

#include "log.h"
#include "gpio.h"
#include "platform_base.h"
/* include test.h **OR** portdef.h */
/* #include "test.h" */
#include "portdef.h"

ADCPort senses[4];

void init_system(void){
  /* init base */
  nvic_priority_group_config(NVIC_PRIORITY_GROUP_4);
  system_clock_config();
  at32_board_init();
  uart_print_init(115200);

  /* init subsystems */
  /* led & inputs */
  init_gpio_output(&ihplate_led, GPIO_OUTPUT_PUSH_PULL, GPIO_DRIVE_STRENGTH_STRONGER);
  init_gpio_input(&NoiseDetected, GPIO_PULL_UP, GPIO_DRIVE_STRENGTH_STRONGER);
  init_gpio_input(&PulseDetected, GPIO_PULL_UP, GPIO_DRIVE_STRENGTH_STRONGER);

  /* adcs */
  
  memcpy(&senses[0], &IGBTVoltage, sizeof(ADCPort));
  memcpy(&senses[1], &IGBTCurrent, sizeof(ADCPort));
  memcpy(&senses[2], &PlateTemperature, sizeof(ADCPort));
  memcpy(&senses[3], &IGBTTemperature, sizeof(ADCPort));
  adc_config(senses, 4);

  /* pwm */
  init_pwm_output(&IGBTPwm, 10000, 10);
}

void main_logic(){
  init_system();

  pwm_output_update_duty(&IGBTPwm, 5);

  while(1) {
    uint16_t * sensed = read_adcs(senses, 4);
    log_printf("IGBT voltage:%u,  IGBT current:%u,  PlateTemperature:%u,  IGBTTemperature:%u, "
	       "noise det: %u, "
	       "pulse det:%u\n",
	       sensed[0], sensed[1], sensed[2], sensed[3],
	       gpio_read(&NoiseDetected),
	       gpio_read(&PulseDetected));
    delay_ms(500);
  }

}

int main_test(void) {
  init_system();
  /* test_led();			/\* pass *\/ */
  /* test_gpin();			/\* pass, but pulse det not connected to adc *\/ */
  /* test_adc();			/\* adc pass *\/ */
  /* test_pwm();			/\* pass *\/ */
  return 0;
}

int main(void){
  main_logic();
  /* main_test(); */
}
