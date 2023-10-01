#ifndef STALL_DETECT_H
#define STALL_DETECT_H
#include "gpio.h"
#include <stdint.h>

#define SD_STOP_THRESHOLD 500 /* 0.5 mm */
#define SD_STOP_TICK_THRESHOLD 1000 /* 1 s */
#define SD_STALL_TICK_THRESHOLD 5000 /* 5 s */

typedef struct {
  uint32_t last_tick;
  int32_t last_pos;
  bool_t inited;
  uint32_t _stop_threshold;	  /* 判定运动停止的距离阈值 */
  uint32_t _stop_tick_threshold;  /* 判定运动停止的时间阈值 */
  uint32_t _stall_tick_threshold; /* 判定运动卡死的时间阈值 */
} StallDetect;

enum StallResult{
  SD_UNKNOWN,
  SD_STOP,
  SD_MOVING,
  SD_STALL,
};

void init_stall_detection(StallDetect *sd, uint32_t stop_threshold,
			  uint32_t stop_tick_threshold, uint32_t stall_tick_threshold);
enum StallResult stall_detect_feed(StallDetect *sd, int32_t pos, uint32_t tick, uint32_t delay);

#endif
