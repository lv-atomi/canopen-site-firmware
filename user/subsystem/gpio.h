#ifndef GPIO_H
#define GPIO_H

#include "at32f403a_407_board.h"
#include <stdint.h>

typedef uint_fast8_t bool_t;

typedef struct {
  gpio_type *port;
  uint16_t pin;
} IOPort;

void init_gpio_output(IOPort * devport,
		      gpio_output_type output_type,
		      gpio_mode_type gpio_mode,
		      gpio_drive_type drive_strength);
void init_gpio_input(IOPort * devport,
		     gpio_pull_type pull_type,
		     gpio_drive_type drive_strength);
void init_gpio_analogy(IOPort *devport, gpio_pull_type pull_type,
                       gpio_drive_type drive_strength);

void gpio_set(IOPort * devport, bool_t bit);
bool_t gpio_read(IOPort *devport);


#endif
