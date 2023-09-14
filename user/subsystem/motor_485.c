#include "motor_485.h"
#include "gpio.h"
#include "log.h"
#include "rs485.h"
#include <stdint.h>
#include <stdlib.h>

uint32_t tobe32(uint32_t e){
  union {
    uint32_t i;
    char c[4];
  } result;
  result.c[0] = (e & 0xFF000000) >> 24;
  result.c[1] = (e & 0x00FF0000) >> 16;
  result.c[2] = (e & 0x0000FF00) >> 8;
  result.c[3] = (e & 0x000000FF);
  return result.i;
}

void init_motor_485(Motor485 *devport) {
  ASSERT(devport);

  init_rs485(&devport->port);
  if (devport->limit_left.port != NULL)
    init_gpio_input(&devport->limit_left, GPIO_PULL_UP, GPIO_DRIVE_STRENGTH_STRONGER);
  if (devport->limit_right.port != NULL)
    init_gpio_input(&devport->limit_right, GPIO_PULL_UP, GPIO_DRIVE_STRENGTH_STRONGER);
  devport->homed = FALSE;
  devport->cur_pos = 0;
}

void send_cmd(Motor485 *devport, uint8_t *buf, uint8_t size){
  static uint8_t head[] = {0xfa, 0};
  head[1] = devport->addr;
  uint8_t crc = 0;
  uint8_t i;

  for (i=0; i<2; i++)
    crc = (crc + head[i]) & 0xff;
  for (i=0; i<size; i++)
    crc = (crc + buf[i]) & 0xff;
  
  rs485_transmit(&devport->port, head, 2);
  rs485_transmit(&devport->port, buf, size);
  rs485_transmit(&devport->port, &crc, 1);
}

void motor_485_homing(Motor485 *devport, HOMING_DIR dir) { 
  ASSERT(devport);
  
}

void motor_485_set_soft_limit(Motor485 *devport, int32_t min, int32_t max) {
  ASSERT(devport);
  
}

/*
  pos: 脉冲数，范围0 - 0x7FFFFFFF，正负代表方向
  speed: 速度，范围0-1600，0为停止
  acc: 加速度，0-32，越大加速度越高，AccINF(0)表示电机直接以设定的速度运行（可以理解为加速度无穷大）
*/
void motor_go_pos(Motor485 *devport, int32_t pos, uint16_t speed, enum Acceleration acc) {
  ASSERT(devport);
  ASSERT(speed>=0);
  ASSERT(speed<=1600);

  int32_t delta_pos = pos - devport->cur_pos;
  uint8_t dir_bit = delta_pos >= 0 ? 0 : 0b10000000;
  uint8_t speed_high = (speed >> 8) & 0b1111;
  uint8_t speed_low = speed & 0xff;

  position_cmd cmd = {
    .func_code=0xfd,
    .dir_speed_high=dir_bit | speed_high,
    .speed_low=speed_low,
    .acceleration=acc,
    .position=tobe32(abs(delta_pos))
  };

  send_cmd(devport, cmd.raw, sizeof(position_cmd));
}
