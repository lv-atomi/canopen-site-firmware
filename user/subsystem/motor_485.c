#include "motor_485.h"
#include "log.h"
#include <stdint.h>
#include <stdlib.h>

uint32_t tobe32u(uint32_t e){
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

int32_t frombe32(const char *e) {
  return ((int32_t)e[0] << 24) |
          ((int32_t)(unsigned char)e[1] << 16) |
          ((int32_t)(unsigned char)e[2] << 8) |
          (int32_t)(unsigned char)e[3];
}

uint16_t frombe16u(const char *e) {
  return ((uint16_t)(unsigned char)e[2] << 8) |
          (uint16_t)(unsigned char)e[3];
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

void read_response(Motor485 *devport, uint8_t *buf, uint8_t size){
  static uint8_t head[] = {0xfb, 0};

  rs485_receive(&devport->port, head, 2, 0);
  ASSERT(head[0] == 0xfb);
  ASSERT(head[1] == devport->addr);
  
  uint8_t crc = 0, crc_read=0;
  uint8_t i;

  rs485_receive(&devport->port, buf, size, 0);
  
  for (i=0; i<2; i++)
    crc = (crc + head[i]) & 0xff;
  for (i=0; i<size; i++)
    crc = (crc + buf[i]) & 0xff;
  
  rs485_receive(&devport->port, &crc_read, 1, 0);
  ASSERT(crc = crc_read);
}

void stepper_set_speed(Motor485 *devport, int16_t speed, enum Acceleration acc){
  ASSERT(devport);
  ASSERT(speed>=0);
  ASSERT(speed<=1600);

  uint8_t dir_bit = speed >= 0 ? 0 : 0b10000000;
  uint8_t speed_high = (speed >> 8) & 0b1111;
  uint8_t speed_low = speed & 0xff;
  
  speed_cmd cmd = {
    .func_code=0xf6,
    .dir_speed_high=dir_bit | speed_high,
    .speed_low=speed_low,
    .acceleration=acc,
  };

  send_cmd(devport, cmd.raw, sizeof(speed_cmd));
}

/*
  pos: 脉冲数，范围0 - 0x7FFFFFFF，正负代表方向
  speed: 速度，范围0-1600，0为停止
  acc: 加速度，0-32，越大加速度越高，AccINF(0)表示电机直接以设定的速度运行（可以理解为加速度无穷大）
*/
void stepper_go_pos(Motor485 *devport, int32_t pos, uint16_t speed, enum Acceleration acc) {
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
    .position=tobe32u(abs(delta_pos))
  };

  send_cmd(devport, cmd.raw, sizeof(position_cmd));
}

void stepper_read_position(Motor485 *devport, int32_t * pos, uint16_t * ring){
  ASSERT(devport);

  /* uint8_t speed_high = (speed >> 8) & 0b1111; */
  /* uint8_t speed_low = speed & 0xff; */
  uint8_t read_pos_cmd = 0x30;
  read_encoder_response response;

  send_cmd(devport, &read_pos_cmd, sizeof(read_pos_cmd));
  read_response(devport, response.raw, sizeof(response));
  ASSERT(pos);
  ASSERT(ring);
  
  *pos = frombe32((char*)&response.position);
  *ring = frombe16u((char*)&response.ring);
}
