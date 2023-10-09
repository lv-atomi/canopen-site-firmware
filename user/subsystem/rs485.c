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

void rs485_flush(RS485Port *devport) {
  ASSERT(devport);

  rs232_flush(&devport->port);
}

void rs485_transmit(RS485Port *devport, uint8_t *buf, uint16_t size) {
  ASSERT(devport);

  gpio_set(&devport->DE_nRE, SET);
  
  // 1. 清空输入缓冲区
  rs485_flush(devport); 

  // 2. 延时一段时间（这里假设我们有一个名为"delay_us"的微秒延时函数）
  uint32_t char_time_us = (1000000 / devport->port.baud_rate) * 10; // 单字符时间 (10位：1起始，8数据，1停止)
  delay_us(char_time_us * 3.5);
  
  rs232_transmit(&devport->port, buf, size);

  gpio_set(&devport->DE_nRE, RESET);
}

bool_t rs485_receive(RS485Port *devport, uint8_t *buf, uint16_t size, uint32_t timeout) {
  ASSERT(devport);

  return rs232_receive(&devport->port, buf, size, timeout);
}


uint16_t rs485_receive_variable(RS485Port *devport, uint8_t *buf,
                                uint16_t *max_size, uint16_t timeout) {
  ASSERT(devport);
  return rs232_receive_variable(&devport->port, buf, max_size, timeout);
}
