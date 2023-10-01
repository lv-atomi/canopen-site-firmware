#include "rs485.h"
#include "gpio.h"
#include "log.h"
#include "rs232.h"
#include <stdint.h>


void init_rs485(RS485Port *devport) {
  ASSERT(devport);

  init_gpio_output(&devport->DE_nRE, GPIO_OUTPUT_PUSH_PULL, GPIO_DRIVE_STRENGTH_STRONGER);
  gpio_set(&devport->DE_nRE, RESET);
  init_rs232(&devport->port);
}

void rs485_transmit(RS485Port *devport, uint8_t *buf, uint16_t size) {
  ASSERT(devport);

  gpio_set(&devport->DE_nRE, SET);
  rs232_transmit(&devport->port, buf, size);
  gpio_set(&devport->DE_nRE, RESET);
}

bool_t rs485_receive(RS485Port *devport, uint8_t *buf, uint16_t size, uint32_t timeout) {
  ASSERT(devport);

  return rs232_receive(&devport->port, buf, size, timeout);
}

