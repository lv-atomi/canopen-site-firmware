#include "board.h"
#include "laser_displacement.h"
#include "rs485.h"
#include "timer.h"
#include "log.h"
#include <stdint.h>
#include <stdlib.h>
#include "motor.h"

MotorUnified motor_burshless = {
    .brushless = {.direction = {GPIOB, GPIO_PINS_SOURCE0},
                  .speed_sense =
                      {
                          .port = {GPIOA, GPIO_PINS_SOURCE10},
                          .tmr = TMR1,
                          .channel = TMR_SELECT_CHANNEL_3,
                          .complementary = FALSE,
                      },
                  .speed_set =
                      {
                          .port = {GPIOB, GPIO_PINS_SOURCE1},
                          .tmr = TMR3,
                          .channel = TMR_SELECT_CHANNEL_4,
                          .complementary = FALSE,
                      }},
    .is_brushless = 1};

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
  
  init_motor_brushless(&motor_burshless);  
  init_laser_displacement(&ld);
  laser_displacement_measure_continously(&ld);
  
  nvic_priority_group_config(NVIC_PRIORITY_GROUP_4);
  nvic_irq_enable(USART3_IRQn, 2, 1);
  usart_interrupt_enable(ld.port.uart_controller.port.controller, USART_RDBF_INT, TRUE);
  
  int8_t speed = 0;
  int8_t dir = 1;
  
  while (1) {
    if (speed >=98) dir = -1;
    else if(speed <=-98) dir = 1;
    speed += dir;
    
    motor_set_speed(&motor_burshless, speed);
    /* gpio_set(&motor_brush.brush.pwm_a.port, dir); */
    /* gpio_set(&motor_brush.brush.pwm_b.port, !dir); */
    //printf("tick:%ld\n", get_ticks());

    log_printf("motor:%d distance: %ld\n", speed, distance);
    delay_ms(100);
  }
}
