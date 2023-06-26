#ifndef SLAVESTATION_MODEL_3_H
#define SLAVESTATION_MODEL_3_H

#include "app_common.h"

void init_slavestation_model_3();
void GPOutSet(uint16_t slave_addr, uint8_t channel, bool_t val);
bool_t GPOutRead(uint16_t slave_addr, uint8_t channel);
void motor_speed_set(uint16_t slave_addr, int32_t speed);

#endif
