#include <stdint.h>
#include <stdio.h>
#include "gpiolink_crc8.h"
#include "log.h"
#include "platform_base.h"

gpiolink link ={
  .port={GPIOA, GPIO_PINS_SOURCE6}
};


int main(){
  nvic_priority_group_config(NVIC_PRIORITY_GROUP_4);
  system_clock_config();
  at32_board_init();
  uart_print_init(115200);  
  
  init_gpiolink(&link, FALSE);
  log_printf("start at32f421\n");
  while (1){
    uint8_t data;
    if(gpiolink_slave_recv_byte(&link, &data)){
      log_printf("recv: %u\n", data);
    } else{
      log_printf("recv failed\n");
      delay_ms(100);
    }
  }
}

