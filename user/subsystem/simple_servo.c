#include "simple_servo.h"
#include "at32f403a_407.h"
#include "at32f403a_407_debug.h"
#include "at32f403a_407_usb.h"
#include "gpio.h"
#include "log.h"
#include "motor.h"
#include "motor_485.h"
#include <stdint.h>
#include <stdlib.h>
#include "pid.h"
#include "pwm.h"
#include "stall_detect.h"
#include "timer.h"
#include "homing_logic.h"

#define MAX_TRACKER 10
#define INT_MAX    2147483648

Tracker tracker[MAX_TRACKER];
uint8_t tracker_num = 0;

bool_t tick_position(PositionTracker * pt);
bool_t tick_homing(HomingTracker *ht);

const char* HomingResultToString(enum HomingResult value) {
    return value == HR_HOMING_NOT_INITED ? "HR_HOMING_NOT_INITED" :
           value == HR_HIT_LIMIT_A       ? "HR_HIT_LIMIT_A"       :
           value == HR_HIT_LIMIT_B       ? "HR_HIT_LIMIT_B"       :
           value == HR_NO_MOVE           ? "HR_NO_MOVE"           :
           value == HR_NO_SENSE          ? "HR_NO_SENSE"          :
           value == HR_SUCCESS           ? "HR_SUCCESS"           :
                                           "INVALID_VALUE";
}

const char* MotorTypeToString(enum MotorType value) {
    return value == MT_Stepper        ? "MT_Stepper"        :
           value == MT_MotorBrush     ? "MT_MotorBrush"     :
           value == MT_MotorBrushless ? "MT_MotorBrushless" :
                                        "INVALID_VALUE";
}

const char* HomingDirectionToString(enum HomingDirection value) {
    return value == TOWARD_LIMIT_A ? "TOWARD_LIMIT_A" :
           value == TOWARD_LIMIT_B ? "TOWARD_LIMIT_B" :
                                     "INVALID_VALUE";
}

const char* LimitedStatusToString(enum LimitedStatus value) {
    return value == NOT_LIMITED ? "NOT_LIMITED" :
           value == LIMITED_A   ? "LIMITED_A"   :
           value == LIMITED_B   ? "LIMITED_B"   :
                                  "INVALID_VALUE";
}

const char* TrackerTypeToString(enum TrackerType value) {
    return value == TT_UNDEFINED ? "TT_UNDEFINED" :
           value == TT_POSITION  ? "TT_POSITION"  :
           value == TT_HOMING    ? "TT_HOMING"    :
                                    "INVALID_VALUE";
}

void start_homing(SimpleServo *devport, IOPort * port,
		  bool_t dir, uint32_t travel_limit){
  ASSERT(devport);
  ASSERT(tracker_num < MAX_TRACKER);
  tracker[tracker_num].tt = TT_HOMING;
  tracker[tracker_num].ht.status = HS_HOMING_START;
  tracker[tracker_num].ht.devport = devport;
  tracker[tracker_num].ht.signal = port;
  tracker[tracker_num].ht.cur_dir = dir;
  tracker[tracker_num].ht.soft_travel_limit = travel_limit;
  int32_t pos = ss_read_position(devport, TRUE, NULL);
  tracker[tracker_num].ht.start_pos = pos;
  tracker[tracker_num].ht.last_pos = pos;
  tracker[tracker_num].ht.last_tick = get_ticks();
  init_stall_detection(&tracker[tracker_num].sd,
		       SD_STOP_THRESHOLD,
		       SD_STOP_TICK_THRESHOLD,
		       SD_STALL_TICK_THRESHOLD);

  devport->_cur_position_available = FALSE;
  devport->_homing_result = HR_HOMING_NOT_INITED;
  devport->_cur_tracker = &tracker[tracker_num];
  log_printf("reg tracker, num:%d .root/ptr:%p\n", tracker_num, devport->_cur_tracker);
  timer_pause();
  tracker_num += 1;
  timer_resume();
}

void ss_set_stop(SimpleServo *devport){
  ASSERT(devport);

  switch (devport->_type) {
  case MT_MotorBrush:
  case MT_MotorBrushless:
    motor_set_speed(&devport->motor, 0);
    break;
  case MT_Stepper:
    /* map 0-100 speed to 0-1600 stepper speed */
    stepper_set_speed(&devport->stepper, 0, AccINF);
    break;
  }
}

void ss_set_speed(SimpleServo *devport, uint8_t speed, bool_t dir){
  int16_t stepper_speed;
  ASSERT(devport);
  
  switch(devport->_type){
  case MT_MotorBrush:
  case MT_MotorBrushless:
    motor_set_speed(&devport->motor, speed * (dir == 0 ? -1 : 1));
    break;
  case MT_Stepper:
    /* map 0-100 speed to 0-1600 stepper speed */
    stepper_speed = speed * 16 * (dir == 0 ? -1: 1);
    stepper_set_speed(&devport->stepper, stepper_speed, Acc5);
    break;
  default:
    ASSERT(0);			/* unknown motor type */
  }
}
 
int32_t ss_read_position(SimpleServo *devport, bool_t precise, uint32_t * delay) {
  int32_t pos;
  uint16_t ring;
  ASSERT(devport);

  switch(devport->_type){
  case MT_MotorBrush:
  case MT_MotorBrushless:
    ASSERT(devport->displace.spi.controller);
    do {
      pos = read_displacement(&devport->displace, delay, TRUE);
      if (delay) {
	log_printf("pos:%ld delay:%ld\n", pos, *delay);
      }
    } while((delay) && (*delay > 50) && precise);
    return pos;
  case MT_Stepper:
    /* map 0-100 speed to 0-1600 stepper speed */
    stepper_read_position(&devport->stepper, &pos, &ring);
    return pos;			/* FIXME */
  default:
    ASSERT(0);			/* unknown motor type */
    return 0;
  }
}

void ss_set_ori(SimpleServo *devport){ /* set current position to origin */
  int32_t pos = ss_read_position(devport, TRUE, NULL);
  devport->homed_position = pos;
}

void tick_simple_servo(){
  uint8_t i, j;
  bool_t finished;
  for (i=0; i<tracker_num; i++){
    finished = FALSE;
    switch(tracker[i].tt){
    case TT_UNDEFINED:		/* should never happen */
      ASSERT(0);
      break;
    case TT_POSITION:
      finished = tick_position(&tracker[i].pt);
      break;
    case TT_HOMING:
      log_printf("tracker num:%d, .root/ptr:%p, .ht/ptr:%p\n", i, &tracker[i], &tracker[i].ht);
      finished = tick_homing(&tracker[i].ht);
      break;
    }

    if (finished) {	  /* remove entry */
      SimpleServo *devport = tracker[i].tt == TT_POSITION ? tracker[i].pt.devport :\
	tracker[i].tt == TT_HOMING ? tracker[i].ht.devport : NULL;
      if (devport) {
	devport->_cur_tracker = NULL;
      }
      for (j=i+1; j<tracker_num; j++){
	tracker[j-i-1 + i] = tracker[j];
      }
      tracker_num -= 1;
      i--;			/* restart this loop */
    }
  }
}

bool_t tick_position(PositionTracker * pt){ /* return true => track finished */
  ASSERT(pt);
  uint32_t delay = 0;
  int32_t cur_pos = ss_read_position(pt->devport, FALSE, &delay);
  uint32_t cur_tick = get_ticks();
  pt->devport->_cur_position = cur_pos;
  /* uint32_t tick_pass = cur_tick - pt->start_ticks; */
  /* int32_t cur_target_pos = (pt->target_pos - pt->start_pos) * tick_pass / (pt->target_ticks - pt->start_ticks); */
  double velocity_delta = pid_update(&pt->pid_ins, cur_pos);
  double cur_speed = pt->cur_speed;
  cur_speed += velocity_delta;
  cur_speed = min(cur_speed, 100);
  cur_speed = max(cur_speed, -100);
  pt->cur_speed = cur_speed;
  ss_set_speed(pt->devport, abs(pt->cur_speed), pt->cur_speed <= 0 ? 0 : 1);

  if (abs(pt->target_pos - cur_pos) < 0.1 && (cur_tick > pt->target_ticks)){
    /* position track ends */
    ss_set_stop(pt->devport);
    pt->devport->_cur_position = cur_pos;
    return TRUE;
  }

  return FALSE;
}

void init_simple_servo(SimpleServo *devport, uint8_t homing_speed) {
  ASSERT(devport);
  ASSERT(devport->_homing_speed <= 100);

  if (devport->motor.brush.pwm_a.port.port || devport->motor.brushless.speed_sense.port.port) { /* motor defined */
    ASSERT(devport->stepper.port.port.RX.port == 0); /* motor_unified & stepper are exclusive  */
    if (devport->motor.is_brushless) { 
      init_motor_brushless(&devport->motor);
      devport->_type = MT_MotorBrushless;
    } else {
      init_motor_brush(&devport->motor);
      devport->_type = MT_MotorBrush;
    }
  } else {
    ASSERT(devport->stepper.port.port.RX.port); /* must define stepper */
    init_motor_485(&devport->stepper);
    devport->_type = MT_Stepper;
  }
  devport->_homing_speed = homing_speed;

  /* init limit switch */
  if (devport->limit_a.port) init_gpio_input(&devport->limit_a, GPIO_PULL_UP, GPIO_DRIVE_STRENGTH_STRONGER);
  if (devport->limit_b.port) init_gpio_input(&devport->limit_b, GPIO_PULL_UP, GPIO_DRIVE_STRENGTH_STRONGER);
  if (devport->limit_homing.port) init_gpio_input(&devport->limit_homing, GPIO_PULL_UP, GPIO_DRIVE_STRENGTH_STRONGER);

  if (devport->_type != MT_Stepper)
    ASSERT(devport->displace.spi.controller); /* for non stepper, displace measurement must defined */
  if (devport->displace.spi.controller)
    init_capacitor_displacement_measurement(&devport->displace);

  timer_add_tick(tick_simple_servo, 500);
}

void ss_home(SimpleServo *devport, bool_t to_homing) {
  bool_t dir = 0;		/* dir=0 => toward limit_a; dir=1 -> toward limit_b */
  uint32_t travel_limit = 0;
  int32_t cur_pos = 0;
  ASSERT(devport);
  if (to_homing)
    ASSERT(devport->limit_homing.port); /* homing signal must defined */
  dir = devport->homing_direction == TOWARD_LIMIT_A ? 0 : 1;
  dir = devport->invert_direction ? !dir: dir;
  dir = devport->invert_limit_ab ? !dir : dir;

  if (devport->_homing_result == HR_SUCCESS)
    cur_pos = ss_read_position(devport, FALSE, NULL);
  uint32_t max_soft_limit = INT_MAX;
  if (devport->soft_limit_b)
    max_soft_limit = devport->soft_limit_b - devport->soft_limit_a;
  
  travel_limit = devport->homing_max_travel ? devport->homing_max_travel : \
    (dir == 1) && (devport->soft_limit_b) ? 10 + min(devport->soft_limit_b - cur_pos, max_soft_limit) : \
    (dir == 0) ? 10 + min(cur_pos - devport->soft_limit_a, max_soft_limit) : \
    100;
  start_homing(devport,
	       to_homing ? &devport->limit_homing :	\
	       dir ? &devport->limit_b : &devport->limit_a,
	       dir,
	       travel_limit);
}

void ss_go_position_absolute(SimpleServo * devport, int32_t position, uint32_t speed_F) {
  ASSERT(devport);
  ASSERT(devport->_homing_result == HR_SUCCESS);
  ASSERT(tracker_num < MAX_TRACKER);

  log_printf("go position abs:%ld F:%ld\n", position, speed_F);
  
  tracker[tracker_num].tt = TT_POSITION;
  pid_init(&tracker[tracker_num].pt.pid_ins, position, 1.0, 0.1, 0.01);
  int32_t pos;
  if (!devport->_cur_position_available) {
    pos = ss_read_position(devport, TRUE, NULL);
    devport->_cur_position = pos;
    devport->_cur_position_available = TRUE;
  } else {
    pos = devport->_cur_position_available;
  }
  uint32_t cur_tick = get_ticks();
  tracker[tracker_num].pt.devport = devport;
  tracker[tracker_num].pt.cur_speed = position / abs(position);
  tracker[tracker_num].pt.start_pos = pos;
  tracker[tracker_num].pt.start_ticks = cur_tick;
  tracker[tracker_num].pt.target_pos = position;
  tracker[tracker_num].pt.target_ticks = cur_tick + (position - pos) * 60 * 10 / speed_F;

  devport->_cur_tracker = &tracker[tracker_num];
  
  timer_pause();
  tracker_num += 1;
  timer_resume();

  /* set_ss_speed(devport, 1, dir); */
}

void ss_go_position_relative(SimpleServo *devport, int32_t position_delta, uint32_t speed_F) {
  ASSERT(devport);
}

int32_t ss_current_position(SimpleServo *devport) {
  return 0;
}

bool_t ss_is_homing(SimpleServo *devport){
  return devport->_homing_result == HR_HOMING_NOT_INITED;
}

bool_t ss_is_moving(SimpleServo *devport) {
  return devport->_cur_tracker != NULL;
}
enum LimitedStatus ss_is_limited(SimpleServo *devport) {
  return NOT_LIMITED;
}
