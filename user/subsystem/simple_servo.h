#ifndef SIMPLE_SERVO_H
#define SIMPLE_SERVO_H

#include "cap_displacement.h"
#include "gpio.h"
#include "motor_485.h"
#include "motor.h"
#include "pwm.h"
#include "pid.h"
#include "stall_detect.h"

enum MotorType { MT_Stepper, MT_MotorBrush, MT_MotorBrushless };
enum HomingDirection{
  TOWARD_LIMIT_A,
  TOWARD_LIMIT_B
};
enum HomingResult{
  HR_HOMING_NOT_INITED,		/* 初始化 */
  HR_HIT_LIMIT_A,		/* 撞到了限位a */
  HR_HIT_LIMIT_B,		/* 撞到了限位b */
  HR_NO_MOVE,			/* 完全没有移动 */
  HR_NO_SENSE,			/* 限定范围内没触发信号 */
  HR_SUCCESS,			/* Homing成功 */
};
enum LimitedStatus{
  NOT_LIMITED,
  LIMITED_A,
  LIMITED_B,
};
enum HomingStatus{
  HS_HOMING_START,		/* 开始归零 */
  HS_HOMING_ENGAGE,		/* 驱动电机 */
  HS_HOMING_HIT_ENDSTOP1,	/* 首次遇到限位 */
  HS_HOMING_REVERT_ENGAGE,	/* 反向离开限位 */
  HS_HOMING_ENGAGE_AGAIN,	/* 再次反向，朝向限位运动 */
  HS_HOMING_HIT_ENDSTOP2,	/* 第二次遇到限位 */
};
enum TrackerType{
  TT_UNDEFINED,
  TT_POSITION,
  TT_HOMING,
};
typedef struct _Tracker Tracker;

typedef struct {
  MotorUnified motor;
  Motor485 stepper;
  IOPort limit_a, limit_b, limit_homing;
  CapacitorDisplacementMeasurePort displace;
  bool_t invert_direction;
  bool_t invert_limit_ab;
  enum HomingDirection homing_direction;
  int32_t homed_position;
  int32_t homing_max_travel;
  int32_t soft_limit_a;
  int32_t soft_limit_b;
  
  /* internal attr */
  Tracker * _cur_tracker;
  enum MotorType _type;
  uint8_t _homing_speed;
  bool_t _cur_position_available;
  int32_t _cur_position;
  enum HomingResult _homing_result;
} SimpleServo;

typedef struct {
  enum HomingStatus status;
  SimpleServo * devport;
  IOPort * signal;
  bool_t cur_dir;
  int32_t start_pos;
  uint32_t soft_travel_limit;

  int32_t last_pos;
  uint32_t last_tick;
} HomingTracker;

typedef struct{
  SimpleServo * devport;
  int8_t cur_speed;
  int32_t start_pos;
  uint32_t start_ticks;
  int32_t target_pos;
  uint32_t target_ticks;
  PIDController pid_ins;
} PositionTracker;

typedef struct _Tracker{
  enum TrackerType tt;
  StallDetect sd;
  union{
    PositionTracker pt;
    HomingTracker ht;
  };
} Tracker;

void init_simple_servo(SimpleServo *devport, uint8_t homing_speed);
void ss_home(SimpleServo * devport, bool_t to_homing);
/* position unit: 0.01mm, speed_F */
void ss_go_position_absolute(SimpleServo * devport, int32_t position, uint32_t speed_F);
void ss_go_position_relative(SimpleServo * devport, int32_t position_delta, uint32_t speed_F);
int32_t ss_current_position(SimpleServo * devport);
bool_t ss_is_homing(SimpleServo *devport);
bool_t ss_is_moving(SimpleServo * devport);

int32_t ss_read_position(SimpleServo *devport, bool_t precise, uint32_t *delay);
void ss_set_speed(SimpleServo *devport, uint8_t speed, bool_t dir);
void ss_set_ori(SimpleServo *devport);  /* set current position to origin */
void ss_set_stop(SimpleServo *devport);

enum LimitedStatus ss_is_limited(SimpleServo * devport);

#endif
