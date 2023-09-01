#ifndef I2C_H
#define I2C_H

#include "board.h"
#include <stdint.h>
#include "gpio.h"

#define BUF_SIZE 8

typedef struct {
  IOPort clk, data;
  i2c_type * controller;
  uint8_t address;
  uint8_t tx_buf[BUF_SIZE];
  uint8_t rx_buf[BUF_SIZE];
} I2CPort;

void init_i2c(I2CPort *port, uint32_t speed);
void init_i2c_soft(I2CPort *port);

#endif
