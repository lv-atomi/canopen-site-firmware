#include "homing_logic.h"
#include <stdint.h>
#include <stdlib.h>
#include "gpio.h"
#include "log.h"
#include "simple_servo.h"
#include "timer.h"

#define MAX_WAITING_MS  5000
#define MIN_MOVE_THRESHOLD 50 /* 0.5 mm */
#define REVERT_TIME 1000      /* 1000ms */
#define REVERT_DISTANCE 1     /* 1mm */

/* bool_t stage13_do_hit_endswitch(HomingTracker * ht){ */
/*   ASSERT(ht); */

/*   ht->cur_dir = !ht->cur_dir; */
/*   ss_set_speed(ht->devport, ht->devport->_homing_speed, ht->cur_dir); */
/*   if (ht->status == HS_HOMING_STAGE1) */
/*     ht->status = HS_HOMING_STAGE2; */
/*   else { /\* finish homing *\/ */
/*     ht->devport->_homing_result = HR_SUCCESS; */
/*     ss_set_stop(ht->devport); */
/*     ss_set_ori(ht->devport); */
/*     return TRUE; */
/*   } */
/*   return FALSE; */
/* } */

/* bool_t stage2_do_release_endswitch(HomingTracker * ht){ */
/*   ASSERT(ht); */

/*   ht->cur_dir = !ht->cur_dir; */
/*   ss_set_speed(ht->devport, */
/* 	       max(ht->devport->_homing_speed / 5, 1), /\* speed cannot be zero *\/ */
/* 	       ht->cur_dir); */
/*   ht->status = HS_HOMING_STAGE3; */
/*   return FALSE; */
/* } */

bool_t tick_homing_with_signal_sensor_available(HomingTracker * ht) {
  ASSERT(ht);

  switch (ht->status) {
  case HS_HOMING_START:
    ss_set_speed(ht->devport, ht->devport->_homing_speed, ht->cur_dir);
    ht->status = HS_HOMING_ENGAGE;
    break;
  case HS_HOMING_ENGAGE:
    if (gpio_read(ht->signal) == RESET) { /* hit limit switch */
      ht->status = HS_HOMING_HIT_ENDSTOP1;
    }
    break; 
  case HS_HOMING_HIT_ENDSTOP1:
    ht->cur_dir = !ht->cur_dir;
    ss_set_speed(ht->devport, ht->devport->_homing_speed, ht->cur_dir);
    ht->status = HS_HOMING_REVERT_ENGAGE;
    ht->last_tick = get_ticks(); /* 记录起始时间：对于该模式，反向运动一小段时间 */
    break;
  case HS_HOMING_REVERT_ENGAGE:
    if ((gpio_read(ht->signal) == SET) && (get_ticks() - ht->last_tick) > REVERT_TIME)  { /* limit switch released */
      ht->status = HS_HOMING_ENGAGE_AGAIN;
      ht->cur_dir = !ht->cur_dir;
      ss_set_speed(ht->devport, ht->devport->_homing_speed / 5, ht->cur_dir);
    }
    break;
  case HS_HOMING_ENGAGE_AGAIN:
    if (gpio_read(ht->signal) == RESET) { /* hit limit switch again */
      ht->status = HS_HOMING_HIT_ENDSTOP2;
      ss_set_stop(ht->devport);
    }
    break;
  case HS_HOMING_HIT_ENDSTOP2:
    ht->devport->_homing_result = HR_SUCCESS;
    ss_set_ori(ht->devport);
    break;
  default:
    ASSERT(0);
  }
  return FALSE;
}

const char* StallResultToString(enum StallResult value) {
    return value == SD_UNKNOWN ? "SD_UNKNOWN" :
           value == SD_STOP     ? "SD_STOP"     :
           value == SD_MOVING   ? "SD_MOVING"   :
           value == SD_STALL    ? "SD_STALL"    :
                                   "INVALID_VALUE";
}

const char* HomingStatusToString(enum HomingStatus value) {
    return value == HS_HOMING_START          ? "HS_HOMING_START"          :
           value == HS_HOMING_ENGAGE         ? "HS_HOMING_ENGAGE"         :
           value == HS_HOMING_HIT_ENDSTOP1   ? "HS_HOMING_HIT_ENDSTOP1"   :
           value == HS_HOMING_REVERT_ENGAGE  ? "HS_HOMING_REVERT_ENGAGE"  :
           value == HS_HOMING_ENGAGE_AGAIN   ? "HS_HOMING_ENGAGE_AGAIN"   :
           value == HS_HOMING_HIT_ENDSTOP2   ? "HS_HOMING_HIT_ENDSTOP2"   :
                                               "INVALID_VALUE";
}

bool_t tick_homing_with_displace_sensor_only(HomingTracker * ht) {
  ASSERT(ht);
  ASSERT(ht->devport->_cur_tracker);
  //log_printf("homing tick...\n");
  
  uint32_t delay;
  int32_t cur_pos = ss_read_position(ht->devport, FALSE, &delay);
  uint32_t cur_tick = get_ticks();

  StallDetect *sd = &ht->devport->_cur_tracker->sd;
  enum StallResult sr = stall_detect_feed(sd, cur_pos, cur_tick, 0);
  log_printf("cur_pos:%ld, cur_tick:%ld, delay:%ld, stall detection:%s, status:%s\n",
	     cur_pos, cur_tick, delay,
	     StallResultToString(sr),
	     HomingStatusToString(ht->status));
  if (sr == SD_STALL){
    ht->devport->_homing_result = HR_NO_MOVE;
    ss_set_stop(ht->devport);
    return TRUE;
  }

  switch (ht->status) {
  case HS_HOMING_START:
    ss_set_speed(ht->devport, ht->devport->_homing_speed, ht->cur_dir);
    ht->status = HS_HOMING_ENGAGE;
    break;
  case HS_HOMING_ENGAGE:
    if (sr == SD_STOP){		/* hit end? */
      ht->status = HS_HOMING_HIT_ENDSTOP1;
      ht->last_pos = ss_read_position(ht->devport, TRUE, NULL); /* 记录起始位置：对于该模式，反向运动一小段距离 */
    }
    break;
  case HS_HOMING_HIT_ENDSTOP1:
    ht->cur_dir = !ht->cur_dir;
    ss_set_speed(ht->devport, ht->devport->_homing_speed, ht->cur_dir);
    ht->status = HS_HOMING_REVERT_ENGAGE;
    break;
  case HS_HOMING_REVERT_ENGAGE:
    if ((sr == SD_MOVING) && abs(cur_pos - ht->last_pos) > REVERT_DISTANCE){ /* limit switch released */
      ht->status = HS_HOMING_ENGAGE_AGAIN;
      ht->cur_dir = !ht->cur_dir;
      ss_set_speed(ht->devport, ht->devport->_homing_speed / 5, ht->cur_dir);
    }
    break;
  case HS_HOMING_ENGAGE_AGAIN:
    if (sr == SD_STOP) { /* hit limit switch again */
      ht->status = HS_HOMING_HIT_ENDSTOP2;
      ss_set_stop(ht->devport);
    }
    break;
  case HS_HOMING_HIT_ENDSTOP2:
    ht->devport->_homing_result = HR_SUCCESS;
    ss_set_ori(ht->devport);
    break;
  default:
    ASSERT(0);
  }
  
  return FALSE;
}

bool_t tick_homing(HomingTracker * ht){ /* return true => track finished */
  ASSERT(ht);
  if (ht->signal->port) {
    if (tick_homing_with_signal_sensor_available(ht)) return TRUE;
  } else {
    if (tick_homing_with_displace_sensor_only(ht)) return TRUE;
  }

  /* check if we are hitting endswitch */
  if ((ht->devport->limit_a.port) &&
      (gpio_read(&ht->devport->limit_a))) { /* hit limit a */
    ht->devport->_homing_result = HR_HIT_LIMIT_A;
    ss_set_stop(ht->devport);
    return TRUE;
  } else if ((ht->devport->limit_b.port) &&
	     (gpio_read(&ht->devport->limit_b))) { /* hit limit b */
    ht->devport->_homing_result = HR_HIT_LIMIT_B;
    ss_set_stop(ht->devport);
    return TRUE;
  } else
    /* check soft limit? */
    if (ht->soft_travel_limit) { 
      int pos = ss_read_position(ht->devport, FALSE, NULL);

      if (pos - ht->start_pos >
          ht->soft_travel_limit) { /* exceed soft limit? */
        bool_t dir = 0;
        dir = ht->devport->homing_direction == TOWARD_LIMIT_A ? 0 : 1;
        dir = ht->devport->invert_direction ? !dir : dir;
        dir = ht->devport->invert_limit_ab ? !dir : dir;

        ht->devport->_homing_result = dir ? HR_HIT_LIMIT_A : HR_HIT_LIMIT_B;
        ss_set_stop(ht->devport);
	return TRUE;
      }
    }
  
  return FALSE;
}
