#include "board.h"
#include "laser_displacement.h"
#include "rs485.h"
#include "timer.h"
#include "log.h"
#include <stdint.h>
#include <stdlib.h>


int32_t distance;

LaserDisplacement ld = {
    .port = {.slave_address = 1,
             .uart_controller = {.DE_nRE = {GPIOB, GPIO_PINS_SOURCE12},
                                 .port = {.TX = {GPIOB, GPIO_PINS_SOURCE10},
                                          .RX = {GPIOB, GPIO_PINS_SOURCE11},
                                          .controller = USART3,
                                          .baud_rate = 115200,
                                          .data_bit = USART_DATA_8BITS,
                                          .stop_bit = USART_STOP_1_BIT}}}};

void USART3_IRQHandler(void) {
  if(USART3->ctrl1_bit.rdbfien != RESET) {
    if(usart_flag_get(USART3, USART_RDBF_FLAG) != RESET) {
      usart_interrupt_enable(ld.port.uart_controller.port.controller, USART_RDBF_INT, FALSE);
      if(laser_displacement_get_distance(&ld, false, true, &distance)){
      } else {
	distance = -1;
      }
      usart_interrupt_enable(ld.port.uart_controller.port.controller, USART_RDBF_INT, TRUE);
    }
  }
}


int main(void) {
  system_clock_config(); // 8M HSE
  at32_board_init();
  Timer_Init();
  uart_print_init(115200);	/* init debug uart */

  log_printf("start\n");

  init_laser_displacement(&ld);
  /* LaserDisplacement_SetBaudRate(&ld, BAUD_RATE_38400); */
  laser_displacement_measure_continously(&ld);
  
  nvic_priority_group_config(NVIC_PRIORITY_GROUP_4);
  nvic_irq_enable(USART3_IRQn, 2, 1);
  usart_interrupt_enable(ld.port.uart_controller.port.controller, USART_RDBF_INT, TRUE);
  uint8_t response[MODBUS_MAX_MESSAGE_LENGTH];
  uint16_t max_size = MODBUS_MAX_MESSAGE_LENGTH;
  while (1) {
    /* gpio_set(&motor_brush.brush.pwm_a.port, dir); */
    /* gpio_set(&motor_brush.brush.pwm_b.port, !dir); */
    //printf("tick:%ld\n", get_ticks());

    /* rs485_receive_variable(&ld.port.uart_controller, */
    /* 			   response, &max_size, 10); */
    /* rs485_flush(&ld.port.uart_controller); */
    /* rs485_receive(&ld.port.uart_controller, */
    /* 		  response, 9, 0); */
    /* DumpHex(response, 9); */

    //distance = laser_displacement_get_distance(&ld, false, true);
    /* if (laser_displacement_get_distance(&ld, false, true, &distance)){ */
    /*   log_printf("distance: %ld\n", distance); */
    /* } else { */
    /*   log_printf("distance: inf\n"); */
    /* } */
    log_printf("distance: %ld\n", distance);
    delay_ms(100);
  }
}
