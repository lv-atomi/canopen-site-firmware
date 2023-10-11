#include <stdio.h>
#include "at32f421_gpio.h"
#include "adc.h"
#include "gpio.h"
#include "motor.h"
#include "platform_base.h"

IOPort gpin[2] = {{GPIOB, GPIO_PINS_SOURCE6}, {GPIOB, GPIO_PINS_SOURCE7}};

int main(){
  nvic_priority_group_config(NVIC_PRIORITY_GROUP_4);
  system_clock_config();
  at32_board_init();
  uart_print_init(115200);  
  
  uint8_t num_ports = sizeof(gpin) / sizeof(gpin[0]);
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

