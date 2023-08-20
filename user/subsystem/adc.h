#ifndef ADC_H
#define ADC_H

#include "board.h"
#include <stdint.h>
#include "gpio.h"

typedef struct {
  IOPort port;
  adc_channel_select_type channel;
} ADCPort;


void init_adcs(ADCPort *ports, uint8_t num_ports);
uint16_t *read_adcs(ADCPort * ports, uint8_t num_ports);

#endif
