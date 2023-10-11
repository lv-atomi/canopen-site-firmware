#include <stdio.h>
#include "at32f421_gpio.h"
#include "adc.h"
#include "gpio.h"
#include "motor.h"
#include "platform_base.h"

ADCPort senses[4];

int main(){
  nvic_priority_group_config(NVIC_PRIORITY_GROUP_4);
  system_clock_config();
  at32_board_init();
  uart_print_init(115200);  
  
  adc_config(senses, 2);
  while(1) {
    adc_ordinary_software_trigger_enable(ADC1, TRUE);
    uint16_t * sensed = read_adcs(senses, 2);
    printf("sense1:%u sense2:%u\n", sensed[0], sensed[1]);
    delay_ms(1000);
  }
}


