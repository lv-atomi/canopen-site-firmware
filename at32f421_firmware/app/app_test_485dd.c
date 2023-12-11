#include <stdint.h>
#include <stdio.h>
#include "at32f421_board.h"
#include "at32f421_gpio.h"
#include "crc8.h"
#include "gpio.h"
#include "log.h"
#include "platform_base.h"
#include "rs232.h"
#include "timer.h"
#include "portdef_motor_direct_drive.h"
#include <stdlib.h>

#define GPIOLINK_HEAD 0xa7

RS232Port link = {
  .RX={GPIOA, GPIO_PINS_SOURCE3, GPIO_MUX_1},
  .controller = USART2,
  .baud_rate = 38400,
  .data_bit = USART_DATA_8BITS,
  .stop_bit = USART_STOP_1_BIT
};


int main(){
  uint8_t buf[10];
  nvic_priority_group_config(NVIC_PRIORITY_GROUP_4);
  system_clock_config();
  at32_board_init();
  Timer_Init();
  uart_print_init(115200);  
 
  init_rs232(&link);
  init_motor_brush(&mdd_motor);
  init_gpio_output(&mdd_led, GPIO_OUTPUT_OPEN_DRAIN, GPIO_DRIVE_STRENGTH_STRONGER);

  log_printf("start at32f421\n");
  
  uint8_t counter = 0;
  uint8_t counter_cycle = 20;
  int8_t speed = 0;
  
  while (1){
    uint8_t data, crc;
    counter_cycle = ((100-abs(speed)) / 2) + 2;
    /* led */
    if (counter < counter_cycle / 2){
      /* log_printf("led on, %d/%d\n", counter, counter_cycle); */
      gpio_set(&mdd_led, 0);
    } else {
      /* log_printf("led off, %d/%d\n", counter, counter_cycle); */
      gpio_set(&mdd_led, 1);
    }
    counter += 1;
    if (counter > counter_cycle) counter = 0;
    
    if(rs232_receive(&link, buf, 3, 50)){
      crc = compute_crc8(buf, 3);
      log_printf("recv: %02X %02X %02X, crc:%u\n", buf[0], buf[1], buf[2], crc);
      if ((crc == 0) && (buf[0] = GPIOLINK_HEAD)) {
	speed = (int8_t) buf[1];
	motor_set_speed(&mdd_motor, speed);
      }
    } else {
      /* log_printf("recv failed\n"); */
      /* delay_ms(500); */
    }
  }
}
