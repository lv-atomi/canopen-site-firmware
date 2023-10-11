#include <stdio.h>
#include "at32f421_gpio.h"
#include "adc.h"
#include "gpio.h"
#include "motor.h"
#include "platform_base.h"

IOPort gpout[2] = {{GPIOA, GPIO_PINS_SOURCE3}, {GPIOC, GPIO_PINS_SOURCE6}};

int main(){
  nvic_priority_group_config(NVIC_PRIORITY_GROUP_4);
  system_clock_config();
  at32_board_init();
  uart_print_init(115200);
  
  uint8_t num_ports = sizeof(gpout) / sizeof(gpout[0]);
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

