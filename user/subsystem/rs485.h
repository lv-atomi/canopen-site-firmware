#ifndef RS485_H
#define RS485_H

#include "CO_driver_target.h"
#include "gpio.h"
#include "board.h"
#include <stdint.h>

typedef struct {
  IOPort DE_nRE;
  IOPort TX, RX;
  usart_type * controller;
  uint32_t baud_rate;
  usart_data_bit_num_type data_bit;
  usart_stop_bit_num_type stop_bit;
} RS485Port;

void init_rs485(RS485Port *);
void rs485_transmit(RS485Port *, uint8_t * buf, uint16_t size);
bool_t rs485_receive(RS485Port *, uint16_t * buf, uint16_t size, uint32_t timeout);

#endif
