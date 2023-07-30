#ifndef SLAVE_BOARD_BASE_H
#define SLAVE_BOARD_BASE_H

#include <stdint.h>
#include "gpio.h"
#include "adc.h"
#include "i2c.h"

uint16_t sense_position(ADCPort *senses, uint16_t num_senses);
void adc_config(ADCPort * senses, uint8_t num_senses);
void i2c_config(I2CPort * i2c);

#endif
