#include "at32f403a_407_gpio.h"
#include "at32f403a_407_usart.h"
#include "board.h"
#include "motor_485.h"
#include "rs232.h"
#include "rs485.h"
#include "timer.h"
#include "log.h"
#include <stdint.h>
#include <stdlib.h>
#include "motor.h"
#include "crc8.h"
#include "cap_displacement.h"
#include "laser_displacement.h"


#define GPIOLINK_HEAD 0xa7


RS232Port motor_485dd={
  .TX={GPIOA, GPIO_PINS_SOURCE2},
  .controller=USART2,
  .baud_rate=38400,
  .data_bit=USART_DATA_8BITS,
  .stop_bit=USART_STOP_1_BIT
};

int32_t distance;

CapacitorDisplacementMeasurePort displacement_test = {
  .spi={
    .miso = {GPIOB, GPIO_PINS_SOURCE15},
    .clk = {GPIOB, GPIO_PINS_SOURCE13},
    .controller = SPI2,
    .init_type = {
      .transmission_mode = SPI_TRANSMIT_SIMPLEX_RX,
      .master_slave_mode = SPI_MODE_SLAVE,
      .mclk_freq_division = SPI_MCLK_DIV_32,
      .first_bit_transmission = SPI_FIRST_BIT_LSB,
      .frame_bit_num = SPI_FRAME_8BIT,
      .clock_polarity = SPI_CLOCK_POLARITY_HIGH,
      .clock_phase = SPI_CLOCK_PHASE_2EDGE,
      .cs_mode_selection = SPI_CS_SOFTWARE_MODE,
    }
  }
};

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

void init_motor_485dd(RS232Port *port) {
  init_rs232(port);
  generate_crc8_table();
}

void send_byte_485dd(RS232Port *port, uint8_t data){
  uint8_t buf[3] = {GPIOLINK_HEAD, data, 0};
  buf[2] = compute_crc8(buf, 2); // 假设compute_crc8函数已经定义

  rs232_transmit(port, buf, 3);
}

int main(void) {
  system_clock_config(); // 8M HSE
  at32_board_init();
  Timer_Init();
  uart_print_init(115200);	/* init debug uart */

  log_printf("start\n");
  
  /* init_motor_brushless(&motor_burshless);   */
  init_motor_485dd(&motor_485dd);
  init_laser_displacement(&ld);
  laser_displacement_measure_continously(&ld);
  init_capacitor_displacement_measurement(&displacement_test);
  
  nvic_priority_group_config(NVIC_PRIORITY_GROUP_4);
  nvic_irq_enable(USART3_IRQn, 2, 1);
  usart_interrupt_enable(ld.port.uart_controller.port.controller, USART_RDBF_INT, TRUE);
  
  int8_t speed = 0;
  int8_t dir = 1;
  
  while (1) {
    uint32_t delay = 0;

    if (speed >=98) dir = -1;
    else if(speed <=-98) dir = 1;
    speed += dir;

    /* motor_set_speed(&motor_burshless, speed); */
    send_byte_485dd(&motor_485dd, speed);
    //printf("tick:%ld\n", get_ticks());
    int32_t pos = read_displacement(&displacement_test, &delay, 1);
    log_printf("motor:%d laser_distance: %ld cap_distance: %ld, delay:%ld\n", speed, distance, pos, delay);
    delay_ms(100);
  }
}
