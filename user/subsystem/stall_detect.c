#include "stall_detect.h"
#include "log.h"
#include "timer.h"
#include <stdlib.h>

void init_stall_detection(StallDetect *sd, uint32_t stop_threshold,
			  uint32_t stop_tick_threshold, uint32_t stall_tick_threshold) {
  ASSERT(sd);
  sd->inited = FALSE;
  sd->_stop_threshold = stop_threshold;
  sd->_stop_tick_threshold = stop_tick_threshold;
  sd->_stall_tick_threshold = stall_tick_threshold;
}

enum StallResult stall_detect_feed(StallDetect *sd, int32_t pos, uint32_t tick,
                                   uint32_t delay) {
  enum StallResult sr = SD_UNKNOWN;
  if (!sd->inited){
    sd->inited = TRUE;
    sd->last_tick = tick-delay;
    sd->last_pos = pos;

    return sr;
  }

  log_printf("last_tick:%ld last_pos:%ld  cur_tick:%ld, cur_pos:%ld\n",
	     sd->last_tick, sd->last_pos,
	     tick-delay, pos
	     );

  if (abs(pos - sd->last_pos) > sd->_stop_threshold) {
    sd->last_tick = tick-delay;
    sd->last_pos = pos;
    sr = SD_MOVING;
  } else if (tick - delay - sd->last_tick > sd->_stop_tick_threshold) {
    sr = SD_STOP;
  }
  else if (tick - delay - sd->last_tick > sd->_stall_tick_threshold) {
    sr = SD_STALL;
  }

  return sr;
}
