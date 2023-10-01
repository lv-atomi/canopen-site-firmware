#ifndef HOMING_LOGIC_H
#define HOMING_LOGIC_H
#include "simple_servo.h"

bool_t tick_homing_with_signal_sensor_available(HomingTracker * ht);
bool_t tick_homing_with_displace_sensor_only(HomingTracker * ht);
bool_t tick_homing(HomingTracker * ht);

#endif
