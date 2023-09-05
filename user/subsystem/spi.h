#ifndef SPI_H
#define SPI_H

#include "board.h"
#include <stdint.h>
#include "gpio.h"

#define BUF_SIZE 8
#define MAX_SPI_PORT 4

typedef struct {
  IOPort miso, mosi, clk, cs;
  spi_type * controller;
  spi_init_type init_type;
  uint8_t tx_buf[BUF_SIZE];
  uint8_t rx_buf[BUF_SIZE];
} SPIPort;

void init_spi(SPIPort *devport);
uint8_t get_spi_index(SPIPort *dev);
void enable_spi_irq(spi_type * dev, uint32_t preempt_priority, uint32_t sub_priority);

#endif
