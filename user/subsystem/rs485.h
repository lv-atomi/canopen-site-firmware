#ifndef RS485_H
#define RS485_H

#include "board.h"
#include "rs232.h"
#include <stdint.h>

typedef struct {
  IOPort DE_nRE;
  RS232Port port;
} RS485Port;

void init_rs485(RS485Port *);
void rs485_transmit(RS485Port *, uint8_t * buf, uint16_t size);
bool_t rs485_receive(RS485Port *, uint8_t * buf, uint16_t size, uint32_t timeout);

#endif
