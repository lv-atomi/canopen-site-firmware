#ifndef MOTOR_485_H
#define MOTOR_485_H

#include "motor.h"
#include "rs485.h"
#include <stdint.h>


enum Acceleration {
  AccINF,
  Acc1,
  Acc2,
  Acc3,
  Acc4,
  Acc5,
  Acc6,
  Acc7,
  Acc8,
  Acc9,
  Acc10,
  Acc11,
  Acc12,
  Acc13,
  Acc14,
  Acc15,
  Acc16,
  Acc17,
  Acc18,
  Acc19,
  Acc20,
  Acc21,
  Acc22,
  Acc23,
  Acc24,
  Acc25,
  Acc26,
  Acc27,
  Acc28,
  Acc29,
  Acc30,
  Acc31,
  Acc32};

typedef union {
  struct __attribute__((__packed__)){
    uint8_t func_code;
    uint8_t dir_speed_high;
    uint8_t speed_low;
    uint8_t acceleration;
    uint32_t position;
  };
  uint8_t raw[8];
} position_cmd;

typedef union {
  struct __attribute__((__packed__)){
    uint8_t func_code;
    uint8_t dir_speed_high;
    uint8_t speed_low;
    uint8_t acceleration;
  };
  uint8_t raw[4];
} speed_cmd;

typedef union {
  struct __attribute__((__packed__)){
    uint8_t func_code;
    int32_t position;
    uint16_t ring;
  };
  uint8_t raw[7];
} read_encoder_response;

typedef struct {
  RS485Port port;
  uint8_t addr;
  IOPort limit_left;
  IOPort limit_right;
  int32_t cur_pos;
  bool_t homed;
} Motor485;

typedef enum{
  HOMING_LEFT,
  HOMING_RIGHT
} HOMING_DIR;

void init_motor_485(Motor485 *);
void stepper_set_speed(Motor485 *, int16_t speed, enum Acceleration acc);
void stepper_go_pos(Motor485 *, int32_t pos, uint16_t speed, enum Acceleration acc);
void stepper_read_position(Motor485 *devport, int32_t * pos, uint16_t * ring);

#endif
