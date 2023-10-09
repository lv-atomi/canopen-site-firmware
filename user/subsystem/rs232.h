#ifndef RS232_H
#define RS232_H

#include "gpio.h"
#include "board.h"
#include <stdint.h>

#define MIN_RECV_TIMEOUT 5  // 5ms 作为小的超时时间，可以根据实际情况调整

typedef struct {
  IOPort TX, RX;
  usart_type * controller;
  uint32_t baud_rate;
  usart_data_bit_num_type data_bit;
  usart_stop_bit_num_type stop_bit;
} RS232Port;

void init_rs232(RS232Port *);
void rs232_transmit(RS232Port *, uint8_t * buf, uint16_t size);
bool_t rs232_receive(RS232Port *, uint8_t * buf, uint16_t size, uint32_t timeout);
uint16_t rs232_receive_variable(RS232Port *devport, uint8_t *buf,
                                uint16_t *max_size, uint16_t timeout);
void rs232_flush(RS232Port *devport);

#endif
