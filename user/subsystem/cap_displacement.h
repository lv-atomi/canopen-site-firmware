#ifndef CAPACITOR_DISPLACEMENT_MEASURE_H
#define CAPACITOR_DISPLACEMENT_MEASURE_H

#include "gpio.h"
#include "spi.h"
#include "board.h"
#include <stdint.h>

#define DISPLACEMENT_MIN_INTERVAL   250 /* minimum read interval, in ms */

typedef struct{
  SPIPort spi;
  int32_t zero_value;
  int32_t last_value;
  uint32_t last_ticks;
  uint8_t recv_idx;
} CapacitorDisplacementMeasurePort;

void init_capacitor_displacement_measurement(CapacitorDisplacementMeasurePort * devport);
void displacement_set_zero(CapacitorDisplacementMeasurePort * devport);
int32_t read_displacement(CapacitorDisplacementMeasurePort * devport, uint32_t * delay, bool_t get_raw);
void cap_irq_handler(int spi_index);

#endif
