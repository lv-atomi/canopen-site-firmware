#ifndef GPIO_H
#define GPIO_H

#include "board.h"
#include <stdint.h>

typedef uint_fast8_t bool_t;

typedef struct {
  gpio_type *port;
  gpio_pins_source_type pin_source;
#if defined (__AT32F421_GPIO_H)
  gpio_mux_sel_type mux_sel;
#endif
#if defined (AT32F403Axx)
  uint32_t gpio_remap;
#endif
} IOPort;

void init_gpio_output(IOPort * devport,
		      gpio_output_type output_type,
		      gpio_drive_type drive_strength);
void init_gpio_mux(IOPort * devport,
		   gpio_output_type output_type,
		   gpio_pull_type pull_type,
		   gpio_drive_type drive_strength);
void init_gpio_input(IOPort * devport,
		     gpio_pull_type pull_type,
		     gpio_drive_type drive_strength);
void init_gpio_analogy(IOPort *devport, gpio_pull_type pull_type,
                       gpio_drive_type drive_strength);

void gpio_set_input_mode(IOPort *devport, bool_t is_input);

void gpio_set(IOPort * devport, bool_t bit);
bool_t gpio_read(IOPort *devport);
uint8_t waiting_for_timeout(IOPort * devport, bool_t current_bit, uint32_t timeout_us);


#endif
